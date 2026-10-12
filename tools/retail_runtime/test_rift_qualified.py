"""Exact qualified Rift arithmetic/own resource proof, not unmodified fidelity."""
import tempfile,unittest
from pathlib import Path
from rift_qualified_contract import run as uv
from rift_qualified_profile_probe import run as profile
E=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-rift-caller-20261011')
class RiftQualifiedTests(unittest.TestCase):
 def test_modified_original_all_vertices_uv_and_kill(self):
  with tempfile.TemporaryDirectory(prefix='rift-qualified-uv-')as t:r=uv(Path(t)/'proof',E/'modified-parser-d-first104/manifest.json')
  self.assertEqual((r['status'],r['vertex_channels']),('pass',3168));self.assertFalse(r['errors']);self.assertLess(r['maximum_uv_error'],2e-6)
 def test_exact_own_body_runtime_header_negative_guards(self):
  with tempfile.TemporaryDirectory(prefix='rift-qualified-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,74))
if __name__=='__main__':unittest.main()
