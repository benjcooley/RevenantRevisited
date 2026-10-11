// ************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.cpp - FORSAKEN Stat Pane                   *
// *************************************************************************
//
// See statpane.h. Every constant is retail's, cited by the instruction that
// holds it in docs/ui/forensics/StatPane_RETAIL.md. The 1998 pane (a skill
// tree in the gold font) is in attic/src/statpane.*.
//
// *************************************************************************

#include "statpane.h"

#include "dialog.h"
#include "font.h"
#include "fonttable.h"
#include "logging.h"
#include "multi.h"
#include "object.h"
#include "parse.h"
#include "player.h"
#include "renderer.h"
#include "revenant.h"       // ResourcePath
#include "revutils.h"
#include "spellpane.h"      // AcquireSpellIcons
#include "textbar.h"
#include "uidragstate.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace {

constexpr int32_t kStart = 0x14;            // the cursor's first place and line start (0x547601)
constexpr int32_t kLineHeight = 0xc;
constexpr int32_t kFieldChars = 0x50;       // the object's field text (0x547576)
constexpr int32_t kMaxColours = 8;          // named colours in a block (0x547d88)
constexpr size_t kColourNameChars = 9;
constexpr int32_t kMaxDescPage = 2;         // a multipage field's pages, 0..2 (0x548816)
constexpr int32_t kTrainMax = 30;           // an attribute over this takes no more training (0x54962b)
constexpr int32_t kSecondPickLevel = 15;    // the second pick shows below this level (0x5472c6)
constexpr const char *kInvalidLayout = "Invalid Stat Layout";

// The attributes, as train<name> and stmod<name> spell them (0x0051a280):
// the player stats PLRSTAT_FIRST + 0..5.
constexpr const char *kAttributes[NUM_PLRSTATS] = { "strn", "cons", "agil", "rflx", "mind", "luck" };

int32_t AttributeIndex(const char *name, size_t length)
{
    for (int32_t i = 0; i < NUM_PLRSTATS; ++i)
        if (strlen(kAttributes[i]) == length && strnicmp(name, kAttributes[i], length) == 0)
            return i;
    return -1;
}

// REVSYNC: 0x0051a320 (its flag 0) -- the modifier an attribute's value
// gives: 0..2 -4, 3..6 -3, 7..9 -2, 10..11 -1, 12..15 0, 16..17 1,
// 18..20 2, 21..24 3, 25..29 4, above that 5.
int32_t AttributeModifier(int32_t value)
{
    static constexpr int8_t kModifiers[30] = { -4, -4, -4, -3, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0,
                                               0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4 };
    return value < 0 ? kModifiers[0] : value < 30 ? kModifiers[value] : 5;
}

// The weapon classes `type` names (0x0054758c).
constexpr const char *kWeaponTypes[] = { "STATCFGHANDS", "STATCFGKNIFE", "STATCFGSWORD", "STATCFGBLUDGEON",
                                         "STATCFGAXES", "STATCFGBLUDGEON", "STATCFGBOWS", "STATCFGBOWS" };

}  // namespace

// ---- the layout ------------------------------------------------------------

// One statement of a block (the interpreter's dispatch, 0x005479a4).
struct TStatPane::SOp
{
    enum class EKind : uint8_t
    {
        Button, Icon, LineHeight, LineWrap, Colour, Pos, Move, Tab, NextLine, Text, Field,
        Invalid,        // what retail stops at, writing kInvalidLayout
    };
    enum class EModifier : uint8_t { None, Block, Multipage, Grayable, PrevButton, NextButton, Center, Right, Left };

    EKind kind = EKind::Invalid;
    std::string name;           // BUTTON / COLOR name; TEXT text; FIELD field
    std::string prefix, suffix; // FIELD
    std::string colour;         // TEXT / FIELD: the named colour given, if any
    int32_t v[4] = {};          // BUTTON x y w h; POS / MOVE / TAB / LINEHEIGHT / LINEWRAP; block w h
    bool hasrgb = false;        // COLOR: with r g b
    SColor3 rgb;
    EModifier modifier = EModifier::None;
};

// A CLASS block: the object classes it is for (a comma list, or
// "default"), the format it is for (none: any), its statements.
struct TStatPane::SBlock
{
    std::string classes;
    std::string format;
    bool anyformat = true;
    std::vector<SOp> ops;
};

// The DEF's text through retail's tokenizer, once: blocks of statements.
// A block whose statements don't parse ends in an Invalid statement, as
// retail's walk stops there; text that isn't a block ends the list, and an
// object no block takes shows kInvalidLayout, as in retail.
bool TStatPane::LoadLayout()
{
    blocks.clear();
    layoutvalid = false;
    std::vector<uint8_t> bytes;
    const std::string path = std::string(ResourcePath) + "statpane.def";
    if (!rev_read_file(path.c_str(), bytes) || bytes.empty())     // packs first (0x00546c7e)
    {
        log_error("[statpane] Unable to load STATPANE.DEF (%s)", path.c_str());
        return false;
    }
    if (bytes[0] & 0x80)                    // an obfuscated DEF (0x004834e0)
        for (uint8_t &b : bytes)
            b ^= 0xcc;
    bytes.push_back(0);

    using EKind = SOp::EKind;
    using EModifier = SOp::EModifier;
    TStringParseStream stream(reinterpret_cast<char *>(bytes.data()), static_cast<int32_t>(bytes.size() - 1));
    TToken t(stream);
    t.DefineGet();
    while (t.Type() != TKN_EOF)
    {
        if (!t.Is("CLASS"))
            break;
        t.WhiteGet();
        if (t.Type() != TKN_TEXT)
            break;
        SBlock &block = blocks.emplace_back();
        block.classes = t.Text();
        t.WhiteGet();
        if (t.Is("FORMAT"))
        {
            t.WhiteGet();
            block.format = t.Text();
            block.anyformat = false;
            t.WhiteGet();
        }
        t.LineGet();
        if (!t.Is("BEGIN"))
        {
            block.ops.push_back({});        // Invalid
            break;
        }
        t.LineGet();
        std::vector<std::string> palette;   // the colour names declared so far
        auto named = [&palette](const char *name) {
            return std::any_of(palette.begin(), palette.end(),
                               [name](const std::string &n) { return stricmp(n.c_str(), name) == 0; });
        };
        bool ok = true;
        while (ok && !t.Is("END") && t.Type() != TKN_EOF)
        {
            SOp op;
            if (t.Is("BUTTON"))
            {
                char name[21] = {};
                t.WhiteGet();
                op.kind = EKind::Button;
                ok = Parse(t, "%20s %d %d %d %d\n", name, &op.v[0], &op.v[1], &op.v[2], &op.v[3]);
                op.name = name;
            }
            else if (t.Is("ICON"))
            {
                op.kind = EKind::Icon;
                t.WhiteGet();
                t.LineGet();
            }
            else if (t.Is("LINEHEIGHT") || t.Is("TAB"))
            {
                op.kind = t.Is("TAB") ? EKind::Tab : EKind::LineHeight;
                t.WhiteGet();
                ok = Parse(t, "%d\n", &op.v[0]);
            }
            else if (t.Is("LINEWRAP"))
            {
                op.kind = EKind::LineWrap;
                t.WhiteGet();
                if (t.Is("none"))
                {
                    op.v[0] = -1;
                    t.WhiteGet();
                    t.LineGet();
                }
                else
                    ok = Parse(t, "%d\n", &op.v[0]);
            }
            else if (t.Is("COLOR"))
            {
                op.kind = EKind::Colour;
                t.WhiteGet();
                if (t.Type() == TKN_IDENT)
                {
                    op.name = std::string(t.Text()).substr(0, kColourNameChars);
                    if (!named(op.name.c_str()))
                        palette.push_back(op.name);
                    t.WhiteGet();
                }
                if (t.Type() == TKN_NUMBER)
                {
                    op.hasrgb = true;
                    ok = Parse(t, "%b %b %b\n", &op.rgb.r, &op.rgb.g, &op.rgb.b);
                }
                else
                    t.LineGet();
            }
            else if (t.Is("POS") || t.Is("MOVE"))
            {
                op.kind = t.Is("POS") ? EKind::Pos : EKind::Move;
                t.WhiteGet();
                ok = Parse(t, "%d %d\n", &op.v[0], &op.v[1]);
            }
            else if (t.Is("NEXTLINE"))
            {
                op.kind = EKind::NextLine;
                t.WhiteGet();
                t.LineGet();
            }
            else if (t.Is("TEXT") || t.Is("FIELD"))
            {
                op.kind = t.Is("TEXT") ? EKind::Text : EKind::Field;
                t.WhiteGet();
                // At most one modifier (0x548031 .. 0x5481f4).
                static constexpr std::pair<const char *, EModifier> kModifiers[] = {
                    { "block", EModifier::Block }, { "multipage", EModifier::Multipage },
                    { "grayable", EModifier::Grayable }, { "prevbutton", EModifier::PrevButton },
                    { "nextbutton", EModifier::NextButton }, { "center", EModifier::Center },
                    { "right", EModifier::Right }, { "left", EModifier::Left },
                };
                for (const auto &[word, modifier] : kModifiers)
                    if (t.Type() == TKN_IDENT && t.Is(word))
                    {
                        op.modifier = modifier;
                        t.WhiteGet();
                        if (modifier == EModifier::Block || modifier == EModifier::Multipage)
                            ok = Parse(t, "%d %d", &op.v[0], &op.v[1]);
                        break;
                    }
                if (ok && t.Type() == TKN_IDENT && named(t.Text()))
                {
                    op.colour = t.Text();
                    t.WhiteGet();
                }
                if (ok && op.kind == EKind::Text)
                {
                    op.name = t.Text();
                    t.WhiteGet();
                }
                else if (ok)
                {
                    if (t.Type() == TKN_TEXT)
                    {
                        op.prefix = t.Text();
                        t.WhiteGet();
                    }
                    char field[31] = {};
                    ok = Parse(t, "%30t", field);
                    op.name = field;
                    if (ok && t.Type() == TKN_TEXT)
                    {
                        op.suffix = t.Text();
                        t.WhiteGet();
                    }
                }
                t.LineGet();
            }
            else
                ok = false;
            if (ok)
                block.ops.push_back(std::move(op));
        }
        if (!ok || !t.Is("END"))
        {
            block.ops.push_back({});        // Invalid
            t.SkipBlock();
        }
        t.LineGet();
    }
    layoutvalid = !blocks.empty();
    return layoutvalid;
}

// The first block for the object's class and the format (0x005477a9):
// "default" is any class; a block without a FORMAT is any format.
const TStatPane::SBlock *TStatPane::SelectBlock(TObjectInstance &shown) const
{
    const char *classname = shown.GetClassName();
    for (const SBlock &block : blocks)
    {
        bool forclass = stricmp(block.classes.c_str(), "default") == 0;
        for (size_t start = 0; !forclass && start <= block.classes.size();)
        {
            const size_t comma = (std::min)(block.classes.find(',', start), block.classes.size());
            forclass = strlen(classname) == comma - start
                    && strnicmp(block.classes.c_str() + start, classname, comma - start) == 0;
            start = comma + 1;
        }
        if (forclass && (block.anyformat || stricmp(block.format.c_str(), format.c_str()) == 0))
            return &block;
    }
    return nullptr;
}

// ---- the walk --------------------------------------------------------------

// What a walk lays out: text as WriteText (0x00546de0) and WriteField
// (0x00546f40) draw it, and the portrait.
struct TStatPane::SRun
{
    enum class EKind : uint8_t { Text, Block, Icon };
    EKind kind = EKind::Text;
    std::string text;
    int32_t x = 0, y = 0;       // Text: the cell's left and top; Block: the rect's; Icon: the point
    int32_t w = 0, h = 0;       // Text: the text's width; Block: the rect's size
    SColor3 colour;
};

struct TStatPane::SWalk
{
    std::vector<SRun> runs;
};

int32_t TStatPane::TextWidthOf(const std::string &text) const
{
    return static_cast<int32_t>(TextWidth(font, text.c_str()));
}

// The height DrawTextA (DT_WORDBREAK | DT_EDITCONTROL) gives `text` in a w x h
// rect: whole lines, the first always.
int32_t TStatPane::BlockHeight(const std::string &text, int32_t w, int32_t h) const
{
    std::vector<std::string> lines;
    const int32_t count = WrapTextLines(font, text.c_str(), static_cast<float>(w), lines);
    const int32_t step = static_cast<int32_t>(font->lineStep);
    int32_t shown = 1;
    while (shown < count && (shown + 1) * step <= h)
        ++shown;
    return shown * step;
}

namespace {

// How much of `text` the first lines of `lines` (its wrapped lines) cover,
// with the break after them: DrawTextExA's uiLengthDrawn.
size_t CharsDrawn(const std::string &text, const std::vector<std::string> &lines, size_t count)
{
    size_t at = 0;
    for (size_t i = 0; i < count && i < lines.size(); ++i)
        if (!lines[i].empty())
            at = text.find(lines[i], at) + lines[i].size();
    while (at < text.size() && (text[at] == ' ' || text[at] == '\r' || text[at] == '\n'))
        ++at;
    return (std::min)(at, text.size());
}

}  // namespace

// REVSYNC: 0x005475e0 -- the block's statements: the cursor (x, y) from
// (20, 20), the line's start, the line height and wrap, the colours.
void TStatPane::Walk(const SBlock &block, TObjectInstance &shown, SWalk &walk)
{
    using EKind = SOp::EKind;
    using EModifier = SOp::EModifier;
    struct SNamed { std::string name; SColor3 rgb; };
    int32_t x = kStart, y = kStart, linestart = kStart, lineheight = kLineHeight, linewrap = -1;
    SColor3 current;
    std::vector<SNamed> palette;
    EAlign align = EAlign::Fixed;
    auto find = [&palette](const std::string &name) -> SNamed * {
        for (SNamed &n : palette)
            if (stricmp(n.name.c_str(), name.c_str()) == 0)
                return &n;
        return nullptr;
    };
    // WriteText: the text at the cursor as the alignment places it; the
    // cursor moves to where GDI leaves its current position.
    auto writetext = [&](const std::string &text, const SColor3 &colour) {
        const int32_t w = TextWidthOf(text);
        const int32_t left = align == EAlign::Right ? x - w : align == EAlign::Center ? x - w / 2 : x;
        if (!text.empty())
            walk.runs.push_back({ SRun::EKind::Text, text, left, y, w, 0, colour });
        x = align == EAlign::Left ? x + w : align == EAlign::Right ? x - w : x;
    };
    // WriteField: wrapped in w x h, the cursor moved down by its height.
    auto writefield = [&](const std::string &text, int32_t w, int32_t h) {
        walk.runs.push_back({ SRun::EKind::Block, text, x, y, w, h, current });
        y += BlockHeight(text, w, h);
        align = EAlign::Fixed;
    };

    for (const SOp &op : block.ops)
    {
        switch (op.kind)
        {
          case EKind::Invalid:
            align = EAlign::Left;
            x = y = kStart;
            writetext(kInvalidLayout, SColor3{});
            return;
          case EKind::Button:
            break;      // SyncButtons makes the hot spots
          case EKind::Icon:
            walk.runs.push_back({ SRun::EKind::Icon, {}, x, y });
            break;
          case EKind::LineHeight:
            lineheight = op.v[0];
            break;
          case EKind::LineWrap:
            linewrap = op.v[0];
            break;
          case EKind::Colour:
          {
            SColor3 *target = &current;
            SNamed *entry = nullptr;
            if (!op.name.empty())
            {
                entry = find(op.name);
                if (!entry && static_cast<int32_t>(palette.size()) < kMaxColours)
                    entry = &palette.emplace_back(SNamed{ op.name, SColor3{ 0, 0, 0 } });
                if (entry)
                    target = &entry->rgb;
            }
            if (op.hasrgb)
                *target = op.rgb;
            else if (entry)
                current = entry->rgb;
            break;
          }
          case EKind::Pos:
            x = linestart = op.v[0];
            y = op.v[1];
            break;
          case EKind::Move:
            x = linestart = x + op.v[0];
            y += op.v[1];
            break;
          case EKind::Tab:
            x = linestart + op.v[0];
            break;
          case EKind::NextLine:
            y += lineheight;
            x = linestart;
            break;
          case EKind::Text:
          case EKind::Field:
          {
            bool wrapgate = true;
            switch (op.modifier)
            {
              case EModifier::Center: align = EAlign::Center; wrapgate = false; break;
              case EModifier::Right: align = EAlign::Right; wrapgate = false; break;
              case EModifier::Left:
              case EModifier::None: align = EAlign::Left; break;
              default: break;     // block, multipage, grayable: the alignment as it is
            }
            SColor3 colour = current;
            if (const SNamed *named = op.colour.empty() ? nullptr : find(op.colour))
                colour = named->rgb;
            std::string text;
            bool resolved = true;
            if (op.kind == EKind::Field)
            {
                std::string value, colourname;
                resolved = Resolve(shown, op.name.c_str(), value, colourname);
                text = op.prefix + (resolved ? value : std::string()) + op.suffix;
                if (const SNamed *named = resolved && !colourname.empty() ? find(colourname) : nullptr)
                    colour = named->rgb;
            }
            else
                text = op.name;
            if (linewrap > 0 && wrapgate && x + TextWidthOf(text) > linewrap)
            {
                y += lineheight;
                x = linestart;
            }
            if (!resolved)
                break;
            if (op.modifier == EModifier::Multipage)
            {
                // Up to three pages of what fits the rect (0x00548686).
                std::vector<std::string> lines;
                const int32_t step = static_cast<int32_t>(font->lineStep);
                const int32_t fit = (std::max)(1, op.v[1] / step);
                WrapTextLines(font, text.c_str(), static_cast<float>(op.v[0]), lines);
                const size_t drawn0 = CharsDrawn(text, lines, static_cast<size_t>(fit));
                std::string pages[3] = { text, {}, {} };
                desclast = 0;
                if (drawn0 < text.size())
                {
                    desclast = 1;
                    pages[1] = text.substr(drawn0);
                    lines.clear();
                    WrapTextLines(font, pages[1].c_str(), static_cast<float>(op.v[0]), lines);
                    const size_t drawn1 = CharsDrawn(pages[1], lines, static_cast<size_t>(fit));
                    if (drawn1 < pages[1].size())
                    {
                        desclast = 2;
                        pages[2] = pages[1].substr(drawn1);
                    }
                }
                if (descpage > desclast)
                {
                    descpage = desclast;
                    pages[2].clear();       // retail's clamp loses the third page (0x00548816)
                }
                if (descpage == 0 || !pages[descpage].empty())
                    writefield(pages[descpage], op.v[0], op.v[1]);
            }
            else if (op.modifier == EModifier::Block)
                writefield(text, op.v[0], op.v[1]);
            else if (op.modifier == EModifier::Grayable || op.modifier == EModifier::PrevButton
                     || op.modifier == EModifier::NextButton)
            {
                // Prev / Next: green while that way pages, grey when not (0x005484b9).
                static constexpr SColor3 kGreen{ 6, 221, 0 }, kGrey{ 60, 60, 60 };
                if (op.modifier == EModifier::NextButton || stricmp(text.c_str(), "Next") == 0)
                    writetext(text, descpage < desclast ? kGreen : kGrey);
                else if (op.modifier == EModifier::PrevButton || stricmp(text.c_str(), "Prev") == 0)
                    writetext(text, descpage > 0 ? kGreen : kGrey);
            }
            else
                writetext(text, colour);
            break;
          }
        }
    }
}

// REVSYNC: 0x00547240 -- the pane's own fields, else the object's
// (GetFieldText, retail vtable +0xc8). False: nothing is drawn.
bool TStatPane::Resolve(TObjectInstance &shown, const char *field, std::string &text, std::string &colour) const
{
    text.clear();
    if (strnicmp(field, "train", 5) == 0)
    {
        // The level-up picks: "+" for the first, another for the second
        // below level 15; only the main player's.
        if (&shown != Player)
            return true;
        const int32_t attribute = AttributeIndex(field + 5, strlen(field + 5));
        if (attribute < 0)
            return false;
        if (Player->LevelUpPick(0) == attribute)
            text += '+';
        if (Player->Level() < kSecondPickLevel && Player->LevelUpPick(1) == attribute)
            text += '+';
        return true;
    }
    if (stricmp(field, "objdesc") == 0)
    {
        // The type's description, "obj<unique id>", else the class's.
        char tag[16];
        snprintf(tag, sizeof(tag), "obj%08x", shown.ObjId());
        const int32_t id = DialogList.FindLine(tag);
        text = id >= 0 ? DialogList.GetLine(id) : DialogList.GetLine((std::string("obj") + shown.GetClassName()).c_str());
        return true;
    }
    if (stricmp(field, "type") == 0)
    {
        const int32_t type = shown.GetStat("type");
        if (type < 0 || type >= static_cast<int32_t>(std::size(kWeaponTypes)))
            return false;
        text = DialogList.GetLine(kWeaponTypes[type]);
        return true;
    }
    if (strnicmp(field, "stmod", 5) == 0 && strlen(field) == 10 && (field[9] == '1' || field[9] == '2'))
    {
        // An attribute's modifier, x 10 for 1, coloured by the attribute.
        const int32_t attribute = AttributeIndex(field + 5, 4);
        if (attribute < 0)
            return false;
        const int32_t value = shown.GetObjStat(PLRSTAT_FIRST + attribute);
        colour = value < 12 ? "red" : value > 16 ? "green" : "blue";
        const int32_t modifier = AttributeModifier(value) * (field[9] == '1' ? 10 : 1);
        text = (modifier >= 0 ? "+" : "") + std::to_string(modifier);
        return true;
    }
    char buf[kFieldChars] = {};
    const bool found = shown.GetFieldText(field, buf, kFieldChars);
    text = buf;
    return found;
}

std::string TStatPane::Signature(const SWalk &walk)
{
    std::string signature;
    for (const SRun &run : walk.runs)
    {
        char head[64];
        snprintf(head, sizeof(head), "%d %d %d %d %d %02x%02x%02x|", static_cast<int32_t>(run.kind), run.x, run.y,
                 run.w, run.h, run.colour.r, run.colour.g, run.colour.b);
        signature += head;
        signature += run.text;
        signature += '\n';
    }
    return signature;
}

// ---- drawing ---------------------------------------------------------------

void TStatPane::DrawRun(const SRun &run, TObjectInstance &shown, int32_t target_w, int32_t target_h) const
{
    const float r = run.colour.r / 255.0f, g = run.colour.g / 255.0f, b = run.colour.b / 255.0f;
    switch (run.kind)
    {
      case SRun::EKind::Text:
        DrawTextShadowedToTarget(font, run.text.c_str(), run.x, run.y, run.w, static_cast<int32_t>(font->lineStep),
                                 ETextAlign::Left, r, g, b, target_w, target_h);
        break;
      case SRun::EKind::Block:
        DrawTextWrappedToTarget(font, run.text.c_str(), run.x, run.y, run.w, run.h, ETextAlign::Left, r, g, b, true,
                                target_w, target_h);
        break;
      case SRun::EKind::Icon:
        DrawIcon(shown, run.x, run.y, target_w, target_h);
        break;
    }
}

// The ICON statement (0x00547aeb): the object's inventory image (vtable
// +0x130), keyed on magenta when it has no key of its own (but money); a
// character's in the ring RingT, anything else over its black shadow 3 px
// down and right (0x004bda20: fully faded to black).
void TStatPane::DrawIcon(TObjectInstance &shown, int32_t x, int32_t y, int32_t target_w, int32_t target_h) const
{
    TBitmap *image = shown.InventoryImage();
    if (!image || !Renderer)
        return;
    const EBitmapDecode decode = image->keycolor == 0 && shown.ObjClass() != OBJCLASS_MONEY
                               ? EBitmapDecode::MagentaKeyed : EBitmapDecode::Pixels;
    const bool character = shown.ObjClass() == OBJCLASS_CHARACTER;
    if (!character)
        Renderer->DrawBitmapTintedToTarget(image, x + 3, y + 3, target_w, target_h, 0.0f, 0.0f, 0.0f, 1.0f);
    Renderer->DrawBitmapToTarget(image, x, y, target_w, target_h, decode);
    if (character && ringt)
        Renderer->DrawBitmapToTarget(ringt, x, y, target_w, target_h, EBitmapDecode::Pixels);
}

// ---- the pane --------------------------------------------------------------

TStatPane::TStatPane() : TButtonPane(0, 0, kWidth, kHeight) {}
TStatPane::~TStatPane() = default;

// REVSYNC: 0x00546b50. Its host places it in the side panel's upper slot.
bool TStatPane::Initialize()
{
    if (IsOpen())
        return true;
    archive.reset(TMulti::LoadMulti("StatsPane.dat"));
    icons = AcquireSpellIcons();
    font = FontTable ? FontTable->Atlas("Small") : nullptr;
    if (!archive || !icons || !font)
    {
        log_error("[statpane] StatsPane.dat, SpellIcons.dat or the \"Small\" font is missing");
        archive.reset();
        icons.reset();
        return false;
    }
    chrome = archive->Bitmap("Stats");
    ringt = icons->Bitmap("RingT");
    LoadLayout();               // without it the pane says kInvalidLayout (retail: a fatal error)
    if (!TButtonPane::Initialize())
        return false;
    descpage = desclast = 0;
    buttonsfor = nullptr;
    SetObject(Player);
    return true;
}

// REVSYNC: 0x00546d70
void TStatPane::Close()
{
    if (!IsOpen())
        return;
    TButtonPane::Close();
    blocks.clear();
    layoutvalid = false;
    buttonsfor = nullptr;
    shown.clear();
    chrome = ringt = nullptr;
    icons.reset();
    archive.reset();
}

TObjectInstance *TStatPane::Object() const
{
    TObjectInstance *bound = object.Get();
    return bound ? bound : Player;
}

void TStatPane::SetObject(TObjectInstance *obj)
{
    object = obj;
    format = "page1";
    SetDirty(true);
}

void TStatPane::SetFormat(const char *name)
{
    format = name ? name : "";
    SetDirty(true);
}

// The hot spots are the selected block's BUTTONs, made again only when the
// block changes (0x005478f7): invisible buttons, no art, no key.
void TStatPane::SyncButtons()
{
    TObjectInstance *const bound = Object();
    const SBlock *const block = bound ? SelectBlock(*bound) : nullptr;
    if (block == buttonsfor)
        return;
    buttonsfor = block;
    ClearButtons();
    if (!block)
        return;
    for (const SOp &op : block->ops)
    {
        if (op.kind == SOp::EKind::Invalid)
            break;
        if (op.kind == SOp::EKind::Button)
            NewButton(new TButton(op.name.c_str(), op.v[0], op.v[1], op.v[2], op.v[3], 0, nullptr));
    }
}

void TStatPane::Pulse()
{
    TButtonPane::Pulse();
    if (!IsOpen())
        return;
    SyncButtons();
    TObjectInstance *const bound = Object();
    const SBlock *const block = bound ? SelectBlock(*bound) : nullptr;
    SWalk walk;
    if (block)
        Walk(*block, *bound, walk);
    if (Signature(walk) != shown)
        SetDirty(true);
}

// REVSYNC: 0x005491c0 -- the chrome, then the block's text and portrait
// (retail cleared the pane to black first, which the opaque chrome covers).
void TStatPane::ComposeBackground(int32_t target_w, int32_t target_h)
{
    SyncButtons();
    if (Renderer)
        Renderer->DrawBitmapToTarget(chrome, 0, 0, target_w, target_h, EBitmapDecode::Unkeyed);
    SWalk walk;
    TObjectInstance *const bound = Object();
    if (bound)                          // no player: the chrome alone (0x00547657)
    {
        if (const SBlock *block = SelectBlock(*bound))
            Walk(*block, *bound, walk);
        else
            walk.runs.push_back({ SRun::EKind::Text, kInvalidLayout, kStart, kStart, TextWidthOf(kInvalidLayout), 0,
                                  SColor3{} });
        for (const SRun &run : walk.runs)
            DrawRun(run, *bound, target_w, target_h);
    }
    shown = Signature(walk);
}

// REVSYNC: 0x005493b0 -- a right press shows the main player again; a
// release on the pane ends any drag and repaints.
void TStatPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_RIGHTDOWN)
        SetObject(Player);
    else if (button == MB_LEFTUP && InPane(x, y))
    {
        if (UIDragState::IsActive())
            UIDragState::Cancel();
        SetDirty(true);
        RedrawButtons();
    }
    TButtonPane::MouseClick(button, x, y);
}

// REVSYNC: 0x00549470 -- "page..." picks the format of that name; Prev and
// Next page a multipage field; train<attribute> picks it for the next level,
// or says STATMAX past 30.
void TStatPane::OnControl(TButton *button, int32_t msg)
{
    if (msg != CONTROL_CLICKED || !button)
        return;
    const char *name = button->GetName();
    if (strnicmp(name, "page", 4) == 0)
        SetFormat(name);
    else if (stricmp(name, "Next") == 0)
    {
        descpage = (std::min)(descpage + 1, kMaxDescPage);
        SetDirty(true);
    }
    else if (stricmp(name, "Prev") == 0)
    {
        descpage = (std::max)(descpage - 1, 0);
        SetDirty(true);
    }
    if (Player && Object() == Player && strnicmp(name, "train", 5) == 0)
    {
        const int32_t attribute = AttributeIndex(name + 5, strlen(name + 5));
        if (attribute < 0)
            return;
        if (Player->GetObjStat(PLRSTAT_FIRST + attribute) > kTrainMax)
            TextBar.Print("%s", DialogList.GetLine("STATMAX"));
        else
        {
            Player->TrainStat(attribute);
            SetDirty(true);
        }
    }
}
