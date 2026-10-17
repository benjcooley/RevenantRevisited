"""Original stationary IceBolt full normal-lit software renderer.

Resource adapters supply authored assets; explicit empty-world/audio/reaper
boundaries are declared in trace.json. No native vertices/colors are replaced.
"""
import sys,struct,json,zipfile
from pathlib import Path
sys.path.insert(0,str(Path.cwd()/'tools/retail_runtime'))
from fireball_visual_reference import FireBallScene,read_fireball
from speed_controller_preflight import NativeSpeed
from projectile_probe import MissileProbe
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
class IceWorld(MissileProbe):
 def boundary(self,uc,address,size,user):
  if address==0x452e10 and getattr(self,'wall_length',None)is not None:
   sp=uc.reg_read(UC_X86_REG_ESP);x,y,z=struct.unpack('<3i',self.vm.uc.mem_read(self.vm.u32(sp+4),12));sx,sy,_=self.source_xyz
   self.ret(12,10000 if (x-sx)**2+(y-sy)**2>=self.wall_length**2 else 1)
  else:super().boundary(uc,address,size,user)

class IceScene(FireBallScene):
 def __init__(self,exe,data,wall_length=None,facing=0):
  self.data=data;self.asset=read_fireball(data);self.rng_draws=0;self.software=LitSoftwareFixture(exe,1024,768,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,310);v.put_u32(0x675fc4,485)
  self.missile=IceWorld(exe,vm=v);self.missile.wall_length=wall_length;self.missile.configure([0,0,78],[0,-500,128],target_hp=0,animator_present=True);self.owner=self.missile.obj;self.imagery=v.allocate(0x200)
  v.write(self.owner+0x36,bytes([facing]));v.call(0x4eb210,this=self.owner)
  # Real spell descriptor interfaces for the frame10 damage request.
  for off in(0x11c,0x120):v.put_u32(self.missile.spell+off,v.allocate(0x180))
  self.boundaries={0x40dd60:0,0x49c430:4,0x471b50:4,0x452060:32,0x4de250:0,0x40e2e0:0}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(0x4f7530,(self.owner,),this=0x66cd20);v.uc.hook_del(h);assert v.u32(self.animator)==0x5ab1f4;v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices=set(range(7));self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'IceGetLight',getlight),(24,'IceSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1;v.call(0x4eb570,this=self.animator,instruction_limit=10000000)
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
 def step(self):
  self.vm.call(0x4eb230,this=self.owner);self.vm.call(0x4eba00,this=self.animator)
  from firecone_owner_probe import native_owner
  native_owner(self)
 def state(self):
  v=self.vm;a=self.animator;base=dict(owner_flags=v.u32(self.owner+8),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),length=struct.unpack('<f',v.uc.mem_read(self.owner+0x188,4))[0],frame=v.u32(a+0x100),angle=v.u32(a+0xfc),seed=v.rng_seed,rng_draws=self.rng_draws,scales=list(struct.unpack('<7f',v.uc.mem_read(a+0x104,28))))
  floats=lambda offset,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+offset,n*4)))
  base['frost']=[dict(p=floats(0x120+i*12,3),v=floats(0x378+i*12,3),s=floats(0x5d0+i*4,1)[0],t=floats(0x698+i*4,1)[0])for i in range(50)]
  base['snow']=[dict(zip(('h','th','r','rs','sz'),[floats(offset+i*4,1)[0]for offset in(0x760,0x828,0x8f0,0x9b8,0xa80)]))for i in range(50)]
  return base
 def render(self):
  self.draws=[];self.software.clear();self.vm.call(0x4ebea0,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen')

if __name__=='__main__':
 import argparse,hashlib
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--state-only',action='store_true');p.add_argument('--frames',type=int,default=121);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile('data/imagery.rvi')as z:data=z.read('Imagery/Magic/icebolt.i3d')
 f=IceScene(a.executable,data);initial=f.state();records=[]
 for tick in range(a.frames):
  f.step();f.draws=[];f.raster_enabled=not a.state_only and tick in(0,1,5,9,10,19,20,29,39,49,59,89,109,118,119,120,129);pixels=f.render() if not(f.vm.u32(f.owner+8)&0x1000) else (f.software.clear() or f.vm.surface_bytes('screen'));print(tick,f.state()['frame'],len(f.draws),flush=True)
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1024,768)
  records.append(dict(tick=tick,state=f.state(),draws=f.draws if tick in(0,1,5,9,10,19,20,29,39,49,59,89,109,118,119,120,129) else []))
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,executable_sha256=hashlib.sha256(a.executable.read_bytes()).hexdigest(),asset_sha256=hashlib.sha256(data).hexdigest(),policy=dict(owner_initialize='4eb210',owner_pulse='4eb230',animator_initialize='4eb570',animator_pulse='4eba00',render='4ebea0',fvf='112',seed=1,ambient_rgb=[38,38,38],directional_rgb=[1,1,1],camera=[0,0,0],screen_center=[310,485],source=[0,0,78],owner_z_after_initialize=128,length=500,empty_world=True,kill_flag=4096,render_after_kill=False,boundaries={hex(k):v for k,v in f.boundaries.items()},limits=['No live enemy, damage, posted Iced victim, audio or natural map dispatch certified','Post-kill state pulses are fixture controls only; rendering stops at original owner kill flag']),source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest()),indent=2)+'\n')
