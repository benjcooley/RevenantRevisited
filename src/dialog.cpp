// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.cpp - TDialogPane module                     *
// *************************************************************************

#include "revenant.h"
#include "dialog.h"
#include "display.h"
#include "multi.h"
#include "playscreen.h"
#include "character.h"
#include "command.h"
#include "bitmap.h"
#include "mappane.h"
#include "module.h"
#include "revutils.h"
#include "script.h"
#include "player.h"
#include "textbar.h"
#include "logging.h"
#include "font.h"
#include "fonttable.h"
#include "renderer.h"
#include "surface.h"
#include "textencoding.h"
#include "time.h"

#include <algorithm>
#include <cmath>
#include <string>

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

namespace {

// Lookups compare at most this many characters of the probe (0x0049d6d0).
constexpr size_t kMaxProbe = 39;

std::string UpperTag(const char *tag, size_t maxlen = std::string::npos)
{
    std::string up(tag ? tag : "");
    if (up.size() > maxlen)
        up.resize(maxlen);
    for (char &c : up)
        c = (char)toupper((unsigned char)c);
    return up;
}

}  // namespace

// Reads one dialog file: a #define header, then `TAG "line"` per line
// (retail Parse("%63t %4091s")), sorted by tag. Duplicate tags keep file
// order and the first one answers (retail's bsearch picks either).
bool TDialogList::LoadTable(const char *path, TTable &table)
{
    table.clear();
    FILE *fp = rev_fopen(path, "rb");
    if (!fp)
        return false;

    TFileParseStream s(fp, path);
    TToken t(s);
    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        char tag[64];
        char line[4092];
        if (!Parse(t, "%63t %4091s", tag, line))
            t.Error("tag \"line\" expected");
        if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Error("RETURN expected");
        table.push_back({UpperTag(tag), line});
        t.DefineGet();
    }
    fclose(fp);

    std::stable_sort(table.begin(), table.end(),
                     [](const SLine &a, const SLine &b) { return a.tag < b.tag; });
    return true;
}

int32_t TDialogList::Find(const TTable &table, const std::string &tag)
{
    const auto it = std::lower_bound(table.begin(), table.end(), tag,
                                     [](const SLine &l, const std::string &t) { return l.tag < t; });
    if (it == table.end() || it->tag != tag)
        return -1;
    return (int32_t)(it - table.begin());
}

// REVSYNC: 0x0049ceb0
bool TDialogList::Initialize()
{
    if (initialized)
        return true;
    const std::string path = std::string(ClassDefPath) + Language.CStr() + ".def";
    if (!LoadTable(path.c_str(), base))
    {
        log_error("[dialog] base dialog list '%s' not found", path.c_str());
        return false;
    }
    log_info("[dialog] base list %s: %d lines", path.c_str(), (int)base.size());
    initialized = true;
    return true;
}

// REVSYNC: 0x0049d2a0
bool TDialogList::LoadModule()
{
    misses.clear();
    const std::string language = Language.CStr();
    for (const std::string &file : {language + "_dialog.def", language + ".def",
                                    std::string("english.def")})
    {
        const std::string path = ModuleManager.ModuleFilePath(file.c_str());
        if (!path.empty() && rev_file_exists(path.c_str()) && LoadTable(path.c_str(), module))
        {
            log_info("[dialog] module list %s: %d lines", path.c_str(), (int)module.size());
            return true;
        }
    }
    module.clear();
    log_error("[dialog] the active module has no dialog list");
    return false;
}

void TDialogList::Close()
{
    base.clear();
    module.clear();
    misses.clear();
    initialized = false;
}

// REVSYNC: 0x0049d6d0
int32_t TDialogList::FindLine(const char *tag) const
{
    const std::string probe = UpperTag(tag, kMaxProbe);
    const int32_t inbase = Find(base, probe);
    if (inbase >= 0)
        return inbase;
    const int32_t inmodule = Find(module, probe);
    return inmodule >= 0 ? (int32_t)base.size() + inmodule : -1;
}

// REVSYNC: 0x0049d780
const char *TDialogList::GetLine(int32_t id) const
{
    if (id >= 0 && id < (int32_t)base.size())
        return base[id].line.c_str();
    id -= (int32_t)base.size();
    if (id >= 0 && id < (int32_t)module.size())
        return module[id].line.c_str();
    return "[badid]";
}

// REVSYNC: 0x0049d7c0
const char *TDialogList::GetTag(int32_t id) const
{
    if (id >= 0 && id < (int32_t)base.size())
        return base[id].tag.c_str();
    id -= (int32_t)base.size();
    if (id >= 0 && id < (int32_t)module.size())
        return module[id].tag.c_str();
    return "[badid]";
}

// REVSYNC: 0x0049d800
const char *TDialogList::GetLine(const char *tag) const
{
    const int32_t id = FindLine(tag);
    if (id >= 0)
        return GetLine(id);
    auto [it, added] = misses.try_emplace(tag ? tag : "");
    if (added)
        it->second = "[" + it->first + "]";
    return it->second.c_str();
}

// ************************************************************
// * TDialogPane - Shows the dialog options for the character *
// ************************************************************

TDialogPane DialogPane;

static TObjectInstance* DlgContext;
static bool savecontrolon;
static bool saveisfullscreen;

void SetDialogContext(TObjectInstance* context)
{
    DlgContext = context;
}

// Translates a dialog line into `outbuf` (at most buflen - 1 characters):
// [me] becomes the player's name, [chr] the name of the object whose command
// is running, and [[ / ]] are literal brackets.
// REVSYNC: 0x00533dd0 -- retail's lead-byte test is inverted, so its
// substitution never runs; no shipped line contains '[', so the text matches.
char *DialogLine(const char *line, char *outbuf, int32_t buflen)
{
    if (!outbuf || buflen <= 0)
        return outbuf;

    std::string out;
    for (const char *p = line ? line : ""; *p; )
    {
        if (*p != '[')
        {
            out += *p++;
            continue;
        }
        p++;
        if (*p == '[' || *p == ']')
        {
            out += *p++;
            continue;
        }
        std::string tag;
        while (*p && *p != ']')
            tag += *p++;
        if (*p == ']')
            p++;

        const TObjectInstance* named = nullptr;
        if (!stricmp(tag.c_str(), "me"))
            named = Player;
        else if (!stricmp(tag.c_str(), "chr"))
            named = DlgContext;
        if (named && named->GetName())
            out += named->GetName();
    }

    snprintf(outbuf, (size_t)buflen, "%s", out.c_str());
    return outbuf;
}

// ****************
// * TDialogEntry *
// ****************

namespace {

// Colours as retail stores them, 0x00RRGGBB: red is byte 2. The overlay
// quad's D3D diffuse is alpha << 24 | [2] << 16 | [1] << 8 | [0] (0x00534b60,
// vertex format 0x1c4) and GDI text swaps bytes 0 and 2 into its COLORREF
// (0x004be7ba), so both of retail's draw paths read byte 2 as red.
constexpr uint32_t kPlayerColor    = 0x3cafff;     // AddSpeech 0x00535b90, ShowResponses 0x00535e90
constexpr uint32_t kHighlightColor = 0xffffff;
constexpr uint32_t kNpcColors[4]   = {0xff0000, 0x00ff00, 0xffff00, 0x00ffff};    // by slot & 3

// The panes retail lays the entries out against (0x005351d0): the status bar
// on top (TPlyrStatusBar 0x0065a8c0: y 0, h 0x70 from its static init
// 0x00480663) and the side tabs at the map's right edge (0x0065be50, its 52 px
// strip). The map view is MapPane's rect (0x006668d8).
constexpr int32_t kStatusBarBottom = 0x70;
constexpr int32_t kSideTabsWidth   = 52;
constexpr int32_t kNpcTopGap       = 10;
constexpr int32_t kPlayerBottomGap = 50;
constexpr int32_t kEntryGap        = 5;

float Red(uint32_t c)   { return float((c >> 16) & 0xff) / 255.0f; }
float Green(uint32_t c) { return float((c >> 8) & 0xff) / 255.0f; }
float Blue(uint32_t c)  { return float(c & 0xff) / 255.0f; }

// A counter that steps by one per tick toward `target` (fades, highlights),
// `frac` of the way into its next step.
float StepToward(int32_t value, int32_t target, float frac)
{
    if (value < target)
        return float(value) + frac;
    if (value > target)
        return float(value) - frac;
    return float(value);
}

// One tick of a slide (0x005348f0): the 16.16 position's whole part, held at
// the target.
int32_t SlideStep(int32_t value, int32_t target, int32_t pos)
{
    const int32_t next = pos >> 16;
    return value < target ? (next < target ? next : target)
                          : (next > target ? next : target);
}

// The slide `frac` of the way into its next step.
float SlidePosition(int32_t value, int32_t target, int32_t pos, int32_t step, float frac)
{
    if (value == target)
        return float(value);
    const float next = (float(pos) + float(step) * frac) / 65536.0f;
    return value < target ? (next < float(target) ? next : float(target))
                          : (next > float(target) ? next : float(target));
}

}  // namespace

// REVSYNC: 0x00533f10. Each text wraps to 350 px in the rect x 50..399, 10 px
// under the one before; a box shorter than 44 px centres its texts. A
// response list gets one button per choice, named by its label.
TDialogEntry::TDialogEntry(TDialogPane& owner, TObjectInstance* speaker, EMode mode,
                           uint32_t color, uint32_t hicolor, std::vector<std::string> texts,
                           std::vector<std::string> labels, int32_t ticks)
  : pane(owner), speaker(speaker), mode(mode), color(color), hicolor(hicolor),
    ticksleft(ticks), texts(std::move(texts)), labels(std::move(labels))
{
    if (this->texts.size() > kMaxTexts)
        this->texts.resize(kMaxTexts);

    const int32_t lineheight = pane.LineHeight();
    int32_t top = 0;
    for (size_t i = 0; i < this->texts.size(); ++i)
    {
        std::vector<std::string>& wrapped = lines.emplace_back();
        const int32_t count = WrapTextLines(pane.Font(), this->texts[i].c_str(), kWrapWidth, wrapped);
        SRect& r = rects[i];
        r.left = kTextLeft;
        r.top = top;
        r.right = kWidth - 1;
        r.bottom = top - 1 + count * lineheight;
        height = r.bottom + 1;
        top = r.bottom + 1 + kTextGap;
    }
    if (height < kMinHeight)
    {
        const int32_t shift = (kMinHeight - height) / 2;
        height = kMinHeight;
        for (size_t i = 0; i < this->texts.size(); ++i)
        {
            rects[i].top += shift;
            rects[i].bottom += shift;
        }
    }

    if (mode == EMode::Responses)
        for (size_t i = 0; i < this->texts.size() && i < this->labels.size(); ++i)
            buttons[i] = pane.NewChoiceButton(this->labels[i].c_str());
}

TDialogEntry::~TDialogEntry()
{
    RemoveButtons();
}

void TDialogEntry::RemoveButtons()
{
    for (TButton*& button : buttons)
    {
        if (button)
            pane.DeleteButton(button);
        button = nullptr;
    }
}

TObjectInstance* TDialogEntry::Speaker() const
{
    return speaker.Get();
}

// REVSYNC: 0x005348f0. An entry isn't timed until the pane has placed it.
void TDialogEntry::Pulse()
{
    if (!IsPlaced())
        return;

    if (ticksleft >= 0)
    {
        if (ticksleft == 0)
            Dismiss();
        ticksleft--;
    }

    if (offx != targetx || offy != targety)
    {
        posx += stepx;
        offx = SlideStep(offx, targetx, posx);
        posy += stepy;
        offy = SlideStep(offy, targety, posy);
    }

    if (fade < fadetarget)
        fade++;
    else if (fade > fadetarget)
        fade--;

    for (size_t i = 0; i < texts.size(); ++i)
    {
        TButton* button = buttons[i];
        if (!button)
            continue;
        hightarget[i] = button->IsHover() ? kHighlightTicks : 0;
        if (highlight[i] < hightarget[i])
            highlight[i]++;
        else if (highlight[i] > hightarget[i])
            highlight[i]--;
        const SRect& r = rects[i];
        button->SetRect(basex + offx + r.left - pane.GetPosX(), basey + offy + r.top - pane.GetPosY(),
                        r.right - r.left + 1, r.bottom - r.top + 1);
    }
}

// REVSYNC: 0x00534a40
void TDialogEntry::Dismiss()
{
    dismissed = true;
    fadetarget = 0;
    RemoveButtons();
}

// REVSYNC: the layout step of 0x005351d0 for one entry. A new entry appears
// at its place; a placed one slides there in kSlideTicks equal 16.16 steps.
void TDialogEntry::MoveTo(int32_t offset)
{
    if (targetx == 0 && targety == offset)
        return;
    if (!IsPlaced())
    {
        offx = targetx = 0;
        offy = targety = offset;
        return;
    }
    targetx = 0;
    targety = offset;
    posx = offx * 65536;
    posy = offy * 65536;
    stepx = (targetx - offx) * 65536 / kSlideTicks;
    stepy = (targety - offy) * 65536 / kSlideTicks;
}

// The speaker's inventory image (0x0046f190). REVSYNC-DIVERGENCE: retail's
// imagery getter falls back to the state-0 icon (0x0040ce60), where a
// character's portrait is baked; the port's InventoryImage doesn't, because
// IsInventoryItem leans on it, so the fallback is taken here.
TBitmap* TDialogEntry::Portrait() const
{
    TObjectInstance* who = speaker.Get();
    if (!who)
        return nullptr;
    if (TBitmap* face = who->InventoryImage())
        return face;
    TObjectImagery* imagery = who->GetImagery();
    return imagery ? imagery->GetInvImage(0) : nullptr;
}

// REVSYNC: 0x00534470 (NoTexOverlay off): the portrait centred at (22, 22),
// the texts in white with the 3-pass black shadow (font flags 0x401), the
// "Ring" over the portrait. The text colour is applied when drawn.
void TDialogEntry::Compose()
{
    if (!Renderer || height <= 0)
        return;
    if (!surface || surface->Height() != height)
        surface = std::make_unique<TSurface>(kWidth, height, SG_PIXELFORMAT_RGBA8);

    surface->StartPass(0.0f, 0.0f, 0.0f, 0.0f);
    if (TBitmap* face = Portrait())
        Renderer->DrawBitmapToTarget(face, kPortraitCenter - face->width / 2,
                                     kPortraitCenter - face->height / 2, kWidth, height);
    const int32_t lineheight = pane.LineHeight();
    for (size_t i = 0; i < lines.size(); ++i)
        for (size_t j = 0; j < lines[i].size(); ++j)
            DrawTextShadowedToTarget(pane.Font(), lines[i][j].c_str(), rects[i].left,
                                     rects[i].top + int32_t(j) * lineheight, kWrapWidth, lineheight,
                                     ETextAlign::Left, 1.0f, 1.0f, 1.0f, kWidth, height);
    if (TBitmap* ring = pane.Ring())
        Renderer->DrawBitmapToTarget(ring, kPortraitCenter - ring->width / 2,
                                     kPortraitCenter - ring->height / 2, kWidth, height);
    surface->EndPass();
    composed = true;
}

// REVSYNC: 0x00534b60. Alpha = fade / 12. The portrait column draws in white,
// the texts in the entry's colour; a hovered choice's text draws again over
// it in the highlight colour at highlight / 8 of that alpha.
void TDialogEntry::Draw(float frac) const
{
    if (!IsPlaced() || !composed || !surface || !Renderer)
        return;
    const float alpha = StepToward(fade, fadetarget, frac) / float(kFadeTicks);
    if (alpha <= 0.0f)
        return;

    const int32_t x = basex + int32_t(std::lround(SlidePosition(offx, targetx, posx, stepx, frac)));
    const int32_t y = basey + int32_t(std::lround(SlidePosition(offy, targety, posy, stepy, frac)));
    Renderer->DrawSurfaceSubrectTinted(surface.get(), x, y, 0, 0, kTextLeft, height,
                                       1.0f, 1.0f, 1.0f, alpha);
    Renderer->DrawSurfaceSubrectTinted(surface.get(), x + kTextLeft, y, kTextLeft, 0,
                                       kWidth - kTextLeft, height,
                                       Red(color), Green(color), Blue(color), alpha);
    for (size_t i = 0; i < texts.size(); ++i)
    {
        if (!buttons[i])
            continue;
        const float lit = StepToward(highlight[i], hightarget[i], frac) / float(kHighlightTicks) * alpha;
        if (lit <= 0.0f)
            continue;
        const SRect& r = rects[i];
        Renderer->DrawSurfaceSubrectTinted(surface.get(), x + r.left, y + r.top, r.left, r.top,
                                           r.right - r.left + 1, r.bottom - r.top + 1,
                                           Red(hicolor), Green(hicolor), Blue(hicolor), lit);
    }
}

// ***************
// * TDialogPane *
// ***************

// REVSYNC: 0x00534fd0. The pane tracks the hovered choice (+0x60 |= 2). The
// entries use the "Dialog" font (0x0065c134, from FontTable at 0x00485e01)
// and the "Ring" of the status bar archive (0x0065a9d0, 0x0047a917).
bool TDialogPane::Initialize()
{
    if (IsOpen())
        return true;
    if (!TButtonPane::Initialize())
        return false;
    SetPaneFlags(BPF_HOVER);
    entries.clear();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    chosen = -1;
    committed = false;

    font = FontTable ? FontTable->Atlas("Dialog") : nullptr;
    const TGenericFont* fontdef = FontTable ? FontTable->FindFont("Dialog") : nullptr;
    lineheight = fontdef ? fontdef->height + fontdef->lextra : 0;
    if (!statusbardat)
        statusbardat = TMulti::LoadMulti(const_cast<char*>("statusbarnotex.dat"));
    ring = nullptr;
    for (int32_t i = 0; statusbardat && i < statusbardat->numoffsets; ++i)
    {
        const char* name = static_cast<const char*>(static_cast<void*>(statusbardat->names[i]));
        if (name && !stricmp(name, "Ring"))
            ring = statusbardat->Bitmap(i);
    }
    if (!font || !ring)
        log_warn("[dialog] font \"Dialog\" %s, \"Ring\" %s", font ? "ok" : "missing",
                 ring ? "ok" : "missing");
    return true;
}

// REVSYNC: 0x00535060
void TDialogPane::Close()
{
    if (!IsOpen())
        return;
    entries.clear();                        // their buttons leave the pane first
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    ring = nullptr;
    delete statusbardat;
    statusbardat = nullptr;
    font = nullptr;
    TButtonPane::Close();
}

// REVSYNC: 0x00535120 -- hidden, ignoring input, entries and choices gone.
// (Retail forgot the entries without freeing them.)
void TDialogPane::Hide()
{
    TButtonPane::Hide();
    entries.clear();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
}

// REVSYNC: 0x005351d0, once per simulation tick: age the entries, lay them
// out, drop the faded, and commit a picked response -- after the scripts
// have pulsed, so the waiting script has seen it (TScript::WaitSatisfied).
// While the player has no control (a response wait) the hovered choice stays
// when the pointer leaves it, and the arrows and Enter work the choices
// (+0x60 bits 4 and 8).
void TDialogPane::Pulse()
{
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        entry->Pulse();

    LayOut();
    DeleteGoneEntries();

    constexpr uint32_t kChoosing = BPF_KEEPHOVER | BPF_KEYFOCUS;
    SetPaneFlags(PlayScreen.IsControlOn() ? PaneFlags() & ~kChoosing : PaneFlags() | kChoosing);

    if (committed && chosen >= 0)
    {
        log_info("[dialog] choice %d committed (label '%s')", chosen + 1, choicelabels[chosen].c_str());
        committed = false;
        PlayScreen.SetControlOn(savedcontrol);
        if (responses)
            responses->Dismiss();
        responses = nullptr;
        answering.Clear();
        controlonwhilechoosing = false;
    }
}

// REVSYNC: the layout of 0x005351d0. The entries are 400 wide, centred in
// the map's width less the side tabs. NPC lines stack down from 10 px under
// the status bar; the player's lines and the response list stack up to 50 px
// above the map's bottom, oldest on top; 5 px apart, in creation order. A
// dismissed entry keeps its place while it fades but holds no slot. The pane
// covers the map view, so the choice buttons hit-test there.
void TDialogPane::LayOut()
{
    int32_t mapx = 0, mapy = 0, mapw = 0, maph = 0;
    PlayScreen.GetMapViewRect(mapx, mapy, mapw, maph);
    if (GetPosX() != mapx || GetPosY() != mapy || GetWidth() != mapw || GetHeight() != maph)
        Resize(mapx, mapy, mapw, maph);

    const int32_t x = mapx + (mapw - kSideTabsWidth - TDialogEntry::kWidth) / 2;
    const int32_t top = kStatusBarBottom + kNpcTopGap;
    const int32_t bottom = mapy + maph - kPlayerBottomGap;

    int32_t lowerheight = 0;
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        if (!entry->IsDismissed() && entry->Mode() != TDialogEntry::EMode::NpcSpeech)
            lowerheight += (lowerheight ? kEntryGap : 0) + entry->Height();

    int32_t upper = 0;
    int32_t lower = -lowerheight;
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
    {
        const bool npc = entry->Mode() == TDialogEntry::EMode::NpcSpeech;
        entry->SetBase(x, npc ? top : bottom);
        if (entry->IsDismissed())
            continue;
        int32_t& offset = npc ? upper : lower;
        entry->MoveTo(offset);
        offset += entry->Height() + kEntryGap;
    }
}

void TDialogPane::DeleteGoneEntries()
{
    const auto gone = std::remove_if(entries.begin(), entries.end(),
                                     [this](const std::unique_ptr<TDialogEntry>& entry) {
        if (!entry->IsGone())
            return false;
        if (entry.get() == responses)
            responses = nullptr;
        return true;
    });
    entries.erase(gone, entries.end());
}

// REVSYNC: 0x00535500 -- render the new entries' surfaces.
void TDialogPane::Compose()
{
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        if (entry->IsPlaced() && !entry->IsComposed())
            entry->Compose();
}

// REVSYNC: 0x00535550 -- the entries over the map in creation order, between
// this tick's state and the next.
void TDialogPane::Draw()
{
    const float frac = float(TTime::LegacyFrameFraction());
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        entry->Draw(frac);
}

// REVSYNC: 0x00535610 (key down). Space silences the spoken lines; 1-6 pick
// a choice while the responses are up. Other keys, Space included, go on to
// the button pane (0x004361f0): the arrows and Enter while choosing.
void TDialogPane::KeyPress(int32_t key, bool down)
{
    if (down)
    {
        if (key == ' ')
            SkipSpeech();
        else if (key >= '1' && key <= '6')
        {
            const int32_t index = key - '1';
            if (responses && index < (int32_t)choicelabels.size())
            {
                Choose(index);
                return;
            }
        }
    }
    TButtonPane::KeyPress(key, down);
}

// REVSYNC: 0x00535760 -- the joystick's skip button (0x40a, "JOY2" in
// retail's key table) silences the spoken lines.
void TDialogPane::Joystick(int32_t key, bool down)
{
    if (down && key == VK_JOYBUTTON3)
        SkipSpeech();
}

// REVSYNC: 0x005362b0 -- a clicked choice button: its name is the choice's
// label (every label is compared; the last match wins).
void TDialogPane::OnControl(TButton *button, int32_t msg)
{
    if (msg != CONTROL_CLICKED || !button)
        return;
    for (size_t i = 0; i < choicelabels.size(); ++i)
        if (!stricmp(button->GetName(), choicelabels[i].c_str()))
        {
            log_info("[dialog] choice %d clicked (label '%s')", int(i) + 1, button->GetName());
            Choose(int32_t(i));
        }
}

// A response entry's choice button: named by the label, invisible, hoverable
// (retail ctor 0x0042c600, flags 0x100010). The entry places it over its text.
TButton* TDialogPane::NewChoiceButton(const char *label)
{
    auto* button = new TButton(label, 0, 0, 0, 0, 0, nullptr);
    button->SetHoverable(true);
    if (NewButton(button))
        return button;
    delete button;
    return nullptr;
}

void TDialogPane::Choose(int32_t index)
{
    chosen = index;
    committed = true;
}

// REVSYNC: 0x00535870
void TDialogPane::AddChoice(const char *label, const char *text)
{
    if (!IsOpen() && !responses)
        return;
    if (chosen >= 0)                        // answered: a new list begins
        ClearResponses();
    if ((int32_t)choicelabels.size() >= kMaxChoices)
        return;
    choicetexts.emplace_back(text ? text : "");
    choicelabels.emplace_back(label ? label : "");
}

// REVSYNC: 0x00535e90. The choices show as their dialog lines in quotes, in
// the player's colour.
bool TDialogPane::ShowResponses(TObjectInstance* player, bool controlon)
{
    if (choicelabels.empty() || !player)
        return false;

    answering = player;
    controlonwhilechoosing = controlon;
    savedcontrol = PlayScreen.IsControlOn();
    PlayScreen.SetControlOn(controlonwhilechoosing);

    std::vector<std::string> lines;
    for (const std::string& tag : choicetexts)
        lines.push_back("\"" + std::string(DialogList.GetLine(tag.c_str())) + "\"");
    entries.push_back(std::make_unique<TDialogEntry>(*this, player, TDialogEntry::EMode::Responses,
                                                     kPlayerColor, kHighlightColor, std::move(lines),
                                                     choicelabels, TDialogEntry::kNoTimeout));
    responses = entries.back().get();
    log_info("[dialog] %d choice(s) shown", (int)choicelabels.size());
    return true;
}

// REVSYNC: 0x00535b90. The player speaks in the player's colour; an NPC in
// its speaker slot's.
void TDialogPane::AddSpeech(TObjectInstance* speaker, const char *text, int32_t ticks)
{
    if (!speaker || !text || ticks <= 0)
        return;
    const bool player = speaker->ObjClass() == OBJCLASS_PLAYER;
    const TDialogEntry::EMode mode = player ? TDialogEntry::EMode::PlayerSpeech
                                            : TDialogEntry::EMode::NpcSpeech;
    const uint32_t color = player ? kPlayerColor : NpcColor(speaker);
    log_debug("[dialog] %s says: %s", speaker->GetName() ? speaker->GetName() : "?", ToUtf8(text).c_str());
    entries.push_back(std::make_unique<TDialogEntry>(*this, speaker, mode, color, kHighlightColor,
                                                     std::vector<std::string>{text},
                                                     std::vector<std::string>{}, ticks));
}

// REVSYNC: the speaker slots of 0x00535b90 -- a 16-entry round robin
// (0x0066f6f8, counter 0x0066f73c); a new speaker takes the next slot, the
// first one slot 1. Retail keys the slots by object pointer; the port by map
// index.
uint32_t TDialogPane::NpcColor(const TObjectInstance* speaker)
{
    const int32_t id = speaker->GetMapIndex();
    int32_t slot = -1;
    for (int32_t i = 0; i < kSpeakerSlots && slot < 0; ++i)
        if (speakerslots[i] == id)
            slot = i;
    if (slot < 0)
    {
        lastslot = (lastslot + 1) & (kSpeakerSlots - 1);
        speakerslots[lastslot] = id;
        slot = lastslot;
    }
    return kNpcColors[slot & 3];
}

// REVSYNC: 0x00536010
void TDialogPane::SkipSpeech()
{
    if (!IsOpen() || responses)
        return;
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
    {
        if (TObjectInstance* speaker = entry->Speaker(); speaker && speaker->IsCharacter())
            static_cast<TCharacter*>(speaker)->StopTalking();
        entry->Dismiss();
    }
}

// REVSYNC: 0x00535a10 -- control is not given back here.
void TDialogPane::ClearResponses()
{
    if (responses)
        responses->Dismiss();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    chosen = -1;
    committed = false;
}

// REVSYNC: 0x00535d80
void TDialogPane::ClearSpeech(bool now)
{
    if (!IsOpen())
        return;
    SkipSpeech();
    ClearResponses();
    if (now)
        entries.clear();
    else
        for (const std::unique_ptr<TDialogEntry>& entry : entries)
            entry->Dismiss();
}

// REVSYNC: 0x005360f0 (its tail -- give control back, jump the answering
// player's script to "Finish", Hide -- is unreachable in retail).
void TDialogPane::ResetForLoad()
{
    if (responses)
        responses->Dismiss();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    entries.clear();
}

const char *TDialogPane::CommittedLabel() const
{
    if (!committed || chosen < 0 || chosen >= (int32_t)choicelabels.size())
        return nullptr;
    return choicelabels[chosen].c_str();
}

const char *TDialogPane::ChosenText() const
{
    if (chosen < 0 || chosen >= (int32_t)choicetexts.size())
        return nullptr;
    return choicetexts[chosen].c_str();
}

// REVSYNC: choice @ 0x004282f0 -- choice <label> <text-part>... The text is
// in practice the dialog tag of the line the choice shows. From a script the
// choice goes through the script (TScript::AddChoice); from the console,
// straight to the pane.
COMMAND(CmdChoice)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;
    const std::string label = t.Text();

    t.WhiteGet();
    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    std::string text;
    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        if (t.Type() == TKN_TEXT)
            text += t.Text();
        else if (t.Type() == TKN_IDENT)
        {
            // A prototype variable of the context: retail checked the type
            // on the context but read the value with no object, so a number
            // came out as its not-found value and text dereferenced null.
            // DEVIATION: the port appends nothing (no shipped choice names a
            // variable). Any other identifier replaces the text with its
            // DialogLine form -- the identifier itself.
            if (ScriptManager.VariableType(t.Text(), context) < 0)
            {
                char buf[256];
                text = DialogLine(t.Text(), buf, sizeof(buf));
            }
        }
        t.WhiteGet();                       // numbers and symbols are skipped
    }

    if (script)
        script->AddChoice(label.c_str(), text.c_str());
    else
        DialogPane.AddChoice(label.c_str(), text.c_str());
    return 0;
}

// REVSYNC: message @ 0x004280e0 -- message ("<text>" | <TAG>): quoted text,
// or the tag's dialog line, on the text bar. (Retail passes the text as the
// format string; no shipped line contains '%'.)
COMMAND(CmdMessage)
{
    const char *text = nullptr;
    if (t.Type() == TKN_TEXT)
        text = t.Text();
    else if (t.Type() == TKN_IDENT)
        text = DialogList.GetLine(t.Text());
    else
    {
        Output("There was no message to display!\n");
        return CMD_BADCOMMAND;
    }
    TextBar.Print(const_cast<char *>("%s"), text);
    t.WhiteGet();
    return 0;
}

// A busy line: quoted text, or a tag that must exist (its line and voice).
static bool BusyLine(TToken &t, const char *&text, const char *&voice)
{
    voice = nullptr;
    if (t.Type() == TKN_TEXT)
        text = t.Text();
    else if (t.Type() == TKN_IDENT)
    {
        const int32_t id = DialogList.FindLine(t.Text());
        if (id < 0)
            return false;
        text = DialogList.GetLine(id);
        voice = DialogList.GetTag(id);
    }
    else
        return false;
    return true;
}

// REVSYNC: busysay @ 0x004298b0 -- busysay ("<text>" | <TAG>), from a
// script only. DEVIATION: retail also ANDs the script's taken flags with
// 0x10000 (probably meant |=), which would drop them; not copied.
COMMAND(CmdBusySay)
{
    const char *text = nullptr, *voice = nullptr;
    if (!script || !BusyLine(t, text, voice))
        return CMD_BADPARAMS;
    script->SetBusySay(text, voice);
    t.WhiteGet();
    return 0;
}

// REVSYNC: busymsg @ 0x00429850 -- as busysay, the text only (same
// DEVIATION for the taken flags, 0x20000 there).
COMMAND(CmdBusyMsg)
{
    const char *text = nullptr, *voice = nullptr;
    if (!script || !BusyLine(t, text, voice))
        return CMD_BADPARAMS;
    script->SetBusyMessage(text);
    t.WhiteGet();
    return 0;
}
