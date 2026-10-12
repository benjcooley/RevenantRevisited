"""Observe every genuine Nakrnoth terrain query without replacing its return."""
import argparse,json,struct
from collections import Counter
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_ECX
from nakrnoth_visual_reference import NakrnothScene
from character_native_preflight import ROOT,sha

def run(executable,boot_image,baseline,output):
 output.mkdir(exist_ok=False,parents=True)
 original=json.loads(baseline.read_text())['records'];f=NakrnothScene(executable,boot_image,fast=True);v=f.vm;call=f.world.call;queries=[];pending=None;tick=0;states=[]
 def observe(uc,address,size,data):
  nonlocal pending
  if address==0x403082:
   assert pending is None
   sp=uc.reg_read(UC_X86_REG_ESP);ptr,level,option=struct.unpack('<3I',uc.mem_read(sp,12))
   pending=dict(tick=tick,position=list(struct.unpack('<3i',uc.mem_read(ptr,12))),level=level,option=option,map_this=hex(uc.reg_read(UC_X86_REG_ECX)))
  else:
   assert pending is not None
   value=uc.reg_read(UC_X86_REG_EAX);pending['returned_height']=value if value<0x80000000 else value-0x100000000;queries.append(pending);pending=None
 for address in(0x403082,0x403087):v.uc.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
 for tick in range(1,len(original)+1):
  v.put_u32(0x65cb38,tick);call(0x470cc0,this=f.owner);call(0x4de800,this=f.owner);f.raster_enabled=False;f.render();state=f.state()
  for k in('flags','frame','seed','rng_draws'):assert state[k]==original[tick-1]['state'][k],(tick,k)
  states.append(dict(tick=tick,**{k:state[k]for k in('flags','frame','seed','rng_draws')}))
  if tick%100==0:print(tick,len(queries),flush=True)
 assert pending is None and queries and all(q['returned_height']==0 and q['level']==0 and q['option']==0 and q['map_this']=='0x6668d8'for q in queries)
 report=dict(status='pass',effect='Nakrnoth',type_id='0xad92bd21',executable_sha256=sha(executable),boot_image_sha256=sha(boot_image),baseline=dict(path=str(baseline.resolve()),sha256=sha(baseline)),original_call='0x403082 calls 0x452e10; nonreplacing before/after observers at0x403082/0x403087',query_count=len(queries),return_histogram=dict(Counter(q['returned_height']for q in queries)),position_bounds=[[min(q['position'][i]for q in queries),max(q['position'][i]for q in queries)]for i in range(3)],queries=queries,states=states,scope='All790 original NextFrame/whole owner Pulse/Animate and full default Render ticks in declared empty-map screen-prefix fixture. Terminal raster omitted only; original ground function and every query execute unchanged. All owner flags/frame/RNG states equal frozen full reference. Zero-height source callback applies only to this explicit fixture, not natural terrain.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print({k:report[k]for k in('status','query_count','return_histogram','position_bounds')},flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.boot_image,a.baseline,a.output)
