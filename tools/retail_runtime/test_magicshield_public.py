"""Exact genuine MagicShield default-name-branch core and immutable submission."""
import tempfile,unittest
from pathlib import Path
from magicshield_profile_probe import run as profile
from magicshield_state_probe import run as state
from magicshield_owner_probe import run as owner
E=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-magicshield-20261011')
class MagicShieldPublicTests(unittest.TestCase):
 def test_resource_guard(self):
  with tempfile.TemporaryDirectory(prefix='magicshield-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,884))
 def test_full_real_variant_owner_state_render_rng(self):
  for name,minimum in [('aura',0),('fireflash',50)]:
   with self.subTest(variant=name),tempfile.TemporaryDirectory(prefix='magicshield-state-')as t:r=state(Path(t)/'proof',E/f'native-{name}-alive-final170/manifest.json',minimum)
   self.assertEqual((r['status'],r['state_fields'],r['matrix_fields']),('pass',9010,21376));self.assertFalse(r['errors']);self.assertLess(r['max_matrix_error'],2e-6)
 def test_authored_geometry_material_and_absoluteposition(self):
  with tempfile.TemporaryDirectory(prefix='magicshield-owner-')as t:r=owner(Path(t)/'proof',E/'state-production-final/source.trace')
  self.assertEqual((r['status'],r['vertices'],r['draws']),('pass',59074,1336))
if __name__=='__main__':unittest.main()
