#!/usr/bin/env python3
"""Combat A/B, retail side, the melee katas (docs/gameplay/COMBAT_DOJO.md
C2-C5; forensics COMBAT_ATTACK_CHOICE.md, COMBAT_HIT.md).

One original TCharacter method per case on a fixture world (guest.py's
CombatWorld) whose characters carry their whole character data -- the
attack table and impacts as char.def gives them, by the combat-data dump's
field names (data_parse.py's layouts) -- and their attack bookkeeping
(`attackstate`). The case's `call`:

- `iva`: IsValidAttack `0x4d1120` (thiscall, 12 args, ret 0x30) once per
  entry of `calls` ({attack, tdist, button, pcnt, dmgpcnt, mask, flags,
  targ, damage, tohit, roll}: the out-values start as given, -1 by
  default), in order on the same world.
- `find-button` FindButtonAttack `0x4d1dd0` (button, dmgpcnt, isaction,
  targ), `find-pcnt` FindPcntAttack `0x4d1eb0` / `find-interactive`
  FindInteractiveAttack `0x4d1ff0` (pcnt, dmgpcnt): the out-values from
  `args` (-1 by default).
- `do-attack` DoAttack `0x4d2120` (attack, impact, damage, tohit, roll,
  targ).
- `button-attack` ButtonAttack `0x4d2480` / `button-action` ButtonAction
  `0x4d27f0` (button), `random-attack` RandomAttack `0x4d2900` (pcnt),
  `specific-attack` SpecificAttack `0x4d2a60` (attack).
- `resolve-attack` ResolveAttack `0x4c6dd0` (bits) on the doing block,
  `resolve-hit` ResolveHit `0x4c62b0` (attack, impact, targ, damage, tohit,
  roll), `on-attacked` OnAttacked `0x4cdce0` (attacker, victim, flag).
- `damage` Damage `0x4c4950` (damage, type, mod, block, attacker): `block`
  a block spec as the case's roles take them, its attack / impact the
  attacker's by default; run with the case's `seam_damage` false.
- `resolve-impact` ResolveImpact `0x4c74b0`, `resolve-block` ResolveBlock
  `0x4c77a0`, `resolve-dead` ResolveDead `0x4c7810` (bits) on the doing
  block; `block` Block `0x4d2e30` (frames; with `seam_block` false),
  `stop-block` StopBlock `0x4d30f0`, `dodge` Dodge `0x4d3150` (dir).
- `ai` AI `0x4c8b60` (one tick). A character's `aistate` gives its AI
  state: `lookat` (+0x234, a name), `lastpos` (+0x23c), `waypointticks`
  (+0x248), and Wander's: `lastwaypoint` (+0x238, a name), `wanderidle`
  (+0x24c), `runticks` (+0x250); NetOwner reads 1 (`0x676838`, single
  player).
- `per-monster` (kata C3c, BOSS_AI.md): `ticks` ticks of AI_PerMonster
  `0x4c9b70` (tick k on game frame `frame` + k), each followed by a `Tick`
  record. A character's `boss` gives its state (+0x284, by BOSS_FIELDS'
  names; a buffer of the kind's size, the rest zero; Baez's `watchers` are
  the global crowd `0x66caac`, names or null); the case's `props` are the
  map's other objects ({name, type, class, pos, id, states}: a name at
  +0x38, a type at +0x4c, SetState through the shared seam, ResetFlags
  `0x472e90` as original); `paneobjects` the names the pane's walk gives
  (`0x44cf80`, recorded `PaneObjects`); `made` the characters NewObject
  hands out in turn (their ids; with none left an id no object has).
  GetInstance `0x452690` answers the ids of the case's characters (but
  `made` ones not yet handed out) and props. Seams recorded: EndFighting
  `0x4d3fd0`, SetControl `0x47c580`, Tint / Untint `0x4d6a30` / `0x4d6b40`,
  CreateAnimator `0x46e8b0` (a stand-in animator at +0x58). SetCameraObject
  `0x4538d0` runs as original; the dump gives the camera (MapPane +0xdc,
  +0xd8). A case's `bossworld` gives an `ai` call the same world.
- `pulse` (kata C6, CHARACTER_PULSE.md): `ticks` ticks of the character's
  Pulse (TCharacter `0x4c1bb0`, TPlayer `0x518aa0`). Tick k runs on game
  frame `frame` + k, game time `gametime` + k * 100 / 24 (PlayScreen
  `+0x68c`) and screen frame `screenframe` + k (CurrentScreen `+0x48`), and
  is followed by a `Tick` record (the vitals). A character's `pulsestate`
  gives the tick's own fields (PULSESTATE), `noai` takes OF_AI off; a
  player's `maxhealth` / `maxfatigue` / `maxmana` answer those slots.
- `kill-exp` KillExp `0x51a5b0` (value), `exp-kill` AwardKillExp
  `0x51a630` (victim), `exp-skill` AwardSkillExp `0x51abe0` (skill,
  victim), `exp-stealth` AwardStealthExp `0x51ad80` (victim),
  `add-skill-exp` AddSkillExp `0x51ac90` (skill, exp) (kata C7): the
  originals, with the case's `seam_exp` false. A player's `basestats` are
  its stats themselves (`+0xa8` count, `+0xac` values), `levelup` the two
  attributes the level-up raises (`+0x360`).

Runs as original code: the method and what it calls -- the searches,
IsValidAttack, DoAttack, HasActionAni, CombatAnimName `0x4ce1b0` and the
prefix `0x4cdf60`, CalculateDamage, GetDamageType, the stat slots
(Offense / Defense / Luck / StrengthMod / DamageMod / EdgeBonus /
AttackModifier / DefenseModifier / Resist / ArmorValue), the STATLEVEL
lookup over the case's tables, FaceAngleTo, GetSnapPos `0x46f010`,
Distance, SetDesired / ForceCommand, the TActionBlock ctor, the network
sends (no session: each returns at its first test).

Seams (guest.py's, and these), recorded in order:
- FindCharacters `0x4cd690`: the case's `found` (a name, or none) -- as
  original with the case's `perception` (the senses, CanSeeCharacter and
  IsEnemy too, over the map iterator's characters; the line of sight
  between two characters' eyes clear unless `walls` name the pair, or with
  a `ground` the original over it, as combat_call.py's).
- FindClearPath `0x4c39d0`: the case's `blocked`; when blocked, the
  blocker is the case's `blocker` (a name, or none) -- as original over
  the case's `ground` with `seam_path` false.
- Block `0x4d2e30` (frames): recorded on the character, the case's
  `block_result` (1); the original runs with the case's `seam_block` false.
- EffectCombatFlash `0x4c8500`: recorded (it spawns an effect).
- TMapPane::FindObjectsInRange `0x452060` (AI's waypoint search): the
  case's `waypoints` ({name, pos, type, level}), in order, recorded;
  GetInstance `0x452690` gives each (a helper: class 0xf, its type's name
  at +0x4c, map index HELPER_INDEX + k at +0x40, level +0xe, out of any
  inventory, IsInInventory its only virtual); LineOfSight `0x4533d0` from
  one (to a character) clear unless the case's `walls` name the pair --
  only in a case with waypoints. With `real_map`, both run as original
  instead: the search over the helpers (guest.py's map iterator seam), the
  line of sight over the case's `ground`.
- Wander's (kata M9, the case's `seam_wander`): SetWalkMode `0x4cf000`,
  SetRunMode `0x4cf490`, EndFighting `0x4d3fd0`, each recorded (result 1).
- CanSeeCharacter `0x4cd540` (target, angle): the case's `sees` (1; or
  the names seen), recorded.
- IsEnemy `0x4c89c0`: an enemy unless the case's `friends` hold the pair,
  recorded.
- BeginFighting `0x4d3b90` (target, action): recorded, 1; the original runs
  with the case's `seam_begin` false.
- The spell list's Find `0x53f010` and Cast `0x4d5c20`, as one record
  `CastByName` (spell, targets, source, result): a spell in the case's
  `spells` is cast with `cast_result` (1); any other isn't found (result 0).
- TPlayer WeaponType `0x520810` / WeaponDamage `0x520830`: the player's
  `weapon` {type, damage}.
- TPlayer SetPlayerState `0x51d680`: recorded, the value stored.
- The tick's (`pulse`, the scenario kata's -- scenario.py --, and the
  experience calls'): UpdateAction `0x4c3260` (for `pulse`, with the
  case's `seam_update`, by default) and AI `0x4c8b60` (with `seam_ai`)
  recorded and skipped; the spell list's pulse `0x540750`
  skipped; the death screen `0x48ea40` recorded; RefreshStats `0x51c660`
  recorded and skipped; the effect type lookup `0x475210` and NewObject
  `0x450e40` (the def recorded, a stand-in made whose SetParams, slot
  0x200, is recorded as `blood.SetParams`); TPlayer MaxHealth / MaxFatigue
  / MaxMana from the case. With `quiet_reads`, reading a stat isn't
  recorded (the tick reads them over and over; what is set is).

Rules (`rules` in the case, the combat-data dump's names): TOHITCENTER ..
TOHITDAMAGE at Rules `0x65d7a8` + `0x94` .. `0xcc`, the six STATLEVEL
tables behind `+0x58`. Globals: guest.py's, `nahkranoth` (`0x668108`),
`alreadydead` (`0x668104`), `nocombatresults` (`0x668194`).

Schema `combat.melee.v1`, shared with the port's `Revenant
--retail-ab=melee-*` (src/game/ab/retailab_melee.cpp).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from guest import (AB, G_PLAYSCREEN, LITERAL_OBJSTATS, MALLOC, OBJSTAT_IDS, Boundaries, CombatWorld, call,  # noqa: E402
                   flag_names, s32, serve, start, write_fields, write_text)
from data_parse import (A_IMPACTS, A_NUMIMPACTS, ATTACK_BODY, ATTACK_HEAD, ATTACK_MAGIC, C_ATTACKS,  # noqa: E402
                        C_IMPACTS, C_NUMIMPACTS, CA_MAGICATTACK, CHAR_FIELDS, IMPACT_FIELDS, IMPACT_SIZE)

SCHEMA = 'combat.melee.v1'
TEXT = 'cp1252'
TAG_SOUND = 0x40af50                             # thiscall (object, name, out, &volume), ret 0x10

IS_VALID_ATTACK = 0x4d1120
FIND_BUTTON, FIND_PCNT, FIND_INTERACTIVE = 0x4d1dd0, 0x4d1eb0, 0x4d1ff0
DO_ATTACK = 0x4d2120
BUTTON_ATTACK, BUTTON_ACTION, RANDOM_ATTACK, SPECIFIC_ATTACK = 0x4d2480, 0x4d27f0, 0x4d2900, 0x4d2a60
FIND_CHARACTERS, FIND_CLEAR_PATH, BLOCK = 0x4cd690, 0x4c39d0, 0x4d2e30
STOP_BLOCK, DODGE = 0x4d30f0, 0x4d3150
RESOLVE_IMPACT, RESOLVE_BLOCK, RESOLVE_DEAD = 0x4c74b0, 0x4c77a0, 0x4c7810
EFFECT_COMBAT_FLASH = 0x4c8500
AI, FIND_OBJECTS_IN_RANGE, CAN_SEE = 0x4c8b60, 0x452060, 0x4cd540
GET_INSTANCE, LINE_OF_SIGHT = 0x452690, 0x4533d0
HELPER_INDEX = 0x7000                           # map indices the case's helpers take
IS_IN_INVENTORY, SLOT_IS_IN_INVENTORY = 0x477a10, 0xb4
G_NETOWNER, G_EDITOR = 0x676838, 0x668154
O_LOOKAT, O_LASTPOS, O_WAYPOINTTICKS, O_MONSTERKIND = 0x234, 0x23c, 0x248, 0x280
O_LASTWAYPOINT, O_WANDERIDLE, O_RUNTICKS = 0x238, 0x24c, 0x250
# Wander's callees the port has as 1998 code (their own katas): thiscall, no arguments.
WANDER_SEAMS = ((0x4cf000, 'SetWalkMode'), (0x4cf490, 'SetRunMode'), (0x4d3fd0, 'EndFighting'))
SPELL_FIND, CAST = 0x53f010, 0x4d5c20
PLAYER_WEAPONTYPE, PLAYER_WEAPONDAMAGE, SET_PLAYER_STATE = 0x520810, 0x520830, 0x51d680
# Hit resolution (COMBAT_HIT.md): original ResolveAttack / ResolveHit /
# OnAttacked; seams for what lies past them.
RESOLVE_ATTACK, RESOLVE_HIT, ON_ATTACKED = 0x4c6dd0, 0x4c62b0, 0x4cdce0
IS_ENEMY, BEGIN_FIGHTING, EFFECT_BURST, DAMAGE = 0x4c89c0, 0x4d3b90, 0x4c85d0, 0x4c4950
SOUND_FIND = 0x49c430                            # the sound manager's lookup by name (thiscall, ret 4)
TEXT_FIND, TEXT_GET = 0x49d6d0, 0x49d800         # the dialog list: id of a tag / its line
TEXTBAR_PRINT = 0x54d170                         # cdecl (bar, fmt, ...)
EXP = {'kill': (0x51a630, 4), 'skill': (0x51abe0, 8), 'stealth': (0x51ad80, 4)}   # TPlayer slots 0x414/0x41c/0x420

# Kata C6, the character's tick (CHARACTER_PULSE.md), and C7, experience.
PULSE = {12: 0x4c1bb0, 11: 0x518aa0}            # TCharacter::Pulse, TPlayer::Pulse
SPELL_LIST_PULSE, UPDATE_ACTION, DEATH_SCREEN, REFRESH_STATS = 0x540750, 0x4c3260, 0x48ea40, 0x51c660
FIND_OBJ_TYPE, NEW_OBJECT = 0x475210, 0x450e40   # thiscall (name, 0) ret 8; thiscall (def, -1) ret 8
PLAYER_MAX = {'MaxHealth': (0x520630, 'maxhealth'), 'MaxFatigue': (0x5206d0, 'maxfatigue'),
              'MaxMana': (0x520770, 'maxmana')}
KILL_EXP, ADD_SKILL_EXP = 0x51a5b0, 0x51ac90
SCRIPT_TRIGGER, O_SCRIPT = 0x492640, 0x84         # TScript::Trigger (thiscall, 7 args); the object's script
G_SCREEN, G_UPDATING_RECT = 0x667fd0, 0x6682c4   # CurrentScreen (frames +0x48); TComplexObject::Pulse's skip
G_PS_5D4 = 0x65d0c4                              # PlayScreen +0x5d4: Pulse returns at once while set
G_GAMETYPES, G_GAMETYPE = 0x65a77c, 0x65a784     # game type table / index (flags bit 8: player killing)
G_AUTOBEGIN, G_NETGAME, G_NETHOST = 0x5d7a68, 0x676828, 0x67682c
PS_GAMETIME, SCREEN_FRAMES = 0x68c, 0x48
BLOOD_INDEX = 0x7100                             # map indices the made objects take
OF_AI = 0x20
# Rules: the recovery data (HEALTHDATA etc.), the experience tables.
RULES_RECOVERY = [('healthrecovval', 0x74), ('fatiguerecovval', 0x78), ('manarecovval', 0x7c),
                  ('healthrecovrate', 0x80), ('fatiguerecovrate', 0x84), ('manarecovrate', 0x88),
                  ('poisondamageval', 0x8c), ('poisondamagerate', 0x90)]
R_EXP, R_SKILLEXP = 0xf0, 0x168
# The tick's own fields (CHARACTER_PULSE.md §1), by retail's names.
PULSESTATE = [('lasthealth', 0x138), ('lastfatigue', 0x13c), ('lastmana', 0x140), ('lastrun', 0x144),
              ('accumhealth', 0x148), ('accumfatigue', 0x14c), ('accummana', 0x150), ('accumrun', 0x154),
              ('accumpoison', 0x158), ('lastpoison', 0x15c), ('invokedelay', 0x188), ('fade', 0x194),
              ('fadestep', 0x198), ('fadelimit', 0x19c), ('fadedir', 0x1a0), ('burnticks', 0x1bc),
              ('deadticks', 0x228), ('exittimestamp', 0x114), ('forcecommanddone', 0x108)]
PLAYER_PULSESTATE = [('deathcountdown', 0x39c), ('stateframes', 0x374), ('statetime', 0x370),
                     ('stateffectexpires', 0x354)]
# The object flags the tick sets or clears: OF_KILL, OF_ONEXIT, OF_PARALIZE, OF_ICED, OF_VIRGIN.
PULSE_OBJFLAGS = 0x1000 | 0x100000 | 0x800000 | 0x2000000 | 0x4000000
P_BASECOUNT, P_BASESTATS, P_LEVELUP = 0xa8, 0xac, 0x360
O_CLASSREC = 0x48                                # the object's class record
ATTRIBUTE_NAMES = {34: 'Strn', 35: 'Cons', 36: 'Agil', 37: 'Rflx', 38: 'Mind', 39: 'Luck'}
NUM_OBJSTATS = 73                                # every player stat id (charstats.h)
EXP_CALLS = ('kill-exp', 'exp-kill', 'exp-skill', 'exp-stealth', 'add-skill-exp')
# The calls that run the tick: C6, the scenario kata (scenario.py), C7.
TICKING_CALLS = ('pulse', 'scenario') + EXP_CALLS
EYE_HEIGHT = 50                                  # LIGHTINGCHARHEIGHT (CanSeeCharacter's eyes)

# Kata C3c, the bosses (BOSS_AI.md): AI_PerMonster and what it reaches.
PER_MONSTER, PANE_ITER = 0x4c9b70, 0x44cf80      # thiscall (); iterator init thiscall, 5 args, ret 0x14
END_FIGHTING, SET_CONTROL, CREATE_ANIMATOR = 0x4d3fd0, 0x47c580, 0x46e8b0
TINT, UNTINT, SET_STATE, RESET_FLAGS = 0x4d6a30, 0x4d6b40, 0x46f250, 0x472e90
G_WATCHERS, WATCHERS = 0x66caac, 11              # Baez's crowd: 11 pointers
MAPPANE, MP_CAMERA_FLAGS, MP_CAMERA_OBJ = 0x6668d8, 0xd8, 0xdc
O_BOSS, OF_NOWALK, MADE_INDEX = 0x284, 0x400000, 0x7200
BOSS_SIZE = {1: 0x58, 2: 0x34, 3: 0x5c, 4: 0xc}
# Each boss's state by the port's names (bossai.h): an offset (an int32),
# ('pt', offset) a point, ('ints', offset, n) int32s; Jhaga's points keep x,
# y and z in three arrays of five.
BOSS_FIELDS = {
    1: [('stunned', 0x00), ('stunticks', 0x04), ('stunlevel', 0x08), ('stage', 0x0c), ('baezspot', ('pt', 0x10)),
        ('playerspot', ('pt', 0x1c)), ('table', 0x28), ('frame', 0x2c), ('throwlength', 0x30),
        ('watchersfound', 0x34), ('playerhealth', 0x38), ('health', 0x3c), ('falling', 0x40),
        ('firstspot', ('pt', 0x44)), ('playerarrived', 0x50), ('waitticks', 0x54)],
    2: [('skytimer', 0x00), ('skyphase', 0x04), ('skyticks', 0x08), ('band', 0x0c), ('stunticks', 0x10),
        ('stunned', 0x14), ('tinted', 0x18), ('brood', ('ints', 0x1c, 3)), ('landing', 0x28),
        ('pissedoff', 0x2c), ('slam', 0x30)],
    3: [('mode', 0x00), ('points', ('jhaga', 0x04)), ('resttimer', 0x40), ('draw', 0x44), ('hop', 0x48),
        ('dest', ('pt', 0x4c)), ('frame', 0x58)],
    4: [('mode', 0x00), ('towalk', 0x04), ('tocombat', 0x08)]}

# The functions under test, for coverage: [start, end).
COVERED = {'AnimPrefix': (0x4cdf60, 0x4ce1b0), 'CombatAnimName': (0x4ce1b0, 0x4ce290),
           'IsValidAttack': (0x4d1120, 0x4d1db8), 'FindButtonAttack': (0x4d1dd0, 0x4d1eab),
           'FindPcntAttack': (0x4d1eb0, 0x4d1feb), 'FindInteractiveAttack': (0x4d1ff0, 0x4d211f),
           'DoAttack': (0x4d2120, 0x4d2480), 'ButtonAttack': (0x4d2480, 0x4d27e3),
           'ButtonAction': (0x4d27f0, 0x4d28fb), 'RandomAttack': (0x4d2900, 0x4d2a53),
           'SpecificAttack': (0x4d2a60, 0x4d2bd9), 'ResolveHit': (0x4c62b0, 0x4c6dca),
           'ResolveAttack': (0x4c6dd0, 0x4c748c), 'OnAttacked': (0x4cdce0, 0x4cdef1),
           'Damage': (0x4c4950, 0x4c5810), 'ObjectDamage': (0x46e970, 0x46ea0d),
           'PlayerKilled': (0x518ed0, 0x518f8b), 'PlayerDied': (0x518f90, 0x518fda),
           'ResolveImpact': (0x4c74b0, 0x4c7798), 'ResolveBlock': (0x4c77a0, 0x4c7804),
           'ResolveDead': (0x4c7810, 0x4c7973), 'Block': (0x4d2e30, 0x4d30ec), 'StopBlock': (0x4d30f0, 0x4d3147),
           'Dodge': (0x4d3150, 0x4d3471), 'AI': (0x4c8b60, 0x4c9453), 'AIPerMonster': (0x4c9b70, 0x4c9ce8),
           'Baez': (0x4c9ce8, 0x4cbb1d), 'Solifuge': (0x4cbb1d, 0x4ccaa4), 'Jhaga': (0x4ccaa4, 0x4cd07d),
           'Yhagoro': (0x4cd07d, 0x4cd4ac),
           'Pulse': (0x4c1bb0, 0x4c3256), 'PlayerPulse': (0x518aa0, 0x518dea),
           'TComplexObject::Pulse': (0x4db190, 0x4db1c5), 'KillExp': (0x51a5b0, 0x51a625),
           'AwardKillExp': (0x51a630, 0x51abdb), 'AwardSkillExp': (0x51abe0, 0x51ac8e),
           'AddSkillExp': (0x51ac90, 0x51ad7e), 'AwardStealthExp': (0x51ad80, 0x51ada8),
           'Wander': (0x4c9790, 0x4c9ae8), 'PickWanderWaypoint': (0x4c95e0, 0x4c9785),
           'ProbeLedge': (0x4c9460, 0x4c95d9)}

ATTACK_SIZE = 0x320
RULES = 0x65d7a8
RULES_TOHIT = [('tohitcenter', 0x94), ('tohitrangechar', 0x98), ('tohitrangeplyr', 0x9c),
               ('tohitblock', 0xa0), ('tohitface', 0xa4)]
R_TOHITDAMAGE, R_STATLEVELS = 0xa8, 0x58
G_NAHKRANOTH, G_ALREADYDEAD, G_NOCOMBATRESULTS, G_PLAYER = 0x668108, 0x668104, 0x668194, 0x667fcc

# TCharacter attack bookkeeping (COMBAT_ATTACK_CHOICE.md §2.4). Defaults as
# ClearChar 0x4c18a0 leaves them (the port's ClearChar is the same).
ATTACKSTATE = [('nextattack', 0x120, 1), ('magictimer', 0x124, 1), ('requestbits', 0x128, 0),
               ('attackcount', 0x12c, 0), ('lastattackticks', 0x164, 0), ('lasthit', 0x168, 0),
               ('chainhits', 0x16c, 0), ('lastbutton', 0x28c, -1), ('buttonrepeat', 0x290, 0),
               ('flashticks', 0x224, 0), ('autocombat', 0xe8, 1), ('movevert', 0xb8, None),
               ('snapticks', 0x220, -1), ('charflags', 0x110, None)]
O_LASTATTACK = 0x160
O_FLAGS, OF_ICED_PARALIZE = 0x8, 0x2800000
P_FRAGS = 0x650                                 # TPlayer kills / deaths, 4 ints
# TPlayer's modified stat copy (PLAYER_STATS.md §1): count +0x34c (short),
# {id, value} pairs at +0x350. Resist reads entry type + 6.
P_MODCOUNT, P_MODSTATS, RESIST_FIRST = 0x34c, 0x350, 6
# TPlayer's equipment, 11 slots from +0x2a0; ArmorValue (0x519850) adds the
# item's slot 0x1f0 (TArmor's Protection) for each of class 2 but the ammo's.
P_EQUIPMENT, ITEM_PROTECTION = 0x2a0, 0x1f0


class MeleeFixture:
    def __init__(self, executable):
        started = time.perf_counter()
        self.vm, self.sha = start(executable)
        self.world = CombatWorld(self.vm)
        b = self.world.boundaries
        b.add(FIND_CHARACTERS, 'FindCharacters', 0x18, self._find_characters)
        b.add(FIND_CLEAR_PATH, 'FindClearPath', 0x14, self._find_clear_path)
        b.add(EFFECT_COMBAT_FLASH, 'EffectCombatFlash', 0, self._combat_flash)
        b.add(FIND_OBJECTS_IN_RANGE, 'FindObjectsInRange', 0x20, self._find_objects)
        b.add(CAN_SEE, 'CanSeeCharacter', 8, self._can_see)
        b.add(GET_INSTANCE, 'GetInstance', 8, self._get_instance)
        b.add(LINE_OF_SIGHT, 'LineOfSight', 0x14, self._helper_sight)
        b.add(SPELL_FIND, 'SpellFind', 4, self._spell_find)
        b.add(CAST, 'Cast', 0x10, self._cast)
        b.add(PLAYER_WEAPONTYPE, 'WeaponType', 0, lambda a, c: self._weapon(c, 'type', 'WeaponType'))
        b.add(PLAYER_WEAPONDAMAGE, 'WeaponDamage', 0, lambda a, c: self._weapon(c, 'damage', 'WeaponDamage'))
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
        b.add(IS_ENEMY, 'IsEnemy', 4, self._is_enemy)
        b.add(BEGIN_FIGHTING, 'BeginFighting', 8, self._begin_fighting)
        b.add(EFFECT_BURST, 'EffectBurst', 8, self._effect_burst)
        b.add(SOUND_FIND, 'SoundFind', 4, self._sound_find)
        b.add(TEXT_FIND, 'TextFind', 4, lambda a, c: -1)
        b.add(TEXT_GET, 'TextGet', 4, self._text_get)
        b.add(TEXTBAR_PRINT, 'TextBar', 0, self._textbar)
        for kind, (address, pop) in EXP.items():
            self._conditional(address, pop, lambda a, c, kind=kind: self._exp(kind, a, c),
                              lambda: self.case.get('seam_exp', True))
        # The tick's (C6, the scenario kata's) and the experience calls' (C7).
        ticking = self._ticking
        b.add(SPELL_LIST_PULSE, 'SpellListPulse', 0, lambda a, c: 0)
        b.add(DEATH_SCREEN, 'DeathScreen', 0, self._death_screen)
        self._conditional(UPDATE_ACTION, 4, self._update_action,
                          lambda: self.case.get('call') == 'pulse' and self.case.get('seam_update', True))
        self._conditional(AI, 0, self._ai, lambda: self.case.get('call') == 'pulse' and self.case.get('seam_ai', True))
        self._conditional(REFRESH_STATS, 0, self._refresh_stats, ticking)
        # A case's script (`script`): empty, as the port's -- no trigger matches.
        self._conditional(SCRIPT_TRIGGER, 0x1c, lambda a, c: 0, ticking)
        boss = self._boss_world
        self._conditional(FIND_OBJ_TYPE, 8, self._find_obj_type, lambda: ticking() or boss())
        self._conditional(NEW_OBJECT, 8, self._new_object, lambda: ticking() or boss())
        # The bosses' (C3c).
        self._conditional(PANE_ITER, 0x14, self._pane_objects, boss)
        self._conditional(END_FIGHTING, 0, self._end_fighting, boss)
        self._conditional(SET_CONTROL, 4, self._set_control, boss)
        self._conditional(TINT, 0, lambda a, c: self._tint('Tint', c), boss)
        self._conditional(UNTINT, 0, lambda a, c: self._tint('Untint', c), boss)
        self._conditional(CREATE_ANIMATOR, 0, self._create_animator, boss)
        self.prop_vtable = self.world._stub_table(0x80, {}, lambda ecx: ecx, 'prop')
        self.vm.put_u32(self.prop_vtable + 0x18, SET_STATE)
        self.vm.put_u32(self.prop_vtable + 0x40, RESET_FLAGS)
        for seam, (address, key) in PLAYER_MAX.items():
            b.add(address, seam, 0, lambda a, c, seam=seam, key=key: self._player_max(seam, key, c))
        self.made_vtable = self.vm.allocate(0x400)
        self.vm.put_u32(self.made_vtable + 0x200, b.add_stub('blood.SetParams', 0x18, self._blood_params))
        # Worn items: one vtable, whose Protection answers by the item.
        self.item_vtable = self.vm.allocate(0x400)
        self.vm.put_u32(self.item_vtable + ITEM_PROTECTION,
                        b.add_stub('item.Protection', 0, lambda a, c: self.protections.get(c, 0) & 0xffffffff))
        self.protections = {}
        self._conditional(DAMAGE, 0x14, self._damage, lambda: self.case.get('seam_damage', True))
        self._conditional(BLOCK, 4, self._block, lambda: self.case.get('seam_block', True))
        for address, name in WANDER_SEAMS:
            self._conditional(address, 0, (lambda n: lambda a, c: self._wander_seam(n, c))(name),
                              lambda: self.case.get('seam_wander', False))
        # The helpers' vtable: IsInInventory as original (a map search asks
        # it), every other slot one stub that fails the case.
        self.helper_vtable = self.vm.allocate(0x400)
        unexpected = b.add_stub('helper.unexpected', 0, self._helper_unexpected)
        for slot in range(0, 0x400, 4):
            self.vm.put_u32(self.helper_vtable + slot, unexpected)
        self.vm.put_u32(self.helper_vtable + SLOT_IS_IN_INVENTORY, IS_IN_INVENTORY)
        self._emulator_fixes()
        self._coverage()
        self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}
        self.helpers = []
        self.types = []
        self.made = []
        self.props = {}            # the bosses' map objects: name -> address
        self.instances = {}        # GetInstance's answers in a boss case: id -> address
        self.made_used = 0

    def _coverage(self):
        """With MELEE_COVERAGE=<dir>: the code reached in the melee functions
        (each block run, as [start, size]), written to <dir>/<pid>.json when the process ends
        (tools/retail_ab/melee_coverage.py reads them)."""
        import atexit
        import os
        out = os.environ.get('MELEE_COVERAGE')
        if not out:
            return
        from unicorn import UC_HOOK_BLOCK
        reached = set()
        lo, hi = min(r[0] for r in COVERED.values()), max(r[1] for r in COVERED.values())
        self.vm.uc.hook_add(UC_HOOK_BLOCK, lambda uc, address, size, user: reached.add((address, size)),
                            begin=lo, end=hi)

        def write():
            Path(out).mkdir(parents=True, exist_ok=True)
            (Path(out) / f'{os.getpid()}.json').write_text(json.dumps(sorted(reached)))
        atexit.register(write)

    # -- seams --------------------------------------------------------------
    def _obj(self, name):
        return self.world.by_name[name] if name else 0

    def _ticking(self):
        """The calls that run the character's tick (its seams answered)."""
        return self.case.get('call') in TICKING_CALLS

    def _find_characters(self, args, ecx):
        if self.case.get('perception'):
            return Boundaries.ORIGINAL
        found = self._obj(self.case.get('found'))
        query = dict(max=s32(args[1]), range=s32(args[2]), angle=s32(args[3]),
                     anglerange=s32(args[4]), flags=s32(args[5]))
        n = 1 if found and s32(args[1]) > 0 else 0
        if s32(args[1]) > 0:
            self.vm.put_u32(args[0], found)
        self.world.seams.append(dict(seam='FindCharacters', who=self.world._name(ecx), **query, result=n))
        return n

    def _find_clear_path(self, args, ecx):
        if not self.case.get('seam_path', True):
            return Boundaries.ORIGINAL
        point = list(struct.unpack('<3i', self.vm.uc.mem_read(args[1], 12)))
        blocked = int(bool(self.case.get('blocked', False)))
        blocker = self.case.get('blocker') if blocked else None
        if args[4]:
            self.vm.put_u32(args[4], self._obj(blocker))
        self.world.seams.append(dict(seam='FindClearPath', who=self.world._name(ecx), to=point,
                                     blocker=blocker, result=blocked))
        return blocked

    def _block(self, args, ecx):
        result = int(self.case.get('block_result', 1))
        self.world.seams.append(dict(seam='Block', who=self.world._name(ecx), frames=s32(args[0]), result=result))
        return result

    def _can_see(self, args, ecx):
        """`sees`: yes or no, or the names seen."""
        if self.case.get('perception'):
            return Boundaries.ORIGINAL
        sees = self.case.get('sees', True)
        them = self.world._name(args[0])
        seen = int(them in sees) if isinstance(sees, list) else int(bool(sees))
        self.world.seams.append(dict(seam='CanSeeCharacter', who=self.world._name(ecx), target=them, result=seen))
        return seen

    def _wander_seam(self, name, ecx):
        self.world.seams.append(dict(seam=name, who=self.world._name(ecx), result=1))
        return 1

    @staticmethod
    def _helper_unexpected(args, ecx):
        raise RuntimeError('a helper was asked a virtual it does not model')

    def _find_objects(self, args, ecx):
        if self.case.get('real_map'):
            return Boundaries.ORIGINAL
        centre = list(struct.unpack('<3i', self.vm.uc.mem_read(args[0], 12)))
        n = len(self.helpers)
        for i in range(min(n, s32(args[6]))):
            self.vm.put_u32(args[2] + 4 * i, HELPER_INDEX + i)
        self.world.seams.append(dict(seam='FindObjectsInRange', who=self.world._name(self._ai_self), centre=centre,
                                     max=s32(args[6]), result=n))
        return n

    def _get_instance(self, args, ecx):
        if self._boss_world():
            return self.instances.get(s32(args[0]), 0)
        if 0 <= s32(args[0]) - BLOOD_INDEX < len(self.made):
            return self.made[s32(args[0]) - BLOOD_INDEX]
        i = s32(args[0]) - HELPER_INDEX
        if not 0 <= i < len(self.helpers):
            return Boundaries.ORIGINAL
        return self.helpers[i][1]

    def _helper_sight(self, args, ecx):
        if self.case.get('real_map'):
            return Boundaries.ORIGINAL          # over the case's `ground`
        if not self.helpers:
            if self.case.get('perception') and 'ground' not in self.case:
                return self._eyes_sight(args)
            return Boundaries.ORIGINAL
        fx, fy, _ = struct.unpack('<3i', self.vm.uc.mem_read(args[0], 12))
        to = struct.unpack('<3i', self.vm.uc.mem_read(args[1], 12))
        a = next((name for name, obj in self.helpers
                  if struct.unpack('<2i', self.vm.uc.mem_read(obj + 0x10, 8)) == (fx, fy)), '?')
        b = next((name for name, obj in self.world.by_name.items()
                  if struct.unpack('<3i', self.vm.uc.mem_read(obj + 0x10, 12)) == to), '?')
        clear = int([a, b] not in self.case.get('walls', []))
        self.world.seams.append(dict(seam='LineOfSight', **{'from': a}, to=b, result=clear))
        return clear

    def _eyes_sight(self, args):
        """CanSeeCharacter's line of sight between two characters' eyes
        (LIGHTINGCHARHEIGHT above their positions), as combat_call.py
        answers it: clear unless the case's `walls` name the pair."""
        def eyes_of(point):
            x, y, z = struct.unpack('<3i', self.vm.uc.mem_read(point, 12))
            for name, obj in self.world.by_name.items():
                px, py, pz = struct.unpack('<3i', self.vm.uc.mem_read(obj + 0x10, 12))
                if (px, py, pz + EYE_HEIGHT) == (x, y, z):
                    return name
            return '?'
        line = [eyes_of(args[0]), eyes_of(args[1])]
        clear = int(line not in self.case.get('walls', []))
        self.world.seams.append(dict(seam='LineOfSight', **{'from': line[0]}, to=line[1], result=clear))
        return clear

    def _make_helpers(self, case):
        """The case's helpers: class 0xf, a type record whose first word
        names the type, the position, the level, the map index HELPER_INDEX
        + k, no inventory slot; with `real_map`, what a map search walks."""
        vm = self.vm
        self.helpers = []
        for k, w in enumerate(case.get('waypoints', [])):
            obj = vm.allocate(0x200)
            name = vm.allocate(32)
            vm.write(name, w.get('type', 'Waypoint').encode(TEXT) + b'\0')
            info = vm.allocate(16)
            vm.put_u32(info, name)
            vm.put_u32(obj, self.helper_vtable)
            vm.write(obj + 4, struct.pack('<h', 0xf))
            vm.write(obj + 0xe, struct.pack('<h', w.get('level', 0)))
            vm.write(obj + 0x10, struct.pack('<3i', *w['pos']))
            vm.put_u32(obj + 0x40, HELPER_INDEX + k)
            vm.put_u32(obj + 0x4c, info)
            vm.write(obj + 0x7c, struct.pack('<h', -1))
            self.helpers.append((w['name'], obj))
        if case.get('real_map'):
            self.world.set_map_objects((name, obj, w.get('level', 0))
                                       for (name, obj), w in zip(self.helpers, case.get('waypoints', [])))

    def _combat_flash(self, args, ecx):
        self.world.seams.append(dict(seam='EffectCombatFlash', who=self.world._name(ecx)))
        return 0

    def _spell_find(self, args, ecx):
        name = self.vm.string(args[0])
        if name.lower() not in [s.lower() for s in self.case.get('spells', [])]:
            self.world.seams.append(dict(seam='CastByName', spell=name, result=0))
            return 0
        spell = self.vm.allocate(0x80)
        self.vm.write(spell + 0x24, name.encode(TEXT) + b'\0')
        return spell

    def _cast(self, args, ecx):
        name = self.vm.string(args[0])
        n = s32(args[2])
        targets = [self.world._name(self.vm.u32(args[1] + 4 * i)) for i in range(max(0, n))]
        source = list(struct.unpack('<3i', self.vm.uc.mem_read(args[3], 12))) if args[3] else None
        result = int(self.case.get('cast_result', 1))
        self.world.seams.append(dict(seam='CastByName', spell=name, targets=targets, source=source, result=result))
        return result

    def _weapon(self, ecx, key, seam):
        spec = self.specs[self.world._name(ecx)]
        if 'weapon' not in spec:
            raise KeyError(f"{spec['name']}: a player needs `weapon` in the case")
        value = spec['weapon'][key]
        if not self.world.quiet:
            self.world.seams.append(dict(seam=seam, who=spec['name'], result=value))
        return value

    def _set_player_state(self, args, ecx):
        self.vm.put_u32(ecx + 0x36c, args[0])
        self.world.seams.append(dict(seam='SetPlayerState', who=self.world._name(ecx), value=args[0]))
        return 0

    def _is_enemy(self, args, ecx):
        """`friends`: [who, other] pairs where who doesn't count other an enemy."""
        if self.case.get('perception'):
            return Boundaries.ORIGINAL
        who, other = self.world._name(ecx), self.world._name(args[0])
        result = 0 if [who, other] in self.case.get('friends', []) else 1
        self.world.seams.append(dict(seam='IsEnemy', who=who, other=other, result=result))
        return result

    def _begin_fighting(self, args, ecx):
        if not self.case.get('seam_begin', True):
            return Boundaries.ORIGINAL
        self.world.seams.append(dict(seam='BeginFighting', who=self.world._name(ecx),
                                     target=self.world._name(args[0]) if args[0] else None, action=s32(args[1]),
                                     result=1))
        return 1

    def _effect_burst(self, args, ecx):
        self.world.seams.append(dict(seam='EffectBurst', who=self.world._name(ecx), name=self.vm.string(args[0]),
                                     height=s32(args[1])))
        return 0

    def _sound_find(self, args, ecx):
        """No sound by that name: nothing plays."""
        self.world.seams.append(dict(seam='Sound', name=self.vm.string(args[0])))
        return -1

    def _text_get(self, args, ecx):
        """The dialog list is empty: a tag's line is "[tag]", as the port's
        unloaded list answers."""
        line = '[' + self.vm.string(args[0]) + ']'
        address = self.vm.allocate(len(line) + 1)
        self.vm.write(address, line.encode(TEXT) + b'\0')
        return address

    def _textbar(self, args, ecx):
        self.world.seams.append(dict(seam='TextBar', text=self.vm.string(args[1])))
        return 0

    def _exp(self, kind, args, ecx):
        skill = s32(args[0]) if kind == 'skill' else -1
        victim = args[1] if kind == 'skill' else args[0]
        self.world.seams.append(dict(seam='Exp', who=self.world._name(ecx), kind=kind, skill=skill,
                                     victim=self.world._name(victim)))
        return 0

    def _update_action(self, args, ecx):
        self.world.seams.append(dict(seam='UpdateAction', who=self.world._name(ecx), bits=s32(args[0])))
        return 0

    def _ai(self, args, ecx):
        self.world.seams.append(dict(seam='AI', who=self.world._name(ecx)))
        return 0

    def _death_screen(self, args, ecx):
        self.world.seams.append(dict(seam='DeathScreen'))
        return 0

    def _refresh_stats(self, args, ecx):
        self.world.seams.append(dict(seam='RefreshStats', who=self.world._name(ecx)))
        return 0

    def _player_max(self, seam, key, ecx):
        """A player's maximum, when the case gives it; else the original."""
        spec = self.specs[self.world._name(ecx)]
        if key not in spec:
            return Boundaries.ORIGINAL
        if not self.world.quiet:
            self.world.seams.append(dict(seam=seam, who=spec['name'], result=spec[key]))
        return spec[key]

    def _find_obj_type(self, args, ecx):
        self.types.append(self.vm.string(args[0]))
        return len(self.types) - 1

    def _new_object(self, args, ecx):
        """The def recorded (as the spells katas record it); a stand-in made
        at the next map index, its SetParams recorded."""
        vm, d = self.vm, args[0]
        objclass, objtype = struct.unpack('<hh', vm.uc.mem_read(d, 4))
        state, level = struct.unpack('<HH', vm.uc.mem_read(d + 8, 4))
        self.world.seams.append(dict(seam='NewObject', **{'class': objclass},
                                     type=self.types[objtype].lower() if 0 <= objtype < len(self.types) else objtype,
                                     flags=vm.u32(d + 4), state=state, level=level,
                                     pos=list(struct.unpack('<3i', vm.uc.mem_read(d + 0x0c, 12))),
                                     vel=list(struct.unpack('<3i', vm.uc.mem_read(d + 0x18, 12))),
                                     facing=vm.uc.mem_read(d + 0x32, 1)[0],
                                     imagery=struct.unpack('<i', struct.pack('<I', args[1]))[0]))
        if self._boss_world():
            return self._hand_out()
        made = vm.allocate(0x100)
        vm.put_u32(made, self.made_vtable)
        self.made.append(made)
        self.world.objects[made] = f'made {len(self.made)}'
        return BLOOD_INDEX + len(self.made) - 1

    # -- the bosses (C3c) ------------------------------------------------------
    def _boss_world(self):
        """A boss case: `per-monster`, or another call (`ai`) on the case's
        `bossworld`."""
        return self.case.get('call') == 'per-monster' or bool(self.case.get('bossworld'))

    def _hand_out(self):
        """NewObject in a boss case: the case's next `made` character, now in
        the map (GetInstance finds it); none left, an id no object has."""
        made = self.case.get('made', [])
        n = self.made_used
        self.made_used += 1
        if n >= len(made):
            return MADE_INDEX + n
        ident = self.specs[made[n]]['id']
        self.instances[ident] = self.world.by_name[made[n]]
        return ident

    def _object(self, name):
        return self.props.get(name) or self.world.by_name[name]

    def _pane_objects(self, args, ecx):
        """The walk of the pane's sectors (0x44cf80 with no rect): the case's
        `paneobjects`, in order, through the shared Next 0x44d080."""
        self.world.seams.append(dict(seam='PaneObjects', who=self.world._name(self._ai_self)))
        self.world.iters[ecx] = [self._object(n) for n in self.case.get('paneobjects', [])]
        self.world._iter_next(args, ecx)
        return ecx

    def _end_fighting(self, args, ecx):
        self.world.seams.append(dict(seam='EndFighting', who=self.world._name(ecx)))
        return 1

    def _set_control(self, args, ecx):
        self.world.seams.append(dict(seam='SetControl', on=s32(args[0])))
        return 0

    def _tint(self, kind, ecx):
        self.world.seams.append(dict(seam=kind, who=self.world._name(ecx)))
        return 0

    def _create_animator(self, args, ecx):
        """A stand-in animator at +0x58 (its calls recorded)."""
        self.world.seams.append(dict(seam='CreateAnimator', who=self.world._name(ecx)))
        self.vm.put_u32(ecx + 0x58, self.world.new_animator(ecx))
        return 1

    def _make_props(self, case):
        """The case's `props`: objects of the map that aren't characters (the
        ring wall, the table): class, flags, position, name, id, type, no
        sector; SetState through the shared seam, ResetFlags as original."""
        vm, world = self.vm, self.world
        self.props = {}
        for p in case.get('props', []):
            obj = vm.allocate(0x100)
            vm.put_u32(obj, self.prop_vtable)
            vm.write(obj + 4, struct.pack('<h', p.get('class', 8)))
            vm.put_u32(obj + 8, p.get('flags', 0))
            vm.write(obj + 0x10, struct.pack('<3i', *p.get('pos', [0, 0, 0])))
            name = vm.allocate(len(p['name']) + 1)
            vm.write(name, p['name'].encode(TEXT) + b'\0')
            vm.put_u32(obj + 0x38, name)
            vm.put_u32(obj + 0x40, p.get('id', 0))
            typename = vm.allocate(len(p['type']) + 1)
            vm.write(typename, p['type'].encode(TEXT) + b'\0')
            info = vm.allocate(16)
            vm.put_u32(info, typename)
            vm.put_u32(obj + 0x4c, info)
            world.objects[obj] = p['name']
            world.states[obj] = [dict(name=n, frames=10, aniflags=0) for n in p.get('states', [])]
            self.props[p['name']] = obj

    def _boss_state(self, obj, spec):
        """A character's `boss`: its state buffer at +0x284 by BOSS_FIELDS
        (the kind is `monsterkind`), Baez's crowd into the global."""
        vm, world = self.vm, self.world
        boss = spec.get('boss')
        if boss is None:
            return
        kind = spec.get('monsterkind', 0)
        state = vm.call(MALLOC, (BOSS_SIZE[kind],))
        vm.write(state, bytes(BOSS_SIZE[kind]))
        for name, at in BOSS_FIELDS[kind]:
            if name not in boss:
                continue
            v = boss[name]
            if isinstance(at, int):
                vm.put_u32(state + at, v & 0xffffffff)
            elif at[0] == 'pt':
                vm.write(state + at[1], struct.pack('<3i', *v))
            elif at[0] == 'ints':
                vm.write(state + at[1], struct.pack(f'<{at[2]}i', *v))
            else:                               # Jhaga's points: x[5], y[5], z[5]
                for i, (x, y, z) in enumerate(v):
                    for axis, value in enumerate((x, y, z)):
                        vm.put_u32(state + at[1] + 0x14 * axis + 4 * i, value & 0xffffffff)
        vm.put_u32(obj + O_BOSS, state)
        if kind == 1:
            for i, name in enumerate(boss.get('watchers', [None] * WATCHERS)):
                vm.put_u32(G_WATCHERS + 4 * i, world.by_name[name] if name else 0)

    def _boss_dump(self, obj):
        """The boss as compared: its kind, its state by name (Baez's crowd
        from the global), and where a MoveTo heads."""
        vm, world = self.vm, self.world
        kind = s32(vm.u32(obj + O_MONSTERKIND))
        out = dict(kind=kind, movetopos=vm.u32(obj + 0xec),
                   movepos=list(struct.unpack('<3i', vm.uc.mem_read(obj + 0xf0, 12))))
        state = vm.u32(obj + O_BOSS)
        if kind not in BOSS_FIELDS or not state:
            return out
        fields = {}
        for name, at in BOSS_FIELDS[kind]:
            if isinstance(at, int):
                fields[name] = s32(vm.u32(state + at))
            elif at[0] == 'pt':
                fields[name] = list(struct.unpack('<3i', vm.uc.mem_read(state + at[1], 12)))
            elif at[0] == 'ints':
                fields[name] = list(struct.unpack(f'<{at[2]}i', vm.uc.mem_read(state + at[1], 4 * at[2])))
            else:
                fields[name] = [[s32(vm.u32(state + at[1] + 0x14 * axis + 4 * i)) for axis in range(3)]
                                for i in range(5)]
        if kind == 1:
            fields['watchers'] = [world._name(vm.u32(G_WATCHERS + 4 * i)) if vm.u32(G_WATCHERS + 4 * i) else None
                                  for i in range(WATCHERS)]
        out['state'] = fields
        return out

    def _blood_params(self, args, ecx):
        self.world.seams.append(dict(seam='blood.SetParams', who=self.world._name(ecx),
                                     args=[s32(a) for a in args[:6]], result=0))
        return 0

    def _damage(self, args, ecx):
        """Damage (the C2 kata runs it): recorded, the hit block by meaning."""
        ab = args[3]
        block = None
        if ab:
            vm = self.vm
            block = dict(action=s32(vm.u32(ab + AB['action'])), name=vm.string(ab + AB['name']),
                         wait=s32(vm.u32(ab + AB['wait'])), damage=s32(vm.u32(ab + AB['damage'])),
                         obj=self.world._name(vm.u32(ab + AB['obj'])) if vm.u32(ab + AB['obj']) else None,
                         attack=self._attack_name(vm.u32(ab + AB['attack'])), impact=self._impact_index(ab),
                         flags=flag_names(vm.u32(ab + AB['flags'])))
        self.world.seams.append(dict(seam='Damage', who=self.world._name(ecx), damage=s32(args[0]),
                                     type=s32(args[1]), mod=s32(args[2]), block=block,
                                     attacker=self.world._name(args[4]) if args[4] else None))
        return 0

    def _emulator_fixes(self):
        """Where the emulator runs retail's code wrong. ResolveAttack's block
        sound switch (0x4c727c) is `cmp eax, 6; rep movsb; ja default`: the
        emulator loses the cmp's flags across the rep movsb (seen with the
        attacker's weapon type 0, eax -1, the ja not taken and the table
        read out of range). The ja is taken here as the hardware takes it."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP

        def ja(uc, at, size, user):
            if uc.reg_read(UC_X86_REG_EAX) > 6:
                uc.reg_write(UC_X86_REG_EIP, 0x4c731e)
        self.vm.uc.hook_add(UC_HOOK_CODE, ja, begin=0x4c7281, end=0x4c7281)

    def _conditional(self, address, pop, handler, active):
        """A seam only while active() holds; otherwise the original runs."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
        vm = self.vm

        def enter(uc, at, size, user):
            if not active():
                return
            sp = uc.reg_read(UC_X86_REG_ESP)
            args = struct.unpack('<8I', uc.mem_read(sp + 4, 32))
            try:
                result = handler(args, uc.reg_read(UC_X86_REG_ECX))
            except Exception as error:
                vm.error = error
                uc.emu_stop()
                return
            uc.reg_write(UC_X86_REG_EAX, (result or 0) & 0xffffffff)
            uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
        vm.uc.hook_add(UC_HOOK_CODE, enter, begin=address, end=address)

    # -- the world ------------------------------------------------------------
    def _impact(self, address, imp):
        write_text(self.vm, address, 32, imp.get('name', ''))
        write_fields(self.vm, address, IMPACT_FIELDS, imp)

    def _chardata(self, obj, spec):
        """The case's whole character data over what CombatWorld wrote:
        every field the dump names, the CHARIMPACTs, the attack table (a
        pointer array: count +0xcc, items +0xdc, the empty record +0xe0)."""
        vm = self.vm
        cd = vm.u32(obj + 0xfc)
        data = spec.get('chardata', {})
        write_fields(self.vm, cd, CHAR_FIELDS, data)
        if 'impacts' in data:
            vm.put_u32(cd + C_NUMIMPACTS, len(data['impacts']))
            for i, imp in enumerate(data['impacts']):
                self._impact(cd + C_IMPACTS + IMPACT_SIZE * i, imp)
        attacks = data.get('attacks', [])
        items = vm.allocate(4 * max(1, len(attacks)))
        empty = vm.allocate(ATTACK_SIZE + 0x60)
        vm.put_u32(cd + C_ATTACKS, len(attacks))
        vm.put_u32(cd + C_ATTACKS + 0x10, items)
        vm.put_u32(cd + C_ATTACKS + 0x14, empty)
        for i, ad in enumerate(attacks):
            # A little zeroed room after the record: IsValidAttack reads the
            # impact one past the last (0x4d1d35).
            rec = vm.allocate(ATTACK_SIZE + 0x60)
            vm.put_u32(items + 4 * i, rec)
            write_text(self.vm, rec, 32, ad['name'])
            write_fields(self.vm, rec, ATTACK_HEAD, ad)
            if ad['flags'] & CA_MAGICATTACK:
                write_fields(self.vm, rec, ATTACK_MAGIC, ad)
            else:
                write_fields(self.vm, rec, ATTACK_BODY, ad)
                vm.put_u32(rec + A_NUMIMPACTS, len(ad.get('impacts', [])))
                for k, imp in enumerate(ad.get('impacts', [])):
                    self._impact(rec + A_IMPACTS + IMPACT_SIZE * k, imp)
            self.records[rec] = (spec['name'], i)

    def _attackstate(self, obj, spec):
        vm = self.vm
        state = spec.get('attackstate', {})
        for name, off, default in ATTACKSTATE:
            if default is not None:             # (None: dumped, set elsewhere)
                vm.put_u32(obj + off, int(state.get(name, default)) & 0xffffffff)
        last = state.get('lastattack')
        cd = vm.u32(obj + 0xfc)
        vm.put_u32(obj + O_LASTATTACK, vm.u32(vm.u32(cd + C_ATTACKS + 0x10) + 4 * last) if last is not None else 0)

    def _apply_block_fields(self, ab, obj, b):
        """What a block of the case carries beyond the shared spec: its
        attack (an index into the table of `obj`, or of the character
        `attack_of` names), impact (of that attack, or of attack
        `impact_attack` of `impact_of`), damage, to-hit and roll."""
        vm = self.vm

        def record(owner, index):
            ocd = vm.u32((self.world.by_name[owner] if owner else obj) + 0xfc)
            return vm.u32(vm.u32(ocd + C_ATTACKS + 0x10) + 4 * index)
        if 'attack' in b:
            vm.put_u32(ab + AB['attack'], record(b.get('attack_of'), b['attack']))
        if 'impact' in b:
            rec = record(b.get('impact_of', b.get('attack_of')), b.get('impact_attack', b.get('attack')))
            vm.put_u32(ab + AB['impact'], rec + A_IMPACTS + IMPACT_SIZE * b['impact'])
        for key, off2 in (('damage', AB['damage']), ('tohit', 0x54), ('roll', 0x58)):
            if key in b:
                vm.put_u32(ab + off2, b[key] & 0xffffffff)

    def _helper_or_char(self, name):
        return next((o for n, o in self.helpers if n == name), None) or self._obj(name)

    def _helper_or_char_name(self, address):
        if not address:
            return None
        return next((n for n, o in self.helpers if o == address), None) or self.world._name(address)

    def _aistate(self, obj, a):
        vm = self.vm
        vm.put_u32(obj + O_LOOKAT, self._helper_or_char(a.get('lookat')))
        vm.write(obj + O_LASTPOS, struct.pack('<3i', *a.get('lastpos', [0, 0, 0])))
        vm.put_u32(obj + O_WAYPOINTTICKS, a.get('waypointticks', 0) & 0xffffffff)
        vm.put_u32(obj + O_LASTWAYPOINT, self._helper_or_char(a.get('lastwaypoint')))
        vm.put_u32(obj + O_WANDERIDLE, a.get('wanderidle', 0) & 0xffffffff)
        vm.put_u32(obj + O_RUNTICKS, a.get('runticks', 0) & 0xffffffff)

    def _ai_dump(self, obj):
        vm = self.vm
        return dict(lookat=self._helper_or_char_name(vm.u32(obj + O_LOOKAT)),
                    lastpos=list(struct.unpack('<3i', vm.uc.mem_read(obj + O_LASTPOS, 12))),
                    waypointticks=s32(vm.u32(obj + O_WAYPOINTTICKS)), monsterkind=s32(vm.u32(obj + O_MONSTERKIND)),
                    lastwaypoint=self._helper_or_char_name(vm.u32(obj + O_LASTWAYPOINT)),
                    wanderidle=s32(vm.u32(obj + O_WANDERIDLE)), runticks=s32(vm.u32(obj + O_RUNTICKS)))

    def _block_fields(self, obj, spec):
        for role, off in (('root', 0xe0), ('doing', 0xd8), ('desired', 0xdc)):
            if isinstance(spec.get(role), dict):
                self._apply_block_fields(self.vm.u32(obj + off), obj, spec[role])

    def _resists(self, obj, spec):
        """A player's modified stat copy, as far as Resist reads it."""
        vm = self.vm
        resists = spec.get('resists', [0] * 10)
        count = RESIST_FIRST + 10
        pairs = vm.allocate(8 * count)
        for i in range(count):
            value = resists[i - RESIST_FIRST] if i >= RESIST_FIRST else 0
            vm.write(pairs + 8 * i, struct.pack('<ii', i, value))
        vm.write(obj + P_MODCOUNT, struct.pack('<h', count))
        vm.put_u32(obj + P_MODSTATS, pairs)

    def _equipment(self, obj, spec):
        """A player's worn items (`equipment`: {slot, class, protection}),
        as ArmorValue reads them: the class at +0x04, Protection through the
        item vtable."""
        vm = self.vm
        self.protections = {}
        for item in spec.get('equipment', []):
            o = vm.allocate(0x100)
            vm.put_u32(o, self.item_vtable)
            vm.write(o + 4, struct.pack('<h', item['class']))
            self.protections[o] = item['protection']
            vm.put_u32(obj + P_EQUIPMENT + 4 * item['slot'], o)

    def _rules(self, rules):
        vm = self.vm
        for name, off in RULES_TOHIT:
            vm.put_u32(RULES + off, int(rules.get(name, 0)) & 0xffffffff)
        for name, off in RULES_RECOVERY:
            if name in rules:
                vm.put_u32(RULES + off, int(rules[name]) & 0xffffffff)
        for key, off in (('exp', R_EXP), ('skillexp', R_SKILLEXP)):
            if key in rules:
                vm.write(RULES + off, struct.pack(f'<{len(rules[key])}i', *rules[key]))
        for i, (key, pct) in enumerate(rules.get('tohitdamage', [[0, 0]] * 5)):
            vm.write(RULES + R_TOHITDAMAGE + 8 * i, struct.pack('<ii', key, pct))
        tables = rules.get('statlevels')
        if tables:
            ptrs = vm.allocate(4 * len(tables))
            for t, row in enumerate(tables):
                table = vm.allocate(4 * len(row))
                vm.write(table, struct.pack(f'<{len(row)}i', *row))
                vm.put_u32(ptrs + 4 * t, table)
            vm.put_u32(RULES + R_STATLEVELS, ptrs)
        else:
            vm.put_u32(RULES + R_STATLEVELS, 0)

    def _pulse_world(self, case):
        """The tick's globals: the screen and its frames, the game type, no
        network game, the AutoBeginCombat option, no bounding-rect update."""
        vm, g = self.vm, case.get('globals', {})
        self.screen = vm.allocate(0x100)
        vm.put_u32(G_SCREEN, self.screen)
        vm.put_u32(self.screen + SCREEN_FRAMES, int(g.get('screenframe', 0)))
        vm.put_u32(G_PLAYSCREEN + PS_GAMETIME, int(g.get('gametime', 0)) & 0xffffffff)
        gametype = vm.allocate(0x40)
        vm.write(gametype, bytes([8 if g.get('pvp') else 0]))
        table = vm.allocate(4)
        vm.put_u32(table, gametype)
        vm.put_u32(G_GAMETYPES, table)
        vm.put_u32(G_GAMETYPE, 0)
        for address in (G_NETGAME, G_NETHOST, G_UPDATING_RECT, G_PS_5D4):
            vm.put_u32(address, 0)
        vm.put_u32(G_AUTOBEGIN, int(g.get('autobegincombat', 1)))

    def _pulse_state(self, obj, spec):
        vm = self.vm
        st = spec.get('pulsestate')
        if st is not None:
            fields = PULSESTATE + (PLAYER_PULSESTATE if spec.get('class', 12) == 11 else [])
            for name, off in fields:
                vm.put_u32(obj + off, int(st[name]) & 0xffffffff)
        if spec.get('noai'):
            vm.put_u32(obj + O_FLAGS, vm.u32(obj + O_FLAGS) & ~OF_AI)
        if spec.get('script'):
            vm.put_u32(obj + O_SCRIPT, vm.allocate(0x100))  # its Trigger answered below
        if spec.get('class', 12) == 11:
            base = spec.get('basestats', {})
            literal = {n: i for i, n in LITERAL_OBJSTATS.items()}
            values = vm.allocate(4 * NUM_OBJSTATS)
            for name, value in base.items():
                vm.put_u32(values + 4 * literal[name], value & 0xffffffff)
            vm.write(obj + P_BASECOUNT, struct.pack('<h', NUM_OBJSTATS))
            vm.put_u32(obj + P_BASESTATS, values)
            for i, pick in enumerate(spec.get('levelup', [0, 0, 0])):
                vm.put_u32(obj + P_LEVELUP + 4 * i, pick)

    def _pulse_dump(self, obj, spec):
        vm = self.vm
        fields = PULSESTATE + (PLAYER_PULSESTATE if spec.get('class', 12) == 11 else [])
        out = {name: s32(vm.u32(obj + off)) for name, off in fields}
        out['objflags'] = vm.u32(obj + O_FLAGS) & PULSE_OBJFLAGS
        out['charflags'] = vm.u32(obj + 0x110)
        if spec.get('class', 12) == 11:
            out['levelup'] = [s32(vm.u32(obj + P_LEVELUP + 4 * i)) for i in range(3)]
        return out

    def _vitals(self, obj):
        stats = self.world.stats.get(obj, {})
        return {k: stats.get(OBJSTAT_IDS[k][1]) for k in ('health', 'fatigue', 'mana', 'poisoned')}

    # -- dumps ------------------------------------------------------------------
    def _attack_name(self, rec):
        if not rec:
            return None
        owner = self.records.get(rec)
        return f'{owner[0]}#{owner[1]}' if owner else f'{rec:#x}'

    def _impact_index(self, ab):
        """An attack's impact as "<owner>#<attack>/<impact>", a CHARIMPACT as
        "<owner>/<impact>"; the slot after a full list (six) "past-end"."""
        imp = self.vm.u32(ab + AB['impact'])
        if not imp:
            return None
        for rec, (owner, i) in self.records.items():
            k, r = divmod(imp - rec - A_IMPACTS, IMPACT_SIZE)
            if r == 0 and 0 <= k <= 6:
                return 'past-end' if k == 6 else f'{owner}#{i}/{k}'
        for name, obj in self.world.by_name.items():
            k, r = divmod(imp - self.vm.u32(obj + 0xfc) - C_IMPACTS, IMPACT_SIZE)
            if r == 0 and 0 <= k <= 6:
                return 'past-end' if k == 6 else f'{name}/{k}'
        return f'{imp:#x}'

    def _block_extra(self, ab, out):
        if out is None:
            return out
        vm = self.vm
        out.update(attack=self._attack_name(vm.u32(ab + AB['attack'])), impact=self._impact_index(ab),
                   damage=s32(vm.u32(ab + AB['damage'])), tohit=s32(vm.u32(ab + 0x54)),
                   roll=s32(vm.u32(ab + 0x58)))
        return out

    def _dump(self, obj, new_blocks):
        vm, world = self.vm, self.world
        out = world.character_dump(obj, new_blocks)
        for role, off in (('root', 0xe0), ('doing', 0xd8), ('desired', 0xdc)):
            self._block_extra(vm.u32(obj + off), out[role])
        state = {name: s32(vm.u32(obj + off)) for name, off, _ in ATTACKSTATE}
        state['objflags'] = vm.u32(obj + O_FLAGS) & OF_ICED_PARALIZE
        owner = self.records.get(vm.u32(obj + O_LASTATTACK))
        state['lastattack'] = owner[1] if owner and owner[0] == world._name(obj) else None
        if self.specs[world._name(obj)].get('class', 12) == 11:
            state['frags'] = [s32(vm.u32(obj + P_FRAGS + 4 * i)) for i in range(4)]
        out['attackstate'] = state
        out['motion'] = world.motion_dump(obj)
        return out

    # -- a case -------------------------------------------------------------------
    def _prepare(self, case):
        """The world's modes for a case, before its characters are built
        (a subclass's: scenario.py's imagery and animation layer)."""

    def build(self, case):
        """The case's world in guest memory: the globals, rules, characters
        with their data, blocks, attack and AI state, memories and tick
        state; then the seams so far forgotten."""
        vm, world = self.vm, self.world
        vm.restore()
        world.reset()
        self.case = case
        self._prepare(case)
        self.records = {}
        self.specs = {c['name']: c for c in case['chars']}
        world.set_globals(case.get('globals', {}))
        world.set_ground(case)
        self._make_helpers(case)
        vm.put_u32(G_NAHKRANOTH, int(case.get('globals', {}).get('nahkranoth', 0)))
        vm.put_u32(G_ALREADYDEAD, int(case.get('globals', {}).get('alreadydead', 0)))
        vm.put_u32(G_EDITOR, int(case.get('globals', {}).get('editor', 0)))
        vm.put_u32(G_NETOWNER, 1)
        vm.put_u32(G_NOCOMBATRESULTS, int(case.get('globals', {}).get('nocombatresults', 0)))
        vm.put_u32(G_PLAYER, 0)
        self._rules(case.get('rules', {}))
        world.set_rng(case)
        for spec in case['chars']:
            obj = world.new_character(spec)
            self._chardata(obj, spec)
            if spec.get('class', 12) == 11:
                self._resists(obj, spec)
                self._equipment(obj, spec)
        for spec in case['chars']:
            obj = world.by_name[spec['name']]
            world.set_blocks(obj, spec)
            self._block_fields(obj, spec)
            self._attackstate(obj, spec)
            self._aistate(obj, spec.get('aistate', {}))
            world.set_memory(obj, spec)
            self._pulse_state(obj, spec)
        self.types, self.made = [], []
        self.props, self.instances, self.made_used = {}, {}, 0
        if self._boss_world():
            self._make_props(case)
            made = set(case.get('made', []))
            self.instances = {spec['id']: world.by_name[spec['name']] for spec in case['chars']
                              if spec.get('id') and spec['name'] not in made}
            self.instances.update({p['id']: self.props[p['name']] for p in case.get('props', []) if p.get('id')})
            for spec in case['chars']:
                self._boss_state(world.by_name[spec['name']], spec)
        ticking = self._ticking()
        if ticking:
            self._pulse_world(case)
            # A player's class record, as far as the level-up's
            # STATCFG<name> messages read it (0x0051a8ae): its object
            # stats' names (count +0x1c, records of 0x50 bytes at +0x20,
            # the name first). Static init, which registers them in retail
            # (0x00517af0 ...), doesn't run here.
            record = vm.allocate(0x40)
            names = vm.allocate(0x50 * NUM_OBJSTATS)
            for sid, name in ATTRIBUTE_NAMES.items():
                vm.write(names + 0x50 * sid, name.encode(TEXT) + b'\0')
            vm.write(record + 0x1c, struct.pack('<h', NUM_OBJSTATS))
            vm.put_u32(record + 0x20, names)
            for spec in case['chars']:
                if spec.get('class', 12) == 11:
                    vm.put_u32(world.by_name[spec['name']] + O_CLASSREC, record)
        world.quiet = bool(case.get('quiet_reads', ticking))
        world.seams.clear()

    def run(self, case):
        vm, world = self.vm, self.world
        self.build(case)
        ticking = self._ticking()
        me = world.by_name[case['self']]
        self._ai_self = me
        kind = case['call']
        a = case.get('args', {})
        outs = vm.allocate(32)
        result = dict(schema=SCHEMA, side='retail')

        def put_outs(src):
            for i, key in enumerate(('attack', 'impact', 'damage', 'tohit', 'roll')):
                vm.put_u32(outs + 4 * i, int(src.get(key, -1)) & 0xffffffff)

        def read_outs():
            return {key: s32(vm.u32(outs + 4 * i)) for i, key in enumerate(('attack', 'impact', 'damage', 'tohit', 'roll'))}

        o = [outs + 4 * i for i in range(5)]         # attack, impact, damage, tohit, roll
        if kind == 'iva':
            results = []
            for c in case['calls']:
                put_outs(c)
                r = s32(call(vm, IS_VALID_ATTACK,
                             (c['attack'], o[1], o[2], o[3], o[4], c.get('tdist', 10000) & 0xffffffff,
                              c.get('button', -1) & 0xffffffff, c.get('pcnt', -1) & 0xffffffff,
                              c.get('dmgpcnt', 0) & 0xffffffff, c.get('mask', 0) & 0xffffffff,
                              c.get('flags', 0) & 0xffffffff, self._obj(c.get('targ'))), this=me))
                got = read_outs()
                del got['attack']
                results.append(dict(returned=r, **got))
            result['calls'] = results
        elif kind in ('find-button', 'find-pcnt', 'find-interactive'):
            put_outs(a)
            if kind == 'find-button':
                r = call(vm, FIND_BUTTON, (a['button'] & 0xffffffff, a.get('dmgpcnt', 0) & 0xffffffff,
                                           o[0], o[1], o[2], o[3], o[4], int(a.get('isaction', 0)),
                                           self._obj(a.get('targ'))), this=me)
            else:
                r = call(vm, FIND_PCNT if kind == 'find-pcnt' else FIND_INTERACTIVE,
                         (a['pcnt'] & 0xffffffff, a.get('dmgpcnt', 0) & 0xffffffff, o[0], o[1], o[2], o[3], o[4]),
                         this=me)
            result['returned'] = s32(r)
            result['outs'] = read_outs()
        elif kind == 'do-attack':
            result['returned'] = s32(call(vm, DO_ATTACK, (
                a['attack'] & 0xffffffff, a.get('impact', -1) & 0xffffffff, a.get('damage', 0) & 0xffffffff,
                a.get('tohit', 0) & 0xffffffff, a.get('roll', 0) & 0xffffffff, self._obj(a.get('targ'))), this=me))
        elif kind in ('resolve-attack', 'resolve-impact', 'resolve-block', 'resolve-dead'):
            entry = {'resolve-attack': RESOLVE_ATTACK, 'resolve-impact': RESOLVE_IMPACT,
                     'resolve-block': RESOLVE_BLOCK, 'resolve-dead': RESOLVE_DEAD}[kind]
            result['returned'] = s32(call(vm, entry, (vm.u32(me + 0xd8), a.get('bits', 0) & 0xffffffff), this=me))
        elif kind == 'block':
            result['returned'] = s32(call(vm, BLOCK, (a.get('frames', -1) & 0xffffffff,), this=me))
        elif kind == 'stop-block':
            result['returned'] = s32(call(vm, STOP_BLOCK, (), this=me))
        elif kind == 'dodge':
            result['returned'] = s32(call(vm, DODGE, (a.get('dir', -1) & 0xffffffff,), this=me))
        elif kind == 'ai':
            call(vm, AI, (), this=me)
            result['returned'] = 0
        elif kind == 'per-monster':
            frame = int(case.get('globals', {}).get('frame', 0))
            for k in range(a.get('ticks', 1)):
                vm.put_u32(G_PLAYSCREEN + 0x680, frame + k)
                call(vm, PER_MONSTER, (), this=me)
                world.seams.append(dict(seam='Tick', n=k))
            result['returned'] = 0
        elif kind == 'resolve-hit':
            cd = vm.u32(me + 0xfc)
            rec = vm.u32(vm.u32(cd + C_ATTACKS + 0x10) + 4 * a['attack'])
            imp = rec + A_IMPACTS + IMPACT_SIZE * a['impact'] if a.get('impact', -1) >= 0 else 0
            result['returned'] = s32(call(vm, RESOLVE_HIT, (
                self._obj(a.get('targ')), rec, imp, a['damage'] & 0xffffffff, a['tohit'] & 0xffffffff,
                a['roll'] & 0xffffffff), this=me))
        elif kind == 'on-attacked':
            call(vm, ON_ATTACKED, (self._obj(a.get('attacker')), self._obj(a.get('victim')),
                                   a.get('flag', 0) & 0xffffffff), this=me)
            result['returned'] = 0
        elif kind in ('button-attack', 'button-action', 'random-attack', 'specific-attack'):
            entry = {'button-attack': BUTTON_ATTACK, 'button-action': BUTTON_ACTION,
                     'random-attack': RANDOM_ATTACK, 'specific-attack': SPECIFIC_ATTACK}[kind]
            arg = a['pcnt'] if kind == 'random-attack' else a['attack'] if kind == 'specific-attack' else a['button']
            result['returned'] = s32(call(vm, entry, (arg & 0xffffffff,), this=me))
        elif kind == 'pulse':
            pulse = PULSE[self.specs[case['self']].get('class', 12)]
            g = case.get('globals', {})
            frame, gametime, screenframe = int(g.get('frame', 0)), int(g.get('gametime', 0)), int(g.get('screenframe', 0))
            for k in range(a.get('ticks', 1)):
                vm.put_u32(G_PLAYSCREEN + 0x680, frame + k)
                vm.put_u32(G_PLAYSCREEN + PS_GAMETIME, (gametime + k * 100 // 24) & 0xffffffff)
                vm.put_u32(self.screen + SCREEN_FRAMES, screenframe + k)
                call(vm, pulse, (), this=me)
                world.seams.append(dict(seam='Tick', n=k, **self._vitals(me)))
            result['returned'] = 0
        elif kind == 'kill-exp':
            result['returned'] = s32(call(vm, KILL_EXP, (a['value'] & 0xffffffff,), this=me))
        elif kind in ('exp-kill', 'exp-stealth'):
            call(vm, EXP['kill' if kind == 'exp-kill' else 'stealth'][0], (self._obj(a.get('victim')),), this=me)
            result['returned'] = 0
        elif kind == 'exp-skill':
            call(vm, EXP['skill'][0], (a['skill'] & 0xffffffff, self._obj(a.get('victim'))), this=me)
            result['returned'] = 0
        elif kind == 'add-skill-exp':
            call(vm, ADD_SKILL_EXP, (a['skill'] & 0xffffffff, a['exp'] & 0xffffffff), this=me)
            result['returned'] = 0
        elif kind == 'tag-sound':
            # The sample each name plays (0x40af50: object, name, out, &volume),
            # empty for none.
            out, vol, name = vm.allocate(96), vm.allocate(4), vm.allocate(64)
            samples = []
            for text in a['names']:
                vm.write(name, text.encode(TEXT) + b'\0')
                vm.write(out, bytes(96))
                vm.put_u32(vol, 0x7f)
                call(vm, TAG_SOUND, (me, name, out, vol), this=0)
                samples.append(vm.string(out))
            result['returned'] = samples
        elif kind == 'damage':
            ab = 0
            attacker = self._obj(a.get('attacker'))
            if isinstance(a.get('block'), dict):
                ab = world.new_block(a['block'], world.by_name)
                self._apply_block_fields(ab, attacker, a['block'])
            call(vm, DAMAGE, (a['damage'] & 0xffffffff, a.get('type', -1) & 0xffffffff, a.get('mod', 0) & 0xffffffff,
                              ab, attacker), this=me)
            result['returned'] = 0
        else:
            raise ValueError(f'unknown call {kind!r}')
        # A block made in the call is `new N` per character, as the port numbers them.
        result['chars'] = {spec['name']: self._dump(world.by_name[spec['name']], []) for spec in case['chars']}
        result['ai'] = {spec['name']: self._ai_dump(world.by_name[spec['name']]) for spec in case['chars']}
        if self._boss_world():
            result['boss'] = {spec['name']: self._boss_dump(world.by_name[spec['name']]) for spec in case['chars']}
            result['props'] = {name: dict(nowalk=int(bool(vm.u32(o + O_FLAGS) & OF_NOWALK)),
                                          state=struct.unpack('<h', vm.uc.mem_read(o + 0x0c, 2))[0])
                               for name, o in self.props.items()}
            camera = vm.u32(MAPPANE + MP_CAMERA_OBJ)
            result['camera'] = dict(obj=world._name(camera), flags=vm.u32(MAPPANE + MP_CAMERA_FLAGS)) if camera else None
        if ticking:
            result['pulse'] = {spec['name']: self._pulse_dump(world.by_name[spec['name']], spec)
                               for spec in case['chars'] if 'pulsestate' in spec}
        result['seams'] = list(world.seams)
        result['draws'] = list(world.draws)
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('executable')
    parser.add_argument('--serve', action='store_true')
    parser.add_argument('--case', type=Path)
    args = parser.parse_args()
    fixture = MeleeFixture(args.executable)
    if args.serve:
        serve(fixture.sha, SCHEMA, fixture.setup_ms, fixture.run)
    elif args.case:
        print(json.dumps(fixture.run(json.loads(args.case.read_text())), indent=1))


if __name__ == '__main__':
    main()
