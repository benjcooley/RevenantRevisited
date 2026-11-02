// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   inventorycomponent.h - what a container holds                       *
// *************************************************************************
//
// Retail kept a container's items in a pointer array inside every object.
// They are a component now, added the first time something is put in
// and kept for life, reached through a plain pointer. The array keeps
// retail's semantics (TPointerArray: Add takes the first empty slot,
// Remove leaves a hole, the count is the high-water mark), which the
// sector stream's inventory order and the saved invindex depend on. The
// items themselves are objects owned by their nodes; the container's
// destructor deletes them before its components go.
//
// *************************************************************************

#pragma once

#include "engine/scene/component.h"
#include "engine/core/revtypes.h"

_CLASSDEF(TObjectInstance)

class TInventoryComponent final : public TComponent
{
  public:
    [[nodiscard]] const char* ComponentName() const override { return "inventory"; }

    int32_t Add(TObjectInstance* item) { return items_.Add(item); }
        // The index the item took: the first empty slot
    void Remove(int32_t index) { if ((uint32_t)index < (uint32_t)items_.NumItems()) items_.Remove(index); }
    [[nodiscard]] TObjectInstance* At(int32_t index) const
        { return (uint32_t)index < (uint32_t)items_.NumItems() ? items_[index] : nullptr; }
    [[nodiscard]] int32_t Count() const { return items_.NumItems(); }
        // Slots in use up to the last one, holes included
    void Clear() { items_.Clear(); }
        // Forgets the items (they are deleted by whoever owns their nodes)

  private:
    TPointerArray<TObjectInstance, 0, 4> items_;
};
