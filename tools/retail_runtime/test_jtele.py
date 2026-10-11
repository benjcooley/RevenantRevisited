"""Exact two teleport resources, independent keys/cache/runtime and native bounds."""
from pathlib import Path
import tempfile,unittest
from jtele_profile_contract import run as profile,ROOT
from jtele_pose_probe import run as pose
from jtele_state_probe import run as state
from jtele_software_bounds_probe import run as bounds
class JteleTests(unittest.TestCase):
 def test_strict_two_own_resource_profiles(self):
  with tempfile.TemporaryDirectory(prefix='jtele-profile-')as tmp:r=profile(ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual([(c['accept'],c['reject'])for c in r['cases']],[(4,2871),(4,2871)])
 def test_all_authored_integer_channels_and_camera_normal_matrices(self):
  with tempfile.TemporaryDirectory(prefix='jtele-pose-')as tmp:r=pose(Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass')
  for c in r['cases']:self.assertEqual((c['key_fields'],c['matrix_fields']),(3600,5920));self.assertLess(c['max_key_error'],3e-5);self.assertLess(c['max_matrix_error'],2e-4)
 def test_full_runtime_finite50_and_preserved_strict_jteled_residual(self):
  with tempfile.TemporaryDirectory(prefix='jtele-state-')as tmp:r=state(Path(tmp)/'proof')
  a,b=r['cases'];self.assertEqual((a['status'],a['fields']),('pass',17628));self.assertFalse(a['errors']);self.assertEqual((b['status'],b['fields']),('fail',17628));self.assertEqual(len(b['errors']),3);self.assertTrue(all(e['kind']=='numeric'for e in b['errors']));self.assertLess(b['max_error'],4.6e-5)
  for c in r['cases']:self.assertTrue(all(int(t['owner_flags'],16)&0x1000 and not t['draws']for t in c['lifecycle'][49:]))
 def test_original_constants_only_beam_span_control(self):
  with tempfile.TemporaryDirectory(prefix='jtele-bounds-')as tmp:r=bounds(Path(tmp)/'proof')
  self.assertEqual(r['status'],'diagnostic_negative_control')
  for c in r['cases']:self.assertTrue(c['state_rng_vertices_matrices_unchanged']);self.assertGreater(c['controls'][1]['nonzero_pixels'],c['controls'][0]['nonzero_pixels'])
if __name__=='__main__':unittest.main()
