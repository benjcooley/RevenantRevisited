// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      textbar.h - Text Bar Pane                        *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "render3d_types.h"
#include "screen.h"

#include <array>
#include <cstdarg>
#include <memory>
#include <string>

class TSurface;
struct SFontAtlas;

// REVSYNC: TTextBar @ 0x0065c5d0 (vtable 0x005a5560; docs/ui/forensics/
// TTextBar_SPEC.md). The message feed over the bottom-left of the map view.
// Messages stack up from its bottom edge, newest lowest; the three newest
// stay, older ones fade and go five seconds after they move up past them.
// The newest line doubles as the map-loading progress bar (SetHealthDisplay,
// SetLevels: the "texthealthbar" strip under the "Loading Map..." line).
//
// The 1998 pane was one line of text, or the name and health of the creature
// Locke was fighting. Retail replaced both: the feed above, and the target's
// health went to TPlyrStatusBar.
//
// Enter opens a "Message: " prompt on the newest line (TTextBar_SPEC §10):
// the typed line runs as a script line on the player when it starts with
// "@", else as a cheat word. Not ported: the multiplayer chat it sends and
// the chat feed in Pulse.
class TTextBar : public TPane
{
  public:
    // REVSYNC: a line's type (record +0x00) picks its colour in the switch of
    // Composite 0x0054cd40. Who prints each, in retail:
    enum class ELineType : int32_t
    {
        Message = 0x01,     // gold: Print, the 1-type Print
        Violet  = 0x02,     // no caller
        Pink    = 0x04,     // no caller
        System  = 0x08,     // red: multiplayer server lines (Pulse)
        Notice  = 0x10,     // sky blue: "You are too far away." (ITEMTOFAR)
        Prompt  = 0x20,     // white: the typed-message prompt
        Chat    = 0x40,     // the line's own colour, else green: chat
        Status  = 0x80,     // white: the loading line (SetHealthDisplay)
    };

    // A feed line, as retail's 0x5c-byte record.
    struct SLine
    {
        ELineType type = ELineType::Message;    // +0x00
        uint32_t color = 0;                      // +0x04, 0x00RRGGBB for Chat (0: green)
        int32_t ticks = 0;                       // +0x08, life left
        std::string text;                        // +0x0c, at most kTextChars
    };

    static constexpr int32_t kMaxLines    = 9;     // DAT_005e5800 -> +0x64
    static constexpr int32_t kPinnedLines = 3;     // DAT_005e5804 -> +0x68: never age
    static constexpr int32_t kLineTicks   = 0x78;  // a new line's life, 5 s
    static constexpr int32_t kFadeTicks   = 0x18;  // the last second fades out
    static constexpr int32_t kTextChars   = 0x4f;  // AddLine's strncpy
    static constexpr int32_t kInputChars  = 0x50;  // the typed line, prefix included (+0xa4)

    TTextBar() = default;
    ~TTextBar() override;
    TTextBar(const TTextBar&) = delete;
    TTextBar& operator=(const TTextBar&) = delete;

    bool Initialize() override;     // 0x0054bf70 (slot 0)
    void Close() override;          // 0x0054c3d0 (slot 1)
    void Hide() override;           // 0x0054c9c0 (slot 13)
    void Pulse() override;          // 0x0054c460 (slot 19)
    void Compose() override;        // 0x0054c440 (slot 20) -> Composite 0x0054cd40
    void Draw() override;           // 0x0054c600 (slot 7)
    void CharPress(int32_t key, bool down) override;    // 0x0054d4a0 (slot 28)

    // REVSYNC: 0x0054d390 -- the typed line becomes "<player>: <text>" in the
    // chat colour and runs (SubmitInput). Nothing while the prompt is closed.
    // Starting a fight commits it too.
    void CommitInput();
    // The prompt is open (+0xa0): the play screen takes no keys meanwhile.
    [[nodiscard]] bool IsPrompting() const { return prompting; }

    // REVSYNC: 0x0054d170 / 0x0054d190 -> 0x0054d1b0. printf-style; each
    // '\n'-separated piece becomes a line. Logged as "[textbar] <text>".
    [[gnu::format(printf, 2, 3)]] void Print(const char *fmt, ...);
    [[gnu::format(printf, 3, 4)]] void Print(ELineType type, const char *fmt, ...);
    // REVSYNC: 0x0054d0c0 -- push a line (dropped if empty or if it starts
    // with a space). `color` is read for Chat only.
    void AddLine(ELineType type, uint32_t color, const char *text);
    // Empties the feed and drops the loading bar and an open prompt, unrun
    // (the state Initialize leaves).
    // Port addition: retail has no such call.
    void Clear();

    // REVSYNC: 0x0054ca20 -- `name` becomes the newest line, over the bar.
    void SetHealthDisplay(const char *name);
    // REVSYNC: 0x0054ca60 -- the bar's level, 0..186 fills it (the map loader
    // passes percent * 180 / 1000); with no SetHealthDisplay, the "loadmsg" line.
    void SetLevels(int32_t newlevel, int32_t newtargetlevel);
    // REVSYNC: 0x0054cad0 -- the bar goes; the newest line stays as a message.
    void ClearHealthDisplay();

    // The feed, newest first.
    [[nodiscard]] int32_t NumLines() const { return numlines; }
    [[nodiscard]] const SLine& Line(int32_t index) const { return lines[index]; }
    [[nodiscard]] bool IsHealthDisplayed() const { return healthshown; }
    [[nodiscard]] int32_t Level() const { return level; }

  private:
    void VPrint(ELineType type, const char *fmt, va_list args);
    void BeginInput();
    void ShowInput();
    void SubmitInput(const std::string& text);
    void LayOut();
    void RecomposeAll();
    void RecomposeFirst();
    void ComposeLine(int32_t index);
    void UpdateHealthBarTexture();
    void DrawHealthBar(int32_t top);
    [[nodiscard]] int32_t SlotTop(int32_t index) const;
    [[nodiscard]] float LineAlpha(int32_t index, float frac) const;

    std::array<SLine, kMaxLines> lines{};          // +0x6c
    int32_t numlines = 0;                          // +0x60
    const SFontAtlas* font = nullptr;              // "Small" (DAT_0065abc4)
    int32_t lineheight = 0;                        // +0x78
    std::unique_ptr<TSurface> surface;             // the composed lines (+0x84..+0x8c)
    TBitmap* healthbar = nullptr;                  // "texthealthbar" in gamedata.dat
    TTextureHandle healthbartexture = kInvalidTexture;    // the strip at barhue
    int32_t barhue = -1;
    bool healthshown = false;                      // +0x90
    int32_t level = 0;                             // +0x98
    int32_t targetlevel = 0;                       // +0x9c
    bool barshown = false;                         // the newest line was last composed over the bar
    bool prompting = false;                        // +0xa0
    int32_t inputroom = 0;                         // +0xa4: kInputChars less the prefix
    std::string input;                             // +0xd0: the typed text
    uint32_t composerequest = 1;                   // bumped by each Composite retail would run
    uint32_t composed = 0;                         // the request the surface shows
};
