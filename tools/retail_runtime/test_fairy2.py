"""Exact Fairy2 admission and finite native-controller regression controls."""
from pathlib import Path
import tempfile,unittest
from fairy2_profile_contract import run as profile
from fairy2_state_probe import run as state,ROOT
class Fairy2Tests(unittest.TestCase):
 def test_exact_resource_payload_mutations(self):
  with tempfile.TemporaryDirectory(prefix='fairy2-profile-')as tmp:r=profile(Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['accept'],r['reject']),(1,8529))
 def test_full_finite_lifecycle_discrete_state_and_retained_numeric_residual(self):
  with tempfile.TemporaryDirectory(prefix='fairy2-state-')as tmp:r=state(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertTrue(r['discrete_controls_pass']);self.assertEqual(r['ticks'],401);self.assertGreater(r['fields'],400000)
  self.assertEqual(r['status'],'fail');self.assertTrue(r['numerical_residual_open']);self.assertLess(r['max_error'],1e-3)
  self.assertEqual([p['draws']for p in r['lifecycle'][399:]],[0,0]);self.assertTrue(all(int(p['owner_flags'],16)&0x1000 for p in r['lifecycle'][399:]))
if __name__=='__main__':unittest.main()
