"""Exact multi-texture generic resource admission and independent mutations."""
import tempfile,unittest
from pathlib import Path
from generic_static_profile_contract import ROOT,run
class GenericStaticProfiles(unittest.TestCase):
 def test_actual_guard_all_roots_and_materials(self):
  with tempfile.TemporaryDirectory(prefix='generic-static-guard-')as tmp:r=run(ROOT/'data/imagery.rvi',Path(tmp)/'proof')
  self.assertEqual(r['status'],'pass');self.assertEqual([c['accept']for c in r['cases']],[4,4]);self.assertEqual([c['reject']for c in r['cases']],[778,2717]);self.assertEqual([c['categories']['reject vertex']for c in r['cases']],[400,1848]);self.assertEqual([c['categories']['reject index']for c in r['cases']],[120,780]);self.assertEqual([c['categories']['reject material_float']for c in r['cases']],[85,34]);self.assertFalse(r['accepted'])
if __name__=='__main__':unittest.main()
