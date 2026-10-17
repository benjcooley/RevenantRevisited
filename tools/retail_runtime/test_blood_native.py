"""Exact whole registered Blood asset, genuine controller and production submission."""
import tempfile,unittest
from pathlib import Path
from blood_native_actor_reference import run as native
from blood_state_probe import run as state
from blood_profile_probe import run as profile
from blood_owner_probe import run as owner
from blood_rng_probe import run as rng
class BloodNativeTests(unittest.TestCase):
 def test_live_random_provider_retains_unreseeded_stream(self):
  with tempfile.TemporaryDirectory(prefix='blood-rng-')as t:r=rng(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertGreater(r['provider_draws'],r['first_lifetime_draws'])
 def test_whole_resource_rejects_foreign_path_and_body(self):
  with tempfile.TemporaryDirectory(prefix='blood-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['accept'],r['reject']),(4,2156))
 def test_genuine_original_controller_and_source_whole_lifetime(self):
  with tempfile.TemporaryDirectory(prefix='blood-native-')as t:
   p=Path(t);n=native(p/'native',90,True,2,True,True,False);r=state(p/'source',p/'native/manifest.json')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual((r['state_fields'],r['matrix_fields'],r['color_fields'],r['rng_fields']),(32760,11104,2776,91));self.assertEqual(r['max_state_error'],0);self.assertLess(r['max_matrix_error'],2e-5);self.assertEqual(n['initialization']['parameters'],[40,224,3,5,5,1]);self.assertEqual(n['initialization']['map_ambient'],52);self.assertEqual(n['rows'][59]['done'],1);self.assertTrue(n['rows'][60]['dead_no_calls']);self.assertEqual(n['actor_before']['health'],n['actor_after']['health'])
 def test_actual_submission_all_vertices_origin_and_translated(self):
  with tempfile.TemporaryDirectory(prefix='blood-owner-')as t:r=owner(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['geometry_checks'],r['draw_checks']),(5552,1388))
if __name__=='__main__':unittest.main()
