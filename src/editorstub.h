// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *                editor.h - Editor mode + stubs (transitional)          *
// *************************************************************************
//
// The retail TPane-based editor (Console / StatusBar / ToolBar / ClassPane
// / ScriptEditor / ScrollEditor) was retired to attic/src/editor.{h,cpp}
// during the 2026 port. The replacement is being built on ImGui; while
// that's standing up this header keeps the *names* the rest of the engine
// references (notably src/command.cpp + src/mappane.cpp) so the existing
// call sites compile unchanged.
//
// Every method on the stub panes is a no-op; every pane is a global
// instance with the same legacy name. Once an ImGui replacement for a
// given concept lands, the corresponding stub forwards to it (or its
// callers migrate off the stub and the symbol gets retired).
//
// Public flags + entry points:
//   bool Editor              -- true when in editor mode (F12 toggles).
//   void StartEditor(bool)   -- enter editor mode.
//   void ShutDownEditor()    -- leave editor mode.
//
// *************************************************************************

#pragma once

#include "revenant.h"

#include <cstdint>

class TObjectInstance;
class TMulti;
struct S3DPoint;

// ---------------------------------------------------------------------------
// Stub panes -- match the legacy method surface used by command.cpp,
// mappane.cpp, and a few other call sites. Return types match the retail
// signatures so existing call sites compile unchanged. All bodies are
// no-ops while the new ImGui editor is being built.
// ---------------------------------------------------------------------------

struct TEditorStubConsole
{
    int32_t winwidth = 0;   // legacy member (read directly, not as method)

    void Output(const char* /*line*/) {}
    void Hide()                       {}
    void Show()                       {}
    [[nodiscard]] bool IsHidden() const          { return true; }
    [[nodiscard]] bool AdjustingBounds() const   { return false; }
    bool Input(char* /*string*/)                 { return false; }
    [[nodiscard]] int32_t GetChar() const        { return 0; }
    void SetDirty(bool /*on*/)                   {}
};

struct TEditorStubStatusBar
{
    bool Select(int32_t /*mapindex*/, bool /*add*/ = false) { return false; }
    bool Deselect()                               { return false; }
    bool Deselect(int32_t /*mapindex*/)           { return false; }
    bool SetCurObj(int32_t /*index*/)             { return false; }
    bool Next()                                   { return false; }
    bool Prev()                                   { return false; }
    void Validate()                               {}
    void Undo()                                   {}
    void StartMoving()                            {}
    void StopMoving()                             {}
    [[nodiscard]] bool GridSnap()    const        { return false; }
    [[nodiscard]] bool RestrictX()   const        { return false; }
    [[nodiscard]] bool RestrictY()   const        { return false; }
    [[nodiscard]] bool RestrictZ()   const        { return false; }
    [[nodiscard]] bool EditWalkmap() const        { return false; }
    [[nodiscard]] int32_t GetSelectedObj() const  { return -1; }
    [[nodiscard]] int32_t GetNextObj()            { return -1; }
    [[nodiscard]] int32_t GetFirstObj()           { return -1; }
    void SetDirty(bool /*on*/)                    {}
};

struct TEditorStubClassPane
{
    [[nodiscard]] int32_t GetObjType() const      { return 0; }
    void SelObjType(int32_t /*type*/)             {}
    void PutObject(const S3DPoint& /*pos*/)       {}
    void SetDirty(bool /*on*/)                    {}
};

struct TEditorStubScriptPane
{
    void LoadScript(TObjectInstance* /*ctx*/)     {}
    void Show()                                   {}
    void Hide()                                   {}
};

struct TEditorStubScrollPane
{
    void SetScroll(TObjectInstance* /*ctx*/)      {}
    void Show()                                   {}
    void Hide()                                   {}
    [[nodiscard]] int32_t GetChar() const         { return 0; }
    [[nodiscard]] int32_t winwidth() const        { return 0; }
};

// Legacy class names kept as aliases so existing `extern TEditStatusPane
// StatusBar;` declarations across the codebase still resolve.
using TConsolePane      = TEditorStubConsole;
using TEditStatusPane   = TEditorStubStatusBar;
using TEditClassPane    = TEditorStubClassPane;
using TScriptPane       = TEditorStubScriptPane;
using TScrollEditorPane = TEditorStubScrollPane;

extern TMulti*         EditorData;
extern TConsolePane    Console;
extern TScriptPane     ScriptEditor;
extern TScrollEditorPane ScrollEditor;
extern TEditStatusPane StatusBar;
extern TEditClassPane  ClassPane;

// ---------------------------------------------------------------------------
// Editor mode flag + entry / exit points. F12 toggles `Editor`; while the
// ImGui editor is being built these calls are no-ops.
// ---------------------------------------------------------------------------

extern bool Editor;
void StartEditor(bool starting = false);
void ShutDownEditor();

// Legacy command-interpreter abbreviation depth used by the script + console
// command system. Kept at the same value the retail editor.h defined.
#define MINCMDABREV 1
