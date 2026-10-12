"""Executed retail Invisible lifetime, particles, strict asset and translated draws."""
import tempfile,unittest
from pathlib import Path
from invisible_native_preflight import run as native
from invisible_state_probe import run as state
from invisible_profile_probe import run as profile,ROOT
from invisible_owner_probe import run as owner
class InvisibleTests(unittest.TestCase):
 def test_whole_exact_own_resource(self):
  with tempfile.TemporaryDirectory(prefix='invisible-profile-')as t:r=profile(ROOT/'data/imagery.rvi',Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['cases'][0]['accept'],r['cases'][0]['reject']),(4,271))
 def test_original_whole_finite_owner_and_source_state_draws(self):
  with tempfile.TemporaryDirectory(prefix='invisible-state-')as t:
   p=Path(t);n=native(p/'native',501,capture_visual=False);r=state(p/'source',p/'native/manifest.json')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual((r['state_fields'],r['matrix_fields']),(221884,81792));self.assertLess(r['max_state_error'],1e-5);self.assertLess(r['max_matrix_error'],2e-4)
  self.assertEqual([len(n['rows'][i-1]['draws'])for i in(1,16,35,100,430,431,459,499,500,501)],[10,74,70,0,0,4,72,0,0,0]);self.assertEqual([int(n['rows'][i-1]['owner_flags'],16)&0x1000 for i in(499,500,501)],[0,0x1000,0x1000]);self.assertEqual(n['rows'][499]['life'],500)
 def test_actual_submit_and_original_translated_owner(self):
  with tempfile.TemporaryDirectory(prefix='invisible-owner-')as t:r=owner(Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertFalse(r['errors']);self.assertEqual((r['geometry_checks'],r['world_matrix_fields']),(6112,21120));self.assertLess(r['max_matrix_error'],2e-4)
if __name__=='__main__':unittest.main()
