"""Installed CFire missing-presets contract, full original raster and production poses."""
import contextlib,io,tempfile,unittest
from pathlib import Path
from cfire_loaded_reference import run
from cfire_source_probe import compile_source,compare
BOOT=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image')
class CFire(unittest.TestCase):
 @unittest.skipUnless(BOOT.exists(),'Requires source-matched original startup cache')
 def test_installed_failed_presets_and_base_lifetime(self):
  with tempfile.TemporaryDirectory(prefix='cfire-test-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);r=run(BOOT,p/'native',32)
   self.assertEqual(r['controllers'],[])
   self.assertEqual([x['tick']for x in r['records']if x['nonzero_pixels']],list(range(1,30)))
   self.assertEqual(sum(len(x['draws'])for x in r['records']),58)
   self.assertEqual([x['path'].lower()for x in r['file_requests']if x['result']=='0xffffffff'],['.\\resources\\cfire.txt','.\\resources\\csparks.txt'])
   proof=compare(compile_source(p/'source'),p/'native/manifest.json',p/'source')
   self.assertEqual(proof['matrix_fields'],1856);self.assertEqual(proof['original_kill'],30)
if __name__=='__main__':unittest.main()
