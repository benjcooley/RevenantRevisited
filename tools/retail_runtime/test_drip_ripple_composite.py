"""Linked native/production lifecycle regression, including a broken-child challenge."""
import copy
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import zipfile

import drip_ripple_probe as probe


class DripRippleComposite(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        image = Path(os.environ.get('RETAIL_RUNTIME_EXE',
            str(probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe')))
        archive = Path(os.environ.get('RETAIL_RUNTIME_ARCHIVE',
            str(probe.ROOT / 'data/imagery.rvi')))
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        output = Path(cls.temp.name)
        with zipfile.ZipFile(archive) as z:
            drip = z.read('Imagery/Magic/drip.i3d')
            ripple = z.read('Imagery/Magic/ripples.i3d')
        if probe.sha(drip) != probe.DRIP_SHA or probe.sha(ripple) != probe.RIPPLE_SHA:
            raise AssertionError('Regression requires the verified shipped assets')
        cls.fixture = probe.CompositeFixture(image, drip, ripple)
        cls.native = []
        cls.native_draws = {}
        for tick in range(probe.TICKS + 1):
            if tick:
                cls.fixture.step()
            cls.native.append(cls.fixture.state())
            if tick in (26, 45, 120, 141):
                cls.native_draws[tick] = cls.fixture.draws()
        inputs = output / 'random.txt'
        inputs.write_text(''.join(f'{a} {b} {c} {d}\n'
            for a, b, c, d in cls.fixture.parent.random))
        cls.port, _ = probe.build_port(output, drip, ripple, inputs)

        # Compile an isolated mutation of the actual producer. Native input and
        # the real child factory/render still run, but its link stops advancing.
        # A packet-only comparison can miss this; full lifecycle comparison must
        # reject the first slipped age and the missing removals.
        pieces = probe.production_spans()
        needle = 'for (auto& ripple : spawned_ripples_) ripple->Advance(seconds);'
        body = pieces['drip_advance_submit']
        if body.count(needle) != 1:
            raise AssertionError('Expected one actual Drip child advancement site')
        pieces['drip_advance_submit'] = body.replace(needle,
            'for (auto& ripple : spawned_ripples_) (void)ripple;')
        broken_dir = output / 'broken-link'
        broken_dir.mkdir()
        with patch.object(probe, 'production_spans', return_value=pieces):
            cls.broken, _ = probe.build_port(broken_dir, drip, ripple, inputs)

    def test_full_linked_ages_births_and_removals(self):
        self.assertEqual(len(self.port), 145)
        for tick, native in enumerate(self.native):
            with self.subTest(tick=tick):
                self.assertEqual(probe.state_errors(native, self.port[tick]), [])
        self.assertEqual(sum(len(x['children']) for x in self.native), 105)
        self.assertEqual([x['tick'] for x in self.native[-1]['births']],
                         [24, 48, 72, 96, 120])
        self.assertEqual([x['tick'] for x in self.native[-1]['deaths']],
                         [45, 69, 93, 117, 141])
        for child_id in range(1, 6):
            ages = [c['frameon'] for state in self.native
                    for c in state['children'] if c['id'] == child_id]
            self.assertEqual(ages, list(range(21)))
        self.assertEqual(self.native[-1]['children'], [])

    def test_broken_child_advance_is_detected_before_render(self):
        self.assertEqual(probe.state_errors(self.native[24], self.broken[24]), [])
        self.assertIn('children', probe.state_errors(self.native[25], self.broken[25]))
        self.assertIn('deaths', probe.state_errors(self.native[45], self.broken[45]))
        self.assertNotEqual(self.broken[144]['children'], [])

    def test_mixed_authored_textures_and_final_empty_frame(self):
        for tick, draws in self.native_draws.items():
            with self.subTest(tick=tick):
                self.assertEqual(self.fixture.pixels(draws),
                                 self.fixture.pixels(self.port[tick]['draws']))
        self.assertEqual([x['texture'] for x in self.native_draws[26]], [1, 2])
        self.assertEqual(self.native_draws[141], [])
        color, _ = self.fixture.pixels(self.native_draws[26])
        self.assertTrue(any(color))

    def test_parent_cardinality_cannot_silently_truncate(self):
        truncated = copy.deepcopy(self.port[26])
        truncated['parent']['values'].pop()
        with self.assertRaisesRegex(AssertionError, 'cardinality'):
            probe.state_errors(self.native[26], truncated)


if __name__ == '__main__':
    unittest.main()
