import json
import sys
import tempfile
import unittest
from argparse import Namespace
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from autotest import AutoTest, Stage, incidents, report  # noqa: E402
from test_waypoints import slot  # noqa: E402


class Scoring(unittest.TestCase):
    def test_incidents_count_deaths_once_each(self):
        with tempfile.TemporaryDirectory() as tmp:
            bundle = Path(tmp)
            entries = [{'t': 100, 'kind': 'dead'}, {'t': 110, 'kind': 'dead'}, {'t': 900, 'kind': 'dead'},
                       {'t': 950, 'kind': 'router'}, {'t': 990, 'kind': 'save suspect'}, {'t': 999, 'kind': 'walk'}]
            (bundle / 'director.jsonl').write_text(''.join(json.dumps(e) + '\n' for e in entries))
            self.assertEqual(incidents(bundle), {'deaths': 2, 'router': 1, 'save suspect': 1})

    def test_a_stage_is_clean_only_when_everything_held(self):
        good = Stage('port', 'rand questioned', 3, status='reached', interop={'ok': True},
                     replay={'diverged': None}, variances=[])
        self.assertTrue(good.ok)
        for bad in (dict(retired='story states'), dict(interop={'ok': False}), dict(replay={'diverged': {'t': 5}}),
                    dict(variances=[{'kind': 'screen'}]), dict(status='missed')):
            self.assertFalse(Stage(**{**good.__dict__, **bad}).ok, bad)

    def test_the_report_escapes_game_text(self):
        page = report({'story': 'keep', 'started': 'now', 'minutes': 5,
                       'stages': [dict(Stage('port', 'opening', 0, status='broke', error='<b>boom</b>').__dict__,
                                       ok=False)]})
        self.assertIn('&lt;b&gt;boom', page)


class Starting(unittest.TestCase):
    def test_a_stage_starts_from_its_own_game_else_the_other(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            args = Namespace(out=tmp / 'out', waypoints=tmp / 'wp', story='keep', stages=None, games=['port', 'retail'])
            test = AutoTest(args)
            test.catalogue.add('port', 'opening', 'keep', 1, 'newgame', slot(tmp / 'a' / 'Quick Save 1', 'a'))
            first, second = (next(s for s in test.stages if s.game == 'retail' and s.index == i) for i in (0, 1))
            self.assertEqual(test.start_point(first), '')                      # a new game
            self.assertEqual(test.start_point(second), 'port:opening')         # retail has none of its own
            third = next(s for s in test.stages if s.game == 'retail' and s.index == 2)
            self.assertIsNone(test.start_point(third))                         # nobody has 'rahul defeated'


if __name__ == '__main__':
    unittest.main()
