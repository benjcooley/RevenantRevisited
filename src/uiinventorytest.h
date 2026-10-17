// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiinventorytest.h - --test=ui-inventory: TInventory                  *
// *************************************************************************
//
// Hosts the production inventory (TInventory, inventory.h) on the test
// screen in the side panel's lower slot, showing the demo player's
// inventory (uidemoplayer.h). --test=ui-hud hosts it the same way beside the
// HUD harness's other side panes, shown while the side panel is open on the
// inventory.
//
// --test=ab-inventory is one case of the inventory's retail A/B
// (tools/retail_ab/hud_ab.py inventory), from --ab-case, fields split by ';':
//   items=I@S|I@S|...  the player's carried items: an item (uipanehost.h,
//                      SAbItem) and its inventory slot; the rest are empty
//   bag=S              the pane shows the inside of the bag in slot S
//   first=N            the first slot the grid shows (a multiple of 3)
//   frame=N            the game frame, which picks an animated icon's frame
// On the first frame at a tick boundary (run it with --snapstep=0.03125)
// the host stops the clock and pulses the pane once, over black.
// --snap=<png> writes the frame (give it --snapwarmup=12); --ab-out=<json>
// writes what the cells and the gold readout show.
//
// *************************************************************************

#pragma once

bool InitializeUIInventoryMode();
bool InitializeUIInventoryModeEmbedded();   // --test=ui-hud: shown with the side panel's inventory
void RenderUIInventoryMode();
void CloseUIInventoryMode();

bool InitializeABInventoryMode();
void RenderABInventoryMode();
void CloseABInventoryMode();
