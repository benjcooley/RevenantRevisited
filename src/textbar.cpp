// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     textbar.cpp - Text Bar Pane                       *
// *************************************************************************

#include "textbar.h"

#include "bitmap.h"
#include "bitmapdecode.h"
#include "cheats.h"
#include "command.h"
#include "ctrlmap.h"
#include "dialog.h"
#include "display.h"
#include "font.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "parse.h"
#include "player.h"
#include "playscreen.h"
#include "renderer.h"
#include "sound.h"
#include "surface.h"
#include "textencoding.h"
#include "time.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

using ELineType = TTextBar::ELineType;

// REVSYNC: the line colours, built by the static initializers at
// 0x0054be70..0x0054bf68 and picked in Composite 0x0054cd40. As 0x00RRGGBB:
// the stored bytes are B, G, R, and the GDI text call swaps them into its
// COLORREF (0x004be2b0), so byte 2 is red.
constexpr uint32_t kMessageColor = 0xffc800;   // 0x0067064c
constexpr uint32_t kVioletColor  = 0xb400ff;   // 0x00670664
constexpr uint32_t kPinkColor    = 0xff00b4;   // 0x00670654
constexpr uint32_t kSystemColor  = 0xff2828;   // 0x00670668
constexpr uint32_t kNoticeColor  = 0x00beff;   // 0x0067065c
constexpr uint32_t kPromptColor  = 0xffffff;   // 0x00670658
constexpr uint32_t kChatColor    = 0x00d200;   // 0x00670660, a chat line without its own
constexpr uint32_t kStatusColor  = 0xffffff;   // 0x00670650

// REVSYNC: 0x0054cd40 draws each line's text at (4, 9) of its line surface
// (0x00438ed0, single line, left, font flags 0x400: the 3-pass black shadow).
// 9 is the baseline: retail captures put the cap tops 2 px and the baseline
// 9 px below each 12 px line's top.
constexpr int32_t kTextX        = 4;
constexpr int32_t kTextBaseline = 9;

// REVSYNC: DrawHealthBar 0x0054cb00 -- the strip (200 x 11) at (x, 1) of the
// newest line, x = min(0, level - 186): it slides in from the left as the
// level rises and sits whole at 186.
constexpr int32_t kBarTop       = 1;
constexpr int32_t kBarFullLevel = 0xba;
constexpr int32_t kLevelStep    = 4;

// The side tabs' strip at the map view's right edge (TSideTabsPane
// 0x0065be50, 52 px). TPlayScreen's layout (0x0047bc50) makes the bar the
// screen's width less the side pane and the tabs.
constexpr int32_t kSideTabsWidth = 52;

// Rows of space around each line in the composed surface. Retail draws each
// line into a surface one line tall, which clips the shadow and descenders
// at the line's edges; the port keeps the lines apart instead and draws only
// each line's own rows.
constexpr int32_t kSlotPad = 4;

// Print's buffer: retail's is 256 bytes on the stack, filled by an unbounded
// vsprintf; the port bounds it.
constexpr size_t kPrintChars = 256;

// The prompt's keys, as WM_CHAR delivers them (CharPress 0x0054d4a0).
constexpr int32_t kCharBackspace = 0x08;
constexpr int32_t kCharEnter     = 0x0d;

uint32_t LineColor(const TTextBar::SLine& line)
{
    switch (line.type)
    {
        case ELineType::Violet: return kVioletColor;
        case ELineType::Pink:   return kPinkColor;
        case ELineType::System: return kSystemColor;
        case ELineType::Notice: return kNoticeColor;
        case ELineType::Prompt: return kPromptColor;
        case ELineType::Chat:   return line.color ? line.color : kChatColor;
        case ELineType::Status: return kStatusColor;
        case ELineType::Message:
        default:                return kMessageColor;
    }
}

float Red(uint32_t c)   { return float((c >> 16) & 0xff) / 255.0f; }
float Green(uint32_t c) { return float((c >> 8) & 0xff) / 255.0f; }
float Blue(uint32_t c)  { return float(c & 0xff) / 255.0f; }

// REVSYNC: 0x0054cb00 -- the strip's hue (degrees) for a level: red when
// empty, toward green as it fills.
int32_t BarHue(int32_t level)
{
    const int32_t hue = level * 0x9b / 0xb0;
    return hue < 0x11 ? 0 : hue - 0x10;
}

}  // namespace

TTextBar::~TTextBar() = default;

// REVSYNC: 0x0054bf70. Nine lines of the "Small" font (Arial 12, drawn with
// Arimo), each the font's height plus LEXTRA tall, anchored to the bottom of
// the map view; an empty feed, no loading bar. Retail composes into three
// mosaic surfaces as wide as the display; the port uses one render target.
bool TTextBar::Initialize()
{
    if (IsOpen())
        return true;

    font = FontTable ? FontTable->Atlas("Small") : nullptr;
    const TGenericFont* small = FontTable ? FontTable->FindFont("Small") : nullptr;
    lineheight = small ? small->height + small->lextra : 0;
    if (!font || lineheight <= 0)
    {
        log_error("[textbar] the \"Small\" font is missing");
        return false;
    }
    healthbar = GameData ? GameData->Bitmap(const_cast<char *>("texthealthbar")) : nullptr;

    LayOut();
    if (!TPane::Initialize())
        return false;

    lines = {};
    numlines = 0;
    healthshown = false;
    level = targetlevel = 0;
    prompting = false;
    input.clear();
    RecomposeAll();
    return true;
}

// REVSYNC: 0x0054c3d0
void TTextBar::Close()
{
    if (!IsOpen())
        return;
    surface.reset();
    if (healthbartexture != kInvalidTexture && Renderer)
        Renderer->DestroyDynamicTexture(healthbartexture);
    healthbartexture = kInvalidTexture;
    barhue = -1;
    healthbar = nullptr;
    font = nullptr;
    lines = {};
    numlines = 0;
    healthshown = barshown = false;
    prompting = false;
    input.clear();
    TPane::Close();
}

// REVSYNC: 0x0054c9c0 (slot 13). An open prompt runs what was typed, then the
// loading bar goes and the newest line is an ordinary message again.
// Deviation: retail submits the typed text with the prompt closed too, and
// after a commit that text is still the last line, so every Hide ran the last
// cheat word again (TTextBar_SPEC §11.3).
void TTextBar::Hide()
{
    if (prompting)
    {
        prompting = false;
        SubmitInput(input);
    }
    healthshown = false;
    level = targetlevel = 0;
    lines[0].type = ELineType::Message;
    RecomposeFirst();
    TPane::Hide();
}

// REVSYNC: 0x0054c460 (slot 19), once per tick from the screen's pane pass:
// every line but the three newest ages, and the oldest goes once its life is
// out. (Its multiplayer half, which feeds incoming chat in, isn't ported.)
// The pane also follows the map view, as TPlayScreen's layout moves it
// (0x0047bc50).
void TTextBar::Pulse()
{
    LayOut();
    for (int32_t i = numlines - 1; i >= kPinnedLines; --i)
        if (--lines[i].ticks < 1 && i == numlines - 1)
            --numlines;
}

// REVSYNC: the rect. Initialize (0x0054bf70) anchors the bar's bottom to the
// map pane's and makes it nine lines tall; TPlayScreen's layout (0x0047bc50)
// keeps it on the bottom panel's top edge at x 0, as wide as the screen less
// the side pane and the side tabs. That is the port's map view less the tabs.
void TTextBar::LayOut()
{
    int32_t mapx = 0, mapy = 0, mapw = 0, maph = 0;
    PlayScreen.GetMapViewRect(mapx, mapy, mapw, maph);
    const int32_t height = kMaxLines * lineheight;
    Resize(mapx, mapy + maph - height, (std::max)(0, mapw - kSideTabsWidth), height);
}

void TTextBar::Print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VPrint(ELineType::Message, fmt, args);
    va_end(args);
}

void TTextBar::Print(ELineType type, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VPrint(type, fmt, args);
    va_end(args);
}

// REVSYNC: 0x0054d1b0. Nothing while the bar is closed; each '\n'-separated
// piece is a line. Retail appends each message to TextDump.txt when started
// with TEXTDUMP; the port logs every message, closed bar or not, as
// "[textbar] <text>". Deviation: after a '\n', retail adds the message's
// first piece again where the last piece belongs (it passes the buffer's
// start, 0x0054d2ba); the port adds the last piece.
void TTextBar::VPrint(ELineType type, const char *fmt, va_list args)
{
    char text[kPrintChars];
    vsnprintf(text, sizeof(text), fmt ? fmt : "", args);
    log_debug("[textbar] %s", ToUtf8(text).c_str());
    if (!IsOpen())
        return;

    char *line = text;
    for (char *eol = strchr(line, '\n'); eol; eol = strchr(line, '\n'))
    {
        *eol = '\0';
        AddLine(type, 0, line);
        line = eol + 1;
    }
    if (*line)
        AddLine(type, 0, line);
}

// REVSYNC: 0x0054d0c0. The new line goes first, second under an open
// prompt; the oldest falls off a full feed.
void TTextBar::AddLine(ELineType type, uint32_t color, const char *text)
{
    if (!text || text[0] == '\0' || text[0] == ' ')
        return;
    const int32_t slot = prompting ? 1 : 0;
    std::move_backward(lines.begin() + slot, lines.end() - 1, lines.end());
    numlines = (std::min)(numlines + 1, kMaxLines);
    SLine& line = lines[slot];
    line.type = type;
    line.color = color;
    line.ticks = kLineTicks;
    line.text.assign(text, strnlen(text, kTextChars));
    RecomposeAll();
}

// REVSYNC: CharPress @ 0x0054d4a0 (slot 28), a WM_CHAR code: Enter opens the
// prompt and commits it, Backspace deletes, 32..255 type. With control off or
// Locke in a fight (root COMBAT or BOW) any character commits an open prompt
// and does nothing else. Not ported: DBCS input (two-byte characters, and
// Backspace dropping two bytes after a high one).
void TTextBar::CharPress(int32_t key, bool down)
{
    const bool fighting = Player && (Player->IsRoot(ACTION_COMBAT) || Player->IsRoot(ACTION_BOW));
    if (!PlayScreen.IsControlOn() || fighting)
    {
        CommitInput();
        return;
    }
    if (!down)
        return;
    if (!prompting)
    {
        if (key == kCharEnter)
            BeginInput();
        return;
    }

    if (key == kCharEnter)
    {
        CommitInput();
        return;
    }
    if (key == kCharBackspace)
    {
        if (!input.empty())
            input.pop_back();
    }
    else if (key >= 0x20 && key < 0x100 && int32_t(input.size()) < inputroom - 1)
        input.push_back(char(key));
    ShowInput();
}

// REVSYNC: 0x0054d2f0 (inlined in CharPress). The held controls are let go
// and Locke stops in walk mode; "Message: " becomes the newest line.
void TTextBar::BeginInput()
{
    if (prompting)
        return;
    ControlMap.ReleaseAll();
    if (Player)
    {
        Player->Stop();
        Player->SetWalkMode();
    }
    const char *prefix = DialogList.GetLine("msgprefix");
    inputroom = kInputChars - int32_t(strlen(prefix));
    input.clear();
    AddLine(ELineType::Prompt, 0, prefix);
    prompting = true;
}

// REVSYNC: 0x0054d4a0's tail -- the prompt line shows the prefix and the text.
void TTextBar::ShowInput()
{
    lines[0].text = (std::string(DialogList.GetLine("msgprefix")) + input).substr(0, kTextChars);
    RecomposeFirst();
}

// REVSYNC: 0x0054d390. In single player the line takes the chat green (retail
// picks a multiplayer slot's colour from Player +0x494 / +0x4d8).
void TTextBar::CommitInput()
{
    if (!prompting)
        return;
    prompting = false;
    SLine& line = lines[0];
    line.type = ELineType::Chat;
    line.color = 0;
    line.text = (std::string(Player ? Player->GetName() : "") + ": " + input).substr(0, kTextChars);
    RecomposeFirst();
    SubmitInput(input);
}

// REVSYNC: 0x0054d700, single player. "@<line>" runs the line through the
// command interpreter with Locke as its context; anything else may be a
// cheat word (cheats.cpp), answered "Cheat Enabled" / "Cheat Disabled" with
// the potionmix sound. Multiplayer sent the text as chat.
void TTextBar::SubmitInput(const std::string& text)
{
    if (!text.empty() && text[0] == '@' && Player)
    {
        // The interpreter tokenizes in place and wants the line ended.
        std::string line = text.substr(1) + "\n";
        TStringParseStream stream(line.data(), int32_t(line.size()));
        TToken t(stream);
        t.WhiteGet();
        CommandInterpreter(Player, t, 1);           // commands abbreviate, as at the console
        return;
    }

    const ECheatResult result = ApplyCheatWord(text.c_str());
    if (result == ECheatResult::Unknown)
        return;
    log_info("[textbar] cheat '%s' %s", text.c_str(), result == ECheatResult::Enabled ? "on" : "off");
    const char *tag = result == ECheatResult::Enabled ? "cheatenabled" : "cheatdisabled";
    if (DialogList.FindLine(tag) >= 0)
        Print("%s", DialogList.GetLine(tag));
    const int32_t sound = SoundPlayer.FindSound("potionmix");
    if (sound >= 0 && SoundPlayer.Mount(sound))
        SoundPlayer.Play(sound);
}

void TTextBar::Clear()
{
    lines = {};
    numlines = 0;
    prompting = false;
    input.clear();
    healthshown = false;
    level = targetlevel = 0;
    RecomposeAll();
}

void TTextBar::SetHealthDisplay(const char *name)
{
    healthshown = true;
    AddLine(ELineType::Status, 0, name);
}

void TTextBar::SetLevels(int32_t newlevel, int32_t newtargetlevel)
{
    if (!healthshown)
    {
        healthshown = true;
        AddLine(ELineType::Status, 0, DialogList.GetLine("loadmsg"));
    }
    level = newlevel;
    targetlevel = newtargetlevel;
    RecomposeFirst();
}

void TTextBar::ClearHealthDisplay()
{
    healthshown = false;
    level = targetlevel = 0;
    lines[0].type = ELineType::Message;
    RecomposeFirst();
}

// Retail recomposes as it goes: Composite(-1) redraws every line without the
// bar (AddLine, a dirty pane), Composite(0) redraws the newest line, over the
// bar while it's up, stepping the bar's level 4 toward its target
// (0x0054cb00). The port records the outcome and renders it in Compose.
void TTextBar::RecomposeAll()
{
    barshown = false;
    ++composerequest;
}

void TTextBar::RecomposeFirst()
{
    if (healthshown)
    {
        if (std::abs(level - targetlevel) <= kLevelStep)
            level = targetlevel;
        else
            level += level < targetlevel ? kLevelStep : -kLevelStep;
    }
    barshown = healthshown;
    ++composerequest;
}

// REVSYNC: 0x0054c440 (slot 20): a dirty pane recomposes every line; then
// Composite 0x0054cd40 into the render target, outside the frame's passes.
void TTextBar::Compose()
{
    if (IsDirty())
    {
        RecomposeAll();
        SetDirty(false);
    }
    if (composed == composerequest || !font || !Renderer)
        return;

    const int32_t width = Display.Width();      // +0x70
    const int32_t height = kMaxLines * (lineheight + 2 * kSlotPad);
    if (!surface || surface->Width() != width || surface->Height() != height)
        surface = std::make_unique<TSurface>(width, height, SG_PIXELFORMAT_RGBA8);
    if (barshown)
        UpdateHealthBarTexture();

    surface->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    for (int32_t i = 0; i < numlines; ++i)
        ComposeLine(i);
    surface->EndPass();
    composed = composerequest;
}

// REVSYNC: 0x0054cd40, one line: the bar under the newest line while it's up,
// then the text in the line's colour with the black shadow.
void TTextBar::ComposeLine(int32_t index)
{
    const int32_t top = SlotTop(index);
    if (index == 0 && barshown)
        DrawHealthBar(top);
    const SLine& line = lines[index];
    const uint32_t color = LineColor(line);
    DrawTextShadowedAtBaseline(font, line.text.c_str(), float(kTextX), float(top + kTextBaseline),
                               Red(color), Green(color), Blue(color),
                               surface->Width(), surface->Height());
}

// REVSYNC: PutHue 0x004bd8c0 draws the strip through the hue-change blit
// (DM_CHANGEHUE, 0x004b21d0) in DM_TRANSPARENT. The port decodes the strip at
// the level's hue into a streamed texture when the hue changes.
void TTextBar::UpdateHealthBarTexture()
{
    if (!healthbar)
        return;
    const int32_t hue = BarHue(level);
    if (hue == barhue && healthbartexture != kInvalidTexture)
        return;
    if (healthbartexture == kInvalidTexture)
        healthbartexture = Renderer->CreateDynamicTexture(healthbar->width, healthbar->height,
                                                          ERendererTextureFilter::Nearest);
    std::vector<uint8_t> rgba(size_t(healthbar->width) * size_t(healthbar->height) * 4);
    if (healthbartexture == kInvalidTexture
        || !DecodeBitmapHueChangedToRGBA(healthbar, hue, rgba.data(), healthbar->width * 4))
    {
        log_warn("[textbar] texthealthbar: no texture at hue %d", hue);
        return;
    }
    Renderer->UpdateDynamicTexture(healthbartexture, rgba.data(), rgba.size());
    barhue = hue;
}

// REVSYNC: 0x0054cb00
void TTextBar::DrawHealthBar(int32_t top)
{
    if (!healthbar || healthbartexture == kInvalidTexture || barhue != BarHue(level))
        return;
    const int32_t w = healthbar->width;
    const int32_t h = healthbar->height;
    Renderer->Composite(healthbartexture, (std::min)(0, level - kBarFullLevel), top + kBarTop, w, h,
                        surface->Width(), surface->Height(), 0, 0, w, h, w, h);
}

int32_t TTextBar::SlotTop(int32_t index) const
{
    return index * (lineheight + 2 * kSlotPad) + kSlotPad;
}

// REVSYNC: 0x0054c600 -- alpha = ticks * 255 / 24 over a line's last 24
// ticks. The port eases it between ticks for the lines that age.
float TTextBar::LineAlpha(int32_t index, float frac) const
{
    float ticks = float(lines[index].ticks);
    if (index >= kPinnedLines)
        ticks -= frac;
    return std::clamp(ticks / float(kFadeTicks), 0.0f, 1.0f);
}

// REVSYNC: 0x0054c600 (slot 7; NoTexOverlay off -- slot 23, 0x0054c780, is the
// NoTexOverlay path, which drops a line once its alpha falls to half). The
// lines stack up from the pane's bottom edge, newest lowest, cut to the
// pane's width, each at its own alpha.
void TTextBar::Draw()
{
    if (!surface || composed == 0 || !Renderer)
        return;
    const float frac = float(TTime::LegacyFrameFraction());
    const int32_t width = (std::min)(GetWidth(), surface->Width());
    const int32_t bottom = GetPosY() + GetHeight();
    for (int32_t i = 0; i < numlines; ++i)
    {
        const float alpha = LineAlpha(i, frac);
        if (alpha <= 0.0f)
            continue;
        Renderer->DrawSurfaceSubrectTinted(surface.get(), GetPosX(), bottom - (i + 1) * lineheight,
                                           0, SlotTop(i), width, lineheight,
                                           1.0f, 1.0f, 1.0f, alpha);
    }
}
