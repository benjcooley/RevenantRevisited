"""Original Sparks allocator, initialization and bounce/trail state oracle."""
import tempfile
import unittest
from pathlib import Path

from fizzle_probe import f32
from sparks_probe import ROOT, SparksFixture, build_port


class SparksNativeComparison(unittest.TestCase):
    def test_original_particle_initialization_and_bounce_lifecycle(self):
        for controlled in (False, True):
            with self.subTest(controlled_bounce=controlled), tempfile.TemporaryDirectory() as directory:
                output=Path(directory)
                native=SparksFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
                native.reset()
                if controlled:
                    params=dict(particles=20,pos=[0,0,45],pspread=[3,3,3],dir=[1,0,0],spread=[.5,.5,.5],gravity=.25,trails=2,minstart=0,maxstart=8,minlife=20,maxlife=40,bounce=True,killobj=True,objflags=2,seektargets=False,seekz=False,numtargets=0)
                    native.initialize(params)
                native.advance();states=[native.initial_state,native.state()]
                for _ in range(2,53):
                    native.advance();states.append(native.state())
                rng=output/'native-rng.txt';rng.write_text(''.join('%d %d %d\n'%record for record in native.random))
                production,_=build_port(output,native.params,rng,52)
                self.assertEqual(len(native.random),180 if controlled else 90)
                self.assertEqual(native.raw_random_draws,160 if controlled else 90)
                for tick,expected in enumerate(states):
                    actual=production[tick]
                    self.assertEqual(expected['done'],actual['done'])
                    self.assertEqual(expected['random_count'],actual['random_count'])
                    self.assertEqual(len(expected['particles']),len(actual['particles']))
                    for p,q in zip(expected['particles'],actual['particles']):
                        for field in ('life','start','variant'):
                            self.assertEqual(p[field],q[field],(controlled,tick,p['index'],field))
                        self.assertEqual(b''.join(f32(v)for v in p['values']),b''.join(f32(v)for v in q['values']), (controlled,tick,p['index']))


if __name__=='__main__':
    unittest.main()
