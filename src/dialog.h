// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.h - TDialogPane module                       *
// *************************************************************************

#ifndef _DIALOG_H
#define _DIALOG_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "saferef.h"

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

// REVSYNC: TDialogList @ 0x0065d4d0 (DIALOG.md §3.6). Two tables of
// {tag, line}, each sorted by upper-cased tag: the game-wide base table
// (<ClassDefPath><Language>.def: UI text, item names, messages), loaded at
// boot, and the module's (its dialog), loaded when the module is mounted.
// Ids run through the base table, then the module's. Lookups never fail: a
// miss answers "[TAG]" or "[badid]", as retail's do.
class TDialogList
{
  public:
    // REVSYNC: 0x0049ceb0 -- the base table. Idempotent.
    bool Initialize();
    // REVSYNC: 0x0049d2a0 -- the active module's table: <Language>_dialog.def,
    // else <Language>.def, else english.def. Replaces any earlier module's.
    bool LoadModule();
    void Close();

    // REVSYNC: 0x0049d6d0 -- the id of `tag`, base table first; -1 if absent.
    [[nodiscard]] int32_t FindLine(const char *tag) const;
    // REVSYNC: 0x0049d780 / 0x0049d7c0 -- "[badid]" out of range.
    [[nodiscard]] const char *GetLine(int32_t id) const;
    [[nodiscard]] const char *GetTag(int32_t id) const;
    // REVSYNC: 0x0049d800 -- "[TAG]" when absent.
    [[nodiscard]] const char *GetLine(const char *tag) const;

  private:
    struct SLine
    {
        std::string tag;            // upper case
        std::string line;
    };
    using TTable = std::vector<SLine>;

    static bool LoadTable(const char *path, TTable &table);
    static int32_t Find(const TTable &table, const std::string &tag);

    TTable base;
    TTable module;
    bool initialized = false;
    mutable std::unordered_map<std::string, std::string> misses;    // "[TAG]" answers
};

// ****************************************************************
// * TDialogPane - speech and response boxes over the map (retail) *
// ****************************************************************

// Global function for translating dialog lines

void SetDialogContext(TObjectInstance* context); // The script context
char *DialogLine(const char *line, char *buf, int32_t buflen);

// REVSYNC: dialog entry, ctor 0x00533f10 (0x160 bytes; DIALOG.md §4.2). One
// floating box: a spoken line, or the response list. Lifetimes and fades
// count simulation ticks (24 Hz) as retail's do; the presentation reads
// their progress.
class TDialogEntry
{
  public:
    enum class EMode : int32_t { NpcSpeech = 1, PlayerSpeech = 2, Responses = 3 };

    static constexpr int32_t kFadeTicks   = 12;    // fade in or out
    static constexpr int32_t kMaxTexts    = 8;
    static constexpr int32_t kNoTimeout   = -1;

    TDialogEntry(TObjectInstance* speaker, EMode mode, std::vector<std::string> texts,
                 std::vector<std::string> labels, int32_t ticks);

    // REVSYNC: Pulse @ 0x005348f0 -- the lifetime and the fade, one tick.
    void Pulse();
    // REVSYNC: Dismiss @ 0x00534a40 -- start fading out.
    void Dismiss();
    // Laid out on screen (retail +0x20 leaves -10000); its clock runs from then.
    void Place();
    [[nodiscard]] bool IsPlaced() const { return placed; }

    [[nodiscard]] EMode Mode() const { return mode; }
    [[nodiscard]] TObjectInstance* Speaker() const;
    [[nodiscard]] const std::vector<std::string>& Texts() const { return texts; }
    [[nodiscard]] const std::vector<std::string>& Labels() const { return labels; }
    [[nodiscard]] bool IsDismissed() const { return dismissed; }
    // Dismissed and faded out: the pane deletes it.
    [[nodiscard]] bool IsGone() const { return dismissed && fade == 0; }
    // 0 (invisible) .. kFadeTicks (fully shown).
    [[nodiscard]] int32_t Fade() const { return fade; }

  private:
    TSafeRef<TObjectInstance> speaker;      // +0x04
    EMode mode = EMode::NpcSpeech;          // +0x08
    int32_t ticksleft = kNoTimeout;         // +0x14
    bool placed = false;                    // +0x20 != -10000
    bool dismissed = false;                 // +0x50
    int32_t fade = 0;                       // +0x54
    int32_t fadetarget = kFadeTicks;        // +0x58
    std::vector<std::string> texts;         // +0x5c/+0x60
    std::vector<std::string> labels;        // the response buttons' names (mode 3)
};

// REVSYNC: TDialogPane @ 0x00667cc8 (vtable 0x005a5c60, 0x1f0 bytes;
// DIALOG.md §4). Not a panel: it manages the floating entries -- NPC lines
// stacked from the top of the map view, the player's lines and the response
// list from the bottom -- and the choices a script offers.
class TDialogPane : public TPane
{
  public:
    static constexpr int32_t kMaxChoices = 8;

    TDialogPane() : TPane(0, 0, WIDTH, HEIGHT) {}

    bool Initialize() override;                     // 0x00534fd0
    void Close() override;                          // 0x00535060
    void Hide() override;                           // 0x00535120
    void Pulse() override;                          // 0x005351d0
    void KeyPress(int32_t key, bool down) override; // 0x00535610
    void Joystick(int32_t key, bool down) override; // 0x00535760

    // REVSYNC: AddChoice @ 0x00535870 -- `text` is the dialog tag shown for
    // the choice; `label` is where the script goes if it's picked. After an
    // answer, the next choice starts a fresh list.
    void AddChoice(const char *label, const char *text);
    // REVSYNC: ShowResponses @ 0x00535e90 -- show the choices to `player`
    // and turn control off (on, for `controlon`) until one is picked.
    bool ShowResponses(TObjectInstance* player, bool controlon);
    // REVSYNC: AddSpeech @ 0x00535b90 -- `speaker` says `text` for `ticks`.
    void AddSpeech(TObjectInstance* speaker, const char *text, int32_t ticks);
    // REVSYNC: SkipSpeech @ 0x00536010 -- silence and dismiss the spoken
    // lines; nothing while the choices are up.
    void SkipSpeech();
    // REVSYNC: ClearResponses @ 0x00535a10
    void ClearResponses();
    // REVSYNC: ClearSpeech @ 0x00535d80
    void ClearSpeech(bool now);
    // REVSYNC: ResetForLoad @ 0x005360f0
    void ResetForLoad();

    [[nodiscard]] bool IsShowingResponses() const { return responses != nullptr; }
    // The label of the choice picked this tick, for the waiting script
    // (retail +0x1e4/+0x1dc), else nullptr.
    [[nodiscard]] const char *CommittedLabel() const;
    // The tag of the last choice picked (+0x1dc survives the commit, for
    // `say choice`), else nullptr.
    [[nodiscard]] const char *ChosenText() const;
    [[nodiscard]] const std::vector<std::unique_ptr<TDialogEntry>>& Entries() const { return entries; }

  private:
    void Choose(int32_t index);                     // a click or key 1-6
    void DeleteGoneEntries();

    std::vector<std::unique_ptr<TDialogEntry>> entries;    // +0x17c..
    TDialogEntry* responses = nullptr;                      // +0x190
    TSafeRef<TObjectInstance> answering;                   // +0x194
    std::vector<std::string> choicetexts;                  // +0x198[8]
    std::vector<std::string> choicelabels;                 // +0x1b8[8]
    int32_t chosen = -1;                                    // +0x1dc
    bool committed = false;                                 // +0x1e4
    bool controlonwhilechoosing = false;                    // +0x1e8
    bool savedcontrol = true;                               // +0x1ec
};

#endif