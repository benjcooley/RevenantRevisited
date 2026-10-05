// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   savegamepane.h - the load and save dialogs                          *
// *************************************************************************
//
// Retail's load dialog (cls_0x5b9584, instance 0x0066f8d0) and save dialog
// (cls_0x5b963c, instance 0x0066fb08): loadgame.def / savegame.def over the
// save slot list, with the selected slot's thumbnail, name and module. The
// load dialog runs in the game (over it, modally) and from the title's Load
// Game screen; the save dialog only in the game. Behaviour:
// docs/gameflow/forensics/INGAME_MENU.md §5-6; layout:
// docs/ui/forensics/{LoadGame,SaveGame}Def_SPEC.md.
#pragma once

#include "defpane.h"
#include "render3d_types.h"   // TTextureHandle

#include <cstdint>
#include <string>
#include <vector>

// What the two dialogs share (retail repeats it in each class): the slot list,
// the thumbnail and the fields of the selected slot.
class TSaveSlotPane : public TDefPane
{
  public:
    // Retail results (+0x5c): the dialog's button that ended it.
    static constexpr int32_t RESULT_EXIT = 0;
    static constexpr int32_t RESULT_DONE = 1;

    void Close() override;

  protected:
    // REVSYNC: Open @ 0x00539380 / 0x005399f0 -- rescan the slots, a black
    // thumbnail, `defName`.def panel "default" full screen at display (x,y).
    bool OpenSlots(const char* defName, uint32_t defFlags, int32_t x, int32_t y);

    // REVSYNC: Control @ 0x00539590 / 0x00539c00, event 1: the slot names into
    // "gamelist", Enter on the dialog's button and ESC on "exit", the last
    // slot selected.
    void OnOpened() override;
    // Event 5000 on "gamelist": the slot's thumbnail, name, module and
    // character.
    void OnListSelect(const SDefWidget& list, int32_t row) override;
    // REVSYNC: field getter @ 0x00539550 / 0x00539bc0 -- "picture" is the
    // thumbnail.
    void DrawField(const SDefWidget& widget) override;
    void Compose() override;

    // The button that confirms the dialog ("loadgame" / "savegame").
    [[nodiscard]] virtual const char* ConfirmButton() const = 0;

    // The selected slot's row, -1 for none (retail +0x234).
    [[nodiscard]] int32_t Selected() const { return selected; }

  private:
    void ReleaseThumbnail();

    int32_t              selected  = -1;
    TTextureHandle       thumbnail = kInvalidTexture;   // retail +0x19c, 216x160
    std::vector<uint8_t> picture;                       // RGBA waiting to be uploaded
    bool                 pictureDirty = false;
    bool                 hasPicture   = false;          // retail +0x1a0
};

class TLoadGamePane : public TSaveSlotPane
{
  public:
    // REVSYNC: 0x00539380 -- `fromGame` (retail +0x198, DAT_0066fa68) gives
    // the in-game chrome and fade (DEF flags 0x11, else 0).
    bool OpenLoad(bool fromGame, int32_t x, int32_t y);

    // The slot to load once "loadgame" is pressed.
    [[nodiscard]] std::string SelectedSlot() const;

  protected:
    // REVSYNC: 0x00539590 event 3000 -- "loadgame" needs a slot selected;
    // then, like "exit", it goes to the host's activation handler, which
    // loads or leaves (in the game: ends the modal; from the title: switches
    // screens).
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    [[nodiscard]] const char* ConfirmButton() const override { return "loadgame"; }
};

class TSaveGamePane : public TSaveSlotPane
{
  public:
    // REVSYNC: 0x005399f0 -- always in-game (DEF flags 0x11).
    bool OpenSave(int32_t x, int32_t y);

    // The name to save under once the dialog has ended with RESULT_DONE.
    [[nodiscard]] const std::string& SaveName() const { return savename; }

    // REVSYNC: 0x00539c00 -- the name typed: every character outside
    // 0x20..0x7e and every . \ / ? * | < > : " removed.
    [[nodiscard]] static std::string SanitizeName(const std::string& typed);

  protected:
    void OnListSelect(const SDefWidget& list, int32_t row) override;
    // REVSYNC: 0x00539c00 event 3000 -- "savegame" with a usable name ends
    // the dialog with RESULT_DONE (retail saved right there); "exit" with
    // RESULT_EXIT.
    void OnActivate(const SDefWidget& widget, int32_t buttonIndex) override;
    [[nodiscard]] const char* ConfirmButton() const override { return "savegame"; }

  private:
    std::string savename;
};
