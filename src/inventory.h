// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Inventory.h - TInventory Object                     *
// *************************************************************************

#pragma once

#include "revenant.h"
#include "screen.h"
#include "object.h"

// *******************************
// * TInventory - Inventory pane *
// *******************************

// Inventory for Our Hero which allows him to fool around with his possessions.

_CLASSDEF(TInventory)
class TInventory : public TPane
{
  public:
    TInventory() : TPane(INVENTORYPANEX, INVENTORYPANEY, INVENTORYPANEWIDTH, INVENTORYPANEHEIGHT) { container = nullptr; }
    ~TInventory() {}

    virtual bool Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Animate(bool draw);
    virtual void Show() { TPane::Show(); Update(); }
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);

    PTObjectInstance GetContainer() { return container; }
        // Container whose inventory contents are currently displayed
    PTObjectInstance GetTopContainer()
        { PTObjectInstance inst = container; while (inst->GetOwner()) inst = inst->GetOwner(); return inst; }
        // Master of inventory (usually Player)
    void SetContainer(PTObjectInstance cont)
        { if (container != cont) { container = cont; Update(); } }
        // Sets the object whose inventory the pane displays
    int32_t GetHeldSlot() { return heldslot; }
        // Function for transfering objects to other panes (namely, the equipment pane)

    void DrawAnim(PTObjectInstance inst, PTBitmap bm);
        // Draw an animation for an inventory object

  private:
    int32_t OnSlot(int32_t x, int32_t y);
        // Return slotnumber that pixel position x, y is on
    void SwapSlots(int32_t oldslot, int32_t newslot);
        // Swap the inventory objects in the slots specified

    PTObjectInstance container;         // Object inventory is shown for

    int32_t grabslot;                       // slot number currently dragging with mouse
    int32_t heldslot;                       // for passing objects to other panes
    bool isdragging;                    // whether object is in motion
    int32_t startposx, startposy;           // dragging
};


