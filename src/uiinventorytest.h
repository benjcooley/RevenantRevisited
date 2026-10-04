// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiinventorytest.h - --test=ui-inventory                              *
// *************************************************************************
//
// Isolated harness for the InventorySidebarPane (the 188x174 right-sidebar
// inventory grid: 4x3 column-major item cells, gold readout, container icon,
// L/R page arrows). Built from docs/ui/forensics/InventoryPane_SPEC.md using
// the real retail chrome from inventory.dat and real baked .i3d icons
// extracted from spawned item instances via oi->InventoryImage() (the same
// path the in-engine inventory paints with, [[project-ui-portrait-and-icons]]).
//
// *************************************************************************

#pragma once

bool InitializeUIInventoryMode();
void RenderUIInventoryMode();
void RenderUIInventoryModeEmbedded();
void SetUIInventoryModeVisible(bool visible);
void CloseUIInventoryMode();
