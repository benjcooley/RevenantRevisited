"""Exact ceyes states, native loops, normal-lit draw policy and owner teardown."""
import json,subprocess,sys,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT
class CEyesTests(unittest.TestCase):
 def invoke(self,tool,*args):
  q=subprocess.run([sys.executable,str(ROOT/'tools/retail_runtime'/tool),*map(str,args)],cwd=ROOT,capture_output=True,text=True,timeout=120)
  self.assertEqual(q.returncode,0,q.stdout+q.stderr)
 def test_all_state_keys_and_profile_controls(self):
  with tempfile.TemporaryDirectory(prefix='ceyes-pose-')as tmp:
   out=Path(tmp);self.invoke('ceyes_pose_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'manifest.json').read_text());self.assertEqual(r['status'],'pass',r)
   self.assertEqual(r['key_fields'],14436);self.assertEqual(r['matrix_fields'],4752)
   self.assertIn('profile_mutations=470',r['controls']);self.assertIn('default_unchanged=1604',r['controls']);self.assertIn('inclusive_changed=72',r['controls'])
 def test_actual_owner_two_loops_and_mode16(self):
  with tempfile.TemporaryDirectory(prefix='ceyes-owner-')as tmp:
   out=Path(tmp);self.invoke('ceyes_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only')
   trace=json.loads((out/'trace.json').read_text());r=trace['records']
   self.assertEqual([r[i]['after']['frame']for i in(118,119,120,238,239,244)],[119,0,1,119,0,5])
   self.assertTrue(all(not row['after']['flags']&0x1000 for row in r));self.assertEqual(r[-1]['after']['rng_draws'],0)
   self.assertEqual(trace['initial']['controllers'],[dict(vtable='0x5a357c',state=0,tag_frame=15,blend=16)])
   for draw in r[14]['draws']:
    self.assertEqual(draw['object'],0);self.assertEqual(draw['arguments'][1],0x112)
    self.assertEqual(draw['states']['22'],1);self.assertEqual(draw['states']['7'],1);self.assertEqual(draw['states']['14'],0)
 def test_explicit_end_state_uses_original_kill(self):
  with tempfile.TemporaryDirectory(prefix='ceyes-end-')as tmp:
   out=Path(tmp);self.invoke('ceyes_visual_reference.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out,'--state-only','--initial-state',1,'--frames',3)
   r=json.loads((out/'trace.json').read_text());self.assertFalse(r['initial']['flags']&0x1000)
   self.assertTrue(r['records'][0]['after']['flags']&0x1000);self.assertTrue(all(not v['draws']for v in r['records']))
 def test_original_integer_triangle_packets(self):
  with tempfile.TemporaryDirectory(prefix='ceyes-packets-')as tmp:
   out=Path(tmp);self.invoke('ceyes_raster_packet_probe.py',ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe','--output',out)
   r=json.loads((out/'proof.json').read_text());self.assertEqual(r['status'],'pass');self.assertEqual(r['triangles'],430);self.assertEqual(r['integer_fields'],10320)
if __name__=='__main__':unittest.main()
