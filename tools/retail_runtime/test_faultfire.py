"""Guard actual native/ported FaultFire phase wrap and stored per-vertex U."""
import tempfile
import unittest
import zipfile
from pathlib import Path
from faultfire_probe import ROOT,FaultFireFixture,build_port,f32


class FaultFireNativeTests(unittest.TestCase):
    def test_actual_theta_wrap_and_per_vertex_scroll(self):
        exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/faultfire.i3d')
        fixture=FaultFireFixture(exe,asset);fixture.reset();native=[]
        for tick in range(129):
            if tick:fixture.animate()
            native.append(fixture.state())
        with tempfile.TemporaryDirectory(prefix='faultfire-native-')as tmp:
            port,_=build_port(Path(tmp),fixture.mesh,128,fixture.random_calls)
            for tick,state in enumerate(native):
                row=port[(1,tick)];self.assertEqual(f32(state['theta']),f32(row['theta']),f'theta tick{tick}')
                u=[None]*4
                for face,draw in enumerate(row['draws'][:2]):
                    for index,uv in zip(fixture.mesh['indices'][face*3:face*3+3],draw['uvs']):u[index]=uv[0]
                self.assertEqual(list(map(f32,state['u'])),list(map(f32,u)),f'stored U tick{tick}')
                self.assertEqual(port[(0,tick)],row,f'preview/map tick{tick}')
            self.assertEqual(len(fixture.random_calls),128)
            self.assertEqual({tuple(c[:2])for c in fixture.random_calls},{(2,8)})
            self.assertLess(native[63]['theta'],native[62]['theta'])
            self.assertLess(native[126]['theta'],native[125]['theta'])
        fixture.reset()
        for tick,state in enumerate(native):
            if tick:fixture.animate()
            self.assertEqual(fixture.state(),state,f'warm replay tick{tick}')


if __name__=='__main__':unittest.main()
