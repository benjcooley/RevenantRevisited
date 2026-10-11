#!/usr/bin/env python3
"""Bounded exact EssenceDrain default constructor, controllers and all29 roots."""
import argparse,json,struct,zipfile
from pathlib import Path
from generic_mesh_visual_reference import NativeGenericMeshScene
from static_particles_probe import registry,punctuation
from firecone_owner_probe import native_owner
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
ASSET_SHA='b16b09a84879eb089687a08b7887d8231433e8f067d891c5fd1ec67297427c39'
class NativeEssence(NativeGenericMeshScene):
 def setup_resources(self):
  v=self.vm;arrays=bytes(v.uc.mem_read(self.animator+0x30,40));super().setup_resources();v.uc.mem_write(self.animator+0x30,arrays)
 def __init__(self,exe,data):
  super().__init__(exe,data,'EssenceDrain',1280,960,native_binding=(0x40dc00,0x5e8508,0x5a370c),eager_pose=False);v=self.vm;self.tick=0;self.raster_enabled=True;self.object=[0]
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x4c,self.imagery);v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery)
  for offset,value in((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,self.asset['state0_frames'])):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
  v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  v.uc.hook_add(UC_HOOK_CODE,lambda uc,a,n,u:self.object.__setitem__(0,v.u32(uc.reg_read(UC_X86_REG_ESP)+4)),begin=0x40a8f0,end=0x40a8f0)
  def draw(uc,a,n,u):
   d=self.draws[-1];obj=self.object[0];d.update(object=v.u32(obj+4),flags=hex(v.u32(obj)),matrix=list(struct.unpack('<16f',uc.mem_read(obj+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
 def initialize(self):
  v=self.vm;self.dispatch=registry(self);punctuation(self);v.call(0x401610);v.call(0x40dd60,this=self.animator,instruction_limit=5000000);self.animobjs=[v.call(0x40eef0,(i,),this=self.animator)for i in range(29)];self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(v.u32(self.animator+0x58))];return dict(constructor=self.constructor,controllers=[dict(vtable=hex(v.u32(c)),frame=v.u32(c+8),objects=v.u32(c+0x18))for c in self.controllers],initial_objects=[dict(index=i,flags=hex(v.u32(o)),scale=list(struct.unpack('<3f',v.uc.mem_read(o+0x40,12))))for i,o in enumerate(self.animobjs)])
 def step(self):
  self.tick+=1;v=self.vm;v.put_u32(0x65cb38,self.tick)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x470cc0,this=self.owner);v.call(0x4de800,this=self.owner)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x40e2e0,this=self.animator,instruction_limit=5000000);v.call(0x40e460,(0,),this=self.animator,instruction_limit=5000000)
 def render_whole(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};v.call(0x417d60,(0,1),this=0x65a57c)
  if not v.u32(self.owner+8)&0x1000:v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  if not self.raster_enabled:
   for d in self.draws:d.pop('projected_geometry_bounds',None)
  return v.surface_bytes('screen'),dict(tick=self.tick,frame=v.u32(self.animator+0x14),owner_flags=hex(v.u32(self.owner+8)),owner=owner,draws=self.draws,object_flags=[hex(v.u32(o))for o in self.animobjs])
def run(output,ticks=201,selected=(1,2,3,4,6,16,40,100,198,199,200,201)):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/worm.i3d'))
 assert sha(data)==ASSET_SHA;f=NativeEssence(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);init=f.initialize();rows=[]
 for tick in range(1,ticks+1):
  f.step();f.raster_enabled=tick in selected;raw,r=f.render_whole()
  if f.raster_enabled:
   image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,1280,960);r.update(image=str(image),image_sha256=sha(image.read_bytes()),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(tick,len(r['draws']),r['pixels'],[d['object']for d in r['draws']],flush=True)
  rows.append(r)
 r=dict(status='bounded_native_preflight',accepted=False,type_id='0xdefeca7e',asset_sha256=ASSET_SHA,initialization=init,rows=rows,scope='Exact own modern EssenceDrain/Worm all93 constructors/default factory/fullInitialize/SetupObjects/scrolltex and blendcont/ownerPulse/Animate/wholeRender over all29roots. Zero-key26auxiliaries retain real constructor flags/scale and originalCalcObjectMatrix visibility; no exclusion by host. Immutable resource/COM and nullscript/audio owner boundaries explicit. Port/Metal/lifecycle/full acceptance separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=201);p.add_argument('--selected',default='1,2,3,4,6,16,40,100,198,199,200,201');a=p.parse_args();run(a.output,a.ticks,tuple(map(int,a.selected.split(','))))
