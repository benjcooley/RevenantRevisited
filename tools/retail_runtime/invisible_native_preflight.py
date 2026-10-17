#!/usr/bin/env python3
"""Exact Invisible custom null-invoker authored-owner native feasibility."""
import argparse,json,struct,zipfile
from pathlib import Path
from generic_mesh_visual_reference import NativeGenericMeshScene
from essencedrain_native_preflight import NativeEssence
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
from static_particles_probe import registry,punctuation
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
ASSET_SHA='dd2c7ebe6110b07b404ad4afa0ea6938ce9538448c376336931caabd6be43b52'
class NativeInvisible(NativeEssence):
 def __init__(self,exe,data):
  NativeGenericMeshScene.__init__(self,exe,data,'invisible',640,480,native_binding=(0x500830,0x66cf38,0x5aeae0),eager_pose=False);v=self.vm;self.tick=0;self.raster_enabled=True;self.object=[0]
  v.put_u32(self.owner,0x5ae8e0);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x4c,self.imagery);v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery)
  for offset,value in((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,self.asset['state0_frames'])):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
  v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  v.rng_seed=1;v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(v.random_msvc(),0),begin=0x58c582,end=0x58c582)
  v.uc.hook_add(UC_HOOK_CODE,lambda uc,a,n,u:self.object.__setitem__(0,v.u32(uc.reg_read(UC_X86_REG_ESP)+4)),begin=0x40a8f0,end=0x40a8f0)
  def draw(uc,a,n,u):
   d=self.draws[-1];o=self.object[0];d.update(object=v.u32(o+4),flags=hex(v.u32(o)),matrix=list(struct.unpack('<16f',uc.mem_read(o+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
   if self.tick==1:
    args=d['args'];d.update(vertex_hex=bytes(v.uc.mem_read(args[2],args[3]*32)).hex(),index_hex=bytes(v.uc.mem_read(args[4],args[5]*2)).hex())
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
 def initialize(self):
  v=self.vm;punctuation(self);v.call(0x401610);v.call(0x4fd890,this=self.owner);v.call(0x4fd970,this=self.animator,instruction_limit=5000000);self.animobjs=[v.call(0x40eef0,(i,),this=self.animator)for i in range(2)];assert v.u32(self.animator+0x58)==0
  return dict(constructor=self.constructor,rng_after=v.rng_seed,state_hex=bytes(v.uc.mem_read(self.animator+0xfc,0x814-0xfc)).hex(),life=v.u32(self.owner+0x184),owner_flags=hex(v.u32(self.owner+8)),object_flags=[hex(v.u32(o))for o in self.animobjs])
 def step(self):
  self.tick+=1;v=self.vm;v.put_u32(0x65cb38,self.tick)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x470cc0,this=self.owner);v.call(0x4fd8c0,this=self.owner)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x4fdae0,this=self.animator,instruction_limit=5000000)
 def render_whole(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};before=v.rng_seed;v.call(0x417d60,(0,1),this=0x65a57c);
  if not v.u32(self.owner+8)&0x1000:v.call(0x4fe040,this=self.animator,instruction_limit=100000000)
  if not self.raster_enabled:
   for d in self.draws:d.pop('projected_geometry_bounds',None)
  return v.surface_bytes('screen'),dict(tick=self.tick,frame=v.u32(self.animator+0x14),owner=owner,rng_before=before,rng_after=v.rng_seed,state_hex=bytes(v.uc.mem_read(self.animator+0xfc,0x814-0xfc)).hex(),life=v.u32(self.owner+0x184),owner_flags=hex(v.u32(self.owner+8)),draws=self.draws)
def run(output,ticks=1,capture_visual=True):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/invisible.i3d'))
 assert sha(data)==ASSET_SHA;f=NativeInvisible(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);init=f.initialize();rows=[]
 for tick in range(1,ticks+1):
  f.step();f.raster_enabled=capture_visual and tick in(1,6,16,30,35,40,100,430,431,432,459,461,480,499,500,501);raw,r=f.render_whole()
  if f.raster_enabled:
   image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,640,480);r.update(image=str(image),image_sha256=sha(image.read_bytes()),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(tick,len(r['draws']),r['pixels'],flush=True)
  rows.append(r)
 r=dict(status='bounded_native_preflight',accepted=False,type_id='0xad92bc16',asset_sha256=ASSET_SHA,configuration=dict(viewport=[640,480],screen_origin=[320,240],owner=[0,0,0],ambient_native=38,directional_native=1,seed=1),initialization=init,rows=rows,scope='Exact own Invisible resource/all93 constructors/custom500830/Init4fd970/Animate4fdae0/Render4fe040 and genuine effectInit4fd890/Pulse4fd8c0. Null generic authored owner, originalMSVC seed1, readonly resource/material/COM/cached-key/owner/normal-light/software boundaries explicit. Full null-invoker6rings/70sparks original caller/allgeometry; generic owner provider/flags-command-world callbacks and full object gameplay constructor remain declared boundaries, no source poses/colors/meshes. Port/Metal/naturalcaller/device/full acceptance separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=1);a=p.parse_args();run(a.output,a.ticks)
