#!/usr/bin/env python3
"""Full original Blood constructor/setup/animator in genuine boot/actor world.

The original EffectBurst map insertion remains open; the original public
SetParams interface is supplied its actual caller literals and native RNG.
"""
import argparse,json,struct
from pathlib import Path
from types import SimpleNamespace
from character_native_preflight import AuthenticCharacters,ROOT,SOURCE,sha
from blood_actor_preflight import BOOT
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP

def run(output,ticks=1,isolate_map_pulse=False,seed=1,origin_zero=False,selected_raster=False,capture_rasters=True):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;actor_position=(0,0,16)if origin_zero else SOURCE;actor=f.new_character('Acolyte',actor_position);before=f.dump(actor);call(0x58c575,(seed,));position=(*actor_position[:2],actor_position[2]+40);o=f.new_object('Blood',position,25,0x5acb0c);count=call(0x483300,(1,5));face=v.uc.mem_read(actor+0x36,1)[0];setter=v.u32(v.u32(o)+0x200);call(setter,(40,(face+128)&255,3,5,5,count),this=o);call(0x58c575,(seed,));assert call(0x46e8b0,this=o,instruction_limit=100000000)==1;a=v.u32(o+0x58);av=v.u32(a);assert av==0x5acd18;init=dict(owner=hex(o),animator=hex(a),setter=hex(setter),parameters=[40,(face+128)&255,3,5,5,count],owner_position=position,animator_methods=[hex(v.u32(av+i*4))for i in range(6,18)],seed=seed,state_hex=bytes(v.uc.mem_read(a+0xfc,1080)).hex(),detail=v.u32(0x5d79e8),initial_slots=[dict(index=i,used=v.u32(a+0x118+i*36),object=v.uc.mem_read(a+0x11c+i*36,1)[0],stage=v.uc.mem_read(a+0x11d+i*36,1)[0],delay=v.uc.mem_read(a+0x11f+i*36,1)[0],scale=struct.unpack("<f",v.uc.mem_read(a+0x114+i*36,4))[0])for i in range(30)])
 boundary_calls=[]
 if isolate_map_pulse:
  def world_pulse(uc,address,size,user):
   sp=uc.reg_read(UC_X86_REG_ESP);boundary_calls.append(dict(address=hex(address),receiver=hex(uc.reg_read(__import__('unicorn.x86_const',fromlist=['UC_X86_REG_ECX']).UC_X86_REG_ECX))))
   uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
  v.uc.hook_add(UC_HOOK_CODE,world_pulse,begin=0x4708e0,end=0x4708e0)
 material=[0]
 for name,args in f.world.ddraw.device_calls:
  if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 original_device=f.world.ddraw._device_method
 def device(interface,method,args):
  if method in('GetLightState','SetLightState'):
   assert interface=='IDirect3DDevice3'and args[1]==1
   if method=='GetLightState':v.put_u32(args[2],material[0])
   else:material[0]=args[2]
   return 0
  return original_device(interface,method,args)
 f.world.ddraw._device_method=device
 color=v.allocate(4);v.write(color,b'\xff\xff\xff\x00');call(0x453640,(32,1),this=0x6668d8);call(0x4536b0,(color,1),this=0x6668d8);init['map_ambient']=v.u32(0x6668d8+0x8cc);init['gamma']=v.u32(0x5d7a48)
 s=LitSoftwareFixture(exe,640,480,camera=(*actor_position[:2],0),vm=v);s.lighting((38,38,38),(1,1,1));tls=call(0x58fbf1);init['rng_after']=v.u32(tls+0x14);v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,320);draws=[];states={};current_object=[0];raster=[True]
 def object_entry(uc,addr,size,u):current_object[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
 def state_entry(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);states[v.u32(sp+4)]=v.u32(sp+8)
 v.uc.hook_add(UC_HOOK_CODE,object_entry,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,state_entry,begin=0x56d400,end=0x56d400)
 def observe(uc,addr,size,u):
  args=list(struct.unpack('<7I',uc.mem_read(uc.reg_read(UC_X86_REG_ESP)+4,28)));draws.append(dict(object=v.u32(current_object[0]+4),states=dict(states),matrix=list(struct.unpack("<16f",uc.mem_read(current_object[0]+0x58,64))),args=args,vertex_hex=bytes(uc.mem_read(args[2],args[3]*32)).hex(),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64)))))
  if not raster[0]:
   sp=uc.reg_read(UC_X86_REG_ESP);uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32)
 v.uc.hook_add(UC_HOOK_CODE,observe,begin=0x56eb30,end=0x56eb30);fixture=SimpleNamespace(vm=v,owner=o,animator=a);rows=[]
 for tick in range(1,ticks+1):
  if v.u32(o+8)&0x1000:
   s.clear();raw=v.surface_bytes('screen');image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,640,480);rows.append(dict(tick=tick,rng_after=v.u32(tls+0x14),state_hex=bytes(v.uc.mem_read(a+0xfc,1080)).hex(),done=v.u32(a+0x554),owner_flags=hex(v.u32(o+8)),draws=[],pixels=0,image=str(image),image_sha256=sha(image),dead_no_calls=True));continue
  raster[0]=capture_rasters and(not selected_raster or tick in(1,3,6,16,30,40,50,57,58,59,60,90))
  v.put_u32(0x65cb38,tick);flags={"before":hex(v.u32(o+8))};call(0x470cc0,this=o);flags["next_frame"]=hex(v.u32(o+8));call(0x4f18a0,this=o);flags["pulse"]=hex(v.u32(o+8));call(0x4f1c60,(1,),this=a);flags["animate"]=hex(v.u32(o+8));owner=native_owner(fixture);s.clear();draws.clear();call(0x416f70,this=0x65a57c);call(0x40e460,this=a);flags["prerender"]=hex(v.u32(o+8));v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera);call(0x4f1de0,this=a,instruction_limit=100000000);call(0x416fb0,this=0x65a57c);raw=v.surface_bytes('screen');image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,640,480);rows.append(dict(tick=tick,rng_after=v.u32(tls+0x14),state_hex=bytes(v.uc.mem_read(a+0xfc,1080)).hex(),done=v.u32(a+0x554),flags_by_stage=flags,owner_flags=hex(v.u32(o+8)),owner_matrix=owner,draws=list(draws),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(image),image_sha256=sha(image)));print(tick,len(draws),rows[-1]['pixels'],flush=True)
 r=dict(status='bounded_original_reference',accepted=False,visual_pass=False,type_id='0xddc4042e',executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),initialization=init,actor_before=before,actor_after=f.dump(actor),rows=rows,inherited_boundaries=f.world.boundary_calls,map_pulse_boundary_calls=boundary_calls,isolate_map_pulse=isolate_map_pulse,scope='Genuine boot/Acolyte constructors/stats and exactBlood class/immutable loadedimagery/fullCreateAnimator/fullInit/Animate/PreRender/Render with original softwarelighting/kernel. Original publicSetParams receives actualEffectBurst literals and actualRandomRange count; MapPane insertion is explicitly upstream/open. Declaredcamera/sourceposition/lighting/RAMdevice boundary and logged materialHandle key1 get/set semantics, original seed1 srand; no fakebones/stats/damage/emitter/geometry. Port/Metal/appearance/naturalmap separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=1);p.add_argument('--isolate-map-pulse',action='store_true');p.add_argument('--seed',type=int,default=1);p.add_argument('--origin-zero',action='store_true');p.add_argument('--selected-raster',action='store_true');a=p.parse_args();run(a.output,a.ticks,a.isolate_map_pulse,a.seed,a.origin_zero,a.selected_raster)
