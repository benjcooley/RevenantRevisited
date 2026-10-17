"""Retail TBloodAnimator droplets vs effects/bloodspray.cpp, with negative controls.

Run with the retail venv:
    $PY tools/retail_runtime/test_blood_spray.py
"""
from pathlib import Path
import tempfile
import unittest

import blood_spray_probe as probe

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
SOURCE = probe.ROOT / 'src/effects/bloodspray.cpp'


def mutated(directory: Path, old: str, new: str) -> Path:
    text = SOURCE.read_text()
    assert old in text, old
    path = directory / 'bloodspray.cpp'
    path.write_text(text.replace(old, new))
    return path


class BloodSprayAgainstRetail(unittest.TestCase):
    def test_every_droplet_field_rng_and_kill_tick_match(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'proof', extra_cases=8, ticks=120)
        self.assertEqual(report['status'], 'pass')
        self.assertEqual(report['near_float_fields'], 0)
        first = report['cases'][0]
        self.assertGreater(first['drops'], 0)
        self.assertGreater(first['rand_calls'], 20)
        self.assertEqual(report['cases'][7]['drops'], 0)   # violence 0: no blood

    def test_snapshot_vertical_speed_is_detected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = mutated(Path(directory), 'sin_elevation * kOneTwoHundredth', 'sin_elevation * kOneHundredth')
            report = probe.run(EXE, Path(directory) / 'proof', extra_cases=0, ticks=40, simulation=path)
        self.assertEqual(report['status'], 'fail')

    def test_single_count_draw_is_detected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = mutated(Path(directory),
                           'if (most - random(0, most) > 0)\n        count = most - random(0, most);',
                           'count = (std::max)(most - random(0, most), 0);')
            report = probe.run(EXE, Path(directory) / 'proof', extra_cases=0, ticks=40, simulation=path)
        self.assertEqual(report['status'], 'fail')


if __name__ == '__main__':
    unittest.main()
