#!/usr/bin/env python3
"""Original Iced owner/animator with explicit live stationary victim, exact meshes."""
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
from heal_probe import HealFixture


def read_mesh(data):
 if sha(data)!='0a2cdd9255322792ecb15f80bec3330a04aac363232c8adf5c40570949ab9ac8':raise ValueError('Wrong Iced asset')
 u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a);body=20+u(16)
 vertices=r(r(r(body+16)));faces=r(body+24)
 m=dict(sha256=sha(data),body_offset=body,flags=u(body),version=u(body+4),vertices=u(body+12),faces=u(body+20),materials=u(body+28),textures=u(body+36),states=u(24),state0_frames=1,objects=[],tags=[])
 for i in range(u(body+44)):
  p=r(body+48)+48*i;material,start,count=struct.unpack_from('<3H',data,p+32)
  m['objects'].append(dict(index=i,name=data[p:data.index(0,p)].decode(),material=material,vertex_start=start,vertex_count=count,
   texture_runs=list(struct.unpack_from('<6H',data,r(p+40))),vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex(),
   states=[dict(parent=-1,keys=[])]))
 m['face_hex']=data[faces:faces+6*m['faces']].hex()
 # This provider exposes geometry only. Original Iced replaces every matrix
 # with OBJ3D_MATRIX(0x100); it does not read version1 animation/hierarchy tracks.
 if (m['version'],len(m['objects']),m['textures'])!=(1,50,2):raise AssertionError('Unexpected geometry')
 return m


class IcedFixture(NativeSpeed):
 observe_random=FountainFixture.observe_random
 pixels=HealFixture.pixels
 def __init__(self,exe,data):
  self.data=data;self.asset=read_mesh(data)
  self.software=LitSoftwareFixture(exe,640,480);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.victim=v.allocate(0x400);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  regs=original_registry(v,[])['registrations'];v.put_u32(0x5e872c,0);v.call(0x406160)
  for row in regs:v.call(int(row['call'],16)-10)
  builder=v.call(0x40dca0,(self.put_string('Iced'),));factory=v.u32(v.u32(builder))
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0)
  self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h)
  assert v.u32(self.animator)==0x5ab460
  self.dispatch=dict(registrations=v.u32(0x5e872c),builder=hex(builder),factory=hex(factory),vtable=hex(v.u32(self.animator)))
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.setup_resources();self.objects=self.animobjs
  for obj,name in ((self.owner,'Owner'),(self.victim,'Victim')):
   vt=v.allocate(0x200);v.put_u32(obj,vt)
   v.handlers[name+'Flags']=(1,lambda args,p=obj:v.put_u32(p+8,args[0])or 0)
   v.handlers[name+'Done']=(1,lambda args:0)
   v.handlers[name+'False']=(0,lambda args:0)
   v.handlers[name+'Health']=(0,lambda args:100)
   for off,suffix in ((0x40,'Flags'),(0x158,'Done'),(0x3c,'False'),(0x1c0,'Health')):v.put_u32(vt+off,v.api_address('kernel32.dll',name+suffix))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x4de800:0,0x4cee70:4,0x49c430:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  for address in (0x483300,0x483312,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
  self.random=[];self.random_pending=[];self.packets=[];self.stop_calls=0
  self.software.clear();self.software.checkpoint();self.reset()
 def external(self,uc,address,size,user):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
  if address==0x417d60 and getattr(self,'execute_blend',False):return
  if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff)
  elif address==0x4cee70:self.stop_calls+=1
  elif address==0x417d60:assert (v.u32(sp+4),v.u32(sp+8))==(2,1)
  elif address==0x40a8f0:
   obj=v.u32(sp+4);assert v.u32(obj)==0x100
   self.packets.append(dict(object=self.objects.index(obj),record=bytes(uc.mem_read(obj,0xa0))))
  uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])
 def reset(self):
  self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.stop_calls=0
  v=self.vm;v.call(0x4ec880,this=self.owner);v.call(0x4ec930,(self.victim,),this=self.owner);v.call(0x4eca60,this=self.animator)
  assert v.u32(self.victim+8)==0x2800000
 def state(self):
  v=self.vm;a=self.animator;floats=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),frame=v.u32(self.owner+0x188),angle=floats(0xfc,1)[0],done=v.u32(a+0x100),
   positions=[floats(0x104+i*12,3)for i in range(30)],velocities=[floats(0x26c+i*12,3)for i in range(30)],
   rotations=[floats(0x3d4+i*12,3)for i in range(30)],angular_velocity=[floats(0x53c+i*12,3)for i in range(30)],
   scales=[floats(0x6a4+i*12,3)for i in range(30)],life=list(struct.unpack('<30i',v.uc.mem_read(a+0x80c,120))),
   rng_calls=len(self.random),victim_flags=hex(v.u32(self.victim+8)),stop_calls=self.stop_calls)
 def advance(self):
  if self.state()['alive']:
   self.vm.call(0x4ec890,this=self.owner);self.vm.call(0x4ecaa0,this=self.animator)
 def render_packets(self):
  self.packets=[]
  if self.state()['alive']:self.vm.call(0x4ecff0,this=self.animator)
  native_owner(self);result=[]
  for p in self.packets:
   obj=self.objects[p['object']];self.vm.write(obj,p['record']);matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)))
   result.append(dict(object=p['object'],matrix=matrix,world=native_world(self,dict(object=p['object'],matrix=matrix))))
  return result


def run(exe,archive,output,ticks=421):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read('Imagery/Magic/iced.i3d')
 f=IcedFixture(exe,data);cases=[];samples=(0,1,2,4,13,120,239,240,241,250,260,280,300,320,322,400,420,421)
 for repeat in range(2):
  if repeat:f.reset()
  for tick in range(ticks+1):
   if tick:f.advance()
   c=dict(tick=tick,state=f.state())
   if tick in samples:
    packets=f.render_packets();image=f.pixels(packets)
    c.update(packets=packets,rgb565_sha256=sha(image))
   if repeat:
    expected={k:v for k,v in cases[tick].items()if k not in ('image','sha256')}
    if c!=expected:raise AssertionError('Warm Iced replay differs at tick '+str(tick))
   else:
    if tick in samples:
     p=output/f'native-{tick:03d}.png';save_rgb565_png(p,image,640,480);c.update(image=str(p),sha256=sha(p.read_bytes()))
    cases.append(c)
 (output/'native-random-inputs.txt').write_text(''.join(f'{a} {b} {v}\n'for a,b,v in f.random))
 f.reset();f.vm.put_u32(f.owner+0x184,0);f.advance();no_victim=f.render_packets()
 if no_victim or f.state()['rng_calls']!=1:raise AssertionError('Null victim unexpectedly emits')
 report=dict(status='reference_complete',dispatch=f.dispatch,cases=cases,asset_sha256=f.asset['sha256'],
  executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),warm_replays=2,
  null_victim_control=dict(draws=len(no_victim),state=f.state()),
  declared_caller=dict(victim='stationary living actor',health=100,owner_position=[0,0,0],owner_facing=0,heap_initialization='fresh zeroed allocation'),
  declared_renderer=dict(width=640,height=480,source_ambient=[38]*3,directional=[1]*3,blend_mode=2,cull=2,z_test=True,z_write=False),
  scope='Explicit live stationary victim withhealth100; real InitIced flag handshake and ownerPulse; Stop/basePulse/audio/geometry residency boundaries. Whole native animator, native owner/world/raster, sourceARGB4444. Version1 tracks unused: every leaf draw asserts flags0x100.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('complete',len(cases),'firstdead',next((c['tick']for c in cases if not c['state']['alive']),None))

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
