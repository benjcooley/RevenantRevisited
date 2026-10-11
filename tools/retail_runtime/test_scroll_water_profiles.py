"""Exact typed scroll-water resource admission and fail-closed mutations."""
import unittest,tempfile
from pathlib import Path
from scroll_water_profile_contract import ROOT,run
class ScrollWaterProfileTests(unittest.TestCase):
 def test_actual_guard_all_three_resources(self):
  with tempfile.TemporaryDirectory(prefix='scroll-water-guard-')as tmp:r=run(ROOT/'data/imagery.rvi',Path(tmp))
  self.assertEqual(r['status'],'pass');self.assertEqual([c['accept']for c in r['cases']],[4,4,4]);self.assertEqual([c['reject']for c in r['cases']],[105,399,399]);self.assertEqual([c['categories']['reject vertex']for c in r['cases']],[32,224,224]);self.assertEqual([c['categories']['reject index']for c in r['cases']],[6,108,108]);self.assertFalse(r['accepted'])
if __name__=='__main__':unittest.main()
