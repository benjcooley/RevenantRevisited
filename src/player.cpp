// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Player.cpp - TPlayer module                       *
// *************************************************************************

#include "player.h"

#include "armor.h"
#include "automap.h"
#include "dialog.h"
#include "equip.h"
#include "gameflow.h"
#include "gameoptions.h"
#include "inventory.h"
#include "logging.h"
#include "mappane.h"
#include "playerstats.h"
#include "playscreen.h"
#include "sound.h"
#include "spell.h"
#include "spellpane.h"
#include "textbar.h"
#include "weapon.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


extern TObjectClass  CharacterClass; // Used to indicate player is derived from character
extern TObjectClass TalismanClass;

REGISTER_BUILDER(TPlayer)
TObjectClass PlayerClass("PLAYER", OBJCLASS_PLAYER, 0, &CharacterClass);

// REVSYNC: the shipped game's PLAYER object stats 17-83 (SStatEntry
// registrations; recon/scripts/object_stats.py). Index, unique id, default
// and range as retail; class.def appends the six it adds after them.

// Miscellaneous player values
DEFOBJSTAT(Player, Level,           LEV,  PLRVAL_FIRST + PLRVAL_LEVEL, 1, 1, 100)
DEFOBJSTAT(Player, Exp,             EXP,  PLRVAL_FIRST + PLRVAL_EXP, 0, 0, 1000000)
DEFOBJSTAT(Player, NextExp,         NEXP, PLRVAL_FIRST + PLRVAL_NEXTEXP, 0, 0, 1000000)
DEFOBJSTAT(Player, AttackLevel,     ATKL, PLRVAL_FIRST + PLRVAL_ATTACKLEVEL, 1, 1, 100)
DEFOBJSTAT(Player, HealthPct,       HPCT, PLRVAL_FIRST + PLRVAL_HEALTHPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, ManaPct,         MPCT, PLRVAL_FIRST + PLRVAL_MANAPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, FatiguePct,      FPCT, PLRVAL_FIRST + PLRVAL_FATIGUEPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxHealthFlat,   MHFT, PLRVAL_FIRST + PLRVAL_MAXHEALTHFLAT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxManaFlat,     MMFT, PLRVAL_FIRST + PLRVAL_MAXMANAFLAT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxFatigueFlat,  MFFT, PLRVAL_FIRST + PLRVAL_MAXFATIGUEFLAT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxHealthPct,    MHPC, PLRVAL_FIRST + PLRVAL_MAXHEALTHPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxManaPct,      MMPC, PLRVAL_FIRST + PLRVAL_MAXMANAPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, MaxFatiguePct,   MFPC, PLRVAL_FIRST + PLRVAL_MAXFATIGUEPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, ACBonus,         ACBN, PLRVAL_FIRST + PLRVAL_ACBONUS, 0, -100000, 100000)
DEFOBJSTAT(Player, ManaCostPct,     MCPC, PLRVAL_FIRST + PLRVAL_MANACOSTPCT, 0, -100000, 100000)
DEFOBJSTAT(Player, SpellDamageInc,  SINC, PLRVAL_FIRST + PLRVAL_SPELLDAMAGEINC, 0, -100000, 100000)
DEFOBJSTAT(Player, EdgeBonus,       EBNS, PLRVAL_FIRST + PLRVAL_EDGEBONUS, 0, -100000, 100000)

// Character stats
DEFOBJSTAT(Player, Strn,            STRN, PLRSTAT_FIRST + PLRSTAT_STRN, 14, 0, 100)
DEFOBJSTAT(Player, Cons,            CONS, PLRSTAT_FIRST + PLRSTAT_CONS, 14, 0, 100)
DEFOBJSTAT(Player, Agil,            AGIL, PLRSTAT_FIRST + PLRSTAT_AGIL, 14, 0, 100)
DEFOBJSTAT(Player, Rflx,            RFLX, PLRSTAT_FIRST + PLRSTAT_RFLX, 14, 0, 100)
DEFOBJSTAT(Player, Mind,            MIND, PLRSTAT_FIRST + PLRSTAT_MIND, 14, 0, 100)
DEFOBJSTAT(Player, Luck,            LUCK, PLRSTAT_FIRST + PLRSTAT_LUCK, 14, 0, 100)

// Character skill values
DEFOBJSTAT(Player, Attack,          ATK,  SK_FIRST + SK_ATTACK, 0, 0, 100)
DEFOBJSTAT(Player, Defense,         DEF,  SK_FIRST + SK_DEFENSE, 0, 0, 100)
DEFOBJSTAT(Player, Invoke,          INV,  SK_FIRST + SK_INVOKE, 0, 0, 100)
DEFOBJSTAT(Player, Hands,           HAN,  SK_FIRST + SK_HANDS, 0, 0, 100)
DEFOBJSTAT(Player, Knife,           KNF,  SK_FIRST + SK_KNIFE, 0, 0, 100)
DEFOBJSTAT(Player, Sword,           SWR,  SK_FIRST + SK_SWORD, 0, 0, 100)
DEFOBJSTAT(Player, Bludgeon,        BLD,  SK_FIRST + SK_BLUDGEONS, 0, 0, 100)
DEFOBJSTAT(Player, Axes,            AXE,  SK_FIRST + SK_AXES, 0, 0, 100)
DEFOBJSTAT(Player, Bows,            BOW,  SK_FIRST + SK_BOWS, 0, 0, 100)
DEFOBJSTAT(Player, Stealth,         SLT,  SK_FIRST + SK_STEALTH, 0, 0, 100)
DEFOBJSTAT(Player, LockPick,        LPK,  SK_FIRST + SK_LOCKPICK, 0, 0, 100)

// Character skill experience
DEFOBJSTAT(Player, AttackExp,       ATKE, SKE_FIRST + SK_ATTACK,0, 0, 1000000)
DEFOBJSTAT(Player, DefenseExp,      DEFE, SKE_FIRST + SK_DEFENSE,0, 0, 1000000)
DEFOBJSTAT(Player, InvokeExp,       INVE, SKE_FIRST + SK_INVOKE, 0, 0, 1000000)
DEFOBJSTAT(Player, HandsExp,        HANE, SKE_FIRST + SK_HANDS, 0, 0, 1000000)
DEFOBJSTAT(Player, KnifeExp,        KNFE, SKE_FIRST + SK_KNIFE, 0, 0, 1000000)
DEFOBJSTAT(Player, SwordExp,        SWRE, SKE_FIRST + SK_SWORD, 0, 0, 1000000)
DEFOBJSTAT(Player, BludgeonExp,     BLDE, SKE_FIRST + SK_BLUDGEONS, 0, 0, 1000000)
DEFOBJSTAT(Player, AxesExp,         AXEE, SKE_FIRST + SK_AXES, 0, 0, 1000000)
DEFOBJSTAT(Player, BowsExp,         BOWE, SKE_FIRST + SK_BOWS, 0, 0, 1000000)
DEFOBJSTAT(Player, StealthExp,      SLTE, SKE_FIRST + SK_STEALTH, 0, 0, 1000000)
DEFOBJSTAT(Player, LockPickExp,     LPKE, SKE_FIRST + SK_LOCKPICK, 0, 0, 1000000)

// Experience for the next skill level
DEFOBJSTAT(Player, AttackNxtExp,    ATKN, SKN_FIRST + SK_ATTACK, 0, 0, 1000000)
DEFOBJSTAT(Player, DefenseNxtExp,   DEFN, SKN_FIRST + SK_DEFENSE, 0, 0, 1000000)
DEFOBJSTAT(Player, InvokeNxtExp,    INVN, SKN_FIRST + SK_INVOKE, 0, 0, 1000000)
DEFOBJSTAT(Player, HandsNxtExp,     HANN, SKN_FIRST + SK_HANDS, 0, 0, 1000000)
DEFOBJSTAT(Player, KnifeNxtExp,     KNFN, SKN_FIRST + SK_KNIFE, 0, 0, 1000000)
DEFOBJSTAT(Player, SwordNxtExp,     SWRN, SKN_FIRST + SK_SWORD, 0, 0, 1000000)
DEFOBJSTAT(Player, BludgeonNxtExp,  BLDN, SKN_FIRST + SK_BLUDGEONS, 0, 0, 1000000)
DEFOBJSTAT(Player, AxesNxtExp,      AXEN, SKN_FIRST + SK_AXES, 0, 0, 1000000)
DEFOBJSTAT(Player, BowsNxtExp,      BOWN, SKN_FIRST + SK_BOWS, 0, 0, 1000000)
DEFOBJSTAT(Player, StealthNxtExp,   SLTN, SKN_FIRST + SK_STEALTH, 0, 0, 1000000)
DEFOBJSTAT(Player, LockPickNxtExp,  LPKN, SKN_FIRST + SK_LOCKPICK, 0, 0, 1000000)

// Skill caps
DEFOBJSTAT(Player, AttackCap,       ATKC, SKC_FIRST + SK_ATTACK, 0, 0, 100)
DEFOBJSTAT(Player, DefenseCap,      DEFC, SKC_FIRST + SK_DEFENSE, 0, 0, 100)
DEFOBJSTAT(Player, InvokeCap,       INVC, SKC_FIRST + SK_INVOKE, 0, 0, 100)
DEFOBJSTAT(Player, HandsCap,        HANC, SKC_FIRST + SK_HANDS, 0, 0, 100)
DEFOBJSTAT(Player, KnifeCap,        KNFC, SKC_FIRST + SK_KNIFE, 0, 0, 100)
DEFOBJSTAT(Player, SwordCap,        SWRC, SKC_FIRST + SK_SWORD, 0, 0, 100)
DEFOBJSTAT(Player, BludgeonCap,     BLDC, SKC_FIRST + SK_BLUDGEONS, 0, 0, 100)
DEFOBJSTAT(Player, AxesCap,         AXEC, SKC_FIRST + SK_AXES, 0, 0, 100)
DEFOBJSTAT(Player, BowsCap,         BOWC, SKC_FIRST + SK_BOWS, 0, 0, 100)
DEFOBJSTAT(Player, StealthCap,      SLTC, SKC_FIRST + SK_STEALTH, 0, 0, 100)
DEFOBJSTAT(Player, LockPickCap,     LPKC, SKC_FIRST + SK_LOCKPICK, 0, 0, 100)

// **************
// * Skill Tree *
// **************

SSkill SkillTree[NUM_SKILLS] =
{
    { "Attack",         true,   false,  -1, 3 },
    { "Defence",        true,   false,  -1, 3 },
    { "Short Blades",   true,   false,  -1, 3 },
    { "Long Blades",    true,   false,  -1, 3 },
    { "Bludgeons",      true,   false,  -1, 3 },
    { "Axes",           true,   false,  -1, 3 },
    { "Bows",           true,   false,  -1, 3 },
    { "Stealth",        true,   false,  -1, 3 },
    { "Lockpicking",    true,   false,  -1, 3 },
    { "Invocation",     true,   false,  -1, 3 }
};

// *********************************************
// * TPlayer - Represents a game player object *
// *********************************************

// REMEMBER: !!!!! Please attempt to put MOST of player functionality into character instead.
// Player should only be the place for functionality that CAN'T be placed into character!!

TPlayer::TPlayer(TObjectImagery* newim) : TCharacter(newim)
{
    ClearPlayer();
}

TPlayer::TPlayer(SObjectDef* def, TObjectImagery* newim) : TCharacter(def, newim)
{
    ClearPlayer();
}

TPlayer::~TPlayer()
{
    PlayerManager.RemovePlayer(this);
}

void TPlayer::ClearPlayer()
{
    memset(equipment, 0, NUM_EQ_SLOTS * sizeof(TObjectInstance*));

    flags |= OF_NONMAP; // Allows us to insert object into the game map, and not
                        // worry about it getting deleted, saved, or whatnot

    memset(quickspells, 0, QSPELL_NUM * MAXTALISMANLEN);

    OnTheHog = false;

  // REVSYNC: the record fields of TPlayer::ClearPlayer @ 0x00518750. A player
  // is never VIRGIN: its stats are its own.
    flags &= ~OF_VIRGIN;
    knownspells.clear();
    ++knownspellsversion;
    hudwords = {};
    levelupstats = {};
    team = {};
    modulename.clear();
    playerstate = 1;
    statetime = PlayScreen.GameTime();
    stateframes = PlayScreen.GameFrame();
    profile = {};
    frags = {};
    automap = {};

  // REVSYNC: the stats of TPlayer::ClearPlayer @ 0x00518750. 84 attribute
  // points: an attribute the class requires (STATREQS non-zero) is
  // |requirement|; each other one gets the points left over divided by six
  // -- retail divides by all six attributes, not the free ones, so the free
  // ones share only part of the rest -- in 16.16 fixed point with the
  // fraction carried. Every skill cap is 30. Level, experience and the
  // skills come from the save the player is loaded from (newgame.sav for a
  // new game).
    if (chardata && chardata->classdata)
    {
        const SClassData* cd = chardata->classdata;

        int32_t rest = 84;
        for (int32_t i = 0; i < NUM_PLRSTATS; i++)
            rest -= std::abs(cd->statreqs[i]);
        const int32_t share = rest * 0x10000 / NUM_PLRSTATS + 1;

        int32_t acc = share;
        for (int32_t i = 0; i < NUM_PLRSTATS; i++)
        {
            if (cd->statreqs[i] == 0)
            {
                SetObjStat(PLRSTAT_FIRST + i, acc >> 16);
                acc = (acc & 0xffff) + share;
            }
            else
                SetObjStat(PLRSTAT_FIRST + i, std::abs(cd->statreqs[i]));
        }

        for (int32_t s = 0; s < NUM_SKILLS; s++)
            SetObjStat(SKC_FIRST + s, PlayerStats::kStatCap);
    }

  // The modified stats: one entry per PLAYER object stat, filled by
  // RefreshStats; no stat effects.
    stateffects.clear();
    stateffectexpires = -1;
    modstats.assign(cl->NumObjStats(), 0);
    RefreshStats();
}

// REVSYNC: TPlayer::SetPlayerLevel @ 0x0051d840 (`playerlevel`) -- rebuild
// the player as a fresh level-'level' character of its class. The stat
// indices are retail's (charstats.h): 0x22+i PLRSTAT_FIRST+i, 0x28+s
// SK_FIRST+s, 0x33+s SKE_FIRST+s, 0x3e+s SKN_FIRST+s, 0x14 AttackLevel.
void TPlayer::SetPlayerLevel(int32_t level)
{
    SetLevel(level);

  // Attributes from the class's STATREQS: |req|, or 14 where the class
  // leaves the attribute free.
    const SClassData* cd = chardata->classdata;
    for (int32_t i = 0; i < NUM_PLRSTATS; i++)
    {
        const int32_t req = std::abs(cd->statreqs[i]);
        SetObjStat(PLRSTAT_FIRST + i, req != 0 ? req : 14);
    }

  // Every skill back to level 0 with no experience, 300 to reach level 1.
    for (int32_t s = 0; s < NUM_SKILLS; s++)
    {
        SetObjStat(SK_FIRST + s, 0);
        SetObjStat(SKE_FIRST + s, 0);
        SetObjStat(SKN_FIRST + s, 300);
    }

  // Each level above the first: two attribute points (one from level 15
  // on) to random attributes, then experience for every skill. Retail picks
  // from random(0, 6), seven ids from Strn, so one pick in seven lands on
  // the Attack skill, the id after Luck. Kept. The point goes on the stat
  // itself, not the modified copy.
    for (int32_t lvl = 1; lvl < level; lvl++)
    {
        for (int32_t points = (lvl < 15) ? 2 : 1; points > 0; points--)
        {
            const int32_t pick = random(0, NUM_PLRSTATS);
            const int32_t stat = (pick < NUM_PLRSTATS) ? PLRSTAT_FIRST + pick : SK_FIRST + SK_ATTACK;
            SetObjStat(stat, BaseObjStat(stat) + 1);
        }
        for (int32_t s = 0; s < NUM_SKILLS; s++)
            AddSkillExp(s, 333 + 100 * (lvl - 1));
    }

    SetAttackLevel(level);

  // Exp and NextExp are left as they were.
    SetHealth(MaxHealth());
    SetMana(MaxMana());
    SetFatigue(MaxFatigue());
    RefreshStats();

    LogStats("playerlevel");
}

// The player's level, attributes and vitals, for the log.
void TPlayer::LogStats(const char *why)
{
    log_info("[player] %s: L%d Exp=%d/%d STR=%d CON=%d AGI=%d RFL=%d MND=%d LCK=%d "
             "AttackLevel=%d H=%d/%d F=%d/%d M=%d/%d Armor=%d",
             why, (int)Level(), (int)Exp(), (int)NextExp(), (int)Strn(), (int)Cons(),
             (int)Agil(), (int)Rflx(), (int)Mind(), (int)Luck(), (int)AttackLevel(),
             (int)Health(), (int)MaxHealth(), (int)Fatigue(), (int)MaxFatigue(),
             (int)Mana(), (int)MaxMana(), (int)ArmorValue());
}

// REVSYNC: TPlayer::AddSkillExp @ 0x0051ac90 -- add experience to a skill.
// Reaching the next threshold raises the skill one level (one per call) and
// sets the threshold after it; at the top level the experience pins to that
// level's figure. Retail first forwards the call in a network game, or
// drops it for a remote player (no multiplayer in the port). The level and
// experience read are the modified stats', as retail's.
void TPlayer::AddSkillExp(int32_t skillnum, int32_t exp)
{
    if (skillExpSeam)
    {
        skillExpSeam(this, skillnum, exp);
        return;
    }
    const int32_t have = GetObjStat(SKE_FIRST + skillnum);
    const int32_t level = GetObjStat(SK_FIRST + skillnum);
    if (level >= TRules::kMaxSkillLevel)
    {
        SetObjStat(SKE_FIRST + skillnum, Rules.SkillExpForLevel(TRules::kMaxSkillLevel));
        return;
    }

    SetObjStat(SKE_FIRST + skillnum, have + exp);
    if (have + exp >= Rules.SkillExpForLevel(level + 1))
    {
        SetObjStat(SK_FIRST + skillnum, level + 1);
        SetObjStat(SKN_FIRST + skillnum, Rules.SkillExpForLevel(level + 2));
    }
}

// ******************
// * Modified stats *
// ******************

// REVSYNC: TPlayer::GetObjStat @ 0x0051ae30 -- the modified copy; 0 past
// its end.
int32_t TPlayer::GetObjStat(int32_t statid) const
{
    if ((uint32_t)statid < (uint32_t)modstats.size())
        return modstats[statid];
    return 0;
}

// REVSYNC: TPlayer::SetObjStat @ 0x0051adb0 -- the stat and its entry in
// the modified copy, which keeps that value until the next RefreshStats
// applies equipment and effects to it again. (Retail also marks the stat
// pane dirty; the port's panes read the player every frame.)
void TPlayer::SetObjStat(int32_t statid, int32_t value)
{
    TCharacter::SetObjStat(statid, value);
    if ((uint32_t)statid < (uint32_t)modstats.size())
        modstats[statid] = value;
}

// REVSYNC: TPlayer::RefreshStats @ 0x0051c660. The modified copy starts as
// the stats, takes the STATLINE of each equipped weapon and piece of armor
// (WEAPON.DEF / ARMOR.DEF) and of each stat effect, then health, fatigue
// and mana over their maximums come down to them and the copy's attributes
// and skills stop at 30. An effect found expired is removed and the refresh
// runs again from inside; the outer pass then carries on with the next
// effect, as retail's does. Ends with RefreshEquip.
void TPlayer::RefreshStats()
{
    if (refreshStatsSeam)
    {
        refreshStatsSeam(this);
        return;
    }
    for (int32_t id = 0; id < (int32_t)modstats.size(); id++)
        modstats[id] = BaseObjStat(id);

    for (TObjectInstance* item : equipment)
    {
        if (!item)
            continue;
        const SItemData* data = Rules.GetItemData(item->ObjClass(), item->GetTypeName());
        if (data && !data->statline.empty())
            ApplyStatLine(data->statline.c_str(), -1);
    }

    for (int32_t effect = 0; effect < (int32_t)stateffects.size(); effect++)
    {
        const std::string statline = stateffects[effect].statline;  // the call may remove it
        ApplyStatLine(statline.c_str(), effect);
    }

    if (MaxHealth() < Health())
        SetHealth(MaxHealth());
    if (MaxFatigue() < Fatigue())
        SetFatigue(MaxFatigue());
    if (MaxMana() < Mana())
        SetMana(MaxMana());

    for (int32_t id = PLRSTAT_FIRST; id < SKE_FIRST; id++)
    {
        if (id < (int32_t)modstats.size() && modstats[id] > PlayerStats::kStatCap)
            modstats[id] = PlayerStats::kStatCap;
    }

    RefreshEquip();
}

// REVSYNC: TPlayer::ApplyStatLine @ 0x0051cc40 -- one STATLINE onto the
// modified copy (PlayerStats::ApplyStatLine), then, for a stat effect, its
// time: no TIME lasts for ever; the INITIALIZE pass starts the clock; a
// later pass past the end removes the effect. Every line first clears the
// next-refresh time, so it ends as the last timed effect's end. The main
// player is told about bad values, and on the INITIALIZE pass what changed.
void TPlayer::ApplyStatLine(const char *statline, int32_t effect)
{
    stateffectexpires = -1;

    const PlayerStats::SStatLineResult result = PlayerStats::ApplyStatLine(statline, modstats,
        [this](const char *name) { return cl->FindObjStat(name); });

    const bool mainplayer = (this == Player);
    if (mainplayer)
    {
        for (const PlayerStats::SStatLineResult::SNote& note : result.notes)
        {
            const bool badtime = note.kind == PlayerStats::SStatLineResult::ENote::BadTime;
            if (badtime && DialogList.FindLine("FULLSTATTIME") >= 0)
                TextBar.Print(DialogList.GetLine("FULLSTATTIME"), note.mod.value.c_str());
            else if (badtime)
                TextBar.Print("%s %s", DialogList.GetLine("STATINVTIME"), note.mod.value.c_str());
            else if (DialogList.FindLine("FULLSTATMOD") >= 0)
                TextBar.Print(DialogList.GetLine("FULLSTATMOD"), note.mod.stat.c_str(), note.mod.value.c_str());
            else
                TextBar.Print("%s %s. %s %s", DialogList.GetLine("STATINVMOD"), note.mod.stat.c_str(),
                              DialogList.GetLine("STATVAL"), note.mod.value.c_str());
        }
    }

  // The first pass drops INITIALIZE from the stored line (the blank before
  // it stays).
    const bool live = (uint32_t)effect < (uint32_t)stateffects.size();
    if (result.initialize && live)
    {
        std::string& line = stateffects[effect].statline;
        const size_t at = line.find("INITIALIZE");
        if (at != std::string::npos)
            line.erase(at);
    }

    if (!result.complete)
        return;

    if (live)
    {
        SStatEffect& entry = stateffects[effect];
        if (result.duration == -1)
            entry.expires = -1;
        else
        {
            if (result.initialize)
                entry.expires = statetime + result.duration;
            else if (entry.expires < statetime)
            {
                RemoveStatEffect(effect);
                RefreshStats();
                return;
            }
            if (entry.expires < stateffectexpires || (entry.expires != -1 && stateffectexpires == -1))
                stateffectexpires = entry.expires;
        }
    }

    if (result.initialize && mainplayer)
    {
        std::string message = DialogList.GetLine("STATMODS");
        for (const PlayerStats::SStatLineMod& mod : result.mods)
        {
            std::string tag = "STATCFG" + mod.stat;
            message += ' ';
            message += DialogList.GetLine(tag.c_str());
            message += ' ';
            message += mod.value;
        }
        TextBar.Print("%s", message.c_str());
    }
}

// REVSYNC: TPlayer::AddStatEffect @ 0x0051c2c0 (the statmod command
// 0x00428200, and a spell with a STATLINE cast on the player 0x0053f090).
// The line is stored with INITIALIZE appended unless it has it; every
// effect whose stored line differs from the new line ends -- a stored line
// has lost its INITIALIZE, so in practice all of them: one effect at a time.
void TPlayer::AddStatEffect(const char *statline)
{
    if (statEffectSeam)
    {
        statEffectSeam(this, statline);
        return;
    }
    if (!statline)
        return;

    for (int32_t effect = 0; effect < (int32_t)stateffects.size(); )
    {
        if (stateffects[effect].statline != statline)
            RemoveStatEffect(effect);
        else
            effect++;
    }

    SStatEffect entry;
    entry.statline = statline;
    if (!strstr(statline, "INITIALIZE"))
        entry.statline += " INITIALIZE";
    stateffects.push_back(std::move(entry));

    RefreshStats();
}

// REVSYNC: 0x0051d4a0
void TPlayer::RemoveStatEffect(int32_t effect)
{
    if ((uint32_t)effect < (uint32_t)stateffects.size())
        stateffects.erase(stateffects.begin() + effect);
}

// REVSYNC: TPlayer::MaxHealth @ 0x00520630, from the modified stats.
int32_t TPlayer::MaxHealth()
{
    const SClassData* cd = chardata ? chardata->classdata : nullptr;
    return PlayerStats::MaxHealth(Level(), Rules.healthperlevel, MaxHealthFlat(), MaxHealthPct(),
                                  Rules.StatLevel(PLRSTAT_CONS, Cons()), cd ? cd->healthmod : 0);
}

// REVSYNC: TPlayer::MaxFatigue @ 0x005206d0, from the modified stats.
int32_t TPlayer::MaxFatigue()
{
    const SClassData* cd = chardata ? chardata->classdata : nullptr;
    return PlayerStats::MaxFatigue(Level(), Rules.fatigueperlevel, MaxFatigueFlat(), MaxFatiguePct(),
                                   Rules.StatLevel(PLRSTAT_CONS, Cons()), cd ? cd->fatiguemod : 0);
}

// REVSYNC: TPlayer::MaxMana @ 0x00520770, from the modified stats.
int32_t TPlayer::MaxMana()
{
    const SClassData* cd = chardata ? chardata->classdata : nullptr;
    return PlayerStats::MaxMana(Level(), Rules.manaperlevel, MaxManaFlat(), MaxManaPct(),
                                Rules.StatLevel(PLRSTAT_MIND, Mind()), cd ? cd->manamod : 0);
}

// REVSYNC: TPlayer::ArmorValue @ 0x00519850 -- ACBonus plus the Protection
// of every piece of armor worn, in all slots but the ammo slot.
int32_t TPlayer::ArmorValue()
{
    int32_t armor = ACBonus();
    for (int32_t slot = 0; slot < NUM_EQ_SLOTS; slot++)
    {
        TObjectInstance* item = equipment[slot];
        if (slot != EQ_AMMO && item && item->ObjClass() == OBJCLASS_ARMOR)
            armor += static_cast<TArmor*>(item)->Protection();
    }
    return armor;
}

// REVSYNC: TPlayer::WeaponDamage @ 0x00520830 (the item's class isn't checked)
int32_t TPlayer::WeaponDamage()
{
    return PrimeHand() ? PrimeHand()->GetStat("Damage") : chardata->weapondamage;
}

// REVSYNC: TPlayer::GetDamageType @ 0x0051b2f0 (slot 0x2d4) -- a weapon in
// hand whose DamageType isn't -1 deals that, whatever the attack. WEAPON.DEF
// gives every shipped weapon 0 (DT_MISC), so an armed player's blows are
// misc damage; bare-handed, the attack's own (DT_HAND).
int32_t TPlayer::GetDamageType(int32_t weapontype, int32_t attackflags)
{
    TObjectInstance* hand = PrimeHand();
    if (hand && hand->ObjClass() == OBJCLASS_WEAPON)
    {
        const int32_t type = static_cast<TWeapon*>(hand)->DamageType();
        if (type != -1)
            return type;
    }
    return TCharacter::GetDamageType(weapontype, attackflags);
}

// REVSYNC: TPlayer::AttackModifier @ 0x0051a480
int32_t TPlayer::AttackModifier()
{
    int32_t mod = chardata->attackmod;
    mod += Rules.StatLevel(PLRSTAT_AGIL, Agil());
    mod += SpellManager.GetOffense();
    return mod + GetObjStat(SK_FIRST + SK_WEAPONSKILLS + WeaponType());
}

// REVSYNC: TPlayer::DefenseModifier @ 0x0051a4e0
int32_t TPlayer::DefenseModifier()
{
    int32_t mod = chardata->defensemod;
    mod += Rules.StatLevel(PLRSTAT_RFLX, Rflx());
    return mod + SpellManager.GetDefense();
}

// REVSYNC: TPlayer::Offense @ 0x0051a520
int32_t TPlayer::Offense()
{
    const int32_t level = Level() * Rules.tohitrangeplyr;
    return level + AttackModifier();
}

// REVSYNC: TPlayer::Defense @ 0x0051a550
int32_t TPlayer::Defense()
{
    const int32_t level = Level() * Rules.tohitrangeplyr;
    return level + DefenseModifier();
}

// REVSYNC: TPlayer::HoldsLight @ 0x00519970
bool TPlayer::HoldsLight()
{
    TObjectInstance* item = equipment[EQ_L_ACCESSORY];
    return item && item->ObjClass() == OBJCLASS_LIGHTSOURCE;
}

// **************
// * Experience *
// **************

int32_t TPlayer::KillExp(int32_t value)
{
    return PlayerStats::KillExp(value, Level());
}

// REVSYNC: TPlayer @ 0x0051a630 (vtable +0x414), from the end of hit
// resolution (0x004c62b0) -- experience for a dead victim: its Value (a
// player's Level, and half as much again) against this player's level.
// Reaching the next level (to 30): Level + 1, NextExp to the level after,
// the level-up attribute points (both chosen attributes up to level 15,
// the first after) on the stats themselves, a chosen attribute already at
// 29 moving on to the next one below 29, the messages, the "invoke2"
// animation (SetCast 0x004d5900), the PowerUp sound and effect, health,
// mana and fatigue full. Not ported: the network forwarding.
void TPlayer::AwardKillExp(TCharacter *victim)
{
    if (killExpSeam)
    {
        killExpSeam(this, victim);
        return;
    }
    if (!victim || victim->Health() > 0)
        return;

    const int32_t exp   = Exp();
    const int32_t level = Level();
    const int32_t next  = Rules.ExpForLevel(level + 1);

    int32_t gain;
    if (victim->ObjClass() == OBJCLASS_PLAYER)
        gain = static_cast<int32_t>(static_cast<double>(KillExp(victim->GetStat("Level"))) * 1.5);
    else
        gain = KillExp(victim->GetStat("Value"));
    SetExp(exp + gain);

    if (level >= TRules::kMaxPlayerLevel || exp + gain < next)
        return;

    const int32_t newlevel = level + 1;
    SetLevel(newlevel);
    SetNextExp(Rules.ExpForLevel(newlevel + 1));

    const bool mainplayer = (this == Player);
    auto attribute_tag = [this](int32_t attribute)
        { return std::string("STATCFG") + ObjStatName(PLRSTAT_FIRST + attribute); };
    auto raise = [this](int32_t attribute)
        { SetObjStat(PLRSTAT_FIRST + attribute, BaseObjStat(PLRSTAT_FIRST + attribute) + 1); };

    if (newlevel < 25 && newlevel % 3 == 0)
        TextBar.Print("%s", DialogList.GetLine("LUPJONG"));
    if (newlevel > 15)
    {
        if (mainplayer)
            TextBar.Print("%s", DialogList.GetLine("LUPBASE"));
        raise(levelupstats[0]);
    }
    else
    {
        if (mainplayer)
        {
            TextBar.Print("%s", DialogList.GetLine(attribute_tag(levelupstats[0]).c_str()));
            TextBar.Print("%s", DialogList.GetLine(attribute_tag(levelupstats[1]).c_str()));
            TextBar.Print("%s", DialogList.GetLine("LUPBASE"));
        }
        raise(levelupstats[0]);
        raise(levelupstats[1]);
    }

    for (int32_t k = 0; k < 2; k++)
    {
        int32_t attribute = levelupstats[k];
        if (GetObjStat(PLRSTAT_FIRST + attribute) < 29)
            continue;
        for (int32_t tries = 0; tries < NUM_PLRSTATS; tries++)
        {
            attribute = (attribute + 1 < NUM_PLRSTATS) ? attribute + 1 : 0;
            if (GetObjStat(PLRSTAT_FIRST + attribute) < 29)
            {
                levelupstats[k] = attribute;
                break;
            }
        }
    }

  // The level-up shows: the "invoke2" animation held 90 ticks, the PowerUp
  // sound, then its effect.
    SetCast("invoke2", nullptr, 90);
    extern TObjectClass EffectClass;
    SObjectDef def{};
    def.objclass = OBJCLASS_EFFECT;
    def.objtype = EffectClass.FindObjType("PowerUp");
    def.level = MapPane.GetMapLevel();
    def.pos = IntPos(*this);
    def.facing = uint8_t(IntFacing(*this));
    SoundPlayer.Play("POWERUP");
    if (def.objtype >= 0)
        MapPane.NewObject(&def);

    RefreshStats();
    SetHealth(MaxHealth());
    SetMana(MaxMana());
    SetFatigue(MaxFatigue());

    LogStats("level up");
}

// REVSYNC: TPlayer @ 0x0051abe0 (vtable +0x41c) -- skill experience for a
// dead victim, by its Value.
void TPlayer::AwardSkillExp(int32_t skillnum, TCharacter *victim)
{
    if (awardSkillExpSeam)
    {
        awardSkillExpSeam(this, skillnum, victim);
        return;
    }
    if (!victim || victim->Health() > 0)
        return;
    AddSkillExp(skillnum, KillExp(victim->GetStat("Value")));
}

// REVSYNC: TPlayer @ 0x0051ad80 (vtable +0x420) -- stealth experience for a
// victim that never saw the player (0x004c58f0).
void TPlayer::AwardStealthExp(TCharacter *victim)
{
    if (stealthExpSeam)
    {
        stealthExpSeam(this, victim);
        return;
    }
    if (victim && !victim->HasSeenMe(this))
        AwardSkillExp(SK_STEALTH, victim);
}

// REVSYNC: TPlayer::Pulse @ 0x00518aa0 (PLAYER_STATS.md's "Animate": it
// fills the Pulse slot and calls TCharacter's first) -- the character's
// tick, then the play clock, the stat effects, the death screen countdown.
void TPlayer::Pulse()
{
    TCharacter::Pulse();

  // The play clock runs while the player state has bit 0 and not bit 1, in
  // game time (1/100 s) from a frame count. When it passes the next
  // stat-effect expiry the stats are refreshed (an expiry more than 50000
  // away is first pulled to now).
    if ((playerstate & 1) && !(playerstate & 2))
    {
        ++stateframes;
        statetime = int32_t(int64_t(stateframes) * 100 / 24);
    }
    if (stateffectexpires != -1)
    {
        if (std::abs(statetime - stateffectexpires) > 50000)
            stateffectexpires = statetime - 1;
        if (stateffectexpires < statetime)
            RefreshStats();
    }

  // Placed and since touched (Load treats an untouched one as new).
    ClearFlag(OF_VIRGIN);

  // While alive the countdown is held at 192 frames (8 s at 24 Hz); once
  // health drops below 1 it runs down and at zero the game ends on the
  // death screen (it goes on below zero). A held death (charflags 0x1000)
  // keeps it from reaching zero. (The 1998 snapshot showed a death pane on
  // the PlayScreen after 100 frames instead.)
    if (!Editor)
    {
        if (charflags & kCharFlagDeathHold)
            deathcountdown = (std::max)(deathcountdown, 1);
        if (Health() < 1)
        {
            if (deathcountdown == 0)
                GameFlow.PlayerDied();
            --deathcountdown;
        }
        else
            deathcountdown = 0xc0;
    }

  // Not ported: the held torch's light at his left hand, flickering (two to
  // four random draws a tick while he holds one, 0x00518bcc..0x00518d95),
  // and a lit player's glow; nor the network sends every 120 frames.
}

// REVSYNC: 0x004c2baf (TCharacter::Pulse, for a player) -- HealthPct,
// FatiguePct and ManaPct add to the recovery every tick, in 1/10000 points:
// a 1440th of the stat, so that many points a minute at 24 ticks a second.
int32_t TPlayer::RecoveryBonus(EVital vital)
{
    const int32_t pct = vital == EVital::Health ? HealthPct()
                      : vital == EVital::Fatigue ? FatiguePct() : ManaPct();
    return pct * 10000 / 1440;
}

uint32_t TPlayer::Move()
{
    uint32_t retval = TCharacter::Move();

    if (!OnTheHog)
        return retval;

    static bool riding = false;
    bool oldriding = riding;

    S3DPoint nextmove;
    GetNextMove(nextmove);

    if (nextmove.x || nextmove.y || nextmove.z)
        riding = true;
    else
        riding = false;

    if (oldriding && !riding)
    {
        SoundPlayer.Stop("hog drive");
        SoundPlayer.Play("hog idle");
    }
    else if (!oldriding && riding)
    {
        SoundPlayer.Stop("hog idle");
        SoundPlayer.Play("hog drive");
    }

    return retval;
}

// REVSYNC: TPlayer::Damage @ 0x005191e0 (vtable +0x48).
void TPlayer::Damage(int32_t damage, int32_t type)
{
    TCharacter::Damage(damage, type);
}

// REVSYNC: TPlayer::Killed @ 0x00518ed0 (vtable +0x244) -- a kill counts in
// the frag record: +0x650 for a player, +0x658 for anything else. (The
// network score message and the HUD refresh are multiplayer's.)
void TPlayer::Killed(TCharacter* victim)
{
    if (victim)
        ++frags[victim->ObjClass() == OBJCLASS_PLAYER ? 0 : 2];
}

// REVSYNC: TPlayer::Died @ 0x00518f90 (vtable +0x248; SetFrags 0x00519050) --
// a death counts: +0x654 by a player, +0x65c by anything else; none when
// nothing killed it (Pulse's call).
void TPlayer::Died(TCharacter* killer)
{
    if (killer)
        ++frags[killer->ObjClass() == OBJCLASS_PLAYER ? 1 : 3];
}

// REVSYNC: TPlayer::Resist @ 0x005208d0 -- CalculateDamage's percent off:
// read straight from the modified copy (+0x34c), not through GetObjStat.
int32_t TPlayer::DamageModifier(int32_t damagetype)
{
    const int32_t id = CHRRESIST_FIRST + damagetype;
    return (uint32_t)id < (uint32_t)modstats.size() ? modstats[id] : 0;
}

// REVSYNC: TPlayer::GetFieldText = retail 0x0051dfb0 (vtable +0xc8).
bool TPlayer::GetFieldText(const char *field, char *buf, int32_t buflen)
{
    if (field && buf && buflen > 0 && stricmp(field, "class") == 0)
    {
        const SClassData *classdata = chardata ? chardata->classdata : nullptr;
        snprintf(buf, buflen, "%s", classdata ? classdata->name : "");
        return true;
    }
    return TCharacter::GetFieldText(field, buf, buflen);
}

int32_t TPlayer::ResolveCombat(PTActionBlock ab, int32_t bits)
{
    return TCharacter::ResolveCombat(ab, bits);
}

// REVSYNC: TPlayer::RefreshEquip = retail 0x00519230. An item at
// 0x100 + slot is taken as that slot's equipment, then re-equipped: Equip
// returns at once for an item that may be equipped there, and moves one
// that may not back into the pack.
void TPlayer::RefreshEquip()
{
    for (TInventoryIterator i(this); i; i++)
    {
        TObjectInstance* item = i.Item();
        const int32_t slot = item->InventNum() - kInvSlotEquipFirst;
        if ((uint32_t)slot < NUM_EQ_SLOTS)
        {
            equipment[slot] = item;
            Equip(CanEquip(item, slot) ? item : nullptr, slot);
        }
    }

    if (this == Player)
        EquipPane.SetDirty(true);
}

// REVSYNC: TPlayer::CanEquip = retail 0x00519300. Refused: no item, an item
// of a class with no EqSlot or of another slot; a weapon the player has no
// action animation for, of a type his class may not use (char.def WEAPONS)
// or asking more Strn than he has; armor asking more Strn or Cons than he
// has. The main player reads the requirement that refused him.
bool TPlayer::CanEquip(TObjectInstance* oi, int32_t slot)
{
    if (!oi || oi->FindStat("EqSlot") < 0 || oi->GetStat("EqSlot") != slot)
        return false;

    if (oi->ObjClass() == OBJCLASS_WEAPON)
    {
        auto* const weapon = static_cast<TWeapon*>(oi);
        const int32_t type = weapon->Type();
        const bool bow = type == WT_BOW || type == WT_CROSSBOW;
        if (!HasActionAni(bow ? GetBowRoot(oi) : GetCombatRoot(oi)))
            return false;
        const SClassData* const cd = chardata ? chardata->classdata : nullptr;
        if (cd && !(cd->weapons & (1 << type)))
            return false;
        if (Strn() < weapon->MinStrength())
        {
            if (this == Player)
                TextBar.Print("%s. ( %s: %d )", DialogList.GetLine("EQUIPUNABLE"), DialogList.GetLine("BSARM4"),
                              weapon->MinStrength());
            return false;
        }
    }
    else if (oi->ObjClass() == OBJCLASS_ARMOR)
    {
        auto* const armor = static_cast<TArmor*>(oi);
        if (Strn() < armor->MinStrength() || Cons() < armor->MinConstitution())
        {
            if (this == Player)
                TextBar.Print("%s. ( %s: %d )  ( %s: %d )", DialogList.GetLine("EQUIPUNABLE"),
                              DialogList.GetLine("BSARM4"), armor->MinStrength(), DialogList.GetLine("BSARM5"),
                              armor->MinConstitution());
            return false;
        }
    }

    return true;
}

// REVSYNC: the player half of RemoveFromInventory @ 0x0046faf0 -- an item
// leaving one of this player's equipment slots is unequipped first
// (Equip(nullptr, slot) 0x005199b0), so the equipment never points at an
// item the player no longer holds. Retail tested the owner's class and
// slot; the equipment pointer check makes a stale slot number harmless. The
// 1998 callers that unequip by hand beforehand find the slot empty.
void TPlayer::OnInventoryRemove(TObjectInstance* item)
{
    const int32_t slot = item->InventNum() - kInvSlotEquipFirst;
    if ((uint32_t)slot < NUM_EQ_SLOTS && equipment[slot] == item)
    {
        log_debug("[inv] %s: %s leaves equipment slot %d, unequipped", GetName(), item->GetName(), slot);
        Equip(nullptr, slot);
    }
}

// REVSYNC: TPlayer::Equip = retail 0x005199b0. The item moves to inventory
// slot 0x100 + slot of this player; the item it displaces takes the item's
// old place (the same slot of the same container, or a free carried slot
// when the item came from an equipment slot or from no container).
// Equipping an item already at its equipment slot moves nothing, which
// RefreshEquip relies on after a load. Ends with RefreshStats (0x0051c660):
// the item's STATLINE. Not ported: the equip effect hookup (0x00519500) and
// the body-part rebuild (0x00584e00); a new combat or bow root without its
// animation ends the fight there (EndFighting 0x004d3fd0), where the port
// ends the combat or bow mode.
bool TPlayer::Equip(TObjectInstance* oi, int32_t slot)
{
    if (equipSeam)
        return equipSeam(this, oi, slot);
    if (slot < 0)
    {
        if (!oi || oi->FindStat("EqSlot") < 0)
            return false;
        slot = oi->GetStat("EqSlot");
    }
    if ((uint32_t)slot >= NUM_EQ_SLOTS)
        return false;
    if (equipment[slot] == oi)
        return true;
    if (oi && !CanEquip(oi, slot))
        return false;

  // Inventory bookkeeping
    TObjectInstance* displaced = equipment[slot];
    TObjectInstance* container = (oi && oi->GetOwner()) ? oi->GetOwner() : this;
    int32_t place = (oi && oi->GetOwner()) ? oi->InventNum() : -1;
    if (place < 0 || (place >= kInvSlotEquipFirst && place < kInvSlotBeltFirst))
        place = container->FindFreeInventorySlot();
    const int32_t equipslot = kInvSlotEquipFirst + slot;
    if (container == this)
    {
        if (displaced)
            displaced->SetInventNum(short(place));
        if (oi && !oi->GetOwner())
            AddToInventory(oi, equipslot);
        else if (oi)
            oi->SetInventNum(short(equipslot));
    }
    else
    {
        if (oi)
            oi->RemoveFromInventory();
        if (displaced)
        {
            displaced->RemoveFromInventory();
            container->AddToInventory(displaced, place);
        }
        if (oi)
            AddToInventory(oi, equipslot);
    }

  // Get old combat root name
    char old[RESNAMELEN];
    if (slot == EQ_PRIMEHAND)
        strcpy(old, GetCombatRoot());
    else if (slot== EQ_RANGEDWEAPON)
        strcpy(old, GetBowRoot());

  // Change slot
    equipment[slot] = oi;
    if (this == Player)
        EquipPane.SetDirty(true);

  // A light taken up while walking: the torch walk (0x00519c24..0x00519ca1)
    if (oi && oi->ObjClass() == OBJCLASS_LIGHTSOURCE && IsWalkMode())
        SetWalkMode();

  // Combat root changed
    if (IsCombat() && slot == EQ_PRIMEHAND && stricmp(old, GetCombatRoot()) != 0)
    {
        if (!HasActionAni(GetCombatRoot()))
            EndCombat();
        else
        {
            PTActionBlock ab = new TActionBlock(GetCombatRoot(), ACTION_COMBAT);
            ab->interrupt = true;
            ab->angle = doing->angle;
            ab->moveangle = doing->moveangle;
            ForceCommand(ab);
            SetRoot(ab);        // Set new root
        }
    }
    
  // Bow root changed
    if (IsBowMode() && slot == EQ_RANGEDWEAPON && stricmp(old, GetBowRoot()) != 0)
    {
        if (!HasActionAni(GetBowRoot()))
            EndBowMode();
        else
        {
            PTActionBlock ab = new TActionBlock(GetBowRoot(), ACTION_BOW);
            ab->interrupt = true;
            ab->angle = doing->angle;
            ab->moveangle = doing->moveangle;
            ForceCommand(ab);
            SetRoot(ab);        // Set new root
        }
    }

    RefreshStats();
    return true;
}

// REVSYNC: TPlayer::UseBelt = retail 0x0051e420. A weapon or armour goes to
// its own equipment slot (EquipByName 0x00519ea0, the item it displaces
// taking the belt box); anything else is used (vtable 0xbc, Use(this, -1)).
// Retail's EquipByName equips the first item of the box item's name in the
// inventory -- the same item unless two share the name; the port equips the
// box's own.
void TPlayer::UseBelt(int32_t box)
{
    TObjectInstance* item = GetInventorySlot(kInvSlotBeltFirst + box);
    if (!item)
        return;
    if (item->ObjClass() == OBJCLASS_WEAPON || item->ObjClass() == OBJCLASS_ARMOR)
        Equip(item, -1);
    else
        item->Use(this, -1);
}

// Returns the modifier value for the given player stat
int32_t TPlayer::PlyrStatMod(int32_t plyrstat, int32_t flags)
{
    int32_t mod;

    switch (PlyrStat(plyrstat))
    {
      case 0: case 1:  case 2:
        mod = -4;
        break;
      case 3: case 4: case 5: case 6:
        mod = -3;
        break;
      case 7: case 8: case 9:
        mod = -2;
        break;
      case 10: case 11:
        mod = -1;
        break;
      case 12: case 13: case 14: case 15:
        mod = 0;
        break;
      case 16: case 17:
        mod = 1;
        break;
      case 18: case 19: case 20:
        mod = 2;
        break;
      case 21: case 22: case 23: case 24:
        mod = 3;
        break;
      case 25: case 26: case 27: case 28: case 29:
        mod = 4;
        break;
      default:
        if (plyrstat < 0)
            mod = -5;
        else
            mod = 5;
        break;
    }

    if (flags == PLYRSTATMOD_HALFROUNDUP)
    {
        if (mod > 0)
            mod = (mod + 1) / 2;
        else
            mod = (mod - 1) / 2;
    }
    else if (flags == PLYRSTATMOD_HALFROUNDDOWN)
    {
        mod = mod / 2;
    }

    return mod;
}

// Returns a modifier value for skill based on a physical attribute
int32_t TPlayer::PlyrStatSkillMod(int32_t skill)
{
    return 0;
}

char *TPlayer::GetCombatRoot(TObjectInstance* oi)
{
    if (!oi)
        oi = PrimeHand();

    if (!oi)
        return "hand";

    if (oi->ObjClass() != OBJCLASS_WEAPON)
        return "combat";

    PTWeapon weapon = (PTWeapon)oi;

    if (weapon->Type() == WT_STAFF)
        return "cstaff";
    else if (weapon->Type() == WT_KNIFE)
        return "cknife";
    else
        return "combat";
}

char *TPlayer::GetBowRoot(TObjectInstance* oi)
{
    if (!oi)
        oi = RangedWeapon();

    if (!oi || oi->ObjClass() != OBJCLASS_WEAPON)
        return "bow";

    PTWeapon weapon = (PTWeapon)oi;

    if (weapon->Type() == WT_CROSSBOW)
        return "cbow";
    else 
        return "bow";
}

// Special cheat function
void TPlayer::GetOnYerHog()
{
    int32_t newtype = PlayerClass.FindObjType(OnTheHog ? "Locke" : "Hog");
    if (newtype < 0)
        return;

    FreeAnimator();

    TObjectImagery::FreeImagery(imagery);
    imagery = TObjectImagery::LoadImagery(PlayerClass.GetObjType(newtype)->imageryid);
    if (!imagery)
        return;

    objtype = newtype;

    if (!OnTheHog)
    {
        SoundPlayer.Mount("hog idle");
        SoundPlayer.Mount("hog drive");

        SoundPlayer.Play("hog idle");

        OnTheHog = true;
    }
    else
    {
        SoundPlayer.Unmount("hog idle");
        SoundPlayer.Mount("hog drive");

        OnTheHog = false;
    }
}

// Gets the talisman list for the given quickspell
const char *TPlayer::GetQuickSpell(int32_t button) const
{
    if ((uint32_t)button >= QSPELL_NUM)
        return "";

    return quickspells[button];
}

// Sets the quickspell button
void TPlayer::SetQuickSpell(int32_t button, const char *talismans)
{
    if ((uint32_t)button >= QSPELL_NUM)
        return;

    strncpyz(quickspells[button], talismans, MAXTALISMANLEN);

  // Update spell panes
    if (button == QSPELL_CONSTRUCT)
        SpellPane.SetDirty(true);
    else
        QuickSpells.SetDirty(true);
}

// REVSYNC: TPlayer::InvokeQuickSpell @ 0x0051b5d0 -- cast quick spell
// `button` with no target: alive and not in an interactive attack or impact;
// an empty slot or a missing talisman fizzles; a cast that fails fizzles
// (TCharacter::CastByTalismans has usually fizzled already). The main player
// is told how it went. Retail's multiplayer-client branches (no fizzle and
// no message when a talisman is missing or the cast fails) have no port: the
// port has no network play.
bool TPlayer::InvokeQuickSpell(int32_t button)
{
    if ((uint32_t)button >= QSPELL_NUM || Health() <= 0)
        return false;
    if (!(charflags & kCharFlagInteractive))
    {
        if (doing->attack && (doing->attack->flags & CA_INTERACTIVE))
            return false;
        if (doing->impact && (doing->impact->flags & CAI_INTERACTIVE))
            return false;
    }

    char *talismans = quickspells[button];
    const bool mainplayer = (this == Player);
    if (!talismans[0])
    {
        if (mainplayer)
            TextBar.Print("%s", DialogList.GetLine("SPLNOTAL"));
        CastByName("Fizzle");
        return true;
    }
    if (!HasTalismans(talismans))
    {
        if (mainplayer)
            TextBar.Print("%s", DialogList.GetLine("SPLMISTAL"));
        CastByName("Fizzle");
        return true;
    }
    if (!CastByTalismans(talismans))
    {
        CastByName("Fizzle");
        if (mainplayer)
            TextBar.Print("%s", DialogList.GetLine("SPLCASTFAIL"));
    }
    else if (mainplayer)
        TextBar.Print("%s", DialogList.GetLine("SPLCASTOK"));
    return true;
}

// REVSYNC: TPlayer::HasTalismans @ 0x0051b7c0 -- the Spell Pouch's talismans,
// counted by TALISMAN type name, cover the string: each code (the type's Code
// stat, case-sensitive) takes one. Codes of no type (M-P) cost nothing.
bool TPlayer::HasTalismans(const char *talismans) const
{
    bool has = true;
    int32_t len = strlen(talismans);
    int32_t x;

    // clear array for counting talismans
    int32_t quanttal[25];
    for (x = 0; x < 25; x++)
        quanttal[x] = 0;

    // now parse the inventory and count how many of each talisman there are
    // find their spell pouch. Retail looks for a "spellpouch" when there is
    // none and drops the answer, so only a Spell Pouch counts.
    TObjectInstance* pouch = FindObjInventory("Spell Pouch");
    if (!pouch)
        (void)FindObjInventory("spellpouch");
    if (pouch)
    {
        for(x = 0; x < TalismanClass.NumTypes(); ++x)
        {
            char *talname = TalismanClass.GetObjType(x)->name;
            for (TInventoryIterator i(pouch); i; i++)
            {
                if (stricmp(i.Item()->GetName(), talname) == 0)
                    quanttal[x]++;
            }
        }
    }

    // now go through this button's talismans and check if they have at least that many of each
    for (int32_t i = 0; i < len; i++)
    {
        for(int32_t j = 0; j < TalismanClass.NumTypes(); j++)
        {
            char code = TalismanClass.GetStat(j, "Code"); 
            if (code == talismans[i])
            {
                if (--quanttal[j] < 0)
                    has = false;
            }                               
        }
    }

    return has;
}

bool TPlayer::LearnSpell(const char* talismans)
{
    if (!talismans || !*talismans)
        return false;
    for (const TSpellCode& code : knownspells)
    {
        if (strncmp(code.data(), talismans, kSpellCodeBytes) == 0)
            return false;
    }
    TSpellCode& code = knownspells.emplace_back();
    strncpy(code.data(), talismans, kSpellCodeBytes);
    ++knownspellsversion;
    return true;
}

void TPlayer::ForgetSpell(int32_t i)
{
    if (i >= 0 && i < NumKnownSpells())
    {
        knownspells.erase(knownspells.begin() + i);
        ++knownspellsversion;
    }
}

bool TPlayer::TrainStat(int32_t attribute)
{
    if (attribute < 0 || attribute >= NUM_PLRSTATS)
        return false;
    const int32_t value = GetObjStat(PLRSTAT_FIRST + attribute);
    int32_t &last = levelupstats[2];                // the pick made last (+0x368)
    if (value >= 30 || (attribute == levelupstats[last != 0] && value >= 29))
        return false;
    last = last == 0 ? 1 : 0;
    levelupstats[last] = attribute;
    return true;
}

void TPlayer::SetMoney(int32_t amount)
{
    constexpr const char* kGoldName = "gold";       // 0x005e2b44
    TObjectInstance* gold = FindObjInventory(kGoldName);
    if (amount < 1)
    {
        if (gold)
        {
            gold->RemoveFromInventory();
            delete gold;
        }
    }
    else if (!gold)
        AddToInventory(kGoldName, amount);
    else
        gold->SetAmount(amount);
}

// REVSYNC: TPlayer::SetPlayerState @ 0x0051d680. Not ported: with bit 2 set
// retail also stopped certain actions in progress (0x004cee70), and the
// multiplayer control and message handling.
void TPlayer::SetPlayerState(int32_t newstate)
{
    if (playerStateSeam)
        playerStateSeam(this, newstate);
    playerstate = newstate;
}


// *******************************************************************
// * TPlayerManager - Object to manager the player list for the game *
// *******************************************************************

// Initialize the player manager (idempotent).
bool TPlayerManager::Initialize()
{
    if (initialized)
        return true;
    players.Clear();
    mainplayernum = -1;
    Player = nullptr;
    initialized = true;
    return true;
}

// Closes the player manager (idempotent — leaves the array empty so the
// trivial default dtor only walks zeroed state).
void TPlayerManager::Close()
{
    if (!initialized)
        return;
    players.DeleteAll();
    Player = nullptr;
    mainplayernum = -1;
    initialized = false;
}

// Clears the player manager (deletes all players for the current game)
void TPlayerManager::Clear()
{
    SetMainPlayer(-1);
    players.DeleteAll();
}

// Sets the main player for the game
void TPlayerManager::SetMainPlayer(int32_t newplayernum)
{
    if (mainplayernum == newplayernum)
        return;

    mainplayernum = newplayernum;

    if ((uint32_t)mainplayernum >= (uint32_t)players.NumItems())
        mainplayernum = players.NumItems() - 1;  // The last player added

    Player = mainplayernum < 0 ? nullptr : GetPlayer(mainplayernum);
    if (CurrentScreen == &PlayScreen)
        BindMainPlayerToScreen();
}

// The play screen's hookups for the main player: the camera follows it, and
// the equipment and inventory panes show it (none: the camera stays where it
// is and the inventory pane shows nothing). Retail made them in
// SetMainPlayer, which ran inside TPlayScreen::Initialize (0x0047a660). The
// port's session loads a game under the loading screen, so the play screen
// calls this as it starts.
//
// REVSYNC: SetMainPlayerNum @ 0x0051eef0 -- the camera jumps to the player at
// its next update, then follows it: scrolling to it with SquishyScroll on
// (flags 12 at 0x0051efda), jumping to it each update with it off (8).
void TPlayerManager::BindMainPlayerToScreen()
{
    if (!Player)
    {
        S3DPoint pos;
        MapPane.GetMapPos(pos);
        MapPane.CenterOnPos(pos, MapPane.GetMapLevel());
        Inventory.SetContainer(nullptr);
        AutoMap.BindPlayer(nullptr);            // REVSYNC: 0x0051ef84
        return;
    }
    MapPane.CenterOnObj(Player, SquishyScroll ? CENTERON_SCROLL | CENTERON_SNAP : CENTERON_SNAP);
    Player->RefreshEquip();
    Inventory.SetContainer(Player);
    AutoMap.BindPlayer(Player);                 // REVSYNC: 0x0051f03e
}

// Sets the main player for the game
void TPlayerManager::SetMainPlayer(TPlayer* player)
{
    int32_t newplayernum = -1;

    for (int32_t c = 0; c < players.NumItems(); c++)
    {
        if (players[c] == player)
        {
            newplayernum = c;
            break;
        }
    }

    SetMainPlayer(newplayernum);
}

// Add player
int32_t TPlayerManager::AddPlayer(TPlayer* newplayer)
{
    int32_t c;
    int32_t empty = -1;
    for (c = 0; c < players.NumItems(); c++)
    {
        if (players[c] == nullptr)
            empty = c;
        else if (players[c] == newplayer)
            return c;
    }

    if (empty >= 0)
        c = players.Set(newplayer, empty);
    else
        c = players.Add(newplayer);

    if (!Player)
        SetMainPlayer(newplayer);

    return c;
}

// Remove a player
// The collapse flag allows you to choose whether you want to leave an empty space
// in the player list or not.  You need to leave empty slots when in a net game so that
// player id numbers don't change.  When setting up a game, however, you need to collapse
// the list.
void TPlayerManager::RemovePlayer(int32_t removenum, bool collapse)
{
    if ((uint32_t)removenum >= (uint32_t)players.NumItems())
        return;

    if (collapse)
        players.Collapse(removenum, false);
    else
        players.Remove(removenum);

    if (removenum == mainplayernum)
        SetMainPlayer(-1); // Set to last player in list
}

// Remove a player (given player pointer)
void TPlayerManager::RemovePlayer(TPlayer* removeplayer, bool collapse)
{
    int32_t removenum;
    for (removenum = 0; removenum < players.NumItems(); removenum++)
    {
        if (players[removenum] == removeplayer)
        {
            RemovePlayer(removenum, collapse);
            return;
        }
    }
}       
