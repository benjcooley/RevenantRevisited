"""Recursive native Ripple regression and opposing-sign truncation challenge."""
import copy
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import zipfile

import ripple_splash_probe as probe


class RippleSplashFamily(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        image = Path(os.environ.get('RETAIL_RUNTIME_EXE',
            str(probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe')))
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        output = Path(cls.temp.name)
        with zipfile.ZipFile(probe.ROOT / 'data/imagery.rvi') as z:
            asset = z.read('Imagery/Magic/ripples.i3d')
        cls.fixture = probe.RippleFamily(image, asset)
        cls.fixture.reset(96, (37, -29, 0))
        cls.native = [cls.fixture.state()]
        for _ in range(probe.TICKS):
            cls.fixture.step()
            cls.native.append(cls.fixture.state())
        cls.random = cls.fixture.random.copy()
        inputs = output / 'random.txt'
        inputs.write_text(''.join(f'{a} {b} {c} {d}\n'
            for a, b, c, d, _, _ in cls.random))
        cls.port, _ = probe.build_family(output, asset, inputs, 96, (37, -29, 0))

        # This common erroneous rewrite truncates local displacement before
        # adding the integer owner. Compile the actual producer with only that
        # isolated mutation and require the original native request to reject it.
        pieces = probe.production_spans()
        body = pieces['ripple_methods']
        for axis in ('X', 'Y'):
            member = axis.lower()
            before = f'int32_t(drop.pos.{axis} + base.{member})'
            after = f'(int32_t(drop.pos.{axis}) + base.{member})'
            if body.count(before) != 1:
                raise AssertionError('Expected one actual landing conversion per axis')
            body = body.replace(before, after)
        pieces['ripple_methods'] = body
        broken_dir = output / 'wrong-conversion'
        broken_dir.mkdir()
        with patch('drip_ripple_probe.production_spans', return_value=pieces):
            cls.broken, _ = probe.build_family(broken_dir, asset, inputs, 96, (37, -29, 0))

    def test_actual_recursive_states_and_rng(self):
        self.assertEqual(len(self.port), 121)
        for tick, native in enumerate(self.native):
            with self.subTest(tick=tick):
                self.assertEqual(probe.compare_state(native, self.port[tick]), [])
        final = self.native[-1]
        self.assertEqual(final['random_count'], 40)
        self.assertEqual(len(final['births']), 12)
        self.assertEqual(len(final['deaths']), 12)
        self.assertEqual({x['arguments'][3] for x in final['births']}, {48, 24})
        self.assertEqual(len(final['actors']), 1)
        self.assertFalse(final['actors'][0]['alive'])

    def test_opposing_signs_reject_truncation_before_owner_addition(self):
        self.assertEqual(probe.compare_state(self.native[18], self.broken[18]), [])
        self.assertIn('births', probe.compare_state(self.native[19], self.broken[19]))
        self.assertNotEqual(self.native[19]['births'][0]['arguments'],
                            self.broken[19]['births'][0]['arguments'])

    def test_shared_native_rng_and_recursive_state_reset(self):
        self.fixture.reset(96, (37, -29, 0))
        for tick in range(probe.TICKS + 1):
            if tick:
                self.fixture.step()
            self.assertEqual(self.fixture.state(), self.native[tick])
        self.assertEqual(self.fixture.random, self.random)

    def test_pool_cardinality_is_checked_before_zip(self):
        truncated = copy.deepcopy(self.port[1])
        truncated['actors'][0]['drops'].pop()
        self.assertIn('actor0.drop cardinality',
                      probe.compare_state(self.native[1], truncated))
        truncated = copy.deepcopy(self.port[19])
        truncated['actors'].pop()
        self.assertIn('actor cardinality', probe.compare_state(self.native[19], truncated))


if __name__ == '__main__':
    unittest.main()
