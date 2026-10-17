"""Exact base Ribbon key/defaultoldbody controls and original state/RNG."""
from pathlib import Path
import tempfile,unittest
from ribbon_legacy_contract import run as keys
from ribbon_state_probe import run as state
from ribbon_owner_contract import run as owner
class RibbonTests(unittest.TestCase):
 def test_exact_keys_guards_and_untouched_default_oldbody_policy(self):
  with tempfile.TemporaryDirectory(prefix='ribbon-keys-')as tmp:r=keys(Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['accept'],r['reject'],r['native_pose_channels']),(4,2183,18));self.assertLess(r['max_pose_error'],1e-7)
 def test_translated_owner_composes_every_authored_vertex(self):
  with tempfile.TemporaryDirectory(prefix='ribbon-owner-')as tmp:r=owner(Path(tmp)/'proof')
  self.assertEqual((r['status'],r['authored_vertices'],r['geometry_checks']),('pass',186,2232))
 def test_original_all30sparks_grow20_render_rng_and_repeated_cycles(self):
  with tempfile.TemporaryDirectory(prefix='ribbon-state-')as tmp:r=state(Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['first_render_tick'],r['ticks'],r['fields']),(1,90,22230));self.assertLess(r['max_error'],1e-6);self.assertFalse(r['errors'])
if __name__=='__main__':unittest.main()
