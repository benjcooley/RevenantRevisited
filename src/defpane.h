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
// `data/resources.rvr`). How retail draws each widget -- the rect, flags and
// draw mode of every call -- is docs/ui/forensics/DEF_ENGINE.md; the screens
// are docs/ui/forensics/{Options,SaveGame,LoadGame,InGameMenu,Popup}Def_SPEC.md.
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

#include "bitmap.h"   // TBitmap
#include "screen.h"   // TPane

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class TMulti;
class TSurface;
struct SFontAtlas;

// Widget classes, in the retail registry order (DEF_ENGINE.md §1.1).
// Droplist is parsed but unused by the in-scope screens.
enum class EDefWidget : uint8_t
{
    Bitmap, Frame, Text, Button, Scrollbar, Listbox, Edit, Droplist, Unknown
};

// widgets.def DM_USEDEFAULT: draw with the bitmap's (or the font's) own mode.
inline constexpr uint32_t kDefDrawDefault = 0x80000000u;

struct SDefColor
{
    uint8_t r = 255, g = 255, b = 255;
};

// An l t r b quad: FRAME insets, MARGINS, RECT insets, label rects.
struct SDefInsets
{
    int32_t l = 0, t = 0, r = 0, b = 0;
};

// How a widget draws its text (`FONT name <COLOR r g b> <FLAGS f>
// <DRAWMODE d>`): a TEXT, LISTBOX or EDIT style's text, a button label, a
// list format item. Flags are retail's compose flags (TextFlag, widgets.def
// TEXT_*). Labels and EDIT text default to TEXT_LEFT (0x00429ffa); a TEXT
// style to TEXT_LEFT | TEXT_SHADOW (DefaultStyle).
struct SDefTextFormat
{
    std::string font  = "Med";
    SDefColor   color;
    uint32_t    flags = 0x0001;
    uint32_t    drawmode = kDefDrawDefault;
};

// A scrollbar's arrow or thumb: the art is "<bitmap>U", "<bitmap>D" while
// held. An arrow (`UP|DOWN name <RELR> a <RELB> b`) sits a across (from the
// right with RELR) and b down (from the bottom with RELB). The thumb
// (`THUMB name a b <RELBR> c`) travels from a past the bar's start to b
// short of its end, c in from the side (the far side with RELBR).
struct SDefScrollPart
{
    std::string bitmap;
    int32_t     a = 0, b = 0, c = 0;
    bool        relR = false, relB = false;   // the thumb's RELBR is relR
};

// A resolved widget style. One per (type, variant) is parsed from widgets.def
// (and any screen-local STYLE override, e.g. popup.def); a widget copies its
// type's and parses the rest of its line over the copy. Not every field
// applies to every widget type.
struct SDefStyle
{
    // The chrome: BGBITMAP (a FRAME widget's BITMAP), tiled as a frame when
    // the style has FRAME, else drawn once at its size.
    std::string bgbitmap;
    bool        framed = false;
    SDefInsets  frame;                        // FRAME l t r b
    SDefInsets  margins;                      // MARGINS: the frame overhangs the rect by these
    bool        nocenter = false;             // NOCENTER: the frame's centre is left unpainted
    uint32_t    drawmode = kDefDrawDefault;

    SDefInsets     rect;                      // RECT: the text / content inset
    SDefInsets     outerRect;                 // EDIT: the RECT after BGBITMAP (frame and text)
    SDefTextFormat text;
    SDefColor      selcolor{0, 112, 74};      // LISTBOX selection
    SDefColor      editcolor{255, 255, 0};    // EDIT text while edited

    // BUTTON: faces and the label of each state (a state without a label
    // draws none).
    std::string    up, down;
    bool           hasUpLabel = false, hasDownLabel = false;
    SDefTextFormat uplabel, downlabel;
    SDefInsets     uplabelrect, downlabelrect;

    // LISTBOX: the item cell, and its scrollbar (`SCROLLBAR <RELRB> a b c d`
    // then the parts).
    int32_t     itemw = 0, itemh = 0;
    bool        hasScrollbar = false;
    bool        scrollRel = false;
    SDefInsets  scrollRect;                   // a b c d as l t r b

    // SCROLLBAR, a list's scrollbar.
    SDefScrollPart scrollUp, scrollDown, scrollThumb;
};

// A LISTBOX row-format item (the lines of its BEGIN ... END block), drawn in
// each item's cell. FIELD asks the pane for the text (FieldText), TEXT draws
// `name` as it is, BITMAP draws a bitmap field.
struct SDefFormatItem
{
    enum class EKind : uint8_t { Field, Text, Bitmap };

    EKind          kind = EKind::Field;
    int32_t        x = 0, y = 0, w = 0, h = 0;
    SDefTextFormat format;                    // defaults: the list's text
    std::string    name;
};

// A scrollbar's state: a SCROLLBAR widget's own, or a list's (its rect from
// the style's SCROLLBAR clause; the value is the top row).
struct SDefScrollbar
{
    int32_t x = 0, y = 0, w = 0, h = 0;
    bool    vertical = true;
    int32_t value = 0, minval = 0, maxval = 1000;   // retail defaults
    int32_t page = 100;                              // how far a track click moves it
};

// One parsed widget instance.
struct SDefWidget
{
    EDefWidget  type = EDefWidget::Unknown;
    int32_t     x = 0, y = 0, w = 0, h = 0;        // POS, pane-local
    std::string name, text, field;
    uint32_t    flags = 0;                         // the control word (the line's first FLAGS)
    int32_t     group = -1;                        // GROUP: radio buttons
    int32_t     maxlen = 20;                       // EDIT MAXLEN
    int32_t     hotkey = 0;                        // BUTTON key (retail button +0x70)
    SDefStyle   style;                             // resolved + overridden
    std::vector<SDefFormatItem> format;            // LISTBOX row format

    // Runtime state
    bool        hidden   = false;                  // CTRLFLAG_HIDDEN: neither drawn nor hit
    bool        disabled = false;                  // CTRLFLAG_DISABLED
    bool        pressed  = false;                  // a push button held (BTNFLAG_DOWN)
    bool        hovered  = false;
    bool        selected = false;                  // a toggle on (BTNFLAG_DOWN)
    bool        focused  = false;                  // EDIT is being edited
    double      focusTime = 0.0;                   // when its editing began (the caret's blink)
    TBitmap*    fieldBitmap = nullptr;             // BITMAP bound via SetFieldBitmap
    TBitmap*    faceUp    = nullptr;               // sprite BUTTON faces (AddSpriteButton):
    TBitmap*    faceDown  = nullptr;               //   up / pressed / hover; when set they
    TBitmap*    faceHover = nullptr;               //   replace the style's frame + label
    int32_t     count   = 0;                       // LISTBOX items
    int32_t     top     = 0;                       // LISTBOX first item shown (a row's first)
    int32_t     selitem = -1;                      // LISTBOX selected item, -1 none
    SDefScrollbar bar;                             // SCROLLBAR; a LISTBOX's own

    [[nodiscard]] bool Contains(int32_t px, int32_t py) const
    {
        return px >= x && py >= y && px < x + w && py < y + h;
    }

    // REVSYNC: each widget class's hit test (vtable +0x54), which the click
    // asks of the widgets in DEF order (0x00436530). A hidden or disabled
    // widget is passed over (0x004365f3..0x00436603). TEXT, BITMAP and FRAME
    // never take a click (0x00438a40, 0x00438910, 0x00438920): they are
    // decoration, so a FRAME drawn behind a list leaves the list its clicks.
    // The others take one in their rect (0x0042a7a0; BUTTON 0x0042d0d0,
    // whose BTNFLAG_NOTSQUARE key-colour test no shipped screen uses).
    [[nodiscard]] bool TakesClick(int32_t px, int32_t py) const
    {
        if (hidden || disabled)
            return false;
        if (type == EDefWidget::Text || type == EDefWidget::Bitmap || type == EDefWidget::Frame)
            return false;
        return Contains(px, py);
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

    // A static text label, drawn as a DEF TEXT widget: `flags` are its text
    // flags (TextFlag / widgets.def TEXT_*, e.g. TEXT_LEFT | TEXT_VCENTER);
    // `font` a FONT.DEF name ("Med", "Large", "small").
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
    // Open and done fading in (at once without DEF_FADE).
    [[nodiscard]] bool FadedIn() const;

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
    void SetFieldBitmap(const char* field, TBitmap* bm);  // BITMAP FIELD source
    // REVSYNC: list SetCount @ 0x00430c50 -- `count` items, shown from the
    // top; a selection past the end is dropped. Their text comes from
    // FieldText.
    void SetListCount(const char* listName, int32_t count);
    // A TEXT or EDIT widget's text (retail widget SetText, vtable +0x18).
    void SetText(const char* name, const std::string& text);
    // A BUTTON's key (retail button +0x70, set through vtable +0x24).
    void SetHotKey(const char* name, int32_t vk);
    // REVSYNC: list SetSelection @ 0x00430b80 -- select `item` (-1 = none)
    // of list `listName`, scroll it into view, and raise OnListSelect when
    // the selection changed.
    void SelectListItem(const char* listName, int32_t item);
    // REVSYNC: scrollbar SetRange @ 0x0042e3d0 -- a SCROLLBAR's range; the
    // value is clamped into it without an event.
    void SetSliderRange(const char* name, int32_t minval, int32_t maxval);
    // REVSYNC: scrollbar SetValue @ 0x0042e440 -- clamped into the range;
    // a change raises OnSliderChanged (OPTIONS.md §8).
    void SetSliderValue(const char* name, int32_t value);
    // A SCROLLBAR's page (+0xa0): how far a click on its track moves it.
    // Retail's constructors make it 100; screens set their own.
    void SetSliderPage(const char* name, int32_t page);

  protected:
    // Subclass hooks (retail OnControl, vtable slot 37, by event).
    // OnOpened: the widgets are built (event 1). OnActivate: a button was
    // clicked, or a toggle flipped (events 3000, 0xbb9 / 0xbba; the default
    // forwards it to the handler). OnListSelect: a list's selection changed
    // (event 5000). OnListScrolled: a list's first item changed (5001).
    // OnListClick: the mouse was released on a list's item (5002).
    // OnSliderChanged: a SCROLLBAR's value changed, by the player or
    // SetSliderValue (event 4000). OnKey: a key (retail DispatchInput
    // 0x004361f0: the EDIT being edited, then the buttons' keys).
    virtual void OnOpened() {}
    virtual void OnActivate(const SDefWidget& widget, int32_t buttonIndex);
    virtual void OnListSelect(const SDefWidget& list, int32_t item) { (void)list; (void)item; }
    virtual void OnListScrolled(const SDefWidget& list) { (void)list; }
    virtual void OnListClick(const SDefWidget& list, int32_t item) { (void)list; (void)item; }
    virtual void OnSliderChanged(const SDefWidget& slider) { (void)slider; }
    virtual void OnKey(int32_t vk, bool down);

    // The pane's field getter (retail vtable +0xa4): the text of `field` for
    // item `index` (a list's item; 0 for a TEXT widget), which may change
    // `colour` for this draw. Empty draws nothing.
    [[nodiscard]] virtual std::string FieldText(const std::string& field, int32_t index, SDefColor& colour);

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
    void RequestEnd();                               // EndModal once; the screen removes it next tick
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
    // The widgets' layout once the art is loaded (the constructors'): a
    // SCROLLBAR's size from its art, a list's grid and scrollbar.
    void LayoutWidgets();

    // --- assets (defpane.cpp) ---
    [[nodiscard]] TBitmap*          LookupBitmap(const std::string& entry);
    [[nodiscard]] const SFontAtlas* FontFor(const std::string& name);
    TMulti* LoadDat(const std::string& name);

    // --- rendering (defpane.cpp) ---
    void DrawWidget(const SDefWidget& w, int32_t index);
    void DrawText(const SDefWidget& w);
    void DrawButton(const SDefWidget& w);
    void DrawListbox(const SDefWidget& w, int32_t index);
    void DrawEdit(const SDefWidget& w);
    void DrawScrollbar(const SDefWidget& w, int32_t index);
    void RestoreBackground(const SDefInsets& margins, int32_t x, int32_t y, int32_t w, int32_t h);
    void RestoreUnderChrome(const SDefStyle& style, int32_t x, int32_t y, int32_t w, int32_t h);
    void DrawChrome(const SDefStyle& style, const std::string& bitmap,
                    int32_t x, int32_t y, int32_t w, int32_t h);
    void DrawFrame(TBitmap* bm, const SDefStyle& style, int32_t x, int32_t y,
                   int32_t w, int32_t h);
    void DrawString(const std::string& text, int32_t x, int32_t y, int32_t w,
                    int32_t h, const SDefTextFormat& format, const SDefColor& color);

    // --- lists (defpane.cpp) ---
    // A list's grid (retail layout 0x004308b0): the cells start at (cx,cy),
    // cols x rows of itemw x itemh.
    struct SListGrid
    {
        int32_t cx = 0, cy = 0, itemw = 1, itemh = 1, cols = 1, rows = 1;
        [[nodiscard]] int32_t Visible() const { return cols * rows; }
    };
    [[nodiscard]] static SListGrid ListGrid(const SDefWidget& w);
    [[nodiscard]] static bool IsVList(const SDefWidget& w);
    void ScrollList(SDefWidget& w, int32_t row);     // the first row shown, clamped
    [[nodiscard]] static int32_t ListItemAt(const SDefWidget& w, int32_t lx, int32_t ly);
    [[nodiscard]] static bool InListBar(const SDefWidget& w, int32_t lx, int32_t ly);
    [[nodiscard]] int32_t WidgetAt(int32_t lx, int32_t ly) const;
    void ClickListItem(SDefWidget& w, int32_t lx, int32_t ly);
    void SetSelection(SDefWidget& w, int32_t item);

    // --- scrollbars (defpane.cpp) ---
    // A scrollbar's parts in pane coordinates (retail layout 0x0042e240 and
    // SetValue 0x0042e440): each piece's top-left, the thumb's along its
    // track.
    struct SBarLayout
    {
        TBitmap* up = nullptr;       // "<base>U" art
        TBitmap* down = nullptr;
        TBitmap* thumb = nullptr;
        int32_t  upX = 0, upY = 0, downX = 0, downY = 0, thumbX = 0, thumbY = 0;
        int32_t  trackStart = 0, trackEnd = 0;    // the thumb's run along the bar
        bool     showThumb = false;
    };
    [[nodiscard]] SBarLayout BarLayout(const SDefWidget& w);
    void PressBar(int32_t index, int32_t lx, int32_t ly);
    void DragBar(SDefWidget& w, int32_t lx, int32_t ly);
    void ReleaseBarArrow(int32_t lx, int32_t ly);
    void ChangeBar(SDefWidget& w, int32_t value);    // clamp, set, raise the change

    [[nodiscard]] SDefWidget* EditingWidget();

    // --- state ---
    std::vector<SDefWidget> widgets;
    std::unordered_map<std::string, SDefStyle>  styles;   // key = "TYPE/VARIANT"
    std::unordered_map<std::string, uint32_t>   defines;  // #define symbol table
    std::unordered_map<std::string, TMulti*>    archives; // dat name -> archive
    std::vector<TMulti*>      bitmapDats;                  // search order
    std::string               targetPanel;                 // PANEL name to build
    std::string               bgDatName;
    TBitmap*                  background = nullptr;
    TSurface*                 surface    = nullptr;
    int32_t paneW = 0, paneH = 0;
    TActivateHandler onActivate;

    int32_t draggingBar = -1;                    // widget whose thumb is held
    int32_t dragGrab = 0;                        // where on the thumb it was taken
    int32_t arrowBar = -1, arrowStep = 0;        // the arrow held (-1 up / +1 down), stepped on release
    int32_t focusWidget = -1;                    // retail +0xa0: the widget with the focus

    bool    open  = false;
    // DEF flags and the DEF_FADE transition (retail +0x60, +0xb8..+0xc0).
    uint32_t defflags      = 0;
    double   fadeStartTime = 0.0;                // when the current fade began
    float    fadeFromLevel = 0.0f;               // the level it began at
    bool     finishing     = false;              // fading out before ending
    bool     endRequested  = false;              // faded out; the screen removes it next
    int32_t  finishResult  = 0;
};
