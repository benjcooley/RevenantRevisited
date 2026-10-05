// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Revenant Revisited (port) - 2026                     *
// *           savegame.cpp - save slots and the save file format          *
// *************************************************************************
//
// Retail format and sequence: docs/gameflow/forensics/SAVE_GAME.md.

#include "savegame.h"

#include "area.h"
#include "bitmap.h"
#include "display.h"
#include "hudstate.h"
#include "logging.h"
#include "mapmanager.h"
#include "module.h"
#include "object.h"
#include "player.h"
#include "playscreen.h"
#include "revenant.h"
#include "revutils.h"
#include "script.h"
#include "stream.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <memory>
#include <system_error>

namespace fs = std::filesystem;

namespace {

constexpr const char* kDefaultSlotName = "Default Save";   // retail 0x005d9d40
constexpr const char* kNewGameFile     = "newgame.sav";
constexpr const char* kSaveFile        = "game.sav";
constexpr const char* kThumbnailFile   = "ss.bmp";
constexpr const char* kSlotCurMapDir   = "CurMap";
constexpr const char* kSinglePlayerDir = "Single";

constexpr int32_t kPlayerListFormat     = 2;      // header +0x18, as retail writes it
constexpr int32_t kMultiplayerBlockSize = 0x200;
constexpr int32_t kLegacyPlayerPadding  = 0x50;   // format 0, version > 12
constexpr int32_t kGameStatesVersion    = 10;     // game states present from version 10
constexpr int32_t kSoldUniquesVersion   = 11;     // merchant table present from version 11

// Player state bits LoadGame adjusts (TPlayer +0x36c; SetPlayerState 0x0051d680).
constexpr int32_t kPlayerStateActive = 1 << 0;   // set on every loaded player
constexpr int32_t kPlayerStateStop   = 1 << 1;   // stops the action in progress; cleared on load

void SkipBytes(TInputStream& is, int32_t count)
{
    uint8_t unused = 0;
    for (int32_t i = 0; i < count; i++)
        is >> unused;
}

void WriteZeros(TOutputStream& os, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
        os << (uint8_t)0;
}

// <SavePath>/<SaveGamePath>Single
fs::path SinglePlayerSlotsDir()
{
    char relative[MAXPATHLEN];
    std::snprintf(relative, sizeof(relative), "%s%s", SaveGamePath, kSinglePlayerDir);
    char resolved[MAXPATHLEN];
    return fs::path(makepath(relative, resolved, sizeof(resolved)));
}

bool ReadHeader(const fs::path& file, SSaveHeader& header)
{
    uint8_t bytes[SSaveHeader::kSize];
    FILE* fp = std::fopen(file.string().c_str(), "rb");
    if (!fp)
        return false;
    const size_t got = std::fread(bytes, 1, sizeof(bytes), fp);
    std::fclose(fp);
    TInputStream is(bytes, (int32_t)got);
    return header.Read(is);
}

const char* ActiveModuleName()
{
    const TModule* module = ModuleManager.Active();
    return module ? module->dirname.c_str() : "";
}

// The module name a save records. Retail lowercases module directory names
// when it scans the modules (0x00460620), so its saves name "ahkuilon"; the
// port keeps the directory's own case for file lookups.
std::string SaveModuleName()
{
    std::string name = ActiveModuleName();
    for (char& c : name)
        c = (char)std::tolower((unsigned char)c);
    return name;
}

}  // namespace

// ***************
// * SSaveHeader *
// ***************

bool SSaveHeader::Read(TInputStream& is)
{
    if (is.Remaining() < kSize)
        return false;

    is >> gametime;
    SkipBytes(is, 0x10);
    is >> multiplayer >> playerformat >> version;
    for (char& c : module)
        is >> c;
    module[kModuleNameLen - 1] = '\0';
    SkipBytes(is, 0x40);
    return true;
}

void SSaveHeader::Write(TOutputStream& os) const
{
    os.MakeFreeSpace(kSize);
    os << gametime;
    WriteZeros(os, 0x10);
    os << multiplayer << playerformat << version;
    for (char c : module)
        os << c;
    WriteZeros(os, 0x40);
}

// *************
// * TSaveGame *
// *************

bool TSaveGame::Load(const char* name)
{
    if (!name)
        name = kDefaultSlotName;

    const int32_t index = FindSlot(name);
    if (index < 0)
    {
        log_warn("[savegame] no save slot named '%s'", name);
        return false;
    }

    const SSaveSlot& slot = slots[index];
    const fs::path file = slot.dir / kSaveFile;
    return LoadFile(file.string().c_str(), slot.dir / kSlotCurMapDir);
}

bool TSaveGame::LoadNewGame()
{
    // Retail: <modules path><dirname>\newgame.sav. The resource layer finds it
    // in the mounted module, packed or loose.
    char file[MAXPATHLEN];
    std::snprintf(file, sizeof(file), "Modules\\%s\\%s", ActiveModuleName(), kNewGameFile);
    return LoadFile(file, {});
}

// REVSYNC: LoadGame @ 0x0048df70 (flags & 2 clear). REVSYNC-DIVERGENCE: the
// file is read and its header checked before the world is reset; retail reset
// first, so a missing or damaged save left an empty world.
bool TSaveGame::LoadFile(const char* file, const fs::path& slotCurMap)
{
    std::vector<uint8_t> data;
    if (!rev_read_file(file, data))
    {
        log_warn("[savegame] can't read %s", file);
        return false;
    }

    TInputStream is(data.data(), (int32_t)data.size());
    SSaveHeader header;
    if (!header.Read(is))
    {
        log_error("[savegame] %s is not a save file (%zu bytes)", file, data.size());
        return false;
    }
    if (header.multiplayer && header.playerformat >= 1)
    {
        // Multiplayer data; the port plays single player only.
        if (is.Remaining() < kMultiplayerBlockSize)
        {
            log_error("[savegame] %s: truncated multiplayer block", file);
            return false;
        }
        SkipBytes(is, kMultiplayerBlockSize);
    }

    loading = true;
    ResetWorld(slotCurMap);

    bool ok = true;
    if (header.version >= kGameStatesVersion && !ScriptManager.GameStates().LoadStream(is))
    {
        log_error("[savegame] %s: truncated game states", file);
        ok = false;
    }
    ok = ok && ReadSoldUniques(is, header);
    ok = ok && ReadPlayers(is, header);
    if (ok && !Player)
    {
        log_error("[savegame] %s holds no player", file);
        ok = false;
    }

    // A legacy single-player save doesn't name its module; it belongs to the
    // active one.
    const char* module = header.playerformat >= 1 ? header.module : ActiveModuleName();
    if (!ModuleManager.SetCurModule(module))
        ok = false;

    PlayScreen.SetGameTime(header.gametime);
    PlayScreen.SetControlOn(true);
    loading = false;

    if (Player)
    {
        const S3DPoint pos = Player->Pos();
        log_info("[savegame] loaded %s: module=%s time=%d version=%d states=%d "
                 "players=%d; main player '%s' L%d HP %d/%d items=%d on level %d at (%d,%d,%d)",
                 file, module, header.gametime, header.version,
                 ScriptManager.GameStates().NumStates(), PlayerManager.NumPlayers(),
                 Player->GetName() ? Player->GetName() : "?", (int)Player->Level(),
                 (int)Player->Health(), (int)Player->MaxHealth(), Player->RealNumInventoryItems(),
                 Player->GetLevel(), pos.x, pos.y, pos.z);
    }
    return ok;
}

// The reset block of LoadGame @ 0x0048df70, in retail order
// (SAVE_GAME.md §4). Not yet ported, and joining this sequence with their
// systems: finishing a PlayScreen fade (0x0047ece0), ending a conversation
// (dialog pane 0x005360f0) and emptying the buy/sell pane (0x00532f40).
void TSaveGame::ResetWorld(const fs::path& slotCurMap)
{
    if (slotCurMap.empty())
        MapManager.ClearCurMap();               // 0x0044e460
    else
        MapManager.LoadCurMap(slotCurMap);      // 0x0044e460 + 0x0044e050
    ScriptManager.ResetScripts();               // 0x00496e20
    ScriptManager.ReloadStates();               // 0x004975c0
    AreaManager.ExitAll();                      // 0x0041c600
    PlayerManager.Clear();                      // 0x0051eda0
    PlayScreen.SetControlOn(true);              // 0x0047c580(1)
}

bool TSaveGame::ReadSoldUniques(TInputStream& is, const SSaveHeader& header)
{
    soldUniques.clear();
    if (header.version < kSoldUniquesVersion)
        return true;

    if (is.Remaining() < 4)
        return false;
    int32_t count = 0;
    is >> count;

    // Retail reads the pairs only from counted-player-list saves; older ones
    // store one int per entry, which it skips.
    const int32_t entrySize = header.playerformat > 1 ? 8 : 4;
    if (count < 0 || is.Remaining() < (int64_t)count * entrySize)
        return false;
    if (entrySize == 4)
    {
        SkipBytes(is, count * entrySize);
        return true;
    }

    soldUniques.resize(count);
    for (SSoldUnique& sold : soldUniques)
        is >> sold.objclass >> sold.objtype;
    return true;
}

// REVSYNC: the player loop of LoadGame @ 0x0048df70. Players are loaded
// without the map flag (LoadGame clears retail's map-load global), so they
// aren't discarded as NONMAP objects; see SAVE_GAME.md §4.
bool TSaveGame::ReadPlayers(TInputStream& is, const SSaveHeader& header)
{
    int32_t count = 1;
    if (header.playerformat < 1)
    {
        if (header.version > 12)
        {
            if (is.Remaining() < kLegacyPlayerPadding)
                return false;
            SkipBytes(is, kLegacyPlayerPadding);
        }
    }
    else
    {
        if (is.Remaining() < 4)
            return false;
        is >> count;
    }

    for (int32_t i = 0; i < count && is.Remaining() > 0; i++)
    {
        TObjectInstance* object = TObjectInstance::LoadObject(is, header.version);
        if (!object)
            continue;
        if (object->ObjClass() != OBJCLASS_PLAYER)
        {
            log_warn("[savegame] player entry %d is class %d, not a player; dropped",
                     i, object->ObjClass());
            delete object;
            continue;
        }

        TPlayer* player = static_cast<TPlayer*>(object);
        PlayerManager.AddPlayer(player);
        player->SetPlayerState((player->PlayerState() & ~kPlayerStateStop) | kPlayerStateActive);
        if (!header.multiplayer)
        {
            PlayerManager.SetMainPlayer(player);
            RestoreHud(player->HudWords());
        }
    }
    if (is.Overrun())
    {
        log_error("[savegame] the player data is truncated");
        return false;
    }
    return true;
}

// REVSYNC-DIVERGENCE: retail stored the HUD words it saved (§11.4) but never
// applied them, so a load left the HUD as it was. The port restores the
// sidebar from them.
void TSaveGame::RestoreHud(const SPlayerHudWords& words)
{
    SHudState& hud = GetHudState();
    hud.sidebarState = words.sidebarOpen ? HUD_SIDEBAR_OPEN : HUD_SIDEBAR_CLOSED;
    if (words.upperMode >= HUD_TOP_EQUIP && words.upperMode <= HUD_TOP_BOOK)
        hud.topSlot = words.upperMode;
    if (words.lowerMode >= HUD_BOT_INV && words.lowerMode <= HUD_BOT_SPELL)
        hud.bottomSlot = words.lowerMode;
}

// What retail's SaveGame wrote from its HUD globals (TPlayer::Save
// @ 0x0051bdc0); the word the port can't identify keeps the loaded value.
SPlayerHudWords TSaveGame::CurrentHudWords(const TPlayer& player)
{
    const SHudState& hud = GetHudState();
    SPlayerHudWords words = player.HudWords();
    words.sidebarOpen = hud.sidebarState == HUD_SIDEBAR_OPEN ? 1 : 0;
    words.upperMode   = hud.topSlot;
    words.lowerMode   = hud.bottomSlot;
    return words;
}

// REVSYNC: SaveGame @ 0x0048d720. Not ported: the editor path that writes
// the module's own newgame.sav (the port never writes into the install).
bool TSaveGame::Save(const char* name)
{
    if (!Player)
    {
        log_warn("[savegame] no player to save");
        return false;
    }
    if (!name)
        name = kDefaultSlotName;

    const fs::path slot = SinglePlayerSlotsDir() / name;
    std::error_code ec;
    fs::create_directories(slot, ec);
    if (ec)
    {
        log_error("[savegame] can't create %s: %s", slot.string().c_str(), ec.message().c_str());
        return false;
    }
    fs::remove(slot / kThumbnailFile, ec);

    MapManager.SaveCurMap(slot / kSlotCurMapDir);
    StoreThumbnail(slot);

    SSaveHeader header;
    header.gametime     = PlayScreen.GameTime();
    header.playerformat = kPlayerListFormat;
    header.version      = MAP_VERSION;
    strncpyz(header.module, SaveModuleName().c_str(), SSaveHeader::kModuleNameLen);

    TOutputStream os(0x8000, 0x4000);
    header.Write(os);
    WriteBody(os);

    // Write beside the old file and swap, so a failed write never destroys
    // the slot's previous save.
    const fs::path file = slot / kSaveFile;
    const fs::path partial = slot / (std::string(kSaveFile) + ".partial");
    FILE* fp = std::fopen(partial.string().c_str(), "wb");
    bool ok = fp && std::fwrite(os.Buffer(), 1, os.DataSize(), fp) == (size_t)os.DataSize();
    if (fp)
        ok = (std::fclose(fp) == 0) && ok;
    if (ok)
    {
        fs::rename(partial, file, ec);
        ok = !ec;
    }
    if (!ok)
    {
        fs::remove(partial, ec);
        log_error("[savegame] writing %s failed", file.string().c_str());
        return false;
    }

    log_info("[savegame] saved '%s' (%d bytes, time=%d)", name, os.DataSize(), header.gametime);
    return true;
}

// <SavePath>/ss.bmp: retail's ".\ss.bmp" in the install directory; the port
// writes only under SavePath.
fs::path TSaveGame::ThumbnailFile()
{
    return fs::path(SavePath) / kThumbnailFile;
}

// REVSYNC: the thumbnail TPlayScreen writes when the player opens the
// in-game menu or the save dialog, or quick-saves (0x0047cb52, 0x0047dc05,
// 0x0047dd08; SAVE_GAME.md §11.7): the screen in a 640x480 16-bit bitmap,
// written by SaveBMP at scale 3. The port captures the next frame the display
// presents. `slot`, when given, also gets a copy.
void TSaveGame::CaptureThumbnail(const fs::path& slot)
{
    const bool requested = Display.RequestCapture(
        [this, slot](const uint8_t* rgba, int32_t width, int32_t height) {
            if (!WriteThumbnail(rgba, width, height))
                return;
            ++thumbnailVersion;
            if (!slot.empty())
                CopyThumbnailTo(slot);
        });
    if (!requested)
        log_info("[savegame] no display to capture a thumbnail from");
}

// Retail copied the current thumbnail into the slot (SAVE_GAME.md §5 step 6).
// REVSYNC-DIVERGENCE: retail copied whatever .\ss.bmp was there, however
// old; when nothing has been captured since the last save (a save from the
// console), the port captures the next frame for this slot instead.
void TSaveGame::StoreThumbnail(const fs::path& slot)
{
    if (thumbnailVersion != storedThumbnailVersion)
        CopyThumbnailTo(slot);
    else
        CaptureThumbnail(slot);
}

void TSaveGame::CopyThumbnailTo(const fs::path& slot)
{
    std::error_code ec;
    fs::copy_file(ThumbnailFile(), slot / kThumbnailFile, fs::copy_options::overwrite_existing, ec);
    if (ec)
        log_warn("[savegame] can't copy the thumbnail into %s: %s", slot.string().c_str(),
                 ec.message().c_str());
    storedThumbnailVersion = thumbnailVersion;
}

// The 640x480 16-bit screen retail captured, from the display's RGBA frame.
// REVSYNC-DIVERGENCE: a display that isn't 4:3 is cropped to its centred
// 4:3 area first, so the thumbnail isn't squashed.
bool TSaveGame::WriteThumbnail(const uint8_t* rgba, int32_t width, int32_t height)
{
    constexpr int32_t kScreenWidth  = 640;
    constexpr int32_t kScreenHeight = 480;
    constexpr int32_t kShrink       = 3;

    int32_t cropw = width, croph = height;
    if ((int64_t)width * kScreenHeight > (int64_t)height * kScreenWidth)
        cropw = height * kScreenWidth / kScreenHeight;
    else
        croph = width * kScreenHeight / kScreenWidth;
    const int32_t cropx = (width - cropw) / 2;
    const int32_t cropy = (height - croph) / 2;
    if (cropw < 1 || croph < 1)
        return false;

    std::unique_ptr<TBitmap> screen(TBitmap::NewBitmap(kScreenWidth, kScreenHeight, BM_16BIT));
    if (!screen)
        return false;
    uint16_t* dst = (uint16_t*)screen->data16;
    for (int32_t y = 0; y < kScreenHeight; y++)
    {
        const uint8_t* row = rgba + ((size_t)(cropy + y * croph / kScreenHeight) * width) * 4;
        for (int32_t x = 0; x < kScreenWidth; x++)
        {
            const uint8_t* p = row + (size_t)(cropx + x * cropw / kScreenWidth) * 4;
            *dst++ = (uint16_t)(((p[0] & 0xf8) << 8) | ((p[1] & 0xfc) << 3) | (p[2] >> 3));
        }
    }

    const std::string file = ThumbnailFile().string();
    if (!screen->SaveBMP(file.c_str(), kShrink))
    {
        log_warn("[savegame] can't write the thumbnail %s", file.c_str());
        return false;
    }
    return true;
}

// The body after the header (SAVE_GAME.md §3.2). REVSYNC: the player loop of
// SaveGame @ 0x0048d720 clears OF_VIRGIN on each player and on each item
// directly in its inventory (a loaded player keeps its own stats), and the
// player record carries the HUD state.
void TSaveGame::WriteBody(TOutputStream& os) const
{
    ScriptManager.GameStates().SaveStream(os);

    os << (int32_t)soldUniques.size();
    for (const SSoldUnique& sold : soldUniques)
        os << sold.objclass << sold.objtype;

    const int32_t count = PlayerManager.NumPlayers();
    os << count;
    for (int32_t i = 0; i < count; i++)
    {
        TPlayer* player = PlayerManager.GetPlayer(i);
        player->ResetFlags(player->Flags() & ~OF_VIRGIN);
        for (TInventoryIterator item(player); item; item++)
            item->ResetFlags(item->Flags() & ~OF_VIRGIN);
        player->SetHudWords(CurrentHudWords(*player));
        TObjectInstance::SaveObject(player, os);
    }
}

void TSaveGame::RefreshSlots()
{
    slots.clear();

    const fs::path root = SinglePlayerSlotsDir();
    std::error_code ec;
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec))
    {
        std::error_code entryError;
        const fs::path file = it->path() / kSaveFile;
        if (!it->is_directory(entryError) || !fs::is_regular_file(file, entryError))
            continue;

        SSaveSlot slot;
        slot.name = it->path().filename().string();
        slot.dir  = it->path();
        SSaveHeader header;
        slot.module = (ReadHeader(file, header) && header.module[0]) ? header.module
                                                                     : ActiveModuleName();
        slots.push_back(std::move(slot));
    }

    // Retail listed slots in directory order, which on its file system was
    // alphabetical.
    std::sort(slots.begin(), slots.end(), [](const SSaveSlot& a, const SSaveSlot& b) {
        return stricmp(a.name.c_str(), b.name.c_str()) < 0;
    });
}

int32_t TSaveGame::FindSlot(const char* name) const
{
    if (!name)
        return -1;
    for (int32_t i = 0; i < (int32_t)slots.size(); i++)
        if (stricmp(slots[i].name.c_str(), name) == 0)
            return i;
    return -1;
}

bool TSaveGame::HasSoldUnique(int32_t objclass, int32_t objtype) const
{
    return std::any_of(soldUniques.begin(), soldUniques.end(), [&](const SSoldUnique& sold) {
        return sold.objclass == objclass && sold.objtype == objtype;
    });
}

void TSaveGame::AddSoldUnique(int32_t objclass, int32_t objtype)
{
    if (!HasSoldUnique(objclass, objtype))
        soldUniques.push_back({ objclass, objtype });
}
