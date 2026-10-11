#!/usr/bin/env python3
"""Click to walk in retail: the `clickwalk` build's left-button release on
open ground, run as original code (docs/playtest/CLICKWALK.md;
recon/retail_asm/patches/clickwalk).

The fixture drives the original TMapPane::MouseClick `0x0044f140` (thiscall
(button, x, y), ret 0xc) on the map pane `0x006668d8`: a press (1) and a
release (4), or the right button's press (2) for the walk it already had.
Locke is a TPlayer built by the combat slot's CombatWorld
(`slots/combat/guest.py`: the real vtable, action blocks by the original
constructor, his states and stats from the case), so Goto `0x004cedb0` and
Go `0x004ce350` run as original with the combat slot's seams.

Worlds:
- `crt` (default): CRT heap/TLS init, the angle tables and the stat ids, as
  the combat slot starts. About 10 ms.
- `hud`: the HUD slot's machine (`slots/hud/hudworld.py`): the shipped game
  booted by its own code to the end of game init, then the same fixture.
  About 95 s per executable the first time; the boot image is cached.

Either executable: the unchanged baseline, or a named build whose record
passes the build contract (`build_contract.verify_build`).

Seams (recorded in order in `seams`):
- OnObject `0x00452520` (thiscall (x, y, with), ret 0xc): the case's `on`,
  one answer per call in order (null = open ground, else a character's
  name). The pick itself (the 3D probe and the 2D walk) is MAP_INPUT.md §2.
- Observed, then run as original: ScreenToWorld `0x0046dad0` (cdecl (sx,
  sy, S3DPoint*, z)) and Goto `0x004cedb0` (thiscall (x, y, pickup)).
- Combat and bow, recorded: ButtonAttack `0x004d2480`, SetFighting
  `0x004d4790`, IsEnemy `0x004c89c0`, IsBowDrawn `0x004d1050` (the case's
  `bow_drawn`), DrawBow `0x004d0aa0`, AimBow `0x004d0c70`, ShootBow
  `0x004d0fd0`.
- The cursor: the bitmap lookup `0x0046d710`, SetMouseBitmap `0x0043a020`,
  SetMouseShadow `0x0043a090`, the corner bitmap `0x0043a0b0` (the
  right-button walk), the drag bitmap, name and object `0x0043a100` /
  `0x0043a170` / `0x0043a140` (cleared at MouseClick's exit), and a
  stand-in current screen (`0x00667fd0`) whose key slot (+0x30) records the
  numpad key UpdateMouseMovement presses.
- From combat_call: FindClearPath, FindCharacters, SetPlayerState,
  CanSeeCharacter, LineOfSight, and CombatWorld's own (FindState, stats,
  random, SetState, ...). CombatWorld answers MouseClick itself for the
  player's Stop; here MouseClick runs as original.

Run the standard cases on both builds and compare:

    clickwalk_probe.py --baseline EXE --clickwalk EXE --build build.json [--world hud]
"""
from __future__ import annotations

import argparse
import copy
import json
import struct
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE / 'slots' / 'combat'))
from build_contract import verify_build  # noqa: E402
from fixturekit import CRT_INIT, CRT_INIT_STOP, Boundaries, s32  # noqa: E402
from runtime import Runtime  # noqa: E402
from unicorn import UC_HOOK_CODE  # noqa: E402
import guest  # noqa: E402
from guest import CombatWorld, O_CHARDATA, PLAY_MOUSE_CLICK, call, write_fields  # noqa: E402
from combat_call import (CAN_SEE, FIND_CHARACTERS, FIND_CLEAR_PATH, LINE_OF_SIGHT,  # noqa: E402
                         SET_PLAYER_STATE, CallFixture)
from data_parse import CHAR_FIELDS  # noqa: E402

SCHEMA = 'clickwalk.v1'

# TMapPane (MAP_INPUT.md §1, §4): the global pane and the fields MouseClick reads.
MAP_PANE, MOUSE_CLICK = 0x006668d8, PLAY_MOUSE_CLICK
MP = dict(x=0x04, y=0x08, w=0x0c, h=0x10, posx=0x78, posy=0x7c, onobject=0xf8, walking=0x11c,
          clicked=0x120, lastkey=0x128, cursor=0x12c)
G_EDITOR, G_INFOMODE = 0x00668154, 0x0065c9e0
G_CURSORX, G_CURSORY, G_SCREEN = 0x00668510, 0x00668514, 0x00667fd0
MB = dict(left_down=1, right_down=2, left_up=4, right_up=5)
CURSOR_NONE, CURSOR_DOOR, CURSOR_SWORDS = -1, 3, 6

ON_OBJECT = 0x00452520
CAVE = 0x005a2a40                                 # the clickwalk build's code (zero padding in the baseline)
SCREEN_TO_WORLD, GOTO = 0x0046dad0, 0x004cedb0
SET_RUN_MODE = 0x004cf490                         # TCharacter::SetRunMode, thiscall ()
COMBAT_SEAMS = ((0x004d2480, 'ButtonAttack', 4), (0x004d4790, 'SetFighting', 4), (0x004c89c0, 'IsEnemy', 4),
                (0x004d0aa0, 'DrawBow', 0), (0x004d0c70, 'AimBow', 4), (0x004d0fd0, 'ShootBow', 4))
IS_BOW_DRAWN = 0x004d1050
CURSOR_SEAMS = ((0x0043a020, 'SetMouseBitmap', 0), (0x0043a090, 'SetMouseShadow', 0),
                (0x0043a0b0, 'SetMouseCornerBitmap', 0),
                # MouseClick's exit inside the pane lets go of the drag (0x00450499..)
                (0x0043a100, 'SetDragBitmap', 0), (0x0043a170, 'SetDragName', 0), (0x0043a140, 'SetDragObject', 0))
BITMAP_LOOKUP = 0x0046d710                        # TBitmapArchive lookup, thiscall (name), ret 4


def screen_to_world(sx, sy, z):
    """ScreenToWorld `0x0046dad0`, independently: the screen row raised by
    z * 867 / 1000, then the 2:1 diamond. C division truncates toward zero."""
    def div(a, b):
        q = abs(a) // abs(b)
        return q if (a >= 0) == (b >= 0) else -q
    if z:
        sy += div(z * 867, 1000)
    half = div(sx, 2)
    return [sy + half, sy - half, z]


def import_hudworld():
    """slots/hud's HudWorld. Its modules sit first on the path while it loads
    (its `profile` is not the standard library's), then leave."""
    hud = str(HERE / 'slots' / 'hud')
    saved = sys.modules.pop('profile', None)
    sys.path.insert(0, hud)
    try:
        from hudworld import HudWorld
    finally:
        sys.path.remove(hud)
        sys.modules.pop('profile', None)
        if saved is not None:
            sys.modules['profile'] = saved
    return HudWorld


def start(executable, build=None, world='crt'):
    """A machine on `executable` (the baseline, or a named build that passes
    the build contract) ready for the combat slot's world: (vm, record, hud)."""
    record = verify_build(executable, build)
    hud = None
    if world == 'hud':
        hud = import_hudworld().open(executable, build=build)
        vm = hud.vm
    else:
        vm = Runtime(executable)
        vm.call(CRT_INIT, stop_address=CRT_INIT_STOP)
        vm.call(guest.ANGLE_TABLES[0], stop_address=guest.ANGLE_TABLES[1], instruction_limit=20_000_000)
    # The combat slot's fixed stat ids (its stats are keyed by them).
    for address, sid in list(guest.OBJSTAT_IDS.values()) + list(guest.CLASSSTAT_IDS.values()):
        vm.put_u32(address, sid)
    guest._watch_faults(vm)
    return vm, record, hud


class ClickFixture(CallFixture):
    """combat_call's fixture (its Go seams and case objects) with the map
    pane's MouseClick running as original."""

    def __init__(self, executable, build=None, world='crt'):
        started = time.perf_counter()
        self.vm, self.record, self.hud = start(executable, build, world)
        self.sha = self.record['executable_sha256']
        self.world = CombatWorld(self.vm)
        b = self.world.boundaries
        b.add(FIND_CLEAR_PATH, 'FindClearPath', 0x14, self._find_clear_path)
        b.add(FIND_CHARACTERS, 'FindCharacters', 0x18, self._find_characters)
        b.add(SET_PLAYER_STATE, 'SetPlayerState', 4, self._set_player_state)
        b.add(CAN_SEE, 'CanSeeCharacter', 8, self._can_see)
        b.add(LINE_OF_SIGHT, 'LineOfSight', 0x14, self._line_of_sight)
        # CombatWorld answers MouseClick for the player's Stop; here it runs.
        b.handlers[MOUSE_CLICK] = ('PlayMouseClick', 0xc, lambda args, ecx: Boundaries.ORIGINAL)
        b.add(ON_OBJECT, 'OnObject', 0xc, self._on_object)
        b.add(SCREEN_TO_WORLD, 'ScreenToWorld', 0, self._screen_to_world)
        b.add(GOTO, 'Goto', 0xc, self._goto)
        for address, name, pop in COMBAT_SEAMS:
            b.add(address, name, pop, (lambda n: lambda args, ecx: self._record(n, args, ecx))(name))
        b.add(IS_BOW_DRAWN, 'IsBowDrawn', 0, self._is_bow_drawn)
        for address, name, pop in CURSOR_SEAMS:
            b.add(address, name, pop, (lambda n: lambda args, ecx: self._record(n, args, ecx, 0))(name))
        b.add(BITMAP_LOOKUP, 'Bitmap', 4, self._bitmap)
        self.screen = self.vm.allocate(0x40)
        screen_vtable = self.vm.allocate(0x100)
        self.vm.put_u32(self.screen, screen_vtable)
        self.vm.put_u32(screen_vtable + 0x30, b.add_stub('Screen.KeyPress', 8, self._key_press))
        self.bitmaps = {}
        # Entries into the clickwalk code; the baseline never runs there.
        self.cave_entries = 0
        self.vm.uc.hook_add(UC_HOOK_CODE, self._cave, begin=CAVE, end=CAVE)
        if self.hud is not None:
            self.hud.checkpoint()
        else:
            self.vm.checkpoint()
        self.setup_ms = (time.perf_counter() - started) * 1000
        self.case = {}

    # -- seams ------------------------------------------------------------
    def _cave(self, uc, address, size, user):
        self.cave_entries += 1

    def _on_object(self, args, ecx):
        answers = self.case.get('on', [])
        index = sum(1 for s in self.world.seams if s['seam'] == 'OnObject')
        name = answers[index] if index < len(answers) else None
        self.world.seams.append(dict(seam='OnObject', x=s32(args[0]), y=s32(args[1]),
                                     held=self.world._name(args[2]) if args[2] else None, result=name))
        return self.world.by_name[name] if name else 0

    def _screen_to_world(self, args, ecx):
        sx, sy, z = s32(args[0]), s32(args[1]), s32(args[3])
        self.world.seams.append(dict(seam='ScreenToWorld', sx=sx, sy=sy, z=z, point=screen_to_world(sx, sy, z)))
        return Boundaries.ORIGINAL

    def _goto(self, args, ecx):
        self.world.seams.append(dict(seam='Goto', who=self.world._name(ecx), x=s32(args[0]), y=s32(args[1]),
                                     pickup=self.world._name(args[2]) if args[2] else None))
        return Boundaries.ORIGINAL

    def _record(self, name, args, ecx, result=0):
        self.world.seams.append(dict(seam=name, who=self.world._name(ecx), arg=s32(args[0])))
        return result

    def _is_bow_drawn(self, args, ecx):
        drawn = int(bool(self.case.get('bow_drawn', 0)))
        self.world.seams.append(dict(seam='IsBowDrawn', who=self.world._name(ecx), result=drawn))
        return drawn

    def _bitmap(self, args, ecx):
        name = self.vm.string(args[0])
        if name not in self.bitmaps:
            self.bitmaps[name] = self.vm.allocate(0x48)
        self.world.seams.append(dict(seam='Bitmap', name=name))
        return self.bitmaps[name]

    def _key_press(self, args, ecx):
        self.world.seams.append(dict(seam='Screen.KeyPress', key=s32(args[0]), down=s32(args[1])))
        return 0

    # -- a case -----------------------------------------------------------
    def run(self, case):
        vm, world = self.vm, self.world
        if self.hud is not None:
            self.hud.restore()
        else:
            vm.restore()
        world.reset()
        self.bitmaps = {}
        self.case = case
        world.set_globals(case.get('globals', {}))
        world.set_rng(case)
        world.set_ground(case)
        for spec in case['chars']:
            world.new_character(spec)
        for spec in case['chars']:
            obj = world.by_name[spec['name']]
            world.set_blocks(obj, spec)
            write_fields(vm, vm.u32(obj + O_CHARDATA), CHAR_FIELDS, spec.get('chardata', {}))
            world.set_memory(obj, spec)
        me = world.by_name[case['self']]
        pane = dict(x=0, y=0, w=640, h=480, onobject=-1, walking=0, clicked=0, lastkey=-1, cursor=CURSOR_NONE)
        pane.update(case['pane'])
        for field, value in pane.items():
            vm.put_u32(MAP_PANE + MP[field], value & 0xffffffff)
        vm.put_u32(G_EDITOR, 0)
        vm.put_u32(G_INFOMODE, 0)
        vm.put_u32(G_SCREEN, self.screen)
        world.seams.clear()
        for name in case.get('pre', []):            # the run key's SetRunMode, as Command 0x4a calls it
            if name != 'SetRunMode':
                raise ValueError(f'unknown pre step {name!r}')
            world.seams.append(dict(seam='pre', call=name,
                                    returned=s32(call(vm, SET_RUN_MODE, (), this=me, instruction_limit=20_000_000))))
        steps = []
        for button, x, y in case['clicks']:
            # The pointer the screen loop polled (GetCursorPos) is where the
            # click is: the map pane sits at the screen's (0, 0).
            vm.put_u32(G_CURSORX, x + pane['x'])
            vm.put_u32(G_CURSORY, y + pane['y'])
            first, entries = len(world.seams), self.cave_entries
            call(vm, MOUSE_CLICK, (MB[button], x & 0xffffffff, y & 0xffffffff), this=MAP_PANE,
                 instruction_limit=50_000_000)
            steps.append(dict(button=button, x=x, y=y, seams=world.seams[first:],
                              clickwalk_code=self.cave_entries - entries,
                              pane={k: s32(vm.u32(MAP_PANE + MP[k])) for k in ('onobject', 'clicked', 'walking')}))
        new_blocks = []
        return dict(schema=SCHEMA, case=case['name'], build=self.record['name'], sha=self.sha,
                    pre=[s for s in world.seams if s['seam'] == 'pre'], steps=steps,
                    self=world.character_dump(me, new_blocks), draws=list(world.draws))


# ---- the standard cases --------------------------------------------------

LOCKE_AT = [6280, 4338, 33]            # PLAN.md §5's example position
WALK_STATES = ['walk', 'walkf', 'walkl', 'walkr', 'walk to walkf', 'pivotl', 'pivotr']
RUN_STATES = ['run', 'runf', 'runl', 'runr', 'run to runf', 'walk to run', 'run to walk']
COMBAT_STATES = ['combat', 'combatf', 'combat to combatf']
BOW_STATES = ['bow', 'bowf']


def scroll_for(pos, at=(320, 240)):
    """posx, posy putting `pos` at screen point `at` (WorldToScreen: x - y,
    (x + y) / 2 - z * 867 / 1000)."""
    x, y, z = pos
    return dict(posx=x - y - at[0], posy=(x + y) // 2 - (z * 867) // 1000 - at[1])


def locke(root, states, facing=64, health=100, **extra):
    spec = dict(name='Locke', type='Locke', **{'class': 11}, pos=list(LOCKE_AT), facing=facing,
                moveangle=facing, stats=dict(health=health), classstats=dict(radius=16),
                chardata=dict(combatrangemax=300), states=states, root=root)
    spec.update(extra)
    return spec


def npc(name='Tendrick', at=(6380, 4338, 33)):
    return dict(name=name, type=name, **{'class': 12}, pos=list(at), facing=192, moveangle=192, id=1712,
                stats=dict(health=100), classstats=dict(radius=16), chardata=dict(combatrangemax=300),
                states=['walk'], root=dict(name='walk', action=1, angle=192, moveangle=192))


def click(x, y):
    return [['left_down', x, y], ['left_up', x, y]]


def cases():
    walk_root = dict(name='walk', action=1, angle=64, moveangle=64)
    pane = scroll_for(LOCKE_AT)
    ahead = (400, 200)                  # north-east of Locke on screen: about his facing
    behind = (200, 330)                 # south-west: a turn first
    out = []

    def case(name, chars, clicks, **extra):
        out.append(dict(name=name, self='Locke', chars=chars, clicks=clicks, pane=dict(pane, **extra.pop('pane', {})),
                        globals=dict(combatface=1, frame=100), **extra))

    # The patched path: open ground, walk mode.
    case('open-ground', [locke(walk_root, WALK_STATES)], click(*ahead))
    case('open-ground-turn', [locke(walk_root, WALK_STATES)], click(*behind))
    # Run: the run key held is SetRunMode (Command 0x4a), then the click.
    case('open-ground-run', [locke(walk_root, WALK_STATES + RUN_STATES)], click(*ahead), pre=['SetRunMode'])
    # Unchanged paths, both builds alike.
    case('right-button', [locke(walk_root, WALK_STATES)], [['right_down', *ahead]])
    case('press-object-release-ground', [locke(walk_root, WALK_STATES), npc()], click(*ahead), on=['Tendrick', None])
    case('press-ground-release-object', [locke(walk_root, WALK_STATES), npc()], click(*ahead), on=[None, 'Tendrick'])
    case('drag-release', [locke(walk_root, WALK_STATES)], [['left_up', *ahead]])
    case('outside-pane', [locke(walk_root, WALK_STATES)], click(700, 200))
    case('dead', [locke(walk_root, WALK_STATES, health=0)], click(*ahead))
    combat_root = dict(name='combat', action=3, angle=64, moveangle=64)
    case('combat-attack-cursor', [locke(combat_root, COMBAT_STATES)], click(*ahead), tape=[1])
    case('combat-swords-cursor', [locke(combat_root, COMBAT_STATES)], click(*ahead), tape=[1],
         pane=dict(cursor=CURSOR_SWORDS))
    case('combat-stale-door-cursor', [locke(combat_root, COMBAT_STATES)], click(*ahead),
         pane=dict(cursor=CURSOR_DOOR))
    bow_root = dict(name='bow', action=0x19, angle=64, moveangle=64)
    case('bow-not-drawn', [locke(bow_root, BOW_STATES)], click(*ahead))
    case('bow-drawn', [locke(bow_root, BOW_STATES)], click(*ahead), bow_drawn=1)
    return out


WALKS = ('open-ground', 'open-ground-turn', 'open-ground-run')


def _without_entries(steps):
    return [{k: v for k, v in step.items() if k != 'clickwalk_code'} for step in steps]


def compare(baseline, clickwalk):
    """Per case: the expected difference, checked. Returns (rows, failures)."""
    rows, failures = [], []
    by_name = {r['case']: r for r in baseline}
    right = next(r for r in clickwalk if r['case'] == 'right-button')
    right_points = [s for s in right['steps'][0]['seams'] if s['seam'] == 'ScreenToWorld']
    for result in clickwalk:
        name = result['case']
        base = by_name[name]
        row = dict(case=name, clickwalk_code=sum(s['clickwalk_code'] for s in result['steps']))
        if any(s['clickwalk_code'] for s in base['steps']):
            failures.append(f'{name}: the baseline ran code at {CAVE:#x}')
        if name in WALKS:
            release = result['steps'][-1]['seams']
            gotos = [s for s in release if s['seam'] == 'Goto']
            points = [s for s in release if s['seam'] == 'ScreenToWorld']
            base_gotos = [s for st in base['steps'] for s in st['seams'] if s['seam'] == 'Goto']
            row.update(baseline_goto=len(base_gotos), clickwalk_goto=gotos, point=points)
            expect_point = points and points[0]['point']
            if base_gotos:
                failures.append(f'{name}: the baseline called Goto')
            if len(gotos) != 1 or len(points) != 1:
                failures.append(f'{name}: expected one ScreenToWorld and one Goto, got {len(points)}, {len(gotos)}')
            elif [gotos[0]['x'], gotos[0]['y']] != expect_point[:2] or gotos[0]['pickup'] is not None:
                failures.append(f'{name}: Goto {gotos[0]} is not the point {expect_point}')
            if name != 'open-ground-turn' and right_points and points and \
                    (points[0]['sx'], points[0]['sy'], points[0]['z']) != \
                    (right_points[0]['sx'], right_points[0]['sy'], right_points[0]['z']):
                failures.append(f'{name}: ScreenToWorld args differ from the right-button walk\'s')
            # Goto's target goes on the block Go queued (desired), or on the
            # one being done when desired is the root (0x004cedec..0x004cedfc).
            root, desired, doing = (result['self'][k] for k in ('root', 'desired', 'doing'))
            block = desired if desired and desired['id'] != root['id'] else doing
            row['walk_block'] = block
            if not block or block['target'][:2] != expect_point[:2] or 'walkto' not in block['flags']:
                failures.append(f'{name}: no walk block toward the point (desired {desired}, doing {doing})')
            if base['self'] == result['self']:
                failures.append(f'{name}: Locke unchanged by the click')
        else:
            same = _without_entries(base['steps']) == _without_entries(result['steps']) and \
                base['self'] == result['self']
            row['identical_to_baseline'] = same
            if not same:
                failures.append(f'{name}: differs from the baseline')
            if any(s['seam'] == 'Goto' for st in result['steps'] for s in st['seams']):
                failures.append(f'{name}: Goto called')
        rows.append(row)
    return rows, failures


def run_suite(fixture):
    return [fixture.run(copy.deepcopy(c)) for c in cases()]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--clickwalk', type=Path, required=True)
    parser.add_argument('--build', type=Path, required=True, help="the clickwalk build's build.json")
    parser.add_argument('--world', choices=('crt', 'hud'), default='crt')
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    results = {}
    for label, exe, build in (('baseline', args.baseline, None), ('clickwalk', args.clickwalk, args.build)):
        started = time.perf_counter()
        fixture = ClickFixture(exe, build, args.world)
        results[label] = dict(sha=fixture.sha, build=fixture.record['name'], setup_ms=round(fixture.setup_ms, 1),
                              results=run_suite(fixture), seconds=round(time.perf_counter() - started, 2))
        if fixture.hud is not None:
            results[label]['hud_boot'] = dict(
                configuration={name: fixture.vm.u32(a) for name, a in
                               (('NoTexOverlay', 0x006680c8), ('InternalRaster', 0x005d7a28),
                                ('TexturedDevice', 0x00669ad8))},
                boundaries=[list(b) for b in fixture.hud.boundary_calls[-4:]])
    rows, failures = compare(results['baseline']['results'], results['clickwalk']['results'])
    report = dict(schema=SCHEMA, world=args.world, rows=rows, failures=failures,
                  builds={k: {f: v for f, v in r.items() if f != 'results'} for k, r in results.items()})
    if args.out:
        args.out.write_text(json.dumps(dict(report, results=results), indent=1) + '\n')
    print(json.dumps(report, indent=1))
    sys.exit(1 if failures else 0)


if __name__ == '__main__':
    main()
