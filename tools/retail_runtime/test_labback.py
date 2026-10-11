"""Whole authored Labback keys/state and original culling controls."""
from pathlib import Path
import tempfile,unittest
from labback_state_probe import run as state,ROOT
from labback_pose_probe import run as poses
from labback_cull_probe import run as culling
class LabbackTests(unittest.TestCase):
 def test_authored_poses(self):
  with tempfile.TemporaryDirectory(prefix='labback-poses-')as tmp:r=poses(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['key_fields'],18000);self.assertEqual(r['matrix_fields'],32000)
 def test_loop_and_particle_state(self):
  with tempfile.TemporaryDirectory(prefix='labback-state-')as tmp:r=state(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['fields'],204685);self.assertTrue(r['default_zero_identical']);self.assertLess(r['max_error'],.0005)
 def test_original_cull_none_and_forced_cull_rejection(self):
  with tempfile.TemporaryDirectory(prefix='labback-cull-')as tmp:r=culling(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual([c['pixels']for c in r['cases']],[57301,0,0]);self.assertFalse(r['accepted'])
if __name__=='__main__':unittest.main()
