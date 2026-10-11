#!/usr/bin/env python3
"""Whole original Blast default Render, authored keys, controllers and raster.

Immutable resource residency and named COM surfaces are fixture boundaries.
No production poses, generated geometry or fitted vertex colors enter draws.
"""
import argparse, copy, hashlib, json, struct, zipfile
from pathlib import Path
from scroll_water_visual_reference import NativeScrollScene
from speed_controller_preflight import NativeSpeed, ROOT
from static_particles_probe import registry, punctuation
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png

class NativeBlast(NativeScrollScene):
 def __init__(self,exe,data,width=640,height=480):
  if hashlib.sha256(data).hexdigest()!='9b796f41bd9113be6bf0d2d8ec9b230227296dad1fa09ed612ec5fc46588532d':raise ValueError('Expected exact shipped Blast')
  super().__init__(exe,data,'Blast',width,height)
  from unicorn import UC_HOOK_CODE
  from unicorn.x86_const import UC_X86_REG_ESP
  def observe_object(uc,address,size,user):self.current_object=self.vm.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def observe_packet(uc,address,size,user):
   self.draws[-1].update(object=self.vm.u32(self.current_object+4),matrix=list(struct.unpack('<16f',uc.mem_read(self.current_object+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))),vertices_sha256=hashlib.sha256(uc.mem_read(self.draws[-1]['args'][2],self.draws[-1]['args'][3]*32)).hexdigest())
  self.vm.uc.hook_add(UC_HOOK_CODE,observe_object,begin=0x40a8f0,end=0x40a8f0)
  self.vm.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x56eb30,end=0x56eb30)
  def observe_raster(uc,address,size,user):
   if self.draws:self.draws[-1]['lit_vertices_sha256']=hashlib.sha256(uc.mem_read(self.software.vertices,self.draws[-1]['args'][3]*36)).hexdigest()
  self.vm.uc.hook_add(UC_HOOK_CODE,observe_raster,begin=0x56d960,end=0x56d960)
  from unicorn.x86_const import UC_X86_REG_EFLAGS
  def observe_size_guard(uc,address,size,user):
   if uc.reg_read(UC_X86_REG_EFLAGS)&0x40:
    self.draws[-1]['oversized_triangles']=self.draws[-1].get('oversized_triangles',0)+1
  for address in (0x56da08,0x56da33,0x56da59,0x56da7f,0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):self.vm.uc.hook_add(UC_HOOK_CODE,observe_size_guard,begin=address,end=address)

 def setup_resources(self):
  # Zero authored scale legitimately rejects a part at this initial frame.
  self.required_pose_indices=set();NativeSpeed.setup_resources(self)
  v=self.vm;a=self.asset;data=self.data;u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);bo=a['body_offset']
  raw=r(r(r(bo+16)));p=self.bytes(data[raw:raw+a['vertices']*32]);p=self.bytes(struct.pack('<I',p));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',p)))
  v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o in a['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in ((0x20,self.bytes(struct.pack('<i',o['states'][0]['parent']))),(0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])):v.put_u32(resource+off,value)
   runs=o['texture_runs'];fmt='<'+'I'*(a['textures']+1)
   v.put_u32(resource+0x34,self.bytes(struct.pack(fmt,*runs[::2])));v.put_u32(resource+0x38,self.bytes(struct.pack(fmt,*runs[1::2])))
  materials=[];rawmat=r(bo+32)
  for i in range(a['materials']):
   mat=v.allocate(0x58);v.write(mat,data[rawmat+i*80:rawmat+i*80+80]);v.put_u32(mat+0x54,0);materials.append(mat)
  v.put_u32(self.imagery+0x34,len(materials));v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<'+'I'*len(materials),*materials)))
  self.surfaces=[];textures=[]
  for i in range(a['textures']):
   surface=copy.copy(self.software);surface.texture_object=v.allocate(4);surface.vtable=v.allocate(33*4);v.put_u32(surface.texture_object,surface.vtable)
   surface.com_trace=[];surface.locked=False;surface.references=1
   v.write(surface.vtable,bytes(v.uc.mem_read(self.software.vtable,33*4)))
   for slot,name,argc,method in ((1,'AddRef',1,surface._addref),(2,'Release',1,surface._release),(21,'GetPixelFormat',2,surface._pixel_format),(25,'Lock',5,surface._lock),(32,'Unlock',2,surface._unlock)):
    label=f'BlastTexture{i}{name}';v.handlers[label]=(argc,method);v.put_u32(surface.vtable+slot*4,v.api_address('kernel32.dll',label))
   tex=r(bo+40)+i*120;h,w=struct.unpack_from('<2I',data,tex+8);bits=r(r(tex+108))
   surface.set_texture(w,h,data[bits:bits+w*h*2]);self.surfaces.append(surface)
   t=v.allocate(0xa0);v.put_u32(t+0x94,1);v.put_u32(t+0x90,surface.texture_object);textures.append(t)
  v.put_u32(self.imagery+0x4c,len(textures));v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<'+'I'*len(textures),*textures)));v.put_u32(self.imagery+0x10,1)
 def initialize_scene(self):
  v=self.vm;dispatch=registry(self);punctuation(self);v.call(0x401610);v.call(0x40df90,(0,),this=self.animator,instruction_limit=5000000)
  self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(v.u32(self.animator+0x58))]
  assert len(self.controllers)==2
  return dict(dispatch=dispatch,controllers=[hex(v.u32(c))for c in self.controllers],flags=[hex(v.u32(o))for o in self.animobjs],blend_modes=[v.u32(o+0x348)for o in self.animobjs])
 def render(self,tick,frame):
  v=self.vm;v.put_u32(0x65cb38,tick);v.write(self.owner+0x5c,struct.pack('<h',frame));v.put_u32(self.animator+0x14,frame);owner=native_owner(self)
  self.software.clear();self.draws=[];self.render_states={}
  # Mode0 is an original no-op; mode1 updates both native state and its cache.
  v.call(0x417d60,(1,1),this=0x65a57c)
  v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  assert not any(s.locked for s in self.surfaces)
  return v.surface_bytes('screen'),v.surface_bytes('depth'),dict(owner=owner,draws=self.draws,flags=[hex(v.u32(o))for o in self.animobjs])

def run(executable,archive,output,samples):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/blast.i3d'))
 f=NativeBlast(executable,data);initialization=f.initialize_scene();cases=[]
 for tick in samples:
  raw,depth,draw=f.render(tick,tick%120);replay=f.render(tick,tick%120)
  if replay!=(raw,depth,draw):raise AssertionError('Original warm frame replay differs')
  path=output/f'native-{tick:03d}.png';save_rgb565_png(path,raw,640,480)
  cases.append(dict(tick=tick,image=str(path),image_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),rgb565_sha256=hashlib.sha256(raw).hexdigest(),depth_sha256=hashlib.sha256(depth).hexdigest(),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',raw)),**draw));print(tick,len(draw['draws']),cases[-1]['nonzero_pixels'],flush=True)
 report=dict(status='reference_complete',accepted=False,warm_replays=2,retail_type_id='0x0ea34fa3',viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,source_mode=80,initialization=initialization,cases=cases,asset_sha256=hashlib.sha256(data).hexdigest(),original_executable_sha256=hashlib.sha256(executable.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),scope='Complete original default Render40e8d0/controller Render/RenderObject40a8f0/authored keys/camera-facing matrices/FVF112 normal illumination/software RGB565 raster. Actual full registry and RefreshControllers/parser/Initialize; native owner/projector. Immutable decoded resources, material COM handles and exact three single-frame textures are explicit boundaries. Integer owner clock is a declared fixture input; natural spell/light/audio caller and original outer owner lifecycle are separate. Original textured raster retains fixed640x480 oversized-edge rejection; Metal clipping differs. No fitted geometry/colors.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--samples',type=int,nargs='+',default=[1,5,15,29,45,59,89,119]);a=p.parse_args();run(a.executable,a.archive,a.output,a.samples)
