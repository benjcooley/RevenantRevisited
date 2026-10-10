"""Combat slot: retail characters in guest memory, and the seams the combat
katas share (docs/gameplay/COMBAT_DOJO.md on feature/combat, §1 and §6).

- `start(executable)`: Runtime on the unchanged retail image, original CRT
  heap/TLS init (fixturekit's `start`) and the original angle/distance tables
  (`0x41e2de`..`0x41e535`, as the missile probe builds them).
- `CombatWorld`: characters and action blocks laid out as retail's own
  (layouts in COMBAT_DOJO.md §6), built per case after the checkpoint and
  gone at the restore; the shared seams answered from the case (a
  character's states and stats); the globals combat reads, set per case.
- `block_dump` / `flag_names`: an action block as the compared record,
  flags by meaning (retail's bits are not the port's).

Seams (each call recorded in `world.seams`, in order):
- FindState `0x477c10` (name, pcnt) and FindTransitionState `0x477c30`
  (from, to, pcnt): from the character's state table, case-blind names,
  "<from> to <to>" for a transition. Index in the table, or -1.
- GetObjStat `0x4d7520` (TCharacter) / `0x51ae30` (TPlayer), slot 0xdc:
  the object's own stats (class.def OBJSTATS: Health, Fatigue, Mana, ...),
  from the case's `stats`. Their ids come from globals that static init
  fills in retail; the fixture gives them fixed ids (OBJSTAT_IDS).
- GetStat `0x4d74d0`, slot 0xd8: the stats of the object's type (class.def
  STATS: Radius, ...), from the case's `classstats` (CLASSSTAT_IDS).
  Names as the port's: OBJSTATFUNC / STATFUNC.
- GetStat by name `0x4d7510`, slot 0xd4 (TCharacter's and TPlayer's): a
  type stat the case's `classstats` names ("Value" -> value).
- SetObjStat `0x4d74b0` (TCharacter) / `0x51adb0` (TPlayer), slot 0xe8:
  recorded, the case's stat takes the value. Object stats the code names by
  a literal id (the player's attributes and skills, charstats.h) are named
  by LITERAL_OBJSTATS.
- The RNG tape: random(lo, hi) `0x483300` and rand `0x58c582` answer from
  the case's `tape` (values 0..32767, in order), then retail's generator
  from the case's `seed`; every draw is recorded in `world.draws` as
  {lo, hi, result} (random) or {rand} (a direct rand). random itself is
  answered here: lo when equal (no draw), the pair in order,
  lo + v % (hi - lo + 1) -- 0x483300 exactly.
- The animation layer below the action-state machine: SetState `0x46f250`
  (slot 0x18; recorded, the state index stored at +0x0c -- what it does
  past that, redraw, walkmap, animator, is its own kata), and a stand-in
  imagery object at +0x54 whose vtable the host answers (NumStates +0x3c,
  GetAniFlags +0x8c; any other slot fails the case) and whose header holds
  each state's frame count. A state in the case is a name or
  {"name", "frames" (default 10), "aniflags" (default 0)}.
- The ground (`set_ground`, a case's `ground`): the sector lookup `0x499e10`
  answers a stand-in sector (or none, for the case's `nosector`), and
  TSector::ReturnWalkmap `0x499720` the case's height for the walk cell --
  `z`, or the last of its `cells` boxes [gx0, gy0, gx1, gy1, height] that
  holds it. Not recorded (a read of map data). GetWalkHeight `0x452e10`
  and GetWalkHeightRadius `0x4530a0` run as original over it.
- The characters near a point: the map iterator CharBlocking walks
  (`0x44ceb0` init, `0x44d080` next; the item at +0x0c) gives the case's
  `nearby` names, else every character, in case order. Recorded
  (NearbyCharacters: pos, range).
- SetPos `0x46ed70` (slot 8; sector moves, walkmaps, redraw): recorded, the
  position stored at +0x10.
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
from fixturekit import Boundaries, MALLOC, s32, serve, start as crt_start  # noqa: E402,F401

ANGLE_TABLES = (0x41e2de, 0x41e535)

# Objects (COMBAT_DOJO.md §6.1)
O_VTABLE, O_CLASS, O_FLAGS, O_POS, O_FACING, O_NAME, O_ID = 0x00, 0x04, 0x08, 0x10, 0x36, 0x38, 0x40
O_MOVEANGLE = 0xb0
O_DOING, O_DESIRED, O_ROOT = 0xd8, 0xdc, 0xe0
O_CHARDATA, O_CHARFLAGS, O_RETREATING, O_MONSTER = 0xfc, 0x110, 0x254, 0x280
O_PLAYERSTATE = 0x36c
# Motion (forensics/COMBAT_MOTION.md §3.8): vel and the carried fraction in
# 1/0x10000 units, the animation's move (GetNextMove 0x470c30), a MoveTo, the
# shove side, and the three fields a blocked step clears (+0x254..+0x25c).
O_VEL, O_ACCUM, O_INVENTNUM = 0x1c, 0x28, 0x7c
O_MOVEDIST, O_MOVEVERT = 0xb4, 0xb8
O_MOVETOPOS, O_MOVEPOS, O_FORCENOMOVE, O_SHOVEDIR = 0xec, 0xf0, 0x10c, 0x11c
O_RETREAT_LATCH, O_RETREAT_FRAMES = 0x258, 0x25c
CHAR_SIZE, PLAYER_SIZE = 0x2a0, 0x674
CHAR_FLAGS = 0x8 | 0x20 | 0x4000 | 0x8000        # OF_MOVING | OF_AI | OF_ANIMATE | OF_PULSE
CHAR_VTABLE, PLAYER_VTABLE = 0x5a7848, 0x5b4f30
CLASS_PLAYER, CLASS_CHARACTER = 0x0b, 0x0c
CHARDATA_SIZE = 0x600
CD_DAMAGEMODS, CD_COMBATRANGEMAX, CD_ARMOR = 0xe4, 0x15c, 0x1c4
# Walk speeds: -1 in the shipped char.def (the parser's default), so the
# animation moves every character.
CD_SPEEDS = dict(walkspeed=0x1ec, runspeed=0x1f0, sneakspeed=0x1f4, combatwalkspeed=0x1f8)

# TActionBlock (§6.2)
AB_SIZE, AB_CTOR = 100, 0x4da9f0                 # ctor thiscall (name, action), ret 8
AB = dict(action=0x00, name=0x04, frame=0x24, wait=0x28, angle=0x2c, moveangle=0x30,
          turnrate=0x34, target=0x38, obj=0x44, attack=0x48, impact=0x4c, damage=0x50,
          data=0x5c, flags=0x60)
# Retail's flag bits by meaning. Confirmed from SetRoot/SetDoing/SetDesired/
# UpdateAction, ForceCommand and Go/ResolveCombat (COMBAT_DOJO.md §5.4):
# firsttime, transition, priority, interrupt, nowaitdone, dontforce, stop
# (ResolveCombatMove 0x4c7f80), waitpivot, noroot, walkto (walking to an
# item, Goto's third argument). The rest are
# the 1998 names shifted up one bit past the inserted one -- unconfirmed,
# so they keep a `?`, and a block that sets one shows up as a difference.
AB_FLAGS = {0x1: 'firsttime', 0x2: 'transition', 0x4: 'forced', 0x8: 'retail-0x8',
            0x10: 'priority', 0x20: 'interrupt', 0x40: 'nowaitdone', 0x80: 'dontforce',
            0x100: 'stop', 0x200: 'waitpivot', 0x400: 'noroot', 0x800: 'loop?',
            0x1000: 'walkto'}

# Globals
G_COMBATFACE = 0x5d7a64                          # Revenant.ini CombatFace (default 1)
G_MULTIPLAYER = 0x66829c
G_AI_OFF = 0x668110
G_PLAYSCREEN = 0x65caf0                          # GameFrame = [+0x688] - [+0x684] + [+0x680]
# PlayScreen +0x5e0: the player has control (set by 0x47c580, which also
# clears the control-off flag 0x666924); 0 while a script holds control.
# IsValidTarget (0x4cd990) takes the player as a target only while it, or
# +0x5d8 (set by 0x47c550; meaning unknown), is set.
G_PS_5D8, G_PS_CONTROL = 0x65d0c8, 0x65d0d0
G_PLAYER = 0x667fcc
OBJSTAT_IDS = {'health': (0x66ca4c, 0x101), 'fatigue': (0x66ca3c, 0x102), 'mana': (0x66ca38, 0x103),
               # Retail's own ids for the rest (charstats.h), the globals static init fills.
               'aggressive': (0x66caa4, 0), 'sleeping': (0x66ca58, 0x104), 'damagemod': (0x66ca28, 16),
               'level': (0x66da34, 17), 'acbonus': (0x66da38, 30), 'edgebonus': (0x66d950, 33)}
# Object stats the code asks for by a literal id: the player's attack level,
# attributes and skills (charstats.h; IsValidAttack 0x4d1652, TPlayer
# 0x51a480 / 0x51a580 / 0x520900).
LITERAL_OBJSTATS = {20: 'attacklevel', 34: 'strn', 35: 'cons', 36: 'agil', 37: 'rflx', 38: 'mind', 39: 'luck',
                    **{40 + i: n for i, n in enumerate(('attack', 'defense', 'invoke', 'hands', 'knife', 'sword',
                                                        'bludgeons', 'axes', 'bows', 'stealth', 'lockpick'))}}
# SetObjStat (slot 0xe8, thiscall (id, value) ret 8): recorded, the value stored.
SET_OBJSTAT = {CLASS_CHARACTER: 0x4d74b0, CLASS_PLAYER: 0x51adb0}
G_AMBIENT = 0x6671a4                             # MapPane ambient light (Visibility 0x4c5aa0)
# The action state UpdateAction reads: commanddone, the animator (only
# tested for null: a case's `animator` gives a stand-in), the stealth values.
O_COMMANDDONE, O_ANIMATOR, O_GLIMPSE, O_NOISE = 0x80, 0x58, 0x130, 0x134
O_FRAMERATE, O_PREVSTATE, O_PREVFRAME, O_MOVEBITS = 0x5e, 0x60, 0x62, 0xbc
O_COMBATFLASH = 0x224                            # combat flash ticks (a blow sets 5)
# Perception (kata M9b): the object info (its first field the type name,
# GetTypeName), the invisibility spell, the memory of characters seen
# (MAXHASSEEN entries of 12 bytes: character, game frame, no-autocombat),
# a player's team record name (char[50]).
O_INFO, O_INVISIBLE_SPELL, O_HASSEEN, O_TEAM = 0x4c, 0x1a4, 0x1c0, 0x494
MAXHASSEEN, HASSEEN_SIZE = 8, 12
TEXT = 'cp1252'
# Type stats, read through slot 0xd8. Radius: TCharacter::Radius (slot
# 0x258, 0x4d6e40), which Distance (0x4d61b0) subtracts.
CLASSSTAT_IDS = {'radius': (0x66ca30, 0x201)}

FIND_STATE, FIND_TRANSITION = 0x477c10, 0x477c30
SET_STATE = 0x46f250                             # slot 0x18, thiscall (state), ret 4
O_STATE, O_IMAGERY, O_FRAME = 0x0c, 0x54, 0x5c   # state (short), imagery, frame (short)
# The stand-in imagery object at +0x54 (what ForceCommand reads directly):
# vtable slots answered by the host, header +0x04 -> states +0x54, each
# 0x4c bytes with the frame count at +0x32.
IMAGERY_SLOTS = 0x80
IMAGERY_SIZE, IM_MESHINIT, IM_MOTION, HDR_NUMSTATES = 0x100, 0x0c, 0x98, 0x04
IM_SLOT_SET_OBJECT_MOTION, SET_OBJECT_MOTION_3D = 0x4c, 0x40cd20   # T3DImagery::SetObjectMotion
ANIMATOR_SLOTS = 0x40
# Kata M8, the animation layer as original code: SetState 0x46f250 itself
# (with ResetState 0x46f1e0), SetObjectMotion 0x470bb0 -> imagery slot 0x4c,
# NextFrame 0x470cc0. What SetState does to the map is left out: the
# background redraw (slot 0xf4 0x471020, ret 4; MapPane 0x4548a0, ret 8),
# the walkmap (MapPane 0x452750, ret 0xc), the object lock (0x456790, ret 8;
# 0x4567c0).
MAP_NOOPS = ((0x471020, 'RedrawBackground', 4), (0x4548a0, 'MapPane.Redraw', 8),
             (0x452750, 'MapPane.Walkmap', 0xc), (0x456790, 'MapPane.Lock', 8), (0x4567c0, 'MapPane.Unlock', 0))
IM_HEADER, HDR_STATES, STATE_SIZE, ST_FRAMES = 0x04, 0x54, 0x4c, 0x32
GET_STAT = 0x4d74d0                              # slot 0xd8, thiscall (id), ret 4
GET_STAT_NAMED = 0x4d7510                        # slot 0xd4, thiscall (name), ret 4
O_FRAME_SHORT = 0x5c
RANDOM, RAND = 0x483300, 0x58c582                 # random(lo, hi) cdecl; MSVC rand()
GET_OBJSTAT = {CLASS_CHARACTER: 0x4d7520, CLASS_PLAYER: 0x51ae30}
FIND_SECTOR, RETURN_WALKMAP = 0x499e10, 0x499720  # cdecl (level, sx, sy); thiscall (x, y) ret 8
ITER_INIT, ITER_NEXT, ITER_ITEM = 0x44ceb0, 0x44d080, 0x0c   # init thiscall, 6 args, ret 0x18
SET_POS = 0x46ed70                               # slot 8, thiscall (pos*, level, override), ret 0xc
SLOT_MOVE = 0x114                                # TCharacter::Move 0x4c46d0, TPlayer::Move 0x518df0


def ground_height(ground, gx, gy):
    """A walk cell's height in the case's ground: `z`, or the last `cells`
    box [gx0, gy0, gx1, gy1, height] holding (gx, gy)."""
    h = ground.get('z', 0)
    for gx0, gy0, gx1, gy1, ch in ground.get('cells', []):
        if gx0 <= gx <= gx1 and gy0 <= gy <= gy1:
            h = ch
    return h


def start(executable):
    vm, sha = crt_start(executable)
    vm.call(ANGLE_TABLES[0], stop_address=ANGLE_TABLES[1], instruction_limit=20_000_000)
    for address, sid in list(OBJSTAT_IDS.values()) + list(CLASSSTAT_IDS.values()):
        vm.put_u32(address, sid)
    _watch_faults(vm)
    return vm, sha


def _watch_faults(vm):
    """Name an unmapped access: `vm.last_fault` = (eip, kind, address)."""
    from unicorn import UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE_UNMAPPED, UC_HOOK_MEM_FETCH_UNMAPPED
    from unicorn.x86_const import UC_X86_REG_EIP
    vm.last_fault = None

    from unicorn.x86_const import UC_X86_REG_ESP

    def hook(uc, access, address, size, value, user):
        # Code addresses on the stack: the likely callers, innermost first.
        sp = uc.reg_read(UC_X86_REG_ESP)
        words = struct.unpack('<64I', uc.mem_read(sp, 256))
        callers = [w for w in words if 0x401000 <= w < 0x5a0000][:8]
        vm.last_fault = (uc.reg_read(UC_X86_REG_EIP), access, address, callers)
        return False

    vm.uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED | UC_HOOK_MEM_FETCH_UNMAPPED, hook)


def call(vm, address, args=(), this=None, instruction_limit=5_000_000):
    """vm.call, a fault reported with where it happened."""
    vm.last_fault = None
    try:
        return vm.call(address, args, this=this, instruction_limit=instruction_limit)
    except Exception as error:
        if vm.last_fault:
            eip, kind, at, callers = vm.last_fault
            raise RuntimeError(f'{error} at eip {eip:#x}, access {at:#x}, '
                               f'stack {" ".join(f"{c:#x}" for c in callers)}') from error
        raise


def flag_names(flags):
    names = [name for bit, name in AB_FLAGS.items() if flags & bit]
    rest = flags & ~sum(AB_FLAGS)
    if rest:
        names.append(f'bits-{rest:#x}')
    return sorted(names)


def write_text(vm, address, size, text):
    """A fixed char[size], NUL-padded."""
    raw = text.encode(TEXT)[:size - 1]
    vm.write(address, raw + bytes(size - len(raw)))


def write_fields(vm, base, fields, values):
    """The `fields` that `values` names, at `base`, by kind: an offset (an
    int32), ('s', offset, size) text, ('b', offset) a byte, ('i', offset,
    n) int32s (data_parse's tables)."""
    for name, kind in fields:
        if name not in values:
            continue
        v = values[name]
        if isinstance(kind, int):
            vm.put_u32(base + kind, int(v) & 0xffffffff)
        elif kind[0] == 's':
            write_text(vm, base + kind[1], kind[2], v)
        elif kind[0] == 'b':
            vm.write(base + kind[1], bytes([int(v) & 0xff]))
        else:
            for i, x in enumerate(v[:kind[2]]):
                vm.put_u32(base + kind[1] + 4 * i, int(x) & 0xffffffff)


class CombatWorld:
    """Per-case guest objects plus the shared seams. Create once before the
    checkpoint (it installs the hooks); call `reset()` at the start of each
    case."""

    def __init__(self, vm):
        self.vm = vm
        self.boundaries = Boundaries(vm)
        b = self.boundaries
        b.add(FIND_STATE, 'FindState', 8, self._find_state)
        b.add(FIND_TRANSITION, 'FindTransitionState', 0xc, self._find_transition)
        for objclass, address in GET_OBJSTAT.items():
            b.add(address, 'GetObjStat', 4, self._get_objstat)
        b.add(GET_STAT, 'GetStat', 4, self._get_stat)
        b.add(GET_STAT_NAMED, 'GetStat', 4, self._get_stat_named)
        for objclass, address in SET_OBJSTAT.items():
            b.add(address, 'SetObjStat', 8, self._set_objstat)
        b.add(SET_STATE, 'SetState', 4, self._set_state)
        b.add(RANDOM, 'random', 0, self._random)
        b.add(RAND, 'rand', 0, self._rand)
        b.add(FIND_SECTOR, 'FindSector', 0, self._find_sector)
        b.add(RETURN_WALKMAP, 'ReturnWalkmap', 8, self._return_walkmap)
        b.add(ITER_INIT, 'MapIterator', 0x18, self._iter_init)
        b.add(ITER_NEXT, 'MapIterator.Next', 0, self._iter_next)
        b.add(SET_POS, 'SetPos', 0xc, self._set_pos)
        self.animators = {}        # stand-in animator address -> character
        self.imagery = {}
        self._imagery_stubs()
        self._animator_stubs()
        for address, name, pop in MAP_NOOPS:
            b.add(address, name, pop, lambda args, ecx: 0)
        self.reset()

    def reset(self):
        self.objects = {}          # guest address -> spec name
        self.by_name = {}
        self.states = {}           # guest address -> [state names]
        self.stats = {}            # guest address -> {object stat id: value}
        self.classstats = {}       # guest address -> {type stat id: value}
        self.classstats_named = {} # guest address -> {type stat name: value}
        self.imagery = {}          # stand-in imagery address -> character
        self.animators.clear()
        self.needs_animator = {}   # character -> imagery NeedsAnimator answer
        self.real_setstate = False # kata M8: SetState runs as original
        self.blocks = {}           # guest address -> label of a block built by the fixture
        self.seams = []
        self.draws = []
        self.tape = []
        self.vm.rng_seed = 1
        self.ground = None         # the case's walkmap, if it has one
        self.nearby = None         # names the map iterator gives, else every character
        self.sectors = {}          # (sx, sy) -> stand-in sector, and back
        self.sector_at = {}
        self.iters = {}            # iterator address -> characters still to give

    def set_rng(self, case):
        """The case's RNG: `tape` first, then retail's generator from `seed`."""
        self.tape = list(case.get('tape', []))
        self.vm.rng_seed = case.get('seed', 1) & 0xffffffff
        self.draws = []

    def _next(self):
        if self.tape:
            return self.tape.pop(0) & 0x7fff
        return self.vm.random_msvc()

    def _random(self, args, ecx):
        lo, hi = s32(args[0]), s32(args[1])
        if lo == hi:
            return lo
        if hi < lo:
            lo, hi = hi, lo
        result = lo + self._next() % (hi - lo + 1)
        self.draws.append(dict(lo=lo, hi=hi, result=result))
        return result

    def _rand(self, args, ecx):
        value = self._next()
        self.draws.append(dict(rand=value))
        return value

    def set_ground(self, case):
        """The case's walkmap (`ground`) and the characters the map gives
        near a point (`nearby`)."""
        self.ground = case.get('ground')
        self.nearby = case.get('nearby')

    # -- seams ------------------------------------------------------------
    def _find_sector(self, args, ecx):
        if self.ground is None:
            raise RuntimeError('the walkmap was read, but the case has no ground')
        key = (s32(args[1]), s32(args[2]))
        if list(key) in self.ground.get('nosector', []):
            return 0
        if key not in self.sectors:
            sector = self.vm.allocate(0x10)
            self.sectors[key] = sector
            self.sector_at[sector] = key
        return self.sectors[key]

    def _return_walkmap(self, args, ecx):
        sx, sy = self.sector_at[ecx]
        return ground_height(self.ground, sx * 64 + s32(args[0]), sy * 64 + s32(args[1]))

    def _iter_init(self, args, ecx):
        pos = list(struct.unpack('<3i', self.vm.uc.mem_read(args[0], 12)))
        self.seams.append(dict(seam='NearbyCharacters', pos=pos, range=s32(args[1])))
        names = self.nearby if self.nearby is not None else list(self.by_name)
        self.iters[ecx] = [self.by_name[n] for n in names]
        self._iter_next(args, ecx)
        return ecx

    def _iter_next(self, args, ecx):
        items = self.iters.get(ecx, [])
        self.vm.put_u32(ecx + ITER_ITEM, items.pop(0) if items else 0)
        return 0

    def _set_pos(self, args, ecx):
        pos = list(struct.unpack('<3i', self.vm.uc.mem_read(args[0], 12)))
        self.vm.write(ecx + O_POS, struct.pack('<3i', *pos))
        self.seams.append(dict(seam='SetPos', who=self._name(ecx), pos=pos))
        return 1

    def _name(self, address):
        return self.objects.get(address, f'{address:#x}')

    def _find_state(self, args, ecx):
        name = self.vm.string(args[0])
        table = [s['name'].lower() for s in self.states.get(ecx, [])]
        index = table.index(name.lower()) if name.lower() in table else -1
        self.seams.append(dict(seam='FindState', who=self._name(ecx), name=name, result=index))
        return index

    def _find_transition(self, args, ecx):
        frm, to = self.vm.string(args[0]), self.vm.string(args[1])
        table = [s['name'].lower() for s in self.states.get(ecx, [])]
        key = f'{frm} to {to}'.lower()
        index = table.index(key) if key in table else -1
        self.seams.append(dict(seam='FindTransitionState', who=self._name(ecx), frm=frm, to=to, result=index))
        return index

    def _stat(self, seam, table, ids, args, ecx):
        sid = s32(args[0])
        stats = table.get(ecx, {})
        names = {v[1]: k for k, v in ids.items()}
        if ids is OBJSTAT_IDS:
            names = {**LITERAL_OBJSTATS, **names}
        if sid not in stats:
            raise KeyError(f'{self._name(ecx)} has no {names.get(sid, sid)} in the case')
        self.seams.append(dict(seam=seam, who=self._name(ecx), stat=names.get(sid, sid), result=stats[sid]))
        return stats[sid]

    def _get_objstat(self, args, ecx):
        return self._stat('GetObjStat', self.stats, OBJSTAT_IDS, args, ecx)

    def _set_objstat(self, args, ecx):
        sid, value = s32(args[0]), s32(args[1])
        names = {**LITERAL_OBJSTATS, **{v[1]: k for k, v in OBJSTAT_IDS.items()}}
        self.stats.setdefault(ecx, {})[sid] = value
        self.seams.append(dict(seam='SetObjStat', who=self._name(ecx), stat=names.get(sid, sid), value=value))
        return 0

    def _get_stat(self, args, ecx):
        return self._stat('GetStat', self.classstats, CLASSSTAT_IDS, args, ecx)

    def _get_stat_named(self, args, ecx):
        name = self.vm.string(args[0]).lower()
        stats = self.classstats_named.get(ecx, {})
        if name not in stats:
            raise KeyError(f'{self._name(ecx)} has no {name} in the case')
        self.seams.append(dict(seam='GetStat', who=self._name(ecx), stat=name, result=stats[name]))
        return stats[name]

    def _set_state(self, args, ecx):
        if self.real_setstate:
            return Boundaries.ORIGINAL
        index = s32(args[0])
        table = self.states.get(ecx, [])
        name = table[index]['name'] if 0 <= index < len(table) else None
        self.vm.write(ecx + O_STATE, struct.pack('<h', index))
        self.seams.append(dict(seam='SetState', who=self._name(ecx), state=index, name=name))
        return 1

    def _stub_table(self, slots, answers, owner_of, kind):
        """A vtable of `ret` stubs answered by the host, one hook over all of
        it; `answers`: slot -> (name, argument bytes, fn(owner, args)). The
        owner is `owner_of(ECX)`. A slot not answered fails the case. Each
        call is recorded as `<kind>.<name>`."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
        vm = self.vm
        stubs = vm.allocate(slots * 16)
        vm.write(stubs, b'\xc3' * (slots * 16))
        vtable = vm.allocate(slots * 4)
        for i in range(slots):
            vm.put_u32(vtable + 4 * i, stubs + 16 * i)

        def enter(uc, address, size, user):
            slot = (address - stubs) // 16 * 4
            sp = uc.reg_read(UC_X86_REG_ESP)
            args = struct.unpack('<4I', uc.mem_read(sp + 4, 16))
            owner = owner_of(uc.reg_read(UC_X86_REG_ECX))
            try:
                if slot not in answers:
                    raise RuntimeError(f'{kind} slot {slot:#x} reached for {self._name(owner)} (not modelled)')
                name, pop, fn = answers[slot]
                result = fn(owner, args)
            except Exception as error:
                vm.error = error
                uc.emu_stop()
                return
            self.seams.append(dict(seam=f'{kind}.{name}', who=self._name(owner),
                                   args=[s32(a) for a in args[:pop // 4]], result=result))
            uc.reg_write(UC_X86_REG_EAX, (result or 0) & 0xffffffff)
            uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)

        vm.uc.hook_add(UC_HOOK_CODE, enter, begin=stubs, end=stubs + slots * 16 - 1)
        return vtable

    def _imagery_stubs(self):
        """The stand-in imagery's vtable: the state table's queries answered
        here; SetObjectMotion (slot 0x4c) is the original T3DImagery one,
        over the motion tables `_new_imagery` lays out."""
        frames = lambda owner, args: self._state(owner, args)['frames']
        flags = lambda owner, args: self._state(owner, args)['aniflags']
        self.imagery_vtable = self._stub_table(IMAGERY_SLOTS, {
            0x3c: ('NumStates', 0, self._im_numstates), 0x8c: ('GetAniFlags', 4, flags),
            0x90: ('GetAniLength', 4, frames), 0x94: ('GetInvAniFlags', 4, flags),
            0x98: ('GetInvAniLength', 4, frames),
            0x38: ('NeedsAnimator', 4, lambda owner, args: int(self.needs_animator.get(owner, 1)))},
            lambda ecx: self.imagery.get(ecx), 'imagery')
        self.vm.put_u32(self.imagery_vtable + IM_SLOT_SET_OBJECT_MOTION, SET_OBJECT_MOTION_3D)

    def _animator_stubs(self):
        """The stand-in animator's vtable: what the object layer tells an
        animator, recorded."""
        none = lambda owner, args: 0
        self.animator_vtable = self._stub_table(ANIMATOR_SLOTS, {
            0x00: ('delete', 4, none), 0x1c: ('Close', 0, none), 0x20: ('ResetState', 0, none),
            0x24: ('SetComplete', 4, none), 0x28: ('SetNewState', 4, none)},
            lambda ecx: self.animators.get(ecx), 'animator')

    def _state(self, owner, args):
        index = s32(args[0])
        table = self.states[owner]
        return table[index] if 0 <= index < len(table) else dict(frames=0, aniflags=0)

    def _im_numstates(self, owner, args):
        return len(self.states[owner])

    def _new_imagery(self, obj, table):
        """T3DImagery's layout as far as the code reads it: the header's
        state table (count at +4, frames at state * 0x4c + 0x32), the mesh
        marked initialized (+0x0c), and per state the case's motion, one
        8-byte SMotionData a frame (+0x98; GetMotion 0x40cc40)."""
        vm = self.vm
        imagery = vm.allocate(IMAGERY_SIZE)
        header = vm.allocate(0x60)
        states = vm.allocate(max(1, len(table)) * STATE_SIZE)
        vm.put_u32(imagery, self.imagery_vtable)
        vm.put_u32(imagery + IM_HEADER, header)
        vm.put_u32(imagery + IM_MESHINIT, 1)
        vm.put_u32(header + HDR_STATES, states)
        vm.put_u32(states + HDR_NUMSTATES, len(table))
        motion = vm.allocate(max(1, len(table)) * 4)
        vm.put_u32(imagery + IM_MOTION, motion)
        for i, st in enumerate(table):
            vm.write(states + i * STATE_SIZE + ST_FRAMES, struct.pack('<h', st['frames']))
            if st.get('motion'):
                data = vm.allocate(len(st['motion']) * 8)
                for f, (dist, vert, ang, rx, ry, rz) in enumerate(st['motion']):
                    vm.write(data + 8 * f, struct.pack('<IBBBB', (dist & 0xffff) | ((vert & 0xffff) << 16),
                                                       ang & 0xff, rx & 0xff, ry & 0xff, rz & 0xff))
                vm.put_u32(motion + 4 * i, data)
        self.imagery[imagery] = obj
        return imagery

    def new_animator(self, obj):
        """A stand-in animator at +0x58 (its calls recorded)."""
        animator = self.vm.allocate(0x40)
        self.vm.put_u32(animator, self.animator_vtable)
        self.animators[animator] = obj
        return animator

    # -- globals ----------------------------------------------------------
    def set_globals(self, g):
        vm = self.vm
        vm.put_u32(G_COMBATFACE, int(g.get('combatface', 1)))
        vm.put_u32(G_MULTIPLAYER, 0)
        vm.put_u32(G_AI_OFF, int(g.get('ai_off', 0)))
        vm.put_u32(G_PLAYSCREEN + 0x680, int(g.get('frame', 0)))
        vm.put_u32(G_PLAYSCREEN + 0x684, 0)
        vm.put_u32(G_PLAYSCREEN + 0x688, 0)
        vm.put_u32(G_PS_5D8, int(g.get('ps_5d8', 0)))
        vm.put_u32(G_PS_CONTROL, int(g.get('control', 1)))
        vm.put_u32(G_AMBIENT, int(g.get('ambient', 128)))

    # -- objects ----------------------------------------------------------
    def new_block(self, spec, objmap):
        """An action block from the case: built by the original ctor, then
        the case's fields over it (flags by meaning)."""
        vm = self.vm
        ab = vm.call(MALLOC, (AB_SIZE,))
        name = vm.allocate(len(spec['name']) + 1)
        vm.write(name, spec['name'].encode('cp1252') + b'\0')
        vm.call(AB_CTOR, (name, spec['action']), this=ab)
        for key in ('angle', 'moveangle', 'turnrate', 'frame', 'wait'):
            if key in spec:
                vm.put_u32(ab + AB[key], spec[key] & 0xffffffff)
        if spec.get('obj'):
            vm.put_u32(ab + AB['obj'], objmap[spec['obj']])
        if 'flags' in spec:
            bits = {name: bit for bit, name in AB_FLAGS.items()}
            vm.put_u32(ab + AB['flags'], sum(bits[f] for f in spec['flags']))
        return ab

    def new_character(self, spec):
        """A TCharacter (class 0xc) or TPlayer (0xb), zeroed, the real
        vtable, the case's fields. Blocks are set by `set_blocks` once every
        character exists (a block's obj names another character)."""
        vm = self.vm
        player = spec.get('class', CLASS_CHARACTER) == CLASS_PLAYER
        obj = vm.allocate(PLAYER_SIZE if player else CHAR_SIZE)
        vm.put_u32(obj + O_VTABLE, PLAYER_VTABLE if player else CHAR_VTABLE)
        vm.write(obj + O_CLASS, struct.pack('<h', CLASS_PLAYER if player else CLASS_CHARACTER))
        vm.write(obj + O_POS, struct.pack('<3i', *spec.get('pos', (0, 0, 0))))
        vm.write(obj + O_FACING, bytes([spec.get('facing', 0) & 0xff]))
        vm.put_u32(obj + O_MOVEANGLE, spec.get('moveangle', spec.get('facing', 0)) & 0xffffffff)
        vm.put_u32(obj + O_CHARFLAGS, spec.get('charflags', 0))
        # A character's flags as its constructor leaves them (moving, AI,
        # animate, pulse), plus the case's.
        vm.put_u32(obj + O_FLAGS, CHAR_FLAGS | spec.get('objflags', 0))
        vm.write(obj + O_INVENTNUM, struct.pack('<h', spec.get('inventnum', -1)))
        vm.write(obj + O_VEL, struct.pack('<3i', *spec.get('vel', (0, 0, 0))))
        vm.write(obj + O_ACCUM, struct.pack('<3i', *spec.get('accum', (0, 0, 0))))
        vm.put_u32(obj + O_MOVEDIST, spec.get('movedist', 0) & 0xffffffff)
        vm.put_u32(obj + O_MOVEVERT, spec.get('movevert', 0) & 0xffffffff)
        if 'moveto' in spec:
            vm.put_u32(obj + O_MOVETOPOS, 1)
            vm.write(obj + O_MOVEPOS, struct.pack('<3i', *spec['moveto']))
        vm.put_u32(obj + O_FORCENOMOVE, int(spec.get('forcenomove', 0)))
        vm.put_u32(obj + O_SHOVEDIR, spec.get('shovedir', -1) & 0xffffffff)
        vm.put_u32(obj + O_RETREATING, int(spec.get('retreating', 0)))
        vm.put_u32(obj + O_RETREAT_LATCH, int(spec.get('retreat_latch', 0)))
        vm.put_u32(obj + O_RETREAT_FRAMES, spec.get('retreat_frames', 0))
        vm.put_u32(obj + O_COMMANDDONE, int(spec.get('commanddone', 0)))
        if spec.get('animator'):
            vm.put_u32(obj + O_ANIMATOR, self.new_animator(obj))
        self.needs_animator[obj] = spec.get('needsanimator', 1)
        vm.write(obj + O_FRAMERATE, struct.pack('<h', spec.get('framerate', 1)))
        vm.write(obj + O_PREVSTATE, struct.pack('<h', spec.get('prevstate', spec.get('state', 0))))
        vm.put_u32(obj + O_GLIMPSE, spec.get('glimpse', 0) & 0xffffffff)
        vm.put_u32(obj + O_NOISE, spec.get('noise', 0) & 0xffffffff)
        vm.write(obj + O_FRAME, struct.pack('<h', spec.get('frame', 0)))
        vm.put_u32(obj + O_MONSTER, spec.get('monsterkind', 0))
        if player:
            vm.put_u32(obj + O_PLAYERSTATE, spec.get('playerstate', 0))
            vm.put_u32(G_PLAYER, obj)
        name = vm.allocate(len(spec['name']) + 1)
        vm.write(name, spec['name'].encode('cp1252') + b'\0')
        vm.put_u32(obj + O_NAME, name)
        vm.put_u32(obj + O_ID, spec.get('id', 0))
        typename = spec.get('type', spec['name'])
        info, text = vm.allocate(4), vm.allocate(len(typename) + 1)
        vm.write(text, typename.encode(TEXT) + b'\0')
        vm.put_u32(info, text)
        vm.put_u32(obj + O_INFO, info)
        vm.put_u32(obj + O_INVISIBLE_SPELL, int(spec.get('invisiblespell', 0)))
        if player:
            write_text(vm, obj + O_TEAM, 50, spec.get('team', ''))
        cd = vm.allocate(CHARDATA_SIZE)
        chardata = spec.get('chardata', {})
        vm.put_u32(cd + CD_COMBATRANGEMAX, chardata.get('combatrangemax', 0))
        vm.put_u32(cd + CD_ARMOR, chardata.get('armor', 0))
        for i, v in enumerate(chardata.get('damagemods', [])):
            vm.put_u32(cd + CD_DAMAGEMODS + 4 * i, v & 0xffffffff)
        for key, off in CD_SPEEDS.items():
            vm.put_u32(cd + off, chardata.get(key, -1) & 0xffffffff)
        vm.put_u32(obj + O_CHARDATA, cd)
        self.objects[obj] = spec['name']
        self.by_name[spec['name']] = obj
        table = [dict(name=st, frames=10, aniflags=0) if isinstance(st, str)
                 else dict(dict(frames=10, aniflags=0), **st) for st in spec.get('states', [])]
        self.states[obj] = table
        vm.put_u32(obj + O_IMAGERY, self._new_imagery(obj, table))
        vm.write(obj + O_STATE, struct.pack('<h', spec.get('state', 0)))
        vm.write(obj + O_FRAME_SHORT, struct.pack('<h', spec.get('frame', 0)))
        literal = {n: i for i, n in LITERAL_OBJSTATS.items()}
        self.stats[obj] = {OBJSTAT_IDS[k][1] if k in OBJSTAT_IDS else literal[k]: v
                           for k, v in spec.get('stats', {}).items()}
        self.classstats[obj] = {CLASSSTAT_IDS[k][1]: v for k, v in spec.get('classstats', {}).items()
                                if k in CLASSSTAT_IDS}
        self.classstats_named[obj] = dict(spec.get('classstats', {}))
        return obj

    def set_blocks(self, obj, spec):
        """root, doing, desired from the case: each a block spec, or the name
        of another of the three ("root") to share it."""
        made = {}
        for role in ('root', 'doing', 'desired'):
            b = spec.get(role, 'root' if role != 'root' else None)
            if b is None:
                raise ValueError(f"{spec['name']}: a root block is required")
            made[role] = made[b] if isinstance(b, str) else self.new_block(b, self.by_name)
            self.blocks.setdefault(made[role], role)
        self.vm.put_u32(obj + O_ROOT, made['root'])
        self.vm.put_u32(obj + O_DOING, made['doing'])
        self.vm.put_u32(obj + O_DESIRED, made['desired'])

    def set_memory(self, obj, spec):
        """The characters it remembers seeing (`hasseen`: [name, frame,
        noautocombat] each), once every character exists."""
        for i, (name, frame, noauto) in enumerate(spec.get('hasseen', [])[:MAXHASSEEN]):
            self.vm.write(obj + O_HASSEEN + HASSEEN_SIZE * i,
                          struct.pack('<IiI', self.by_name[name], frame, int(bool(noauto))))

    # -- results ----------------------------------------------------------
    def memory_dump(self, obj):
        """The memory as compared: each entry's character (None when empty),
        frame and no-autocombat flag."""
        out = []
        for i in range(MAXHASSEEN):
            chr_, frame, noauto = struct.unpack('<IiI', self.vm.uc.mem_read(obj + O_HASSEEN + HASSEEN_SIZE * i, 12))
            out.append([self._name(chr_) if chr_ else None, frame, int(noauto != 0)])
        return out

    def block_dump(self, ab, new_blocks):
        """A block as the compared record. Identity: the role it had when the
        case began, or `new N` in the order the function made them."""
        if not ab:
            return None
        vm = self.vm
        if ab in self.blocks:
            ident = self.blocks[ab]
        else:
            if ab not in new_blocks:
                new_blocks.append(ab)
            ident = f'new {new_blocks.index(ab) + 1}'
        obj = vm.u32(ab + AB['obj'])
        return dict(id=ident, action=s32(vm.u32(ab + AB['action'])), name=vm.string(ab + AB['name']),
                    frame=s32(vm.u32(ab + AB['frame'])), wait=s32(vm.u32(ab + AB['wait'])),
                    angle=s32(vm.u32(ab + AB['angle'])), moveangle=s32(vm.u32(ab + AB['moveangle'])),
                    turnrate=s32(vm.u32(ab + AB['turnrate'])),
                    target=list(struct.unpack('<3i', vm.uc.mem_read(ab + AB['target'], 12))),
                    obj=None if not obj else self._name(obj),
                    flags=flag_names(vm.u32(ab + AB['flags'])))

    def motion_dump(self, obj):
        """What a move changes beyond the character dump."""
        vm = self.vm
        return dict(vel=list(struct.unpack('<3i', vm.uc.mem_read(obj + O_VEL, 12))),
                    accum=list(struct.unpack('<3i', vm.uc.mem_read(obj + O_ACCUM, 12))),
                    movetopos=vm.u32(obj + O_MOVETOPOS), forcenomove=vm.u32(obj + O_FORCENOMOVE),
                    shovedir=s32(vm.u32(obj + O_SHOVEDIR)),
                    retreating=vm.u32(obj + O_RETREATING),
                    retreat_latch=vm.u32(obj + O_RETREAT_LATCH),
                    retreat_frames=s32(vm.u32(obj + O_RETREAT_FRAMES)),
                    movedist=s32(vm.u32(obj + O_MOVEDIST)), commanddone=vm.u32(obj + O_COMMANDDONE),
                    glimpse=s32(vm.u32(obj + O_GLIMPSE)), noise=s32(vm.u32(obj + O_NOISE)),
                    framerate=struct.unpack('<h', vm.uc.mem_read(obj + O_FRAMERATE, 2))[0],
                    prevstate=struct.unpack('<h', vm.uc.mem_read(obj + O_PREVSTATE, 2))[0],
                    prevframe=struct.unpack('<h', vm.uc.mem_read(obj + O_PREVFRAME, 2))[0],
                    animate=int(bool(vm.u32(obj + O_FLAGS) & 0x4000)), animator=int(bool(vm.u32(obj + O_ANIMATOR))),
                    combatflash=s32(vm.u32(obj + O_COMBATFLASH)))

    def character_dump(self, obj, new_blocks):
        vm = self.vm
        out = dict(state=struct.unpack('<h', vm.uc.mem_read(obj + O_STATE, 2))[0],
                   frame=struct.unpack('<h', vm.uc.mem_read(obj + O_FRAME, 2))[0],
                   facing=vm.uc.mem_read(obj + O_FACING, 1)[0],
                   moveangle=s32(vm.u32(obj + O_MOVEANGLE)),
                   pos=list(struct.unpack('<3i', vm.uc.mem_read(obj + O_POS, 12))))
        for role, off in (('root', O_ROOT), ('doing', O_DOING), ('desired', O_DESIRED)):
            out[role] = self.block_dump(vm.u32(obj + off), new_blocks)
        return out
