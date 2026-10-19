// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       bossai.cpp - the four bosses' behaviours (AI_PerMonster)        *
// *************************************************************************
//
// Baez, Solifuge, Jhaga and Yhagoro: the cases of retail's AI_PerMonster
// (0x004c9b70), which AI runs for an aggressive character before its own
// (TCharacter::AIPerMonster in character.cpp picks the case). Each boss
// keeps its state in TCharacter::bossstate (bossai.h). Forensics, with
// every address: docs/gameplay/forensics/BOSS_AI.md; kata C3c (`melee-boss`)
// holds this file to the original.

#include "game/ai/bossai.h"

#include "game/imagery/3dimage.h"
#include "game/object/character.h"
#include "game/world/mappane.h"
#include "game/object/player.h"
#include "game/ui/playscreen.h"
#include "game/app/revutils.h"

#include <array>
#include <cstdlib>
#include <strings.h>

namespace
{

// The bosses' inline distance (BOSS_AI.md §1), not Distance's table
// (0x0046de60): dx + dy less half the smaller, on the map plane.
int32_t RoughDistance(const S3DPoint& a, const S3DPoint& b)
{
    const int32_t dx = std::abs(a.x - b.x);
    const int32_t dy = std::abs(a.y - b.y);
    const int32_t smaller = dx < dy ? dx : dy;
    return dy - (smaller >> 1) + dx;
}

bool NameIs(const char* name, const char* wanted)
{
    return name && !strcasecmp(name, wanted);
}

// REVSYNC: 0x004c9af0 -- an effect of `type` at `pos`, on the pane's level.
void MakeEffect(const char* type, const S3DPoint& pos)
{
    SObjectState def{};
    def.objtype = static_cast<short>(TObjectClass::GetClass(OBJCLASS_EFFECT)->FindObjType(type));
    if (def.objtype < 0)
        return;
    def.objclass = OBJCLASS_EFFECT;
    def.level = static_cast<uint16_t>(MapPane.GetMapLevel());
    def.pos = pos;
    MapPane.NewObject(&def);
}

TComplexObject* ComplexAt(int32_t mapindex)
{
    return dynamic_cast<TComplexObject*>(MapPane.GetInstance(mapindex));
}

// Baez's crowd (retail's table at 0x005df54c), by their own names.
constexpr std::array<const char*, SBaezState::kWatchers> kBaezCrowd{
    "mark", "rex", "vince", "redo", "paul", "matt", "austin", "hersh", "carl", "riz", "jason2"};

// The crowd's moods: a watcher's root, what it plays when the player is
// hurt, and when Baez is (or the player heals).
struct SCrowdMood
{
    const char* root;
    const char* cheer;
    const char* boo;
};
constexpr std::array<SCrowdMood, 3> kCrowdMoods{{
    {"watcha", "cheera", "booa"}, {"watchb", "cheerb", "boob"}, {"watchc", "cheerc", "booc"}}};

}  // namespace

// REVSYNC: the walk AI_PerMonster's bosses make of the map (0x0044cf80 with
// no rect, flags 0: the pane's 3x3 sector window, then 0x0044d080), in map
// order.
std::vector<TObjectInstance*> TCharacter::PaneObjects()
{
    if (paneObjectsSeam)
        return paneObjectsSeam(this);
    std::vector<TObjectInstance*> objects;
    for (TMapIterator i; i; i++)
        objects.push_back(i);
    return objects;
}

// *******************************************************************
// * Baez (kind 1, 0x004c9ce8): the ring fight and his fake death    *
// *******************************************************************

// REVSYNC: 0x004c9ce8 -- unkillable and unable to kill; the crowd found once
// (the objects bearing its eleven names) and set to watch; pinned at 1
// health once he has fallen; then the stuns above 1 health, the fall at 1.
void TCharacter::BossBaez(SBaezState& s)
{
    charflags |= kCharFlagUnkillable | kCharFlagNoKill;
    if (!s.watchersfound)
    {
        for (TObjectInstance* o : PaneObjects())
            for (int32_t i = 0; i < SBaezState::kWatchers; ++i)
                if (NameIs(o->GetName(), kBaezCrowd[i]))
                    s.watchers[i] = dynamic_cast<TComplexObject*>(o);
        s.watchersfound = 1;
        s.health = Health();
        s.playerhealth = 0;
        if (Player)
            s.playerhealth = Player->Health();
    }
    BaezCrowd(s);
    if (s.stage)
    {
        SetHealth(1);
        Try("walk");
    }
    if (Health() > 1)
        BaezStuns(s);
    else
        BaezFall(s);
}

// REVSYNC: 0x004c9da7 -- the crowd turns to the middle of the fight; it
// cheers a blow to the player and boos one to Baez (or the player healing),
// cheering first. Then this tick's healths for the next.
void TCharacter::BaezCrowd(SBaezState& s)
{
    const int32_t health = Health();
    int32_t playerhealth = 0;
    if (Player)
        playerhealth = Player->Health();
    bool cheer = false;
    bool boo = false;
    if (playerhealth < s.playerhealth)
        cheer = true;
    else if (playerhealth > s.playerhealth)
        boo = true;
    if (health < s.health)
        boo = true;

    const S3DPoint player = Player ? IntPos(Player) : S3DPoint{};
    const S3DPoint mid((player.x + IntPos(*this).x) / 2, (IntPos(*this).y + player.y) / 2, (IntPos(*this).z + player.z) / 2);
    for (const TSafeRef<TComplexObject>& ref : s.watchers)
    {
        TComplexObject* watcher = ref.Get();
        if (!watcher)
            continue;
        if (!watcher->GetAnimator())
            watcher->CreateAnimator();
        watcher->Face(ConvertToFacing(IntPos(watcher), mid));
        for (const SCrowdMood& mood : kCrowdMoods)
            if (watcher->IsRoot(mood.root))
            {
                if (cheer)
                    watcher->Try(mood.cheer);
                else if (boo)
                    watcher->Try(mood.boo);
                break;
            }
    }
    s.health = Health();
    if (Player)
        s.playerhealth = Player->Health();
}

// REVSYNC: 0x004cb5dd -- above 1 health: a stun pause as he drops through
// each 20% of his health (each band needs the one before), 72 ticks out of
// the fight; the stance back afterwards.
void TCharacter::BaezStuns(SBaezState& s)
{
    if (Health() <= 1)
        return;
    const int32_t pct = Health() * 100 / MaxHealth();
    struct SStun
    {
        int32_t level, before;
    };
    constexpr std::array<SStun, 4> kStuns{{{80, 0}, {60, 80}, {40, 60}, {20, 40}}};
    for (const SStun& stun : kStuns)
        if (pct <= stun.level && s.stunlevel == stun.before)
        {
            s.stunlevel = stun.level;
            s.stunticks = 72;
            s.stunned = 1;
            Try("combat to stunned");
        }

    if (!s.stunned)
    {
        if (IsDoing("stunned"))
            Try("stunned to combat");
        return;
    }
    charflags |= kCharFlagNoTurn;
    SetFighting(nullptr);
    if (IsDoing("combat"))
        Try("combat to stunned");
    else if (IsDoing("combat to stunned"))
        Try("stunned");
    else
        --s.stunticks;
    if (s.stunticks < 1)
    {
        s.stunned = 0;
        charflags &= ~kCharFlagNoTurn;
        s.stunticks = 0;
        Try("stunned to combat");
    }
}

// REVSYNC: 0x004ca20e -- at 1 health: the fall set up (its first tick), the
// walk to the table, the throw, then death.
void TCharacter::BaezFall(SBaezState& s)
{
    charflags |= kCharFlagNoTurn;
    const S3DPoint me = IntPos(*this);
    if (s.stage == 0 && !BaezSetUpFall(s))
        return;
    if (s.stage == 1)
        BaezWalk(s, me);
    if (s.stage == 2)
        BaezThrow(s);
    if (s.stage == 3)
    {
        Force("cdead");
        charflags &= ~kCharFlagUnkillable;
        SetHealth(0);
        MapPane.CenterOnObj(Player, CENTERON_SCROLL);
    }
}

// REVSYNC: 0x004ca241 -- the ring wall stops blocking the walk, the crowd
// leaves, and from the table ("ForBreakTable") the three spots; Baez falls
// to play dead with the camera on him. No table: he is no boss any more.
bool TCharacter::BaezSetUpFall(SBaezState& s)
{
    s.firstspot = S3DPoint{};
    for (TObjectInstance* o : PaneObjects())
        if (NameIs(o->GetName(), "ringwall"))
        {
            o->SetFlag(OF_NOWALK);
            break;
        }
    for (const TSafeRef<TComplexObject>& ref : s.watchers)
        if (TComplexObject* watcher = ref.Get())
            watcher->Try("walk");
    s.stage = 1;

    TObjectInstance* table = nullptr;
    for (TObjectInstance* o : PaneObjects())
        if (NameIs(o->GetTypeName(), "ForBreakTable"))
        {
            table = o;
            break;
        }
    if (!table)
    {
        monsterkind = -1;
        charflags &= ~(kCharFlagNoTurn | kCharFlagUnkillable);
        return false;
    }
  // (Retail sets firstspot only while it is (0, 0, 0): always, cleared above.)
    const S3DPoint t = IntPos(table);
    s.firstspot = S3DPoint(t.x + 0x20, t.y + 0x1b0, t.z + 0x58);
    s.playerspot = S3DPoint(t.x, t.y + 0xa0, t.z);
    s.baezspot = S3DPoint(t.x, t.y + 0x15e, t.z);
    s.table = table->GetMapIndex();
    doing->priority = false;
    Force("combat to fakedead");
    Try("fakedead");
    MapPane.CenterOnObj(this, CENTERON_SCROLL);
    s.falling = 1;
    return true;
}

// REVSYNC: 0x004ca538 -- the player out of the fight and out of control is
// walked to the spot before the table, then to the one beside it, and
// turned his back to Baez; Baez plays dead, gets up and walks to his spot
// behind him, where the throw begins.
void TCharacter::BaezWalk(SBaezState& s, const S3DPoint& me)
{
    if (Player)
        Player->EndFighting();
    PlayScreen.SetControlOn(false);
    if (!GetAnimator())
        CreateAnimator();
    Face(ConvertToFacing(me, s.baezspot));
    S3DPoint spot = s.playerarrived ? s.playerspot : s.firstspot;
  // Retail reads whatever its stack holds for a missing player here.
    const S3DPoint player = Player ? IntPos(Player) : S3DPoint{};
    if (!s.playerarrived && RoughDistance(player, spot) < 32)
    {
      // On this tick the spot is cleared before the test below: the player
      // is sent toward (0, 0) for a tick, as retail sends him.
        s.playerarrived = 1;
        spot = S3DPoint{};
    }

    if (RoughDistance(player, spot) < 32)
    {
        if (Player)
        {
            Player->MoveTo(spot);
            Player->Stop();
            Player->Try("walk");
            Player->Face((ConvertToFacing(player, me) - 0x80) & 0xff);
        }
        const int32_t near = RoughDistance(me, s.baezspot);
        if (near < 0x80)
            s.waitticks = 1000;
        if (near < 0x20)
        {
            const TObjectInstance* table = MapPane.GetInstance(s.table);
            Face(ConvertToFacing(me, table ? IntPos(table) : S3DPoint{}));
            S3DPoint baezspot = s.baezspot;
            MoveTo(baezspot);
            Try("walkf");
            Stop();
            if (Player)
                Player->Face(ConvertToFacing(player, me));
            Force("combat to cdead");
            Try("cdead");
            if (IsDoing("combat to cdead"))
            {
                s.stage = 2;
                s.frame = 0;
                s.throwlength = imagery->GetAniLength(FindState("combat to cdead", -1));
            }
        }
        else
            BaezWalkOn(s, me, true);
    }
    else
    {
        s.waitticks = 0x30;
        if (s.falling)
        {
            if (IsDoing("combat to fakedead"))
                Try("fakedead");
            else
            {
                Force("fakedead");
                if (!IsDoing("combat to fakedead") && !IsDoing("fakedead"))
                {
                    Force("combat to fakedead");
                    Try("fakedead");
                }
            }
            if (IsDoing("fakedead"))
                s.falling = 0;
        }
        else if (!IsDoing("fakedead"))
            BaezWalkOn(s, me, false);
        if (Player)
        {
            Player->Try("walk");
            Player->Goto(spot.x, spot.y, nullptr);
        }
    }
    if (s.waitticks)
        --s.waitticks;
}

// REVSYNC: 0x004ca8e1 (the player there) / 0x004caedf (walking) -- Baez
// walks to his spot: out of "fakedead", into the walk; falling again from
// the stance while the walking rests (its test the other way round in the
// two copies retail has); then the running walk, or the stance once it
// rests no more.
void TCharacter::BaezWalkOn(SBaezState& s, const S3DPoint& me, bool playerthere)
{
    const int32_t angle = ConvertToFacing(me, s.baezspot);
    Goto(s.baezspot.x, s.baezspot.y, nullptr);
    Face(angle);
    if (IsDoing("combat to fakedead"))
        return;
    if (IsDoing("fakedead"))
        Try("fakedead to walk");
    if (IsDoing("fakedead to walk"))
        Try("walk");
    const bool fallagain = playerthere ? s.waitticks == 0 : s.waitticks != 0;
    if (IsDoing("combat") && fallagain)
    {
        Force("combat to fakedead");
        Try("fakedead");
    }
    if (s.waitticks == 0)
        Try("combatf");
    else if (IsDoing("walk"))
        Try("walkf");
}

// REVSYNC: 0x004cb3fc -- the throw: the player's "sunsetflipper" at its 4th
// tick (neither may be targeted while it plays), the table broken with a
// flash 53/150 of the way through Baez's fall, and the end.
void TCharacter::BaezThrow(SBaezState& s)
{
    if (s.frame == 4 && Player)
    {
        Player->Force("sunsetflipper");
        Player->charflags |= kCharFlagNotTargetable;
        charflags |= kCharFlagNotTargetable;
    }
    if (s.frame == s.throwlength * 53 / 150)
    {
        if (TObjectInstance* table = MapPane.GetInstance(s.table))
            table->SetState(1);
        EffectCombatFlash();
    }
    if (s.frame >= s.throwlength)
    {
        s.stage = 3;
      // (Retail doesn't test for a player here.)
        if (Player)
            Player->charflags &= ~kCharFlagNotTargetable;
        charflags &= ~kCharFlagNotTargetable;
    }
    ++s.frame;
}

// *******************************************************************
// * Solifuge (kind 2, 0x004cbb1d): the stun, the sky walk, the brood *
// *******************************************************************

// REVSYNC: 0x004cbb1d -- can't land a killing blow; while the player lies
// dead (and Solifuge isn't in the stance) the death screen waits. A lost
// band of health makes him roar; at no health his brood dies with him.
// Paralysed he turns blue and his stun is armed; stunned he stays down for
// 72 ticks; landing from the roof he slams the ground, hurting the player
// near. Then the sky walk.
void TCharacter::BossSolifuge(SSolifugeState& s)
{
    charflags |= kCharFlagNoKill;
  // Without a player retail never reads this (the slam and the sky walk's
  // facing ask for one, or read it as garbage).
    S3DPoint player{};
    if (Player)
    {
        if (Player->Health() < 1 && !IsDoing("combat"))
        {
            Player->charflags |= kCharFlagDeathHold;
            return;
        }
        Player->charflags &= ~kCharFlagDeathHold;
        player = IntPos(Player);
    }
    const S3DPoint me = IntPos(*this);
    const uint32_t objflags = flags;
    const int32_t pct = Health() * 100 / MaxHealth();
    if (pct > 66 && pct <= 100)
        s.band = 0;
    else if (pct > 33 && pct <= 66)
    {
        if (s.band == 0)
            s.pissedoff = 1;
        s.band = 1;
    }
    else
    {
        if (s.band == 1)
            s.pissedoff = 1;
        s.band = 2;
    }
    if (Health() < 1)
        for (const int32_t spider : s.brood)
            if (TObjectInstance* o = MapPane.GetInstance(spider))
                o->SetHealth(0);

    if (objflags & OF_PARALIZE)
    {
        charflags &= ~kCharFlagDamageSeventh;
        if (s.band > 0)
            s.skytimer = 240;
        s.stunticks = 72;
        s.stunned = 1;
        if (!s.tinted)
        {
            TintMaterials();
            s.tinted = 1;
        }
        return;
    }
    if (s.tinted)
    {
        UntintMaterials();
        s.tinted = 0;
    }

    if (!s.stunned)
    {
        if (!s.slam)
            charflags &= ~(kCharFlagNotTargetable | kCharFlagNoTurn);
        charflags |= kCharFlagDamageSeventh;
        if (s.pissedoff)
        {
            if (IsDoing("pissedoff"))
                s.pissedoff = 0;
            else
                Try("pissedoff");
        }
        if (IsDoing("pissedoff") && !s.slam)
            charflags |= kCharFlagNotTargetable;
        else
            charflags &= ~kCharFlagNotTargetable;
        if (s.slam && (IsDoing("combat") || IsDoing("combatf")))
        {
            s.slam = 0;
            s.stunned = 1;
            charflags = (charflags & ~(kCharFlagNotTargetable | kCharFlagUnkillable)) | kCharFlagNoTurn;
            if (RoughDistance(me, player) < Radius() * 3 && Player)
            {
                char impact[RESNAMELEN];
                Player->CombatAnimName(impact, "impk");
                Player->Try(impact);
                static_cast<TObjectInstance*>(Player)->Damage(chardata->weapondamage * 2, DT_MISC);
            }
        }
    }
    else
    {
        charflags = (charflags & ~(kCharFlagNotTargetable | kCharFlagDamageSeventh)) | kCharFlagNoTurn;
        if (!IsDoing("stunned") && !IsDoing("skywalk to combat") && !IsDoing("skywalk"))
        {
            doing->priority = false;
            Force("stunned");
        }
        if (--s.stunticks < 1)
        {
            s.stunned = 0;
            s.stunticks = 72;
            doing->priority = false;
            Force("combat");
        }
    }
    SolifugeSkyWalk(s, me, player);
}

// REVSYNC: 0x004cc113 -- the timer runs on the ground; at 0 he goes up
// (every 480 ticks in the lowest band) for 120 ticks (240 in the lowest),
// untouchable, facing the player; in the lowest band he calls his brood
// 48 ticks up and wakes it 24 later; down, the slam waits for the stance.
void TCharacter::SolifugeSkyWalk(SSolifugeState& s, const S3DPoint& me, const S3DPoint& player)
{
    if (s.skytimer && !s.skyphase)
    {
        --s.skytimer;
        if (IsDoing("skywalk") || IsDoing("skywalkf"))
            Try("skywalk to combat");
        if (!s.skytimer && !s.skyphase)
        {
            s.skyphase = 1;
            if (s.band > 1)
                s.skytimer = 480;
        }
    }
    if (s.skyphase == 1)
    {
        doing->priority = false;
        Force("combat to skywalk");
        Try("skywalk");
        s.skyphase = 2;
        s.skyticks = s.band > 1 ? 240 : 120;
        s.landing = 0;
    }
    if (s.skyphase != 2)
        return;

    charflags |= kCharFlagNotTargetable | kCharFlagNoTurn | kCharFlagUnkillable;
    if (IsDoing("skywalkf"))
        charflags |= kCharFlagSkyWalkf;
    else
        charflags &= ~kCharFlagSkyWalkf;
    if (--s.skyticks == 192 && s.band > 1)
        SolifugeBrood(s);
    if (s.skyticks == 168 && s.band > 1)
        for (const int32_t spider : s.brood)
            if (TComplexObject* o = ComplexAt(spider); o && o->IsDoing("sleeping"))
                o->Try("sleeping to combat");
    if (s.skyticks < 1)
    {
        Force("skywalk to combat");
        Try("combat");
        if (IsDesired("combat"))
            SetRoot(desired);
        s.skyphase = 0;
        s.slam = 1;
        s.stunned = 0;
        s.stunticks = 48;
        s.skytimer = 240;
        charflags &= ~kCharFlagSkyWalkf;
        return;
    }
    Face(ConvertToFacing(me, player));
    if (s.skyticks == 16)
        s.landing = 1;
    if (s.landing)
    {
        Try("skywalk");
        charflags &= ~kCharFlagSkyWalkf;
    }
    else
        Try("skywalkf");
    if (IsDoing("combat to skywalk"))
        charflags &= ~kCharFlagSkyWalkf;
}

// REVSYNC: 0x004cc39f -- for each slot whose spider is gone or dead, a new
// one asleep round him: a big spider (Arakna or Recluse) in the first, a
// little one (Araknid or Artikulid) in the others, each within four radii
// of him in x and y.
void TCharacter::SolifugeBrood(SSolifugeState& s)
{
    for (size_t slot = 0; slot < s.brood.size(); ++slot)
    {
        if (TObjectInstance* spider = MapPane.GetInstance(s.brood[slot]); spider && spider->Health() > 0)
            continue;
        const char* type = slot == 0 ? (random(0, 1) ? "Arakna" : "Recluse")
                                     : (random(0, 1) ? "Araknid" : "Artikulid");
        SObjectState def{};
        int32_t index = 0;
        TObjectInstance* made = nullptr;
        def.objtype = static_cast<short>(TObjectClass::GetClass(OBJCLASS_CHARACTER)->FindObjType(type));
        if (def.objtype >= 0)
        {
            def.level = static_cast<uint16_t>(MapPane.GetMapLevel());
            def.pos = IntPos(*this);
            def.objclass = OBJCLASS_CHARACTER;
            const int32_t xoffset = random(0, Radius() * 8);
            def.pos.x += xoffset - Radius() * 4;
            const int32_t yoffset = random(0, Radius() * 8);
            def.pos.y += yoffset - Radius() * 4;
            index = MapPane.NewObject(&def);
            made = MapPane.GetInstance(index);
        }
        s.brood[slot] = index;
        if (auto* spider = dynamic_cast<TComplexObject*>(made))
            spider->Try("sleeping");
    }
}

// REVSYNC: 0x004d6a30 -- every material of the imagery saved (diffuse and
// emissive) and blued: diffuse (0.1, 0.1, 1.0), emissive (0.2, 0.2, 1.0);
// once (nothing while a set is saved). The materials are the imagery's,
// shared by its every instance, as retail's are.
void TCharacter::TintMaterials()
{
    if (tintSeam)
    {
        tintSeam(this, true);
        return;
    }
    auto* im = dynamic_cast<T3DImagery*>(imagery);
    if (!savedmaterials.empty() || !im)
        return;
    for (int32_t i = 0; i < im->NumMaterials(); ++i)
    {
        S3DMat mat{};
        im->GetMaterial(i, &mat);
        S3DMaterial& m = mat.matdesc;
        savedmaterials.push_back({m.diffuse.r, m.diffuse.g, m.diffuse.b, m.emissive.r, m.emissive.g, m.emissive.b});
        m.diffuse.r = 0.1f;
        m.diffuse.g = 0.1f;
        m.diffuse.b = 1.0f;
        m.emissive.r = 0.2f;
        m.emissive.g = 0.2f;
        m.emissive.b = 1.0f;
        im->SetMaterial(i, &mat);
    }
}

// REVSYNC: 0x004d6b40 -- the saved materials put back, the copy let go.
void TCharacter::UntintMaterials()
{
    if (tintSeam)
    {
        tintSeam(this, false);
        return;
    }
    auto* im = dynamic_cast<T3DImagery*>(imagery);
    if (savedmaterials.empty() || !im)
        return;
    for (int32_t i = 0; i < im->NumMaterials() && i < (int32_t)savedmaterials.size(); ++i)
    {
        S3DMat mat{};
        im->GetMaterial(i, &mat);
        const std::array<float, 6>& saved = savedmaterials[i];
        mat.matdesc.diffuse.r = saved[0];
        mat.matdesc.diffuse.g = saved[1];
        mat.matdesc.diffuse.b = saved[2];
        mat.matdesc.emissive.r = saved[3];
        mat.matdesc.emissive.g = saved[4];
        mat.matdesc.emissive.b = saved[5];
        im->SetMaterial(i, &mat);
    }
    savedmaterials.clear();
}

// *******************************************************************
// * Jhaga (kind 3, 0x004ccaa4): the priest's teleports              *
// *******************************************************************

// REVSYNC: 0x004ccaa4 -- set up round four waypoints; rest 240 ticks, then a
// cycle of five jumps 72 ticks apart (the corners crosswise, then the
// centre): out in a flash, in at the next point, a fireball at the player
// from each corner and the fire wind from the centre.
void TCharacter::BossJhaga(SJhagaState& s)
{
    if (!GetAnimator())
        CreateAnimator();
    if (s.mode == 0 && !JhagaSetUp(s))
        return;
    if (s.mode == 1 && --s.resttimer == 0)
    {
        s.resttimer = 240;
        s.mode = 2;
        s.draw = random(0, 2);
        charflags |= kCharFlagNoTurn;
        s.hop = 0;
        s.frame = 0;
    }
    if (s.mode != 2)
        return;

    s.draw = 0;
    switch (s.hop)
    {
        case 0: s.dest = s.points[0]; break;
        case 1: s.dest = s.points[3]; break;
        case 2: s.dest = s.points[1]; break;
        case 3: s.dest = s.points[2]; break;
        case 4: s.dest = s.points[4]; break;
        case 5:
            s.mode = 1;
            s.frame = -1;
            break;
        default: break;
    }
    if (s.frame == 0)
    {
        MakeEffect("jtele", IntPos(*this));
        Try("cinv2");
    }
    if (s.frame == 6)
    {
        Try("cinv2 to combat");
        const S3DPoint dest = s.dest;
        SetPos(dest, -1, false);
        MakeEffect("jteled", dest);
    }
    if (s.frame == 24)
        Try("cinv2");
    if (s.frame == 30)
    {
        if (s.hop == 4)
            CastByName("Priest Fire Wind");
        else
        {
            const S3DPoint from = IntPos(*this);
            const S3DPoint to = Player ? IntPos(Player) : s.points[4];
            Face(ConvertToFacing(from, to));
            CastByName("Priest Fireball");
        }
    }
    if (s.frame == 36)
        Try("cinv2 to combat");
    if (s.frame == 72)
    {
        ++s.hop;
        s.frame = 0;
        if (IsDoing("cinv2"))
            Try("cinv2 to combat");
        return;
    }
    ++s.frame;
}

// REVSYNC: 0x004ccac6 -- the first four "waypoint" objects of the map; their
// centre (the mean, toward zero) and the one in each quadrant round it. Fewer
// than four: Jhaga is no boss any more.
bool TCharacter::JhagaSetUp(SJhagaState& s)
{
    std::array<TObjectInstance*, 4> waypoints{};
    size_t found = 0;
    for (TObjectInstance* o : PaneObjects())
        if (NameIs(o->GetTypeName(), "waypoint"))
        {
            waypoints[found++] = o;
            if (found == waypoints.size())
                break;
        }
    if (found < waypoints.size())
    {
        monsterkind = -1;
        charflags &= ~kCharFlagNoTurn;
        return false;
    }

    std::array<S3DPoint, 4> at{};
    S3DPoint sum{};
    for (size_t i = 0; i < at.size(); ++i)
    {
        at[i] = IntPos(waypoints[i]);
        sum.x += at[i].x;
        sum.y += at[i].y;
        sum.z += at[i].z;
    }
    const S3DPoint centre(sum.x / 4, sum.y / 4, sum.z / 4);
  // The waypoint in a quadrant, the first in map order. Retail reads an
  // uninitialised stack slot for an empty one (a waypoint on the centre's
  // x or y); waypoint 0 here.
    auto quadrant = [&](int32_t xside, int32_t yside) -> const S3DPoint& {
        for (const S3DPoint& p : at)
            if ((xside < 0 ? p.x < centre.x : p.x > centre.x) && (yside < 0 ? p.y < centre.y : p.y > centre.y))
                return p;
        return at[0];
    };
    s.points[0] = quadrant(-1, -1);
    s.points[1] = quadrant(-1, 1);
    s.points[2] = quadrant(1, -1);
    s.points[3] = quadrant(1, 1);
    s.points[4] = centre;
    s.mode = 1;
    s.resttimer = 240;
    return true;
}

// *******************************************************************
// * Yhagoro (kind 4, 0x004cd07d): walking or fighting               *
// *******************************************************************

// REVSYNC: 0x004cd07d -- a sword can't kill him; by health he walks (above
// 75%, and 50-25%: halving physical damage, fighting in his walk root) or
// fights (75-50%, and below 25%: halving magic); a change of mind waits for
// the stance or the walk, plays its transition, and can't be targeted while
// it plays.
void TCharacter::BossYhagoro(SYhagoroState& s)
{
    if (Player && Player->WeaponType() == WT_SWORD)
        Player->charflags |= kCharFlagNoKill;
    charflags |= kCharFlagPlayAnimRoots;
    if (!GetAnimator())
        CreateAnimator();
  // The root again as "walk", in the fight (ACTION_COMBAT): a copy of the
  // root block. Retail's copy shares the old root's data pointer, which
  // the old root's release frees; this one keeps no data.
    auto rootAsWalk = [this] {
        auto* walk = new TActionBlock(*root, "walk", ACTION_COMBAT);
        walk->data = nullptr;
        SetRoot(walk);
        DropUnheld(walk);
    };
    const int32_t pct = Health() * 100 / MaxHealth();
    const bool walkband = pct > 75 || (pct > 25 && pct <= 50);
    if (walkband)
    {
        if (s.mode == 1)
            s.towalk = 1;
    }
    else if (s.mode == 0)
        s.tocombat = 1;

    if (s.towalk && IsDoing("combat"))
    {
        Force("combat to walk");
        Try("walk");
        rootAsWalk();
        s.mode = -1;
        charflags |= kCharFlagNoTurn;
        charflags &= ~kCharFlagNotTargetable;
    }
    if (s.tocombat && IsDoing("walk"))
    {
        doing->priority = false;
        Force("walk to combat");
        Try("combat");
        s.tocombat = 0;
        s.mode = 1;
    }
    if (IsDoing("walk to combat") || IsDoing("combat to walk"))
        charflags |= kCharFlagNotTargetable;
    else
        charflags &= ~kCharFlagNotTargetable;

    if (s.mode == 1 && !s.tocombat && !IsDoing("walk to combat"))
    {
        charflags &= ~(kCharFlagHalfPhysical | kCharFlagWalkPrefix | kCharFlagWalkFighter | kCharFlagNoTurn |
                       kCharFlagWalking);
        charflags |= kCharFlagFighting | kCharFlagHalfMagic;
        if (!IsRoot("combat"))
            Try("combat");
        else
            charflags &= ~kCharFlagNotTargetable;
    }
    if (s.mode == 0 && !s.towalk && !IsDoing("combat to walk"))
    {
        charflags &= ~(kCharFlagHalfMagic | kCharFlagNoTurn | kCharFlagFighting);
        charflags |= kCharFlagWalking | kCharFlagWalkPrefix | kCharFlagWalkFighter | kCharFlagHalfPhysical;
        if (!IsRoot("walk"))
        {
            Force("walk");
            rootAsWalk();
            if (desired)
                desired->priority = false;
            Try("walk");
        }
        else
            charflags &= ~kCharFlagNotTargetable;
    }
    if (s.mode == -1 && IsDoing("walk") && IsDesired("walk") && IsRoot("walk"))
    {
        s.towalk = 0;
        s.mode = 0;
        charflags &= ~(kCharFlagNotTargetable | kCharFlagNoTurn);
    }
}
