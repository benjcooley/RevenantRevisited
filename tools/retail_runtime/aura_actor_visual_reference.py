#!/usr/bin/env python3
"""Full genuine actor/named retail Aura spell/controller/render feasibility."""
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
PHASES=(1,3,6,16,30,120,300,500,719,720,721,743,744,745,750)
def run(output,ticks=6,interpolate=True,seed=None):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;v.put_u32(0x5d7a14,int(interpolate));actor=f.new_character('Acolyte',(0,0,16));before=f.dump(actor);assert call(0x46e8b0,this=actor,instruction_limit=100000000)==1;ca=v.u32(actor+0x58);assert call(0x53ead0,this=0x667c38,instruction_limit=100000000)==1;matches=[]
 for i in range(v.u32(0x667c3c)):
  sd=v.u32(v.u32(0x667c4c)+4*i)
  for j in range(v.u32(sd)):
   variant=v.u32(v.u32(sd+0x10)+4*j)
   if v.string(variant+4)=='Aura':matches.append((sd,variant))
 assert len(matches)==1;targets=v.allocate(4);v.put_u32(targets,actor);source=v.allocate(12);v.write(source,struct.pack('<3i',0,0,16));spell=call(0x542200,(actor,targets,1,source,*matches[0],0),this=0x670630);assert spell;o=f.new_object('Aura',(0,0,16),25,0x5a96fc);v.put_u32(o+0xd8,spell);

 if seed is not None:call(0x58c575,(seed,))
 assert call(0x46e8b0,this=o,instruction_limit=100000000)==1;a=v.u32(o+0x58);assert v.u32(a)==0x5a9900;calls=[]
 def pulse(uc,address,size,user):
  sp=uc.reg_read(UC_X86_REG_ESP);calls.append(address);uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4)
 v.uc.hook_add(UC_HOOK_CODE,pulse,begin=0x4708e0,end=0x4708e0);material=[0]
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
 f.world.ddraw._device_method=device;s=LitSoftwareFixture(exe,640,480,camera=(0,0,0),vm=v);s.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,320);v.put_u32(0x675fc4,320);draws=[];current=[0];states={};raster=[False];bones=[];bone_pending=[];ranges=[];initial_tail=bytes(v.uc.mem_read(a+0x120,16)).hex()
 def bone_entry(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP)
  if v.u32(sp)!=0x4e3cab:return
  bone_pending.append(dict(object=v.u32(sp+4),destination=v.u32(sp+4)+0x58,state=v.u32(sp+8),frame=v.u32(sp+12),actor_root=v.u32(sp+16),recursive=v.u32(sp+20),stack=sp,return_address=v.u32(sp)))
 def bone_return(uc,addr,size,u):
  if bone_pending:
   p=bone_pending.pop();p['object_index']=v.u32(p['object']+4);p['actor_root_matrix']=list(struct.unpack('<16f',uc.mem_read(p['actor_root'],64)));p['matrix']=list(struct.unpack('<16f',uc.mem_read(p['destination'],64)));bones.append(p)
 v.uc.hook_add(UC_HOOK_CODE,bone_entry,begin=0x40a420,end=0x40a420);v.uc.hook_add(UC_HOOK_CODE,bone_return,begin=0x4e3cab,end=0x4e3cab)
 def random_entry(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);ranges.append(list(struct.unpack("<2i",uc.mem_read(sp+4,8))))
 v.uc.hook_add(UC_HOOK_CODE,random_entry,begin=0x483300,end=0x483300)
 def object_entry(uc,addr,size,u):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
 def state_entry(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);states[v.u32(sp+4)]=v.u32(sp+8)
 def draw(uc,addr,size,u):
  sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<7I',uc.mem_read(sp+4,28)));draws.append(dict(object=v.u32(current[0]+4),states=dict(states),matrix=list(struct.unpack('<16f',uc.mem_read(current[0]+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))),args=args,vertices=bytes(uc.mem_read(args[2],args[3]*32)).hex()))
  if not raster[0]:uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32)
 for addr,hook in [(0x40a8f0,object_entry),(0x56d400,state_entry),(0x56eb30,draw)]:v.uc.hook_add(UC_HOOK_CODE,hook,begin=addr,end=addr)
 tls=call(0x58fbf1);fixture=SimpleNamespace(vm=v,owner=o,animator=a);rows=[];error=None
 for tick in range(1,ticks+1):
  dead=bool(v.u32(o+8)&0x1000);raster[0]=tick in PHASES;s.clear();draws.clear();bones.clear();ranges.clear()
  try:
   if not dead:
    v.put_u32(0x65cb38,tick);call(0x470cc0,this=o);call(0x4e38a0,this=o);call(v.u32(v.u32(ca)+12*4),this=ca,instruction_limit=100000000);call(0x4e3b30,(1,),this=a);owner=native_owner(fixture);call(0x416f70,this=0x65a57c);call(0x40e460,this=a);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,639,0,479,640),this=s.camera);call(0x4e3f40,this=a,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
  except Exception as exc:error=dict(tick=tick,message=str(exc),fault=f.world.last_fault);break
  raw=v.surface_bytes('screen');image=None
  if raster[0]:image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,640,480)
  rows.append(dict(tick=tick,dead_no_calls=dead,flags=hex(v.u32(o+8)),owner_frame=v.u32(o+0x188),actor=f.dump(actor),actor_state=struct.unpack('<H',v.uc.mem_read(actor+0xc,2))[0],actor_frame=struct.unpack('<h',v.uc.mem_read(actor+0x5c,2))[0],animator_frame=v.u32(a+0x128),to_add=v.u32(a+0x12c),bone_count=v.u32(a+0x124),owner_position=list(struct.unpack("<3i",v.uc.mem_read(o+0x10,12))),spell_defense=v.u32(spell+0x134),spell_offense=v.u32(spell+0x138),bones=list(bones),random_ranges=list(ranges),fire_hex=bytes(v.uc.mem_read(v.u32(a+0x100),8800)).hex(),rng_after=v.u32(tls+0x14),draws=list(draws),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))if raster[0]else None,image=str(image)if image else None,image_sha256=sha(image)if image else None));print(tick,len(draws),len(bones),rows[-1]['pixels'],rows[-1]['actor']['health'],flush=True)
 r=dict(status='bounded_actor_reference'if not error else'actor_fixture_gate',accepted=False,visual_pass=False,type_id='0x00ab1d1d',executable_sha256=sha(exe),boot_image_sha256=sha(BOOT),interpolate=interpolate,seed_after_actor_set=seed,initial_animator_tail=initial_tail,actor_before=before,actor_after=f.dump(actor),public_setter='inline TEffect.SetSpell',spell=hex(spell),variant_name='Aura',owner=hex(o),animator=hex(a),actor_animator=hex(ca),rows=rows,error=error,generic_map_pulse_boundary_calls=len(calls),inherited_boundaries=f.world.boundary_calls,scope='Genuine Acolyte full object/character/animator and Aura full class/publicSetSpell/Init/ownerPulse/Animate/PreRender/Render/softwarelighting/kernel. Actualbone GetObjectMatrix, realspell defense/offense/stat/RNG functions run; no fabricated bones/health/damage. Actor remains genuine constructor-selected static state; genericmap/world ownerPulse explicitly upstream. Declared0,0,16 source and camera, no naturalmapinsertion/caller claim.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=6);p.add_argument('--no-interpolation',action='store_true');p.add_argument('--seed',type=int);a=p.parse_args();run(a.output,a.ticks,not a.no_interpolation,a.seed)
