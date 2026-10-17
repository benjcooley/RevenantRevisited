#!/usr/bin/env python3
"""Qualified modified-original diagnostic: one initializer bound byte4→3.

Assets, actors/callers, matrices, RNG and all ordinary geometry remain original.
This is never an unmodified retail visual pass.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from character_native_preflight import AuthenticCharacters,ROOT,sha
from blood_actor_preflight import BOOT
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

def run(out,ticks=30,width=1920,height=1080,center=(960,600),widen=False):
 out.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe';f=AuthenticCharacters(exe,BOOT);f.select_screen_prefix();v=f.vm;call=f.world.call;assert bytes(v.uc.mem_read(0x50540a,2))==b'\x6a\x04';v.uc.mem_write(0x50540b,b'\x03');o=f.new_object('LabyrinthEffect',(0,0,0),25,0x5b1248);call(0x58c575,(1,));tls=call(0x58fbf1);assert call(0x46e8b0,this=o,instruction_limit=100000000)==1;a=v.u32(o+0x58);assert v.u32(a)==0x5b1448;selectors=list(struct.unpack('<150i',v.uc.mem_read(a+0xa5c,600)));assert set(selectors)<=set((1,2,3));material=[0]
 for name,args in f.world.ddraw.device_calls:
  if name=='IDirect3DDevice3::SetLightState'and args[0]==1:material[0]=args[1]
 original=f.world.ddraw._device_method
 def device(interface,method,args):
  if interface=='IDirect3DDevice3'and method in('GetLightState','SetLightState')and args[1]==1:
   if method=='GetLightState':v.put_u32(args[2],material[0])
   else:material[0]=args[2]
   return 0
  return original(interface,method,args)
 f.world.ddraw._device_method=device;s=LitSoftwareFixture(exe,width,height,camera=(0,0,0),vm=v);s.lighting((38,)*3,(1,)*3);v.put_u32(0x675fc0,center[0]);v.put_u32(0x675fc4,center[1]);
 if widen:v.write(0x5b9cc0,struct.pack('<f',6400.));v.write(0x5b9cbc,struct.pack('<f',4800.))
 draws=[];current=[0];raster=[True]
 def obj(uc,*_):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
 def draw(uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<7I',uc.mem_read(sp+4,28)));draws.append(dict(object=v.u32(current[0]+4),matrix=list(struct.unpack('<16f',uc.mem_read(current[0]+0x58,64))),args=args,vertex_sha256=__import__('hashlib').sha256(bytes(uc.mem_read(args[2],args[3]*32))).hexdigest(),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64)))))
  if not raster[0]:
   from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP
   uc.reg_write(UC_X86_REG_EAX,0);uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+32)
 v.uc.hook_add(UC_HOOK_CODE,obj,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30);rows=[];failure=None;animate_calls=[0]
 def animate(uc,*_):animate_calls[0]+=1
 v.uc.hook_add(UC_HOOK_CODE,animate,begin=0x505430,end=0x505430)
 for tick in range(1,ticks+1):
  s.clear();draws.clear();raster[0]=tick in(1,6,16,30,60,99,100,101,170)or tick==ticks
  try:
   v.put_u32(0x65cb38,tick);call(0x470cc0,this=o);call(0x505290,this=o);call(0x416f70,this=0x65a57c);call(0x40e460,this=a);call(0x40e740,this=a);v.call(0x54de20,(s.screen.address,s.depth.address),this=s.camera);v.call(0x54dd70,(0,width-1,0,height-1,width),this=s.camera);call(0x505580,this=a,instruction_limit=100000000);call(0x416fb0,this=0x65a57c)
  except RuntimeError as e:failure=dict(tick=tick,error=str(e),fault=f.world.last_fault);break
  row=dict(tick=tick,owner_flags=v.u32(o+8),owner_frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],animate_calls=animate_calls[0],rng=v.u32(tls+0x14),draws=list(draws),owner_position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),state_hex=bytes(v.uc.mem_read(a+0xfc,4800)).hex(),raster_executed=raster[0])
  if raster[0]:
   p=out/f'modified-original-{tick:03d}.png';raw=v.surface_bytes('screen');save_rgb565_png(p,raw,width,height);row.update(image=str(p),image_sha256=sha(p),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row);print(tick,len(draws),row.get('pixels'),flush=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Misc/starfield.i3d')
 r=dict(width=width,height=height,center=center,widened_raster_constants=widen,status='qualified_modified_original_render'if not failure else'qualified_modified_original_gate',accepted=False,unmodified_visual_pass=False,patched_original=True,patch=dict(address='0x50540b',before='04',after='03',instruction='50540a push4→push3',reason='Only authored star prototypes1grey/2purple/3yellow;0starmap is texturedquad. Actual initializer uses literal1..4; no subsequent selector reassignment.'),asset_sha256=__import__('hashlib').sha256(asset).hexdigest(),executable_disk_sha256=sha(exe),probe_sha256=sha(__file__),selectors=selectors,rows=rows,failure=failure,scope=__doc__+' Full genuine class/constructor/CreateAnimator/ownerPulse/customAnimate/Render. Explicit cached camera0/empty map/RAM1280x960 ambient38dir1. No new asset, fifthmesh, caller, fitting or ordinarygeometrydrop; only actual device raster handoff skipped outside selected frames. Natural caller and unmodified retail approval remain open.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=170);p.add_argument('--widen',action='store_true');a=p.parse_args();run(a.output,a.ticks,widen=a.widen)
