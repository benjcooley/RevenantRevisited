// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     equip.cpp - Equipment Pane                        *
// *************************************************************************

#include "revenant.h"
#include "equip.h"
#include "bitmap.h"
#include "display.h"
#include "playscreen.h"
#include "inventory.h"
#include "mappane.h"
#include "player.h"
#include "cursor.h"
#include "multi.h"

// Location of equipment slots on eq pane
struct { int32_t x, y; } EquipLoc[NUM_SECTS][NUM_EQ_SLOTS] =
{ { { 22, 83 }, { 64, 24 }, { 64, 66 }, { -1, -1 }, { 106, 83 }, { -1, -1 }, { -1, -1 }, { 106, 5 }, { 22, 5 }, { -1, -1 }, { -1, -1 } },
  { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { 106, 5 }, { 106, 47 }, { 22, 47 }, { -1, -1 }, { -1, -1 }, { 22, 5 }, { 64, 83 } }
};

void BtnEquipDown()
{
    EquipPane.Scroll(1);
}

void BtnEquipUp()
{
    EquipPane.Scroll(-1);
}

bool TEquipPane::Initialize()
{
    TButtonPane::Initialize();

    NewButton("\\/", 64, 108, 40, 15, -1, BtnEquipDown, GameData->Bitmap("equipdowndown"), GameData->Bitmap("equipdownup"));
    NewButton("/\\", 64, 5, 40, 15, -1, BtnEquipUp, GameData->Bitmap("equipupdown"), GameData->Bitmap("equipupup"));

    equipdata = TMulti::LoadMulti("equipscr.dat");

    top = equipdata->Bitmap("equipscrtop");
    bottom = equipdata->Bitmap("equipscrbottom");
    section = SECT_TOP;

    heldslot = grabslot = -1;
    SetDirty(true);

    return (top != nullptr) && (bottom != nullptr);
}

void TEquipPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    TButtonPane::MouseClick(button, x, y);

    if (!Player)
        return;

    if (button == MB_LEFTDOWN)
    {
        int32_t slot = OnSlot(x, y);
        if (slot >= 0)
        {
            if (Player->GetEquip(slot) && Player->GetEquip(slot)->InventoryImage())
            {
                heldslot = grabslot = slot;
                SetDragBitmap(Player->GetEquip(slot)->InventoryImage(),
                                x - EquipLoc[section][slot].x, y - EquipLoc[section][slot].y);
                SetDragObj(Player->GetEquip(slot));
                Update();
            }
        }
    }
    else if (button == MB_LEFTUP)
    {
        if (grabslot >= 0)
        {
            // dragging from the equipment pane to someplace else
            grabslot = -1;
            SetDragBitmap(nullptr);
            SetDragObj(nullptr);
            Update();
        }
        else if (InPane(x, y))
        {
            // dragging from the inventory pane to here
            PTObjectInstance inst = Inventory.GetContainer()->GetInventorySlot(Inventory.GetHeldSlot());
            if (inst)
            {
                int32_t slot = inst->GetStat("eqslot");
                if ((uint32_t)slot < NUM_EQ_SLOTS && Player->CanEquip(inst, slot))
                {
                    if (Player->GetEquip(slot))
                    {
                        // special case: accessories can go in either left or right slot
                        if (slot == EQ_R_ACCESSORY && Player->GetEquip(EQ_L_ACCESSORY) == nullptr)
                            slot = EQ_L_ACCESSORY;
                        else if (slot == EQ_L_ACCESSORY && Player->GetEquip(EQ_R_ACCESSORY) == nullptr)
                                slot = EQ_R_ACCESSORY;
                    }

                    if (inst->GetOwner() == Player)
                    {
                        // something already in that slot?
                        if (Player->GetEquip(slot))
                            Player->GetEquip(slot)->SetInventNum(inst->InventNum());

                        inst->SetInventNum(256 + slot);
                    }
                    else
                    {
                        // it's a container, so can't just swap inventory positions
                        inst->RemoveFromInventory();

                        if (Player->GetEquip(slot))
                        {
                            Player->GetEquip(slot)->RemoveFromInventory();
                            Inventory.GetContainer()->AddToInventory(Player->GetEquip(slot), Inventory.GetHeldSlot());
                        }

                        Player->AddToInventory(inst, 256 + slot);
                    }

                    Player->Equip(inst, slot);
                }
            }
        }
        Inventory.SetDirty(true);
        SetDirty(true);
    }
}

void TEquipPane::DrawBackground()
{
    if (!IsDirty())
        TButtonPane::DrawBackground();
    else
    {
        Display->Put(0, 0, section == SECT_TOP ? top : bottom, DM_BACKGROUND);

        if (Player)
        {
            for (int32_t i = 0; i < NUM_EQ_SLOTS; i++)
                if (grabslot != i && Player->GetEquip(i))
                    if (EquipLoc[section][i].x >= 0)
                        Player->GetEquip(i)->DrawInvItem(EquipLoc[section][i].x, EquipLoc[section][i].y);

            if (grabslot < 0)
                heldslot = -1;
        }

        if (section == SECT_TOP)
        {
            Button(0)->Show();
            Button(1)->Hide();
        }
        else
        {
            Button(1)->Show();
            Button(0)->Hide();
        }

        TButtonPane::DrawBackground();

        PlayScreen.MultiUpdate();
        SetDirty(false);
    }
}

void TEquipPane::Animate(bool draw)
{
    if (draw && !mousebutton && Player)
    {
        PTObjectInstance inst = Player->GetInventorySlot(OnSlot(cursorx - GetPosX(), cursory - GetPosY()));
        CursorOverObject(inst);
    }
}

void TEquipPane::Scroll(int32_t amount)
{
    if (amount > 0)
        section = SECT_BOTTOM;
    else if (amount < 0)
        section = SECT_TOP;
    else
        return;

    SetDirty(true);
}
 
void TEquipPane::DrawAnim(PTObjectInstance inst, PTBitmap bm)
{
    if (inst->InventNum() < 256)
        return;

    if (EquipLoc[section][inst->InventNum() - 256].x >= 0)
        Display->Put(EquipLoc[section][inst->InventNum() - 256].x, EquipLoc[section][inst->InventNum() - 256].y, bm);
}

int32_t TEquipPane::OnSlot(int32_t x, int32_t y)
{
    for (int32_t slot = 0; slot < NUM_EQ_SLOTS; slot++)
        if (EquipLoc[section][slot].x >= 0 && EquipLoc[section][slot].y >= 0 &&
            x >= EquipLoc[section][slot].x && x < (EquipLoc[section][slot].x + INVITEMREALWIDTH) &&
            y >= EquipLoc[section][slot].y && y < (EquipLoc[section][slot].y + INVITEMREALHEIGHT))
        return slot;

    return -1;
}
