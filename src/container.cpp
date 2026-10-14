// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  container.cpp - TContainer module                    *
// *************************************************************************

#include "revenant.h"
#include "container.h"
#include "mappane.h"
#include "inventory.h"
#include "textbar.h"
#include "tool.h"
#include "player.h"
#include "key.h"
#include "dialog.h"
#include "sound.h"
#include "spellpane.h"

REGISTER_BUILDER(TContainer)
TObjectClass ContainerClass("CONTAINER", OBJCLASS_CONTAINER, 0);

// Hard coded class stats
DEFSTAT(Container, Openable,    OPEN, 0, 0, 0, 1)
DEFSTAT(Container, Value,       VAL,  1, 0, 0, 1000000)

// Hard coded object stats
DEFOBJSTAT(Container, Locked,           LOCK, 0, 0, 0, 0)
DEFOBJSTAT(Container, KeyId,            KEY,  1, 0, 0, 0)
DEFOBJSTAT(Container, PickDifficulty,   PICK, 2, 0, 0, 0)

extern TObjectInstance* TakenObject;
extern TObjectInstance* DroppedObject;

// Container states
#define CLOSED      0
#define OPEN        1

// REVSYNC: TContainer::Use @ 0x004dc180 (INVCONTAINER's too): the scripts
// (the base Use), a key or pick on it shut, CONTISLOCKED, then opening it,
// then UseCarried when the user carries it. Not yet retail's: opening (its
// sounds by type, the Explosive and Poisonous traps and the dialog lines,
// 0x004dc22a..0x004dc87d) and the half on the ground (0x004dcac0..).
bool TContainer::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);

    if (TObjectInstance::Use(user, with))
        return true;

    if (state == CLOSED && CheckKeyUse(user, inst))
        return true;

    if (Locked())
    {
        if (user == Player)
            TextBar.Print("%s", DialogList.GetLine("CONTISLOCKED"));
        return false;
    }

    char buf[80];

    if (Openable() && state == CLOSED)
    {
        if (inst)
        {
            sprintf(buf, "The %s is closed.", GetName());
            TextBar.Print("%s", buf);
            return false;
        }

        SetState(OPEN);
        
        sprintf(buf, "%s opened.", GetName());
        TextBar.Print("%s", buf);
        return true;
    }

    if (user && GetTopOwner() == user)
        return UseCarried(user, inst);

    // On the ground
    if (!inst)
    {
        // get from
        TInventoryIterator i(this);
        TObjectInstance* oi = i.Item();

        if (oi)
        {
            if (Inventory.GetContainer() && (uint32_t)Inventory.GetContainer()->FindFreeInventorySlot() < MAXINVITEMS)
            {
                // Gold or food may merge into a pile and be deleted
                snprintf(buf, sizeof(buf), "%s taken from %s.", oi->GetName(), GetName());
                const TSafeRef<TObjectInstance> taken(oi);
                oi->RemoveFromInventory();
                Inventory.GetContainer()->AddToInventory(oi);

                TextBar.Print("%s", buf);

                TakenObject = taken.Get();
            }
            else
                TextBar.Print("Can't carry any more.");
        }
        else
        {
            if (!Openable())
                return false;

            SetState(CLOSED);
            sprintf(buf, "%s closed.", GetName());
            TextBar.Print("%s", buf);
        }
    }
    else
    {
        // add to (gold or food may merge into a pile and be deleted)
        snprintf(buf, sizeof(buf), "%s put in %s.", inst->GetName(), GetName());
        const TSafeRef<TObjectInstance> dropped(inst);
        inst->RemoveFromInventory();
        AddToInventory(inst);

        TextBar.Print("%s", buf);

        DroppedObject = dropped.Get();
    }

    return true;
}

// REVSYNC: 0x004dc880..0x004dcabb. Alone, a bag shows in the main player's
// inventory pane, or, shown already, the inventory at its top does (retail
// skips a bag in slots 0xff..0x10b, the equipment and the first belt box, as
// read; an ingredient, with a CombineID, never opens). With an item it makes
// what their TargetID names (CanCombine), or takes the item when it has room
// and isn't an ingredient ("pouch"). True even when nothing happened, so the
// panes never swap an item with a bag.
bool TContainer::UseCarried(TObjectInstance* user, TObjectInstance* inst)
{
    const int32_t slot = InventNum();
    if (!inst && (slot < 0xff || slot > kInvSlotBeltFirst) && !GetStat("CombineID"))
    {
        if (user == Player)
        {
            TObjectInstance* const top = GetTopOwner();
            Inventory.SetContainer(Inventory.GetContainer() == this && top ? top : this);
        }
        return true;
    }

    if (CanCombine(inst))
    {
        // The made item goes to the owner of the one whose TargetID names it.
        // Deviation: retail only took the two out of their inventories, so
        // they were left nowhere; the port deletes them.
        TObjectInstance* const maker = GetOwner() && GetStat("TargetID") ? this : inst;
        TObjectInstance* const owner = maker->GetOwner();
        const uint32_t target = static_cast<uint32_t>(maker->GetStat("TargetID"));
        if (owner && target && owner->AddToInventoryByUniqueId(target, 1, -1))
        {
            inst->RemoveFromInventory();
            delete inst;
            RemoveFromInventory();
            delete this;    // as a food's use-up deletes the food Use runs on
        }
        return true;
    }

    const int32_t target = GetStat("TargetID");
    if (inst && RealNumInventoryItems() < 0xfe && (target == -1 || target == 0 || target == 1) &&
        !GetStat("CombineID") && FindFreeInventorySlot() < 0xff)
    {
        AddToInventory(inst, -1);
        if (user == Player)
        {
            const int32_t id = SoundPlayer.FindSound("pouch");
            if (id >= 0 && SoundPlayer.Mount(id))
                SoundPlayer.Play(id);
        }
    }
    return true;
}

// REVSYNC: CursorType @ 0x004dd370 -- the hand for a container that opens or
// holds something, unless it's invisible; what's held over it doesn't matter.
int32_t TContainer::CursorType(TObjectInstance* inst)
{
    if ((Openable() || NumObjects() > 0) && !(GetFlags() & OF_INVISIBLE))
        return CURSOR_HAND;

    return CURSOR_NONE;
}

int32_t TContainer::NumObjects()
{
    return RealNumInventoryItems();
}

bool TContainer::CanTrade()
{
    return !(ObjClass() == OBJCLASS_INVCONTAINER && !stricmp(GetName(), "Swag Bag"));
}

// REVSYNC: TContainer::Load @ 0x004dd3e0 (SAVE_GAME.md §11.3).
void TContainer::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    TObjectInstance::Load(is, version, objversion);

    if (version >= 2 && version < 5) // We're using object stats now
    {
        int32_t contflags, pickdifficulty;
        is >> contflags >> pickdifficulty;
        SetStat("Locked", contflags != 0);
        SetStat("PickDifficulty", pickdifficulty);
    }

  // A container in the world re-enters its saved state (open or shut).
    if (!(flags & OF_INVENTORY))
    {
        SetCommandDone(true);
        SetState(state);
    }
}

void TContainer::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

// REVSYNC: CheckKeyUse @ 0x004dd480 -- a key or lockpick used on this locked
// container or door. True for any key or pick attempt, whatever came of it;
// only the main player hears and reads about it. A lock of difficulty 0
// can't be picked; a successful pick earns lockpicking experience.
bool TContainer::CheckKeyUse(TObjectInstance* user, TObjectInstance* inst)
{
    if (!inst || !Locked())
        return false;

    auto report = [user](const char *sound, const char *line, bool keys) {
        if (user != Player)
            return;
        PLAY(sound);
        TextBar.Print("%s", DialogList.GetLine(line));
        if (keys)
            PLAY("keys");
    };

    if (inst->ObjClass() == OBJCLASS_KEY)
    {
        if (static_cast<TKey*>(inst)->KeyId() == KeyId())
        {
            report("unlock succeed", "CONTUNLOCKED", false);
            SetLocked(false);
        }
        else
            report("unlock failed", "CONTWRONGKEY", false);
        return true;
    }

    if (inst->ObjClass() == OBJCLASS_TOOL)
    {
        int32_t abil = static_cast<TTool*>(inst)->Pick();
        if (abil <= 0)
            return false;

        TPlayer* player = user && user->ObjClass() == OBJCLASS_PLAYER ? static_cast<TPlayer*>(user) : nullptr;
        if (player)
            abil += player->Skill(SK_LOCKPICK) + player->Agil();

        if (PickDifficulty() == 0)
            report("unlock fail", "CONTPICKFAIL", true);
        else if (abil < PickDifficulty())
            report("unlock fail", "CONTPICKTOUGH", true);
        else if (random(0, abil) < PickDifficulty())
            report("unlock fail", "CONTPICKFAIL", true);
        else
        {
            report("unlock succeed", "CONTUNLOCK", true);
            SetLocked(false);
            if (player)
                player->AddSkillExp(SK_LOCKPICK, 50);
        }
        return true;
    }

    return false;
}


// *************
// * TVialRack *
// *************

_CLASSDEF(TVialRack)
class TVialRack : public TContainer
{
  public:
    TVialRack(TObjectImagery* newim) : TContainer(newim) {}
    TVialRack(SObjectDef* def, TObjectImagery* newim) : TContainer(def, newim) {}

    virtual bool Use(TObjectInstance* user, int32_t with = -1);
    int32_t CursorType(TObjectInstance* inst = nullptr) override;
    virtual void Save(RTOutputStream os);
};

DEFINE_BUILDER("VIAL RACK", TVialRack)
REGISTER_BUILDER(TVialRack)

bool TVialRack::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);

    if (!inst)
    {
        if (state > 0)
        {
            if ((uint32_t)Inventory.GetContainer()->FindFreeInventorySlot() >= MAXINVITEMS)
                TextBar.Print("Can't carry any more.");
            else
            {
                // generate a new vial
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_CONTAINER;
                def.objtype = ContainerClass.FindObjType("Empty Vial");
                GetPos(def.pos);
                int32_t index = MapPane.NewObject(&def);

                TakenObject = MapPane.GetInstance(index);
                Inventory.GetContainer()->AddToInventory(TakenObject);

                TextBar.Print("Poison Vial taken from Vial Rack.");
                SetState(state - 1);

                return true;
            }
        }
    }
    else
    {
        if (strcmp(inst->GetName(), "Poison Empty Vial") == 0 && state < 4)
        {
            // Originally I just deleted the vial here, but this caused problems
            // in shop scripts.  Now I just stash it into the rack, which clears itself
            // out when it saves to disk.

            //MapPane.RemoveObject(with);
            //delete inst;

            inst->RemoveFromInventory();
            AddToInventory(inst);

            TextBar.Print("Poison Vial put in Vial Rack.");
            SetState(state + 1);

            DroppedObject = inst;

            return true;
        }
    }

    return false;
}

int32_t TVialRack::CursorType(TObjectInstance* inst)
{
    if (state > 0 && (!inst || (state < 4 && strcmp(inst->GetName(), "Poison Vial") == 0)))
        return CURSOR_HAND;

    return CURSOR_NONE;
}

void TVialRack::Save(RTOutputStream os)
{
    inventory.DeleteAll();

    TContainer::Save(os);
}


bool TContainer::AddToInventory(TObjectInstance* inst, int32_t slot)
{
    bool ret = TObjectInstance::AddToInventory(inst, slot);
    if (stricmp(GetTypeName(), "Spell Pouch") == 0 || stricmp(GetTypeName(), "SpellPouch") == 0)
        SpellPane.Update();

    return ret;
}