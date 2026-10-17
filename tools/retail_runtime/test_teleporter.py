"""Exact Teleporter null-spell owner, full original draws and production core."""
import contextlib,io,json,tempfile,unittest
from pathlib import Path
from rift_teleporter_loaded_preflight import run,ROOT
from teleporter_pose_probe import run as compare
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
class Teleporter(unittest.TestCase):
 @unittest.skipUnless(BOOT.exists(),'Requires source-matched original startup cache')
 def test_null_spell_full_owner_and_draws(self):
  with tempfile.TemporaryDirectory(prefix='teleporter-test-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);r=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',BOOT,p/'native','Teleporter',60,samples=())
   self.assertIsNone(r['failure']);self.assertEqual([x['tick']for x in r['records']if x['state']['flags']&4096],list(range(51,61)))
   self.assertEqual(sum(len(x['draws'])for x in r['records']),249)
   compare(p/'native/manifest.json',p/'source');proof=json.loads((p/'source/manifest.json').read_text());self.assertEqual(proof['matrix_channels'],3984);self.assertEqual(proof['lifecycle_channels'],360)
if __name__=='__main__':unittest.main()
