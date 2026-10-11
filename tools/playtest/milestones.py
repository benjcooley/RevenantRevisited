"""Milestone lists: the objectives a session works through, in order.

Each milestone tells Jev what to make progress on and says, from the
observation, when it is done; `save` makes the director quick-save and keep
the slot. The Keep chain follows docs/gameflow/STORY_TESTING.md §5 and the
manual's "Starting the quest"; its checks are tightened as real sessions
show what each moment looks like.
"""
from __future__ import annotations

import math

from director import Milestone, after_scene, character_dead, control_on, every, level_is, near, talked_to

KEEP = [
    Milestone('opening',
              'Watch the opening scene in the Keep. When Locke is asked questions, ask who these people '
              'are, who Locke is and why he was brought here.',
              lambda obs, d: bool(d.choices_taken) and control_on(obs, d), save=True,
              expect={'RAHULSTATE': 1}),                    # keep.s:660, SardokR sets Rahul on
    Milestone('rahul defeated',
              'A crazed priest, Rahul, attacks Locke in the summoning chamber. Fight him until he falls.',
              after_scene(character_dead('Rahul')), save=True,
              expect={'RAHULSTATE': 0}),                    # keep.s:683, Tendrick's scene
    Milestone('at rand',
              'Explore the Keep and find the guard Rand, held in the dungeon below.',
              lambda obs, d: any((o.get('name') or '').lower() == 'randk'
                                 and math.hypot(o['pos'][0] - obs['player']['pos'][0],
                                                o['pos'][1] - obs['player']['pos'][1]) <= 120
                                 for o in obs.get('objects', [])),
              save=True, goal='RandK', expect={'RAHULSTATE': 0}),
    Milestone('rand questioned',
              'Explore the Keep. Talk to the people there, and find the guard Rand in the dungeon below '
              'to question him about the cult.',
              after_scene(talked_to('Rand')), save=True, goal='RandK',
              expect={'RANDSTATE': 1, 'TENDRICKSTATE': 1}),   # keep.s:393-394
    # keep.s: TendrickT, told of Rand (TENDRICKSTATE 1), takes Locke to level 6
    # for GowE's scene, which brings him back to level 2 and unlocks KeepExit.
    Milestone('tendrick told',
              'Go back up through the Keep to Tendrick and tell him what Rand said.',
              after_scene(every(talked_to('TendrickT'), level_is(6))), save=True, goal='TendrickT',
              expect={'TENDRICKSTATE': 2}),                 # keep.s:1251, GowE
    Milestone('left the keep',
              'Leave the Keep by its gate, out into the world.',
              after_scene(level_is(0)), save=True, goal='KeepExit', expect={'TENDRICKSTATE': 2}),
]

# The reference host's toy world (fakehost.py): one person to reach.
SMOKE = [
    Milestone('met sardok', 'Find Sardok and go to him.', near((1000, 1800), 110), save=True),
    Milestone('explored', 'Leave Sardok and explore unexplored ground.',
              lambda obs, d: not near((1000, 1800), 200)(obs, d), save=True),
]

LISTS = {'keep': KEEP, 'smoke': SMOKE}
