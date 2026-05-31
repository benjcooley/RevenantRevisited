// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uiequiptest.h - --test=ui-equip                                      *
// *************************************************************************
//
// Isolated harness for TEquipPane: the tall right-sidebar paperdoll panel
// holding the player's 11 EQ_* equipment slots (HEAD, NECK, BODY, OFFHAND,
// PRIMEHAND, R_ACCESSORY, L_ACCESSORY, RANGEDWEAPON, AMMO, LEGS, FEET).
// Composes the chrome `Equip` 188x306 bitmap at pane (0,0), then for each
// slot draws either the equipped item's baked .i3d inventory icon
// (oi->InventoryImage() / imagery->GetInvImage(state)) or the slot's
// named placeholder pictogram (Head/Chest/Hand/...) at the per-slot
// (x,y) anchor decoded in EquipPane_SPEC.md §4 Wave-6.
//
// Built from docs/ui/forensics/EquipPane_SPEC.md (clean-room).
//
// *************************************************************************

#pragma once

bool InitializeUIEquipMode();
void RenderUIEquipMode();
void CloseUIEquipMode();
