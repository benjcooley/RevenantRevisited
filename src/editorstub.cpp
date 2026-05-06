// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               editor.cpp - Editor mode + stubs (transitional)         *
// *************************************************************************
//
// Stand-in for the retired retail editor. The new ImGui-based editor is
// built up in src/editor/*; until those modules land this file just hosts
// the global flag, the F12-driven entry/exit points, and the stub pane
// instances that command.cpp + a few other call sites still reference.
//
// *************************************************************************

#include "editorstub.h"

#include "editor.h"
#include "logging.h"
#include "runtimemode.h"
#include "template.h"

bool                  Editor       = false;
TMulti*               EditorData   = nullptr;
TEditorStubConsole    Console      = {};
TEditorStubScriptPane ScriptEditor = {};
TEditorStubScrollPane ScrollEditor = {};
TEditorStubStatusBar  StatusBar    = {};
TEditorStubClassPane  ClassPane    = {};

// Real terrain-template manager. Used outside the editor (overlay scripts
// in command.cpp reference it). Allocated lazily on first use.
PTTerrainTemplate     TerrainTemplates = nullptr;

// Legacy scratch buffer the retail editor + command interpreter both used
// for ad-hoc string formatting. Kept in this TU because command.cpp
// `extern char buf[];`s it.
char                  buf[1024]    = {};

// Legacy editor command stubs. The retail bodies lived in attic/src/
// editor.cpp; they're referenced by name from command.cpp's command table
// so we keep the symbols (no-ops). Will be retired or re-implemented as
// the ImGui editor lands.
class TToken;
int32_t CmdAddRC(TObjectInstance* /*ctx*/, TToken& /*t*/)        { return 0; }
int32_t CmdDeleteRC(TObjectInstance* /*ctx*/, TToken& /*t*/)     { return 0; }
int32_t CmdSaveTileBM(TObjectInstance* /*ctx*/, TToken& /*t*/)   { return 0; }

void StartEditor(bool /*starting*/)
{
    if (Editor) return;
    Editor = true;
    EditorEnter();
    SetCurrentMode(EditorMode());
}

void ShutDownEditor()
{
    if (!Editor) return;
    SetCurrentMode(GameMode());
    EditorExit();
    Editor = false;
}
