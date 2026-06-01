// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uidefscreentest.h - --test=ui-ingamemenu / ui-options / ui-savegame   *
// *                      / ui-loadgame  (DEF widget engine harness)        *
// *************************************************************************
//
// Drives the data-driven DEF widget engine (TDefScreen) for one DEF screen in
// isolation, composing it to a render target and blitting over a dim backdrop.
// One entry point serves every DEF screen; the mode string selects the panel.

#pragma once

#include <cstdint>

// Returns false if the mode is not a known DEF-screen mode (so the dispatcher
// can fall through). `mode` is the --test= string (e.g. "ui-ingamemenu").
bool IsUIDefScreenMode(const char* mode);

bool InitializeUIDefScreenMode(const char* mode);
void RenderUIDefScreenMode();
void CloseUIDefScreenMode();
void HandleMouseClickUIDefScreenMode(int32_t button, int32_t x, int32_t y);
void HandleMouseMoveUIDefScreenMode(int32_t button, int32_t x, int32_t y);
void HandleKeyPressUIDefScreenMode(int32_t key, bool down);
