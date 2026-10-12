#!/usr/bin/env python3
"""Genuine exact Labyrinth class/factory/resource/caller failure, no prototype repair."""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT
from lit_software_probe import LitSoftwareFixture
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX
TYPE=0xad92bd20

def single():
 exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;o=f.new_object('LabyrinthEffect',(0,0,0),25,0x5b1248);owner_initial=bytes(v.uc.mem_read(o,0x184)).hex();call(0x58c575,(1,));tls=call(0x58fbf1);assert call(0x46e8b0,this=o,instruction_limit=100000000)==1;a=v.u32(o+0x58);im=v.u32(o+0x54);assert v.u32(a)==0x5b1448;count=v.u32(a+0x44);selectors=list(struct.unpack('<150i',v.uc.mem_read(a+0xa5c,600)));material=[0]
 for name,args in f.world.ddraw.device_calls:
  if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 original=f.world.ddraw._device_method
 def device(interface,method,args):
  if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
   if method=='GetLightState':v.put_u32(args[2],material[0])
   else:material[0]=args[2]
   return 0
  return original(interface,method,args)
 f.world.ddraw._device_method=device;s=LitSoftwareFixture(exe,640,480,camera=(0,0,0),vm=v);s.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,320);lookups=[];pending=[]
 def lookup(uc,addr,*_):
  sp=uc.reg_read(UC_X86_REG_ESP)
  if v.u32(sp)==0x5055cb:pending.append(v.u32(sp+4))
 def returned(uc,*_):lookups.append(dict(index=pending.pop(),result=hex(uc.reg_read(UC_X86_REG_EAX))))
 v.uc.hook_add(UC_HOOK_CODE,lookup,begin=0x40eef0,end=0x40eef0);v.uc.hook_add(UC_HOOK_CODE,returned,begin=0x5055cb,end=0x5055cb);failure=None;stage='NextFrame'
 try:
  v.put_u32(0x65cb38,1);call(0x470cc0,this=o);stage='real Labyrinth owner Pulse';call(0x505290,this=o);stage='custom Animator Pulse';call(0x505430,this=a);stage='BeginScene/PreRender';call(0x416f70,this=0x65a57c);call(0x40e460,this=a);call(0x40e740,this=a);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera);stage='whole Render505580';call(0x505580,this=a,instruction_limit=100000000)
 except RuntimeError as error:failure=dict(stage=stage,error=str(error),fault=f.world.last_fault)
 return dict(owner_vtable=hex(v.u32(o)),animator_vtable=hex(v.u32(a)),loaded_resource=v.string(v.u32(im+4)),objects=count,prototype_selectors=selectors,owner_initial_sha256=hashlib.sha256(bytes.fromhex(owner_initial)).hexdigest(),owner_position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),rng_after=v.u32(tls+0x14),lookups=lookups,failure=failure,executable_sha256=sha(exe),boot_sha256=sha(BOOT),inherited_boundaries=f.world.boundary_calls)

def run(out):
 out.mkdir(parents=True,exist_ok=False);a=single();b=single();stable=('owner_vtable','animator_vtable','loaded_resource','objects','prototype_selectors','owner_position','rng_after','lookups','failure');assert all(a[k]==b[k]for k in stable);assert a['objects']==4 and 4 in a['prototype_selectors']and a['lookups'][-1]==dict(index=4,result='0x0');assert a['failure'];r=dict(status='genuine_loaded_prototype_gate_reproduced',accepted=False,visual_pass=False,type_id=hex(TYPE),cases=[a,b],exact_semantic_replays=2,probe_sha256=sha(__file__),scope='Genuine shipped typed LabyrinthEffect NewObject, full original TEffect/own constructor and actual CreateAnimator/custom150-star Init. Real owner505290 follows cached camera minus360; full actual Pulse/Begin/PreRender/Render reaches GetObject4 null dereference with exact own4-object Starfield. Device Get/SetLightState material-handle COM only; no owner/prototype/selector/map/effect/renderer substitution. This proves a declared loaded fixture gate, not universally broken/unused retail gameplay. Natural caller/resource override is a separate read-only archive/xref audit; no visual credit.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],a['loaded_resource'],a['lookups'],a['failure']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
