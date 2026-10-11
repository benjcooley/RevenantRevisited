"""Original full normal-lit FireWind leaf, explicit null-spell owner lifecycle."""
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
class FireWindScene(FireBallScene):
 def __init__(self,exe,data):
  self.data=data;self.asset=read_fireball(data);self.rng_draws=0;self.software=LitSoftwareFixture(exe,1024,768,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,512);v.put_u32(0x675fc4,485)
  self.missile=MissileProbe(exe,vm=v);self.missile.configure([0,0,78],[0,-500,78],target_hp=0,animator_present=True);self.owner=self.missile.obj;self.imagery=v.allocate(0x200)
  v.put_u32(self.owner+0xd8,0);v.call(0x4e2270,this=self.owner);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
  self.boundaries={0x40dd60:0,0x49c430:4,0x471b50:4,0x40e2e0:0}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(0x4f53c0,(self.owner,),this=0x66cd20);v.uc.hook_del(h);assert v.u32(self.animator)==0x5a9404;v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices=set(range(2));self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'FireWindGetLight',getlight),(24,'FireWindSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1;v.call(0x4e2400,this=self.animator,instruction_limit=10000000)
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
 def step(self):
  self.vm.call(0x4e2290,this=self.owner);self.vm.call(0x4e2620,this=self.animator)
  from firecone_owner_probe import native_owner
  native_owner(self)
 def state(self):
  v=self.vm;a=self.animator
  return dict(frame=v.u32(a+0x118),owner_life=v.u32(self.owner+0x18c),owner_flags=v.u32(self.owner+8),seed=v.rng_seed,rng_draws=self.rng_draws,pool_hex=bytes(v.uc.mem_read(a+0x13c,400*92)).hex(),header_hex=bytes(v.uc.mem_read(a+0xfc,64)).hex(),ring_hex=bytes(v.uc.mem_read(a+0x90fc,52)).hex())
 def render(self):
  self.draws=[];self.software.clear();self.vm.call(0x4e2c80,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen')

if __name__=='__main__':
 import argparse,hashlib
 from speed_controller_preflight import ROOT,sha
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=161);p.add_argument('--render-stride',type=int,default=1);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:data=archive.read('Imagery/Magic/firewind.i3d')
 assert sha(data)=='ab55c2c0bacb33c2dad3d5a84aa51f57fe502fec498045ded9aebd10862266ea'
 assert sha(a.executable.read_bytes())=='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
 f=FireWindScene(a.executable,data);initial=f.state();records=[];selected=(0,1,4,9,15,24,27,28,34,39,40,44,59,79,99,119,139,140,149,159)
 for tick in range(a.frames):
  f.step();f.draws=[];active=not(f.vm.u32(f.owner+8)&0x1000);draw=active and a.render_stride>0 and (tick+1)%a.render_stride==0
  f.raster_enabled=tick in selected and a.render_stride==1
  pixels=f.render()if draw else(f.software.clear()or f.vm.surface_bytes('screen'))
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1024,768)
  records.append(dict(tick=tick,rendered=draw,state=f.state(),draws=f.draws if tick in selected else []));print(tick,len(f.draws),flush=True)
 report=dict(effect='FireWind',initial=initial,records=records,render_stride=a.render_stride,executable_sha256=sha(a.executable.read_bytes()),asset_sha256=sha(data),source_sha256=sha(Path(__file__).read_bytes()),policy=dict(null_spell=True,source=[0,0,78],camera=[0,0,0],screen_center=[512,485],ambient_rgb=[38,38,38],directional_rgb=[1,1,1],seed=1,owner_initialize='4e2270',owner_pulse='4e2290',animator_initialize='4e2400',animator_pulse='4e2620',render='4e2c80',kill_after_pulse=140,boundaries=['decoded resource/COM surfaces','silent audio','base animator services','owner command/script facade; full owner transform separately executed'],limits=['No natural spell/damage/target cleanup or audio acceptance','Post-kill pool pulses are fixture controls only; render stops at original kill flag','YFireWind is a separate controller, excluded']))
 (a.output/'trace.json').write_text(json.dumps(report,indent=2)+'\n')
