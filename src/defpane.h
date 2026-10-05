// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  defpane.h - DEF widget pane (TDefPane)                                *
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
// Retail DEF screens are panes: DefScreen_Open (0x00435150) initializes a
// TButtonPane and loads the widgets into it; the game then pushes it as an
// exclusive (modal) pane. TDefPane is that pane: it composes all its widgets
// into one offscreen TSurface in Compose() and submits it in Draw() (the pane
// draw contract, docs/gameflow/ARCHITECTURE.md §4.1). Panes the game builds in
// code (title screen, death pane) use OpenChrome + AddSpriteButton instead of
// a .def file. Behaviour of the dialogs built on it:
// docs/gameflow/forensics/INGAME_MENU.md.

#pragma once

#include "bitmap.h"   // PTBitmap / TBitmap
#include "screen.h"   // TPane

#include <cstdint>
#include <functional>
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
    int32_t     hotkey = 0;                        // BUTTON key (retail button +0x70)
    SDefStyle   style;                             // resolved + overridden
    std::vector<SDefListField> rowfields;          // LISTBOX row format

    // Runtime state
    bool        pressed  = false;
    bool        hovered  = false;
    bool        disabled = false;
    bool        selected = false;                  // toggle/checkbox checked state
    bool        focused  = false;                  // EDIT is being edited
    PTBitmap    fieldBitmap = nullptr;             // BITMAP bound via SetField
    PTBitmap    faceUp    = nullptr;               // sprite BUTTON faces (AddSpriteButton):
    PTBitmap    faceDown  = nullptr;               //   up / pressed / hover; when set they
    PTBitmap    faceHover = nullptr;               //   replace the style's frame + label
    std::vector<std::vector<std::string>> rows;    // LISTBOX data rows
    int32_t     selrow    = -1;
    int32_t     scrolltop = 0;
    int32_t     value = 0, minval = 0, maxval = 100; // SCROLLBAR

    [[nodiscard]] bool Contains(int32_t px, int32_t py) const
    {
        return px >= x && py >= y && px < x + w && py < y + h;
    }
};

// A DEF widget pane: parsed (or code-built) widget set + style table + assets,
// composed into an owned TSurface.
class TDefPane : public TPane
{
  public:
    // Retail DEF-screen flags: DefScreen_Open's third argument, kept at pane
    // +0x60 (INGAME_MENU.md §4.2).
    // OVERLAY: chrome drawn over the running game -- "<name>tex.dat" and
    // "widgetstex.dat" (0x00435b20); clear, the opaque title-route art
    // "<name>alpha.dat" / "widgetsalpha.dat".
    // FADE: the pane fades in over 5 pulses when it opens and out before it
    // closes (0x00435d70, 0x00436090, 0x00435010).
    static constexpr uint32_t DEF_OVERLAY = 0x01;
    static constexpr uint32_t DEF_FADE    = 0x10;
    // Every in-game dialog (menu, load, save, options, popups) opens with both.
    static constexpr uint32_t DEF_INGAME  = DEF_OVERLAY | DEF_FADE;

    // Retail control events (the pane's OnControl second argument).
    static constexpr int32_t EVENT_CLICKED   = 3000;   // a button
    static constexpr int32_t EVENT_SELECTED  = 5000;   // a list's selection changed
    static constexpr int32_t EVENT_EDITENTER = 6001;   // Enter ended an EDIT

    TDefPane() = default;
    ~TDefPane() override;
    TDefPane(const TDefPane&)            = delete;
    TDefPane& operator=(const TDefPane&) = delete;

    // REVSYNC: DefScreen_Open @ 0x00435150 (+ LoadAndShow 0x00435040). Build
    // panel `panelName` from `<defName>.def` (+ widgets.def styles) as a pane
    // at display (x,y) with size (w,h). The chrome comes from
    // "<datBase><variant>.dat" and the widget art from "widgets<variant>.dat",
    // the variant chosen by DEF_OVERLAY. Initializes the pane, then calls
    // OnOpened (retail control event 1). Returns false and logs if a required
    // asset is missing.
    bool Open(const char* defName, const char* panelName, uint32_t defFlags,
              int32_t x, int32_t y, int32_t w, int32_t h, const char* datBase);

    // Code-built pane (retail TLogoScreen 0x0053a2c0, TDeathPane 0x005339b0):
    // chrome from `datName`, background bitmap `backgroundEntry` (may be null),
    // widgets added with AddSpriteButton. Initializes the pane.
    bool OpenChrome(int32_t x, int32_t y, int32_t w, int32_t h,
                    const char* datName, const char* backgroundEntry);

    // A sprite button (retail TButton(multi, name, ...) 0x0042c400): faces
    // `<faceBase>U` (up), `<faceBase>D` (down) and `<faceBase>S` (hover) from
    // the pane's dats, placed at the up face's registration point (-regx,-regy,
    // pane-local). Returns false (and logs) if the up face is missing.
    bool AddSpriteButton(const char* name, const char* faceBase);

    // A static text label (retail draws code-built labels with the same text
    // engine as DEF TEXT widgets). `flags` are the TEXT_* bits from
    // widgets.def (e.g. TEXT_LEFT 0x1 | TEXT_VCENTER 0x40); `font` is a DEF
    // font name ("Med", "Large", "small").
    void AddText(const char* name, int32_t x, int32_t y, int32_t w, int32_t h,
                 const char* text, uint32_t flags, const SDefColor& color,
                 const char* font);

    // A completed button click. `buttonIndex` is the button's 1-based position
    // among the pane's BUTTON widgets, the order retail RunModal results follow.
    using TActivateHandler =
        std::function<void(TDefPane& pane, const SDefWidget& widget, int32_t buttonIndex)>;
    void SetOnActivate(TActivateHandler handler) { onActivate = std::move(handler); }

    // Ends the pane's modal run with `result` (retail: the result at +0x5c,
    // then the pane's close slot 0x00435010). A DEF_FADE pane fades out
    // first and ends when it reaches 0.
    void Finish(int32_t result);
    [[nodiscard]] bool IsFinishing() const { return finishing; }

    // TPane
    void Close() override;
    void Pulse() override;
    void Compose() override;
    void Draw() override;
    void MouseClick(int32_t button, int32_t x, int32_t y) override;
    void MouseMove(int32_t button, int32_t x, int32_t y) override;
    void KeyPress(int32_t key, bool down) override;
    void CharPress(int32_t key, bool down) override;

    // Data binding
    [[nodiscard]] SDefWidget* Find(const char* name);
    void SetFieldBitmap(const char* field, PTBitmap bm);  // BITMAP FIELD source
    void SetListRows(const char* listName,
                     std::vector<std::vector<std::string>> rows);
    // A TEXT or EDIT widget's text (retail widget SetText, vtable +0x18).
    void SetText(const char* name, const std::string& text);
    // A BUTTON's key (retail button +0x70, set through vtable +0x24).
    void SetHotKey(const char* name, int32_t vk);
    // REVSYNC: list SetSelection @ 0x00430b80 -- select `row` (-1 = none) of
    // list `listName`, scroll it into view, and raise OnListSelect when the
    // selection changed.
    void SelectListRow(const char* listName, int32_t row);
    // REVSYNC: scrollbar SetRange @ 0x0042e3d0 -- a SCROLLBAR's range; the
    // value is clamped into it without an event.
    void SetSliderRange(const char* name, int32_t minval, int32_t maxval);
    // REVSYNC: scrollbar SetValue @ 0x0042e440 -- clamped into the range;
    // a change raises OnSliderChanged (OPTIONS.md §8).
    void SetSliderValue(const char* name, int32_t value);

  protected:
    // Subclass hooks (retail OnControl, vtable slot 37, by event).
    // OnOpened: the widgets are built (event 1). OnActivate: a button was
    // clicked (event 3000; the default forwards to the handler).
    // OnListSelect: a list's selection changed (event 5000). OnSliderChanged:
    // a SCROLLBAR's value changed, by the player or SetSliderValue (event
    // 4000). OnKey: a key (retail DispatchInput 0x004361f0: the EDIT being
    // edited, then the buttons' keys).
    virtual void OnOpened() {}
    virtual void OnActivate(const SDefWidget& widget, int32_t buttonIndex);
    virtual void OnListSelect(const SDefWidget& list, int32_t row) { (void)list; (void)row; }
    virtual void OnSliderChanged(const SDefWidget& slider) { (void)slider; }
    virtual void OnKey(int32_t vk, bool down);
    // Draws a BITMAP FIELD widget (retail field getter, vtable slot 40,
    // 0x00436de0): the bound bitmap. Panes with live pictures override it.
    virtual void DrawField(const SDefWidget& widget);

    // The pane's paint into its surface (retail slot 21, inside the compose
    // pass): by default the background, then the widgets. A code-built pane
    // that draws its own content around the buttons overrides it and calls
    // the two parts itself, as retail paints call the button pane's draw
    // (0x00435de0) between their own steps.
    virtual void Paint();
    void PaintBackground();
    void PaintWidgets();

    // Compose-time helpers for subclasses (valid inside Paint / DrawField).
    [[nodiscard]] TSurface* Surface() const { return surface; }
    [[nodiscard]] uint32_t DefFlags() const { return defflags; }
    // Retail's "click1": once, at full volume, not positioned
    // (0x0049b990(id, 0x7f, 1, 0, 0x50, 700)).
    void PlayClick() const;

  private:
    void ReleaseAssets();
    void Render();                                   // compose widgets into `surface`
    void        OnMouseDown(int32_t lx, int32_t ly);
    const char* OnMouseUp(int32_t lx, int32_t ly);   // name of the activated widget
    void        OnMouseMove(int32_t lx, int32_t ly);
    void        Activate(const char* widgetName);
    [[nodiscard]] float FadeLevel() const;           // 0..kFadeSteps

    // --- parsing (defpane.cpp) ---
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

    // --- assets (defpane.cpp) ---
    [[nodiscard]] PTBitmap        LookupBitmap(const char* entry);
    [[nodiscard]] const SFontAtlas* FontFor(const std::string& name);
    TMulti* LoadDat(const std::string& name);

    // --- rendering (defpane.cpp) ---
    void DrawWidget(const SDefWidget& w);
    void DrawListbox(const SDefWidget& w);
    void DrawListScrollbar(const SDefWidget& w);
    void DrawEdit(const SDefWidget& w);
    void DrawScrollbar(const SDefWidget& w);
    void DrawText(const std::string& text, int32_t x, int32_t y, int32_t w,
                  int32_t h, uint32_t flags, const SDefColor& color,
                  const std::string& font);

    // --- input helpers (defpane.cpp) ---
    void ClickListRow(SDefWidget& w, int32_t lx, int32_t ly);
    bool ClickListScrollbar(SDefWidget& w, int32_t lx, int32_t ly);
    void SetSelection(SDefWidget& w, int32_t row);
    void SetSliderFromCursor(SDefWidget& w, int32_t lx, int32_t ly);
    bool StepSliderArrow(SDefWidget& w, int32_t lx, int32_t ly);
    void ChangeSlider(SDefWidget& w, int32_t value);   // clamp, set, raise OnSliderChanged
    void DrawNineSlice(PTBitmap bm, const SDefInsets& frame, int32_t x,
                       int32_t y, int32_t w, int32_t h);
    [[nodiscard]] SDefWidget* EditingWidget();

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
    int32_t paneW = 0, paneH = 0;
    TActivateHandler onActivate;
    int32_t draggingSlider = -1;                 // widget index being dragged
    bool    open  = false;

    // DEF flags and the DEF_FADE transition (retail +0x60, +0xb8..+0xc0).
    uint32_t defflags      = 0;
    double   fadeStartTime = 0.0;                // when the current fade began
    float    fadeFromLevel = 0.0f;               // the level it began at
    bool     finishing     = false;              // fading out before ending
    int32_t  finishResult  = 0;
};
