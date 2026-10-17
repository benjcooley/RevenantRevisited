"""Own qualified Labyrinth controller/profile/span gate contracts."""
import tempfile,unittest
from pathlib import Path
from labyrinth_qualified_contract import run as core
from labyrinth_qualified_profile_probe import run as profile
from labyrinth_qualified_bounds import run as bounds
E=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-labyrinth-20261011')
class LabyrinthQualifiedTests(unittest.TestCase):
 def test_all150stars_native_owner_cadence170_and_render_rng(self):
  with tempfile.TemporaryDirectory(prefix='labyrinth-core-')as t:r=core(Path(t)/'proof',E/'native-whole-first170/manifest.json')
  self.assertEqual(r['status'],'pass');self.assertEqual(r['state_fields'],204000);self.assertFalse(r['errors'])
 def test_exact_path_body_loop_and_mutation_rejection(self):
  with tempfile.TemporaryDirectory(prefix='labyrinth-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,76))
 def test_original_constants_only_cause_and_actual_triangle_gate(self):
  with tempfile.TemporaryDirectory(prefix='labyrinth-bounds-')as t:r=bounds(Path(t)/'proof',E)
  self.assertEqual(r['status'],'pass');self.assertEqual(r['source_original_triangle_checks'],2)
if __name__=='__main__':unittest.main()
