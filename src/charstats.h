// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                CharStats.h - Defines character stats                  *
// *************************************************************************
//
// Indexes into the CHARACTER and PLAYER object-stat arrays. They are the
// positions the shipped game registers its stats at (SStatEntry constructor
// 0x00473e40; recon/scripts/object_stats.py lists them), which is also the
// order sector and save files list an object's stats in and the order of
// the per-type default lists in class.def. The 1998 source had a shorter
// layout (level at 6, no damage resistances or player modifiers).
// docs/gameflow/forensics/SAVE_GAME.md §11.2.

#ifndef _CHARSTATS_H
#define _CHARSTATS_H

// Character flag defines
#define CHRFLAG_FIRST       0
#define NUM_CHRFLAGS        3

#define CHRFLAG_AGGRESSIVE  0
#define CHRFLAG_POISONED    1
#define CHRFLAG_SLEEPING    2


// Character stat defines
#define NUM_CHRSTATS        3
#define CHRSTAT_FIRST       (CHRFLAG_FIRST + NUM_CHRFLAGS)

#define CHRSTAT_HEALTH      0           // Current health
#define CHRSTAT_FATIGUE     1           // Current fatigue
#define CHRSTAT_MANA        2           // Current mana

// Character damage resistances (percent) and damage modifier
#define NUM_CHRRESISTS      10
#define CHRRESIST_FIRST     (CHRSTAT_FIRST + NUM_CHRSTATS)

#define CHRRESIST_MISC      0
#define CHRRESIST_HAND      1
#define CHRRESIST_PUNCTURE  2
#define CHRRESIST_CUT       3
#define CHRRESIST_CHOP      4
#define CHRRESIST_BLUDGEON  5
#define CHRRESIST_MAGICAL   6
#define CHRRESIST_BURN      7
#define CHRRESIST_FREEZE    8
#define CHRRESIST_POISON    9

#define CHRVAL_DAMAGEMOD    (CHRRESIST_FIRST + NUM_CHRRESISTS)
#define NUM_CHAROBJSTATS    (CHRVAL_DAMAGEMOD + 1)   // the CHARACTER class's object stats

// Player value defines (anything that's not a stat or a skill)
#define NUM_PLRVALS         17
#define PLRVAL_FIRST        NUM_CHAROBJSTATS

#define PLRVAL_LEVEL            0
#define PLRVAL_EXP              1
#define PLRVAL_NEXTEXP          2
#define PLRVAL_ATTACKLEVEL      3
#define PLRVAL_HEALTHPCT        4
#define PLRVAL_MANAPCT          5
#define PLRVAL_FATIGUEPCT       6
#define PLRVAL_MAXHEALTHFLAT    7
#define PLRVAL_MAXMANAFLAT      8
#define PLRVAL_MAXFATIGUEFLAT   9
#define PLRVAL_MAXHEALTHPCT     10
#define PLRVAL_MAXMANAPCT       11
#define PLRVAL_MAXFATIGUEPCT    12
#define PLRVAL_ACBONUS          13
#define PLRVAL_MANACOSTPCT      14
#define PLRVAL_SPELLDAMAGEINC   15
#define PLRVAL_EDGEBONUS        16

// Player stat defines
#define NUM_PLRSTATS        6
#define PLRSTAT_FIRST       (PLRVAL_FIRST + NUM_PLRVALS)

#define PLRSTAT_STRN        0           // Strength
#define PLRSTAT_CONS        1           // Constitution
#define PLRSTAT_AGIL        2           // Agility
#define PLRSTAT_RFLX        3           // Reflexes
#define PLRSTAT_MIND        4           // Mind
#define PLRSTAT_LUCK        5           // Luck

// Skill defines: the skill, its experience, the experience for its next
// level, and its cap, each a block of NUM_SKILLS stats
#define NUM_SKILLS          11
#define SK_FIRST            (PLRSTAT_FIRST + NUM_PLRSTATS)
#define SKE_FIRST           (SK_FIRST + NUM_SKILLS)
#define SKN_FIRST           (SKE_FIRST + NUM_SKILLS)
#define SKC_FIRST           (SKN_FIRST + NUM_SKILLS)

#define SK_ATTACK           0           // Offensive combat
#define SK_DEFENSE          1           // Defensive combat
#define SK_INVOKE           2           // Invocation ability
#define SK_HANDS            3           // Hand attacks          (WT_HAND)
#define SK_KNIFE            4           // Daggers, knives       (WT_SHORTBLADE)
#define SK_SWORD            5           // Swords                (WT_LONGBLADE)
#define SK_BLUDGEONS        6           // Clubs, maces, hammers (WT_BLUDGEON)
#define SK_AXES             7           // Axes                  (WT_AXE)
#define SK_BOWS             8           // Bows                  (WT_BOW)
#define SK_STEALTH          9           // Sneaking around
#define SK_LOCKPICK         10          // Picking locks

#define SK_WEAPONSKILLS (SK_HANDS)      // Where are the weapon skills

#endif
