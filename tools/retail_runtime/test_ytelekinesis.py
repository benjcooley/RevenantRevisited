"""Original player-centered moving chunk state, matrices and actual lifetime."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class YTelekinesisTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=180);self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_complete_stationary_player_null_spell_leaf(self):
  with tempfile.TemporaryDirectory(prefix='ytelekinesis-')as tmp:
   out=Path(tmp);self.invoke('ytelekinesis_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   t=json.loads((out/'trace.json').read_text());rows=t['records'];self.assertEqual(t['dispatch']['lookup']['YTelekinesis'],'0x66d140');self.assertEqual(len(t['initial']['rocks']),4);self.assertEqual(t['initial']['rng_draws'],45)
   self.assertEqual([r['life']for r in t['initial']['rocks']],[90,166,83,90]);self.assertFalse(rows[164]['state']['flags']&4096);self.assertTrue(rows[165]['state']['flags']&4096)
   self.assertEqual(sum(r['hit']for r in rows[90]['state']['rocks']),4);self.assertEqual(rows[-1]['state']['rng_draws'],45)
   self.assertEqual(len(rows[130]['draws']),4) # Expired chunks persist until the owner's actual kill.
   self.invoke('ytelekinesis_state_probe.py',out/'trace.json','--output',out/'source');self.invoke('ytelekinesis_geometry_probe.py','--trace',out/'trace.json','--output',out/'geometry')
   self.assertEqual(json.loads((out/'source/manifest.json').read_text())['fields'],14480)
if __name__=='__main__':unittest.main()
