"""The host client against the reference host: requests, recording, replay."""
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(HERE))
import rec  # noqa: E402
from host import Host, HostError  # noqa: E402
from inputs import Plan, chord  # noqa: E402


def fake(run_dir: Path, *args: str) -> Host:
    return Host([sys.executable, HERE / 'fakehost.py', *args], run_dir, timeout=30)


def play(host: Host, bundle: Path) -> None:
    host.hello()
    host.start('newgame', seed=5, record=bundle)
    plan = Plan()
    plan.hold(0, chord('RIGHT'))
    plan.release(30, chord('RIGHT'))
    plan.click(31, 400, 300)
    host.step(plan.ticks(60), plan.events())
    host.mark('milestone', name='walked')
    host.step(40)


class ReferenceHost(unittest.TestCase):
    def test_record_then_replay_matches(self):
        with tempfile.TemporaryDirectory() as tmp:
            bundle = Path(tmp) / 'bundle'
            with fake(Path(tmp) / 'run') as host:
                play(host, bundle)
                self.assertEqual(host.tick, 100)
                self.assertEqual(host.observe()['player']['pos'], [1000 + 5 + 30 + 20, 2000 + 15, 0])
            recording = rec.Recording.load(bundle)
            self.assertEqual(rec.validate(recording), [])
            self.assertEqual(recording.end, 100)
            self.assertEqual([m.kind for m in recording.marks], ['milestone'])
            self.assertEqual(len(recording.events), 5)

            with fake(Path(tmp) / 'replay') as host:
                self.assertIsNone(host.replay(bundle)['diverged'])

    def test_replay_reports_the_first_divergent_tick(self):
        with tempfile.TemporaryDirectory() as tmp:
            bundle = Path(tmp) / 'bundle'
            with fake(Path(tmp) / 'run') as host:
                play(host, bundle)
            # after the click's walk has arrived, so nothing walks the nudge back
            with fake(Path(tmp) / 'replay', '--drift-at', '80') as host:
                diverged = host.replay(bundle)['diverged']
            self.assertEqual(diverged['t'], 96)                  # the first check line after the drift
            self.assertIn('pos', diverged['fields'])

    def test_errors_come_back_as_exceptions(self):
        with tempfile.TemporaryDirectory() as tmp, fake(Path(tmp)) as host:
            with self.assertRaises(HostError):
                host.observe()                                    # before start
            with self.assertRaises(ValueError):
                host.start('newgame')
                host.step(2, [{'t': 2, 'key': 'A', 'down': True}])


if __name__ == '__main__':
    unittest.main()
