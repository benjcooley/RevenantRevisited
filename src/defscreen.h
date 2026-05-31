// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  defscreen.h - data-driven DEF widget engine (TDefScreen)              *
// *************************************************************************
//
// Reconstruction of Revenant's 1999 DEF widget engine: the data-driven UI
// system that builds modal screens (Options, Save/Load, In-Game Menu, popups,
// the MP lobby...) by parsing per-screen `.def` layout files plus the shared
// `widgets.def` style library, and rendering them from `.dat` bitmap chrome.
//
// This replaces hand-coding each of those screens: every coordinate, asset,
// colour, alignment and flag comes from the shipped `.def` data (mounted out of
// `data/resources.rvr`). See recon/discovered/port_status/DefWidgetEngine.md and
// docs/ui/forensics/{Options,SaveGame,LoadGame,InGameMenu,Popup}Def_SPEC.md.
//
// Composition contract (docs/ui/forensics/NOMENCLATURE.md §3): the whole screen
// is composed into one offscreen TSurface render target via the `…ToTarget`
// primitive family, then DrawSurface'd once. No mixed swapchain/glyph path.

#pragma once

#include "bitmap.h"   // PTBitmap / TBitmap

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class TMulti;
class TSurface;
struct SFontAtlas;

// Widget classes, in the retail registry order (DefWidgetEngine.md §"Widget
// class roster"). Droplist is parsed but unused by the in-scope screens.
enum class EDefWidget : uint8_t
{
    Bitmap, Frame, Text, Button, Scrollbar, Listbox, Edit, Droplist, Unknown
};

struct SDefColor
{
    uint8_t r = 255, g = 255, b = 255;
    bool    set = false;
};

// l/t/r/b pixel quad — used for FRAME 9-slice insets, MARGINS, RECT, and the
// UPLABELRECT/DOWNLABELRECT label-offset rects.
struct SDefInsets
{
    int32_t l = 0, t = 0, r = 0, b = 0;
    bool    set = false;
};

// A resolved widget style. One per (type, variant) is parsed from widgets.def
// (and any screen-local STYLE override, e.g. popup.def); per-widget attribute
// overrides are merged on top when the widget instance is parsed. Not every
// field applies to every widget type — unused fields keep their defaults.
struct SDefStyle
{
    // Common chrome / text
    std::string bgbitmap;                 // BGBITMAP, or FRAME-widget BITMAP
    SDefInsets  frame;                    // FRAME l t r b (9-slice insets)
    SDefInsets  margins;                  // MARGINS l t r b
    SDefInsets  rect;                     // RECT l t r b (content inset)
    bool        nocenter = false;
    std::string font     = "Med";
    SDefColor   color{255, 255, 255, true};
    uint32_t    textflags = 0;
    uint32_t    drawmode  = 0;

    // BUTTON
    std::string up, down;                 // up / down face bitmaps
    std::string uplabelfont   = "Med";
    std::string downlabelfont = "Med";
    SDefColor   uplabelcolor{255, 255, 255, true};
    SDefColor   downlabelcolor{0, 0, 0, true};
    uint32_t    uplabelflags   = 0;
    uint32_t    downlabelflags = 0;
    int32_t     uplabelrect[4]   = {0, 0, 0, 0};   // x y w h offsets
    int32_t     downlabelrect[4] = {0, 0, 0, 0};

    // LISTBOX / EDIT / SCROLLBAR
    SDefColor   selcolor{0, 112, 74, true};        // listbox selection fill
    SDefColor   editcolor{255, 255, 0, true};      // edit caret/text colour
    int32_t     itemw = 0, itemh = 0;              // ITEM w h (row size)
    std::string scrollup, scrolldown, scrollthumb; // scrollbar piece bitmaps
};

// A LISTBOX per-row column format (the inner FIELD lines in a BEGIN…END block).
struct SDefListField
{
    int32_t     x = 0, y = 0, w = 0, h = 0;
    uint32_t    flags = 0;
    std::string font;
    SDefColor   color{255, 255, 255, true};
    std::string field;                    // data key (e.g. "gamelist_name")
};

// One parsed widget instance.
struct SDefWidget
{
    EDefWidget  type = EDefWidget::Unknown;
    int32_t     x = 0, y = 0, w = 0, h = 0;        // POS, pane-local
    std::string name, text, field;
    uint32_t    flags = 0;
    int32_t     maxlen = 0;                        // EDIT MAXLEN
    SDefStyle   style;                             // resolved + overridden
    std::vector<SDefListField> rowfields;          // LISTBOX row format

    // Runtime state
    bool        pressed  = false;
    bool        hovered  = false;
    bool        disabled = false;
    bool        selected = false;                  // toggle/checkbox checked state
    PTBitmap    fieldBitmap = nullptr;             // BITMAP bound via SetField
    std::vector<std::vector<std::string>> rows;    // LISTBOX data rows
    int32_t     selrow    = -1;
    int32_t     scrolltop = 0;
    int32_t     value = 0, minval = 0, maxval = 100; // SCROLLBAR

    [[nodiscard]] bool Contains(int32_t px, int32_t py) const
    {
        return px >= x && py >= y && px < x + w && py < y + h;
    }
};

// A DEF-driven modal screen: parsed widget tree + style table + assets,
// rendered into an owned TSurface render target.
class TDefScreen
{
  public:
    TDefScreen() = default;
    ~TDefScreen();

    TDefScreen(const TDefScreen&)            = delete;
    TDefScreen& operator=(const TDefScreen&) = delete;

    // Build panel `panelName` from `<defName>.def` (+ widgets.def styles), place
    // its pane at screen (x,y) with size (w,h), and resolve the "Background"
    // chrome from `<bgDatName>` (e.g. "ingamemenunotex.dat"). Returns false and
    // logs if a required asset is missing. All assets resolve out of the mounted
    // resources.rvr by name.
    bool Open(const char* defName, const char* panelName,
              int32_t x, int32_t y, int32_t w, int32_t h,
              const char* bgDatName);
    void Close();

    // Compose the whole screen into the owned RT (call once per frame before
    // blitting Surface()). No-op until Open() succeeds.
    void Render();

    [[nodiscard]] TSurface* Surface() const { return surface; }
    [[nodiscard]] int32_t   PaneX() const { return paneX; }
    [[nodiscard]] int32_t   PaneY() const { return paneY; }
    [[nodiscard]] int32_t   PaneW() const { return paneW; }
    [[nodiscard]] int32_t   PaneH() const { return paneH; }

    // Data binding
    [[nodiscard]] SDefWidget* Find(const char* name);
    void SetFieldBitmap(const char* field, PTBitmap bm);  // BITMAP FIELD source
    void SetListRows(const char* listName,
                     std::vector<std::vector<std::string>> rows);

    // Input (coords are pane-local, i.e. already minus PaneX/PaneY). Returns the
    // name of the activated widget on a completed click, else nullptr.
    void        OnMouseDown(int32_t lx, int32_t ly);
    const char* OnMouseUp(int32_t lx, int32_t ly);
    void        OnMouseMove(int32_t lx, int32_t ly);

  private:
    // --- parsing (defscreen.cpp) ---
    bool LoadDefFile(const char* name, bool collectPanels);
    void ParseStyleLine(const std::vector<std::string>& toks,
                        const std::vector<bool>& quoted);
    void ParseWidgetLines(const std::vector<std::vector<std::string>>& lines,
                          const std::vector<std::vector<bool>>& quoted,
                          size_t begin, size_t end);
    [[nodiscard]] uint32_t EvalFlags(const std::vector<std::string>& toks,
                                     size_t& i) const;
    [[nodiscard]] const SDefStyle& ResolveStyle(EDefWidget type,
                                                uint32_t flags) const;

    // --- assets (defscreen.cpp) ---
    [[nodiscard]] PTBitmap        LookupBitmap(const char* entry);
    [[nodiscard]] const SFontAtlas* FontFor(const std::string& name);

    // --- rendering (defscreen.cpp) ---
    void DrawWidget(const SDefWidget& w);
    void DrawListbox(const SDefWidget& w);
    void DrawEdit(const SDefWidget& w);
    void DrawScrollbar(const SDefWidget& w);
    void DrawText(const std::string& text, int32_t x, int32_t y, int32_t w,
                  int32_t h, uint32_t flags, const SDefColor& color,
                  const std::string& font);

    // --- input helpers (defscreen.cpp) ---
    void SelectListRow(SDefWidget& w, int32_t lx, int32_t ly);
    void SetSliderFromCursor(SDefWidget& w, int32_t lx, int32_t ly);
    void DrawNineSlice(PTBitmap bm, const SDefInsets& frame, int32_t x,
                       int32_t y, int32_t w, int32_t h);

    // --- state ---
    std::vector<SDefWidget> widgets;
    std::unordered_map<std::string, SDefStyle>  styles;   // key = "TYPE/VARIANT"
    std::unordered_map<std::string, uint32_t>   defines;  // #define symbol table
    std::unordered_map<std::string, TMulti*>    archives; // dat name -> archive
    std::vector<TMulti*>      bitmapDats;                  // search order
    std::string               targetPanel;                 // PANEL name to build
    std::string               bgDatName;
    PTBitmap                  background = nullptr;
    TSurface*                 surface    = nullptr;
    int32_t paneX = 0, paneY = 0, paneW = 0, paneH = 0;
    int32_t draggingSlider = -1;                 // widget index being dragged
    bool    open  = false;
};
