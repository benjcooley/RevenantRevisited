"""Four strict shipped water resources and typed gate fail-closed mutations."""
import tempfile,unittest
from pathlib import Path
from authored_water_profile_contract import ROOT,run
class AuthoredWaterProfiles(unittest.TestCase):
 def test_actual_sanitized_source_admission(self):
  with tempfile.TemporaryDirectory(prefix='authored-water-guard-')as tmp:r=run(ROOT/'data/imagery.rvi',Path(tmp))
  self.assertEqual(r['status'],'pass');self.assertEqual([c['positive_cases']for c in r['cases']],[4]*4);self.assertEqual([c['rejection_cases']for c in r['cases']],[232,232,235,235]);self.assertEqual(r['water_gate_pair_checks'],169);self.assertEqual(r['water_gate_unselected_checks'],13);self.assertFalse(r['accepted'])
if __name__=='__main__':unittest.main()
