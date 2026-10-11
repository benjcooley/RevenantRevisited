#!/usr/bin/env python3
"""The play slot on the booted game: the tick, determinism, images.

Slow: the first run boots the software machine (about 2 minutes, then
cached); each case starts a new game (seconds) and runs ticks.

    "$PY" -m unittest discover -s tools/retail_runtime/slots/play -p 'test_playworld.py'
"""
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parents[1]))
from unicorn import UC_HOOK_CODE  # noqa: E402
from build_contract import BASELINE  # noqa: E402
from playworld import PlayWorld  # noqa: E402
import observe  # noqa: E402

DRAWN = 0x00491766           # TimerLoop: the drawing pass (clears TickOccurred, then TimerTick(1))
SKIPPED = 0x00491447         # TimerLoop: after a skipped frame's TimerTick(0)
TICKS = 30


def started(seed=1):
    world = PlayWorld.boot(BASELINE)
    world.start(seed=seed)
    return world


class TickTests(unittest.TestCase):
    def test_one_tick_is_one_drawn_timerloop_frame(self):
        world = started()
        passes = {DRAWN: 0, SKIPPED: 0}

        def count(uc, address, size, user):
            passes[address] += 1
        for address in passes:
            world.vm.uc.hook_add(UC_HOOK_CODE, count, begin=address, end=address)
        for _ in range(TICKS):
            frame, drawn = world.frame, passes[DRAWN]
            world.step()
            self.assertEqual((world.frame - frame, passes[DRAWN] - drawn), (1, 1))
        self.assertEqual(passes[SKIPPED], 0)
        self.assertEqual(world.tick, TICKS)

    def test_same_start_and_input_same_game_and_images_continue_it(self):
        events = {3: [{'key': 0x25, 'down': True}], 9: [{'key': 0x25, 'down': False}],
                  12: [{'mouse': [300, 200], 'buttons': 0}]}

        def run(world, ticks, checks):
            while world.tick < ticks:
                world.step(events.get(world.tick, []))
                checks.append((world.tick, observe.check(world), world.front_buffer()))
            return checks
        first = run(started(), TICKS, [])
        second_world = started()
        second = run(second_world, TICKS // 2, [])
        with tempfile.TemporaryDirectory() as temp:
            image = Path(temp) / 'half.image'
            second_world.save(image)
            loaded = PlayWorld.load(BASELINE, image)
        self.assertEqual(loaded.tick, TICKS // 2)
        run(loaded, TICKS, second)
        self.assertEqual([c[:2] for c in first], [c[:2] for c in second])
        self.assertEqual([c[2] for c in first], [c[2] for c in second])      # frames, byte for byte
        self.assertGreater(first[-1][1]['draws'], 0)


if __name__ == '__main__':
    unittest.main()
