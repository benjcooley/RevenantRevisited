"""Strict geometry admission and full original custom factory/render controls."""
import tempfile,unittest,zipfile
from pathlib import Path
from literal_water_mesh_contract import run
from literal_water_visual_reference import NativeLiteralWater,ROOT
class LiteralWaterMesh(unittest.TestCase):
 def test_actual_guard_mutations(self):
  with tempfile.TemporaryDirectory()as d:
   report=run(Path(d)/'proof');self.assertEqual(report['status'],'pass');self.assertEqual(report['positive'],3);self.assertEqual(report['negative'],84)
 def test_original_whole_factory_warmup_render_is_readonly_and_repeatable(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/water.i3d')
  reference=None
  for _ in range(2):
   fixture=NativeLiteralWater(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,'Water');fixture.animate(1);before=fixture.state();raw,depth,record=fixture.render_whole();self.assertEqual(before,fixture.state());self.assertEqual(len(record['draws']),25);self.assertTrue(any(raw));self.assertEqual(fixture.vm.u32(fixture.animator),0x5ad4e4);self.assertEqual(fixture.constructor['exact_builder'],'0x66ce98')
   if reference is not None:self.assertEqual(reference,(raw,depth,before))
   reference=raw,depth,before
if __name__=='__main__':unittest.main()
