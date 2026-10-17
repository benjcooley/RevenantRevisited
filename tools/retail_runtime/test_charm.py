"""Authored charm loop, exact RNG and original cold-to-additive transition."""
from pathlib import Path
import tempfile,unittest,zipfile
from charm_state_probe import run as state,ROOT
from charm_pose_probe import run as poses
from charm_visual_reference import NativeCharm
class CharmTests(unittest.TestCase):
 def test_all_authored_poses(self):
  with tempfile.TemporaryDirectory(prefix='charm-poses-')as tmp:r=poses(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['key_fields'],810);self.assertEqual(r['matrix_fields'],1440)
 def test_loop_state_and_default_stream(self):
  with tempfile.TemporaryDirectory(prefix='charm-state-')as tmp:r=state(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['fields'],77831);self.assertTrue(r['default_zero_identical']);self.assertLess(r['max_error'],.00005)
 def test_original_cold_normal_then_particle_additive(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:d=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/charm.i3d'))
  f=NativeCharm(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',d);r=f.initialize_scene();self.assertEqual(len(r['controllers']),1);f.raster_enabled=False
  raw,cold=f.render();self.assertEqual(len(cold['draws']),1);self.assertEqual(cold['draws'][0]['states'][14],1)
  f.advance();f.render();f.advance();raw,active=f.render();self.assertEqual(len(active['draws']),2);self.assertEqual(active['draws'][-1]['states'][14],0);self.assertEqual(active['draws'][-1]['states'][22],1)
if __name__=='__main__':unittest.main()
