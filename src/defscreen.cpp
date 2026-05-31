// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  defscreen.cpp - data-driven DEF widget engine (TDefScreen)            *
// *************************************************************************
//
// Parses Revenant's `.def` UI layout files + the shared `widgets.def` style
// library and renders the resulting widget tree into a TSurface. See
// defscreen.h for the architecture and the recon citations.

#include "defscreen.h"

#include "font.h"
#include "logging.h"
#include "multi.h"
#include "renderer.h"
#include "revutils.h"   // rev_fopen
#include "surface.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// =====================================================================
// Flag bits — mirror the #define values in data/Resources/widgets.def. Only
// the bits the engine's style-resolution / text-alignment logic tests are
// named here; the full set is parsed generically from the .def #define lines
// into the symbol table and OR'd into widget/style flag words.
// =====================================================================
namespace {

constexpr uint32_t kTextLeft       = 0x0001;
constexpr uint32_t kTextCenter     = 0x0002;
constexpr uint32_t kTextRight      = 0x0004;
constexpr uint32_t kTextVCenter    = 0x0040;
constexpr uint32_t kTextShadow     = 0x0400;
constexpr uint32_t kTextflagCenter = 0x00040000;
constexpr uint32_t kTextflagRight  = 0x00080000;
constexpr uint32_t kCtrlClearBg    = 0x00001000;
constexpr uint32_t kBtnToggle      = 0x00040000;
constexpr uint32_t kBtnRadio       = 0x00080000;
constexpr uint32_t kScrollVScroll  = 0x00010000;
constexpr uint32_t kScrollHScroll  = 0x00020000;
constexpr uint32_t kListHList      = 0x00020000;
constexpr uint32_t kEditSpin       = 0x00040000;

// Retail bitmap fonts ("Med"/"Large"/"small" from font.def) are mapped to the
// Arial-metric Arimo TTF per the UI text-rendering convention
// (project-ui-text-rendering; InGameMenuDef_SPEC §2 maps "Med" -> Arimo-14).
struct SFontMap { const char* name; const char* path; int32_t px; };
constexpr SFontMap kFontMap[] = {
    {"Med",   "thirdparty/fonts/Arimo-Regular.ttf", 14},
    {"Large", "thirdparty/fonts/Arimo-Regular.ttf", 18},
    {"small", "thirdparty/fonts/Arimo-Regular.ttf", 11},
};
constexpr const char* kDefaultFontPath = "thirdparty/fonts/Arimo-Regular.ttf";
constexpr int32_t     kDefaultFontPx   = 14;

// ----- low-level helpers -------------------------------------------------

std::string ReadResourceText(const char* name)
{
    FILE* f = rev_fopen(name, "rb");
    if (!f)
        return {};
    fseek(f, 0, SEEK_END);
    const long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::string s;
    if (sz > 0)
    {
        s.resize(static_cast<size_t>(sz));
        const size_t got = fread(&s[0], 1, static_cast<size_t>(sz), f);
        s.resize(got);
    }
    fclose(f);
    return s;
}

bool ParseNumber(const std::string& t, int64_t& out)
{
    if (t.empty())
        return false;
    char* end = nullptr;
    const long v = strtol(t.c_str(), &end, 0);   // base 0: handles 0x.., decimal, sign
    if (end == t.c_str() || *end != '\0')
        return false;
    out = v;
    return true;
}

bool IsNumber(const std::string& t)
{
    int64_t v = 0;
    return ParseNumber(t, v);
}

// Strip // line + /* */ block comments and join `\`-newline continuations,
// then split into logical lines, pulling `#define NAME VALUE` into `defines`.
void Preprocess(const std::string& raw,
                std::unordered_map<std::string, uint32_t>& defines,
                std::vector<std::string>& outLines)
{
    std::string clean;
    clean.reserve(raw.size());
    const size_t n = raw.size();
    for (size_t i = 0; i < n; )
    {
        const char c = raw[i];
        if (c == '/' && i + 1 < n && raw[i + 1] == '*')          // block comment
        {
            i += 2;
            while (i + 1 < n && !(raw[i] == '*' && raw[i + 1] == '/')) ++i;
            i += 2;
            continue;
        }
        if (c == '/' && i + 1 < n && raw[i + 1] == '/')          // line comment
        {
            i += 2;
            while (i < n && raw[i] != '\n') ++i;
            continue;                                            // keep the newline
        }
        if (c == '"')                                            // string literal
        {
            clean += c; ++i;
            while (i < n && raw[i] != '"') { clean += raw[i]; ++i; }
            if (i < n) { clean += '"'; ++i; }
            continue;
        }
        if (c == '\\')                                           // line continuation
        {
            size_t j = i + 1;
            while (j < n && (raw[j] == ' ' || raw[j] == '\t' || raw[j] == '\r')) ++j;
            if (j < n && raw[j] == '\n') { clean += ' '; i = j + 1; continue; }
        }
        if (c == '\r') { ++i; continue; }
        clean += c; ++i;
    }

    size_t start = 0;
    while (start <= clean.size())
    {
        const size_t nl   = clean.find('\n', start);
        const size_t stop = (nl == std::string::npos) ? clean.size() : nl;
        std::string  line = clean.substr(start, stop - start);

        const size_t a = line.find_first_not_of(" \t");
        if (a != std::string::npos)
        {
            const std::string trimmed = line.substr(a);
            if (trimmed.rfind("#define", 0) == 0)
            {
                // #define NAME VALUE  -> symbol table
                size_t p = 7;
                auto skipws = [&] { while (p < trimmed.size() && std::isspace((unsigned char)trimmed[p])) ++p; };
                auto word   = [&] { const size_t s = p;
                                    while (p < trimmed.size() && !std::isspace((unsigned char)trimmed[p])) ++p;
                                    return trimmed.substr(s, p - s); };
                skipws(); const std::string name = word();
                skipws(); const std::string val  = word();
                int64_t v = 0;
                if (!name.empty() && ParseNumber(val, v))
                    defines[name] = static_cast<uint32_t>(v);
            }
            else
            {
                outLines.push_back(line);
            }
        }
        if (nl == std::string::npos) break;
        start = nl + 1;
    }
}

// Split a logical line into tokens. Quoted strings become one token (quotes
// stripped, `quoted` true); '|' is its own token.
void Tokenize(const std::string& line,
              std::vector<std::string>& toks, std::vector<bool>& quoted)
{
    const size_t n = line.size();
    for (size_t i = 0; i < n; )
    {
        const char c = line[i];
        if (c == ' ' || c == '\t') { ++i; continue; }
        if (c == '"')
        {
            ++i;
            std::string s;
            while (i < n && line[i] != '"') { s += line[i]; ++i; }
            if (i < n) ++i;
            toks.push_back(std::move(s)); quoted.push_back(true);
            continue;
        }
        if (c == '|') { toks.emplace_back("|"); quoted.push_back(false); ++i; continue; }
        std::string s;
        while (i < n && line[i] != ' ' && line[i] != '\t' && line[i] != '|' && line[i] != '"')
            { s += line[i]; ++i; }
        toks.push_back(std::move(s)); quoted.push_back(false);
    }
}

EDefWidget WidgetTypeOf(const std::string& kw)
{
    if (kw == "BITMAP")    return EDefWidget::Bitmap;
    if (kw == "FRAME")     return EDefWidget::Frame;
    if (kw == "TEXT")      return EDefWidget::Text;
    if (kw == "BUTTON")    return EDefWidget::Button;
    if (kw == "SCROLLBAR") return EDefWidget::Scrollbar;
    if (kw == "LISTBOX")   return EDefWidget::Listbox;
    if (kw == "EDIT")      return EDefWidget::Edit;
    if (kw == "DROPLIST")  return EDefWidget::Droplist;
    return EDefWidget::Unknown;
}

const char* TypeName(EDefWidget t)
{
    switch (t)
    {
        case EDefWidget::Bitmap:    return "BITMAP";
        case EDefWidget::Frame:     return "FRAME";
        case EDefWidget::Text:      return "TEXT";
        case EDefWidget::Button:    return "BUTTON";
        case EDefWidget::Scrollbar: return "SCROLLBAR";
        case EDefWidget::Listbox:   return "LISTBOX";
        case EDefWidget::Edit:      return "EDIT";
        case EDefWidget::Droplist:  return "DROPLIST";
        default:                    return "?";
    }
}

}  // namespace

// =====================================================================
// Flag-expression evaluation: `<term> [| <term>]*`, term = #define symbol or
// number. Advances `i` past the consumed terms. Stops at the next keyword.
// =====================================================================
uint32_t TDefScreen::EvalFlags(const std::vector<std::string>& toks, size_t& i) const
{
    uint32_t acc = 0;
    bool wantTerm = true;
    while (i < toks.size())
    {
        const std::string& tk = toks[i];
        if (tk == "|") { wantTerm = true; ++i; continue; }
        if (!wantTerm) break;                       // a non-'|' after a term ends the expr

        int64_t num = 0;
        if (ParseNumber(tk, num))
            acc |= static_cast<uint32_t>(num);
        else
        {
            const auto it = defines.find(tk);
            if (it != defines.end()) acc |= it->second;
            else break;                             // unknown identifier -> end of expr
        }
        wantTerm = false;
        ++i;
    }
    return acc;
}

// =====================================================================
// Style resolution: pick the widgets.def style for a widget's type + the
// variant implied by its flags.
// =====================================================================
const SDefStyle& TDefScreen::ResolveStyle(EDefWidget type, uint32_t flags) const
{
    static const SDefStyle kEmpty;
    std::string variant;
    switch (type)
    {
        case EDefWidget::Button:
            variant = (flags & kBtnToggle) ? "TOGGLE"
                    : (flags & kBtnRadio)  ? "RADIO" : "NORMAL";
            break;
        case EDefWidget::Scrollbar:
            variant = (flags & kScrollVScroll) ? "VSCROLL" : "HSCROLL";
            break;
        case EDefWidget::Listbox:
            variant = (flags & kListHList) ? "HLIST" : "VLIST";
            break;
        case EDefWidget::Edit:
            variant = (flags & kEditSpin) ? "SPIN" : "NORM";
            break;
        case EDefWidget::Droplist:
            variant = "VSCROLL";
            break;
        default:
            variant.clear();                        // FRAME / TEXT / BITMAP: single style
            break;
    }
    const std::string key = std::string(TypeName(type)) + "/" + variant;
    const auto it = styles.find(key);
    return (it != styles.end()) ? it->second : kEmpty;
}

// =====================================================================
// Attribute parsing. Applies STYLE-ish attributes (chrome, fonts, colours,
// rects) onto `s`. `flagsToText` routes FLAGS to the style's text flags (true
// for STYLE blocks); widget lines parse their own FLAGS into the widget word
// and pass false here. Label context (UPLABEL/DOWNLABEL) redirects the
// following FONT/COLOR/FLAGS to the up/down label fields.
// =====================================================================
namespace {

void ApplyAttrs(const std::vector<std::string>& t, const std::vector<bool>& q,
                size_t start, SDefStyle& s, bool flagsToText,
                const std::unordered_map<std::string, uint32_t>& defines)
{
    enum class Ctx { Main, Up, Down } ctx = Ctx::Main;

    auto kw  = [&](size_t i, const char* w) { return i < t.size() && !q[i] && t[i] == w; };
    auto num = [&](size_t i) -> int32_t { int64_t v = 0; if (i < t.size()) ParseNumber(t[i], v); return (int32_t)v; };
    auto str = [&](size_t i) -> std::string { return i < t.size() ? t[i] : std::string(); };
    auto evalFlags = [&](size_t& i) -> uint32_t {
        uint32_t acc = 0; bool wantTerm = true;
        while (i < t.size())
        {
            if (t[i] == "|") { wantTerm = true; ++i; continue; }
            if (!wantTerm) break;
            int64_t v = 0;
            if (ParseNumber(t[i], v)) acc |= (uint32_t)v;
            else { auto it = defines.find(t[i]); if (it != defines.end()) acc |= it->second; else break; }
            wantTerm = false; ++i;
        }
        return acc;
    };

    for (size_t i = start; i < t.size(); )
    {
        if (q[i]) { ++i; continue; }                 // stray quoted token
        const std::string& k = t[i];

        if (k == "FLAGS")
        {
            ++i; const uint32_t f = evalFlags(i);
            if (flagsToText)
            {
                if      (ctx == Ctx::Up)   s.uplabelflags   |= f;
                else if (ctx == Ctx::Down) s.downlabelflags |= f;
                else                       s.textflags      |= f;
            }
            continue;
        }
        if (k == "DRAWMODE")  { ++i; s.drawmode = evalFlags(i); continue; }

        if (k == "FONT")
        {
            const std::string f = str(i + 1);
            if      (ctx == Ctx::Up)   s.uplabelfont   = f;
            else if (ctx == Ctx::Down) s.downlabelfont = f;
            else                       s.font          = f;
            i += 2; continue;
        }
        if (k == "COLOR")
        {
            const SDefColor c{(uint8_t)num(i+1), (uint8_t)num(i+2), (uint8_t)num(i+3), true};
            if      (ctx == Ctx::Up)   s.uplabelcolor   = c;
            else if (ctx == Ctx::Down) s.downlabelcolor = c;
            else                       s.color          = c;
            i += 4; continue;
        }
        if (k == "EDITCOLOR") { s.editcolor = {(uint8_t)num(i+1),(uint8_t)num(i+2),(uint8_t)num(i+3),true}; i += 4; continue; }
        if (k == "SELCOLOR")  { s.selcolor  = {(uint8_t)num(i+1),(uint8_t)num(i+2),(uint8_t)num(i+3),true}; i += 4; continue; }
        if (k == "RECT")      { s.rect      = {num(i+1),num(i+2),num(i+3),num(i+4),true}; i += 5; continue; }
        if (k == "FRAME")     { s.frame     = {num(i+1),num(i+2),num(i+3),num(i+4),true}; i += 5; continue; }
        if (k == "MARGINS")   { s.margins   = {num(i+1),num(i+2),num(i+3),num(i+4),true}; i += 5; continue; }
        if (k == "BITMAP" || k == "BGBITMAP") { s.bgbitmap = str(i+1); i += 2; continue; }
        if (k == "NOCENTER")  { s.nocenter = true; ++i; continue; }
        if (k == "ITEM")      { s.itemw = num(i+1); s.itemh = num(i+2); i += 3; continue; }

        if (k == "UP")    { s.up        = str(i+1); i += 2; continue; }
        if (k == "DOWN")  { s.down      = str(i+1); i += 2; continue; }
        if (k == "THUMB") { s.scrollthumb = str(i+1); i += 2; continue; }

        if (k == "UPLABEL")   { s.uplabelfont   = str(i+1); ctx = Ctx::Up;   i += 2; continue; }
        if (k == "DOWNLABEL") { s.downlabelfont = str(i+1); ctx = Ctx::Down; i += 2; continue; }
        if (k == "UPLABELRECT")   { for (int j=0;j<4;++j) s.uplabelrect[j]   = num(i+1+j); ctx = Ctx::Main; i += 5; continue; }
        if (k == "DOWNLABELRECT") { for (int j=0;j<4;++j) s.downlabelrect[j] = num(i+1+j); ctx = Ctx::Main; i += 5; continue; }

        // Widget-instance attributes handled elsewhere, and position specifiers
        // (RELR/RELB/RELRB + their bare numbers) and unknown tokens: skip one.
        ++i;
    }
}

}  // namespace

// =====================================================================
// STYLE line: `STYLE <TYPE> [<VARIANT>] <attrs...>` -> styles["TYPE/VARIANT"].
// =====================================================================
void TDefScreen::ParseStyleLine(const std::vector<std::string>& toks,
                                const std::vector<bool>& quoted)
{
    if (toks.size() < 2) return;
    const std::string& typeName = toks[1];
    const EDefWidget   type     = WidgetTypeOf(typeName);
    if (type == EDefWidget::Unknown) return;

    // Optional variant keyword in toks[2].
    static const char* const kVariants[] = {
        "NORMAL", "TOGGLE", "RADIO", "VSCROLL", "HSCROLL",
        "VLIST", "HLIST", "NORM", "SPIN"
    };
    std::string variant;
    size_t      attrStart = 2;
    if (toks.size() > 2 && !quoted[2])
    {
        for (const char* v : kVariants)
            if (toks[2] == v) { variant = v; attrStart = 3; break; }
    }

    SDefStyle s;
    ApplyAttrs(toks, quoted, attrStart, s, /*flagsToText*/ true, defines);
    styles[std::string(typeName) + "/" + variant] = std::move(s);
}

// =====================================================================
// Widget lines inside a PANEL body [begin, end).
// =====================================================================
void TDefScreen::ParseWidgetLines(const std::vector<std::vector<std::string>>& lines,
                                  const std::vector<std::vector<bool>>& quoted,
                                  size_t begin, size_t end)
{
    for (size_t idx = begin; idx < end; )
    {
        const auto& t = lines[idx];
        const auto& q = quoted[idx];
        const EDefWidget type = WidgetTypeOf(t[0]);
        if (type == EDefWidget::Unknown) { ++idx; continue; }   // BEGIN/END/FIELD/stray

        SDefWidget w;
        w.type = type;

        // Pass 1: positional + identity attrs + FLAGS (-> w.flags).
        for (size_t i = 1; i < t.size(); )
        {
            if (q[i]) { ++i; continue; }
            const std::string& k = t[i];
            if      (k == "POS")   { int64_t v=0;
                                     if(i+1<t.size())ParseNumber(t[i+1],v); w.x=(int32_t)v;
                                     if(i+2<t.size())ParseNumber(t[i+2],v); w.y=(int32_t)v;
                                     if(i+3<t.size())ParseNumber(t[i+3],v); w.w=(int32_t)v;
                                     if(i+4<t.size())ParseNumber(t[i+4],v); w.h=(int32_t)v; i += 5; }
            else if (k == "NAME")      { if(i+1<t.size()) w.name  = t[i+1]; i += 2; }
            else if (k == "TEXT")      { if(i+1<t.size()) w.text  = t[i+1]; i += 2; }
            else if (k == "FIELD")     { if(i+1<t.size()) w.field = t[i+1]; i += 2; }
            else if (k == "STRINGREF") { if(i+1<t.size()) w.field = t[i+1]; i += 2; }
            else if (k == "MAXLEN")    { int64_t v=0; if(i+1<t.size())ParseNumber(t[i+1],v); w.maxlen=(int32_t)v; i += 2; }
            else if (k == "FLAGS")     { ++i; w.flags |= EvalFlags(t, i); }
            else                       { ++i; }
        }

        // Resolve the base style, then overlay per-widget chrome/text overrides.
        w.style = ResolveStyle(type, w.flags);
        ApplyAttrs(t, q, 1, w.style, /*flagsToText*/ false, defines);

        ++idx;

        // LISTBOX row format: inner BEGIN ... FIELD ... END.
        if (type == EDefWidget::Listbox && idx < end && !lines[idx].empty()
            && lines[idx][0] == "BEGIN")
        {
            ++idx;
            while (idx < end && !lines[idx].empty() && lines[idx][0] != "END")
            {
                const auto& ft = lines[idx];
                if (ft[0] == "FIELD")
                {
                    // FIELD x y w h [FLAGS expr] [FONT "f"] [COLOR r g b] "fieldkey"
                    SDefListField f;
                    int64_t v = 0;
                    if (ft.size()>1) { ParseNumber(ft[1], v); f.x = (int32_t)v; }
                    if (ft.size()>2) { ParseNumber(ft[2], v); f.y = (int32_t)v; }
                    if (ft.size()>3) { ParseNumber(ft[3], v); f.w = (int32_t)v; }
                    if (ft.size()>4) { ParseNumber(ft[4], v); f.h = (int32_t)v; }
                    const auto& fq = quoted[idx];
                    for (size_t i = 5; i < ft.size(); )
                    {
                        if (!fq[i] && ft[i] == "FLAGS") { ++i; f.flags = EvalFlags(ft, i); }
                        else if (!fq[i] && ft[i] == "FONT")  { if(i+1<ft.size()) f.font = ft[i+1]; i += 2; }
                        else if (!fq[i] && ft[i] == "COLOR") {
                            int64_t r=0,g=0,b=0;
                            if(i+1<ft.size())ParseNumber(ft[i+1],r);
                            if(i+2<ft.size())ParseNumber(ft[i+2],g);
                            if(i+3<ft.size())ParseNumber(ft[i+3],b);
                            f.color = {(uint8_t)r,(uint8_t)g,(uint8_t)b,true}; i += 4; }
                        else if (fq[i]) { f.field = ft[i]; ++i; }   // the trailing "fieldkey"
                        else { ++i; }
                    }
                    w.rowfields.push_back(std::move(f));
                }
                ++idx;
            }
            if (idx < end) ++idx;                       // consume END
        }

        widgets.push_back(std::move(w));
    }
}

// =====================================================================
// File loading. widgets.def is loaded styles-only; the screen .def adds any
// STYLE overrides + the requested panel's widgets.
// =====================================================================
bool TDefScreen::LoadDefFile(const char* name, bool collectPanels)
{
    const std::string raw = ReadResourceText(name);
    if (raw.empty())
    {
        log_error("[defscreen] cannot read '%s'", name);
        return false;
    }

    std::vector<std::string> rawLines;
    Preprocess(raw, defines, rawLines);

    std::vector<std::vector<std::string>> toks;
    std::vector<std::vector<bool>>        quoted;
    toks.reserve(rawLines.size());
    quoted.reserve(rawLines.size());
    for (const auto& l : rawLines)
    {
        std::vector<std::string> t;
        std::vector<bool>        q;
        Tokenize(l, t, q);
        if (!t.empty()) { toks.push_back(std::move(t)); quoted.push_back(std::move(q)); }
    }

    for (size_t idx = 0; idx < toks.size(); )
    {
        const std::string& kw = toks[idx][0];
        if (kw == "STYLE")
        {
            ParseStyleLine(toks[idx], quoted[idx]);
            ++idx;
        }
        else if (kw == "PANEL")
        {
            const std::string pname = (toks[idx].size() > 1) ? toks[idx][1] : std::string();
            size_t b = idx + 1;
            if (b < toks.size() && toks[b][0] == "BEGIN")
            {
                // Find the matching END, honouring nested LISTBOX BEGIN/END.
                size_t  e     = b + 1;
                int32_t depth = 1;
                while (e < toks.size() && depth > 0)
                {
                    if      (toks[e][0] == "BEGIN") ++depth;
                    else if (toks[e][0] == "END")   { if (--depth == 0) break; }
                    ++e;
                }
                if (collectPanels && pname == targetPanel)
                    ParseWidgetLines(toks, quoted, b + 1, e);
                idx = (e < toks.size()) ? e + 1 : toks.size();
            }
            else ++idx;
        }
        else ++idx;
    }
    return true;
}

// =====================================================================
// Asset + font lookup
// =====================================================================
PTBitmap TDefScreen::LookupBitmap(const char* entry)
{
    // Non-fatal name walk: TMulti::Object FatalErrors on a miss, which we can't
    // use when searching across several archives. Walk the name table directly
    // (numoffsets/names/offsets are public TMultiData fields).
    for (TMulti* dat : bitmapDats)
    {
        if (!dat) continue;
        for (int32_t c = 0; c < dat->numoffsets; ++c)
        {
            const char* p = reinterpret_cast<const char*>(static_cast<void*>(dat->names[c]));
            if (p && stricmp(p, entry) == 0)
                return reinterpret_cast<PTBitmap>(static_cast<void*>(dat->offsets[c]));
        }
    }
    return nullptr;
}

const SFontAtlas* TDefScreen::FontFor(const std::string& name)
{
    for (const SFontMap& fm : kFontMap)
        if (name == fm.name)
            return BuildTTFAtlas(fm.path, fm.px);
    return BuildTTFAtlas(kDefaultFontPath, kDefaultFontPx);
}

// =====================================================================
// Open / Close
// =====================================================================
bool TDefScreen::Open(const char* defName, const char* panelName,
                      int32_t x, int32_t y, int32_t w, int32_t h,
                      const char* bgDatName_)
{
    Close();

    paneX = x; paneY = y; paneW = w; paneH = h;
    targetPanel = panelName ? panelName : "";
    bgDatName   = bgDatName_ ? bgDatName_ : "";

    // 1) Styles + flag #defines from the shared widgets.def.
    if (!LoadDefFile("widgets.def", /*collectPanels*/ false))
        return false;

    // 2) The screen's own .def: any STYLE overrides + the requested panel.
    std::string screenFile = std::string(defName) + ".def";
    if (!LoadDefFile(screenFile.c_str(), /*collectPanels*/ true))
        return false;

    if (widgets.empty())
        log_warn("[defscreen] panel '%s' in %s produced no widgets",
                 targetPanel.c_str(), screenFile.c_str());

    // 3) Assets: the widget-chrome library + the screen's own chrome dat.
    if (TMulti* wdat = TMulti::LoadMulti(const_cast<char*>("widgetsnotex.dat")))
    {
        archives["widgetsnotex.dat"] = wdat;
        bitmapDats.push_back(wdat);
    }
    else
    {
        log_error("[defscreen] widgetsnotex.dat missing");
    }
    if (!bgDatName.empty())
    {
        if (TMulti* sdat = TMulti::LoadMulti(const_cast<char*>(bgDatName.c_str())))
        {
            archives[bgDatName] = sdat;
            bitmapDats.insert(bitmapDats.begin(), sdat);   // search screen dat first
        }
        else
        {
            log_error("[defscreen] %s missing", bgDatName.c_str());
        }
    }
    background = LookupBitmap("Background");

    // 4) Bind any BITMAP FIELD widgets to a default "Picture" chrome entry when
    //    the screen ships one (e.g. ingamemenu). Save/Load have no default
    //    thumbnail — their picture cell stays empty until a save is selected, so
    //    do NOT fall back to "Background" (that would tile the whole chrome into
    //    the small cell). Runtime SetFieldBitmap overrides this.
    for (SDefWidget& wid : widgets)
        if (wid.type == EDefWidget::Bitmap && !wid.field.empty() && !wid.fieldBitmap)
            wid.fieldBitmap = LookupBitmap("Picture");

    // 5) The compose-to-target render surface (default RGBA8 format).
    surface = new TSurface(paneW, paneH);
    open    = true;
    log_info("[defscreen] opened '%s' panel '%s' %dx%d @(%d,%d): %zu widgets, bg=%s",
             defName, targetPanel.c_str(), paneW, paneH, paneX, paneY,
             widgets.size(), background ? "OK" : "MISS");
    return true;
}

void TDefScreen::Close()
{
    delete surface;
    surface = nullptr;
    for (auto& kv : archives)
        if (kv.second) free(kv.second);     // TMulti is a malloc'd resource blob
    archives.clear();
    bitmapDats.clear();
    widgets.clear();
    styles.clear();
    defines.clear();
    background = nullptr;
    open = false;
}

TDefScreen::~TDefScreen() { Close(); }

// =====================================================================
// Data binding / lookup
// =====================================================================
SDefWidget* TDefScreen::Find(const char* name)
{
    if (!name) return nullptr;
    for (SDefWidget& w : widgets)
        if (w.name == name) return &w;
    return nullptr;
}

void TDefScreen::SetFieldBitmap(const char* field, PTBitmap bm)
{
    if (!field) return;
    for (SDefWidget& w : widgets)
        if (w.type == EDefWidget::Bitmap && w.field == field)
            w.fieldBitmap = bm;
}

void TDefScreen::SetListRows(const char* listName,
                             std::vector<std::vector<std::string>> rows)
{
    if (SDefWidget* w = Find(listName))
        w->rows = std::move(rows);
}

// =====================================================================
// Rendering
// =====================================================================
void TDefScreen::DrawNineSlice(PTBitmap bm, const SDefInsets& frame,
                               int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (!bm || !surface) return;
    if (frame.set && (frame.l || frame.t || frame.r || frame.b))
        Renderer->DrawNineSliceToTarget(bm, frame.l, frame.t, frame.r, frame.b,
                                        x, y, w, h, surface->Width(), surface->Height());
    else
        Renderer->DrawBitmapSubrectStretchedToTarget(bm, x, y, w, h, 0, 0,
                                                     bm->width, bm->height,
                                                     surface->Width(), surface->Height());
}

void TDefScreen::DrawText(const std::string& text, int32_t x, int32_t y,
                          int32_t w, int32_t h, uint32_t flags,
                          const SDefColor& color, const std::string& font)
{
    if (text.empty() || !surface) return;
    const SFontAtlas* atlas = FontFor(font);
    if (!atlas) return;

    ETextAlign align = ETextAlign::Left;
    if      (flags & (kTextflagRight  | kTextRight))  align = ETextAlign::Right;
    else if (flags & (kTextflagCenter | kTextCenter)) align = ETextAlign::Center;

    // The cell baseline is top-aligned at cellY+ascent; honour TEXT_VCENTER by
    // shifting the cell down so the single line centres vertically.
    int32_t cellY = y;
    if (flags & kTextVCenter)
    {
        const float lh = TextLineHeight(atlas);
        cellY = y + (int32_t)((h - lh) * 0.5f + 0.5f);
    }

    const float r = color.r / 255.0f, g = color.g / 255.0f, b = color.b / 255.0f;
    const int32_t tw = surface->Width(), th = surface->Height();
    if (flags & kTextShadow)
        DrawTextShadowedToTarget(atlas, text.c_str(), x, cellY, w, h, align, r, g, b, tw, th);
    else
        DrawTextToTarget(atlas, text.c_str(), x, cellY, w, h, align, r, g, b, tw, th);
}

void TDefScreen::DrawWidget(const SDefWidget& wid)
{
    const SDefStyle& st = wid.style;
    const int32_t tw = surface->Width(), th = surface->Height();

    switch (wid.type)
    {
        case EDefWidget::Bitmap:
        {
            if (wid.fieldBitmap)
                Renderer->DrawBitmapToTarget(wid.fieldBitmap, wid.x, wid.y, tw, th);
            else if (!st.bgbitmap.empty())
                if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
                    DrawNineSlice(bm, st.frame, wid.x, wid.y, wid.w, wid.h);
            break;
        }
        case EDefWidget::Frame:
        {
            if (!st.bgbitmap.empty())
                if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
                    DrawNineSlice(bm, st.frame, wid.x, wid.y, wid.w, wid.h);
            break;
        }
        case EDefWidget::Text:
        {
            DrawText(wid.text, wid.x + st.rect.l, wid.y, wid.w - st.rect.l - st.rect.r,
                     wid.h, st.textflags | wid.flags, st.color, st.font);
            break;
        }
        case EDefWidget::Button:
        {
            if (wid.flags & kBtnToggle)
            {
                // Checkbox/toggle: a fixed-size icon (CheckU unchecked / CheckD
                // checked) at native size, with the label to its right at the
                // UPLABELRECT offset (e.g. 18,-1) — NOT a stretched 9-slice.
                const std::string& box = wid.selected ? st.down : st.up;
                if (PTBitmap bm = LookupBitmap(box.c_str()))
                    Renderer->DrawBitmapToTarget(bm, wid.x, wid.y, tw, th);
                if (!wid.text.empty())
                {
                    const int32_t* lr = st.uplabelrect;
                    DrawText(wid.text, wid.x + lr[0], wid.y + lr[1],
                             wid.w - lr[0], wid.h, st.uplabelflags,
                             st.uplabelcolor, st.uplabelfont);
                }
                break;
            }

            const bool         down = wid.pressed;
            const std::string& face = down ? st.down : st.up;
            if (!face.empty())
                if (PTBitmap bm = LookupBitmap(face.c_str()))
                    DrawNineSlice(bm, st.frame, wid.x, wid.y, wid.w, wid.h);

            // Label (up/down style). The label rect is the button rect plus the
            // per-style offset rect (UPLABELRECT x y w h).
            if (!wid.text.empty())
            {
                const int32_t* lr = down ? st.downlabelrect : st.uplabelrect;
                const SDefColor lc = down ? st.downlabelcolor : st.uplabelcolor;
                const uint32_t  lf = down ? st.downlabelflags  : st.uplabelflags;
                const std::string& lfont = down ? st.downlabelfont : st.uplabelfont;
                DrawText(wid.text, wid.x + lr[0], wid.y + lr[1],
                         wid.w + lr[2], wid.h + lr[3], lf, lc, lfont);
            }
            break;
        }
        case EDefWidget::Scrollbar:
            DrawScrollbar(wid);
            break;
        case EDefWidget::Listbox:
            DrawListbox(wid);
            break;
        case EDefWidget::Edit:
            DrawEdit(wid);
            break;
        default:
            // SCROLLBAR / DROPLIST rendering lands with the Options screen
            // (next phase). Draw the bg chrome if the style names one so the
            // layout reads.
            if (!st.bgbitmap.empty())
                if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
                    DrawNineSlice(bm, st.frame, wid.x, wid.y, wid.w, wid.h);
            break;
    }
}

void TDefScreen::DrawListbox(const SDefWidget& w)
{
    const SDefStyle& st = w.style;
    const int32_t tw = surface->Width(), th = surface->Height();

    // Background frame (e.g. VScrollRect 9-slice), unless the widget clears its
    // own bg (CTRLFLAG_CLEARBG — the options controller list draws over chrome
    // that already has a recess baked in).
    if (!(w.flags & kCtrlClearBg) && !st.bgbitmap.empty())
        if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
            DrawNineSlice(bm, st.frame, w.x, w.y, w.w, w.h);

    // Content rect: RECT inset (the right inset reserves the scrollbar gutter).
    const int32_t cx   = w.x + st.rect.l;
    const int32_t cy   = w.y + st.rect.t;
    const int32_t cw   = w.w - st.rect.l - st.rect.r;
    const int32_t ch   = w.h - st.rect.t - st.rect.b;
    const int32_t rowh = (st.itemh > 0) ? st.itemh : 18;

    for (size_t r = 0; r < w.rows.size(); ++r)
    {
        const int32_t ry = cy + (static_cast<int32_t>(r) - w.scrolltop) * rowh;
        if (ry + rowh <= cy || ry >= cy + ch) continue;       // clip to content

        if (static_cast<int32_t>(r) == w.selrow)
            Renderer->DrawSolidRectToTarget(cx, ry, cw, rowh, tw, th,
                                            st.selcolor.r, st.selcolor.g,
                                            st.selcolor.b, 255);

        const std::vector<std::string>& row = w.rows[r];
        if (w.rowfields.empty())
        {
            if (!row.empty())
                DrawText(row[0], cx + 2, ry, cw - 4, rowh,
                         kTextLeft | kTextShadow | kTextVCenter, st.color, st.font);
        }
        else
        {
            for (size_t c = 0; c < w.rowfields.size() && c < row.size(); ++c)
            {
                const SDefListField& f = w.rowfields[c];
                const std::string&   fnt = f.font.empty() ? st.font : f.font;
                DrawText(row[c], cx + f.x, ry + f.y, f.w, (f.h ? f.h : rowh),
                         f.flags | kTextVCenter, f.color, fnt);
            }
        }
    }
}

void TDefScreen::DrawEdit(const SDefWidget& w)
{
    const SDefStyle& st = w.style;
    if (!st.bgbitmap.empty())
        if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
            DrawNineSlice(bm, st.frame, w.x, w.y, w.w, w.h);

    const int32_t cx = w.x + st.rect.l;
    const int32_t cy = w.y + st.rect.t;
    const int32_t cw = w.w - st.rect.l - st.rect.r;
    const int32_t ch = w.h - st.rect.t - st.rect.b;
    DrawText(w.text, cx, cy, cw, ch, st.textflags | kTextVCenter, st.color, st.font);
}

void TDefScreen::DrawScrollbar(const SDefWidget& w)
{
    const SDefStyle& st = w.style;
    const int32_t tw = surface->Width(), th = surface->Height();

    // Track background (HScrollBar / VScrollBar 9-slice).
    if (!st.bgbitmap.empty())
        if (PTBitmap bm = LookupBitmap(st.bgbitmap.c_str()))
            DrawNineSlice(bm, st.frame, w.x, w.y, w.w, w.h);

    // SCROLLBAR arrow/thumb pieces are each a 2-state button: the .dat stores
    // "<base>U" (unpressed) / "<base>D" (pressed) while widgets.def names the
    // base. Use the unpressed state for a static render.
    PTBitmap up    = LookupBitmap((st.up + "U").c_str());          // decrement arrow
    PTBitmap down  = LookupBitmap((st.down + "U").c_str());        // increment arrow
    PTBitmap thumb = LookupBitmap((st.scrollthumb + "U").c_str());

    const bool vertical = (w.flags & kScrollVScroll) != 0;
    auto vmid = [&](int32_t bh) { return w.y + (w.h - bh) / 2; };
    auto hmid = [&](int32_t bw) { return w.x + (w.w - bw) / 2; };
    const float frac = (w.maxval > w.minval)
        ? float(w.value - w.minval) / float(w.maxval - w.minval) : 0.0f;

    if (vertical)
    {
        if (up)    Renderer->DrawBitmapToTarget(up,   hmid(up->width),   w.y + 1, tw, th);
        if (down)  Renderer->DrawBitmapToTarget(down, hmid(down->width), w.y + w.h - down->height - 1, tw, th);
        if (thumb)
        {
            const int32_t top = w.y + 1 + (up ? up->height : 0) + 1;
            const int32_t bot = w.y + w.h - 1 - (down ? down->height : 0) - 1;
            const int32_t span = bot - top - thumb->height;
            const int32_t ty = top + (span > 0 ? int32_t(frac * span) : 0);
            Renderer->DrawBitmapToTarget(thumb, hmid(thumb->width), ty, tw, th);
        }
    }
    else
    {
        if (up)    Renderer->DrawBitmapToTarget(up,   w.x + 1, vmid(up->height),   tw, th);
        if (down)  Renderer->DrawBitmapToTarget(down, w.x + w.w - down->width - 1, vmid(down->height), tw, th);
        if (thumb)
        {
            const int32_t left  = w.x + 1 + (up ? up->width : 0) + 1;
            const int32_t right = w.x + w.w - 1 - (down ? down->width : 0) - 1;
            const int32_t span  = right - left - thumb->width;
            const int32_t tx    = left + (span > 0 ? int32_t(frac * span) : 0);
            Renderer->DrawBitmapToTarget(thumb, tx, vmid(thumb->height), tw, th);
        }
    }
}

void TDefScreen::Render()
{
    if (!open || !surface) return;

    surface->StartPass(0.0f, 0.0f, 0.0f, 0.0f);   // transparent (modal over frozen frame)

    const int32_t tw = surface->Width(), th = surface->Height();
    if (background)
        Renderer->DrawBitmapToTarget(background, 0, 0, tw, th);

    for (const SDefWidget& w : widgets)
        DrawWidget(w);

    surface->EndPass();
}

// =====================================================================
// Input
// =====================================================================
void TDefScreen::SelectListRow(SDefWidget& w, int32_t lx, int32_t ly)
{
    (void)lx;
    const SDefStyle& st = w.style;
    const int32_t cy   = w.y + st.rect.t;
    const int32_t ch   = w.h - st.rect.t - st.rect.b;
    const int32_t rowh = (st.itemh > 0) ? st.itemh : 18;
    if (rowh <= 0 || ly < cy || ly >= cy + ch) return;
    const int32_t row = (ly - cy) / rowh + w.scrolltop;
    if (row >= 0 && row < static_cast<int32_t>(w.rows.size()))
        w.selrow = row;
}

void TDefScreen::SetSliderFromCursor(SDefWidget& w, int32_t lx, int32_t ly)
{
    const SDefStyle& st = w.style;
    PTBitmap up    = LookupBitmap((st.up + "U").c_str());
    PTBitmap down  = LookupBitmap((st.down + "U").c_str());
    PTBitmap thumb = LookupBitmap((st.scrollthumb + "U").c_str());
    const bool vertical = (w.flags & kScrollVScroll) != 0;

    float frac = 0.0f;
    if (vertical)
    {
        const int32_t top  = w.y + 1 + (up ? up->height : 0) + 1;
        const int32_t bot  = w.y + w.h - 1 - (down ? down->height : 0) - 1;
        const int32_t half = thumb ? thumb->height / 2 : 0;
        const int32_t span = bot - top - (thumb ? thumb->height : 0);
        if (span > 0) frac = float(ly - top - half) / float(span);
    }
    else
    {
        const int32_t left  = w.x + 1 + (up ? up->width : 0) + 1;
        const int32_t right = w.x + w.w - 1 - (down ? down->width : 0) - 1;
        const int32_t half  = thumb ? thumb->width / 2 : 0;
        const int32_t span  = right - left - (thumb ? thumb->width : 0);
        if (span > 0) frac = float(lx - left - half) / float(span);
    }
    frac = (frac < 0.0f) ? 0.0f : (frac > 1.0f ? 1.0f : frac);
    w.value = w.minval + static_cast<int32_t>(frac * (w.maxval - w.minval) + 0.5f);
}

void TDefScreen::OnMouseDown(int32_t lx, int32_t ly)
{
    draggingSlider = -1;
    for (size_t i = 0; i < widgets.size(); ++i)
    {
        SDefWidget& w = widgets[i];
        if (w.disabled || !w.Contains(lx, ly)) continue;
        switch (w.type)
        {
            case EDefWidget::Button:    w.pressed = true;           break;
            case EDefWidget::Listbox:   SelectListRow(w, lx, ly);   break;
            case EDefWidget::Scrollbar:
                draggingSlider = static_cast<int32_t>(i);
                SetSliderFromCursor(w, lx, ly);
                break;
            default: break;
        }
    }
}

const char* TDefScreen::OnMouseUp(int32_t lx, int32_t ly)
{
    draggingSlider = -1;
    const char* activated = nullptr;
    for (SDefWidget& w : widgets)
    {
        if (w.type != EDefWidget::Button) continue;
        if (w.pressed && w.Contains(lx, ly))
        {
            activated = w.name.c_str();
            if (w.flags & kBtnToggle)        // checkbox: flip on click
                w.selected = !w.selected;
        }
        w.pressed = false;
    }
    return activated;
}

void TDefScreen::OnMouseMove(int32_t lx, int32_t ly)
{
    if (draggingSlider >= 0 && draggingSlider < static_cast<int32_t>(widgets.size()))
        SetSliderFromCursor(widgets[draggingSlider], lx, ly);   // drag

    for (SDefWidget& w : widgets)
        w.hovered = (w.type == EDefWidget::Button && !w.disabled && w.Contains(lx, ly));
}
