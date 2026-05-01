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

}  // namespace TestModes
