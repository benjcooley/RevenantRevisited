#!/usr/bin/env python3
"""Full original particle draw caller and software raster; no port poses/colors.

Decoded immutable asset-provider resources, owner GetAnimator, material-handle
COM state and single-frame texture residency are explicit fixture boundaries.
Original CopyVertices, controller/Pulse/RenderSample, hide/unhide, RenderObject,
blend setter, FVF dispatch, camera/transform and pixel kernels execute unchanged.
"""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from speed_controller_preflight import NativeSpeed, read_asset, ROOT
from lit_software_probe import LitSoftwareFixture
from static_particles_probe import punctuation, registry
from static_particles_profile_contract import PROFILES, Y_PROFILES
from firecone_state_probe import sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from software_probe import save_rgb565_png

class NativeParticleScene(NativeSpeed):
 def __init__(self,exe,data,name,width=1024,height=768):
  self.data=data;a=read_asset(data);a['name']=name
  super().__init__(exe,a,width,height,software_factory=LitSoftwareFixture)
  self.software.lighting((38,38,38),(1,1,1))
  v=self.vm
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  method=v.allocate(16);v.write_code(method,b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');vt=v.allocate(0x100);v.put_u32(vt+0x24,method);v.put_u32(self.owner,vt)
  def texture_frame(uc,address,size,user):
   sp=v.uc.reg_read(UC_X86_REG_ESP);assert v.u32(sp+4)==0
   self.ret(1,8)
  v.uc.hook_add(UC_HOOK_CODE,texture_frame,begin=0x40c520,end=0x40c520)
  device=v.allocate(4);devicevt=v.allocate(0xa0);v.put_u32(device,devicevt);v.put_u32(0x668f14,device)
  self.material_state=0
  def getlight(args):
   assert tuple(args[:2])==(device,1);v.put_u32(args[2],self.material_state);return 0
  def setlight(args):
   assert tuple(args[:2])==(device,1);self.material_state=args[2];return 0
  for slot,name,method in [(23,'ParticleSceneGetLightState',getlight),(24,'ParticleSceneSetLightState',setlight)]:
   v.handlers[name]=(3,method);v.put_u32(devicevt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.raster_enabled=False;self.render_states={}
  def observe_state(uc,address,size,user):
   sp=v.uc.reg_read(UC_X86_REG_ESP);self.render_states[v.u32(sp+4)]=v.u32(sp+8)
  v.uc.hook_add(UC_HOOK_CODE,observe_state,begin=0x56d400,end=0x56d400)
  def observe_draw(uc,address,size,user):
   sp=v.uc.reg_read(UC_X86_REG_ESP);args=[v.u32(sp+4+i*4)for i in range(7)]
   if args[:2]!=[4,0x1e2] or args[3]!=4 or args[5:]!=[6,0]:raise AssertionError('Original draw topology/FVF changed')
   self.draws.append(dict(arguments=args,vertices_sha256=sha(bytes(v.uc.mem_read(args[2],128))),
       matrix=list(struct.unpack('<16f',v.uc.mem_read(0x675ed0,64))),states=dict(self.render_states)))
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,observe_draw,begin=0x56eb30,end=0x56eb30)
 def copy_vertices(self,uc,address,size,user):
  # Observe without intercepting: original allocation, VERTEX->LVERTEX
  # conversion and OBJ3D_VERTS/type flags now execute end to end.
  pass
 def setup_resources(self):
  super().setup_resources();v=self.vm;a=self.asset;data=self.data
  u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);bo=a['body_offset']
  raw=r(r(r(bo+16)));pointer=self.bytes(data[raw:raw+a['vertices']*32]);pointer=self.bytes(struct.pack('<I',pointer));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',pointer)))
  v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o in a['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in [(0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])]:v.put_u32(resource+off,value)
   runs=o['texture_runs'];v.put_u32(resource+0x34,self.bytes(struct.pack('<2I',*runs[::2])));v.put_u32(resource+0x38,self.bytes(struct.pack('<2I',*runs[1::2])))
  materials=[];rawmat=r(bo+32)
  for i in range(a['materials']):
   mat=v.allocate(0x58);v.write(mat,data[rawmat+i*80:rawmat+i*80+80]);v.put_u32(mat+0x54,0);materials.append(mat)
  v.put_u32(self.imagery+0x34,len(materials));v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<'+'I'*len(materials),*materials)))
  tex=r(bo+40);h,w=struct.unpack_from('<2I',data,tex+8);pixels=r(r(tex+108));self.software.set_texture(w,h,data[pixels:pixels+w*h*2])
  t=v.allocate(0xa0);v.put_u32(t+0x94,1);v.put_u32(t+0x90,self.software.texture_object);v.put_u32(self.imagery+0x4c,1);v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<I',t)));v.put_u32(self.imagery+0x10,1)


def run(executable, archive, output, profiles, width=1024, height=768, frames=90, samples_override=None):
 output.mkdir(parents=True,exist_ok=False)
 rng_counts={1:0};state=1
 for count in range(1,200001):
  state=(state*214013+2531011)&0xffffffff;rng_counts[state]=count
 cases=[]
 for profile in profiles:
  destination=output/profile['name'];destination.mkdir()
  with zipfile.ZipFile(archive)as z:
   member=next(n for n in z.namelist()if n.lower()==('imagery/'+profile['asset']).lower());data=z.read(member)
  if sha(data)!=profile['sha256']:raise AssertionError('Pinned authored asset changed')
  f=NativeParticleScene(executable,data,profile['name'],width,height);v=f.vm
  dispatch=registry(f);punctuation(f);initialization=f.initialize(0)
  # Install only the existing declared CRT RNG and flat-ground interfaces.
  f.state_trace(0,frame_offset=1,move_tick=None,clamp_frames=profile.get('clamp',False))
  samples=samples_override or profile.get('samples',(5,15,29,60,89))
  records=[];draw_log=[]
  for tick in range(frames+1):
   prior_rng=v.rng_seed
   pulse_frame=v.u32(f.animator+0x14)
   # Original controller403771 reads the cached animator frame. Native
   # GetObjectPos40efc0 reads cached bone matrices; Animate updates both
   # only after Pulse. Owner NextFrame advances independently before Pulse.
   v.call(0x403760,this=f.controller,instruction_limit=5000000)
   frame=min(tick+1,f.asset['state0_frames']-1)if profile.get('clamp')else(tick+1)%f.asset['state0_frames']
   v.put_u32(f.animator+0x14,frame)
   for index,obj in enumerate(f.animobjs):
    if index==f.prototype_index or index in f.unused_rejected_pose_indices:continue
    if not f.asset['objects'][index]['states'][0]['keys']:continue
    if v.call(0x40a420,(obj,0,frame,0,0),this=f.imagery)!=1:raise AssertionError('Original emitter pose rejected')
   f.software.clear();f.draws=[];f.raster_enabled=tick in samples
   with v.bulk_writes([(f.software.screen.address,width*height*2),(f.software.depth.address,width*height*2),
       (v.STACK+v.STACK_SIZE-0x10000,0x10000)]):
    v.call(0x404270,this=f.controller,instruction_limit=100000000)
   if f.software.locked:raise AssertionError('Original texture remained locked')
   if not(v.u32(f.animobjs[f.prototype_index])&1):raise AssertionError('Original controller failed to re-hide prototype')
   records.append(dict(capture=tick,animation_frame=frame,pulse_frame=pulse_frame,draws=len(f.draws),rng_before=prior_rng,
       rng_before_draws=rng_counts[prior_rng],rng_after=v.rng_seed))
   if tick in samples:
    path=destination/f'native-capture{tick:03d}.png'
    pixels=v.surface_bytes('screen');save_rgb565_png(path,pixels,width,height)
    draw_log.append(dict(capture=tick,draws=f.draws))
    cases.append(dict(name=profile['name'],capture=tick,animation_frame=frame,draws=len(f.draws),
        native_rng_draws_before_capture=rng_counts[prior_rng],image=str(path),image_sha256=sha(path.read_bytes()),
        rgb565_sha256=sha(pixels),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',pixels))))
    print(profile['name'],tick,'draws',len(f.draws),'pixels',cases[-1]['nonzero_pixels'],flush=True)
  (destination/'draws.json').write_text(json.dumps(draw_log,indent=2)+'\n')
  (destination/'cadence.json').write_text(json.dumps(records,indent=2)+'\n')
  (destination/'initialization.json').write_text(json.dumps(dict(dispatch=dispatch,initialization=initialization,
      objects=[dict(index=o['index'],name=o['name'],faces=o['texture_runs'][1::2])for o in f.asset['objects']]),indent=2)+'\n')
 report=dict(status='reference_complete',accepted=False,strict_pixel_parity=False,cases=cases,
     viewport=[width,height],camera=[0,0,0],owner=[0,0,0],ambient_rgb=[38]*3,directional_rgb=[1]*3,
     seed=1,quality=0,ground_height=0,cadence='Owner NextFrame then Pulse with cached animator frame/bones, Animate refreshes frame/bones, full controller Render; warmup1; captureN follows PulseN+1',
     original_executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),
     boundaries=['immutable decoded authored resources','owner GetAnimator returns actual native factory animator',
       'COM material handle get/set only; FVF_LVERTEX receives native packed particle colors',
       'single-frame texture residency; exact shipped RGB565 texels; no texture/frame substitution',
       'declared seed1 CRT RNG and ground0', 'nonselected captures skip only raster after complete native draw dispatch'],
     original_functions=['40a0c0 CopyVertices','403760 controller Pulse','404270 controller Render/hide',
       '4026d0 RenderSample','40a420 CalcObjectMatrix','40a8f0 complete RenderObject','417d60 blend',
       '4174b0 Scene dispatch','56eb30 FVF_LVERTEX transform','56d960 raster'],
     scope='Exact prototype-only assets; every other object has zero faces. No host geometry, tint, normal-light carrier or animation poses feed the native branch. Independent actual Metal and ordinary map lifecycle comparisons remain separate.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
 p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True)
 p.add_argument('--profiles',nargs='+',default=[p['name']for p in Y_PROFILES]);p.add_argument('--frames',type=int,default=90)
 p.add_argument('--samples',nargs='+',type=int);p.add_argument('--width',type=int,default=1024);p.add_argument('--height',type=int,default=768)
 a=p.parse_args();available={p['name']:p for p in(*PROFILES,*Y_PROFILES)}
 selected=[available[name]for name in a.profiles]
 run(a.executable,a.archive,a.output,selected,a.width,a.height,a.frames,a.samples)
