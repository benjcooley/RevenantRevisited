"""Actual named-spell RockStorm pool, render pose and owner lifetime regression."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class RockStormTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=180);self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_complete_named_target_leaf(self):
  with tempfile.TemporaryDirectory(prefix='rockstorm-')as tmp:
   out=Path(tmp);self.invoke('rockstorm_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   t=json.loads((out/'trace.json').read_text());r=t['records'];self.assertEqual(t['dispatch']['lookup']['RockStorm'],'0x66d148');self.assertEqual((t['initial']['level'],t['initial']['rocks'],t['initial']['rng_draws']),(1,5,26))
   self.assertEqual([(r[i]['state']['phase'],bool(r[i]['state']['flags']&0x1000))for i in (0,20,48,49,76,77)],[(0,False),(1,False),(2,False),(3,False),(4,False),(4,True)])
   self.assertEqual(r[49]['state']['damage_events'],['0x53f560','0x4d3750']);self.assertEqual(r[-1]['state']['rng_draws'],26)
   self.invoke('rockstorm_state_probe.py','--trace',out/'trace.json','--output',out/'source');self.invoke('rockstorm_geometry_probe.py','--trace',out/'trace.json','--output',out/'geometry')
   proof=json.loads((out/'source/manifest.json').read_text());self.assertEqual(proof['fields'],12489)
if __name__=='__main__':unittest.main()
