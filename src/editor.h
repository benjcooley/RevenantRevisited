// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  editor.h - EXILE editor routines                     *
// *************************************************************************

#ifndef _EDITOR_H
#define _EDITOR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#ifndef _BUTTON_H
#include "button.h"
#endif

#ifndef _FONT_H
#include "font.h"
#endif

#ifndef _SCROLL_H
#include "scroll.h"
#endif

#define PROMPT      "% "
#define MINCMDABREV 1

void StartEditor(bool starting = false);
void ShutDownEditor();
void LoadWorldBitmap();

// *********************************************
// * TTextPane - A generic pane for text entry *
// *********************************************

_CLASSDEF(TTextPane)
class TTextPane : public TPane
{
  public:
    TTextPane(int32_t x, int32_t y, int32_t w, int32_t h) : TPane(x, y, w, h) { text = nullptr; }
    ~TTextPane() {}

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int32_t key, bool down);
        // For extended keys (arrows, etc)
    virtual void CharPress(int32_t key, bool down);
        // Normal text input (shift-processed characters)

    bool InsertText(char *newtext);
        // Insert text at cursor position
    bool RemoveText(int32_t numchars, bool forward);
        // Remove text at cursor position, in direction indicated by forward
    bool SetCursor(int32_t nx, int32_t ny, bool ignorebounds = false);
        // Move the cursor to given position
    void UpdateWindow();
        // Check cursor position and update window to make sure it is visible

    virtual bool Input(char *string) { return InsertText(string); }
        // Process and then input the string to the buffer
    virtual bool Output(char *string) { return InsertText(string); }
        // Process and then output the string to the buffer

    int32_t GetOffset() { return curoffset; }
        // Current cursor offset in text buffer
    int32_t GetTextLen() { return textlen; }
        // Gets length of text buffer
    char *GetText(int32_t start, int32_t len, char *buf);
        // Gets text from the buffer

  // Data Members
    char *text;                                 // text being displayed/edited
    int32_t offset;                                 // offset to top line displayed in window
    int32_t textlen;                                // length of text in buffer
    int32_t curoffset;                              // offset of cursor in text buffer
    int32_t cursorx, cursory;                       // location of cursor in characters

    int32_t curstartx, curstarty;                       // cursor bounding box - start
    int32_t curwidth, curheight;                        // cursor bounding box - size

    int32_t windowx, windowy;                       // window position in text buffer
    int32_t winheight, winwidth;                    // window size
    int32_t wrapwidth;                              // wordwrap
};

// ****************************************************
// * TScrollEditorPane - Editing text data on scrolls *
// ****************************************************

_CLASSDEF(TScrollEditorPane)
class TScrollEditorPane : public TTextPane
{
  public:
    TScrollEditorPane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TScrollEditorPane() {}

    virtual bool Initialize();
    virtual void KeyPress(int32_t key, bool down);

    void SetScroll(PTObjectInstance s);

    void SaveText();
        // Save text back out to the scroll

  private:
    PTScroll scroll;
};

// ***************
// * TScriptPane *
// ***************
_CLASSDEF(TScriptPane)
class TScriptPane : public TTextPane
{
  public:
    TScriptPane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TScriptPane() {}

    virtual bool Initialize();
    virtual void KeyPress(int32_t key, bool down);
        // For extended keys (arrows, etc)

    void LoadScript(PTObjectInstance oi);
        // Load up the script from the object into the text buffer
    void SaveScript();
        // Save the text buffer as the script for the object

  private:
    PTObjectInstance inst;              // Instance for script being edited
};

_STRUCTDEF(SChained)
struct SChained
{
    void (*func)(int32_t x, int32_t y);         // Pointer to chained function
    int32_t xmin, xspan, ymin, yspan;       // Chaining ranges
    int32_t oldxval, oldyval;               // In case of abort
    PSChained next, prev;               // Linked list pointers
};

_STRUCTDEF(SNewRC)
struct SNewRC
{
    char type[MAXNAMELEN];
    char dir[MAXNAMELEN];
    char graphic[MAXNAMELEN];
    int32_t objclass;
    int32_t set;
};

#define KILL_EVENT           0
#define CHAR_AVAILABLE_EVENT 1

_CLASSDEF(TConsolePane)
class TConsolePane : public TTextPane
{
  public:
    TConsolePane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TConsolePane() {}

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int32_t key, bool down);
    virtual void CharPress(int32_t key, bool down);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);

    void ChainMouse(void (*cfunc)(int32_t, int32_t), int32_t x0 = 0, int32_t x1 = 100, int32_t xcur = 50,
                    int32_t y0 = -1, int32_t y1 = -1, int32_t ycur = -1);
        // Mouse chaining - for adjusting values via the mouse
    void SetupMouseBitmap(int32_t xspan, int32_t yspan);
        // Setup new mouse arrow, if any

    void SetBounds();
        // Enter bound-adjust mode
    bool AdjustingBounds() { return (box != nullptr); }
        // Returns whether bounding box is being edited
    bool AddAxis(PTObjectInstance inst);
        // Add the axis object at the location of inst
    void ClearAxis();
        // Clear out the axis object, if necessary

    virtual bool Input(char *string);
        // Send the string to console input
    virtual bool Output(char *string);
        // Output the string in the console

    // NOTE - These cannot be called from the main thread
    static int32_t GetChar();
        // Gets a character from the input stream
    static int32_t GetLine(char *buffer, int32_t buffersize);
        // Gets an entire line from the input stream
    static unsigned _stdcall CommandThread(void *arg);

    PSChained head;                     // Chained mouse func list
    PSChained chained;                  // Node for list traversal
    PSChained tail;                     // Tail of list (new funcs go here)
    int32_t cx, cy;                         // Last cursor pos
    int32_t oldcx, oldcy;                   // Update only when changed
    int32_t oldbuflen;                      // Mouse chain text update

    PTObjectInstance box;               // Bounding box display
    PTObjectInstance axis;              // Axis for lining up registration

    HANDLE cmdthreadhandle;             // Handle of the command processor thread
    static HANDLE cmdevents[2];         // Events used by the command processor thread
    static int32_t cmdchar;                 // Character passed to the command processor thread
    static char *cmdline;               // Command line pointer for console
};

// *************************
// * Status Bar for Editor *
// *************************

// This is a pane running across the top of the screen which gives some basic
// object info and allows management of selected objects.

_CLASSDEF(TEditStatusPane)
class TEditStatusPane : public TButtonPane
{
  public:
    TEditStatusPane() : TButtonPane(STATUSBARX, STATUSBARY, STATUSBARWIDTH, STATUSBARHEIGHT)
        { numobjs = curobj = iterator = 0; }

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Hide() { TButtonPane::Hide(); SetIgnoreInput(false); }

    void Validate();
        // Make sure all the selected objects are valid.

    bool Select(int32_t index, bool add = false);   // Add item to selected list
    bool Deselect();                            // Remove curobj from list
    bool Deselect(int32_t index);                   // Search for and remove indexed item
    bool Reselect(int32_t oldindex, int32_t newindex);  // For sector updates
    bool SetCurObj(int32_t newcurobj);              // Set the curobj
    bool Next();                                // Go to next item in list
    bool Prev();                                // Go to previous item in list
    void Delete();                              // Delete all items in list

    // Axis restriction buttons
    // (these assume the grid, x, y, and z buttons are first in the pane)
    bool GridSnap()  { return Button(0)->GetState(); }
    bool RestrictX() { return !Button(1)->GetState(); }
    bool RestrictY() { return !Button(2)->GetState(); }
    bool RestrictZ() { return !Button(3)->GetState(); }
    void InvertAxisButtons();
    bool EditWalkmap() { return Button(10)->GetState(); }

    // Object drag functions for setting flags
    void StartMoving();
    void StopMoving();

    void Undo();

    int32_t GetSelectedObj()
        { if ((uint32_t)curobj >= (uint32_t)numobjs) return -1; return selected[curobj]; }
    int32_t GetNextObj()
        { if (++iterator < numobjs) return selected[iterator]; return -1; }
    int32_t GetFirstObj()
        { iterator = -1; return GetNextObj(); }

  private:
    int32_t selected[MAXSELECTEDOBJS];
    int32_t numobjs;                            // number of objects in selected array
    int32_t curobj;                             // offset into selected array
    int32_t iterator;                           // for findfirst, findnext

    // saved data for undo
    bool canundo;                           // if this isn't set, there's nothing to undo
    S3DPoint lastpos[MAXSELECTEDOBJS];      // the objects' position prior to dragging
};

// **********************
// * General Edit Tools *
// **********************

// These tools run down the lefthand side of the screen, and consist of
// general object and sector manipulation functions.  Most of the functions operate
// on the currently selected object(s) in TEditStatusPane.

#define NUMBOOKMARKS    4

_CLASSDEF(TEditToolsPane)
class TEditToolsPane : public TButtonPane
{
  public:
    TEditToolsPane() : TButtonPane(TOOLBARX, TOOLBARY, TOOLBARWIDTH, TOOLBARHEIGHT) { ClearBookmarks(); }

    virtual bool Initialize();
        // Only need init to set up buttons, then buttonpane takes care of the rest
    virtual void DrawBackground();
        // Clear before button draw
    virtual void KeyPress(int32_t key, bool down);
        // For bookmarking
    virtual void Hide() { TButtonPane::Hide(); SetIgnoreInput(false); }
        // Still want to be able to use the buttons even when hidden

    void AddBookmark(int32_t mark, S3DPoint pos, int32_t lev);
        // Add a new bookmark
    void GetBookmark(int32_t mark, RS3DPoint pos, int32_t &lev);
        // Get the given bookmark
    void ClearBookmarks();
        // Clear out all bookmarks

  protected:
    S3DPoint bookmark[NUMBOOKMARKS];        // saved positions in the map pane
    int32_t bookmarklev[NUMBOOKMARKS];          // saved map level
};

// *********************
// * Object Class Pane *
// *********************

// This pane manipulates object types within the various classes, as well as providing
// a palette from which the user can choose object types for map creation.

_STRUCTDEF(SBitmapList)
struct SBitmapList
{
    char filename[MAXIMFNAMELEN];   // filename of imagery
    PTBitmap bm;                    // bitmap
    PSBitmapList next;              // next in list
};

_CLASSDEF(TEditClassPane)
class TEditClassPane : public TButtonPane
{
  public:
    TEditClassPane() : TButtonPane(EDMULTIPANEX, CONSOLEY, 636 - EDMULTIPANEX, CONSOLEHEIGHT)
        { curclass = curobj = firstobj = maxlines = scrollrate = accumulator = 0; clicked = false; thumbnails = nullptr; }

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Animate(bool draw);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    void SelClass(int32_t newclass) { curclass = newclass; firstobj = 0; SetDirty(true); }
        // Select a new class to display
    void SelObjType(int32_t newtype) { curobj = firstobj = newtype; SetDirty(true); }
        // Select a new object to display
    void SelectSame(int32_t index);
        // Select the type and class matching the one given
    int32_t GetClass() { return curclass; }
    int32_t GetObjType() { return curobj; }

    void Scroll(int32_t lines);
        // Scrolls the given number of lines in the pane

    int32_t PutObject(S3DPoint pos);
        // Put the currect object at pos
    PTBitmap GetThumbnail(char *fname);
        // Get the thumbnail pic for an object

    int32_t GetMaxLines() { return maxlines; }
        // Gets...well, maxlines

  private:
    int32_t curclass;                               // selected class
    int32_t curobj;                                 // selected object
    int32_t firstobj;                               // first displayed obj

    int32_t maxlines;                               // maximum lines displayable in pane
    int32_t scrollrate;                             // scrolling
    int32_t accumulator;                            // for < 1 fps scrolling

    PSBitmapList thumbnails;                    // Thumbnail pics to display

    bool clicked;                               // Whether click was in this pane
};

// Editor externals
extern PTMulti EditorData;
extern TConsolePane Console;
extern TScriptPane ScriptEditor;
extern TScrollEditorPane ScrollEditor;
extern TEditStatusPane StatusBar;
extern TEditToolsPane ToolBar;
extern TEditClassPane ClassPane;

#endif
