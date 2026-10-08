"""All native Cure records and natural null-spell owner lifetime."""
import tempfile
import unittest
from pathlib import Path
from cure_probe import ROOT,CureFixture,build_port


class CureNativeComparison(unittest.TestCase):
    def test_nullspell_state_matches_original_through_natural_kill(self):
        with tempfile.TemporaryDirectory()as directory:
            output=Path(directory);original=CureFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');original.reset();states=[original.raw_state()]
            for _ in range(140):original.advance();states.append(original.raw_state())
            self.assertEqual(original.state()['frame'],132)
            self.assertFalse(original.state()['alive'])
            self.assertEqual(len(original.state()['swirls']),80)
            self.assertEqual(len(original.state()['balls']),5)
            rng=output/'native-rng.txt';rng.write_text(''.join('%d %d %d\n'%r for r in original.random if r[0]!=r[1]))
            production,_=build_port(output,rng,140)
            for tick,(a,b)in enumerate(zip(states,production)):
                self.assertEqual(a,b,('all85records/frame/counters',tick))


if __name__=='__main__':unittest.main()
