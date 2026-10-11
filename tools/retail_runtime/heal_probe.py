#!/usr/bin/env python3
"""Executed Heal Init/Animate/Render with declared level/caster and exact authored meshes."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP
from speed_controller_preflight import NativeSpeed,read_asset,ROOT
from default_static_probe import original_registry
from lit_software_probe import LitSoftwareFixture
from fountain_probe import FountainFixture
from firecone_owner_probe import native_owner,native_world
from software_probe import save_rgb565_png
from fizzle_probe import sha

class HealFixture(NativeSpeed):
 observe_random=FountainFixture.observe_random
 def __init__(self,exe,data,level=1):
  self.data=data;self.asset=read_asset(data);self.asset['name']='heal'
  if self.asset['sha256']!='ac0ae8fbb1b401598649548304d4c8e030556a9005eeb3d14d6ad0b81e12ccec':raise ValueError('Wrong Heal asset')
  self.software=LitSoftwareFixture(exe,640,480);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  regs=original_registry(v,[])['registrations'];v.put_u32(0x5e872c,0);v.call(0x406160)
  for row in regs:v.call(int(row['call'],16)-10)
  builder=v.call(0x40dca0,(self.put_string('heal'),));factory=v.u32(v.u32(builder))
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0)
  self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h)
  if v.u32(self.animator)!=0x5a8a20:raise AssertionError('Wrong original Heal factory')
  self.dispatch=dict(registrations=v.u32(0x5e872c),builder=hex(builder),factory=hex(factory),vtable=hex(v.u32(self.animator)))
  self.required_pose_indices=set(range(8));self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self)
  self.objects=self.animobjs
  vt=v.allocate(0x200);v.put_u32(self.owner,vt)
  v.handlers['HealSetPos']=(3,lambda args:self.set_pos(args[0]))
  v.handlers['HealFlags']=(1,lambda args:self.set_flags(args[0]))
  v.handlers['HealDone']=(1,lambda args:0)
  v.handlers['HealFalse']=(0,lambda args:0)
  for off,name in ((8,'HealSetPos'),(0x40,'HealFlags'),(0x158,'HealDone'),(0x3c,'HealFalse')):v.put_u32(vt+off,v.api_address('kernel32.dll',name))
  spell=v.allocate(0x180);character=v.allocate(0x200);v.put_u32(self.owner+0xd8,spell);v.put_u32(spell+4,character);v.put_u32(self.owner+0x188,level)
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x49c430:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  for address in (0x483300,0x483312,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
  self.random=[];self.random_pending=[];self.packets=[]
  self.software.clear();self.software.checkpoint();self.reset()
 def set_pos(self,p):self.vm.write(self.owner+0x10,bytes(self.vm.uc.mem_read(p,12)));return 0
 def set_flags(self,f):self.vm.put_u32(self.owner+8,f);return 0
 def external(self,uc,address,size,user):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
  if address==0x417d60 and getattr(self,"execute_blend",False):return
  if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff)
  elif address==0x417d60:
   if (v.u32(sp+4),v.u32(sp+8))!=(2,1):raise AssertionError('Heal blend changed')
  elif address==0x40a8f0:
   obj=v.u32(sp+4);index=self.objects.index(obj)
   self.packets.append(dict(object=index,record=bytes(uc.mem_read(obj,0xa0))))
  uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])
 def reset(self):
  self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.vm.call(0x4df680,this=self.animator)
 def state(self):
  v=self.vm;a=self.animator;floats=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),positions=[floats(0xfc+i*12,3)for i in range(60)],scales=floats(0x3cc,60),rise=floats(0x4bc,60),frames=list(struct.unpack('<60i',v.uc.mem_read(a+0x5ac,240))),active=v.u32(a+0x69c),rotation=floats(0x6a0,1)[0],objects=[v.u32(a+0x6a4),v.u32(a+0x6a8)],rng_calls=len(self.random))
 def advance(self):
  if self.state()['alive']:self.vm.call(0x4df820,this=self.animator)
 def render_packets(self):
  self.packets=[]
  if self.state()['alive']:self.vm.call(0x4dfa60,this=self.animator)
  owner=native_owner(self);result=[]
  for p in self.packets:
   obj=self.objects[p['object']];self.vm.write(obj,p['record']);flags=self.vm.u32(obj)
   if not flags&0x100:
    # Original CalcObjectMatrix rejects the final zero-scale glow.
    if self.vm.call(0x40a420,(obj,0,0,self.animator+0xac,0),this=self.imagery)!=1:continue
   matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)))
   world=native_world(self,dict(object=p['object'],matrix=matrix));result.append(dict(object=p['object'],flags=hex(flags),matrix=matrix,world=world))
  return result
 def pixels(self,packets):
  sw=self.software;v=self.vm;sw.clear()
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):v.put_u32(addr,value)
  self.execute_blend=True
  v.call(0x417d60,(2,1),this=0x65a57c)
  self.execute_blend=False
  for p in packets:
   part=self.asset['objects'][p['object']];m=part['material'];data=self.data;u=lambda x:struct.unpack_from('<I',data,x)[0];r=lambda x:x+u(x)
   t=r(self.asset['body_offset']+40)+m*120;h,w=struct.unpack_from('<2I',data,t+8);bits=r(r(t+108));sw.set_texture(w,h,data[bits:bits+w*h*2],format='ARGB4444')
   runs=part['texture_runs'];start,count=runs[2*(m+1):2*(m+2)];indices=list(struct.unpack_from('<'+str(count*3)+'H',bytes.fromhex(self.asset['face_hex']),start*6))
   verts=list(struct.iter_unpack('<8f',bytes.fromhex(part['vertex_hex'])))
   sw.draw_authored(verts,indices,p['world'],cull=2,z_enabled=True,z_write=False)
  return v.surface_bytes('screen')

def run(exe,archive,output,ticks=91):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read('Imagery/Magic/heal.i3d')
 f=HealFixture(exe,data);cases=[];replay=[]
 for repeat in range(2):
  if repeat:f.reset()
  for tick in range(ticks+1):
   if tick:f.advance()
   state=f.state();packets=f.render_packets();image=f.pixels(packets)
   c=dict(tick=tick,state=state,packets=packets,rgb565_sha256=sha(image))
   if repeat:
    expected={k:v for k,v in cases[tick].items()if k not in ('image','sha256')}
    if c!=expected:raise AssertionError('Warm replay differs at tick '+str(tick))
   else:
    p=output/f'native-{tick:03d}.png';save_rgb565_png(p,image,640,480)
    c.update(image=str(p),sha256=sha(p.read_bytes()));cases.append(c)
 (output/'native-random-inputs.txt').write_text(''.join(f'{a} {b} {v}\n'for a,b,v in f.random))
 report=dict(status='reference_complete',dispatch=f.dispatch,cases=cases,asset_sha256=f.asset['sha256'],
  executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),warm_replays=2,
  declared_caller=dict(level=1,caster_position=[0,0,0],owner_facing=0),
  declared_renderer=dict(width=640,height=480,source_ambient=[38]*3,directional=[1]*3,blend_mode=2,cull=2,z_test=True,z_write=False),
  scope='Level1; explicit spell/caster position0; original owner Init relocation, all Animate/Render, native glow CalcObjectMatrix and relative owner compose, authored FVF_VERTEX/ARGB4444 software. Leaf resource lookup/RenderObject dispatch and audio are boundaries; no acceptance yet.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('complete',len(cases),'firstdead',next((c['tick']for c in cases if not c['state']['alive']),None))

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
