import sys
import tempfile
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import rec  # noqa: E402
from waypoints import ROOT, Catalogue, slug, story_states, wrong_states  # noqa: E402


def slot(where: Path, text: str) -> Path:
    where.mkdir(parents=True)
    (where / 'game.sav').write_text(text)
    (where / 'CurMap').mkdir()
    (where / 'CurMap' / '1_0_0.DAT').write_text('sector')
    return where


class Catalogues(unittest.TestCase):
    def test_add_find_newest_and_chain(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            catalogue = Catalogue(tmp / 'waypoints')
            first = catalogue.add('port', 'rahul defeated', 'keep', 2, 'newgame', slot(tmp / 'a' / 'Quick Save 1', 'a'),
                                  obs={'player': {'level': 0, 'pos': [1, 2, 3], 'health': [50, 100]}}, tick=4000)
            self.assertEqual(first.slot_sha, rec.slot_sha(first.slot_dir))
            self.assertEqual(first.player, {'health': [50, 100]})
            time.sleep(1.1)
            again = catalogue.add('port', 'rahul defeated', 'keep', 2, 'newgame', slot(tmp / 'b' / 'Quick Save 2', 'b'))
            child = catalogue.add('port', 'keep explored', 'keep', 3, again.name, slot(tmp / 'c' / 'Quick Save 3', 'c'),
                                  slot_name='Keep explored')
            self.assertEqual(catalogue.get('port', 'rahul defeated').dir, again.dir)    # the newest
            self.assertEqual(catalogue.get('port', first.dir.name).dir, first.dir)      # or exactly
            self.assertEqual([w.name for w in catalogue.all('port')],
                             ['rahul defeated', 'rahul defeated', 'keep explored'])
            self.assertEqual(child.slot_dir.name, 'Keep explored')
            self.assertEqual(child.parent, 'rahul defeated')
            with self.assertRaises(KeyError):
                catalogue.get('retail', 'rahul defeated')

    def test_recording_attached(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            catalogue = Catalogue(tmp / 'waypoints')
            w = catalogue.add('retail', 'opening', 'keep', 1, 'newgame', slot(tmp / 's' / 'Quick Save 1', 'x'))
            bundle = tmp / 'bundle'
            (bundle / 'frames').mkdir(parents=True)
            (bundle / 'session.rvrec').write_text('{}\n')
            (bundle / 'frames' / 't0000001.png').write_bytes(b'png')
            catalogue.attach_recording(w, bundle, 1234)
            loaded = catalogue.get('retail', 'opening')
            self.assertEqual(loaded.recording, {'bundle': 'recording', 'until': 1234})
            self.assertTrue((loaded.dir / 'recording' / 'session.rvrec').exists())
            self.assertFalse((loaded.dir / 'recording' / 'frames').exists())          # frames stay with the session

    def test_a_retired_waypoint_is_found_only_by_its_directory(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            catalogue = Catalogue(tmp / 'waypoints')
            good = catalogue.add('port', 'rahul defeated', 'keep', 2, 'newgame', slot(tmp / 'a' / 'Quick Save 1', 'a'))
            time.sleep(1.1)
            bad = catalogue.add('port', 'rahul defeated', 'keep', 2, 'newgame', slot(tmp / 'b' / 'Quick Save 2', 'b'),
                                retired='story states {}')
            self.assertEqual(catalogue.get('port', 'rahul defeated').dir, good.dir)     # the newest good one
            self.assertEqual(catalogue.get('port', bad.dir.name).retired, 'story states {}')

    def test_wrong_states(self):
        self.assertEqual(wrong_states({'RANDSTATE': 1}, {'RANDSTATE': 1, 'TENDRICKSTATE': 1}),
                         {'TENDRICKSTATE': [1, 0]})                          # a variable never set reads 0
        self.assertEqual(wrong_states({'RAHULSTATE': 0}, {'RAHULSTATE': 0}), {})

    @unittest.skipUnless(list(ROOT.glob('port/rand-questioned--*/slot/*/game.sav')), 'no saved waypoint')
    def test_story_states_from_a_real_save(self):
        sav = sorted(ROOT.glob('port/rand-questioned--*/slot/*/game.sav'))[0]
        states = story_states(sav.parent)
        self.assertEqual((states['RANDSTATE'], states['TENDRICKSTATE']), (1, 1))

    def test_slug(self):
        self.assertEqual(slug('Rahul defeated!'), 'rahul-defeated')


if __name__ == '__main__':
    unittest.main()
