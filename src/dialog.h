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

#include "button.h"

#include <array>
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

class TDialogPane;
class TMulti;
class TSurface;
struct SFontAtlas;

// REVSYNC: dialog entry, ctor 0x00533f10 (0x160 bytes; DIALOG.md §4.2). One
// floating box: a spoken line, or the response list -- the speaker's portrait
// in the "Ring" on the left, the wrapped texts on the right. Lifetimes, fades,
// slides and highlights step per simulation tick (24 Hz) as retail's do; the
// presentation interpolates between the steps.
class TDialogEntry
{
  public:
    enum class EMode : int32_t { NpcSpeech = 1, PlayerSpeech = 2, Responses = 3 };

    static constexpr int32_t kFadeTicks      = 12;     // fade in or out
    static constexpr int32_t kSlideTicks     = 12;     // restack slide
    static constexpr int32_t kHighlightTicks = 8;      // hovered choice ramp
    static constexpr int32_t kMaxTexts       = 8;
    static constexpr int32_t kNoTimeout      = -1;
    static constexpr int32_t kWidth          = 400;    // +0x28
    static constexpr int32_t kTextLeft       = 50;     // text rects x 50..399
    static constexpr int32_t kWrapWidth      = 350;    // 0x15e
    static constexpr int32_t kTextGap        = 10;     // between texts
    static constexpr int32_t kMinHeight      = 44;     // 0x2c, texts centred
    static constexpr int32_t kPortraitCenter = 22;     // portrait + Ring centre (0x16)

    // `color` / `hicolor` as retail stores them: 0x00RRGGBB.
    TDialogEntry(TDialogPane& owner, TObjectInstance* speaker, EMode mode,
                 uint32_t color, uint32_t hicolor, std::vector<std::string> texts,
                 std::vector<std::string> labels, int32_t ticks);
    // REVSYNC: dtor @ 0x005343e0 -- its choice buttons leave the pane.
    ~TDialogEntry();
    TDialogEntry(const TDialogEntry&) = delete;
    TDialogEntry& operator=(const TDialogEntry&) = delete;

    // REVSYNC: Pulse @ 0x005348f0 -- one tick of the lifetime, the slide, the
    // fade and each choice's highlight; the buttons follow the box.
    void Pulse();
    // REVSYNC: Dismiss @ 0x00534a40 -- fade out; the choice buttons go.
    void Dismiss();

    // Layout (TDialogPane::Pulse 0x005351d0): the stack's base on screen and
    // this entry's place in it. A new entry appears there; a placed one
    // slides to it over kSlideTicks.
    void SetBase(int32_t x, int32_t y) { basex = x; basey = y; }
    void MoveTo(int32_t offset);
    // Laid out on screen (retail +0x20 leaves -10000); its clock runs from then.
    [[nodiscard]] bool IsPlaced() const { return offx != kNotPlaced; }

    // REVSYNC: Render @ 0x00534470 -- portrait, Ring and texts into the
    // entry's surface (once; the text is white and takes its colour when drawn).
    void Compose();
    [[nodiscard]] bool IsComposed() const { return composed; }
    // REVSYNC: DrawOverlay @ 0x00534b60 -- the surface over the map, faded,
    // the text tinted, hovered choices ramped toward the highlight colour.
    // `frac` is the progress (0..1) from this tick toward the next.
    void Draw(float frac) const;

    [[nodiscard]] EMode Mode() const { return mode; }
    [[nodiscard]] TObjectInstance* Speaker() const;
    [[nodiscard]] const std::vector<std::string>& Texts() const { return texts; }
    [[nodiscard]] const std::vector<std::string>& Labels() const { return labels; }
    [[nodiscard]] bool IsDismissed() const { return dismissed; }
    // Dismissed and faded out: the pane deletes it.
    [[nodiscard]] bool IsGone() const { return dismissed && fade == 0; }
    // 0 (invisible) .. kFadeTicks (fully shown).
    [[nodiscard]] int32_t Fade() const { return fade; }
    [[nodiscard]] int32_t Height() const { return height; }
    // Where the entry is and its clock, as the last Pulse left them (retail
    // +0x14 and +0x18..+0x44). Read by the retail A/B dump.
    struct SPlacement
    {
        int32_t ticksleft, basex, basey, offx, offy, targetx, targety, posx, posy, stepx, stepy;
    };
    [[nodiscard]] SPlacement Placement() const
    {
        return {ticksleft, basex, basey, offx, offy, targetx, targety, posx, posy, stepx, stepy};
    }

  private:
    static constexpr int32_t kNotPlaced = -10000;

    void RemoveButtons();
    [[nodiscard]] TBitmap* Portrait() const;
    void TraceGeometry();

    TDialogPane& pane;                              // +0x00
    TSafeRef<TObjectInstance> speaker;              // +0x04
    EMode mode = EMode::NpcSpeech;                  // +0x08
    uint32_t color = 0xffffff;                      // +0x0c
    uint32_t hicolor = 0xffffff;                    // +0x10
    int32_t ticksleft = kNoTimeout;                 // +0x14
    int32_t basex = 0, basey = 0;                   // +0x18 / +0x1c
    int32_t offx = kNotPlaced, offy = kNotPlaced;   // +0x20 / +0x24
    int32_t height = 0;                             // +0x2c
    int32_t targetx = kNotPlaced, targety = kNotPlaced;    // +0x30 / +0x34
    int32_t posx = 0, posy = 0;                     // +0x38 / +0x3c, 16.16
    int32_t stepx = 0, stepy = 0;                   // +0x40 / +0x44, 16.16
    std::unique_ptr<TSurface> surface;              // +0x48
    bool composed = false;
    bool dismissed = false;                         // +0x50
    int32_t fade = 0;                               // +0x54
    int32_t fadetarget = kFadeTicks;                // +0x58
    std::vector<std::string> texts;                 // +0x5c / +0x60
    std::vector<std::string> labels;                // the response buttons' names (mode 3)
    std::vector<std::vector<std::string>> lines;    // each text, wrapped
    std::array<SRect, kMaxTexts> rects{};           // +0x80, inclusive
    std::array<TButton*, kMaxTexts> buttons{};      // +0x100, owned by the pane
    std::array<int32_t, kMaxTexts> highlight{};     // +0x120
    std::array<int32_t, kMaxTexts> hightarget{};    // +0x140
    std::vector<int32_t> traced;                    // the geometry last written to the trace
};

// REVSYNC: TDialogPane @ 0x00667cc8 (vtable 0x005a5c60, 0x1f0 bytes;
// DIALOG.md §4). A button pane, not a panel: it manages the floating
// entries -- NPC lines stacked from the top of the map view, the player's
// lines and the response list from the bottom -- and the choices a script
// offers, each a button named by its label.
class TDialogPane : public TButtonPane
{
  public:
    static constexpr int32_t kMaxChoices = 8;

    TDialogPane() : TButtonPane(0, 0, WIDTH, HEIGHT) {}

    bool Initialize() override;                     // 0x00534fd0
    void Close() override;                          // 0x00535060
    void Hide() override;                           // 0x00535120
    void Pulse() override;                          // 0x005351d0
    void KeyPress(int32_t key, bool down) override; // 0x00535610
    void Joystick(int32_t key, bool down) override; // 0x00535760
    void OnControl(TButton *button, int32_t msg) override;  // 0x005362b0
    void Compose() override;                        // 0x00535500
    void Draw() override;                           // 0x00535550
    void DrawBackground() override {}               // entries draw in Compose/Draw;
                                                    // the choice buttons are invisible

    // REVSYNC: AddChoice @ 0x00535870 -- `text` is the dialog tag shown for
    // the choice; `label` is where the script goes if it's picked. After an
    // answer, the next choice starts a fresh list.
    void AddChoice(const char *label, const char *text);
    // REVSYNC: ShowResponses @ 0x00535e90 -- show the choices to `player`
    // and turn control off (on, for `controlon`) until one is picked.
    bool ShowResponses(TObjectInstance* player, bool controlon);
    // REVSYNC: AddSpeech @ 0x00535b90 -- `speaker` says `text` for `ticks`.
    void AddSpeech(TObjectInstance* speaker, const char *text, int32_t ticks);
    // A new entry (a line, or the response list), laid out at the next
    // Pulse. AddSpeech and ShowResponses decide its mode and colour.
    TDialogEntry& AddEntry(TObjectInstance* speaker, TDialogEntry::EMode mode, uint32_t color,
                           std::vector<std::string> texts, std::vector<std::string> labels,
                           int32_t ticks);
    // The font entries wrap and measure with, and its line height
    // (Initialize: the "Dialog" font; without an atlas, texts break only
    // at '\n' -- what the headless retail A/B uses).
    void UseFont(const SFontAtlas* atlas, int32_t lineheight);
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

    // For the entries: the "Dialog" font (0x0065c134) and its line height,
    // the "Ring" over the portrait, and the response buttons.
    [[nodiscard]] const SFontAtlas* Font() const { return font; }
    [[nodiscard]] int32_t LineHeight() const { return lineheight; }
    [[nodiscard]] TBitmap* Ring() const { return ring; }
    TButton* NewChoiceButton(const char *label);

  private:
    static constexpr int32_t kSpeakerSlots = 16;

    void Choose(int32_t index);                     // a click or key 1-6
    void LayOut();
    void DeleteGoneEntries();
    uint32_t NpcColor(const TObjectInstance* speaker);

    std::vector<std::unique_ptr<TDialogEntry>> entries;    // +0x17c..
    TDialogEntry* responses = nullptr;                      // +0x190
    TSafeRef<TObjectInstance> answering;                   // +0x194
    std::vector<std::string> choicetexts;                  // +0x198[8]
    std::vector<std::string> choicelabels;                 // +0x1b8[8]
    int32_t chosen = -1;                                    // +0x1dc
    bool committed = false;                                 // +0x1e4
    bool controlonwhilechoosing = false;                    // +0x1e8
    bool savedcontrol = true;                               // +0x1ec

    // NPC colour slots (0x0066f6f8, counter 0x0066f73c): speakers by map
    // index, kept for the whole run as retail's global table is.
    std::array<int32_t, kSpeakerSlots> speakerslots = {-1, -1, -1, -1, -1, -1, -1, -1,
                                                       -1, -1, -1, -1, -1, -1, -1, -1};
    int32_t lastslot = 0;

    const SFontAtlas* font = nullptr;
    int32_t lineheight = 0;
    TMulti* statusbardat = nullptr;                        // the Ring's archive
    TBitmap* ring = nullptr;
};

#endif