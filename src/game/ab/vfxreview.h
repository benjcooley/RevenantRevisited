#pragma once
#include <cstdint>

namespace VfxReview {
bool Initialize();
void Close();
void Render();
void DrawOverlay();
void HandleKeyPress(int32_t key, bool down);
}
