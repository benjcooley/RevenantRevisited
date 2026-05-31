// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uibarinvtest.h - --test=ui-barinv                                    *
// *************************************************************************
//
// Isolated harness for TBarInvPane: the row of 42x42 quick-use slot boxes
// stamped onto the bottom bar between the spell ring and the right end cap.
// Each populated slot draws (a) the BarInvBox chrome, (b) the item's
// inventory icon via the baked .i3d icon path (oi->InventoryImage() ->
// imagery->GetInvImage(state)), and — when the item is a Pouch — a small
// "%d" white quantity in the lower-right; any item with value > 1 also gets
// a value label at the slot top. Built from
// docs/ui/forensics/BarInvPane_SPEC.md (clean-room).
//
// *************************************************************************

#pragma once

bool InitializeUIBarInvMode();
void RenderUIBarInvMode();
void CloseUIBarInvMode();
