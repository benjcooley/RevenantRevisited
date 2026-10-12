#!/usr/bin/env python3
"""Bounded exact Jtele/Jteled default factory/controller feasibility."""
import argparse,json,struct,zipfile
from pathlib import Path
from generic_mesh_visual_reference import NativeGenericMeshScene
from scroll_water_visual_reference import NativeScrollScene
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn.x86_const import UC_X86_REG_ESP
from static_particles_probe import registry,punctuation
from unicorn import UC_HOOK_CODE
PROFILES=(('jtele','0x0c05263b','966773fb37e85cb8dd2ca26faa18071d176ec2a7bb50f2345ea64f73beb9aff8'),('jteled','0x0c05263c','72f8dacb19f20690262fd8c2d9d4c6749e213ae89dd288b44644a6e3936ccfef'))
class NativeJtele(NativeGenericMeshScene):
 def setup_resources(self):
  v=self.vm;constructor_arrays=bytes(v.uc.mem_read(self.animator+0x30,40));NativeGenericMeshScene.setup_resources(self);v.uc.mem_write(self.animator+0x30,constructor_arrays)
 def __init__(self,exe,data,name,eager_pose=True):
  NativeScrollScene.__init__(self,exe,data,name,1280,960,particle_required=True,native_binding=(0x40dc00,0x5e8508,0x5a370c),eager_pose=eager_pose);v=self.vm;self.tick=0;self.raster_enabled=True
  v.uc.hook_del(self.texture_frame_hook)
  def boundary(uc,address,size,user):self.ret(v.random_msvc(),0)if address==0x58c582 else self.ret(0,12)
  for address in(0x58c582,0x452e10):v.uc.hook_add(UC_HOOK_CODE,boundary,begin=address,end=address)
  v.rng_seed=1;v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x4c,self.imagery);v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery)
  for offset,value in((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,self.asset['state0_frames'])):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
  v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  def observe_packet(uc,address,size,user):
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x56eb30,end=0x56eb30)
 def initialize_scene(self):
  dispatch=registry(self);punctuation(self);v=self.vm;v.call(0x401610);v.put_u32(0x5d79e4,0);v.call(0x40dd60,this=self.animator,instruction_limit=5000000);prepared=list(self.animobjs);self.animobjs=[v.call(0x40eef0,(i,),this=self.animator)for i in range(len(self.asset['objects']))];assert all(a!=b for a,b in zip(prepared,self.animobjs));self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(v.u32(self.animator+0x58))];assert [v.u32(c)for c in self.controllers]==[0x5a3480,0x5a3544];self.controller=self.controllers[1];return dict(dispatch=dispatch,controllers=[dict(vtable=hex(v.u32(c)),frame=v.u32(c+8),objects=v.u32(c+0x18))for c in self.controllers],constructor=self.constructor)
 def step(self):
  self.tick+=1;v=self.vm;v.put_u32(0x65cb38,self.tick)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x470cc0,this=self.owner);v.call(0x4de800,this=self.owner)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x40e2e0,this=self.animator,instruction_limit=5000000)
  v.call(0x40e460,(0,),this=self.animator,instruction_limit=5000000)
 def render(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};before=v.rng_seed;v.call(0x417d60,(1,1),this=0x65a57c)
  if not v.u32(self.owner+8)&0x1000:v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  return v.surface_bytes('screen'),dict(tick=self.tick,frame=v.u32(self.animator+0x14),owner_flags=hex(v.u32(self.owner+8)),owner=owner,draws=self.draws,rng_before=before,rng_after=v.rng_seed)
def run(output):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name.lower()+'.i3d'));assert sha(data)==digest;attempts=[]
   for eager in (True,False):
    try:
     f=NativeJtele(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name,eager);init=f.initialize_scene();rows=[]
     for tick in range(1,54):
      f.step()
      f.raster_enabled=tick in(2,6,16,25,40,48,49,50,53);raw,row=f.render();rows.append(row)
      if f.raster_enabled:
       image=output/f'{name}-native-{tick:03d}.png';save_rgb565_png(image,raw,1280,960);row.update(image=str(image),image_sha256=sha(image.read_bytes()),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
     attempts.append(dict(eager_pose=eager,status='full_native_initialized_and53renders',rows=rows,**init));break
    except Exception as e:attempts.append(dict(eager_pose=eager,status='failed',error=str(e)))
   cases.append(dict(name=name,type_id=id,asset_sha256=digest,attempts=attempts));print(name,attempts[-1]['status'],attempts[-1].get('error',''),flush=True)
 report=dict(status='bounded_preflight',accepted=False,cases=cases,scope='Exact own Jtele/Jteled body3asset/defaultfullfactory/all93constructors/fullRefreshControllers/Initialize/full53tickdefaultPulse/Animate/Render attempted; no renderedacceptance. Original required initial pose rejection retained; secondeagerfalseattempt preservesoriginaldefaultconstructor pose and deferskeycachetoactualAnimate, nothostposes. Immutable resources/multipletextureCOM residency boundaries only; no audio or actor substitute.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
