"""Exact Halo public controls, default negative, strict own resource and submission."""
import tempfile,unittest
from pathlib import Path
from halo_native_visual_reference import run as native
from halo_state_probe import run as state
from halo_profile_probe import run as profile
from halo_owner_probe import run as owner
class HaloNativeTests(unittest.TestCase):
 def test_two_public_parameter_cases_and_default_blank(self):
  with tempfile.TemporaryDirectory(prefix='halo-native-')as t:
   p=Path(t);a=native(p/'case60',70,True,60,.04);b=native(p/'case20',30,True,20,.08);c=native(p/'default',3,False);r=state(p/'source',[p/'case60/manifest.json',p/'case20/manifest.json',p/'default/manifest.json'])
  self.assertEqual(r['status'],'pass');self.assertEqual((r['state_fields'],r['matrix_fields']),(412,1248));self.assertLess(r['max_matrix_error'],2e-5);self.assertTrue(int(a['rows'][60]['flags'],16)&0x1000);self.assertTrue(a['rows'][61]['dead_no_calls']);self.assertTrue(b['rows'][21]['dead_no_calls']);self.assertTrue(all(not x['draws']for x in c['rows']))
 def test_exact_whole_resource_admission(self):
  with tempfile.TemporaryDirectory(prefix='halo-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,1058))
 def test_production_all_vertices_origin_and_translated(self):
  with tempfile.TemporaryDirectory(prefix='halo-owner-')as t:r=owner(Path(t)/'proof')
  self.assertEqual((r['status'],r['vertices'],r['draws']),('pass',1872,156))
if __name__=='__main__':unittest.main()
