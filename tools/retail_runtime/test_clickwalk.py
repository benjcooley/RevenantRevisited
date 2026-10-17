"""The `clickwalk` retail build (recon/retail_asm/patches/clickwalk,
docs/playtest/CLICKWALK.md): its catalogued bytes against the build
contract, and the map pane's click path in the emulator on it and on the
unchanged baseline (clickwalk_probe.py, the combat slot's world).

The image is made from the manifest's bytes (pinned by its SHA-256, the
same as the NASM build's); the source route is
recon/retail_asm/patches/clickwalk/build.py."""
import copy
import hashlib
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / 'retail_asm'))
import retail_patch  # noqa: E402
from build_contract import BASELINE, RETAIL_SHA, verify_build  # noqa: E402
from clickwalk_probe import (CAVE, LOCKE_AT, ClickFixture, cases, compare, import_hudworld,  # noqa: E402
                             scroll_for, screen_to_world)

CATALOGUE = HERE.parents[1] / 'recon' / 'retail_asm' / 'patches' / 'clickwalk'
GOTO_SITE = 0x0044ff53


class ClickwalkTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.manifest = retail_patch.load(CATALOGUE / 'manifest.json')
        cls.exe = Path(cls.temp.name) / 'Revenant.clickwalk.exe'
        cls.exe.write_bytes(retail_patch.apply(BASELINE.read_bytes(), cls.manifest))
        cls.record = dict(name='clickwalk', build_id='clickwalk-test', executable=str(cls.exe),
                          executable_sha256=cls.manifest['executable_sha256'], baseline_sha256=RETAIL_SHA,
                          patch_manifest=cls.manifest)
        cls.results = {}
        for label, exe, record in (('baseline', BASELINE, None), ('clickwalk', cls.exe, cls.record)):
            fixture = ClickFixture(exe, record)
            cls.results[label] = {c['name']: fixture.run(copy.deepcopy(c)) for c in cases()}

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def test_catalogue_bytes_pass_the_build_contract(self):
        data = self.exe.read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(), self.manifest['executable_sha256'])
        self.assertTrue(verify_build(self.exe, self.record)['experimental'])
        spans = retail_patch.verify(BASELINE.read_bytes(), data, self.manifest)
        self.assertTrue(spans)
        # The patch site keeps its length and opcode (je near); the cave sits
        # in .text's zero padding.
        self.assertEqual(data[GOTO_SITE - 0x400000:][:2], b'\x0f\x84')
        self.assertEqual(int.from_bytes(data[GOTO_SITE - 0x400000 + 2:][:4], 'little', signed=True),
                         CAVE - (GOTO_SITE + 6))

    def test_contract_rejects_an_undeclared_change(self):
        data = bytearray(self.exe.read_bytes())
        data[0x0044ff4b - 0x400000] ^= 1                  # the jne before the patch site
        other = Path(self.temp.name) / 'Revenant.other.exe'
        other.write_bytes(data)
        record = dict(self.record, executable=str(other), executable_sha256=hashlib.sha256(data).hexdigest())
        with self.assertRaisesRegex(ValueError, 'Undeclared'):
            verify_build(other, record)
        with self.assertRaisesRegex(ValueError, 'SHA-256'):
            retail_patch.verify(BASELINE.read_bytes(), bytes(data), self.manifest)

    def test_hud_world_takes_only_contract_builds(self):
        HudWorld = import_hudworld()
        with self.assertRaisesRegex(ValueError, 'explicit named'):
            HudWorld(self.exe)
        bad = copy.deepcopy(self.record)
        bad['patch_manifest']['allowed_modified_spans'] = []
        with self.assertRaisesRegex(ValueError, 'Undeclared'):
            HudWorld(self.exe, build=bad)

    def test_suite_differs_only_where_the_patch_acts(self):
        rows, failures = compare(list(self.results['baseline'].values()), list(self.results['clickwalk'].values()))
        self.assertEqual(failures, [])
        entered = {row['case'] for row in rows if row['clickwalk_code']}
        # The cave ran for each release on open ground; in combat and bow mode
        # it declined (their traces equal the baseline's).
        self.assertEqual(entered, {'open-ground', 'open-ground-turn', 'open-ground-run', 'open-ground-walk',
                                   'combat-stale-door-cursor', 'bow-not-drawn'})

    def test_open_ground_goes_to_the_point_under_the_click(self):
        result = self.results['clickwalk']['open-ground']
        release = result['steps'][1]['seams']
        pane = scroll_for(LOCKE_AT)
        x, y = result['steps'][1]['x'], result['steps'][1]['y']
        point = screen_to_world(pane['posx'] + x, pane['posy'] + y, LOCKE_AT[2] + 50)
        self.assertEqual([s for s in release if s['seam'] == 'Goto'],
                         [dict(seam='Goto', who='Locke', x=point[0], y=point[1], pickup=None)])
        # The right button's walk (UpdateMouseMovement) converts the same pointer the same way.
        right = self.results['clickwalk']['right-button']['steps'][0]['seams']
        self.assertEqual([s['point'] for s in right if s['seam'] == 'ScreenToWorld'], [point])
        doing = result['self']['doing']
        self.assertEqual((doing['name'], doing['target'][:2]), ('walkf', point[:2]))
        self.assertIn('walkto', doing['flags'])
        self.assertEqual(self.results['baseline']['open-ground']['self']['doing']['name'], 'walk')

    def test_the_walk_steers_to_the_point_and_stops_on_it(self):
        # Game ticks of retail's own UpdateMove / ResolveMove / Move on flat
        # ground: the facing follows the point's angle (35) and Locke stands
        # on the point itself, then the stop transition plays.
        ticks = self.results['clickwalk']['open-ground-walk']['ticks']
        goal = [6323, 4301]
        arrived = next(tk['tick'] for tk in ticks if tk['pos'][:2] == goal)
        self.assertLess(arrived, 20)
        self.assertTrue(all(abs(tk['facing'] - 35) <= 1 for tk in ticks[1:arrived]))
        self.assertEqual(ticks[-1]['doing']['name'], 'walk')
        base = self.results['baseline']['open-ground-walk']['ticks']
        self.assertTrue(all(tk['pos'] == base[0]['pos'] for tk in base))

    def test_run_key_runs(self):
        doing = self.results['clickwalk']['open-ground-run']['self']['doing']
        self.assertEqual(doing['name'], 'runf')
        self.assertIn('walkto', doing['flags'])


if __name__ == '__main__':
    unittest.main(verbosity=2)
