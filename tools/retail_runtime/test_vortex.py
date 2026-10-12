"""Exact shipped default Vortex legacy pose, owner placement and raster contracts."""
import tempfile,unittest
from pathlib import Path
from vortex_legacy_contract import run as keys
from vortex_owner_contract import run as owner
from vortex_depth_contract import run as depth
from vortex_material_contract import run as material
class VortexTests(unittest.TestCase):
 def test_authored_material_forwarding_rejects_zero_alpha(self):
  with tempfile.TemporaryDirectory(prefix="vortex-material-")as tmp:r=material(Path(tmp)/"proof")
  self.assertEqual((r["status"],r["materials"],r["channels"]),("pass",2,34))
 def test_exact_legacy_keys_texture_payload_and_default_other_paths(self):
  with tempfile.TemporaryDirectory(prefix='vortex-keys-')as tmp:r=keys(Path(tmp)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject'],r['native_pose_channels']),('pass',4,3676,27));self.assertEqual(r['max_pose_error'],0)
 def test_actual_translated_owner_composition_all_authored_vertices(self):
  with tempfile.TemporaryDirectory(prefix='vortex-owner-')as tmp:r=owner(Path(tmp)/'proof')
  self.assertEqual((r['status'],r['authored_vertices'],r['geometry_checks']),('pass',205,2460))
 def test_native_positive_face_cull_and_integer_max_vertex_depth(self):
  with tempfile.TemporaryDirectory(prefix='vortex-depth-')as tmp:r=depth(Path(tmp)/'proof')
  self.assertEqual((r['status'],r['triangles'],r['positive_faces'],r['source_faces']),('pass',304,148,148));self.assertFalse(r['cull_errors']);self.assertEqual(r['integer_depth_differences'],0);self.assertLess(r['max_depth_error'],.001)
if __name__=='__main__':unittest.main()
