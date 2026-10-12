"""Complete exact EssenceDrain admission,29rootpose,liveUV and owner contracts."""
import tempfile,unittest
from pathlib import Path
from essencedrain_native_preflight import ROOT
from essencedrain_profile_probe import run as profile
from essencedrain_pose_probe import run as pose
from essencedrain_uv_probe import run as uv
from essencedrain_owner_probe import run as owner
class EssenceDrainTests(unittest.TestCase):
 def test_exact_own_profile_with_zero_key_auxiliaries(self):
  with tempfile.TemporaryDirectory(prefix='essence-profile-')as t:r=profile(ROOT/'data/imagery.rvi',Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual([(c['accept'],c['reject'])for c in r['cases']],[(4,4053)])
 def test_actual_pose_branch_retains_all26_identity_auxiliaries(self):
  with tempfile.TemporaryDirectory(prefix='essence-pose-')as t:r=pose(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual((r['key_fields'],r['matrix_fields'],r['zero_key_identity_matrices'],r['native_scale_rejections']),(5400,83296,5200,594))
 def test_full_live_three_cylinder_uv_cache_and_finite_tail(self):
  with tempfile.TemporaryDirectory(prefix='essence-uv-')as t:r=uv(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual(r['fields'],57312);self.assertLess(r['max_error'],2e-5)
 def test_actual_submit_translated_owner_for_all_authored_vertices(self):
  with tempfile.TemporaryDirectory(prefix='essence-owner-')as t:r=owner(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['geometry_checks'],3768)
if __name__=='__main__':unittest.main()
