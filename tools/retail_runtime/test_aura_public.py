"""Exact genuine Aura core, profile and submitted owner geometry contracts."""
import tempfile,unittest
from pathlib import Path
from aura_profile_probe import run as profile
from aura_state_probe import run as state
from aura_owner_probe import run as owner
from aura_pose_compare import run as pose
E=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-aura-20261011')
POSE=E/'metal-integer-final750/run.log'
NATIVE_POSE=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-burn-aura-20261011/actor-all-bones-native-no-interpolation/manifest.json')
class AuraPublicTests(unittest.TestCase):
 def test_actual_genuine_integer_bone_queries(self):
  with tempfile.TemporaryDirectory(prefix='aura-pose-')as t:r=pose(Path(t)/'proof',NATIVE_POSE,POSE)
  self.assertEqual((r['status'],r['fields'],r['objects']),('pass',352,22));self.assertFalse(r['errors'])
 def test_exact_resource(self):
  with tempfile.TemporaryDirectory(prefix='aura-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,305))
 def test_full_original_lifetime_state_matrix_rng(self):
  with tempfile.TemporaryDirectory(prefix='aura-state-')as t:r=state(Path(t)/'proof',E/'native-frozen-repeat750/manifest.json',POSE)
  self.assertEqual((r['status'],r['state_fields'],r['matrix_fields'],r['rng_fields']),('pass',375719,280912,750));self.assertFalse(r['errors']);self.assertLess(r['max_state_error'],2e-6)
 def test_production_owner_relative_geometry(self):
  with tempfile.TemporaryDirectory(prefix='aura-owner-')as t:r=owner(Path(t)/'proof',E/'state-integer-final/source.trace')
  self.assertEqual((r['status'],r['vertices'],r['draws']),('pass',140456,35114))
if __name__=='__main__':unittest.main()
