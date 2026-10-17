// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Revenant Revisited (port) - 2026                     *
// *           savegame.cpp - save slots and the save file format          *
// *************************************************************************
//
// Retail format and sequence: docs/gameflow/forensics/SAVE_GAME.md.

#include "savegame.h"

#include "dialog.h"

#include "area.h"
#include "bitmap.h"
#include "buysell.h"
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
#include "savefile.h"
#include "script.h"
#include "sectorstore.h"
#include "stream.h"

#include "stb_image.h"
#include "stb_image_write.h"

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


bool ReadFileBytes(const fs::path& file, std::vector<uint8_t>& bytes)
{
    bytes.clear();
    FILE* fp = std::fopen(file.string().c_str(), "rb");
    if (!fp)
        return false;
    uint8_t chunk[16384];
    size_t got = 0;
    while ((got = std::fread(chunk, 1, sizeof(chunk), fp)) > 0)
        bytes.insert(bytes.end(), chunk, chunk + got);
    std::fclose(fp);
    return true;
}

bool WriteFileBytes(const fs::path& file, const std::vector<uint8_t>& bytes)
{
    FILE* fp = std::fopen(file.string().c_str(), "wb");
    const bool ok = fp && std::fwrite(bytes.data(), 1, bytes.size(), fp) == bytes.size();
    return fp && (std::fclose(fp) == 0) && ok;
}

// The header of a save's game.sav bytes.
bool ReadHeader(const std::vector<uint8_t>& game, SSaveHeader& header)
{
    // TInputStream takes a mutable buffer but only reads it.
    TInputStream is(const_cast<uint8_t*>(game.data()), (int32_t)std::min<size_t>(game.size(), SSaveHeader::kSize));
    return header.Read(is);
}

// The 24-bit path of TBitmap::LoadBMP @ 0x004a2ce0: "BM", a
// BITMAPINFOHEADER of the bitmap's own width and height, then the rows
// bottom up, B G R per pixel.
bool DecodeRetailThumbnail(const std::vector<uint8_t>& file, std::vector<uint8_t>& rgba)
{
    constexpr size_t kHeadersSize = 14 + 40;
    auto u16 = [&](size_t at) { return (uint32_t)file[at] | ((uint32_t)file[at + 1] << 8); };
    auto u32 = [&](size_t at) { return u16(at) | (u16(at + 2) << 16); };
    if (file.size() < kHeadersSize || u16(0) != 0x4D42)
        return false;
    const uint32_t offset = u32(10);
    const int32_t  width  = (int32_t)u32(18);
    const int32_t  height = (int32_t)u32(22);
    const uint32_t bits   = u16(28);
    const size_t   stride = (size_t)(((width * 3) + 3) & ~3);
    if (width != TSaveGame::kThumbnailWidth || height != TSaveGame::kThumbnailHeight || bits != 24 ||
        file.size() < offset + stride * (size_t)height)
        return false;

    rgba.assign((size_t)width * height * 4, 0);
    for (int32_t y = 0; y < height; y++)
    {
        const uint8_t* src = file.data() + offset + stride * (size_t)(height - 1 - y);
        uint8_t* dst = rgba.data() + (size_t)y * width * 4;
        for (int32_t x = 0; x < width; x++, src += 3, dst += 4)
        {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = 255;
        }
    }
    return true;
}

// A thumbnail as TBitmap::SaveBMP (0x004a2960) writes one, header quirks
// and all, so retail's LoadBMP takes it.
std::vector<uint8_t> EncodeRetailThumbnail(const std::vector<uint8_t>& rgba)
{
    const int32_t width = TSaveGame::kThumbnailWidth, height = TSaveGame::kThumbnailHeight;
    std::vector<uint8_t> file;
    auto u16 = [&](uint32_t v) { file.push_back((uint8_t)v); file.push_back((uint8_t)(v >> 8)); };
    auto u32 = [&](uint32_t v) { u16(v & 0xffff); u16(v >> 16); };
    u16(0x4D42);
    u32((uint32_t)(height * width + 18) * 3);   // SaveBMP's file size
    u16(0); u16(0);
    u32(14 + 40);
    u32(40);
    u32((uint32_t)width); u32((uint32_t)height);
    u16(1); u16(24);
    u32(0); u32(0); u32(0); u32(0); u32(0); u32(0);
    for (int32_t y = height - 1; y >= 0; y--)
        for (int32_t x = 0; x < width; x++)
        {
            const uint8_t* p = rgba.data() + ((size_t)y * width + x) * 4;
            file.push_back(p[2]);
            file.push_back(p[1]);
            file.push_back(p[0]);
        }
    return file;
}

// An opaque picture as an RGB PNG file.
std::vector<uint8_t> EncodePng(const std::vector<uint8_t>& rgba, int32_t width, int32_t height)
{
    std::vector<uint8_t> rgb;
    rgb.reserve((size_t)width * height * 3);
    for (size_t i = 0; i + 3 < rgba.size(); i += 4)
        rgb.insert(rgb.end(), rgba.begin() + (ptrdiff_t)i, rgba.begin() + (ptrdiff_t)i + 3);
    std::vector<uint8_t> png;
    stbi_write_png_to_func(
        [](void* context, void* data, int size) {
            auto* out = static_cast<std::vector<uint8_t>*>(context);
            const auto* bytes = static_cast<const uint8_t*>(data);
            out->insert(out->end(), bytes, bytes + size);
        },
        &png, width, height, 3, rgb.data(), width * 3);
    return png;
}

bool DecodePng(const std::vector<uint8_t>& png, std::vector<uint8_t>& rgba, int32_t& width, int32_t& height)
{
    int w = 0, h = 0, comp = 0;
    stbi_uc* pixels = stbi_load_from_memory(png.data(), (int)png.size(), &w, &h, &comp, 4);
    if (!pixels)
        return false;
    rgba.assign(pixels, pixels + (size_t)w * h * 4);
    stbi_image_free(pixels);
    width = w;
    height = h;
    return true;
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
    if (slot.isFile)
    {
        SaveFile::SSaveFile save;
        std::string error;
        if (!SaveFile::Read(slot.path, save, error))
        {
            log_warn("[savegame] %s", error.c_str());
            return false;
        }
        return LoadGame(save.game, slot.path.string(), [&save] { MapManager.LoadCurMap(save.sectors); });
    }

    const fs::path file = slot.path / kSaveFile;
    std::vector<uint8_t> data;
    if (!ReadFileBytes(file, data))
    {
        log_warn("[savegame] can't read %s", file.string().c_str());
        return false;
    }
    return LoadGame(data, file.string(), [dir = slot.path / kSlotCurMapDir] { MapManager.LoadCurMap(dir); });
}

bool TSaveGame::LoadNewGame()
{
    // Retail: <modules path><dirname>\newgame.sav. The resource layer finds it
    // in the mounted module, packed or loose.
    char file[MAXPATHLEN];
    std::snprintf(file, sizeof(file), "Modules\\%s\\%s", ActiveModuleName(), kNewGameFile);
    std::vector<uint8_t> data;
    if (!rev_read_file(file, data))
    {
        log_warn("[savegame] can't read %s", file);
        return false;
    }
    return LoadGame(data, file, {});
}

// REVSYNC: LoadGame @ 0x0048df70 (flags & 2 clear). REVSYNC-DIVERGENCE: the
// file is read and its header checked before the world is reset; retail reset
// first, so a missing or damaged save left an empty world.
bool TSaveGame::LoadGame(const std::vector<uint8_t>& data, const std::string& source,
                         const std::function<void()>& restoreSectors)
{
    const char* file = source.c_str();
    TInputStream is(const_cast<uint8_t*>(data.data()), (int32_t)data.size());   // read only
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
    ResetWorld(restoreSectors);

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
// (SAVE_GAME.md §4).
void TSaveGame::ResetWorld(const std::function<void()>& restoreSectors)
{
    if (restoreSectors)
        restoreSectors();                       // 0x0044e460 + 0x0044e050
    else
        MapManager.ClearCurMap();               // 0x0044e460
    PlayScreen.CloseDrawer();                   // 0x0047ece0: the shop, or the bottom bar
    DialogPane.ResetForLoad();                  // 0x005360f0
    BuySellPane.Clear();                        // 0x00532f40
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
            PlayerManager.SetMainPlayer(player);
    }
    if (is.Overrun())
    {
        log_error("[savegame] the player data is truncated");
        return false;
    }
    return true;
}

// What retail's SaveGame wrote from its HUD globals (TPlayer::Save
// @ 0x0051bdc0). Only written: a load keeps them in the player record and
// nothing reads them (SAVE_GAME.md §11.4), so the HUD is never restored
// from a save (TPlayScreen::Initialize).
SPlayerHudWords TSaveGame::CurrentHudWords(const TPlayer& player)
{
    const SHudState& hud = GetHudState();
    SPlayerHudWords words = player.HudWords();
    words.sidebarOpen = hud.sidebarState == HUD_SIDEBAR_OPEN ? 1 : 0;
    words.upperMode   = hud.topSlot;
    words.lowerMode   = hud.bottomSlot;
    words.drawerOpen  = PlayScreen.DrawerHeight() > 0 ? 1 : 0;
    return words;
}

// REVSYNC: SaveGame @ 0x0048d720. Not ported: the editor path that writes
// the module's own newgame.sav (the port never writes into the install).
// The port writes a save file (SAVE_FORMAT.md); with OldSaves, retail's slot
// folder.
bool TSaveGame::Save(const char* name)
{
    if (!Player)
    {
        log_warn("[savegame] no player to save");
        return false;
    }
    if (!name)
        name = kDefaultSlotName;
    const bool ok = OldSaves ? SaveFolder(name) : SaveFileSlot(name);
    if (ok)
        lastSaved = name;
    return ok;
}

// The game.sav bytes: the header, then the body.
std::vector<uint8_t> TSaveGame::GameBytes() const
{
    SSaveHeader header;
    header.gametime     = PlayScreen.GameTime();
    header.playerformat = kPlayerListFormat;
    header.version      = MAP_VERSION;
    strncpyz(header.module, SaveModuleName().c_str(), SSaveHeader::kModuleNameLen);

    TOutputStream os(0x8000, 0x4000);
    header.Write(os);
    WriteBody(os);
    const auto* bytes = static_cast<const uint8_t*>(os.Buffer());
    return std::vector<uint8_t>(bytes, bytes + os.DataSize());
}

bool TSaveGame::SaveFileSlot(const char* name)
{
    const fs::path file = SaveFilePath(name);
    SaveFile::SSaveFile save;
    save.game    = GameBytes();
    save.sectors = MapManager.DiffCurMap();

    // The thumbnail captured since the last save; with none, the next frame,
    // put into the file once it is read back.
    const bool fresh = thumbnailVersion != storedThumbnailVersion;
    if (fresh)
    {
        save.thumbnail = ThumbnailPng();
        storedThumbnailVersion = thumbnailVersion;
    }

    std::string error;
    if (!SaveFile::Write(file, save, error))
    {
        log_error("[savegame] %s", error.c_str());
        return false;
    }
    if (!fresh)
        CaptureThumbnail(file);
    size_t changes = 0, whole = 0;
    for (const SaveFile::SSectorDelta& delta : save.sectors)
        delta.whole ? ++whole : changes += delta.changes.size();
    log_info("[savegame] saved '%s' to %s: game %zu bytes, %zu sectors (%zu object changes, %zu whole), "
             "thumbnail %zu bytes", name, file.string().c_str(), save.game.size(), save.sectors.size(), changes,
             whole, save.thumbnail.size());
    return true;
}

// Retail's slot folder: game.sav, ss.bmp and CurMap/ with the working set's
// sectors whole.
bool TSaveGame::SaveFolder(const char* name)
{
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
    const std::vector<uint8_t> game = GameBytes();

    // Write beside the old file and swap, so a failed write never destroys
    // the slot's previous save.
    const fs::path file = slot / kSaveFile;
    const fs::path partial = slot / (std::string(kSaveFile) + ".partial");
    bool ok = WriteFileBytes(partial, game);
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
    log_info("[savegame] saved '%s' as a retail slot folder (%zu bytes)", name, game.size());
    return true;
}

bool TSaveGame::ExportRetail(const char* name, const fs::path& dir)
{
    RefreshSlots();
    const int32_t index = FindSlot(name);
    if (index < 0)
    {
        log_warn("[savegame] no save slot named '%s' to export", name ? name : "");
        return false;
    }
    const SSaveSlot& slot = slots[index];
    std::error_code ec;
    if (!slot.isFile)
    {
        if (fs::equivalent(slot.path, dir, ec))
            return true;
        fs::create_directories(dir, ec);
        fs::copy(slot.path, dir, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
        if (ec)
            log_error("[savegame] can't copy %s to %s: %s", slot.path.string().c_str(), dir.string().c_str(),
                      ec.message().c_str());
        return !ec;
    }

    SaveFile::SSaveFile save;
    std::string error;
    if (!SaveFile::Read(slot.path, save, error))
    {
        log_error("[savegame] %s", error.c_str());
        return false;
    }
    fs::create_directories(dir, ec);
    if (ec || !WriteFileBytes(dir / kSaveFile, save.game))
    {
        log_error("[savegame] can't write %s", (dir / kSaveFile).string().c_str());
        return false;
    }
    std::vector<uint8_t> rgba;
    int32_t width = 0, height = 0;
    fs::remove(dir / kThumbnailFile, ec);
    if (!save.thumbnail.empty() && DecodePng(save.thumbnail, rgba, width, height) &&
        width == kThumbnailWidth && height == kThumbnailHeight)
        WriteFileBytes(dir / kThumbnailFile, EncodeRetailThumbnail(rgba));
    const int32_t sectors = SectorStore::ExportDeltasTo(save.sectors, dir / kSlotCurMapDir);
    if (sectors != (int32_t)save.sectors.size())
    {
        log_error("[savegame] '%s': %d of %zu sectors couldn't be rebuilt against the module's map; the "
                  "folder at %s is incomplete", name, (int32_t)save.sectors.size() - sectors,
                  save.sectors.size(), dir.string().c_str());
        return false;
    }
    log_info("[savegame] exported '%s' as a retail slot folder to %s", name, dir.string().c_str());
    return true;
}

fs::path TSaveGame::SlotsDir()
{
    return SinglePlayerSlotsDir();
}

fs::path TSaveGame::SaveFilePath(const std::string& name)
{
    return SinglePlayerSlotsDir() / (name + SaveFile::kExtension);
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
// presents. `slot`, when given (a save file or a retail slot folder), also
// gets it.
void TSaveGame::CaptureThumbnail(const fs::path& slot, std::function<void()> captured)
{
    const bool requested = Display.RequestCapture(
        [this, slot, captured](const uint8_t* rgba, int32_t width, int32_t height) {
            if (rgba && WriteThumbnail(rgba, width, height))
            {
                ++thumbnailVersion;
                if (!slot.empty())
                    CopyThumbnailTo(slot);
            }
            if (captured)
                captured();
        });
    if (!requested)
    {
        log_info("[savegame] no display to capture a thumbnail from");
        if (captured)
            captured();
    }
}

bool TSaveGame::ReadThumbnail(const SSaveSlot& slot, std::vector<uint8_t>& rgba)
{
    if (slot.isFile)
    {
        SaveFile::SSaveFile save;
        std::string error;
        int32_t width = 0, height = 0;
        if (!SaveFile::Read(slot.path, save, error) || save.thumbnail.empty())
            return false;
        if (!DecodePng(save.thumbnail, rgba, width, height) || width != kThumbnailWidth ||
            height != kThumbnailHeight)
        {
            log_warn("[savegame] %s: the thumbnail isn't a %dx%d PNG", slot.path.string().c_str(),
                     kThumbnailWidth, kThumbnailHeight);
            return false;
        }
        return true;
    }

    std::vector<uint8_t> file;
    if (!ReadFileBytes(slot.path / kThumbnailFile, file))
        return false;
    if (!DecodeRetailThumbnail(file, rgba))
    {
        log_warn("[savegame] %s: not a %dx%d 24-bit thumbnail", (slot.path / kThumbnailFile).string().c_str(),
                 kThumbnailWidth, kThumbnailHeight);
        return false;
    }
    return true;
}

// The current thumbnail, as a PNG file.
std::vector<uint8_t> TSaveGame::ThumbnailPng()
{
    std::vector<uint8_t> file, rgba;
    if (!ReadFileBytes(ThumbnailFile(), file) || !DecodeRetailThumbnail(file, rgba))
        return {};
    return EncodePng(rgba, kThumbnailWidth, kThumbnailHeight);
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
    storedThumbnailVersion = thumbnailVersion;
    if (slot.extension() == SaveFile::kExtension)
    {
        // A save file written before the thumbnail was: rewritten with it.
        SaveFile::SSaveFile save;
        std::string error;
        if (SaveFile::Read(slot, save, error))
        {
            save.thumbnail = ThumbnailPng();
            if (SaveFile::Write(slot, std::move(save), error))
                return;
        }
        log_warn("[savegame] can't put the thumbnail into %s: %s", slot.string().c_str(), error.c_str());
        return;
    }
    std::error_code ec;
    fs::copy_file(ThumbnailFile(), slot / kThumbnailFile, fs::copy_options::overwrite_existing, ec);
    if (ec)
        log_warn("[savegame] can't copy the thumbnail into %s: %s", slot.string().c_str(),
                 ec.message().c_str());
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

// The slots: save files and retail slot folders. A name held by both is the
// newer one's.
void TSaveGame::RefreshSlots()
{
    slots.clear();

    const fs::path root = SinglePlayerSlotsDir();
    std::vector<std::pair<SSaveSlot, fs::file_time_type>> found;
    std::error_code ec;
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec))
    {
        std::error_code entryError;
        SSaveSlot slot;
        SSaveHeader header;
        std::vector<uint8_t> game;
        fs::path stamped;
        if (it->is_directory(entryError))
        {
            stamped = it->path() / kSaveFile;
            if (!fs::is_regular_file(stamped, entryError) || !ReadFileBytes(stamped, game))
                continue;
            slot.name = it->path().filename().string();
        }
        else if (it->path().extension() == SaveFile::kExtension)
        {
            SaveFile::SSaveFile save;
            std::string error;
            if (!SaveFile::Read(it->path(), save, error))
            {
                log_warn("[savegame] %s", error.c_str());
                continue;
            }
            game = std::move(save.game);
            stamped = it->path();
            slot.name = it->path().stem().string();
            slot.isFile = true;
        }
        else
            continue;
        slot.path = it->path();
        slot.module = (ReadHeader(game, header) && header.module[0]) ? header.module : ActiveModuleName();
        found.emplace_back(std::move(slot), fs::last_write_time(stamped, entryError));
    }

    std::sort(found.begin(), found.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
    for (auto& [slot, written] : found)
    {
        const auto same = std::find_if(slots.begin(), slots.end(), [&](const SSaveSlot& other) {
            return stricmp(other.name.c_str(), slot.name.c_str()) == 0;
        });
        if (same == slots.end())
            slots.push_back(std::move(slot));
        else
            *same = std::move(slot);            // the newer of the two
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
