"""Milestone lists: the objectives a session works through, in order.

Each milestone tells Jev what to make progress on and says, from the
observation, when it is done; `save` makes the director quick-save and keep
the slot. The Keep chain follows docs/gameflow/STORY_TESTING.md §5 and the
manual's "Starting the quest"; its checks are tightened as real sessions
show what each moment looks like.
"""
from __future__ import annotations

from director import Milestone, character_dead, control_on, level_is, near

KEEP = [
    Milestone('opening',
              'Watch the opening scene in the Keep. When Locke is asked questions, ask who these people '
              'are, who Locke is and why he was brought here.',
              lambda obs, d: bool(d.choices_taken) and control_on(obs, d)),
    Milestone('rahul defeated',
              'A crazed priest, Rahul, attacks Locke in the summoning chamber. Fight him until he falls.',
              character_dead('Rahul'), save=True),
    Milestone('keep explored',
              'Explore the Keep. Talk to the people there, and find the guard Rand in the dungeon below '
              'to question him about the cult.',
              level_is(2)),
]

# The reference host's toy world (fakehost.py): one person to reach.
SMOKE = [
    Milestone('met sardok', 'Find Sardok and go to him.', near((1000, 1800), 110), save=True),
]

LISTS = {'keep': KEEP, 'smoke': SMOKE}
