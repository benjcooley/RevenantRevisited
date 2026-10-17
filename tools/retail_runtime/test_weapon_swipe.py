"""Retail TWeaponSwipe vs src/weaponswipe.cpp, with negative controls.

Run with the retail venv:
    $PY tools/retail_runtime/test_weapon_swipe.py
"""
from pathlib import Path
import tempfile
import unittest

import weapon_swipe_probe as probe

EXE = probe.ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
SOURCE = probe.ROOT / 'src/weaponswipe.cpp'


def mutated(directory: Path, old: str, new: str) -> Path:
    text = SOURCE.read_text()
    assert old in text, old
    path = directory / 'weaponswipe.cpp'
    path.write_text(text.replace(old, new))
    return path


class WeaponSwipeAgainstRetail(unittest.TestCase):
    def test_histories_and_strips_match(self):
        with tempfile.TemporaryDirectory() as directory:
            report = probe.run(EXE, Path(directory) / 'proof', extra=8)
        self.assertEqual(report['status'], 'pass')
        self.assertGreater(report['running_frames'], 1000)
        by_index = {c['index']: c for c in report['cases']}
        self.assertEqual(by_index[20]['running'], 0)                  # no SWIPECOLOR: never runs
        self.assertLess(by_index[18]['running'], by_index[18]['frames'])   # a new weapon closes it
        self.assertIn('SwipeFull', by_index[17]['stats'])
        self.assertIn('GetObjectNum', report['seam_calls'])

    def _detected(self, old, new):
        with tempfile.TemporaryDirectory() as directory:
            path = mutated(Path(directory), old, new)
            report = probe.run(EXE, Path(directory) / 'proof', extra=0, source=path)
        self.assertEqual(report['status'], 'fail')

    def test_snapshot_strip_from_the_newest_sample_is_detected(self):
        self._detected('Spline(t, p[size_t(seg)], p[size_t(seg + 1)], p[size_t(seg + 2)],\n'
                       '                                         p[size_t(seg + 3)]);',
                       'Spline(t, p[size_t(seg > 0 ? seg - 1 : 0)], p[size_t(seg)], p[size_t(seg + 1)],\n'
                       '                                         p[size_t(seg + 2)]);')

    def test_y_tip_at_the_high_end_is_detected(self):
        self._detected('axis == 1 ? extent_min[1] : extent_max[size_t(axis)]', 'extent_max[size_t(axis)]')

    def test_no_reset_when_the_window_opens_is_detected(self):
        self._detected('        if (now.frame == now.frame_on)\n            Reset(now);\n', '')


if __name__ == '__main__':
    unittest.main()
