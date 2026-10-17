"""Actual genuine caller, source lifecycle and every full native draw matrix."""
import contextlib,io,json,tempfile,unittest
from pathlib import Path
from quicksand_authentic_preflight import run,ROOT
from quicksand_pose_probe import run as compare
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
class Quicksand(unittest.TestCase):
 @unittest.skipUnless(BOOT.exists(),'Requires source-matched local original startup cache')
 def test_genuine_target_and_compiled_production(self):
  with tempfile.TemporaryDirectory(prefix='quicksand-test-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);report=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',BOOT,p/'native',samples=())
   self.assertEqual([r['tick']for r in report['records']if r['state']['flags']&4096],list(range(56,61)))
   self.assertTrue(all(r['state']['target_count']==1 for r in report['records']))
   self.assertEqual(sum(len(r['draws'])for r in report['records']),220)
   compare(p/'native/manifest.json',p/'source')
   proof=json.loads((p/'source/manifest.json').read_text());self.assertEqual(proof['status'],'pass');self.assertEqual(proof['checks'],3520);self.assertEqual(proof['lifecycle_checks'],180)
if __name__=='__main__':unittest.main()
