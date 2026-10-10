"""Pin actual custom factories and the nonstandard <=0 V wrap."""
import json,subprocess,sys,tempfile,unittest,zipfile,struct
from pathlib import Path
from warp_probe import ROOT,PROFILES,WarpFixture,parse_parts,compile_warp

class WarpNativeTests(unittest.TestCase):
    def test_exact_seven_native_atlas_animators(self):
        with tempfile.TemporaryDirectory(prefix='warp-native-')as tmp:
            r=subprocess.run([sys.executable,str(Path(__file__).with_name('warp_probe.py')),
                str(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),'--output',tmp],
                text=True,capture_output=True,timeout=30)
            self.assertEqual(r.returncode,0,r.stdout+r.stderr)
            report=json.loads((Path(tmp)/'manifest.json').read_text())
            self.assertEqual(report['status'],'native_preflight_pass')
            self.assertEqual(len(report['dispatch']),7)
            self.assertFalse(report['rendered_comparison'])
            self.assertFalse(report['full_effect_accepted'])
            for p in PROFILES:
                s=report['states'][p['name']]
                self.assertEqual(len(s),41)
                self.assertEqual([s[t]['offset']for t in(0,1,4,8,12,16,20,24,40)],
                    [[0,0],[.25,0],[0,1],[0,.75],[0,.5],[0,.25],[0,1],[0,.75],[0,.75]])
                self.assertTrue(all(x['velocity']==[.25,-.25]and x['delay']==[0,1]for x in s))
                for x in s:
                    for old,new in zip(s[0]['uvs'],x['uvs']):
                        for i in(0,1):self.assertAlmostEqual(new[i]-old[i],x['offset'][i],places=7)

    def test_compiled_state_payload_and_fractional_clock(self):
        with tempfile.TemporaryDirectory(prefix='warp-header-')as tmp:
            metadata={};native={};fixture=WarpFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
            with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:
                names={n.lower():n for n in archive.namelist()}
                for p in PROFILES:
                    m=parse_parts(archive.read(names[('Imagery/'+p['asset']).lower()]),p)[0]
                    metadata[p['name']]=m;native[p['name']]=fixture.trace(m,profile=p)
            modern,proof=compile_warp(Path(tmp),metadata)
            self.assertIn('ConfigureDraw',proof['bodies'][-2])
            for p in PROFILES:
                for a,b in zip(native[p['name']],modern[p['name']]):
                    self.assertEqual(a['offset'],b['offset'])
                    self.assertEqual(b['flags'],[0,0,1,2])
                    for x,y in zip(a['uvs'],b['uvs']):
                        for c,d in zip(x,y):self.assertEqual(struct.pack('<f',c),struct.pack('<f',d))

if __name__=='__main__':unittest.main()
