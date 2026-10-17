"""Guard cadence reporting against short, broken and mismatched captures."""
import json
from pathlib import Path
import struct
import tempfile
import unittest
import zipfile

from arena_cadence_report import report


class ArenaCadenceReportTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.initial = self.root / 'initial.json'
        self.frames = self.root / 'frames.json'
        self.imagery = self.root / 'imagery.rvi'
        header = bytearray(28 + 76)
        struct.pack_into('<I', header, 24, 1)
        header[28:38] = b'100:watcha'
        struct.pack_into('<HH', header, 28 + 40, 0x400, 3)
        with zipfile.ZipFile(self.imagery, 'w') as z:
            z.writestr('Imagery/Chars/test.i3d', header)
        self.initial.write_text(json.dumps({'frames': [self.frame(3, 2)]}))
        self.data = dict(failure=None, draw=True,
                         multimedia_timers={'1': [0x49e2f0, 41, 0]},
                         frames=[self.frame(s, f) for s, f in
                                 [(4, 2), (5, 1), (6, 2), (7, 2), (8, 1)]])

    @staticmethod
    def frame(screen, phase):
        return dict(screen_frame=screen, targets=[dict(
            index=1, position=[12400, 17400, 32], asset='Chars\\test.i3d',
            state=0, frame=phase, frame_increment=1, clip_frames=3)])

    def analyze(self):
        self.frames.write_text(json.dumps(self.data))
        return report(self.initial, self.frames, self.imagery)

    def test_actual_terminal_hold_and_retrigger(self):
        result = self.analyze()
        actor = result['spectators'][0]
        self.assertEqual(actor['terminal_hold_screen_frames'], [4, 7])
        self.assertEqual(actor['observed_restart_screen_frames'], [5, 8])
        self.assertEqual(actor['observed_restart_intervals_virtual_ms'], [123])
        self.assertFalse(actor['progression_mismatches'])
        self.assertTrue(actor['full_cycle_observed'])
        self.assertFalse(result['accepted'])

    def test_short_capture_does_not_claim_complete_cycle(self):
        self.data['frames'] = self.data['frames'][:2]
        self.assertFalse(self.analyze()['all_selected_cycles_observed'])

    def test_missing_screen_tick_rejected(self):
        self.data['frames'][2]['screen_frame'] += 1
        with self.assertRaisesRegex(ValueError, 'not consecutive'):
            self.analyze()

    def test_wrong_timer_rejected(self):
        self.data['multimedia_timers']['1'][1] = 10
        with self.assertRaisesRegex(ValueError, 'WinMM'):
            self.analyze()

    def test_missing_actor_rejected(self):
        self.data['frames'][1]['targets'] = []
        with self.assertRaisesRegex(ValueError, 'vanished'):
            self.analyze()

    def test_wrong_frame_is_reported_without_pass_credit(self):
        self.data['frames'][2]['targets'][0]['frame'] = 1
        result = self.analyze()
        self.assertTrue(result['spectators'][0]['progression_mismatches'])
        self.assertFalse(result['all_selected_cycles_observed'])

    def test_original_header_disagreement_rejected(self):
        self.data['frames'][1]['targets'][0]['clip_frames'] = 4
        with self.assertRaisesRegex(ValueError, 'packed clip'):
            self.analyze()


if __name__ == '__main__':
    unittest.main()
