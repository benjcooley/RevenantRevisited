#!/usr/bin/env python3
"""Actual original one-frame owner-order and uninitialized strip-width controls."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from windstrip_visual_reference import WindStripScene,ROOT,sha

def run(exe,archive,out):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/wind.i3d'))
 cases=[]
 for control in('original','reversed_owner_order','incoming_width_step_one'):
  f=WindStripScene(exe,data);f.raster_enabled=False;v=f.vm
  if control=='reversed_owner_order':v.call(0x470cc0,this=f.owner);v.call(0x522800,this=f.owner)
  else:f.advance()
  if control=='incoming_width_step_one':
   def width(uc,*_):v.put_u32(uc.reg_read(UC_X86_REG_ESP)+0x1c,0x3f800000)
   v.uc.hook_add(UC_HOOK_CODE,width,begin=0x50be30,end=0x50be30)
  f.render();cases.append(dict(control=control,state=f.state(),draws=len(f.draws),first_vertices_sha256=sha(bytes.fromhex(f.draws[0]['vertices_hex']))if f.draws else None))
 assert cases[0]['state']['alive'] and cases[0]['state']['frame']==1 and cases[0]['draws']==20
 assert not cases[1]['state']['alive'] and cases[1]['state']['frame']==0 and cases[1]['draws']==0
 assert cases[0]['state']==cases[2]['state'] and cases[0]['first_vertices_sha256']!=cases[2]['first_vertices_sha256']
 report=dict(status='pass',cases=cases,scope='Actual original Pulse→NextFrame→Animate versus reversed ordering; original equal-width stack remains unmodified for primary reference. Explicit width-step1 negative control changes only the existing uninitialized local read, proving caller-stack sensitivity without endorsing that value.',probe_sha256=sha(Path(__file__).read_bytes()))
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
