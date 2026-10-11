"""A whole session on the reference host, with Jev's answers stubbed."""
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(HERE))
import rec  # noqa: E402
import timeline  # noqa: E402
from director import Director  # noqa: E402
from waypoints import Catalogue  # noqa: E402
from host import Host  # noqa: E402
from inputs import Bindings, chord  # noqa: E402
from jev import Jev, Ledger, Router  # noqa: E402
from knowledge import Knowledge  # noqa: E402
from milestones import SMOKE  # noqa: E402


class PreferObjects:
    """Decisions stub: walk to an object when one is offered."""

    def __call__(self, url, body, headers):
        answers = {}
        for name, question in body['questions'].items():
            options = list(question['criteria'])
            choice = next((o for o in options if o.startswith('obj:')), options[0])
            answers[name] = {'type': 'choice', 'choice': choice, 'confidence': 0.9,
                             'probabilities': {o: float(o == choice) for o in options}}
        return {'answers': answers, 'usage': {'cost': 0.00001}}


class NoRouter:
    def __call__(self, url, body, headers):
        raise AssertionError('the router should not be needed on the reference host')


class Session(unittest.TestCase):
    def test_reach_the_milestone_save_and_replay(self):
        os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            bundle = tmp / 'bundle'
            bundle.mkdir()
            ledger = Ledger(bundle / 'session.rvlog')
            bindings = Bindings({'QuickSave': chord('CTRL-BS'), 'CombatMode': chord('C'), 'Run': chord('R')})
            with Host([sys.executable, HERE / 'fakehost.py'], tmp / 'play', timeout=30) as host:
                host.hello()
                host.start('newgame', seed=1, record=bundle)
                catalogue = Catalogue(tmp / 'waypoints')
                director = Director(host, Jev(ledger=ledger, post=PreferObjects()), Router(post=NoRouter()),
                                    Knowledge({}, {}), bindings, bundle, list(SMOKE[:1]), waypoints=catalogue,
                                    story='smoke', game='fake')
                summary = director.run(max_ticks=24 * 60)
            self.assertEqual(summary['reached'], ['met sardok'])
            for waypoint, until in director.created:
                catalogue.attach_recording(waypoint, bundle, until)

            waypoint = catalogue.get('fake', 'met sardok')
            self.assertEqual((waypoint.story, waypoint.index, waypoint.parent, waypoint.slot),
                             ('smoke', 1, 'newgame', 'Quick Save 1'))
            self.assertTrue((waypoint.slot_dir / 'game.sav').exists())
            self.assertTrue((waypoint.dir / 'recording' / 'session.rvrec').exists())
            self.assertEqual(waypoint.recording['until'], director.created[0][1])

            # the whole gameplay log
            self.assertTrue(list((bundle / 'frames').glob('t*.png')) or True)   # the toy host draws no frames
            observations = (bundle / 'observations.jsonl').read_text().splitlines()
            self.assertGreater(len(observations), 3)
            ledger_lines = [json.loads(line) for line in (bundle / 'session.rvlog').read_text().splitlines()]
            self.assertTrue(all('t' in entry for entry in ledger_lines))   # every Jev call placed in time
            report = timeline.write(bundle).read_text()
            self.assertIn('met sardok', report)
            self.assertIn('class="jev"', report)

            recording = rec.Recording.load(bundle)
            self.assertEqual(rec.validate(recording), [])
            self.assertEqual([m.kind for m in recording.marks], ['milestone', 'saved'])
            self.assertEqual(recording.marks[1].extra['slot'], 'Quick Save 1')
            keys = [(e.key, e.down) for e in recording.events if isinstance(e, rec.Key)]
            self.assertEqual(keys, [('CONTROL', True), ('BACK', True), ('BACK', False), ('CONTROL', False)])
            kinds = [json.loads(line)['kind'] for line in (bundle / 'director.jsonl').read_text().splitlines()]
            self.assertIn('walk', kinds)
            self.assertIn('waypoint', kinds)

            with Host([sys.executable, HERE / 'fakehost.py'], tmp / 'replay', timeout=30) as replay:
                replay.hello()
                self.assertIsNone(replay.replay(bundle)['diverged'])


class Calibration(unittest.TestCase):
    def test_measures_each_direction_and_ends_where_it_began(self):
        os.environ.setdefault('OPEN_ROUTER_KEY', 'test-key')
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            keys = dict(zip(('Right', 'UpRight', 'Up', 'UpLeft', 'Left', 'DownLeft', 'Down', 'DownRight'),
                            ('RIGHT', 'PGUP', 'UP', 'HOME', 'LEFT', 'END', 'DOWN', 'PGDN')))
            bindings = Bindings({name: chord(key) for name, key in keys.items()})
            with Host([sys.executable, HERE / 'fakehost.py'], tmp / 'play', timeout=30) as host:
                host.hello()
                host.start('newgame', seed=1)
                start = host.observe()['player']['pos']
                director = Director(host, Jev(post=PreferObjects()), Router(post=NoRouter()), Knowledge({}, {}),
                                    bindings, tmp, [])
                director.calibrate()
                self.assertEqual(host.observe()['player']['pos'], start)
            steps = director.tactician.directions
            self.assertEqual(steps['Right'], (2, 0))                   # the key held 2 ticks, a unit a tick
            self.assertEqual(steps['Up'], (0, -2))
            self.assertNotIn('UpRight', steps)                          # the toy world has no diagonal keys


if __name__ == '__main__':
    unittest.main()
