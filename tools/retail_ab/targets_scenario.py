"""The scenario kata for retail_ab.py (docs/gameplay/forensics/SCENARIO_KATA.md):
retail's whole game tick over a small world, tick after tick, against the
port's.

`combat-scenario` runs `ticks` ticks of TScreen::TimerTick `0x490bd0` as
far as it plays the game -- the player's keys, UpdateMove, the map pane's
PulseObjects and MoveObjects, the frame count and game time, NextFrameObjects
-- over two or three characters on a flat floor, every function the tick
reaches as original code (retail: tools/retail_runtime/slots/combat/
scenario.py; port: src/retailab_scenario.cpp, its own tick functions). Both
sides draw from the case's random tape. Each tick is compared whole: every
character's place, facing, move angle, state and frame, its three action
blocks by meaning (with the attack, impact, damage, to-hit and roll they
carry), its attack bookkeeping and motion fields, its vitals; the tick's
seam calls (what it set, made, played, asked of the map) and its random
draws. A difference names its tick.

The worlds are the arena's (docs/gameplay/COMBAT_ARENA.md), built in the
fixture rather than loaded: the shipped character data and rules (kata D1's
dump, as the melee katas take them), each character's whole state table
with its motion (cases/motion/<name>.full.json, the port's loader through
i3ddump), and the state the arena save holds (positions, facings, states
and frames, stats, the recovery stamps, the frame count from its game
time).

Families:
- `a1b`: arena test A1b. Locke idle in his walk root, an Araknid 150 east
  in its combat root with no target. It finds him by ear on its 32-frame
  window, turns, walks in, jumps, bites; Locke falls into combat on his own
  32-frame window (auto-combat). 400 ticks, a seed per case.
- `a1`: the Araknid 450 east, beyond its combat range (COMBATRANGE 214,
  IsValidTarget): it hears him and passes him over; nothing happens.
- `a4`: Locke in combat 60 from the Araknid, swinging (button 1, 2 or 3)
  on a fixed beat; with a Club too (a bludgeon's misc damage).
- `a3`: Locke in combat holding block while the Araknid comes for him.
- `a6`: the orbit: Locke in combat holding one direction, then letting go.
- `a7`: two Araknids.
- `a8`: the combat key: out of combat and back; refused in the run root.
"""
from __future__ import annotations

import json
from pathlib import Path

from combat_targets import _walk, finish, port_fields
from targets_melee import chardata, make_char, shipped
from targets_move import PURE

HERE = Path(__file__).resolve().parent
ANIMATE, COMBAT = 1, 3
BLOCK_KEY = 0x800                                 # the control map's block bit (UpdateMove 0x47de30)
COMBAT_ROOTS = ('combat', 'hand', 'cknife', 'cstaff', 'combatrun', 'handrun')   # the player's (TPlayer 0x51b340)

# The arena save ArenaA1b (tools/combatarena/make_arena_slot.py, the combat
# worktree's build/arena_slots/ArenaA1b; tools/savefmt/revsave.py dump):
# game time 294416 (frame 294416 * 24 / 100), Locke's and the Araknid's
# records.
GAMETIME = 294416
FRAME = GAMETIME * 24 // 100
FLOOR = 17
LOCKE_AT, ARAKNID_EAST = [25375, 3072, FLOOR], 150
LOCKE_ID, ARAKNID_ID = 1955009601, 268463788      # map indices (retail +0x40): the staggered checks' ids
LOCKE_STATS = dict(aggressive=1, poisoned=0, sleeping=0, health=43, fatigue=78, mana=10, damagemod=0, level=1,
                   exp=50, nextexp=300, attacklevel=0, healthpct=0, manapct=0, fatiguepct=0,
                   # Cloth shirt and pants: ARMOR.DEF protection 1 each. The fixture's player carries
                   # no items, so its ArmorValue (0x519850: ACBonus + the equipped armour's protection)
                   # takes them as ACBonus.
                   acbonus=2, edgebonus=0, manacostpct=0, spelldamageinc=0,
                   strn=18, cons=12, agil=14, rflx=14, mind=14, luck=14,
                   attack=0, defense=0, invoke=0, hands=0, knife=0, sword=0, bludgeons=0, axes=0, bows=0,
                   stealth=0, lockpick=0)
# Weapons as WEAPON.DEF has them over class.def (kata D1's dump): Type,
# Damage, DamageType -- 0 for every shipped weapon (TPlayer::GetDamageType
# 0x51b2f0: an armed player deals misc damage).
SHORT_SWORD = dict(name='Short Sword', type=2, damage=13, damagetype=0)
CLUB = dict(name='Club', type=3, damage=14, damagetype=0)
LOCKE_MAX = dict(maxhealth=100, maxfatigue=78, maxmana=105)   # level 1, Cons 12, Mind 14 (PLAYER_STATS.md §6)
ARAKNID_STATS = dict(aggressive=1, poisoned=0, sleeping=0, health=30, fatigue=104, mana=0, damagemod=0)
# The tick's own state as the save leaves it (melee_attack.py PULSESTATE);
# the rest is ClearChar's.
TICK_STATE = dict(lastrun=-1, accumhealth=0, accumfatigue=0, accummana=0, accumrun=0, accumpoison=0,
                  invokedelay=0, fade=100, fadestep=0, fadelimit=100, fadedir=0, burnticks=0, deadticks=0,
                  exittimestamp=0, forcecommanddone=0)
LOCKE_TICK = dict(TICK_STATE, lasthealth=294229, lastfatigue=294379, lastmana=294229, lastpoison=294329,
                  deathcountdown=0xc0, statetime=273029, stateframes=273029 * 24 // 100, stateffectexpires=-1)
ARAKNID_TICK = dict(TICK_STATE, lasthealth=294391, lastfatigue=294391, lastmana=294391, lastpoison=294391)


def _states(name: str) -> list[dict]:
    return json.loads((HERE / 'cases' / 'motion' / f'{name}.full.json').read_text())['states']


def _index(states: list[dict], name: str) -> int:
    return next(i for i, s in enumerate(states) if s['name'] == name)


def locke(data, *, root='walk', state='6:walk', frame=30, facing=51, at=LOCKE_AT, target=None, wielding=SHORT_SWORD,
          **extra):
    """Locke as the arena save has him: in his walk root (or `root`), the
    Short Sword (or `wielding`) in hand."""
    s = shipped(data, None)
    states = _states('locke')
    action = COMBAT if root in COMBAT_ROOTS else ANIMATE
    c = make_char('Locke', 11, at, facing, chardata(s['chars']['Locke']), typ='Locke', states=states,
                  root=dict(name=root, action=action, obj=target), stats=dict(LOCKE_STATS),
                  classstats=dict(radius=16, value=-1), weapon=dict(type=wielding['type'], damage=wielding['damage']),
                  frame=frame, ident=LOCKE_ID)
    c.update(state=_index(states, state), animator=1, pulsestate=dict(LOCKE_TICK), **LOCKE_MAX,
             wielding=dict(wielding))
    c.update(extra)
    return c


def araknid(data, *, east=ARAKNID_EAST, north=0, state='100:combat', frame=1, facing=240, target=None,
            name='Araknid', ident=ARAKNID_ID, health=None, **extra):
    """The arena's Araknid: in its combat root, no target; `east` / `north`
    of Locke, its own `health` for a longer fight."""
    s = shipped(data, None)
    states = _states('araknid')
    at = [LOCKE_AT[0] + east, LOCKE_AT[1] - north, LOCKE_AT[2]]
    stats = dict(ARAKNID_STATS, **({} if health is None else dict(health=health)))
    c = make_char(name, 12, at, facing, chardata(s['chars']['Araknid']), typ='Araknid',
                  states=states, root=dict(name='combat', action=COMBAT, obj=target),
                  stats=stats, classstats=dict(radius=14, value=1), frame=frame, ident=ident)
    c.update(state=_index(states, state), animator=1, pulsestate=dict(ARAKNID_TICK))
    c.update(extra)
    return c


def hold(direction, at, until):
    """Holding one direction key (the control map's bit `direction`: 32 *
    direction is the angle UpdateMove walks) from tick `at`, let go at
    `until`. The changed mask keeps every control that ever changed."""
    bit = 1 << direction
    return [dict(tick=at, controls=[bit, bit]), dict(tick=until, controls=[0, bit])]


def scenario(name, chars, rules, *, ticks, seed, inputs=(), frame=FRAME, gametime=GAMETIME, **extra):
    c = dict(name=f'scn.{name}', call='scenario', ticks=ticks, self=chars[0]['name'], chars=chars, rules=rules,
             globals=dict(combatface=1, frame=frame, gametime=gametime, screenframe=1000, autobegincombat=1,
                          control=1, ps_5d8=0, nahkranoth=0, alreadydead=0, editor=0, pvp=0),
             ground=dict(z=FLOOR), perception=True, seam_ai=False, seam_update=False, seam_damage=False,
             seam_block=False, seam_begin=False, seam_path=False, quiet_reads=True, inputs=list(inputs), tape=[], seed=seed)
    c.update(extra)
    return c


def scenario_cases(data: Path, workdir: Path) -> list[dict]:
    s = shipped(data, workdir)
    rules = s['rules']
    cases = []
    fighting = dict(root='combat', state='100:combat', frame=0, facing=64, target='Araknid')
    # A1b: the Araknid 150 east, Locke idle. Seeds as the arena's runs; one
    # run long enough for Locke to die, his corpse and the Araknid after.
    for seed in range(1, 5):
        cases.append(scenario(f'a1b.seed{seed}', [locke(data), araknid(data)], rules, ticks=400, seed=seed))
    cases.append(scenario('a1b.long', [locke(data), araknid(data)], rules, ticks=900, seed=5))
    # A1: 450 east, beyond its combat range (heard, passed over).
    cases.append(scenario('a1.seed1', [locke(data), araknid(data, east=450)], rules, ticks=200, seed=1))
    # A4: Locke in combat 60 from it, swinging on a beat (every 30 ticks, one
    # button a case), the Araknid on him; its health raised so the exchange
    # goes on (a first thrust takes the shipped 30).
    for button in (1, 2, 3):
        swings = [dict(tick=t, attack=button) for t in range(4, 400, 30)]
        cases.append(scenario(f'a4.button{button}', [
            locke(data, **fighting), araknid(data, east=60, facing=192, target='Locke', health=300)],
            rules, ticks=400, seed=10 + button, inputs=swings))
    # ... and with a Club: a bludgeon's blow is misc damage all the same,
    # which the Araknid doesn't resist (its bludgeon modifier is 20).
    cases.append(scenario('a4.club', [
        locke(data, **fighting, wielding=CLUB), araknid(data, east=60, facing=192, target='Locke', health=300)],
        rules, ticks=400, seed=14, inputs=[dict(tick=t, attack=2) for t in range(4, 400, 30)]))
    # A3: Locke in combat holding block from tick 2, the Araknid coming.
    cases.append(scenario('a3.block', [locke(data, **fighting), araknid(data, east=120, facing=192)], rules,
                          ticks=300, seed=21, inputs=[dict(tick=2, controls=[BLOCK_KEY, BLOCK_KEY])]))
    # A6: the orbit. Locke in combat 120 from it holding a direction (north:
    # round it; east: in; south; west: away) for 100 ticks, then letting go.
    for direction in (0, 2, 4, 6):
        cases.append(scenario(f'a6.hold{direction * 32}', [locke(data, **fighting), araknid(data, east=120, facing=192)],
                              rules, ticks=160, seed=31 + direction, inputs=hold(direction, 2, 102)))
    # A8: the combat key (TPlayScreen::Command case 1). Locke falls into
    # combat on his own as the Araknid comes, leaves it with the key at tick
    # 45 (auto-combat off while it finds someone), takes it up again at 120
    # (on whoever is ahead); and, standing in his run root, presses it with
    # no effect (BeginFighting refuses a runner) and takes the bites so.
    cases.append(scenario('a8.key', [locke(data), araknid(data)], rules, ticks=300, seed=51,
                          inputs=[dict(tick=45, command='combat'), dict(tick=120, command='combat')]))
    cases.append(scenario('a8.run', [locke(data, root='run', state='run', frame=0), araknid(data)], rules,
                          ticks=200, seed=52, inputs=[dict(tick=5, command='combat')]))
    # A7: two Araknids, 150 east and 150 east, 120 north; Locke idle.
    cases.append(scenario('a7.two', [locke(data), araknid(data),
                                     araknid(data, north=120, name='Araknid2', ident=ARAKNID_ID + 0x13, facing=176)],
                          rules, ticks=400, seed=41))
    return finish(cases)


def compare_scenario(case: dict, retail: dict, port: dict) -> list[dict]:
    """Tick by tick, each tick's record whole (the pure queries out of its
    seams, as M7 and M8 leave them); then the AI and tick state each
    character ends with."""
    def strip(tick):
        out = dict(tick)
        out['seams'] = [s for s in tick.get('seams', []) if not s.get('seam', '').startswith(PURE)]
        return out
    out = []
    rt, pt = retail.get('ticks', []), port.get('ticks', [])
    if len(rt) != len(pt):
        out.append(dict(where='ticks', line=None, field='tick count', retail=len(rt), port=len(pt)))
    for i, (r, p) in enumerate(zip(rt, pt)):
        _walk(strip(r), strip(p), f't{i}', out)
    _walk({k: retail.get(k) for k in ('ai', 'pulse')}, {k: port.get(k) for k in ('ai', 'pulse')}, 'end', out)
    return out


TARGETS = {
    'combat-scenario': dict(fixture='slots/combat/scenario.py', cases=scenario_cases, compare=compare_scenario,
                            port_fields=port_fields, unit=lambda r: len(r.get('ticks', []))),
}
