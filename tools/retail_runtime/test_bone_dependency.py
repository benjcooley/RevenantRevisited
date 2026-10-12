"""Production legacy mirror graph/native control with genuine source actor records."""
import tempfile,unittest
from pathlib import Path
from bone_dependency_contract import run
EVIDENCE=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-burn-aura-20261011')
class BoneDependencyTests(unittest.TestCase):
 def test_original_all_bones_and_graph_boundaries(self):
  with tempfile.TemporaryDirectory(prefix='bone-dependency-')as t:r=run(Path(t)/'proof',EVIDENCE/'source-actor-pose-locals-final/run.log',EVIDENCE/'actor-all-bones-native-no-interpolation/manifest.json')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['matrix_fields'],r['graph_controls']),(352,9));self.assertLess(r['max_compiled_matrix_error'],2e-5)
if __name__=='__main__':unittest.main()
