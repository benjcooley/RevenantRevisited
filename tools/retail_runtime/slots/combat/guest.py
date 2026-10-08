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
- The animation layer below the action-state machine: SetState `0x46f250`
  (slot 0x18; recorded, the state index stored at +0x0c -- what it does
  past that, redraw, walkmap, animator, is its own kata), and a stand-in
  imagery object at +0x54 whose vtable the host answers (NumStates +0x3c,
  GetAniFlags +0x8c; any other slot fails the case) and whose header holds
  each state's frame count. A state in the case is a name or
  {"name", "frames" (default 10), "aniflags" (default 0)}.
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
O_CHARDATA, O_CHARFLAGS, O_OUT_OF_SIGHT, O_MONSTER = 0xfc, 0x110, 0x254, 0x280
O_PLAYERSTATE = 0x36c
CHAR_SIZE, PLAYER_SIZE = 0x2a0, 0x674
CHAR_VTABLE, PLAYER_VTABLE = 0x5a7848, 0x5b4f30
CLASS_PLAYER, CLASS_CHARACTER = 0x0b, 0x0c
CHARDATA_SIZE = 0x600
CD_DAMAGEMODS, CD_COMBATRANGEMAX, CD_ARMOR = 0xe4, 0x15c, 0x1c4

# TActionBlock (§6.2)
AB_SIZE, AB_CTOR = 100, 0x4da9f0                 # ctor thiscall (name, action), ret 8
AB = dict(action=0x00, name=0x04, frame=0x24, wait=0x28, angle=0x2c, moveangle=0x30,
          turnrate=0x34, target=0x38, obj=0x44, attack=0x48, impact=0x4c, damage=0x50,
          data=0x5c, flags=0x60)
# Retail's flag bits by meaning. Confirmed from SetRoot/SetDoing/SetDesired/
# UpdateAction, ForceCommand and Go/ResolveCombat (COMBAT_DOJO.md §5.4):
# firsttime, transition, priority, interrupt, nowaitdone, dontforce, stop
# (ResolveCombatMove 0x4c7f80), waitpivot, noroot, goto. The rest are
# the 1998 names shifted up one bit past the inserted one -- unconfirmed,
# so they keep a `?`, and a block that sets one shows up as a difference.
AB_FLAGS = {0x1: 'firsttime', 0x2: 'transition', 0x4: 'terminating?', 0x8: 'retail-0x8',
            0x10: 'priority', 0x20: 'interrupt', 0x40: 'nowaitdone', 0x80: 'dontforce',
            0x100: 'stop', 0x200: 'waitpivot', 0x400: 'noroot', 0x800: 'loop?',
            0x1000: 'goto'}

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
OBJSTAT_IDS = {'health': (0x66ca4c, 0x101), 'fatigue': (0x66ca3c, 0x102), 'mana': (0x66ca38, 0x103)}
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
IM_HEADER, HDR_STATES, STATE_SIZE, ST_FRAMES = 0x04, 0x54, 0x4c, 0x32
GET_STAT = 0x4d74d0                              # slot 0xd8, thiscall (id), ret 4
GET_OBJSTAT = {CLASS_CHARACTER: 0x4d7520, CLASS_PLAYER: 0x51ae30}


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
        b.add(SET_STATE, 'SetState', 4, self._set_state)
        self._imagery_stubs()
        self.reset()

    def reset(self):
        self.objects = {}          # guest address -> spec name
        self.by_name = {}
        self.states = {}           # guest address -> [state names]
        self.stats = {}            # guest address -> {object stat id: value}
        self.classstats = {}       # guest address -> {type stat id: value}
        self.imagery = {}          # stand-in imagery address -> character
        self.blocks = {}           # guest address -> label of a block built by the fixture
        self.seams = []

    # -- seams ------------------------------------------------------------
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
        if sid not in stats:
            raise KeyError(f'{self._name(ecx)} has no {names.get(sid, sid)} in the case')
        self.seams.append(dict(seam=seam, who=self._name(ecx), stat=names.get(sid, sid), result=stats[sid]))
        return stats[sid]

    def _get_objstat(self, args, ecx):
        return self._stat('GetObjStat', self.stats, OBJSTAT_IDS, args, ecx)

    def _get_stat(self, args, ecx):
        return self._stat('GetStat', self.classstats, CLASSSTAT_IDS, args, ecx)

    def _set_state(self, args, ecx):
        index = s32(args[0])
        table = self.states.get(ecx, [])
        name = table[index]['name'] if 0 <= index < len(table) else None
        self.vm.write(ecx + O_STATE, struct.pack('<h', index))
        self.seams.append(dict(seam='SetState', who=self._name(ecx), state=index, name=name))
        return 1

    def _imagery_stubs(self):
        """One block of `ret` stubs for the stand-in imagery's vtable, one
        hook over all of it; the slot is the entry's offset."""
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP
        vm = self.vm
        stubs = vm.allocate(IMAGERY_SLOTS * 16)
        vm.write(stubs, b'\xc3' * (IMAGERY_SLOTS * 16))
        self.imagery_vtable = vm.allocate(IMAGERY_SLOTS * 4)
        for i in range(IMAGERY_SLOTS):
            vm.put_u32(self.imagery_vtable + 4 * i, stubs + 16 * i)
        answers = {0x3c: ('NumStates', 0, self._im_numstates), 0x8c: ('GetAniFlags', 4, self._im_aniflags)}

        def enter(uc, address, size, user):
            slot = (address - stubs) // 16 * 4
            sp = uc.reg_read(UC_X86_REG_ESP)
            args = struct.unpack('<4I', uc.mem_read(sp + 4, 16))
            owner = self.imagery.get(uc.reg_read(UC_X86_REG_ECX))
            try:
                if slot not in answers:
                    raise RuntimeError(f'imagery slot {slot:#x} reached for {self._name(owner)} (not modelled)')
                name, pop, fn = answers[slot]
                result = fn(owner, args)
            except Exception as error:
                vm.error = error
                uc.emu_stop()
                return
            self.seams.append(dict(seam=f'imagery.{name}', who=self._name(owner),
                                   args=[s32(a) for a in args[:pop // 4]], result=result))
            uc.reg_write(UC_X86_REG_EAX, result & 0xffffffff)
            uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4 + pop)

        vm.uc.hook_add(UC_HOOK_CODE, enter, begin=stubs, end=stubs + IMAGERY_SLOTS * 16 - 1)

    def _im_numstates(self, owner, args):
        return len(self.states[owner])

    def _im_aniflags(self, owner, args):
        index = s32(args[0])
        table = self.states[owner]
        return table[index]['aniflags'] if 0 <= index < len(table) else 0

    def _new_imagery(self, obj, table):
        vm = self.vm
        imagery = vm.allocate(0x40)
        header = vm.allocate(0x60)
        states = vm.allocate(max(1, len(table)) * STATE_SIZE)
        vm.put_u32(imagery, self.imagery_vtable)
        vm.put_u32(imagery + IM_HEADER, header)
        vm.put_u32(header + HDR_STATES, states)
        for i, st in enumerate(table):
            vm.write(states + i * STATE_SIZE + ST_FRAMES, struct.pack('<h', st['frames']))
        self.imagery[imagery] = obj
        return imagery

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
        vm.put_u32(obj + O_OUT_OF_SIGHT, int(spec.get('out_of_sight', 0)))
        if player:
            vm.put_u32(obj + O_PLAYERSTATE, spec.get('playerstate', 0))
            vm.put_u32(G_PLAYER, obj)
        name = vm.allocate(len(spec['name']) + 1)
        vm.write(name, spec['name'].encode('cp1252') + b'\0')
        vm.put_u32(obj + O_NAME, name)
        vm.put_u32(obj + O_ID, spec.get('id', 0))
        cd = vm.allocate(CHARDATA_SIZE)
        chardata = spec.get('chardata', {})
        vm.put_u32(cd + CD_COMBATRANGEMAX, chardata.get('combatrangemax', 0))
        vm.put_u32(cd + CD_ARMOR, chardata.get('armor', 0))
        for i, v in enumerate(chardata.get('damagemods', [])):
            vm.put_u32(cd + CD_DAMAGEMODS + 4 * i, v & 0xffffffff)
        vm.put_u32(obj + O_CHARDATA, cd)
        self.objects[obj] = spec['name']
        self.by_name[spec['name']] = obj
        table = [dict(name=st, frames=10, aniflags=0) if isinstance(st, str)
                 else dict(dict(frames=10, aniflags=0), **st) for st in spec.get('states', [])]
        self.states[obj] = table
        vm.put_u32(obj + O_IMAGERY, self._new_imagery(obj, table))
        vm.write(obj + O_STATE, struct.pack('<h', spec.get('state', 0)))
        self.stats[obj] = {OBJSTAT_IDS[k][1]: v for k, v in spec.get('stats', {}).items()}
        self.classstats[obj] = {CLASSSTAT_IDS[k][1]: v for k, v in spec.get('classstats', {}).items()}
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

    # -- results ----------------------------------------------------------
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
