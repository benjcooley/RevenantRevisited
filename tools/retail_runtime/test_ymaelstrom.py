"""Own YMaelstrom profile, original caller and parser proof; base is not an alias."""
import json,tempfile,unittest
from pathlib import Path
import test_maelstrom
from speed_profile_contract import ROOT
class YMaelstromTests(unittest.TestCase):
 invoke=test_maelstrom.MaelstromTests.invoke
 def test_own_profile_and_default_leaf(self):
  with tempfile.TemporaryDirectory(prefix='ymaelstrom-')as tmp:
   out=Path(tmp);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
   self.invoke('maelstrom_pose_probe.py',exe,'--variant','ymaelstrom','--output',out/'pose')
   p=json.loads((out/'pose/manifest.json').read_text());self.assertEqual(p['status'],'pass');self.assertTrue(p['opposite_profile_rejected']);self.assertEqual(p['variant'],'ymaelstrom');self.assertEqual(p['key_fields'],112455);self.assertEqual(p['matrix_fields'],77504)
   self.invoke('maelstrom_visual_reference.py',exe,'--variant','ymaelstrom','--state-only','--output',out/'native')
   t=json.loads((out/'native/trace.json').read_text());self.assertEqual(t['type_id'],'0xae5eeb26');self.assertEqual(t['dispatch']['lookup']['ymaelstrom'],'0x5e8508');self.assertEqual(t['asset_sha256'],'513260c6254726d8c8afecce835218a74efd31410f12f319812a8a81eaaf016e')
   self.assertEqual([c['objects']for c in t['initial']['controllers'][:3]],[[4],[2],[3]])
   self.assertFalse(t['records'][243]['after']['flags']&0x1000);self.assertTrue(t['records'][244]['after']['flags']&0x1000)
   self.assertEqual(t['records'][-1]['after']['clock'],260);self.assertEqual(t['records'][-1]['after']['rng_draws'],0)
   for r in t['records']:
    for d in r['draws']:self.assertNotIn(d['object'],range(20,26))
 def test_control_compatibility_and_own_uv(self):
  with tempfile.TemporaryDirectory(prefix='ymaelstrom-layout-')as tmp:
   out=Path(tmp);self.invoke('ymaelstrom_compatibility_probe.py','--output',out/'compatibility.json')
   p=json.loads((out/'compatibility.json').read_text());self.assertEqual(p['status'],'pass');self.assertNotEqual(p['base_asset_sha256'],p['y_asset_sha256'])
   self.invoke('maelstrom_scroll_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--variant','ymaelstrom','--output',out/'scroll')
   p=json.loads((out/'scroll/proof.json').read_text());self.assertEqual(p['status'],'pass');self.assertEqual(p['uv_fields'],2304);self.assertEqual(p['max_uv_error'],0)
if __name__=='__main__':unittest.main()
