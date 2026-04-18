// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Inventory.cpp - TInvetory object                     *
// *************************************************************************

#include <stdlib.h>

#include "revenant.h"
#include "inventory.h"
#include "object.h"
#include "mappane.h"
#include "display.h"
#include "cursor.h"
#include "player.h"
#include "equip.h"
#include "font.h"
#include "money.h"
#include "animation.h"
#include "textbar.h"

bool TInventory::Initialize()
{
    grabslot = heldslot = -1;
    isdragging = false;

    SetDirty(true);

    return true;
}

void TInventory::Close()
{
}

void TInventory::DrawBackground()
{
    if (container && IsDirty())
    {
        Display->Box(INVENTORYCONTX, INVENTORYCONTY,
            INVITEMREALHEIGHT, INVITEMREALWIDTH, 0, 0xffff, 0, DM_BACKGROUND);
        container->DrawInvItem(INVENTORYCONTX, INVENTORYCONTY);

        for (int32_t y = 0; y < INVITEMSY; y++)
            for (int32_t x = 0; x < INVITEMSX; x++)
                Display->Box((x * INVITEMHEIGHT) + INVENTORYSTARTX,
                    (y * INVITEMWIDTH) + INVENTORYSTARTY, INVITEMREALHEIGHT,
                    INVITEMREALWIDTH, 0, 0xffff, 0, DM_BACKGROUND);

        for (TInventoryIterator i(container); i; i++)
        {
            TObjectInstance* oi = i.Item();

            if ((!isdragging || oi->InventNum() != grabslot) && oi->InventNum() < 256)
            {
                if (oi->InventNum() < 0)
                {
                    oi->SetInventNum(container->FindFreeInventorySlot());
                    TextBar.Print("Bad inventory slot for %s", oi->GetName());
                }

                int32_t x = (oi->InventNum() % INVITEMSX) * INVITEMWIDTH;
                int32_t y = (oi->InventNum() / INVITEMSX) * INVITEMHEIGHT;
                x += INVENTORYSTARTX;
                y += INVENTORYSTARTY;

                oi->DrawInvItem(x, y);

                if (oi->Amount() > 1)
                {
                    char buf[80];
                    itos(oi->Amount(), buf, 80);
                    Display->WriteText(buf, x, y-1, 1, GameData->Font("numbers"));
                }
            }
        }

        if (grabslot < 0)
            heldslot = -1;
        SetDirty(false);
    }
}

void TInventory::Animate(bool draw)
{
    if (draw && container && !mousebutton)
    {
        TObjectInstance* inst = container->GetInventorySlot(OnSlot(cursorx - GetPosX(), cursory - GetPosY()));
        CursorOverObject(inst);
    }
}

void TInventory::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (!container)
        return;

    if (button == MB_LEFTDOWN && InPane(x, y))
    {
        heldslot = grabslot = OnSlot(x, y);
        startposx = x;
        startposy = y;
        isdragging = false;
    }
    else if (button == MB_LEFTUP)
    {
        // check to see if they are on the far left (ie, chest/bag/pack icon)
        if (x >= 0 && x <= INVENTORYSTARTX && y >= 0 && y < INVENTORYPANEHEIGHT)
        {
            if (container->GetOwner())
            {
                if (grabslot < 0)
                    SetContainer(container->GetOwner());
                else
                {
                    TObjectInstance* inst = container->GetInventorySlot(grabslot);
                    if (inst && container->GetOwner()->FindFreeInventorySlot() < MAXINVITEMS)
                    {
                        inst->RemoveFromInventory();
                        container->GetOwner()->AddToInventory(inst);
                    }
                }
            }
        }
        else if (grabslot >= 0)
        {
            // handle swaping and using objects
            int32_t newslot = OnSlot(x, y);

            if (newslot >= 0)
            {
                TObjectInstance* inst = container->GetInventorySlot(grabslot);
                TObjectInstance* oi = container->GetInventorySlot(newslot);

                bool used = false;

                if (inst && oi && ((isdragging && oi != inst) || (!isdragging && oi == inst)))
                {
                    if (oi->Use(GetTopContainer(), oi != inst ? inst->GetMapIndex() : -1))
                    {
                        used = true;
                        Update();
                    }
                }

                if (!used)
                    SwapSlots(grabslot, newslot);
            }
        }
        else if (InPane(x, y))
        {
            // handle transfers from other panes
            if (EquipPane.GetHeldSlot() >= 0)
            {
                TObjectInstance* inst = ((TPlayer*)GetTopContainer())->GetEquip(EquipPane.GetHeldSlot());
                int32_t newslot = OnSlot(x, y);
                if (inst && newslot >= 0)
                {
                    if (GetTopContainer()->GetInventorySlot(newslot) == nullptr)
                    {
                        ((TPlayer*)GetTopContainer())->Equip(nullptr, EquipPane.GetHeldSlot());    // clear from eq list
                        inst->SetInventNum(newslot);                    // add to inventory
                        Update();
                    }
                }
            }
        }

        grabslot = -1;
        SetDragBitmap(nullptr);
        SetDragObj(nullptr);
        isdragging = false;

        if (InPane(x, y))
            Update();
    }
}

void TInventory::MouseMove(int32_t button, int32_t x, int32_t y)
{
    if (!container)
        return;

    if (!isdragging && InPane(x, y))
        if (button == MB_LEFTDOWN || button == MB_RIGHTDOWN)
        {
            // don't bother switching to drag mode until they actually move it
            if (absval(startposx - x) >= 2 || absval(startposy - y) >= 2)
            {
                TObjectInstance* inst = container->GetInventorySlot(grabslot);

                if (inst && inst->InventoryImage())
                {
                    int32_t grabx = (startposx - INVENTORYSTARTX) % INVITEMWIDTH;
                    int32_t graby = (startposy - INVENTORYSTARTY) % INVITEMHEIGHT;

                    if (!inst->InventoryImage()->OnPixel(grabx, graby))
                        grabx = graby = 20;

                    SetDragBitmap(inst->InventoryImage(), grabx, graby);
                    SetDragObj(inst);
                }

                isdragging = true;
                Update();
            }
        }
}

void TInventory::DrawAnim(TObjectInstance* inst, PTBitmap bm)
{
    if (!container || inst->GetOwner() != container)
        return;

    if (inst->InventNum() >= 256)
        EquipPane.DrawAnim(inst, bm);
    else
    {
        int32_t x = (inst->InventNum() % INVITEMSX) * INVITEMWIDTH;
        int32_t y = (inst->InventNum() / INVITEMSX) * INVITEMHEIGHT;
        x += INVENTORYSTARTX;
        y += INVENTORYSTARTY;

        int32_t sx, sy, sw, sh;
        Display->GetClipRect(sx, sy, sw, sh);
        SetClipRect();
        Display->Put(x, y, bm);
        Display->SetClipRect(sx, sy, sw, sh);
    }
}

int32_t TInventory::OnSlot(int32_t x, int32_t y)
{
    x -= INVENTORYSTARTX;
    y -= INVENTORYSTARTY;

    if (x < 0 || y < 0 ||
        (x / INVITEMWIDTH) >= INVITEMSX || (y / INVITEMHEIGHT) >= INVITEMSY ||
        (x % INVITEMWIDTH) >= INVITEMREALWIDTH || (y % INVITEMHEIGHT) >= INVITEMREALHEIGHT)
        return -1;
        
    return ((y / INVITEMHEIGHT) * INVITEMSX) + (x / INVITEMWIDTH);
}

void TInventory::SwapSlots(int32_t oldslot, int32_t newslot)
{
    TObjectInstance* inst0 = container->GetInventorySlot(oldslot);
    TObjectInstance* inst1 = container->GetInventorySlot(newslot);

    if (inst0)
        inst0->SetInventNum(newslot);

    if (inst1)
        inst1->SetInventNum(oldslot);
}
