"""Exact genuine Burn controller/profile/helper contracts."""
import tempfile,unittest
from pathlib import Path
from burn_profile_probe import run as profile
from burn_state_probe import run as state
from burn_owner_probe import run as owner
EVIDENCE=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-burn-aura-20261011')
class BurnPublicTests(unittest.TestCase):
 def test_exact_legacy_resource(self):
  with tempfile.TemporaryDirectory(prefix='burn-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,815))
 def test_full_original_state_matrix_rng_lifetime(self):
  with tempfile.TemporaryDirectory(prefix='burn-state-')as t:r=state(Path(t)/'proof',EVIDENCE/'burn-native-final90/manifest.json',EVIDENCE/'source-actor-pose-locals-final/run.log')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['state_fields'],r['matrix_fields'],r['rng_fields']),(108750,90240,90));self.assertFalse(r['errors']);self.assertLess(r['max_state_error'],3e-5)
 def test_all_authored_vertices_actual_absoluteposition_submission(self):
  with tempfile.TemporaryDirectory(prefix='burn-owner-')as t:r=owner(Path(t)/'proof',EVIDENCE/'burn-state-first/source.trace')
  self.assertEqual((r['status'],r['vertices'],r['draws']),('pass',22560,5640))
if __name__=='__main__':unittest.main()
