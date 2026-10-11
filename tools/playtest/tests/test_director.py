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
from director import Director, SaveLibrary  # noqa: E402
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
                director = Director(host, Jev(ledger=ledger, post=PreferObjects()), Router(post=NoRouter()),
                                    Knowledge({}, {}), bindings, bundle, list(SMOKE), SaveLibrary(tmp / 'saves'),
                                    game='fake')
                summary = director.run(max_ticks=24 * 60)
            self.assertEqual(summary['reached'], ['met sardok'])

            manifest = json.loads((tmp / 'saves' / 'fake' / 'met sardok' / 'manifest.json').read_text())
            self.assertEqual(manifest['slot'], 'Quick Save 1')
            self.assertTrue((tmp / 'saves' / 'fake' / 'met sardok' / 'slot' / 'game.sav').exists())

            recording = rec.Recording.load(bundle)
            self.assertEqual(rec.validate(recording), [])
            self.assertEqual([m.kind for m in recording.marks], ['milestone', 'saved'])
            self.assertIn({'key': 'CONTROL', 'down': True, 't': recording.events[-4].t},
                          [e.line() for e in recording.events])                 # the quick save, as input
            kinds = [json.loads(line)['kind'] for line in (bundle / 'director.jsonl').read_text().splitlines()]
            self.assertIn('walk', kinds)

            with Host([sys.executable, HERE / 'fakehost.py'], tmp / 'replay', timeout=30) as replay:
                replay.hello()
                self.assertIsNone(replay.replay(bundle)['diverged'])


if __name__ == '__main__':
    unittest.main()
