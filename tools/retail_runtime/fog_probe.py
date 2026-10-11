#!/usr/bin/env python3
"""Executed original Fog factory/Init/Animate/Render and prelit software raster."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP
from speed_controller_preflight import NativeSpeed,ROOT
from default_static_probe import original_registry
from lit_software_probe import LitSoftwareFixture
from fountain_probe import FountainFixture
from firecone_owner_probe import native_owner,native_world
from software_probe import save_rgb565_png
from fizzle_probe import sha


def read_mesh(data):
 if sha(data)!='8ff5a3f3718757241a267f0e494d1b4dcc75f67c40f182902a7d46bdde70e8cf':raise ValueError('Wrong Fog asset')
 u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a);b=20+u(16)
 verts=r(r(b+8));faces=r(b+268);count=u(b+264)
 assert u(b)==0 and u(b+4)==36 and count==50 and u(b+916)==0
 return dict(sha256=sha(data),body_offset=b,flags=0,states=1,state0_frames=1,tags=[],textures=0,
  objects=[dict(name='Fog',vertex_count=36,vertex_hex=data[verts:verts+36*32].hex(),states=[dict(parent=-1,keys=[])])],
  face_hex=data[faces:faces+count*6].hex(),faces=count)


class FogFixture(NativeSpeed):
 observe_random=FountainFixture.observe_random
 def __init__(self,exe,data):
  self.asset=read_mesh(data);self.software=LitSoftwareFixture(exe,640,480);v=self.vm=self.software.vm
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  regs=original_registry(v,[])['registrations'];v.put_u32(0x5e872c,0);v.call(0x406160)
  for row in regs:v.call(int(row['call'],16)-10)
  builder=v.call(0x40dca0,(self.put_string('Fog'),));factory=v.u32(v.u32(builder))
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0)
  self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h)
  assert v.u32(self.animator)==0x5aa7bc
  self.dispatch=dict(builder=hex(builder),factory=hex(factory),vtable=hex(v.u32(self.animator)),init='0x4e84c0',animate='0x4e86f0',render='0x4e8b10')
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.setup_resources();self.objects=self.animobjs;self.obj=self.objects[0]
  vt=v.allocate(0x200);v.put_u32(self.owner,vt);v.handlers['FogDone']=(1,lambda args:0);v.put_u32(vt+0x158,v.api_address('kernel32.dll','FogDone'))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x40a0c0:8,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
  for a in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=a,end=a)
  for a in (0x483300,0x483312,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=a,end=a)
  self.random=[];self.random_pending=[];self.software.clear();self.software.checkpoint();self.reset()
 def external(self,uc,address,size,user):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
  if address==0x417d60 and getattr(self,'execute_blend',False):return
  if address==0x417d60:assert (v.u32(sp+4),v.u32(sp+8))==(2,1)
  elif address==0x40a0c0:
   assert (v.u32(sp+4),v.u32(sp+8))==(self.obj,0x1e2)
   # Literal authored position/UV residency; prelit colors are overwritten by Render.
  elif address==0x40a8f0:
   assert v.u32(sp+4)==self.obj and v.u32(self.obj)==0x209c
   self.packet=bytes(uc.mem_read(self.obj,0xa0));self.lverts=bytes(uc.mem_read(v.u32(self.obj+0xa4),36*32))
  uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address]);uc.reg_write(UC_X86_REG_EAX,1)
 def reset(self):
  self.software.restore();self.random=[];self.random_pending=[];self.vm.call(0x4e84c0,this=self.animator)
 def state(self):
  v=self.vm;a=self.animator;floats=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(colors=floats(0xfc,72),positions=floats(0x21c,108),offsets=floats(0x3cc,108),velocities=floats(0x57c,108),color_velocity=floats(0x72c,36),alpha_velocity=floats(0x7bc,36),rng_calls=len(self.random))
 def advance(self):self.vm.call(0x4e86f0,this=self.animator)
 def render(self,alpha_override=None):
  v=self.vm;sw=self.software;sw.clear();v.call(0x4e8b10,this=self.animator);native_owner(self)
  assert v.call(0x40a420,(self.obj,0,0,self.animator+0xac,0),this=self.imagery)==1
  matrix=list(struct.unpack('<16f',v.uc.mem_read(self.obj+0x58,64)));world=native_world(self,dict(object=0,matrix=matrix))
  for a,x in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(a,x)
  self.execute_blend=True;v.call(0x417d60,(2,1),this=0x65a57c);self.execute_blend=False
  verts=bytearray(self.lverts)
  if alpha_override is not None:
   for i in range(36):verts[i*32+19]=alpha_override
  v.write(sw.authored,bytes(verts));indices=bytes.fromhex(self.asset['face_hex']);v.write(sw.indices,indices);v.put_u32(0x675fb8,sw.vertices+36*36)
  m=v.allocate(64);v.write(m,struct.pack('<16f',*world));v.call(0x56d5f0,(1,m))
  v.call(0x56eb30,(4,0x1e2,sw.authored,36,sw.indices,len(indices)//2,0),instruction_limit=5000000)
  transformed=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(sw.vertices,36*36)))
  return v.surface_bytes('screen'),dict(matrix=matrix,world=world,lverts_hex=self.lverts.hex(),transformed=transformed,depth_sha256=sha(v.surface_bytes('depth')))


def run(exe,archive,output,ticks=120):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fog.i3d'))
 f=FogFixture(exe,data);cases=[]
 for repeat in range(2):
  if repeat:f.reset()
  for t in range(ticks+1):
   if t:f.advance()
   row=dict(tick=t,state=f.state())
   if t in (0,1,2,5,10,20,40,60,80,100,120):
    image,packet=f.render()
    row.update(packet=packet,rgb565_sha256=sha(image),nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',image)))
   if repeat:
    assert row=={k:v for k,v in cases[t].items()if k not in ('image','sha256')}, f'Warm replay differs at tick{t}'
   else:
    if 'packet'in row:
     path=output/f'native-{t:03d}.png';save_rgb565_png(path,image,640,480)
     row.update(image=str(path),sha256=sha(path.read_bytes()))
    cases.append(row)
 image,_=f.render(0);opaque,_=f.render(255)
 assert image!=opaque, 'Source Fog alpha must affect pixels'
 controls=[]
 for alpha in (0,1,3,6,12,16,24,31):
  sw=f.software;v=f.vm;sw.clear()
  sw.draw([(8,8,2750,22,22,22,alpha,0,0),(56,8,2750,22,22,22,alpha,0,0),(8,56,2750,22,22,22,alpha,0,0)],
          [0,1,2],z_enabled=True,z_write=False)
  pixel=struct.unpack_from('<H',v.surface_bytes('screen'),(16*640+16)*2)[0]
  depth=struct.unpack_from('<H',v.surface_bytes('depth'),(16*640+16)*2)[0]
  expected=(int(22*(alpha+1)/32)<<11)|(int(44*(alpha+1)/32)<<5)|int(22*(alpha+1)/32)
  assert pixel==expected and depth==2750
  controls.append(dict(alpha5=alpha,rgb565=pixel,depth=depth,scene_zwrite=False))
 (output/'native-random-inputs.txt').write_text(''.join(f'{a} {b} {c}\n'for a,b,c in f.random))
 report=dict(status='reference_complete',dispatch=f.dispatch,cases=cases,asset_sha256=f.asset['sha256'],executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),alpha_zero_equals_full=image==opaque,warm_replays=2,alpha_depth_controls=controls,
  scope='Actual Fog Init/Animate/Render, original CalcObjectMatrix/owner/world and FVF_LVERTEX software draw; immutable authored resource residency and base animator/extents are boundaries; untextured Scene mode2, camera0 owner0 facing0.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('Fog native complete',[(c['tick'],c.get('nonzero_pixels'))for c in cases if 'packet'in c], 'alpha_ignored',image==opaque)

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
