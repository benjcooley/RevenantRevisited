"""Exact IrisFlare admission, controller cache and complete reset matrices."""
import tempfile,unittest
from pathlib import Path
from irisflare_state_probe import run as state
from irisflare_profile_probe import run as profile
from irisflare_owner_probe import run as owner
class IrisFlareTests(unittest.TestCase):
 def test_whole_own_resource_and_legacy_channels(self):
  with tempfile.TemporaryDirectory(prefix='irisflare-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['accept'],r['reject'],r['native_pose_channels']),(4,10401,18))
 def test_actual_controller_matrices_cache_reset_and_restart(self):
  with tempfile.TemporaryDirectory(prefix='irisflare-state-')as t:r=state(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual((r['state_fields'],r['matrix_fields']),(4715,23264));self.assertLess(r['max_state_error'],3e-5);self.assertLess(r['max_matrix_error'],2e-4)
  self.assertEqual([len(r['rows'][i-1]['draws'])for i in(19,20,200,201,202)],[8,7,7,3,8]);self.assertEqual((r['rows'][200]['state']['ticks'],r['rows'][200]['state']['iterations']),(0,0));self.assertEqual(r['rows'][201]['state']['ticks'],1);self.assertGreater(r['rows'][201]['state']['rotation'],20)
 def test_actual_submit_keeps_translated_owner_geometry(self):
  with tempfile.TemporaryDirectory(prefix='irisflare-owner-')as t:r=owner(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['geometry_checks'],2232)
if __name__=='__main__':unittest.main()
