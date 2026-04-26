// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *               editorlog.h - In-editor log viewer                      *
// *************************************************************************
//
// Captures the project's log_info/warn/error stream into a ring buffer
// and renders it as a virtualised ImGui panel inside the editor. The
// buffer is bounded (default 4096 entries) so a per-frame log storm
// doesn't grow it without limit; the panel uses ImGuiListClipper so
// rendering 4096 entries is always one-frame-cheap.
//
// Usage:
//   - EditorLogInstall() once at startup; registers a log.c callback.
//   - EditorLogDrawPanel() each frame inside the editor chrome, between
//     ImGui::Begin("Log") and ImGui::End().
//
// *************************************************************************

#pragma once

#include <cstdint>

// Hook the log.c stream into our in-memory ring buffer. Idempotent.
void EditorLogInstall();

// Render the log entries inside the *current* ImGui window. Caller is
// responsible for the surrounding Begin/End so this can be docked.
void EditorLogDrawPanel();

// Number of entries currently captured (clamped to the ring's capacity).
int32_t EditorLogCount();

// Clear the buffer.
void EditorLogClear();
