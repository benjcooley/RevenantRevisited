// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Object.cpp - TObject module                       *
// *************************************************************************

#include "object.h"

#include "3dscene.h"
#include "command.h"
#include "display.h"
#include "dls.h"
#include "file.h"
#include "inventory.h"
#include "logging.h"
#include "mappane.h"
#include "parse.h"
#include "player.h"
#include "playscreen.h"
#include "resource.h"
#include "revutils.h"
#include "script.h"
#include "sound.h"
#include "stream.h"

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

namespace {

int32_t MapIndexPreservePriority(const TObjectInstance* oi)
{
    if (!oi)
        return 0;
    switch (oi->ObjClass())
    {
        case OBJCLASS_EXIT:
            return 100;
        case OBJCLASS_PLAYER:
        case OBJCLASS_CHARACTER:
            return 90;
        case OBJCLASS_CONTAINER:
        case OBJCLASS_INVCONTAINER:
            return 80;
        case OBJCLASS_ITEM:
        case OBJCLASS_WEAPON:
        case OBJCLASS_ARMOR:
        case OBJCLASS_TALISMAN:
        case OBJCLASS_FOOD:
        case OBJCLASS_LIGHTSOURCE:
        case OBJCLASS_TOOL:
        case OBJCLASS_MONEY:
        case OBJCLASS_KEY:
        case OBJCLASS_POTION:
        case OBJCLASS_AMMO:
        case OBJCLASS_SCROLL:
        case OBJCLASS_RANGEDWEAPON:
        case OBJCLASS_MAPSCROLL:
            return 70;
        case OBJCLASS_TRAP:
        case OBJCLASS_EFFECT:
        case OBJCLASS_HELPER:
            return 60;
        case OBJCLASS_TILE:
        case OBJCLASS_SHADOW:
            return 10;
        default:
            return 50;
    }
}

int32_t FreshRuntimeMapIndex()
{
    int32_t fresh = -1;
    do {
        fresh = MapPane.MakeIndex();
    } while (fresh < 0 || LookupMapIndex(fresh) != nullptr);
    return fresh;
}

struct SComponentUpdateEntry
{
    TSafeComponentRef<TObjectComponent> component;
    TObjectComponentUpdateMethod method = nullptr;
};

std::vector<SComponentUpdateEntry> g_component_updates;

} // namespace

// Declarations of global arrays for object classes, imagery, and builders

int32_t TObjectClass::numclasses = 0;
TObjectClass* TObjectClass::classes[MAXOBJECTCLASSES];

// true if classes need be be resaved
bool TObjectClass::classesdirty = false;

int32_t TObjectBuilder::numobjtypes = 0;
TObjectBuilder* TObjectBuilder::builders[MAXOBJECTTYPES];


// Tables for ConvertToFacing, Move, and other angle/distance related functions
extern uint8_t AngleTable[256][256];
extern uint8_t DistTable[256][256];
extern short DistX[256];
extern short DistY[256];

extern uint32_t ImageryMemUsage;

uint16_t UniqueTypeID;

#define WORLDZOFFSET 0x3900 + 0x7FFF

#define MYSTERYVAL1     (867)
#define MYSTERYVAL2     (1000)

// *******************************
// * Map/Screen Conversion Utils *
// *******************************

void WorldToScreen(const S3DPoint& pos, int32_t &x, int32_t &y)
{
    // 2:1
    x = pos.x - pos.y;
    y = ((pos.x + pos.y) / 2) - (int32_t)((pos.z * MYSTERYVAL1) / MYSTERYVAL2);
        // the screen z is actually pos.z * cos(30), cos(30) = .866
}

void WorldToScreen(const S3DPoint& pos, S3DPoint& spos)
{
    // 2:1
    spos.x = pos.x - pos.y;
    spos.y = ((pos.x + pos.y) / 2) - (int32_t)((pos.z * MYSTERYVAL1) / MYSTERYVAL2);
    spos.z = (uint16_t)(WORLDZOFFSET - (int32_t)(pos.z / 2) - (int32_t)((pos.x + pos.y) * MYSTERYVAL1 / MYSTERYVAL2));
    //spos.z = (uint16_t)(WORLDZOFFSET - (int32_t)(pos.z * 2560 / 5120) - (int32_t)((pos.x + pos.y) * 4434 / 5120));
        // Z must be figured out based on the plates being tilted at a 30 degree angle
        // zratio = 512.0 * sin(30) = 256.0, yratio = 512.0 * cos(30) = 443.4
}

void WorldToScreenZ(const S3DPoint& pos, int32_t &z)
{
    z = (uint16_t)(WORLDZOFFSET - (int32_t)(pos.z / 2) - (int32_t)((pos.x + pos.y) * MYSTERYVAL1 / MYSTERYVAL2));
}

void ScreenToWorld(S3DPoint& pos, const S3DPoint& spos)
{
    pos.z = (-3464 * spos.y / 1000 - 2 * spos.z) / 4;
    pos.y = - spos.x / 2 + MYSTERYVAL1 * pos.z / MYSTERYVAL2 + spos.y;
    pos.x = spos.x + pos.y;
}

void ScreenToWorld(int32_t x, int32_t y, S3DPoint& pos, int32_t zheight)
{
    if (zheight)
        y += (int32_t)((zheight * MYSTERYVAL1) / MYSTERYVAL2);

    // 2:1
    pos.x = (x / 2) + y;
    pos.y = y - (x / 2);
    pos.z = zheight;
}

void ConvertToVector(int32_t angle, int32_t speed, S3DPoint& vect, int32_t zangle)
{
    if (zangle == 0)
    {
        vect.x = (DistX[angle] * speed) / 256;
        vect.y = (DistY[angle > 128 ? angle - 128 : 128 - angle] * speed) / 256;
        vect.z = 0;
    }
    else
    {
        ConvertToVector(angle, (DistX[zangle] * speed) / 256, vect, 0);
        vect.z = (DistY[zangle] * speed) / 256;
    }
}

int32_t ConvertToFacing(const S3DPoint& target)
{
    int32_t absx = absval(target.x);
    int32_t absy = absval(target.y);

    while (absx > 255 || absy > 255)
    {
        absx >>= 1;
        absy >>= 1;
    }

    int32_t angle = 64 - AngleTable[absx][absy];

    // adjust for sign (which of the quadrants does the angle fall into?)
    if (target.x < 0 && target.y > 0)
        angle += 128;
    else if (target.x < 0)
        angle = 256 - angle;
    else if (target.y > 0)
        angle = 128 - angle;

    return angle;
}

int32_t ConvertToFacing(const S3DPoint& pos, const S3DPoint& target)
{
    S3DPoint p;

    p.x = target.x - pos.x;
    p.y = target.y - pos.y;
    p.z = 0;

    return ConvertToFacing(p);
}

int32_t ConvertZToFacing(S3DPoint& target)
{
    int32_t absdist = abs(Distance(target));
    int32_t absz = abs(target.z);

    while (absdist > 255 || absz > 255)
    {
        absdist >>= 1;
        absz >>= 1;
    }

    int32_t angle = AngleTable[absdist][absz];

    // adjust for sign (which of the quadrants does the angle fall into?)
    if (target.z < 0 && target.y < 0)
        angle += 128;
    else if (target.z < 0)
        angle = 256 - angle;
    else if (target.y < 0)
        angle = 128 - angle;

    return angle;
}

int32_t ConvertZToFacing(const S3DPoint& pos, const S3DPoint& target)
{
    S3DPoint p;

    p.x = target.x - pos.x;
    p.y = target.y - pos.y;
    p.z = target.z - pos.z;

    return ConvertZToFacing(p);
}

int32_t Distance(const S3DPoint& pos)
{
    int32_t numshifts = 0;
    int32_t x = abs(pos.x);
    int32_t y = abs(pos.y);

    while (x > 255 || y > 255)
    {
        x >>= 1;
        y >>= 1;
        numshifts++;
    }

    int32_t d = (int32_t)DistTable[x][y];
    d <<= numshifts;

    return d;
}

int32_t Distance(const S3DPoint& pos, const S3DPoint& target)
{
    S3DPoint p;

    p.x = abs(target.x - pos.x);
    p.y = abs(target.y - pos.y);
    p.z = 0;

    return Distance(p);
}

int32_t AngleDiff(int32_t angle1, int32_t angle2)
{
    int32_t diff = (angle1 & 255) - (angle2 & 255);
    if (diff > 128)
        diff -= 255;
    else if (diff <= -128)
        diff += 255;
    return diff;
}

// ******************
// * TObjectBuilder *
// ******************

TObjectBuilder::TObjectBuilder(const char *name)
{
    if (numobjtypes < MAXOBJECTTYPES)
        builders[numobjtypes++] = this;
    else
        log_warn("[object] object builder registry full (%d), dropping builder '%s'",
                 MAXOBJECTTYPES, name ? name : "?");

    objtypename = _strdup(name);
}

TObjectBuilder* TObjectBuilder::GetBuilder(int32_t objtype)
{
    if (objtype < numobjtypes)
        return builders[objtype];

    return nullptr;
}

TObjectBuilder* TObjectBuilder::GetBuilder(const char *name)
{
    for (int32_t i = 0; i < numobjtypes; i++)
        if (stricmp(name, builders[i]->objtypename) == 0)
            return builders[i];

    return nullptr;
}

// **********************
// * TInventoryIterator *
// **********************

TObjectInstance* TInventoryIterator::NextItem()
{
    // Currently this DOES NOT recurse into other object's inventories,
    // because nothing uses it that way.  Copying some code from TMapIterator
    // would make it possible to do so if it is ever needed.
    item = nullptr;

    if (owner)
    {
        do
        {
            if (invindex >= owner->NumInventoryItems())
                break;

            item = owner->GetInventory(invindex++);

        } while (!item);
    }

    return item;
}

const TObjectInstance* TConstInventoryIterator::NextItem() const
{
    item = nullptr;

    if (owner)
    {
        do
        {
            if (invindex >= owner->NumInventoryItems())
                break;

            item = owner->GetInventory(invindex++);

        } while (!item);
    }

    return item;
}

// *******************
// * TObjectInstance *
// *******************

void TObjectComponent::RegisterUpdate(TObjectComponentUpdateMethod method)
{
    if (!method || !Owner())
        return;

    for (const SComponentUpdateEntry& entry : g_component_updates)
        if (entry.method == method && entry.component.Get() == this)
            return;

    SComponentUpdateEntry entry = {};
    entry.component.Set(this);
    entry.method = method;
    g_component_updates.push_back(entry);
}

void TObjectComponent::UnregisterUpdate(TObjectComponentUpdateMethod method)
{
    for (size_t i = 0; i < g_component_updates.size(); )
    {
        TObjectComponent* component = g_component_updates[i].component.Get();
        if (!component || (component == this && (!method || g_component_updates[i].method == method)))
            g_component_updates.erase(g_component_updates.begin() + ptrdiff_t(i));
        else
            ++i;
    }
}

void TObjectComponent::RunUpdateList()
{
    for (size_t i = 0; i < g_component_updates.size(); )
    {
        SComponentUpdateEntry entry = g_component_updates[i];
        TObjectComponent* component = entry.component.Get();
        if (!component || !entry.method)
        {
            g_component_updates.erase(g_component_updates.begin() + ptrdiff_t(i));
            continue;
        }

        (component->*entry.method)();
        ++i;
    }
}

void TObjectInstance::ClearObject()
{
    SetNotify(N_SCRIPTADDED);

    animator            = nullptr;
    lightdef.lightindex = -1;
    lightdef.lightid    = -1;
    lightdef.multiplier = -1;
    SetCommandDone(false);
    script              = nullptr;

    objclass = objtype  = -1;
    flags               = 0;
    notifyflags         = 0;
    sector              = nullptr;
    state               = 0;
    group               = 0;

    frame               = 0;
    framerate           = 1;

    level = 0;
    pos.x = pos.y = pos.z = 0;
    vel.x = vel.y = vel.z = 0;
    rotatex = rotatey = rotatez = 0;
    accum.x = accum.y = accum.z = 0;
    invindex = inventnum = -1;
    shadow = -1;
    mapindex = -1;

    owner = nullptr;

    moveangle = movevert = movedist = 0;
    movebits = 0;

    stats.Clear();

    // make oldpos DIFFERENT from pos here
    oldpos.x = pos.x + 1; oldpos.y = pos.y + 1; oldpos.z = pos.z + 1;
    screenx = screeny = screenz = 0;
}

TObjectInstance::TObjectInstance(TObjectImagery* img)
{
    ClearObject();
    imagery = img;

    if (imagery->NeedsAnimator(this))
        flags |= OF_ANIMATE;

    if (ObjClass() != OBJCLASS_TILE && ObjClass() != OBJCLASS_SHADOW)
        flags |= OF_PULSE;
}

TObjectInstance::TObjectInstance(SObjectDef* def, TObjectImagery* img)
{
    ClearObject();

    memcpy(&objclass, def, sizeof(SObjectDef));

  // Set imagery
    imagery = img;

  // Set ANIMATE and PULSE flags
    if (imagery->NeedsAnimator(this))
        flags |= OF_ANIMATE;

    if (ObjClass() != OBJCLASS_TILE)
        flags |= OF_PULSE;

    if (flags & OF_LIGHT)
        flags = flags | OF_ANIMATE | OF_PULSE;
    
  // Set Object info pointer
    cl = TObjectClass::GetClass(objclass);
    if (!cl)
        FatalError("Bad object class!"); // This should never happen!!
    inf = cl->GetObjType(objtype);
    name = inf->name;

  // Setup stat array
    if (cl->NumObjStats() > 0)
    {
        stats.SetNumItems(cl->NumObjStats());
        for (int32_t c = 0; c < cl->NumObjStats(); c++)
            ResetObjStat(c);
    }

    if (objclass != OBJCLASS_TILE) // No scripts for regular tiles (for efficiency sake)
        InitScript(ScriptManager.ObjectScript(this));
}

void TObjectInstance::SetMapIndex(int32_t newindex)
{
    // Keep the MapPane mapindex → instance registry in sync. Holders of a
    // mapindex resolve via MapPane.GetInstance(), so any reassignment or
    // clear-to-negative must move the registry entry with the instance.
    if (newindex == mapindex)
        return;
    if (newindex >= 0)
    {
        if (TObjectInstance* existing = LookupMapIndex(newindex))
        {
            if (existing != this)
            {
                const int32_t existing_priority = MapIndexPreservePriority(existing);
                const int32_t incoming_priority = MapIndexPreservePriority(this);
                if (incoming_priority > existing_priority)
                {
                    const int32_t replacement = FreshRuntimeMapIndex();
                    existing->SetMapIndex(replacement);
                }
                else
                {
                    newindex = FreshRuntimeMapIndex();
                }
            }
        }
    }
    if (mapindex >= 0)
        MapPane.UnregisterInstance(mapindex);
    mapindex = newindex;
    if (mapindex >= 0)
    {
        MapPane.RegisterInstance(this, mapindex);
        for (int32_t i = 0; i < components.NumItems(); ++i)
            if (TObjectComponent* component = components.Get(i))
                component->Activate();
    }
}

TObjectInstance::~TObjectInstance()
{
    // Drop any registry entry first — even a partially-constructed instance
    // that stashed a mapindex must be removed before its memory is freed.
    if (mapindex >= 0) {
        MapPane.UnregisterInstance(mapindex);
        mapindex = -1;
    }

    if (objclass == -1)
        return;

    if (Inventory.GetContainer() == this)
        Inventory.SetContainer(nullptr);

    for (int32_t i = 0; i < components.NumItems(); ++i)
        if (TObjectComponent* component = components.Get(i))
            component->Detach();
    components.DeleteAll();

    if (animator)
        delete animator;

    TObjectImagery::FreeImagery(imagery);

    if (lightdef.lightindex != -1)
        Scene3D.DeleteLight(lightdef.lightindex);

    if (lightdef.lightid != -1)
        FreeLightIndex(lightdef.lightid);

    // recursively delete all the objects in its inventory
    for (TInventoryIterator i(this); i; i++)
    {
        i.Item()->RemoveFromInventory();
        delete i.Item();
    }

    // take itself out of owner's inventory
    RemoveFromInventory();

    // If in map, remove from map
    if (GetSector() != nullptr)
        MapPane.RemoveObject(this);

    // Delete the name
    if (name && name != inf->name)
    {
        free(name);
        name = nullptr;
    }

    // If we're being centered on, cancel that
    if (MapPane.GetCenterOnObj() == this)
        MapPane.CenterOnObj(nullptr, false); // Don't center on anything

    // Kill the script
    if (script)
        delete script;

}

void TObjectInstance::SetName(char *newname)
{
    if (name && name != inf->name)  // If name is not set to point to objinfo name string
    {
        free(name);                 // delete it
        name = nullptr;
    }

    if (newname && *newname)
        name = _strdup(newname);    // Name set to a specific name
    else
        name = inf->name;           // Name cleared, set to objinfo name instead

  // Reset script when name changes (could have a new script)   
    if (script)
    {
        delete script;
        script = nullptr;
    }

    if (objclass != OBJCLASS_TILE) // No scripts for regular tiles (for efficiency sake)
        InitScript(ScriptManager.ObjectScript(this));
}

TObjectImagery* TObjectInstance::GetImagery() const
{
    return  imagery;
}

bool TObjectInstance::CreateAnimator()
{
    animator = imagery->NewObjectAnimator(this);

    if (animator)
        animator->Initialize();      // Call the animator's initialize function

    return (animator != nullptr);
}

void TObjectInstance::FreeAnimator()
{
    if (animator)
    {
        delete animator;
        animator = nullptr;
    }
}

int32_t TObjectInstance::AddComponent(TObjectComponent* component)
{
    if (!component || component->Owner())
        return -1;

    static uint32_t s_next_component_generation = 1;
    const int32_t slot = components.Add(component);
    if (slot < 0)
        return -1;

    component->Attach(this, slot, ++s_next_component_generation);
    if (GetMapIndex() >= 0)
        component->Activate();
    return slot;
}

void TObjectInstance::RemoveComponent(int32_t component_slot)
{
    if ((uint32_t)component_slot >= (uint32_t)components.NumItems())
        return;
    TObjectComponent* component = components.Get(component_slot);
    if (component)
        component->Detach();
    components.Delete(component_slot);
}

TObjectComponent* TObjectInstance::GetComponent(int32_t component_slot) const
{
    if ((uint32_t)component_slot >= (uint32_t)components.NumItems())
        return nullptr;
    return components.Get(component_slot);
}

bool TObjectInstance::NeedsAnimator() const
{
    if (!animator && imagery)
        return imagery->NeedsAnimator(this);

    return false;
}

void TObjectInstance::Damage(int32_t damage, int32_t type)
{
// this is probably a serious hack
    if (IsIced() && type != DAMAGE_ICE)
        SetIced(false);
// end of serious hack
    if (damage < Health())
        SetHealth(Health() - damage);
    else
        SetHealth(0);
}

void TObjectInstance::RepaintObject()
{
}

int32_t TObjectInstance::Distance(const TObjectInstance* inst) const
{
    return ::Distance(pos, inst->pos);
}

// Returns the angle to the other instance
int32_t TObjectInstance::AngleTo(const TObjectInstance* inst) const
{
    S3DPoint tpos;
    inst->GetPos(tpos);
    return ConvertToFacing(pos, tpos);
}

// Returns the + or - difference between this objects facing and the dest obj
int32_t TObjectInstance::FaceAngleTo(const TObjectInstance* inst) const
{
    int32_t angle = AngleTo(inst) - GetFace();
    if (angle >= 128)
        angle = angle - 256;
    else if (angle <= -128)
        angle = angle + 256;
    return angle;
}

void TObjectInstance::GetScreenPos(int32_t &x, int32_t &y) const
{
    if (oldpos == pos)
    {
        x = screenx;
        y = screeny;
    }
    else
    {
        WorldToScreen(pos, x, y);
        WorldToScreenZ(pos, screenz);

        screenx = x;
        screeny = y;

        oldpos = pos;
    }
}

void TObjectInstance::GetScreenPos(S3DPoint& s) const
{
    if (oldpos == pos)
    {
        s.x = screenx;
        s.y = screeny;
        s.z = screenz;
    }
    else
    {
        WorldToScreen(pos, screenx, screeny);
        WorldToScreenZ(pos, screenz);

        s.x = screenx;
        s.y = screeny;
        s.z = screenz;

        oldpos = pos;
    }
}

// Sets the current object position
int32_t TObjectInstance::SetPos(const S3DPoint& newpos, int32_t newlevel, bool override)
{
    int32_t index = GetMapIndex();

  // Note: can't change level of regular map objects, only non map objects
    if (newlevel < 0 || !(flags & OF_NONMAP))
        newlevel = MapPane.GetMapLevel();

  // Didn't move
    if (newpos == pos && newlevel == level)
        return index;                           // no movement, so bail out

  // Isn't in map
    if (override || !sector || index < 0)
    {
        pos = newpos;
        level = newlevel;
        return index;
    }

  // Handle shadow, if any
    if (shadow >= 0)
    {
        TObjectInstance* s = MapPane.GetInstance(shadow);
        if (s)
        {
            S3DPoint delta = newpos;
            delta -= pos;

            S3DPoint spos;
            s->GetPos(spos);
            spos += delta;
            s->SetPos(spos);
        }
    }

  // Make sure we're still in currently loaded map after we move
    index = MapPane.CheckPos(this, newpos, newlevel);

  // Get original screen rectangle
    SRect oldrect;
    GetScreenRect(oldrect);

    MapPane.ExtractWalkmap(this);

  // Sets new position
    pos = newpos;
    level = newlevel;

    MapPane.TransferWalkmap(this);

  // Update 3D system light
    if (lightdef.lightindex != -1)
    {
        S3DPoint lpos = pos;
        lpos += lightdef.pos;
        Scene3D.SetLightPosition(lightdef.lightindex, lpos);
    }

  // Get new screen rectangle
    SRect newrect;
    GetScreenRect(newrect);

  // Update background
    if (BgDrawMode() != BGDRAW_NONE)
    {
        int32_t numrects = 0;
        SRect rects[4];
        if (SubtractRect(oldrect, newrect, rects, numrects))
        {
            for (int32_t c = 0; c < numrects; c++)
                MapPane.AddBgUpdateRect(rects[c], BgDrawMode());
        }
        RedrawBackground();
    }

    return index;
}

// Gets new snap position
void TObjectInstance::GetSnapPos(const TObjectInstance* oi, int32_t dist, S3DPoint &p) const
{
    ConvertToVector(oi->GetFace(), dist, p);
    p += oi->Pos();
}

// Moves this object so that it is the given distance 'dist' from 'oi'
void TObjectInstance::SnapDist(TObjectInstance* oi, int32_t dist)
{
    if (dist >= 0)
    {
        int32_t curdist = Distance(oi);
        if (dist != curdist)
        {
            S3DPoint p;
            ConvertToVector(oi->AngleTo(this), dist, p);
            p += oi->Pos();
            SetPos(p);
        }
    }
}

void TObjectInstance::ResetState()
{
    int32_t stateflags = (imagery)?imagery->GetAniFlags(state):0;
    int32_t statesize = (imagery)?imagery->GetAniLength(state):0;
    if (stateflags & AF_REVERSE)
    {
        frame = statesize - 1;
        framerate = -1;
    }
    else
    {
        frame = 0;
        framerate = 1;
    }
    if (animator)
        animator->ResetState();
}

// Sets the current animation state for the object
bool TObjectInstance::SetState(int32_t newstate)
{
    if (GetImagery())
    {
        if ((uint32_t)newstate >= (uint32_t)GetImagery()->NumStates())
        {
            SetCommandDone(true);
            return false;
        }
    }

    RedrawBackground();
    MapPane.ExtractWalkmap(this);

    LOCKOBJECTS;   // Prevent update system from drawing objects while we change state

  // Set prevoious values for interpolation system
    prevstate = state; // Previous state
    prevframe = frame; // Previous state's final frame (NOT THIS STATES PREVIOUS FRAME!!!)

  // Set new state now
    state = newstate;
    SetCommandDone(false);
    ResetState();

    UNLOCKOBJECTS; // Ok, go ahead and draw objects again

    MapPane.TransferWalkmap(this);

    if (GetImagery()->NeedsAnimator(this))
        flags |= OF_ANIMATE;            // Cause Animate() function to be called
    else
    {
        FreeAnimator();
        flags &= ~(uint32_t)OF_ANIMATE;    // Prevents Animate() function from being called
    }

    RedrawBackground();

    return true;
}

bool TObjectInstance::AddToInventory(TObjectInstance* inst, int32_t slot)
{
    if (slot < 0)
        slot = FindFreeInventorySlot();

    if ((uint32_t)slot >= MAXINVITEMS)
        return false;

    inst->OffScreen();

    int32_t index = inventory.Add(inst);
    if (index < 0)
        return false;

    if (inst->GetMapIndex() <= 0)
        inst->SetMapIndex(MapPane.MakeIndex());

    inst->invindex = index;
    inst->inventnum = slot;
    inst->owner = this;

    inst->pos.x = inst->pos.y = inst->pos.z = 0;
    inst->level = 0;
    inst->sector = nullptr;

    inst->SignalAddedToInventory();

    if (this == Inventory.GetContainer())
        Inventory.Update();

    return true;
}

bool TObjectInstance::AddToInventory(const char *name, int32_t number, int32_t slot)
{
    TObjectClass* cl;
    int32_t ot;
    for (int32_t i = 0; i < MAXOBJECTCLASSES; i++)
    {
        cl = TObjectClass::GetClass(i);
        if (cl && (ot = cl->FindObjType(name)) >= 0)
            break;
    }

    if (ot < 0)
        return false;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = cl->ClassId();
    def.objtype = ot;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
        return false;

    if (number != 1)
        inst->SetAmount(number);

    return AddToInventory(inst, slot);
}

void TObjectInstance::RemoveFromInventory()
{
    if (owner)
    {
        if (owner == Inventory.GetContainer())
            Inventory.Update();

        owner->inventory.Remove(invindex);
        owner = nullptr;
    }

    invindex = -1;
    inventnum = -1;
}

int32_t TObjectInstance::GiveInventoryTo(TObjectInstance* to, const char *name, int32_t number)
{
    int32_t total = 0;
    if (number < 1)
        return 0;

    do
    {
        TObjectInstance* inst = FindObjInventory(name);

        if (!inst)
            return total;

        int32_t amt = max(inst->Amount(), 1);

        if (number >= amt)
        {
            total += amt;
            number -= amt;
            inst->RemoveFromInventory();
            if (to)
                to->AddToInventory(inst);
            else
                delete inst;
        }
        else
        {
            inst->SetAmount(amt - number);
            if (to)
            {
                if (!to->AddToInventory(name, number))
                    return total;
            }
            total += number;
            number = 0;
            
            if (inst->owner == Inventory.GetContainer())
                Inventory.Update();
        }

    } while (number > 0);

    return total;
}

int32_t TObjectInstance::GetInventoryAmount(const char *name) const
{
    int32_t total = 0;

    for (TConstInventoryIterator i(this); i; i++)
        if (stricmp(i.Item()->GetName(), name) == 0)
            total += max(i.Item()->Amount(), 1);

    return total;
}

bool TObjectInstance::HasEmptySlot() const
{
    uint32_t slot = FindFreeInventorySlot();

    if ((uint32_t)slot >= MAXINVITEMS)
        return false;

    return true;
}

void TObjectInstance::SignalAddedToInventory()
{
}

bool TObjectInstance::AddToMap()
{
    return (MapPane.AddObject(this) >= 0);

}

void TObjectInstance::RemoveFromMap()
{
    TMapIterator i;
    while (i)
    {
        if (i.Item() == this)
            break;
        i++;
    }

    if (i.Item())
        MapPane.RemoveFromSector(this, i.SectorX(), i.SectorY(), i.SectorIndex());
}

int32_t TObjectInstance::FindFreeInventorySlot() const
{
    int32_t lowest = 0;
    bool done = false;

    while (!done)
    {
        done = true;

        for (TConstInventoryIterator i(this); i; i++)
            if (i.Item()->InventNum() == lowest)
            {
                done = false;
                lowest++;
                break;
            }
    }

    return lowest;
}

int32_t TObjectInstance::RealNumInventoryItems()
{
    // count up the real number of items in the inventory
    int32_t num = 0;
    for (TInventoryIterator i(this); i; i++)
        num++;

    return num;
}

TObjectInstance* TObjectInstance::GetInventory(int32_t index) const
{
    return inventory[index];
}

TObjectInstance* TObjectInstance::GetInventorySlot(int32_t slot) const
{
    for (TConstInventoryIterator i(this); i; i++)
        if (i.Item()->InventNum() == slot)
            return i.Item();

    return nullptr;
}

TObjectInstance* TObjectInstance::FindObjInventory(const char *name) const
{
    for (TConstInventoryIterator i(this); i; i++)
        if (stricmp(i.Item()->GetName(), name) == 0)
            return i.Item();

    return nullptr;
}

TObjectInstance* TObjectInstance::FindObjInventory(int32_t objclass, int32_t type) const
{
    for (TConstInventoryIterator i(this); i; i++)
        if (i.Item()->ObjClass() == objclass && (type < 0 || i.Item()->GetStat("Type") == type))
            return i.Item();

    return nullptr;
}

bool TObjectInstance::Use(TObjectInstance* user, int32_t with)
{
    if (with >= 0)  // With object.. use with name as key
    {
        TObjectInstance* inst = MapPane.GetInstance(with);
        if (GetScript())
            GetScript()->Trigger(TRIGGER_USE, inst->GetName());
    }
    else        // No with object.. use 'use' object as key
    {
        if (GetScript())
            GetScript()->Trigger(TRIGGER_USE);
        if (user && user->GetScript())
            user->GetScript()->Trigger(TRIGGER_USE, this->GetName());
    }

    /*
    if (!inst || inst->GetOwner() != GetOwner() || inst->ObjClass() != objclass)
        return false;

    TObjectClass* cl = TObjectClass::GetClass(ObjClass());

    if (GetStatistic("Combining") && inst->GetStatistic("Combining") &&
        absval(ObjType() - inst->ObjType()) == 1)
    {
        int32_t slot = inst->InventNum();
        TObjectInstance* own = GetOwner();
        if (own)
        {
            MapPane.RemoveFromInventory(own, InventNum());
            MapPane.RemoveFromInventory(own, inst->InventNum());
        }

        SetFlags(OF_KILL);
        inst->SetFlags(OF_KILL);

        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));

        def.objclass = ObjClass();
        def.objtype = ObjType() > inst->ObjType() ? (ObjType() + 1) : (inst->ObjType() + 1);
        Player->GetPos(def.pos);

        int32_t index = MapPane.NewObject(&def);
        if (index >= 0 && own)
            MapPane.AddToInventory(own, index, slot);

        return true;
    }
    */
    return false;
}

void TObjectInstance::Pulse()
{
/*  static int32_t i = 230;
    static int32_t d = 1;

    if (flags & OF_LIGHT) // TEST
    {
        SetLightIntensity(i);
        i += d;
        if (i >= 255)
        {
            i = 255;
            d = -1;
        }
        else if (i < 230)
        {
            i = 230;
            d = 1;
        }
    }
*/
  // Pulse the animator
    if (animator)
        animator->Pulse();

  // Check if script is done
    if (CommandDone())
        ContinueScript();
}

uint32_t TObjectInstance::Move()
{
    if (flags & OF_IMMOBILE || IsInInventory() || objclass == OBJCLASS_TILE || objclass == OBJCLASS_EXIT || objclass == OBJCLASS_SHADOW)
        return MOVE_NOTHING;

  // Now do this frame's movement
    S3DPoint newpos = pos;
    uint32_t retval = 0;

    int32_t h = (MapPane.GetWalkHeight(pos) + 1);

    if ((pos.z < h || h == 1) && !(flags & OF_NOCOLLISION))
        return MOVE_BLOCKED;                // very basic collision detection

    if (h < pos.z && vel.z > -TERMINAL_VELOCITY && !(flags & OF_WEIGHTLESS))
    {
        vel.z -= GRAVITY;
        retval |= MOVE_FALLING;

        // $$$ KES $$$ -- check to see that we aren't falling too far...
        if ((pos.z + vel.z) < (h + 1)) vel.z = ((h + 1) - pos.z);
    }


  // ADD IN THE MOVEMENT VALUE HERE!
    S3DPoint nextmove;
    GetNextMove(nextmove);
    accum += nextmove;

  // ADD IN THE VELOCITY VALUE HERE!
    accum += vel;

    rollover(accum.x, newpos.x);
    rollover(accum.y, newpos.y);
    rollover(accum.z, newpos.z);

    if (newpos.z < h)
    {
        if (absval(vel.z) > GRAVITY)
            vel.z = -(vel.z / 4);               // bounce bounce
        else
            vel.z = 0;

        newpos.z = h;
        accum.z = 0;
    }

    if ((objclass == OBJCLASS_CHARACTER) && (newpos.z < h))
        newpos.z = h;

    if (newpos == pos && vel.x == 0 && vel.y == 0 && vel.z == 0)
        return MOVE_NOTHING;        // can only check this _after_ gravity

    if (newpos == pos)
        return MOVE_MOVED | retval;         // they moved, but not a whole unit

    //if (!MapPane.LineOfSight(pos, newpos))
    //  return MOVE_BLOCKED;

    SetPos(newpos);

    return MOVE_MOVED | retval;
}

void TObjectInstance::SetObjectMotion()
{
  // Get next movement values from imagery object
    if (imagery)
        imagery->SetObjectMotion(this);
}

// Set next frame's movement.
void TObjectInstance::SetNextMove(S3DPoint& p)
{
    moveangle = ::ConvertToFacing(p);
    movedist = ::Distance(p);
    movevert = p.z;
}

// Get next frame's movement.
void TObjectInstance::GetNextMove(S3DPoint& p)
{
    ConvertToVector(moveangle, movedist, p);
    p.z = movevert;
}

void TObjectInstance::Animate(bool draw)
{
    if (animator)
        animator->Animate(draw);
}

// Sets next frame for object
void TObjectInstance::NextFrame()
{
    if (flags & OF_PARALIZE)
        return;

    if (!imagery)
        return;

    int32_t stateflags = 0;
    int32_t statesize = 0;

    if (IsInInventory())
    {
        statesize = imagery->GetInvAniLength(state);
        stateflags = imagery->GetInvAniFlags(state);
    }
    else
    {
        statesize = imagery->GetAniLength(state);
        stateflags = imagery->GetAniFlags(state);
    }

    frame += framerate;

    if (framerate < 0)
    {
        if (frame < 0)
        {
            SetCommandDone(true);

            if (frame < 0)
            {
                if (stateflags & AF_LOOPING)
                    frame = statesize - 1;
                else
                {
                    if (stateflags & AF_PINGPONG)
                    {
                        frame = 1;
                        framerate = 1;
                    }
                    else
                    {
                        if (animator)
                            animator->SetComplete(true);
                        frame = 0;
                    }
                }
            }
        }
        else if (CommandDone() == true)
            SetCommandDone(false);
    }
    else
    {
        if (frame > (statesize - 1))
        {
            SetCommandDone(true);

            if (frame >= statesize)
            {
                if (stateflags & AF_LOOPING)
                    frame = 0;
                else
                {
                    if (stateflags & AF_PINGPONG)
                    {
                        frame = statesize - 1;
                        framerate = -1;
                    }
                    else
                    {
                        if (animator)
                            animator->SetComplete(true);
                        frame = statesize - 1;
                    }
                }
            }
        }
        else if (CommandDone() == true)
            SetCommandDone(false);
    }

    if (animator)
        animator->SetNewState(false);
}

void TObjectInstance::OnScreen()
{
    if (NeedsAnimator())
        CreateAnimator();
    else
        FreeAnimator();

    if (flags & OF_LIGHT)
    {
        if (lightdef.lightindex == -1)
        {
            S3DPoint lpos = pos;
            lpos += lightdef.pos;
            lightdef.lightindex = Scene3D.AddLight(lpos, lightdef.color, lightdef.intensity, lightdef.multiplier);
        }
    }
}

void TObjectInstance::OffScreen()
{
    if (animator)
        FreeAnimator();

    if (lightdef.lightindex != -1)
    {
        Scene3D.DeleteLight(lightdef.lightindex);
        lightdef.lightindex = -1;
    }
    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = -1;
    }
}

void TObjectInstance::GetFacingBoundBox(int32_t &nx, int32_t &ny, int32_t &nsx, int32_t &nsy)
{
    int32_t width, length, height;
    GetImagery()->GetWorldBoundBox(state, width, length, height);

    int32_t regx = GetImagery()->GetWorldRegX(state);
    int32_t regy = GetImagery()->GetWorldRegY(state);

    nx = regx;
    ny = regy;
    nsx = width;
    nsy = length;

    if (facing >= 0xE0 || facing < 0x20)
    {
        // north, which is default
        return;
    }
    else if (facing < 0x60)
    {
        // east
        nx = regy;
        ny = regx;
        nsx = length;
        nsy = width;
    }
    else if (facing < 0xA0)
    {
        // south
        nx = width - regx - 1;
    }
    else if (facing < 0xE0)
    {
        // west
        ny = width - regx - 1;
        nsx = length;
        nsy = width;
    }
}

// ********* Background Redrawing Function **********

// Returns the default point in bgdraw pipeline at which to redraw this object.  The
// bg draw pipeline draws the UNLIT data first (raw tile graphics/colors), then applies
// the lighting, then draws any lit graphics to the lit buffer, then copies the whole thing
// to the screen.  Objects like lights only need to refresh from the lighting stage on up,
// thus saving scads of time.
//
// This function doesn't currently check the imagery for what
int32_t TObjectInstance::BgDrawMode()
{
    if (flags & OF_LIGHT)
        return BGDRAW_LIGHTS; 
    else if (flags & (OF_MOVING | OF_INVISIBLE | OF_SELDRAW))
        return BGDRAW_NONE;
    else return BGDRAW_UNLIT;
}

void TObjectInstance::RedrawBackground(int32_t bgdraw)
{
  // If object is tile or light and drawn background buffers add
  // an update rectangle for the object. (moving objects not included)

    if (!(flags & OF_MOVING) &&                       // No moving objs
        (!(flags & OF_SELDRAW) || (flags & OF_LIGHT)) // No sel objs (except lights)
         && InventNum() < 0)                          // No objects inside another object
    {
        SRect r;
        GetScreenRect(r);
        if (bgdraw == -1)               // Use default draw pipeline flag for object
        {
            if (flags & OF_LIGHT)
                bgdraw = BGDRAW_LIGHTS; // Default to redraw from lights on up
            else
                bgdraw = BGDRAW_UNLIT;  // Default to draw from unlit on up (all)
        }
        MapPane.AddBgUpdateRect(r, bgdraw);
    }
}

// ********** Script Functions ************

void TObjectInstance::InitScript(PTScript newscr)
{
    if (objclass == OBJCLASS_TILE)
        return; // Tiles can't do scripts!!

    script = newscr;
    if (script)
    {
        flags |= OF_PULSE;          // Pulse me so script will run
        SetNotify(N_SCRIPTDELETED); // Tell us if script gets hacked
    }

    ResetScript();
}

void TObjectInstance::ResetScript()
{
    if (script)
        script->Start();
}

void TObjectInstance::ContinueScript()
{
    if (script && !(flags & OF_PAUSE))
        script->Continue(this);
}

void TObjectInstance::ScriptJump(char *label)
{
    if (script)
        script->Jump(this, label);
}

// ****************************** Parse Command ********************************

int32_t TObjectInstance::ParseCommand(const TToken &t)
{
    return CMD_BADCOMMAND;
}

// *************************** Lighting Functions *****************************

void TObjectInstance::RedrawLight()
{
    if (flags & OF_LIGHT && lightdef.intensity != 0)
    {
        SRect r;
        GetLightRect(r);
        MapPane.AddBgUpdateRect(r, BGDRAW_LIGHTS);
    }
}

void TObjectInstance::GetLightRect(RSRect r)
{
    S3DPoint wlpos = pos;
    wlpos += lightdef.pos;

    S3DPoint slpos;
    WorldToScreen(wlpos, slpos);

    r.left = slpos.x - lightdef.intensity;
    r.top = slpos.y - lightdef.intensity;
    r.right = r.left + lightdef.intensity + lightdef.intensity;
    r.bottom = r.top + lightdef.intensity + lightdef.intensity;
}

void TObjectInstance::SetLightIntensity(uint8_t newintensity)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;    // Prevent update system from drawing any lights right now

    bool bigger = newintensity > lightdef.intensity;

    if (!bigger)
        RedrawLight();

    lightdef.intensity = newintensity;

    if (lightdef.lightindex != -1)
        Scene3D.SetLightIntensity(lightdef.lightindex, lightdef.intensity);

    if (bigger)
        RedrawLight();

    UNLOCKOBJECTS; // Allow update system to draw lights now
}

void TObjectInstance::SetLightMultiplier(int32_t mult)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;    // Prevent update system from drawing any lights right now

    lightdef.multiplier = mult;

//  if (lightdef.lightindex != -1)
//      Scene3D.SetLightIntensity(lightdef.lightindex, lightdef.intensity);

    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = NewLightIndex(lightdef.color, lightdef.multiplier);
    }

    RedrawLight();

    UNLOCKOBJECTS; // Allow update system to draw lights now
}

void TObjectInstance::SetLightPos(S3DPoint& newpos)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;     // Prevent update system from drawing any lights right now

    RedrawLight();

    lightdef.pos = newpos;

    if (lightdef.lightindex != -1)
    {
        S3DPoint lpos = pos;
        lpos += lightdef.pos;
        Scene3D.SetLightPosition(lightdef.lightindex, lpos);
        RedrawLight();
    }

    UNLOCKOBJECTS;   // Allow update system to draw lights now
}

void TObjectInstance::SetLightColor(SColor color)
{
    if (!(flags & OF_LIGHT) || (color.red == lightdef.color.red &&
        color.green == lightdef.color.green && color.blue == lightdef.color.blue))
        return;

    LOCKOBJECTS;     // Prevent update system from drawing any lights right now

    lightdef.color = color;

    if (lightdef.lightindex != -1)
        Scene3D.SetLightColor(lightdef.lightindex, color);

    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = NewLightIndex(color, lightdef.multiplier);
    }

    RedrawLight();

    UNLOCKOBJECTS;   // Allow update system to draw lights now
}

void TObjectInstance::DrawLight(TSurface* surface, bool resetid)
{
    if ((flags & OF_LIGHT) && (lightdef.intensity > 0))
    {
        if (imagery)
            imagery->DrawLight(this, surface);

        S3DPoint wlpos = pos;
        wlpos += lightdef.pos;

        S3DPoint slpos;
        WorldToScreen(wlpos, slpos);

        if (resetid || lightdef.lightid == -1)
            lightdef.lightid = NewLightIndex(lightdef.color, lightdef.multiplier);

//      if (NoNormals)
            DrawStaticLightNoNormals(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
//      else if (lightdef.flags & LIGHT_DIR)
//          DrawStaticDirLight(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
//      else
//          DrawStaticLight(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
    }
}

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int32_t dist, int32_t intensity, int32_t multiplier);

// Get the amount of illumination from this light to this particular object
int32_t TObjectInstance::GetIllumination(TObjectInstance* oi)
{
  // Not a light
    if (!(flags & OF_LIGHT) || (lightdef.intensity <= 0))
        return 0;

  // Dist from object to light (assumes char, and uses CHARHEIGHT value)
    S3DPoint p;
    oi->GetPos(p);
    p.z += LIGHTINGCHARHEIGHT;
    p -= pos;

  // Out of range
    if (abs(p.x) > lightdef.intensity || abs(p.y) > lightdef.intensity)
        return 0;

  // Get distance from light to object
    float d = (float)sqrt((double)(p.x * p.x + p.y * p.y));
    d = (float)sqrt((double)(d * d + p.z * p.z));

  // Get total brighness from dls brightness tables
    return (int32_t)(
        min(GetLightBrightness((int32_t)d, lightdef.intensity, lightdef.multiplier), 1.0) * 255.0);
}

// ********************** End of Light Functions ************************

void TObjectInstance::SetCommandDone(bool newcmd)
{
    commanddone = newcmd;
}

void TObjectInstance::SetNotify(uint32_t newflags)
{
    if (newflags == 0)
        ResetFlags(flags & (~OF_NOTIFY));
    else
    {
        MapPane.SetNotify(newflags);    // Tells mappane there's at least one object that wants these flags
        ResetFlags(flags | OF_NOTIFY);  // Tell map that we want to be notified of stuff
    }
}

void TObjectInstance::Notify(int32_t notify, void *ptr)
{
    if (notify == N_SCRIPTDELETED) // Check if we're using this script and delete it if so
    {
        if (script && script->GetScriptProto() == (TScriptProto*)ptr)
        {
            delete script;
            script = nullptr;
        }
    }
    else if (notify == N_SCRIPTADDED)   // Check to see if new script matches us
    {
        if (!script && objclass != OBJCLASS_TILE) // No scripts for regular tiles
            InitScript(ScriptManager.ObjectScript(this));
    }
}

// Diagnostic counters for why LoadObject returned nullptr. Wired up by the
// sector test harness to tell "legitimate empty placeholder" apart from
// "loader couldn't recover this object".
int32_t g_loadObjNullObjVerNeg   = 0;
int32_t g_loadObjNullClassNeg    = 0;
int32_t g_loadObjNullBadClass    = 0;
int32_t g_loadObjNullBadType     = 0;
int32_t g_loadObjNullNewObjFail  = 0;
int32_t g_loadObjNullCorruptDrop = 0;
int32_t g_loadObjNullNonMapDrop  = 0;
int32_t g_loadObjOk              = 0;

TObjectInstance* TObjectInstance::LoadObject(RTInputStream is, int32_t version, bool ismap)
{
    uint32_t uniqueid;
    short objversion = 0;
    short objclass;
    short objtype;
    short blocksize;
    short invblocksize = -1;  // v14+: separate block size for inventory body
    SObjectDef def;
    bool forcesimple = false;
    bool corrupted = false;

    // ****** Load object block header ******

    // Get object version
    if (version >= 8)
        is >> objversion;

    if (objversion < 0) // Objversion is the placeholder in map version 8 or above
    {
        ++g_loadObjNullObjVerNeg;
        return nullptr;
    }

    is >> objclass;
    if (objclass < 0)   // Placeholder for empty object slot
    {
        ++g_loadObjNullClassNeg;
        return nullptr;
    }

    // Check the sector map version before we read the type info
    if (version < 1)
    {
        // Version 0 - No Unique ID's, so just read the objtype directly
        is >> objtype;
        uint32_t uniqueid = 0;
        blocksize = -1;     
    }
    else if (version < 4)
    {
        // Version 1 and above - Unique ID's used instead of objtype, so find
        //             the objtype given the Unique ID

        objtype = -1;
        is >> uniqueid;
        blocksize = -1;
    }
    else
    {
        // Version 4 has block size, so we can just skip over objects
        //              we don't recognize
        objtype = -1;
        is >> uniqueid;
        is >> blocksize;

        // v14+ splits the single blocksize into a body blocksize plus a
        // separate inventory blocksize, so the inventory body can also be
        // skipped independently. See retail FUN_00471ce0 gate `param_4 >= 0xe`.
        if (version >= 14)
            is >> invblocksize;
    }

    // ****** Is this object any good? ******
    
    TObjectClass* cl = TObjectClass::GetClass(objclass);
    if (!cl || !cl->ClassName())
    {
        if (Debug)
            FatalError("Object in map file has invalid class - possible file corruption");
        else if (blocksize >= 0)
        {
            // Retail LAB_00471e57: unsupported class (bags/chests/invcontainer
            // added post-1998-source, e.g. retail-only class 18). `blocksize`
            // covers body + inventory together in v14+, so a single MovePos
            // resyncs to the next object. invblocksize is already included.
            is.MovePos(blocksize);
            ++g_loadObjNullBadClass;
            return nullptr;
        }
        else                        // Try to fix it by assuming its a tile
        {
            objclass = OBJCLASS_TILE;
            cl = TObjectClass::GetClass(objclass);
            corrupted = true;
        }
    }

    if (objtype < 0)
    {           
        objtype = cl->FindObjType(uniqueid);

        if (objtype < 0)
        {
            // not found in this class, so check all of them
            int32_t newobjtype, newobjclass;
            TObjectClass* newcl;
            for (newobjclass = 0; newobjclass < MAXOBJECTCLASSES; newobjclass++)
            {
                newcl = TObjectClass::GetClass(newobjclass);
                if (newcl && (newobjtype = newcl->FindObjType(uniqueid)) >= 0)
                {
                    objclass = newobjclass;
                    objtype = newobjtype;
                    cl = newcl;
                    forcesimple = true;
                    break;
                }
            }
        }

        if (objtype < 0)  // Still can't find type
        {
            if (Debug)
            {
                // give a more descriptive error
                char buf[80];
                sprintf(buf, "Object unique id 0x%x not found in class.def", uniqueid);
                FatalError(buf);
            }
            else if (blocksize >= 0)    // Just skip over this object
            {
                is.MovePos(blocksize);
                ++g_loadObjNullBadType;
                return nullptr;
            }
            else      // If attempting to fix, assume type is type 0 - first type in list
            {   
                
                objtype = 0;
                corrupted = true;
            }
        }
    }

    // ****** Create the object ******

    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = objclass;
    def.objtype  = objtype;

    TObjectInstance* inst = cl->NewObject(&def);
    if (!inst)
    {
        // Class factory missing — retail added bag/chest/invcontainer-style
        // classes (e.g. class 25) after this 1998 source was snapshotted, so
        // NewObject returns null for them. Skip rather than abort; blocksize
        // covers body+inventory so a single MovePos resyncs the stream.
        // Retail FUN_00471ce0 would crash here (the "Trouble creating" error
        // is a warning, followed by an unchecked virtual call on null); our
        // port is strictly safer.
        if (blocksize >= 0)
            is.MovePos(blocksize);
        ++g_loadObjNullNewObjFail;
        return nullptr;
    }

    // ****** Load the object ******

  // Retail layout (FUN_00471ce0, v14+): `blocksize` is the TOTAL post-header
  // byte count covering body + inventory together; `invblocksize` is the
  // inventory tail carved out of the end of that block. Body-proper size is
  // `blocksize - invblocksize`. The single final resync at LAB_00471fbc sets
  // the stream to `bodystart + blocksize`. Earlier pre-retail (v4..v13) source
  // used a single blocksize covering body+inventory without a separate
  // invblocksize field.
    uint32_t bodystart = is.GetPos();

    if (forcesimple)
        inst->TObjectInstance::Load(is, version, objversion);
    else
        inst->Load(is, version, objversion);

  // Snap back to start-of-inventory before calling LoadInventory so it reads
  // from the correct offset regardless of how many bytes Load() actually
  // consumed.
    if (version >= 14 && blocksize >= 0 && invblocksize >= 0)
        is.SetPos(bodystart + blocksize - invblocksize);

  // v14+: retail skips LoadInventory entirely when invblocksize < 1 (no
  // inventory body present). See FUN_00471ce0 LAB_00471fa3 — the `0xd <
  // param_4` gate jumps over the vtable dispatch for version > 13.
    if (version < 14 || invblocksize >= 1)
        inst->LoadInventory(is, version);

  // Final resync to end-of-object = bodystart + blocksize (covers body+inv).
    if (blocksize >= 0)
        is.SetPos(bodystart + blocksize);

  // If this object is corrupted in some way, delete it after doing load.
  // Keep OF_NONMAP drops separate from true corruption so the sector harness
  // can distinguish "players intentionally discarded from map-owned sectors"
  // from "loader failed to reconstruct this object."
    const bool nonmap_drop = ismap && (inst->Flags() & OF_NONMAP);
    if (corrupted || nonmap_drop)
    {
        delete inst;
        if (nonmap_drop)
            ++g_loadObjNullNonMapDrop;
        else
            ++g_loadObjNullCorruptDrop;
        return nullptr;
    }

    ++g_loadObjOk;
    return inst;
}

// Mirrors retail FUN_00472110 (v14+). Layout written to disk:
//   [objversion:int16][objclass:int16][uniqueid:uint32]
//   [blocksize:int16][invblocksize:int16]     (invblocksize only when v14+)
//   [body bytes: blocksize - invblocksize]
//   [inventory bytes: invblocksize]           (present only when invblocksize>0)
// An empty slot is encoded as just a single int16 = -1.
void TObjectInstance::SaveObject(TObjectInstance* inst, RTOutputStream os, bool ismap)
{
    os.MakeFreeSpace(1024);

    // Retail gate: when saving a map file, any OF_NONMAP object (players,
    // script-generated effects, etc.) is emitted as a placeholder. The
    // `ismap` argument corresponds to retail `DAT_0065a254 & 1`.
    if (!inst || (ismap && (inst->Flags() & OF_NONMAP)))
    {
        os << (short const)-1;
        return;
    }

    os << (short)inst->ObjVersion();
    os << (short)inst->ObjClass();
    os << (uint32_t)inst->ObjId();
    os << (short)0;                           // blocksize placeholder
    if (MAP_VERSION >= 14)
        os << (short)0;                       // invblocksize placeholder

    uint32_t bodystart = os.GetPos();
    inst->Save(os);
    uint32_t bodyend = os.GetPos();

    // Retail only emits inventory bytes when there is actually something to
    // save. Skipping the SaveInventory call entirely (rather than letting it
    // write just a 4-byte count=0) keeps invblocksize==0 on disk, matching
    // retail byte-for-byte and letting LoadObject's `invblocksize < 1`
    // fast-path skip LoadInventory.
    if (inst->RealNumInventoryItems() > 0)
        inst->SaveInventory(os);

    uint32_t end = os.GetPos();

    if (MAP_VERSION >= 14)
    {
        // v14+: blocksize = total body+inv bytes, invblocksize = inv tail.
        os.SetPos(bodystart - 4);
        os << (short)(end - bodystart);
        os << (short)(end - bodyend);
    }
    else
    {
        // Pre-v14: single blocksize covering body+inventory together.
        os.SetPos(bodystart - 2);
        os << (short)(end - bodystart);
    }
    os.SetPos(end);
}

void TObjectInstance::LoadInventory(RTInputStream is, int32_t version)
{
    if (version < 3)
        return;

    int32_t num;
    is >> num;

    if (num > 2048) // Maddness!!  Maddness!!
    {
        fprintf(stderr, "Invalid inventory size for obj %s\n", this->GetName());
        return;
    }

    for (int32_t i = 0; i < num; i++)
    {
        TObjectInstance* inst = LoadObject(is, version);
        if (inst)
        {
            inventory.Add(inst);
            inst->SetOwner(this);
        }
        else
        {
            fprintf(stderr, "Invalid inventory object for obj %s", this->GetName());
        }
    }
}

void TObjectInstance::SaveInventory(RTOutputStream os)
{
    int32_t num = RealNumInventoryItems();

    os << num;

    if (num > 0)
    {
        // save them out recursively
        for (TInventoryIterator i(this); i; i++)
        {
            TObjectInstance::SaveObject(i.Item(), os);
        }
    }

}

void TObjectInstance::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    uint8_t len;
    is >> len;
    if (len > 0)
    {
        name = (char *)malloc(len + 1);
        int32_t i = 0;
        while (i < len)
        {
            is >> name[i];
            ++i;
        }
        name[i] = 0;
    }

    uint32_t newflags;

    is >> newflags >> pos.x >> pos.y >> pos.z;

  // Make sure fixed flags remain the way they were set in the constructor
    flags = flags & OF_FIXEDFLAGS | (newflags & ~(OF_FIXEDFLAGS));

  // Don't load velocity vectors if not mobile
    if (version < 6 || !(flags & OF_IMMOBILE))
        is >> vel.x >> vel.y >> vel.z;
        
    if (version < 9)
    {
        uint8_t statebyte;
        is >> statebyte;
        state = statebyte;
    }
    else
        is >> state; // uint16_t

  // Non map objects (i.e. players) store their level
  // Players are stored in the save game file, and not in the map, so we need to know what
  // level to put them in when we load them
    if (version >= 6 && (flags & OF_NONMAP))
    {
        if (version < 9)
        {
            uint8_t levelbyte;
            is >> levelbyte;
            level = levelbyte;
        }
        else
            is >> level; // uint16_t
    }
    else
        level = 0;
        
    uint8_t health;
    if (version < 5)
        is >> health; // This is now an objstat
    
    if (version < 3)
    {
        uint8_t dummy8;
        short dummy16;
        is >> facing >> dummy16 >> inventnum >> dummy16 >> shadow >> dummy8;

        // ignore inventories in old version
        inventnum = -1;

        // sector will set old-style indexes after object is loaded
        mapindex = -1;
    }
    else
    {
        int32_t loaded_mapindex = -1;
        is >> inventnum >> invindex >> shadow >> rotatex >> rotatey >> rotatez >> loaded_mapindex;
        // Route through SetMapIndex so the MapPane mapindex→instance registry
        // picks up every streamed-in instance; TSafeRef<T>::Get() relies on it.
        SetMapIndex(loaded_mapindex);
    }
    moveangle = rotatez;    // Set movement angle

    if (version < 5)  // Set up empty stat array and stick health in it
    {
        frame = 0;
        framerate = 1;
        group = 0;

        if (cl->NumObjStats() > 0)
        {
            stats.SetNumItems(cl->NumObjStats());
            for (int32_t c = 0; c < cl->NumObjStats(); c++)
                ResetObjStat(c);
        }

        SetHealth(health);
    }
    else
    {
        if (version >= 6)
        {
            if (flags & OF_ANIMATE)
                is >> frame >> framerate;   // Save framerate and frame if animating
        }
        else
            is >> frame >> framerate;       // Version 5, always save

        is >> group;

      // Sure stat storage is inefficient, but it's only for a few objects in the game
        if (cl->NumObjStats() > 0)
            stats.SetNumItems(cl->NumObjStats());

        uint8_t numstats;
        is >> numstats;
        if (numstats > 0)   // Note: oridnary tiles can't have stats..
        {
            int32_t statid = 0;
            for (int32_t st = 0; st < numstats && statid < stats.NumItems(); st++)
            {
                int32_t stat;
                uint32_t uniqueid;
                is >> stat >> uniqueid;

              // Note: to allow us to change the stats for characters,
              // we check the unique id of the stat and match it to our object stat array
                if (uniqueid == cl->ObjStatUniqueId(statid))
                {
                    stats[statid] = stat;   // Quick case, id's all match
                    statid++;
                }
                else                
                {                           // Slow case.. search for id for stat
                    for (int32_t c = 0; c < cl->NumObjStats(); c++)
                    {
                        if (uniqueid == cl->ObjStatUniqueId(statid))
                            stats[c] = stat;
                    }
                }
            }
        }

    }

    if (flags & OF_LIGHT)
    {
        is >> lightdef.flags >> lightdef.pos.x >> lightdef.pos.y >> lightdef.pos.z >>
            lightdef.color.red >> lightdef.color.green >> lightdef.color.blue >>
            lightdef.intensity >> lightdef.multiplier;
        flags |= OF_LIGHT | OF_ANIMATE;
    }

  // Set Object info pointer
    cl = TObjectClass::GetClass(objclass);
    if (!cl)
        FatalError("Bad object class!"); // This should never happen!!
    inf = cl->GetObjType(objtype);

  // Initialize the script (no tiles for efficiency)
    if (objclass != OBJCLASS_TILE)
        InitScript(ScriptManager.ObjectScript(this));

  // Reset inventory
    if (this == Inventory.GetContainer())
        Inventory.Update();
}

void TObjectInstance::Save(RTOutputStream os)
{
    if (flags & OF_LIGHT)
        flags |= OF_PULSE | OF_ANIMATE;

    uint8_t len = strlen(name);
    os << len;
    for (int32_t i = 0; i < len; i++)
        os << name[i];
  
  // Save general object data
    os << flags << pos.x << pos.y << pos.z;
    
    if (!(flags & OF_IMMOBILE))
        os << vel.x << vel.y << vel.z;
    
    os << state;

  // Non map objects (i.e. players) store their level
  // Players are stored in the save game file, and not in the map, so we need to know what
  // level to put them in when we load them
    if (flags & OF_NONMAP)
        os << level;
        
    os << inventnum << invindex << shadow << 
        rotatex << rotatey << rotatez << mapindex;
        
    if (flags & OF_ANIMATE)
        os << frame << framerate;
        
    os << group;

  // Save object specific stats (unique id is <=4 char code like "AMT", "TYPE", "AC")
  // Now I've got to say that the unique id thing is just pretty damn cool.  That's one
  // super tricky bit of coding there boy... wow.. what an idea.  Super groovy and all
  // that.  
    os << (uint8_t)(cl->NumObjStats());
    for (int32_t c = 0; c < cl->NumObjStats(); c++)
        os << stats[c] << cl->ObjStatUniqueId(c);

  // Save light data
    if (flags & OF_LIGHT)
        os << lightdef.flags << lightdef.pos.x << lightdef.pos.y << lightdef.pos.z <<
            lightdef.color.red << lightdef.color.green << lightdef.color.blue <<
            lightdef.intensity << lightdef.multiplier;
}

// ----------------- Flag Functions --------------

static char *flagnames[] = OBJFLAGNAMES;
#define NUMFLAGS sizearray(flagnames)

int32_t TObjectInstance::GetNumFlags() const
{
    return NUMFLAGS;
}

const char *TObjectInstance::GetFlagName(int32_t flagnum) const
{
    return flagnames[flagnum];
}

int32_t TObjectInstance::GetFlagNum(const char *flagname) const
{
    for (int32_t c = 0; c < NUMFLAGS; c++)
    {
        if (!stricmp(flagnames[c], flagname))
            return c;
    }
    return -1;
}

void TObjectInstance::SetFlag(const char *flagname, bool on)
{
    int32_t flagnum = GetFlagNum(flagname);
    if (flagnum < 0)
        return;

    if (on)
        ResetFlags(flags | (1 << flagnum));
    else
        ResetFlags(flags & (~(1 << flagnum)));
}

bool TObjectInstance::IsFlagSet(const char *flagname) const
{
    int32_t flagnum = GetFlagNum(flagname);
    if (flagnum < 0)
        return false;

    return (flags & (1 << flagnum)) != 0;
}

void TObjectInstance::ResetFlags(uint32_t newflags)
{
    uint32_t oldflags = flags;
    flags = newflags;

  // Call map pane to allow object to be placed on new lists if.. say...
  // the OF_ANIMATE or OF_PULSE flags change.
    if (sector)
        MapPane.ObjectFlagsChanged(this, oldflags, newflags);

  // Update map if object flags change...
    if (sector != nullptr) // In map
    {
      // Did walk status of tile change?
        if (!(oldflags & OF_NOWALK) && (flags & OF_NOWALK))
            MapPane.ExtractWalkmap(this);   // This doesn't seem to be working right now
        else if ((oldflags & OF_NOWALK) && !(flags & OF_NOWALK))
            MapPane.TransferWalkmap(this);  // NOWALK turned off, put tile back on map
    }
}

// ----------------- Object Instance Statistic Functions --------------

int32_t TObjectInstance::GetStat(const char *statname) const
{
    int32_t statid = cl->FindObjStat(statname);
    if (statid >= 0)
        return stats[statid];
    statid = cl->FindStat(statname);
    if (statid >= 0)
        return cl->GetStat(objtype, statid);
    return 0;
}

void TObjectInstance::SetStat(const char *statname, int32_t value)
{
    int32_t statid = cl->FindObjStat(statname);
    if (statid >= 0)
    {
        stats[statid] = value;
        return;
    }
    statid = cl->FindStat(statname);
    if (statid >= 0)
        cl->SetStat(objtype, statid, value);
}

int32_t TObjectInstance::GetStat(const char *statname, char *str, int32_t id) const
{
    char buf[MAXNAMELEN];
    if (str && id >= 0)
    {
        sprintf(buf, "%s%d%s", statname, id, str);
        statname = buf;
    }
    else if (str)
    {
        sprintf(buf, "%s%s", statname, str);
        statname = buf;
    } 
        
    return GetStat(statname);
}

// Plays a sound at the given object position
bool TObjectInstance::PlayWave(char *soundname, int32_t nr, int32_t volume, int32_t freq)
{
    S3DPoint p, mp;
    GetPos(p);
    MapPane.GetMapPos(mp);
    p -= mp;

    int32_t id = SoundPlayer.FindSound(soundname, nr);
    if (id < 0)
        return false;
    if (!SoundPlayer.Mount(id))
        return false;
    SoundPlayer.Play(id, volume, freq, &p);
    SoundPlayer.Unmount(id);

    return true;
}

// ***********************
// * Statistic Functions *
// ***********************

// Statistic entry constructor (adds the given stat to the class)
SStatEntry::SStatEntry(TObjectClass* cl, const char *statname, const char *uniqueid, int32_t statid, 
    int32_t def, int32_t min, int32_t max, bool objstat)
{
    id = statid;
    SStatisticDef statdef;
    strncpyz(statdef.name, statname, MAXNAMELEN);
    char uid[5];
    uid[0] = uid[1] = uid[2] = uid[3] = uid[4] = 0;
    strncpyz(uid, uniqueid, 5);
    statdef.uniqueid = *(uint32_t *)uid;
    statdef.def = def;
    statdef.min = min;
    statdef.max = max;
    if (objstat)
        cl->AddObjStat(statdef, id);
    else
        cl->AddStat(statdef, id);
}

// Add a new statistic
int32_t TStatisticDefList::AddStat(SStatisticDef &newstatdef, int32_t newid)
{
  // Statistic already exists.. update it
    for (int32_t c = 0; c < statdefs.NumItems(); c++)
    {
        if (!stricmp(statdefs[c].name, newstatdef.name))
        {
            memcpy(&(statdefs[c]), &newstatdef, sizeof(SStatisticDef));
            return c;
        }
    }

    if (newid >= 0)
        return statdefs.Set(newstatdef, newid);

    return statdefs.Add(newstatdef);
}

bool TStatisticDefList::ParseStat(SStatisticDef &stat, TToken &t)
{
    char uniqueidstr[5];
    uniqueidstr[0] = uniqueidstr[1] = uniqueidstr[2] = uniqueidstr[3] = 0; 


    memset(&stat, 0, sizeof(SStatisticDef));

    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return false;
    strncpyz(stat.name, t.Text(), MAXNAMELEN);

    t.WhiteGet();
    if (t.Type() != TKN_IDENT) // Old style def
    {
        if (!Parse(t, "%d %d %d", &stat.def, &stat.min, &stat.max))
            return false;

        stat.name[0] = 0; // Eliminate all old style stats!
        return true;
    }
    else
    {
        if (!Parse(t, "%5t %d %d %d", uniqueidstr, &stat.def, &stat.min, &stat.max))
            return false;

        for (char *p = uniqueidstr; *p; ++p)
            *p = (char)toupper((unsigned char)*p);
        stat.uniqueid = *(uint32_t *)uniqueidstr; // Get groovy 4 char unique id string for stats
    }

    return true;
}

const char *TStatisticDefList::GetStatDefString(int32_t statid, char *buf) const
{
    if ((uint32_t)statid >= (uint32_t)statdefs.NumItems())
        return "";

    PSStatisticDef stat = &(statdefs[statid]);

    char uniqueidstr[5];
    uniqueidstr[4] = 0;
    *(uint32_t *)uniqueidstr = stat->uniqueid;

    sprintf(buf, "%s %s %d %d %d", 
        stat->name, uniqueidstr, stat->def, stat->min, stat->max);

    return buf;
}

int32_t TStatisticDefList::FindStat(const char *statname) const
{
    for (TConstSizableIterator<SStatisticDef> i(&statdefs); i; i++)
    {
        if (stricmp(i.Item()->name, statname) == 0)
            return i.ItemNum();
    }

    return -1;
}

// ****************
// * TObjectClass *
// ****************

TObjectClass::TObjectClass(char *classname, int32_t classid, uint16_t flags, TObjectClass* base)
{
    name     = classname;
    id       = classid;
    objflags = flags;
    basedon  = base;

    classes[id] = this;

    if (id >= numclasses)
        numclasses = id + 1;

    Clear();
}

TObjectClass::~TObjectClass()
{
    Clear();
}

void TObjectClass::Clear()
{
    for (int32_t c = 0; c < objinfo.NumItems(); c++)  // Remove all object types
        RemoveType(c);

    statdefs.Clear();
    objstatdefs.Clear();
    objinfo.Clear();
}

int32_t TObjectClass::AddType(char *name, char *imgfilename, uint32_t uniqueid)
{
    TObjectBuilder* objbuilder = TObjectBuilder::GetBuilder(name);
    if (objbuilder == nullptr)
        objbuilder = TObjectBuilder::GetBuilder(ClassName());
    if (uniqueid == 0)
        uniqueid = GenerateUniqueID();
    else
        uniqueid = uniqueid;
    int32_t imageryid = TObjectImagery::RegisterImagery(imgfilename);
    if (imageryid < 0)
        return -1;

    SObjectInfo* inf = new SObjectInfo(name, objbuilder, imageryid, uniqueid);
    int32_t objtype = objinfo.AddPtr(inf);

    inf->stats.SetNumItems(statdefs.NumStats());
    for (int32_t i = 0; i < statdefs.NumStats(); i++)
        ResetStat(objtype, i);

    inf->objstats.SetNumItems(objstatdefs.NumStats());
    for (int32_t i = 0; i < objstatdefs.NumStats(); i++)
        ResetObjStat(objtype, i);

    classesdirty = true;

    return objtype;
}

bool TObjectClass::RemoveType(int32_t objtype)
{
    if ((objtype < 0) || (objtype >= objinfo.NumItems()))
        return false;

    TObjectImagery::FreeImageryEntry(objinfo[objtype].imageryid);

    objinfo.Remove(objtype);

    classesdirty = true;

    return true;
}

TObjectInstance* TObjectClass::NewObject(SObjectDef* objectdef)
{
    if (objectdef->pos.x < 0)
        objectdef->pos.x = 0;
    if (objectdef->pos.y < 0)
        objectdef->pos.y = 0;
    if (objectdef->pos.z < 0)
        objectdef->pos.z = 0;

    if ((uint32_t)((int32_t)((short)objectdef->objtype)) >= (uint32_t)objinfo.NumItems())
        return nullptr;

    if (!objinfo[objectdef->objtype].objbuilder)
        return nullptr;

    objinfo[objectdef->objtype].imagery = TObjectImagery::LoadImagery(objinfo[objectdef->objtype].imageryid);

    if (!objinfo[objectdef->objtype].imagery)
        return nullptr;

    TObjectInstance* inst = objinfo[objectdef->objtype].objbuilder->Build(objectdef,
        objinfo[objectdef->objtype].imagery);

    if (objectdef->objclass == OBJCLASS_TILE || objectdef->objclass == OBJCLASS_EXIT)
        inst->SetFlags(OF_IMMOBILE);

    if (inst->GetMapIndex() <= 0)
        inst->SetMapIndex(MapPane.MakeIndex());

    return inst;
}

int32_t TObjectClass::FindClass(char *name)
{
    for (int32_t i = 0; i < numclasses; i++)
        if (classes[i] && classes[i]->name && stricmp(classes[i]->name, name) == 0)
            return i;

    return 0;
}

int32_t TObjectClass::FindObjType(const char *objtypename, bool partial) const
{
#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(nullptr);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
    for (TConstVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (partial)
        {
            if (abbrevcmp(objtypename, i.Item()->name) > 0)
                return i.ItemNum();
        }
        else
        {
            if (!stricmp(i.Item()->name, objtypename))
                return i.ItemNum();
        }
    }

    return -1;
}

int32_t TObjectClass::FindObjType(uint32_t uniqueid) const
{
    for (TConstVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (i.Item()->uniqueid == uniqueid)
            return i.ItemNum();
    }

    return -1;
}

void TObjectClass::SetClassesDirty()
{
    classesdirty = true;
}

// -------------------- Statistic Functions ----------------------

int32_t TObjectClass::AddStat(SStatisticDef &newstatdef, int32_t newid)
{
    int32_t statid = FindStat(newstatdef.name);
    if (statid >= 0)
        return statid;
        
    statid = statdefs.AddStat(newstatdef, newid);
    if (statid < 0)
        return -1;

    for (int32_t c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].stats.New();
        objinfo[c].stats.Set(statid, newstatdef.def);
    }

    classesdirty = true;
    
    return statid;
}

void TObjectClass::DeleteStat(int32_t statid)
{
    if ((uint32_t)statid >= (uint32_t)statdefs.NumStats())
        return;

    statdefs.DeleteStat(statid);

    for (int32_t c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].stats.Collapse(statid);
    }

    classesdirty = true;
}

bool TObjectClass::ParseNewStat(TToken &t, bool reload)
{
    SStatisticDef stat;

    if (!statdefs.ParseStat(stat, t))
        return false;

  // Stat was eliminated
    if (stat.name[0] == 0)
        return true;

  // Make sure it doesn't already exist
    if (FindStat(stat.name) >= 0 || FindObjStat(stat.name) >= 0)
        return true;

    // Only Add stats if we're NOT reloading the Class.Def
    if (!reload)
        AddStat(stat);

    return true;
}

int32_t TObjectClass::FindStatVal(int32_t statid, int32_t searchvalue) const
{
    for (TConstVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (i.Item()->stats[statid] == searchvalue)
            return i.ItemNum();
    }

    return -1;
}

int32_t TObjectClass::FindRandStatVal(int32_t statid, int32_t searchvalue, int32_t *heightflux) const
// This thing is designed exclusively for the use of the editor's map generator.
{
    int32_t foundlist[64];
    int32_t numfound = 0;
    int32_t closest = 0, thisone;
    uint32_t mask;

    if (heightflux)
        *heightflux = 0;

    for (TConstVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        uint32_t value = i.Item()->stats[statid];

        // build a mask based on the wildcards in the stat value
        mask = 0;
        thisone = 0;
        if (value & 0xF0000000)
        {
            mask |= 0xFF000000;
            thisone++;
        }
        if (value & 0xF00000)
        {
            mask |= 0xFF0000;
            thisone++;
        }
        if (value & 0xF000)
        {
            mask |= 0xFF00;
            thisone++;
        }
        if (value & 0xF0)
        {
            mask |= 0xFF;
            thisone++;
        }

        if (thisone >= closest)
        {
            if (value == (searchvalue & mask))
            {
                if (thisone == closest)
                    foundlist[numfound++] = i.ItemNum();
                else
                {
                    foundlist[0] = i.ItemNum();
                    numfound = 1;
                }

                closest = thisone;
                if (heightflux)
                    *heightflux = 0;
            }
            else if (mask != 0xFFFFFFFF && heightflux)
            {
                int32_t base = 1000;
                for (int32_t j = 0; j < 4; j++)
                {
                    if (value & (0xF0 << (j * 8)))
                    {
                        int32_t v = (value >> (j * 8)) & 0x0F;

                        if (v < base)
                            base = v;
                    }
                }

                if (base < 0xF)
                {
                    for (uint32_t b = 1; b <= 1/*base*/; b++)
                    {
                        uint32_t v = value;
                        for (int32_t j = 0; j < 4; j++)
                            if (v & (0xF0 << (j * 8)) && ((v >> (j * 8)) & 0x0F) >= b)
                                v -= b << (j * 8);

                        if (v == (searchvalue & mask))
                        {
                            if (thisone == closest)
                                foundlist[numfound++] = i.ItemNum();
                            else
                            {
                                foundlist[0] = i.ItemNum();
                                numfound = 1;
                            }

                            closest = thisone;
                            if (heightflux)
                                *heightflux = b;
                        }
                    }
                }
            }
        }
    }

    if (numfound > 0)
        return foundlist[random(0, numfound - 1)];

    return -1;
}

int32_t TObjectClass::AddObjStat(SStatisticDef &newstatdef, int32_t newid)
{
  // MAKE SURE WE DON'T ABUSE THE STAT SYSTEM BY ADDING STATS TO TILES
    if (id == OBJCLASS_TILE)
        FatalError("Tile objects can't have object statistics (DUMMY)!");

    int32_t statid = FindObjStat(newstatdef.name);
    if (statid >= 0)
        return statid;
        
    statid = objstatdefs.AddStat(newstatdef, newid);
    if (statid < 0)
        return -1;

    for (int32_t c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].objstats.New();
        objinfo[c].objstats.Set(statid, newstatdef.def);
    }

    if (MapPane.IsOpen()) // Reset id's only if map is currently active
    {
        for (TMapIterator i; i; i++)
        {
            if (i.Item()->ObjClass() == id)
                i.Item()->SetObjStat(statid, newstatdef.def);
        }
    }

    classesdirty = true;
    
    return statid;
}

void TObjectClass::DeleteObjStat(int32_t statid)
{
    if ((uint32_t)statid >= (uint32_t)objstatdefs.NumStats())
        return;

    objstatdefs.DeleteStat(statid);

    for (int32_t c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].objstats.Collapse(statid);
    }

    if (MapPane.IsOpen()) // Reset id's only if map is currently active
    {
        for (TMapIterator i; i; i++)
        {
            if (i.Item()->ObjClass() == id)
                i.Item()->DelStat(statid);
        }
    }

    classesdirty = true;
}

bool TObjectClass::ParseNewObjStat(TToken &t, bool reload)
{
    SStatisticDef stat;

    if (!objstatdefs.ParseStat(stat, t))
        return false;

  // Stat was eliminated
    if (stat.name[0] == 0)
        return true;

  // Make sure it doesn't already exist
    if (FindStat(stat.name) >= 0 || FindObjStat(stat.name) >= 0)
        return true;

    // Only Add stats if we're NOT reloading the Class.Def
    if (!reload)
        AddObjStat(stat);

    return true;
}

void TObjectClass::CopyStats(const TObjectClass* from)
{
    int32_t c;
    for (c = 0; c < basedon->NumStats(); c++)
        AddStat(*from->GetStatisticDef(c), c);
    for (c = 0; c < basedon->NumObjStats(); c++)
        AddObjStat(*from->GetObjStatisticDef(c), c);
}

// -------------------- Statistic Functions ----------------------

bool TObjectClass::LoadClasses(bool lock, bool reload)
{
    char fname[MAXPATHLEN];
    FILE *classfp;
    struct stat st;

    sprintf(fname, "%sclass.def", ClassDefPath);

    classfp = TryOpen(fname, lock ? "w+" : "r");
    if (classfp == nullptr)
        return false;

    TFileParseStream s(classfp, fname);
    TToken t(s);

  // Attempt to quickload headers (if IMAGERY.DAT file is older than CLASS.DEF)
  // Note: might be nice to quickly do a date/time stamp search of the IMAGERY directory
  // and use the lastest time stamp from that.
    if (!NoQuickLoad && !reload)
    {
        if (stat(fname, &st) == 0)
            TObjectImagery::QuickLoadHeaders(st.st_mtime);
    }

  // Now get first token... 
    t.LineGet();

    // See if they are reloading the Class.Def
    if (reload)
    {
        uint16_t tmp;

        // Read in the Unique Type ID into a tmp variable
        if (!Parse(t, "Unique Type ID = %d", &tmp))
            FatalError("Reading Unique Type ID from class.def");

        // Compare it against the UniqueTypeID that the program has already been
        // using and use whichever is larger
        if ((tmp > UniqueTypeID) || ((tmp < 32000) && (UniqueTypeID > 32000)))
            UniqueTypeID = tmp;
    }
    else
    {
        // Read in the Unique Type ID
        if (!Parse(t, "Unique Type ID = %d", &UniqueTypeID))
            FatalError("Reading Unique Type ID from class.def");
    }
    t.LineGet();


    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        if (t.Is("CLASS"))
        {
            if (!ParseClass(t, reload))
                return false;
        }
        else
        {
            Error("Script identifier expected in class.def.");
            return false;
        }

    } while (t.Type() != TKN_EOF);

    if (!lock)
        fclose(classfp);

    classesdirty = false;

    return true;
}

bool TObjectClass::SaveClasses(bool lock)
{
    if (!classesdirty)
        return true;

    char fname[MAXPATHLEN];
    char bakname[MAXPATHLEN];
    char newname[MAXPATHLEN];
    FILE *classfp;

    // Reload the Class.Def to read any types that have been added since we
    // loaded it
    if (LoadClasses(false, true) == false)
        return false;

    sprintf(fname, "%sClass.Def", ClassDefPath);
    sprintf(bakname, "%sClass.Bak", ClassDefPath);
    sprintf(newname, "%sClass.New", ClassDefPath);

    // Try to open the file
    classfp = TryOpen(newname, "w+");

    // If the file didn't open, then return with failure
    if (classfp == nullptr)
        return false;

    fprintf(classfp, "// ********* Revenant Class Def Save File ********\n"
                "// -----------------------------------------------\n\n"
                "// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

    fprintf(classfp, "Unique Type ID = 0x%04x\n\n", UniqueTypeID);

    for (int32_t i = 0; i < numclasses; i++)
    {
        TObjectClass* cl = classes[i];
        if (cl)
            if (!cl->WriteClass(classfp))
                return false;
    }

    if (!lock)
    {
        fclose(classfp);

        // Delete the Class.Bak file (if it exists)
        if (TryDelete(bakname) == false)
            return false;

        // Rename Class.Def to Class.Bak
        if (TryRename(fname, bakname) == false)
            return false;

        // Rename Class.New to Class.Def
        if (TryRename(newname, fname) == false)
            return false;
    }

    classesdirty = false;

    return true;
}

void TObjectClass::FreeClasses()
{
    for (int32_t c = 0; c < numclasses; c++)
    {
        if (classes[c])
            classes[c]->Clear();
    }
}

bool TObjectClass::WriteClass(FILE *fp)
{
    // header
    fprintf(fp, "CLASS \"%s\"\nBEGIN\n\n", name);
    char buf[80];

    // stats
    if (statdefs.NumStats() > 0)
    {
        fprintf(fp, "\n\t// Stat Defs: <StatName> <FourCharId> <Default> <Min> <Max>\n\tSTATS\n\tBEGIN\n");

        for (int32_t c = 0; c < statdefs.NumStats(); c++)
            fprintf(fp, "\t\t%s\n", statdefs.GetStatDefString(c, buf));

        fprintf(fp, "\tEND\n\n");
    }

    // object stats
    if (objstatdefs.NumStats() > 0)
    {
        fprintf(fp, "\n\t// Stat Defs: <StatName> <FourCharId> <Default> <Min> <Max>\n\tOBJSTATS\n\tBEGIN\n");

        for (int32_t c = 0; c < objstatdefs.NumStats(); c++)
            fprintf(fp, "\t\t%s\n", objstatdefs.GetStatDefString(c, buf));

        fprintf(fp, "\tEND\n\n");
    }

    // types
    if (objinfo.NumItems() > 0)
    {
        fprintf(fp, "\n\tTYPES\n\tBEGIN\n");

        int32_t objtype = 0;
        for (TVirtualIterator<SObjectInfo> o(&objinfo); o; o++, objtype++)
        {
            if (!o.Item() || !o.Used())
                continue;

            SImageryEntry* ie = TObjectImagery::GetImageryEntry(o.Item()->imageryid);
            if (!ie)
                continue;

            char *filename = ie->filename;

            if (strncmp(filename, "IMAGERY", 7) == 0)
            {
                int32_t id = atoi(filename + 8);
                fprintf(fp, "\t\t\"%s\" %03d 0x%08x", o.Item()->name, id, o.Item()->uniqueid);
            }
            else
                fprintf(fp, "\t\t\"%s\" \"%s\" 0x%08x", o.Item()->name,
                    filename, o.Item()->uniqueid);

            if (statdefs.NumStats() > 0)
            {
                int32_t c;
                bool firsttime;

                fprintf(fp, " {");
                firsttime = true;
                for (c = 0; c < statdefs.NumStats(); c++)
                {
                    if (!firsttime)
                        fprintf(fp, ",");

                    fprintf(fp, "%d", (int32_t)objinfo[objtype].stats[c]);
                    firsttime = false;
                }
                fprintf(fp, "}");

                fprintf(fp, " {");
                firsttime = true;
                for (c = 0; c < objstatdefs.NumStats(); c++)
                {
                    if (!firsttime)
                        fprintf(fp, ",");

                    fprintf(fp, "%d", (int32_t)objinfo[objtype].objstats[c]);
                    firsttime = false;
                }
                fprintf(fp, "}");
            }

            fprintf(fp, "\n");
        }

        fprintf(fp, "\tEND\n\n");
    }

    if (fprintf(fp, "END\n\n") < 1)
        return false;

    return true;
}

bool TObjectClass::ParseClass(TToken &t, bool reload)
{
#ifdef _DEBUG
    _CrtMemState memstate;
#endif

    t.WhiteGet();
    if (t.Type() != TKN_TEXT)
    {
        Error("Expecting class name");
        return false;
    }

    TObjectClass* cl = TObjectClass::GetClass(TObjectClass::FindClass((char *)t.Text()));
    if (!cl)
    {
        fprintf(stderr, "Class %s not found\n", t.Text());
        t.LineGet();
        if (!t.SkipBlock())
            Error("Unexpected EOF");
        return true;
    }

    t.LineGet();
    t.DoBegin();

  // Copy the base class's hard coded stats
    if (cl->basedon)
        cl->CopyStats(cl->basedon);

    while (!t.IsEnd())
    {
#ifdef _DEBUG
        _CrtMemCheckpoint(&memstate);
#endif
        if (t.Type() == TKN_EOF)
            Error("Unexpected EOF");

        if (t.Type() != TKN_IDENT)
        {
            t.WhiteGet();
            continue;
        }

      // Get class stats
        if (t.Is("STATS"))
        {
            char *staterr = "Parsing class.def: STATS";

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                if (!cl->ParseNewStat(t, reload))
                    t.Error(staterr);
                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif
        }

      // Get object stats
        else if (t.Is("OBJSTATS"))
        {
            char *staterr = "Parsing class.def: OBJSTATS";

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                if (!cl->ParseNewObjStat(t, reload))
                    t.Error(staterr);
                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif

        }
        else if (t.Is("TYPES"))
        {
#ifdef _DEBUG
            _CrtMemCheckpoint(&memstate);
#endif

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                char filename[MAXIMFNAMELEN];
                char name[MAXNAMELEN];
                int32_t objtype;
                bool newtype = false;
                uint32_t uniqueid;

                strcpy(name, t.Text());

                if (t.Type() != TKN_TEXT)
                    t.Error("Parsing class.def: TYPES (header)");

                t.WhiteGet();
                if (t.Type() == TKN_NUMBER)
                {
                    sprintf(filename, "IMAGERY.%03d", t.Index());
                }
                else if (t.Type() == TKN_TEXT)
                    strcpy(filename, t.Text());
                else
                    t.Error("Parsing class.def: TYPES (imagery)");

                t.WhiteGet();

                // Get the Unique ID for this type
                if (t.Type() != TKN_NUMBER)
                    t.Error("Missing Unique ID in class.def: TYPES (imagery)");
                else
                {
                    if (!Parse(t, "%d", &uniqueid))
                        t.Error("Parsing Unique ID in class.def: TYPES (imagery)");
                }

                // See if we are reloading the Class.Def
                if (reload)
                {
                    // See if the type already exists
                    objtype = cl->FindObjType(name);

                    if (objtype == -1)
                    {
                        // It doesn't exist, so add it
                        newtype = true;

                        objtype = cl->AddType(name, filename, uniqueid);
                    }
                }
                else
                {
                    objtype = cl->AddType(name, filename, uniqueid);
                }

                if (objtype < 0)
                {
                    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                        t.Get();
                    t.LineGet();
                    continue;
                }
                
                int32_t statid, value;

                char *typestaterr = "Parsing class.def: TYPES (stats)";

             // Parse old style statistic list
                while (t.Type() != TKN_RETURN && !t.Is("{"))
                {
                    char statname[80];

                    if (!Parse(t, "%t = %d", statname, &value))
                        t.Error(typestaterr);

                    if ((!reload) || (newtype))
                    {
                        statid = cl->FindStat(statname);
                        if (statid >= 0)
                            cl->SetStat(objtype, statid, value);
                        else
                        {
                            statid = cl->FindObjStat(statname);
                            if (statid >= 0)
                                cl->SetObjStat(objtype, statid, value);
                        }
//                      if (statid < 0)
//                          Error("Invalid stat '%s'", statname);
                    }

                }

             // Parse class statistics
                if (t.Is("{")) // Use bracket style class def stat list
                {
                    int32_t statid = 0;
                    t.WhiteGet();
                
                    while (!t.Is("}") && statid < cl->NumStats())
                    {
                        int32_t value;
                        
                        if (!Parse(t, "%i", &value))
                            t.Error(typestaterr);
                        
                        if (!reload)
                        {
                            cl->objinfo[objtype].stats.New();
                            cl->objinfo[objtype].stats[statid] = value;
                        }

                        statid++;
        
                        if (t.Is(","))
                            t.WhiteGet();

                    }
                    
                    while (!t.Is("}") && !(t.Type() == TKN_RETURN || t.Type() == TKN_EOF))
                    {
                        t.WhiteGet(); // Skip any extra stats on list without erroring (just in case)

                    }

                    if (!t.Is("}"))
                        t.Error(typestaterr);

                    t.WhiteGet();
                }

              // Parse object statistics
                if (t.Is("{")) // Do object Use bracket style class def stat list
                {
                    int32_t statid = 0;
                    t.WhiteGet();
                
                    while (!t.Is("}") && statid < cl->NumObjStats())
                    {
                        int32_t value;
                        
                        if (!Parse(t, "%i", &value))
                            t.Error(typestaterr);
                        
                        if (!reload)
                        {
                            cl->objinfo[objtype].objstats.New();
                            cl->objinfo[objtype].objstats[statid] = value;
                        }

                        statid++;
        
                        if (t.Is(","))
                            t.WhiteGet();

                    }
                    
                    while (!t.Is("}") && !(t.Type() == TKN_RETURN || t.Type() == TKN_EOF))
                    {
                        t.WhiteGet(); // Skip any extra stats on list without erroring (just in case)

                    }

                    if (!t.Is("}"))
                        t.Error(typestaterr);

                    t.WhiteGet();

                }

                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif
        }
        else
        {
            Error("Expecting STATS or TYPES for class %s", cl->ClassName());
            return false;
        }
    }

    t.DoEnd();

    return true;
}

// GenerateUniqueID.
//------------------------------------------------------------------------------
// This will return a Unique ID used for specifying unique object
// types.  The number is generated as follows:
//
// (high word) (high byte) = Random #
// (high word) (low  byte) = Low byte of Current Time (tick count)
// (low  word)             = UniqueTypeID stored in Class.Def
//
// UniqueTypeID is incremented on every call.

uint32_t GenerateUniqueID()
{
    uint32_t UniqueID;
    uint32_t r = (uint32_t)random(0, 255);
    uint32_t t = tickcount() & 255;

    UniqueID = (r << 24) | (t << 16) | UniqueTypeID;

    UniqueTypeID++;

    return UniqueID;
}

