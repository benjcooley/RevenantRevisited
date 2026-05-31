// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    testmodes.h - Test mode helpers                    *
// *************************************************************************

#pragma once

#include <cstdint>

namespace TestModes {

bool Initialize(const char* mode);
void Close(const char* mode);
void Render(const char* mode);
void HandleMouseClick(const char* mode, int32_t button, int32_t x, int32_t y);
void HandleMouseMove(const char* mode, int32_t button, int32_t x, int32_t y);
void HandleKeyPress(const char* mode, int32_t key, bool down);
bool DumpTilesToFolder(const char* path);

// True while an --input-script is actively playing (events still pending, or
// looping). The real-mouse input path consults this to yield control to the
// script: when true, hardware mouse moves/clicks are ignored so the script has
// exclusive control of the cursor. Returns false once a non-looping script has
// fired its last event, handing control back to the real mouse. (Real keyboard
// is NOT gated — the script's key events augment it, and ESC still quits.)
bool InputScriptActive();

}  // namespace TestModes
