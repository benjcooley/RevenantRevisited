// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.cpp - TTalisman module                     *
// *************************************************************************

#include "talisman.h"

#include "spellpane.h"

REGISTER_BUILDER(TTalisman)

TObjectClass TalismanClass("TALISMAN", OBJCLASS_TALISMAN, 0);

// Hard coded class stats
DEFSTAT(Talisman, Code,     CODE, 0, 0, 0, 255)  // Character code for spells

// REVSYNC: 0x00523a70 (vtable 0x5b619c, slot 0x60). A talisman leaving a
// spell pouch refreshes the spell pane; one in no inventory leaves nothing.
void TTalisman::RemoveFromInventory()
{
    if (owner && (stricmp(owner->GetTypeName(), "Spell Pouch") == 0 ||
                  stricmp(owner->GetTypeName(), "SpellPouch") == 0))
        SpellPane.Update();

    TObjectInstance::RemoveFromInventory();
}