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
#include "playscreen.h"
#include "key.h"
#include "dialog.h"
#include "rules.h"
#include "sound.h"
#include "spellpane.h"

#include <string>

REGISTER_BUILDER(TContainer)
TObjectClass ContainerClass("CONTAINER", OBJCLASS_CONTAINER, 0);

// Hard coded class stats
DEFSTAT(Container, Openable,    OPEN, 0, 0, 0, 1)
DEFSTAT(Container, Value,       VAL,  1, 0, 0, 1000000)

// Hard coded object stats
DEFOBJSTAT(Container, Locked,           LOCK, 0, 0, 0, 0)
DEFOBJSTAT(Container, KeyId,            KEY,  1, 0, 0, 0)
DEFOBJSTAT(Container, PickDifficulty,   PICK, 2, 0, 0, 0)
// REVSYNC: the rest as retail registers them (0x004dbf4a..0x004dc16c)
DEFOBJSTAT(Container, Explosive,        EXPL, 3, 0, 0, 1)
DEFOBJSTAT(Container, Poisonous,        POIS, 4, 0, 0, 1)
DEFOBJSTAT(Container, CombineID,        CBID, 5, 0, 0, 32)
DEFOBJSTAT(Container, TargetID,         TGID, 6, 0, 0, -1)
DEFOBJSTAT(Container, Regen,            RGEN, 7, 0, 0, 10000)

extern TObjectInstance* TakenObject;
extern TObjectInstance* DroppedObject;
extern TObjectClass EffectClass;

// Container states
#define CLOSED      0
#define OPEN        1

namespace {

// REVSYNC: the lid of a chest opening (0x004dc2a0..) or closing
// (0x004dcfa2..): the sound by the type name, for types with "Chest" in it:
// "Lab" LabChest, "ForChest" Stone, "Pot" Clay (no shipped type has both),
// else Treasure. Played as the item panes' sounds are, not at the chest.
void PlayLidSound(TObjectInstance* container, bool open)
{
    const char* type = container->GetTypeName();
    if (!type || !strstr(type, "Chest"))
        return;
    const char* kind = strstr(type, "Lab")        ? "LabChest"
                     : strstr(type, "ForChest")   ? "Stone"
                     : strstr(type, "Pot")        ? "Clay"
                     :                              "Treasure";
    char name[32];
    snprintf(name, sizeof(name), "%s%s", kind, open ? "Open" : "Close");
    const int32_t id = SoundPlayer.FindSound(name);
    if (id >= 0 && SoundPlayer.Mount(id))
        SoundPlayer.Play(id);
}

// The language's whole line `full`, with the names in its own order, when it
// has one. Only English ships, and it has none of them: the fallbacks below
// are what shows.
bool FullLine(std::string& line, const char* full, const std::string& a, const std::string& b = {})
{
    if (DialogList.FindLine(full) < 0)
        return false;
    char buf[256];
    snprintf(buf, sizeof(buf), DialogList.GetLine(full), a.c_str(), b.c_str());
    line = buf;
    return true;
}

// "<item> <tag> <container>." unless the language words it otherwise
// (FULL... with the item first, FULL...REV with the container first).
std::string ItemAndContainerLine(const char* full, const char* fullrev, const char* tag,
                                 const std::string& item, const std::string& container)
{
    std::string line;
    if (!FullLine(line, full, item, container) && !FullLine(line, fullrev, container, item))
        line = item + " " + DialogList.GetLine(tag) + " " + container + ".";
    return line;
}

void Say(TObjectInstance* user, const std::string& line)
{
    if (user == Player)
        TextBar.Print("%s", line.c_str());
}

// A trap's effect over the container (0x004dc434.., 0x004dc605..). A type
// the effects don't have spawns nothing, as retail's NewObject failed.
void SpawnTrapEffect(TObjectInstance* container, const char* type, int32_t facing)
{
    SObjectDef def{};
    def.objclass = OBJCLASS_EFFECT;
    def.objtype = short(EffectClass.FindObjType(type));
    if (def.objtype < 0)
        return;
    def.flags = OF_MOVING;
    def.level = uint16_t(MapPane.GetMapLevel());
    def.pos = container->Pos();
    def.pos.z += 50;
    def.facing = uint8_t(facing);
    MapPane.NewObject(&def);
}

} // namespace

// REVSYNC: TContainer::Use @ 0x004dc180 (INVCONTAINER's too): the scripts
// (the base Use), a key or pick on it shut, CONTISLOCKED, then opening it
// (the lid sound, the traps, CONTOPENED; an item used on it shut: "The X is
// closed."), then UseCarried when the user carries it, else TakeOut or
// PutIn. Only the main player reads the lines. Not ported (multiplayer):
// the network messages and the client's gate.
bool TContainer::Use(TObjectInstance* user, int32_t with)
{
    TObjectInstance* inst = MapPane.GetInstance(with);

    if (TObjectInstance::Use(user, with))
        return true;

    if (state == CLOSED && CheckKeyUse(user, inst))
        return true;

    if (Locked())
    {
        Say(user, DialogList.GetLine("CONTISLOCKED"));
        return false;
    }

    if (Openable() && state == CLOSED)
    {
        const std::string name = DialogList.DisplayName(GetName());
        std::string line;
        if (inst)
        {
            if (!FullLine(line, "FULLCONTCLOSED", name))
                line = std::string(DialogList.GetLine("BASETHE")) + " " + name + " " +
                       DialogList.GetLine("CONTISCLOSED");
            Say(user, line);
            return false;
        }

        SetState(OPEN);
        PlayLidSound(this, true);
        SpringTraps(user);
        if (!FullLine(line, "FULLCONTOPEN", name))
            line = name + " " + DialogList.GetLine("CONTOPENED");
        Say(user, line);
        return Finish();
    }

    if (user && GetTopOwner() == user)
        return UseCarried(user, inst);
    return inst ? PutIn(user, inst) : TakeOut(user);
}

// REVSYNC: 0x004dcac0..0x004dd23a -- a container on the ground (or someone
// else's) used alone: its first item goes to the inventory the pane shows
// (an open bag, when one is), one item a click; CONTFULL when that has no
// room. A Regen container hands out copies of its items in turn and, after
// the last, rests Regen seconds. Empty or resting, it shuts. Only the main
// player takes: retail sent anyone else's take to the network.
bool TContainer::TakeOut(TObjectInstance* user)
{
    const int32_t regen = Regen();
    TInventoryIterator it(this);
    for (int32_t k = 0; regen > 0 && k < regencursor && it; ++k)
        it++;
    TObjectInstance* const item = it.Item();
    if (!item || (regen > 0 && PlayScreen.GameFrame() - regentime < regen * FRAMERATE))
        return Shut(user);

    TObjectInstance* const into = Inventory.GetContainer();
    if (!into || into->FindFreeInventorySlot() > kInvSlotLastCarried)
    {
        Say(user, DialogList.GetLine("CONTFULL"));
        return Finish();
    }

    // A dispenser gives a copy: amount and object stats too.
    TObjectInstance* given = item;
    if (regen > 0)
    {
        TObjectClass* const cl = TObjectClass::GetClass(item->ObjClass());
        SObjectDef def{};
        def.objclass = short(item->ObjClass());
        def.objtype = short(item->ObjType());
        given = cl ? cl->NewObject(&def) : nullptr;
        if (!given)
            return false;
        if (item->Amount() > 1)
            given->SetAmount(item->Amount());
        for (int32_t s = 0; s < item->NumObjStats(); ++s)
            given->SetObjStat(s, item->GetObjStat(s));
        it++;
        if (it)
            ++regencursor;
        else
        {
            regencursor = 0;
            regentime = PlayScreen.GameFrame();
        }
    }

    const TSafeRef<TObjectInstance> taken(item);     // gold or food may merge into a pile, deleted
    if (user == Player)
    {
        Say(user, ItemAndContainerLine("FULLCONTTAKEN", "FULLCONTTAKENREV", "CONTTAKENFROM",
                                       DialogList.DisplayName(given->GetName()),
                                       DialogList.DisplayName(GetName())));
        if (!into->AddToInventory(given, -1) && given != item)
            delete given;
    }
    else if (given != item)
        delete given;
    TakenObject = taken.Get();
    return Finish();
}

// REVSYNC: 0x004dd23a..0x004dd33f -- an item used on a container on the
// ground goes in, room or not (an ingredient takes nothing): "<item> put in
// <container>.".
bool TContainer::PutIn(TObjectInstance* user, TObjectInstance* inst)
{
    if (CombineID())
        return Finish();
    const std::string line = ItemAndContainerLine("FULLCONTPUTIN", "FULLCONTPUTINREV", "CONTPUTIN",
                                                  DialogList.DisplayName(inst->GetName()),
                                                  DialogList.DisplayName(GetName()));
    const TSafeRef<TObjectInstance> dropped(inst);   // gold or food may merge into a pile, deleted
    AddToInventory(inst, -1);
    Say(user, line);
    DroppedObject = dropped.Get();
    return Finish();
}

// REVSYNC: 0x004dcf6c..0x004dd03b -- an empty (or resting) container on the
// ground shuts, with its lid sound and "<container> closed."; one that
// doesn't open does nothing.
bool TContainer::Shut(TObjectInstance* user)
{
    if (!Openable())
        return false;
    SetState(CLOSED);
    PlayLidSound(this, false);
    const std::string name = DialogList.DisplayName(GetName());
    std::string line;
    if (!FullLine(line, "FULLCONTCLOSED", name))
        line = name + " " + DialogList.GetLine("CONTCLOSED");
    Say(user, line);
    return Finish();
}

// REVSYNC: 0x004dc434..0x004dc7ba -- the traps a container springs on
// whoever opens it, every time (nothing disarms them). Explosive: a
// "fireflash" over it, facing the main player; the user knocked back, burnt
// and hurt 5-15 burn damage. Poisonous: a "poisoncloud" (no effect of that
// name ships, so nothing shows); the user flails, is poisoned and hurt 5-15
// of retail's damage type 20 (1998's DAMAGE_POISON, which retail reads as
// physical). Deviations: retail knocked the user back from a point it never
// set and turned him by a sum of the container's and his own coordinates;
// the port knocks him back from the container and lets KnockBack turn him.
void TContainer::SpringTraps(TObjectInstance* user)
{
    if (!user || !user->IsCharacter())
        return;
    auto* const who = static_cast<TCharacter*>(user);
    if (Explosive())
    {
        SpawnTrapEffect(this, "fireflash", Player ? AngleTo(Player) : 0);
        who->KnockBack(Pos(), -1);
        who->Burn();
        who->Damage(random(5, 15), DT_BURN);
    }
    if (Poisonous())
    {
        SpawnTrapEffect(this, "poisoncloud", 0);
        who->Flail();
        who->SetPoisoned(1);
        who->Damage(random(5, 15), DAMAGE_POISON);
    }
}

// REVSYNC: 0x004dd33f -- a container without an animator has finished
// what it was doing.
bool TContainer::Finish()
{
    if (!GetAnimator())
        SetCommandDone(true);
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
    if (!inst && (slot < 0xff || slot > kInvSlotBeltFirst) && !CombineID())
    {
        if (user == Player)
        {
            TObjectInstance* const top = GetTopOwner();
            Inventory.SetContainer(Inventory.GetContainer() == this && top ? top : this);
        }
        return Finish();
    }

    if (CanCombine(inst))
    {
        // The made item goes to the owner of the one whose TargetID names it.
        // Deviation: retail only took the two out of their inventories, so
        // they were left nowhere; the port deletes them.
        TObjectInstance* const maker = GetOwner() && TargetID() ? this : inst;
        TObjectInstance* const owner = maker->GetOwner();
        const uint32_t target = static_cast<uint32_t>(maker->GetStat("TargetID"));
        if (owner && target && owner->AddToInventoryByUniqueId(target, 1, -1))
        {
            inst->RemoveFromInventory();
            delete inst;
            RemoveFromInventory();
            delete this;    // as a food's use-up deletes the food Use runs on
            return true;
        }
        return Finish();
    }

    const int32_t target = TargetID();
    if (inst && RealNumInventoryItems() < 0xfe && (target == -1 || target == 0 || target == 1) &&
        !CombineID() && FindFreeInventorySlot() < 0xff)
    {
        AddToInventory(inst, -1);
        if (user == Player)
        {
            const int32_t id = SoundPlayer.FindSound("pouch");
            if (id >= 0 && SoundPlayer.Mount(id))
                SoundPlayer.Play(id);
        }
    }
    return Finish();
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

bool TContainer::IsOpen() const
{
    return state == OPEN;
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