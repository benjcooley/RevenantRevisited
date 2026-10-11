"""Original Funnel owner, three-state controllers, full normal-lit Render.

The declared spell owner moves into an explicit zero-height terrain boundary. Native NextFrame,
SetState, Pulse, controllers, normal lighting and raster all execute. Resource,
map notification, audio and script services are declared fixture boundaries.
"""
import sys,struct,json,zipfile,argparse
from pathlib import Path
sys.path.insert(0,str(Path.cwd()/'tools/retail_runtime'))
from fireball_visual_reference import FireBallScene,read_fireball
from speed_controller_preflight import NativeSpeed,ROOT,sha
from projectile_probe import MissileProbe
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from static_particles_probe import punctuation
from firecone_owner_probe import native_owner
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX
from unicorn.x86_const import UC_X86_REG_ESP
SAMPLES=[1,5,9,13,21,33,49,65,81,97,101,105,117,121,129,133,137,141,145]
class FunnelWorld(MissileProbe):
 def boundary(self,uc,address,size,user):
  if address==0x452e10:
   sp=uc.reg_read(UC_X86_REG_ESP);xyz=struct.unpack('<3i',self.vm.uc.mem_read(self.vm.u32(sp+4),12))
   self.ret(12,0 if self.wall_x is not None and xyz[0]>=self.wall_x else self.ground_height)
  else:super().boundary(uc,address,size,user)
class FunnelScene(FireBallScene):
 owner_init,owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x4fd3f0,0x4fd400,0x500690,0x5ae87c,0x4fd7e0,0x40e2e0,0x4fd800,0x66cf70)
 def __init__(self,exe,data):
  self.put_string=lambda s:self.bytes(s.encode('latin1')+b'\0');self.data=data;self.asset=read_fireball(data);self.rng_draws=0;self.software=LitSoftwareFixture(exe,1280,960,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,640);v.put_u32(0x675fc4,640)
  self.missile=FunnelWorld(exe,vm=v);self.missile.configure([0,0,78],[0,-500,78],target_hp=0,animator_present=True);self.owner=self.missile.obj;self.imagery=v.allocate(0x200)
  v.put_u32(self.owner+0xd8,self.missile.spell);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
  self.boundaries={0x40dd60:0,0x49c430:4,0x40b2e0:12,0x40b5c0:12,0x40c7d0:28,0x471020:4,0x4548a0:8,0x452750:12,0x456790:8,0x4567c0:0}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  v.call(self.owner_init,this=self.owner)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(self.anim_factory,(self.owner,),this=self.builder);v.uc.hook_del(h);assert v.u32(self.animator)==self.anim_vtable;v.put_u32(self.owner+0x58,self.animator)
  v.call(0x4fd3d0);self.registry_lookup=v.call(0x40dca0,(self.put_string('Funnel'),));assert self.registry_lookup==self.builder
  self.required_pose_indices={5,7};self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'FunnelGetLight',getlight),(24,'FunnelSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1;v.call(self.anim_init,this=self.animator,instruction_limit=10000000)
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  self.pulse_inputs={}
  v.uc.hook_add(UC_HOOK_CODE,self.observe_pulse,begin=0x403760,end=0x403760)
  v.put_u32(0x5d79e4,0)
  self.prototype_index=4
  for o in self.asset['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   v.put_u32(resource+0x20,self.bytes(struct.pack('<3i',*[t['parent']for t in o['states']])))
  punctuation(self);v.call(0x403300);v.call(0x405750)
  method=self.bytes(b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');v.put_u32(self.missile.vtable+0x24,method)
  header=bytes.fromhex(self.asset['state_header_hex']);vt=v.u32(self.imagery)
  for slot,off in [(0x8c,40),(0x90,42),(0x94,72),(0x98,74)]:
   values=[struct.unpack_from('<h',header,st*76+off)[0]for st in range(3)]
   table=self.bytes(struct.pack('<3i',*values))
   method=self.bytes(bytes.fromhex('8b4424048b0485')+struct.pack('<I',table)+bytes.fromhex('c20400'))
   v.put_u32(vt+slot,method)
  v.put_u32(vt+0x38,self.bytes(bytes.fromhex('b801000000c20400')))
  v.put_u32(self.owner+0x54,self.imagery);v.write(self.owner+0x5c,struct.pack('<4h',0,1,-1,0))
  v.write(self.owner+0x10,struct.pack('<3i',-180,0,78));v.write(self.owner+0x36,bytes([64]));self.missile.wall_x=180
  v.put_u32(self.missile.vtable+0x18,0x46f250)
  v.call(0x40df90,(0,),this=self.animator,instruction_limit=5000000)
  v.put_u32(self.animator+0x14,0);native_owner(self)
  v.write(self.missile.source+0x36,bytes([64]))
  variant=v.allocate(0x100);kind=v.allocate(0x100)
  v.put_u32(self.missile.spell+0x11c,kind);v.put_u32(self.missile.spell+0x120,variant)
  self.damage_events=[]
  v.uc.hook_add(UC_HOOK_CODE,self.damage,begin=0x4de3c0,end=0x4de3c0)
 def damage(self,uc,*_):
  self.damage_events.append(self.vm.u32(self.owner+0x190));self.missile.ret()
 def observe_pulse(self,uc,*_):
  v=self.vm;c=uc.reg_read(UC_X86_REG_ECX);emitters=[]
  for i in range(v.u32(c+0x18)):
   e=v.u32(v.u32(c+0x28)+4*i)
   emitters.append(dict(position=list(struct.unpack('<3f',v.uc.mem_read(e+0x10,12))),scale=list(struct.unpack('<3f',v.uc.mem_read(e+0x40,12))),matrix=list(struct.unpack('<16f',v.uc.mem_read(e+0x58,64)))))
  self.pulse_inputs[str(c)]=dict(controller=hex(c),frame=v.u32(self.animator+0x14),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),origin=list(struct.unpack('<3f',v.uc.mem_read(c+0x170,12))),emitters=emitters)
 def controller_state(self):
  v=self.vm;result=[]
  for i in range(v.u32(self.animator+0x58)):
   c=v.u32(v.u32(self.animator+0x68)+4*i)
   if v.u32(c)!=0x5a3544:continue
   count=v.u32(c+0xf8);pool=v.u32(c+0x2c)
   result.append(dict(address=c,state=v.u32(c+4),capacity=count,credit=struct.unpack('<f',v.uc.mem_read(c+0x16c,4))[0],pool_hex=bytes(v.uc.mem_read(pool,count*252)).hex()))
  return result
 def step(self):
  v=self.vm
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x470cc0,this=self.owner,instruction_limit=5000000)
  v.call(self.owner_pulse,this=self.owner,instruction_limit=5000000)
  v.call(self.anim_pulse,this=self.animator,instruction_limit=5000000)
 def animate(self):
  self.vm.call(0x40e460,(0,),this=self.animator,instruction_limit=5000000);native_owner(self)
 def state(self):
  v=self.vm;o=self.owner;a=self.animator
  return dict(state=v.u32(o+0xc)&65535,frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],
   position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),velocity=list(struct.unpack('<3i',v.uc.mem_read(o+0x184,12))),
   counter=v.u32(o+0x190),command_done=v.u32(o+0x80),flags=v.u32(o+8),
   cached_state=v.u32(a+0xc),cached_frame=v.u32(a+0x14),seed=v.rng_seed,rng_draws=self.rng_draws,
   damage_events=list(self.damage_events),controllers=self.controller_state(),pulse_inputs=dict(self.pulse_inputs),object_matrices=[list(struct.unpack('<16f',v.uc.mem_read(p+0x58,64)))for p in self.animobjs])
 def render(self):
  self.draws=[];self.software.clear()
  if not(self.vm.u32(self.owner+8)&0x1000):self.vm.call(self.anim_render,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def main():
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=180);p.add_argument('--state-only',action='store_true');p.add_argument('--render-stride',type=int,default=1);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower().endswith('/funnel.i3d')))
 f=FunnelScene(a.exe,data);initial=f.state();records=[]
 for tick in range(a.frames):
  f.step();before=f.state();
  draw=(tick+1)%a.render_stride==0
  if draw and not(before['flags']&0x1000):f.animate()
  f.raster_enabled=draw and not a.state_only and tick in SAMPLES;pixels=f.render() if draw else None
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1280,960)
  state=f.state();records.append(dict(tick=tick,before=before,after=state,rendered=draw,state=state,draws=f.draws if draw and tick in SAMPLES else []));print(tick,state['state'],state['frame'],state['position'],len(state['controllers']),state['rng_draws'],state['flags'],flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,registry_lookup=hex(f.registry_lookup),animator_vtable=hex(f.anim_vtable),asset_sha256=sha(data),executable_sha256=sha(a.exe.read_bytes()),render_stride=a.render_stride,viewport=[1280,960],center=[640,640],policy=dict(source=[-180,0,78],facing=64,terrain='height1 for x<180, otherwise0',spell='explicit invoker face64 and zero damage descriptors; target health0',lighting=dict(ambient=[38,38,38],directional=[1,1,1]),boundaries=['resource/COM construction','base animator Initialize','silent audio','map notification/script callbacks','damage observation only','explicit character health inputs'],limits=['Natural spell/actor selection, damage, audio, arbitrary terrain, device and exact pixels remain open'])),indent=2)+'\n')
if __name__=='__main__':main()
