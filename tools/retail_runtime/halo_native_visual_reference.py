#!/usr/bin/env python3
"""Whole original Halo public InitParams case; no procedural asset or caller claim."""
import argparse,json,struct
from pathlib import Path
from types import SimpleNamespace
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP
PHASES=(1,3,6,10,15,20,21,29,30,31,45,59,60,61,70)
def run(output,ticks=70,configured=True,totalframes=60,step=.04):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;o=f.new_object('Halo',(0,0,16),25,0x5ac130);before=bytes(v.uc.mem_read(o+0x184,8)).hex();params=[totalframes,struct.unpack('<I',struct.pack('<f',step))[0]]
 if configured:call(0x4f8b70,params,this=o)
 assert call(0x46e8b0,this=o,instruction_limit=100000000)==1;a=v.u32(o+0x58);assert v.u32(a)==0x5ac340;init=dict(owner=hex(o),animator=hex(a),public_setter=hex(0x4f8b70),parameters=params if configured else None,owner_params_before=before,owner_params_after=bytes(v.uc.mem_read(o+0x184,8)).hex(),scale=struct.unpack('<f',v.uc.mem_read(a+0xfc,4))[0],frame=v.u32(a+0x100))
 calls=[]
 def pulse(uc,address,size,user):
  sp=uc.reg_read(UC_X86_REG_ESP);calls.append(address);uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
 v.uc.hook_add(UC_HOOK_CODE,pulse,begin=0x4708e0,end=0x4708e0)
 material=[0]
 for name,args in f.world.ddraw.device_calls:
  if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 original=f.world.ddraw._device_method
 def device(interface,method,args):
  if method in('GetLightState','SetLightState'):
   assert interface=='IDirect3DDevice3'and args[1]==1
   if method=='GetLightState':v.put_u32(args[2],material[0])
   else:material[0]=args[2]
   return 0
  return original(interface,method,args)
 f.world.ddraw._device_method=device;s=LitSoftwareFixture(exe,640,480,camera=(0,0,0),vm=v);s.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,320);draws=[];current=[0];states={};raster=[False]
 def object_entry(uc,addr,size,u):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
 def state_entry(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);states[v.u32(sp+4)]=v.u32(sp+8)
 def draw(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<7I',uc.mem_read(sp+4,28)));draws.append(dict(object=v.u32(current[0]+4),states=dict(states),matrix=list(struct.unpack('<16f',uc.mem_read(current[0]+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))),args=args,vertices=bytes(uc.mem_read(args[2],args[3]*32)).hex()))
  if not raster[0]:uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32)
 for addr,hook in [(0x40a8f0,object_entry),(0x56d400,state_entry),(0x56eb30,draw)]:v.uc.hook_add(UC_HOOK_CODE,hook,begin=addr,end=addr)
 fixture=SimpleNamespace(vm=v,owner=o,animator=a);rows=[]
 for tick in range(1,ticks+1):
  dead=bool(v.u32(o+8)&0x1000);raster[0]=tick in PHASES;s.clear();draws.clear()
  if not dead:
   v.put_u32(0x65cb38,tick);call(0x470cc0,this=o);call(0x4f03e0,this=o);call(0x4f0430,(1,),this=a);owner=native_owner(fixture);call(0x416f70,this=0x65a57c);call(0x40e460,this=a);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera);call(0x4f0530,this=a,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
  raw=v.surface_bytes('screen');image=None
  if raster[0]:image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,640,480)
  rows.append(dict(tick=tick,dead_no_calls=dead,flags=hex(v.u32(o+8)),scale=struct.unpack('<f',v.uc.mem_read(a+0xfc,4))[0],frame=v.u32(a+0x100),draws=list(draws),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))if raster[0]else None,image=str(image)if image else None,image_sha256=sha(image)if image else None));print(tick,len(draws),rows[-1]['scale'],rows[-1]['pixels'],flush=True)
 r=dict(status='bounded_original_reference',accepted=False,visual_pass=False,type_id='0xdedbdbed',executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),initialization=init,rows=rows,generic_map_pulse_boundary_calls=len(calls),inherited_boundaries=f.world.boundary_calls,scope=f'Genuine exact class/full owner ctor and original public InitParams({totalframes},{step}) when configured={configured}; declared parameter case, no discovered natural caller. Whole original loaded12vertex mesh/Init/Animate/PreRender/Render, selected full softwarekernel phases, unselected realRender devicehandoff, finite kill/blank. Generic map/world ownerPulse explicitly upstream. No actor/stat/geometry/texture/color substitution.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=70);p.add_argument('--default-parameters',action='store_true');p.add_argument('--totalframes',type=int,default=60);p.add_argument('--step',type=float,default=.04);a=p.parse_args();run(a.output,a.ticks,not a.default_parameters,a.totalframes,a.step)
