"""Named spell constants and complete original state/geometry comparison."""
import json,tempfile,unittest
from pathlib import Path
from essencedrain_spell_state_probe import run
class EssenceDrainSpellTests(unittest.TestCase):
 def test_reject_unrelated_names_before_guest_start(self):
  from manadrain_target_visual_reference import ManaDrainTargetScene
  for name in ("Essencedrain","Worm","YManaDrain",""):
   with self.subTest(name=name),self.assertRaises(ValueError):ManaDrainTargetScene(None,None,spell_name=name)
 def test_complete_original_reference(self):
  fixture=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-essence-viewer-20261011/essence-spell-native-manifest.json')
  if not fixture.exists():self.skipTest('Licensed private original reference required')
  with tempfile.TemporaryDirectory(prefix='essence-spell-')as t:r=run(fixture,Path(t)/'proof')
  self.assertEqual(r['status'],'pass');self.assertGreater(r['fields'],10000);self.assertGreater(r['pose_fields'],19000);self.assertTrue(all(v['rejected']for v in r['negative_controls'].values()))
if __name__=='__main__':unittest.main()
