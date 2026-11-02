// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiscrollpanetest.h - --test=ui-scrollpane                            *
// *************************************************************************
//
// Clean-room reconstruction of TScrollPane (cls_0x5b5750) — the in-game
// parchment scroll reader panel. Exercises both item-scroll and spell-scroll
// content variants from the same parchment chrome (scroll.dat).
//
// Modes exercised (via input scripts):
//   --test=ui-scrollpane          — static item-scroll view
//   --test=ui-scrollpane (spell)  — spell-scroll variant (same chrome, different
//                                   content driven by harness SScrollContent)
//
// *************************************************************************

#pragma once

#include <cstdint>

bool InitializeUIScrollPaneMode();
void RenderUIScrollPaneMode();
void CloseUIScrollPaneMode();

// Harness-accessible state — used by input-script test commands and the
// #6a / #6b spell-scroll harness to drive paging and content binding.
namespace UIScrollPane {

// Switch the displayed content. title and body are static strings (no copy).
// `isSpell`: if true, the item-icon slot is populated from SpellIcons.dat
// by `iconName`; otherwise the icon area is blank.
void SetContent(const char* title, const char* body,
                bool isSpell, const char* iconName);

// Advance/retreat by `lines` (positive = down). Clamped to [0, numlines].
void Scroll(int32_t lines);

// Returns current scroll offset (for test verification).
int32_t GetScrollLine();

} // namespace UIScrollPane
