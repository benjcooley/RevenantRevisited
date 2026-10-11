#!/usr/bin/env python3
"""Full original scroll controller/CopyVertices/RenderObject software scene.

Reusable for exact no-tag StillWater as well as authored scrolling roots. No
production geometry, pose, UV or material color is inserted into native draws.
"""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from speed_controller_preflight import NativeSpeed,read_asset,ROOT
from static_particles_visual_reference import NativeParticleScene
from static_particles_probe import registry,punctuation
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner

class NativeScrollScene(NativeParticleScene):
 def __init__(self,exe,data,name,width=640,height=340,particle_required=False,native_binding=None,eager_pose=True):
  self.data=data;a=read_asset(data);a['name']=name
  NativeSpeed.__init__(self,exe,a,width,height,software_factory=LitSoftwareFixture,particle_required=particle_required,native_binding=native_binding,eager_pose=eager_pose)
  self.software.lighting((38,38,38),(1,1,1));v=self.vm
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  method=v.allocate(16);v.write_code(method,b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');vt=v.allocate(0x100);v.put_u32(vt+0x24,method);v.put_u32(self.owner,vt)
  def texture_frame(uc,address,size,user):self.ret(1,8)
  self.texture_frame_hook=v.uc.hook_add(UC_HOOK_CODE,texture_frame,begin=0x40c520,end=0x40c520)
  device=v.allocate(4);devicevt=v.allocate(0xa0);v.put_u32(device,devicevt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in [(23,'ScrollSceneGetLightState',getlight),(24,'ScrollSceneSetLightState',setlight)]:v.handlers[name]=(3,method);v.put_u32(devicevt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.render_states={}
  def observe_state(uc,address,size,user):sp=v.uc.reg_read(UC_X86_REG_ESP);self.render_states[v.u32(sp+4)]=v.u32(sp+8)
  v.uc.hook_add(UC_HOOK_CODE,observe_state,begin=0x56d400,end=0x56d400)
  def observe_draw(uc,address,size,user):sp=v.uc.reg_read(UC_X86_REG_ESP);self.draws.append(dict(args=[v.u32(sp+4+i*4)for i in range(7)],states=dict(self.render_states)))
  v.uc.hook_add(UC_HOOK_CODE,observe_draw,begin=0x56eb30,end=0x56eb30)
 def setup_resources(self):
  NativeParticleScene.setup_resources(self);v=self.vm;data=self.data;a=self.asset;u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);t=r(a['body_offset']+40);h,w=struct.unpack_from('<2I',data,t+8);bits=r(r(t+108));self.software.set_texture(w,h,data[bits:bits+w*h*2],format='ARGB4444');v.put_u32(v.u32(v.u32(self.imagery+0x5c))+0x90,self.software.texture_object)
  # Native NewObject ARGB texture branch: capture below validates effects
  # through full actual RenderObject and records original blend states.
 def copy_vertices(self,*args):pass

 def initialize_scene(self):
  v=self.vm;self.dispatch=registry(self);punctuation(self);v.call(0x401610)
  v.call(0x40df90,(0,),this=self.animator,instruction_limit=5000000)
  count=v.u32(self.animator+0x58);expected=sum(t['state']==0 for t in self.asset['tags'])
  if count!=expected:raise AssertionError('Whole original controller count differs')
  self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(count)]
  for controller in self.controllers:
   if v.u32(controller)!=0x5a3480:raise ValueError('Only original scrolltex controllers admitted')
  return dict(dispatch=self.dispatch,constructor=self.constructor,controllers=[dict(vtable=hex(v.u32(c)),state=v.u32(c+4),frame=v.u32(c+8),objects=v.u32(c+0x18),rates=list(struct.unpack('<2f',v.uc.mem_read(c+0x2c,8))))for c in self.controllers],object_flags=[hex(v.u32(o))for o in self.animobjs])
 def render(self,global_tick,owner_frame,mode=0,cull=2):
  v=self.vm;v.put_u32(0x65cb38,global_tick);v.write(self.owner+0x5c,struct.pack('<h',owner_frame));v.put_u32(self.animator+0x14,owner_frame)
  owner=native_owner(self)
  for controller in self.controllers:v.call(0x401990,this=controller)
  self.software.clear();self.draws=[];self.render_states={}
  # Explicit initial normal-mode/depth/cull inputs, not fitted per effect.
  v.call(0x417d60,(mode,1),this=0x65a57c)
  for key,value in ((7,1),(14,1),(22,cull)):v.call(0x56d400,(key,value))
  for index,obj in enumerate(self.animobjs):
   if v.call(0x40a420,(obj,0,owner_frame,0,0),this=self.imagery)!=1:raise AssertionError('Original matrix rejected')
   v.call(0x40a8f0,(obj,0,owner_frame,self.animator+0xac,0xffffffff,1),this=self.imagery,instruction_limit=100000000)
  if self.software.locked:raise AssertionError('Native texture remained locked')
  # Read executed software FVF output after RenderObject. Native Y is upward;
  # the raster camera negates it. This observes all vertices, including alpha0.
  transformed=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(self.software.vertices,self.asset['objects'][0]['vertex_count']*36)))
  bounds=[min(row[0]for row in transformed),-max(row[1]for row in transformed),max(row[0]for row in transformed),-min(row[1]for row in transformed)]
  return v.surface_bytes('screen'),v.surface_bytes('depth'),dict(owner=owner,draws=self.draws,projected_geometry_bounds=bounds)


def run(executable,archive,output,names=('FlowWater','BendWater1','BendWater2'),profiles=None,width=640,height=340):
 output.mkdir(parents=True,exist_ok=False);profiles=json.loads((ROOT/'tools/retail_runtime/scrolltex_water_profiles.json').read_text())['profiles']if profiles is None else profiles;profiles=[p for p in profiles if names is None or p['name']in names];cases=[]
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()}
  for p in profiles:
   folder=output/p['name'];folder.mkdir();data=z.read(ns['imagery/'+p['asset'].lower()]);assert hashlib.sha256(data).hexdigest()==p['asset_sha256'];f=NativeScrollScene(executable,data,p['name'],width,height);init=f.initialize_scene();(folder/'initialization.json').write_text(json.dumps(init,indent=2)+'\n')
   for capture in (1,5,15,29,45,59):
    tick=capture+1;frame=tick%f.asset['state0_frames'];raw,depth,record=f.render(tick,frame);replay=f.render(tick,frame)
    if replay[0]!=raw or replay[1]!=depth:raise AssertionError('Original complete frame replay differs')
    if len(record['draws'])!=1 or record['draws'][0]['args'][1]!=0x112:raise AssertionError('Original complete FVF_VERTEX dispatch changed')
    obj=f.animobjs[0];ptr=f.vm.u32(obj+0xa4);uvs=[list(struct.unpack('<2f',f.vm.uc.mem_read(ptr+i*32+24,8)))for i in range(f.asset['objects'][0]['vertex_count'])]
    image=folder/f'native-capture{capture:03d}.png';save_rgb565_png(image,raw,width,height);cases.append(dict(name=p['name'],type_id=p['id'],capture=capture,global_tick=tick,owner_frame=frame,draw=record['draws'],owner_matrix=record['owner'],projected_geometry_bounds=record['projected_geometry_bounds'],uvs=uvs,image=str(image),image_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),rgb565_sha256=hashlib.sha256(raw).hexdigest(),depth_sha256=hashlib.sha256(depth).hexdigest(),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))))
 report=dict(status='reference_complete',cases=cases,profiles=profiles,original_executable_sha256=hashlib.sha256(executable.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),viewport=[width,height],camera=[0,0,0],owner=[0,0,0],ambient=[38]*3,directional=[1]*3,initial_mode=0,cull=2,depth_test=True,depth_write=True,accepted=False,scope='Actual default factory/all full animator registration constructors, original scrolltex registration/whole RefreshControllers/parser/Initialize/Render/CopyVertices, original owner40e470/40e740, original key/matrix, complete RenderObject40a8f0/FVF_VERTEX/Illuminate/ARGB4444/software-raster. Exact immutable decoded resource/COM material handle and one-frame residency boundaries; no host geometry/UV/color inputs. Fixed nullspell plane and declared source scene state; actual Metal/map/caller acceptance separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--other-water',action='store_true');p.add_argument('--width',type=int,default=640);p.add_argument('--height',type=int,default=340);a=p.parse_args();selected=json.loads((ROOT/'tools/retail_runtime/other_water_profiles.json').read_text())['profiles']if a.other_water else None;r=run(a.executable,a.archive,a.output,None if a.other_water else ('FlowWater','BendWater1','BendWater2'),selected,a.width,a.height);print(r['status'],len(r['cases']))
