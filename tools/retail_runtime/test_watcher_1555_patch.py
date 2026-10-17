"""Qualified original1555 patch build, exhaustive decode and unaffected formats."""
import contextlib,io,tempfile,unittest
from pathlib import Path
from speed_controller_preflight import ROOT
from watcher_retail_1555_patch import build
from watcher_1555_patch_proof import run
class Watcher1555Patch(unittest.TestCase):
 def test_exact_patch_and_all_words(self):
  with tempfile.TemporaryDirectory(prefix='watcher-patch-')as temp,contextlib.redirect_stdout(io.StringIO()):
   p=Path(temp);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';a=build(exe,p/'build');b=build(exe,p/'repeat');self.assertEqual(a['patched_sha256'],b['patched_sha256']);r=run(exe,p/'build/patch-manifest.json',p/'proof')
   self.assertEqual(r['channel_assertions'],524288);self.assertEqual(r['exact_format_rejections'],5);self.assertEqual(len(r['binary_alpha_literals']),8);self.assertTrue(all(x['identical']for x in r['original_raster_controls']))
if __name__=='__main__':unittest.main()
