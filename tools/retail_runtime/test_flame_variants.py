"""Exact colored Flame profile and complete native custom loop regressions."""
import tempfile,unittest,zipfile
from pathlib import Path
from flame_variants_profile_contract import run
from flame_variants_visual_reference import NativeFlameVariant,PROFILES,ROOT
class FlameVariantTests(unittest.TestCase):
 def test_exact_typed_resource_mutations(self):
  with tempfile.TemporaryDirectory(prefix='flame-variants-')as tmp:r=run(Path(tmp)/'guard')
  self.assertEqual(r['status'],'pass');self.assertEqual([(c['accept'],c['reject'])for c in r['cases']],[(4,99),(4,99)])
 def test_original_complete_factory_copyverts_and_two_loops(self):
  with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
   for name,id,_ in PROFILES:
    data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name.lower()+'.i3d'));f=NativeFlameVariant(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);init=f.initialize_scene();self.assertEqual(init['constructor']['actual_vtable'],'0x5a9dec');p=f.vm.u32(f.animobjs[0]+0xa4)
    # Original CopyVertices stores packed R255/G255/B248; all RGB5 lanes are31.
    self.assertEqual([f.vm.u32(p+i*32+16)for i in range(4)],[0xfffffff8]*4)
    images=[]
    for tick in range(37):
     if tick:f.advance()
     pixels,row=f.render();self.assertEqual(row['internal_frame'],tick%18);self.assertEqual(len(row['draws']),1);self.assertEqual(row['draws'][0]['args'][1],0x1e2);images.append(pixels)
    self.assertEqual(images[:18],images[18:36]);self.assertEqual(images[0],images[36]);self.assertEqual(len(set(images)),8)
if __name__=='__main__':unittest.main()
