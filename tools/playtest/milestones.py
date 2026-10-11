"""Milestone lists: the objectives a session works through, in order.

Each milestone tells Jev what to make progress on and says, from the
observation, when it is done; `save` makes the director quick-save and keep
the slot. The Keep chain follows docs/gameflow/STORY_TESTING.md §5 and the
manual's "Starting the quest"; its checks are tightened as real sessions
show what each moment looks like.
"""
from __future__ import annotations

import math

from director import Milestone, after_scene, character_dead, control_on, near, talked_to

KEEP = [
    Milestone('opening',
              'Watch the opening scene in the Keep. When Locke is asked questions, ask who these people '
              'are, who Locke is and why he was brought here.',
              lambda obs, d: bool(d.choices_taken) and control_on(obs, d)),
    Milestone('rahul defeated',
              'A crazed priest, Rahul, attacks Locke in the summoning chamber. Fight him until he falls.',
              after_scene(character_dead('Rahul')), save=True),
    Milestone('at rand',
              'Explore the Keep and find the guard Rand, held in the dungeon below.',
              lambda obs, d: any((o.get('name') or '').lower() == 'randk'
                                 and math.hypot(o['pos'][0] - obs['player']['pos'][0],
                                                o['pos'][1] - obs['player']['pos'][1]) <= 120
                                 for o in obs.get('objects', [])),
              save=True, goal='RandK'),
    Milestone('rand questioned',
              'Explore the Keep. Talk to the people there, and find the guard Rand in the dungeon below '
              'to question him about the cult.',
              after_scene(talked_to('Rand')), save=True, goal='RandK'),
]

# The reference host's toy world (fakehost.py): one person to reach.
SMOKE = [
    Milestone('met sardok', 'Find Sardok and go to him.', near((1000, 1800), 110), save=True),
    Milestone('explored', 'Leave Sardok and explore unexplored ground.',
              lambda obs, d: not near((1000, 1800), 200)(obs, d), save=True),
]

LISTS = {'keep': KEEP, 'smoke': SMOKE}
