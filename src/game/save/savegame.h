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
// retail's TPlayScreen did). Retail's format, sequence and addresses:
// docs/gameflow/forensics/SAVE_GAME.md.
//
// The port saves a slot as one file (SaveFile, docs/gameflow/SAVE_FORMAT.md):
// retail's game.sav as is, a PNG thumbnail and the modified sectors as
// diffs against the module's. It loads those and retail's slot folders
// alike, and writes a retail slot folder on request (ExportRetail).
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

// A save slot: the port's save file <SaveGamePath>Single/<name>.rvsave, or
// retail's slot folder <SaveGamePath>Single/<name>/ holding game.sav,
// CurMap/ and the ss.bmp thumbnail.
struct SSaveSlot
{
    std::string           name;
    std::filesystem::path path;               // the save file, or the folder
    bool                  isFile = false;     // a save file (else a retail folder)
    std::string           module;             // from the slot's header
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
    // ("Default Save" if null) as a save file, creating it.
    bool Save(const char* name);

    // Writes slot `name` as a retail slot folder at `dir` (game.sav, ss.bmp,
    // CurMap/), which retail loads, from a save file or a retail folder.
    // A save file's sectors are rebuilt against the active module's map.
    bool ExportRetail(const char* name, const std::filesystem::path& dir);

    // REVSYNC: 0x0048d260 — rescan the save slots on disk.
    void RefreshSlots();

    // REVSYNC: 0x0048d6d0 — index of slot `name` (case-insensitive) or -1.
    [[nodiscard]] int32_t FindSlot(const char* name) const;

    [[nodiscard]] const std::vector<SSaveSlot>& Slots() const { return slots; }

    // Where the single-player slots live: <SavePath>/<SaveGamePath>Single.
    [[nodiscard]] static std::filesystem::path SlotsDir();
    // Slot `name`'s save file there: <name>.rvsave.
    [[nodiscard]] static std::filesystem::path SaveFilePath(const std::string& name);
    // The slot this run wrote last (empty: none yet).
    [[nodiscard]] const std::string& LastSaved() const { return lastSaved; }

    // True while a load is replacing the world.
    [[nodiscard]] bool IsLoading() const { return loading; }

    // REVSYNC: the thumbnail TPlayScreen writes when the in-game menu or the
    // save dialog opens, or on a quick save: the next frame becomes the
    // thumbnail the next save stores. `slot`, when given (a save file), also
    // gets it.
    // `captured` runs once the frame has been read back (at once when there
    // is no display to read), so a menu can open over the game only after
    // the picture without it is taken, as retail's did.
    void CaptureThumbnail(const std::filesystem::path& slot = {},
                          std::function<void()> captured = nullptr);

    // The thumbnail SaveBMP writes at scale 3: 216x160 (SAVE_GAME.md §11.7).
    static constexpr int32_t kThumbnailWidth  = 216;
    static constexpr int32_t kThumbnailHeight = 160;

    // `slot`'s thumbnail as the load and save dialogs show it, RGBA8, top
    // row first: a save file's PNG, or a retail folder's ss.bmp through
    // TBitmap::LoadBMP's rules (0x004a2ce0, as the dialogs use it,
    // 0x00539590: a 24-bit file of exactly the thumbnail's size). False when
    // the slot has none, or not one of the thumbnail's size.
    [[nodiscard]] static bool ReadThumbnail(const SSaveSlot& slot, std::vector<uint8_t>& rgba);

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

    // Loads the game.sav bytes `data` (from `source`, for the log), the
    // working set from `restoreSectors` (none: a new game's empty one).
    bool LoadGame(const std::vector<uint8_t>& data, const std::string& source,
                  const std::function<void()>& restoreSectors);
    void ResetWorld(const std::function<void()>& restoreSectors);
    bool ReadSoldUniques(TInputStream& is, const SSaveHeader& header);
    bool ReadPlayers(TInputStream& is, const SSaveHeader& header);
    void WriteBody(TOutputStream& os) const;
    static SPlayerHudWords CurrentHudWords(const TPlayer& player);
    static std::filesystem::path ThumbnailFile();
    static bool WriteThumbnail(const uint8_t* rgba, int32_t width, int32_t height);
    // The current thumbnail (ThumbnailFile) as a PNG file's bytes; empty
    // when there is none.
    static std::vector<uint8_t> ThumbnailPng();
    // The current thumbnail into `slot` (a save file or a retail slot
    // folder), or the next frame's when none is newer than the last save's.
    void StoreThumbnail(const std::filesystem::path& slot);
    void CopyThumbnailTo(const std::filesystem::path& slot);
    [[nodiscard]] std::vector<uint8_t> GameBytes() const;
    bool SaveFileSlot(const char* name);
    bool SaveFolder(const char* name);

    std::vector<SSaveSlot>   slots;
    std::string              lastSaved;
    std::vector<SSoldUnique> soldUniques;
    bool                     loading = false;
    int32_t                  thumbnailVersion = 0;         // bumped by each thumbnail written
    int32_t                  storedThumbnailVersion = 0;   // the one the last save stored
};
