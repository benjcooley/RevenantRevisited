"""Original Blast keys, source opt-in/default controls and whole-render replay."""
from pathlib import Path
import tempfile
import unittest
from blast_pose_probe import run as poses
from blast_visual_reference import ROOT,run as render

class BlastTests(unittest.TestCase):
 def test_all_authored_keys_and_default_decoder_control(self):
  with tempfile.TemporaryDirectory(prefix='blast-keys-')as tmp:
   result=poses(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'poses')
  self.assertEqual(result['status'],'pass')
  self.assertEqual(result['key_fields'],60480)
  self.assertEqual(result['matrix_fields'],25680)
  self.assertIn('default_unchanged=6720 inclusive_changed=1439',result['controls'])
  self.assertFalse(result['accepted'])
 def test_whole_original_render_and_size_rejection(self):
  with tempfile.TemporaryDirectory(prefix='blast-render-')as tmp:
   result=render(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp)/'render',[2,60])
  self.assertEqual(result['warm_replays'],2)
  self.assertEqual(set(result['initialization']['blend_modes']),{80})
  late=result['cases'][1]
  self.assertEqual(len(late['draws']),17)
  self.assertEqual(next(d for d in late['draws']if d['object']==8)['oversized_triangles'],2)
  self.assertNotIn('oversized_triangles',next(d for d in late['draws']if d['object']==9))
  self.assertGreater(late['nonzero_pixels'],0)
  self.assertFalse(result['accepted'])
if __name__=='__main__':unittest.main()
