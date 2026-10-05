// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Revenant Revisited (port) - 2026                     *
// *            savegame.h - save slots and the save file format           *
// *************************************************************************
//
// TSaveGame is retail's save manager: the list of save slots, the save file
// format, loading a slot or the module's newgame.sav into a reset world, and
// writing a slot. It also carries the merchant unique-item table, which
// persists only through the save file.
//
// Loading replaces the players, game states and sector working set; it does
// not load sectors or place the players in them (TGameSession does that, as
// retail's TPlayScreen did). Format, sequence and retail addresses:
// docs/gameflow/forensics/SAVE_GAME.md.
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

class TInputStream;
class TOutputStream;
class TPlayer;
struct SPlayerHudWords;

// The 0x80-byte header that starts every save file (SAVE_GAME.md §3.1).
struct SSaveHeader
{
    static constexpr int32_t kSize          = 0x80;
    static constexpr int32_t kModuleNameLen = 32;

    int32_t gametime     = 0;   // hundredths of a second
    int32_t multiplayer  = 0;   // a 0x200-byte multiplayer block follows the header
    int32_t playerformat = 0;   // 0: one player (legacy); 2: counted player list + merchant table
    int32_t version      = 0;   // object stream version, passed to LoadObject
    char    module[kModuleNameLen] = {};   // module dirname

    // False when fewer than kSize bytes remain.
    [[nodiscard]] bool Read(TInputStream& is);
    void Write(TOutputStream& os) const;
};

// A save slot: <SaveGamePath>/Single/<name>/ holding game.sav, CurMap/ and
// the ss.bmp thumbnail.
struct SSaveSlot
{
    std::string           name;
    std::filesystem::path dir;
    std::string           module;   // from the slot's header
};

class TSaveGame
{
  public:
    // REVSYNC: LoadGame @ 0x0048df70 — load slot `name` ("Default Save" if
    // null). The slot must be in Slots(); see RefreshSlots.
    bool Load(const char* name);

    // REVSYNC: LoadNewGame @ 0x0048e610 (LoadGame("newgame", 1)) — the active
    // module's newgame.sav into an empty working set.
    bool LoadNewGame();

    // REVSYNC: SaveGame @ 0x0048d720 — write the game to slot `name`
    // ("Default Save" if null), creating it.
    bool Save(const char* name);

    // REVSYNC: 0x0048d260 — rescan the save slots on disk.
    void RefreshSlots();

    // REVSYNC: 0x0048d6d0 — index of slot `name` (case-insensitive) or -1.
    [[nodiscard]] int32_t FindSlot(const char* name) const;

    [[nodiscard]] const std::vector<SSaveSlot>& Slots() const { return slots; }

    // True while a load is replacing the world.
    [[nodiscard]] bool IsLoading() const { return loading; }

    // REVSYNC: the thumbnail TPlayScreen writes when the in-game menu or the
    // save dialog opens, or on a quick save: the next frame becomes the
    // thumbnail the next save stores. `slot`, when given, also gets a copy.
    // `captured` runs once the frame has been read back (at once when there
    // is no display to read), so a menu can open over the game only after
    // the picture without it is taken, as retail's did.
    void CaptureThumbnail(const std::filesystem::path& slot = {},
                          std::function<void()> captured = nullptr);

    // The thumbnail SaveBMP writes at scale 3: 216x160 (SAVE_GAME.md §11.7).
    static constexpr int32_t kThumbnailWidth  = 216;
    static constexpr int32_t kThumbnailHeight = 160;

    // REVSYNC: TBitmap::LoadBMP @ 0x004a2ce0 as the load and save dialogs
    // use it (0x00539590): `slot`'s ss.bmp into a thumbnail-sized picture.
    // Retail took only a file of exactly the bitmap's size; thumbnails are
    // 24-bit. RGBA8, top row first. False when the slot has none or it
    // isn't one.
    [[nodiscard]] static bool ReadThumbnail(const std::filesystem::path& slot,
                                            std::vector<uint8_t>& rgba);

    // Shows the HUD sidebar the way the saved HUD words in the player record
    // say (SAVE_GAME.md §11.4). Called by a load and again by the PlayScreen
    // once it has built the HUD.
    static void RestoreHud(const SPlayerHudWords& words);

    // Merchant unique items already bought (SAVE_GAME.md §6).
    // REVSYNC: HasPair @ 0x0048e630 / AddPair @ 0x0048e670.
    [[nodiscard]] bool HasSoldUnique(int32_t objclass, int32_t objtype) const;
    void AddSoldUnique(int32_t objclass, int32_t objtype);

  private:
    struct SSoldUnique
    {
        int32_t objclass = 0;
        int32_t objtype  = 0;
    };

    // Loads `file` (resolved through the resource layer). `slotCurMap` is
    // the slot's sector working set, or empty for a new game.
    bool LoadFile(const char* file, const std::filesystem::path& slotCurMap);
    void ResetWorld(const std::filesystem::path& slotCurMap);
    bool ReadSoldUniques(TInputStream& is, const SSaveHeader& header);
    bool ReadPlayers(TInputStream& is, const SSaveHeader& header);
    void WriteBody(TOutputStream& os) const;
    static SPlayerHudWords CurrentHudWords(const TPlayer& player);
    static std::filesystem::path ThumbnailFile();
    static bool WriteThumbnail(const uint8_t* rgba, int32_t width, int32_t height);
    void StoreThumbnail(const std::filesystem::path& slot);
    void CopyThumbnailTo(const std::filesystem::path& slot);

    std::vector<SSaveSlot>   slots;
    std::vector<SSoldUnique> soldUniques;
    bool                     loading = false;
    int32_t                  thumbnailVersion = 0;         // bumped by each thumbnail written
    int32_t                  storedThumbnailVersion = 0;   // the one the last save stored
};
