#!/usr/bin/env python3
"""Exact IrisFlare legacy resource and whole original compound controller."""
import argparse,json,struct,zipfile
from pathlib import Path
from barrier_wall_native_preflight import NativeBarrierWall,ROOT
from firecone_owner_probe import native_owner
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
ASSET_SHA='f697b802a7b0edb540c3b4f23ffd864b9b5e1253979af65a1a51c95ff70f30c5'
def read_iris(data):
 assert sha(data)==ASSET_SHA
 u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];rel=lambda p:p+u(p);b=20+u(16)
 assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),h(70))==(4,186,204,2,2,2,1)
 vertices=rel(rel(b+8));faces=rel(b+268);keybase=rel(b+6620);objects=[]
 for i in range(2):
  parent=struct.unpack_from('<b',data,rel(b+7132+i*4))[0];assert parent==-1
  start=h(b+4060+i*2);count=h(b+4188+i*2)
  objects.append(dict(index=i,name=data[b+1884+i*32:b+1916+i*32].split(b'\0')[0].decode(),material=h(b+3932+i*2),vertex_start=start,vertex_count=count,vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex(),texture_runs=[v for t in range(3)for v in(h(b+4316+i*16+t*2),h(b+5340+i*16+t*2))],states=[dict(parent=parent,keys=[0],legacy_key_hex=data[keybase+i*8:keybase+(i+1)*8].hex())]))
 textures=[]
 for i in range(2):
  desc=b+920+i*108;hh,w=struct.unpack_from('<2I',data,desc+8);assert (hh,w,u(b+1848+i*4))==(64,64,1)
  pixel=rel(rel(b+1784+i*4));pf=data[desc+72:desc+104];assert struct.unpack('<8I',pf)[3:]==(16,0xf00,0xf0,0xf,0xf000)
  textures.append(dict(width=w,height=hh,pixel_format=pf.hex(),raw=data[pixel:pixel+w*hh*2].hex()))
 return dict(name='IrisFlare',sha256=ASSET_SHA,body_offset=b,flags=4,version=0,vertices=186,faces=204,materials=2,textures=2,states=1,state0_frames=1,objects=objects,tags=[],face_hex=data[faces:faces+204*6].hex(),vertex_hex=data[vertices:vertices+186*32].hex(),material_hex=[data[b+276+i*80:b+356+i*80].hex()for i in range(2)],texture_data=textures)
class NativeIris(NativeBarrierWall):
 def __init__(self,exe,data):
  super().__init__(exe,data,('IrisFlare','0x120054d1',0x4f5f50,0x66cc58,ASSET_SHA),read_iris(data),0x5a9f2c,0x4e5fc0)
  v=self.vm;self.current=[0]
  def obj(uc,a,n,u):self.current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def draw(uc,a,n,u):
   d=self.draws[-1];o=self.current[0];d.update(object=v.u32(o+4),flags=hex(v.u32(o)),matrix=list(struct.unpack('<16f',uc.mem_read(o+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))),vertex_sha256=sha(bytes(uc.mem_read(d['args'][2],d['args'][3]*32))))
  v.uc.hook_add(UC_HOOK_CODE,obj,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
 def state(self):
  v=self.vm;return dict(rotation=struct.unpack('<f',v.uc.mem_read(self.animator+0xfc,4))[0],positions=list(struct.unpack('<15f',v.uc.mem_read(self.animator+0x100,60))),iterations=v.u32(self.animator+0x13c),ticks=v.u32(self.animator+0x140),cylsizes=list(struct.unpack('<2f',v.uc.mem_read(self.animator+0x144,8))),growth=list(struct.unpack('<2f',v.uc.mem_read(self.animator+0x14c,8))),flare_angle=struct.unpack('<f',v.uc.mem_read(self.animobjs[0]+0x30,4))[0])
 def advance(self):
  self.tick+=1;self.vm.put_u32(0x65cb38,self.tick);self.vm.call(0x4e6070,this=self.animator,instruction_limit=10000000)
 def render(self):
  owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};self.vm.call(0x4e61b0,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen'),dict(tick=self.tick,owner=owner,state=self.state(),draws=self.draws,rng_seed=self.vm.rng_seed)
def run(output,ticks=205,selected=(1,6,19,20,25,26,49,50,100,199,200,201,202,205)):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/irisflare.i3d')
 f=NativeIris(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);initial=f.state();rows=[]
 for tick in range(1,ticks+1):
  f.advance();f.raster_enabled=tick in selected;raw,row=f.render();row['raster_executed']=f.raster_enabled
  if f.raster_enabled:
   image=output/f'native-{tick:03d}.png';save_rgb565_png(image,raw,1280,960);row.update(image=str(image),image_sha256=sha(image.read_bytes()),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(tick,len(row['draws']),row['nonzero_pixels'],row['state']['iterations'],row['state']['ticks'],flush=True)
  rows.append(row)
 report=dict(status='native_reference_complete',accepted=False,name='IrisFlare',type_id='0x120054d1',asset_sha256=ASSET_SHA,constructor=f.constructor,original_sha256=sha((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()),initial_state=initial,rows=rows,scope='Exact own legacy resource and full original custom factory/all93 constructors/Initialize/Animate/RenderObject; five base flares and two cylinder draws plus early expanding third cylinder. Selected full normal-lit ARGB4444 software raster, original controller/render packet execution at all intervening ticks. Upstream decoded readonly resource/COM residency, original zeroing allocator, null owner spell/command callback, cached initial legacy keys, owner/FIX/facing, ambient38/directional1/cull2/depth and no actor are explicit bounds. Teleporter is a separate factory/caller. Source/Metal/natural/device/exact pixels are separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=205);p.add_argument('--selected',default='1,6,19,20,25,26,49,50,100,199,200,201,202,205');a=p.parse_args();run(a.output,a.ticks,tuple(map(int,a.selected.split(','))))
