"""Full original SetVortex caller versus actual source update/submission functions."""
import contextlib,io,tempfile,unittest
from pathlib import Path
from setvortex_loaded_preflight import ROOT,run
from setvortex_source_probe import run as compare
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
class SetVortex(unittest.TestCase):
 @unittest.skipUnless(BOOT.exists(),'Requires source-matched original startup cache')
 def test_full_caller_and_source_geometry(self):
  with tempfile.TemporaryDirectory(prefix='setvortex-test-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);r=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',BOOT,p/'native',105)
   self.assertTrue(all(not x['state']['flags']&4096 for x in r['records']))
   self.assertTrue(all(x['state']['rng_draws']==0 for x in r['records']))
   compare(p/'native/manifest.json',p/'source')
if __name__=='__main__':unittest.main()
