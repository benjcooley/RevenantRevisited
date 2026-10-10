"""Exact FireFlash angles/fade/RNG and explicit remaining matrix-rounding scope."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]


class FireFlashNativeComparison(unittest.TestCase):
    def test_full_pool_exact_angles_scale_integer_rng_and_native_blend_mapping(self):
        with tempfile.TemporaryDirectory()as directory:
            output=Path(directory)/'proof'
            result=subprocess.run([sys.executable,str(Path(__file__).with_name('fireflash_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--state-only','--output',str(output)],
                capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            report=json.loads((output/'manifest.json').read_text())
            self.assertEqual(report['native_particle_count'],150)
            self.assertEqual(report['native_birth_rng'],225);self.assertEqual(report['native_final_rng'],759)
            self.assertEqual(len(report['cases']),101)
            self.assertTrue(report['native_blend_mapping']['source_additive_metadata_correct'])
            states=dict(report['native_blend_mapping']['states'])
            self.assertEqual((states[19],states[20]),(2,2))
            f32=lambda value:struct.pack('<f',value)
            for case in report['cases']:
                a,b=case['original_state'],case['port_state']
                self.assertEqual(a['frame'],b['frame']);self.assertEqual(a['random_count'],b['random_count'])
                self.assertEqual([f32(v)for v in a['values']],[f32(v)for v in b['values']])
                self.assertEqual(len(a['particles']),150);self.assertEqual(len(b['particles']),150)
                for native,modern in zip(a['particles'],b['particles']):
                    self.assertEqual(native['integers'],modern['integers'])
                    # Only the explicitly characterized shared matrix output
                    # may differ; angles, scale and all other fields stay exact.
                    self.assertEqual([f32(v)for v in native['values'][3:]],[f32(v)for v in modern['values'][3:]])
                    for x,y in zip(native['values'][:3],modern['values'][:3]):self.assertLessEqual(abs(x-y),5e-6)
                if case['original_draw_count']is not None:
                    self.assertEqual(case['original_draw_count'],len(b['draws']))
                    self.assertTrue(all(d['additive']for d in b['draws']))
                    self.assertTrue(all(f32(d['normal_z_scale']) == f32(1/1.5) for d in b['draws']))
            self.assertEqual(report['cases'][0]['float32_differences'],0)
            self.assertFalse(report['software_pixel_comparison_executed'])
            self.assertFalse(report['full_acceptance'])
            self.check_native_normal_bridge(report)

    def check_native_normal_bridge(self, report):
        import math
        import zipfile
        from fireflash_probe import FireFlashFixture
        from firecone_owner_probe import native_owner, native_world
        from lit_software_probe import LitSoftwareFixture
        with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
            asset = archive.read('Imagery/Magic/fireflash.i3d')
        image = ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        native = FireFlashFixture(image,asset)
        native.reset()
        native.vm.write(native.owner+0x10,struct.pack('<3i',10000,10000,16))
        owner = native_owner(native)
        self.assertEqual(owner[:12],[1,0,0,0,0,1,0,0,0,0,1,0])
        lit = LitSoftwareFixture(image,camera=(10000,10000,0))
        lit.lighting((38,38,38),(1,1,1))
        for _ in range(30): native.step()
        draws = native.render()
        current = report['cases'][30]['port_state']['draws']
        self.assertEqual(len(draws),len(current))
        changed = 0
        for before,after in zip(draws,current):
            matrix = native_world(native,before)
            part = native.parts[before['object']]
            _,_,transformed = lit.draw_authored(part['vertices'],part['indices'],matrix,raster=False)
            for vertex,reference in zip(part['vertices'],transformed):
                m,n = after['matrix'],vertex[3:6]
                wn = [sum(n[j]*m[j*4+i] for j in range(3)) for i in range(3)]
                def shader_light(z_scale):
                    w = [wn[0],wn[1],wn[2]*z_scale]
                    length = math.sqrt(sum(x*x for x in w))
                    return math.floor(min(1,38/255+max(0,(w[1]*.78125+w[2]*.625)/length))*31)
                expected = shader_light(after['normal_z_scale'])
                self.assertEqual(list(reference[3:6]),[expected]*3)
                changed += shader_light(1) != expected
        self.assertGreater(changed,0) # Reject omitting the explicit geometry bridge.



if __name__=='__main__':unittest.main()
