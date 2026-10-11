"""Original Cataclysm custom 30-piece/70-dust/50-glow leaf and full Render.

The null-spell owner advances its falling and impact phases until life 80.
Native NextFrame, owner Pulse (including Animate), normal lighting and raster
all execute. Resource,
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
class CataclysmWorld(MissileProbe):
 def boundary(self,uc,address,size,user):
  if address==0x4de800:return # Execute original generic effect Pulse.
  if address==0x44ceb0:
   receiver=uc.reg_read(UC_X86_REG_ECX);self.vm.write(receiver,bytes(0x30));self.ret(24,receiver);return # Explicit empty character query.
  super().boundary(uc,address,size,user)

class CataclysmScene(FireBallScene):
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x4fea00,0x500b50,0x5aefb8,0x4fec80,0x4fef10,0x4ff170,0x66cfe0)
 def __init__(self,exe,data,width=1280,height=960,center=(640,640),registry_name="cataclysm"):
  self.put_string=lambda s:self.bytes(s.encode('latin1')+b'\0');self.data=data;self.asset=read_fireball(data);
  self.rng_draws=0;self.software=LitSoftwareFixture(exe,width,height,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,center[0]);v.put_u32(0x675fc4,center[1])
  self.missile=CataclysmWorld(exe,vm=v);self.missile.configure([0,0,78],[0,-500,78],target_hp=0,animator_present=True);self.owner=self.missile.obj;self.imagery=v.allocate(0x200)
  # Generic EFFECT owner vtable, not the missile subclass used to allocate world services.
  v.write(self.missile.vtable,bytes(v.uc.mem_read(0x5aedb8,0x184)))
  v.put_u32(self.owner+0xd8,0);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
  self.boundaries={0x40dd60:0,0x49c430:4,0x40b2e0:12,0x40b5c0:12,0x40c7d0:28,0x471020:4,0x4548a0:8,0x452750:12,0x456790:8,0x4567c0:0}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  # Original allocator 482fb0 zeroes the allocation; discard missile-only fields.
  v.write(self.owner+0xd8,bytes(0xac));v.write(self.owner+0x184,bytes(0x28));v.put_u32(self.owner+8,0x48001);v.call(0x4fe9e0,this=self.owner)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(self.anim_factory,(self.owner,),this=self.builder);v.uc.hook_del(h);assert v.u32(self.animator)==self.anim_vtable;v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'CataclysmGetLight',getlight),(24,'CataclysmSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1 # Resource construction above supplies complete immutable animator resources.
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  self.pulse_inputs=[]
  v.uc.hook_add(UC_HOOK_CODE,self.observe_pulse,begin=0x403760,end=0x403760)
  v.put_u32(0x5d79e4,0)
  self.prototype_index=0
  from default_static_probe import original_registry
  self.dispatch=original_registry(v,())
  name=self.put_string('Cataclysm');self.dispatch['lookup']['Cataclysm']=hex(v.call(0x40dca0,(name,)));assert self.dispatch['lookup']['Cataclysm']=='0x66cfe0'
  assert v.u32(0x5aefb4)==self.anim_factory
  for o in self.asset['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   v.put_u32(resource+0x20,self.bytes(struct.pack('<'+'i'*len(o['states']),*[t['parent']for t in o['states']])))
  punctuation(self);v.call(0x403300);v.call(0x405750);v.call(0x401610)
  method=self.bytes(b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');v.put_u32(self.missile.vtable+0x24,method)
  header=bytes.fromhex(self.asset['state_header_hex']);vt=v.u32(self.imagery)
  for slot,off in [(0x8c,40),(0x90,42),(0x94,72),(0x98,74)]:
   values=[struct.unpack_from('<h',header,st*76+off)[0]for st in range(self.asset['states'])]
   table=self.bytes(struct.pack('<'+'i'*len(values),*values))
   method=self.bytes(bytes.fromhex('8b4424048b0485')+struct.pack('<I',table)+bytes.fromhex('c20400'))
   v.put_u32(vt+slot,method)
  v.put_u32(vt+0x38,self.bytes(bytes.fromhex('b801000000c20400')))
  v.put_u32(self.owner+0x54,self.imagery);v.write(self.owner+0x5c,struct.pack('<4h',0,1,-1,0))
  v.write(self.owner+0x10,struct.pack('<3i',0,0,0));v.write(self.owner+0x36,bytes([0]));self.missile.wall_x=None
  v.put_u32(self.missile.vtable+0x18,0x46f250)
  v.call(self.anim_init,this=self.animator,instruction_limit=10000000)
  v.put_u32(self.animator+0x14,0);native_owner(self)
  assert self.controller_state()==[]
 def observe_pulse(self,uc,*_):
  v=self.vm;c=uc.reg_read(UC_X86_REG_ECX);emitters=[]
  for i in range(v.u32(c+0x18)):
   e=v.u32(v.u32(c+0x28)+4*i)
   emitters.append(dict(position=list(struct.unpack('<3f',v.uc.mem_read(e+0x10,12))),scale=list(struct.unpack('<3f',v.uc.mem_read(e+0x40,12))),matrix=list(struct.unpack('<16f',v.uc.mem_read(e+0x58,64)))))
  self.pulse_inputs.append(dict(controller=c,frame=v.u32(self.animator+0x14),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),origin=list(struct.unpack('<3f',v.uc.mem_read(c+0x170,12))),emitters=emitters))
 def controller_state(self):
  v=self.vm;result=[]
  for i in range(v.u32(self.animator+0x58)):
   c=v.u32(v.u32(self.animator+0x68)+4*i);vt=v.u32(c)
   row=dict(vtable=hex(vt),state=v.u32(c+4),tag_frame=v.u32(c+8))
   if vt==0x5a357c:row['blend']=v.u32(c+0x2c)
   else:
    assert vt==0x5a3480
    row['rates']=list(struct.unpack('<2f',v.uc.mem_read(c+0x2c,8)));row['objects']=[v.u32(v.u32(v.u32(c+0x28)+i*4)+4)for i in range(v.u32(c+0x18))]
   result.append(row)
  return result
 def step(self):
  v=self.vm;v.put_u32(0x65cb38,v.u32(0x65cb38)+1)
  if v.u32(self.owner+8)&0x1000:return
  self.pulse_inputs=[]
  v.call(0x470cc0,this=self.owner,instruction_limit=5000000)
  v.call(self.owner_pulse,this=self.owner,instruction_limit=5000000)
 def animate(self):
  # Original owner Pulse already calls this custom animator through base Pulse.
  native_owner(self)
 def state(self):
  v=self.vm;o=self.owner;a=self.animator
  return dict(frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],flags=v.u32(o+8),phase=v.u32(o+0x190),life=v.u32(o+0x184),position=list(struct.unpack('<3i',v.uc.mem_read(o+0x1a0,12))),velocity=list(struct.unpack('<3f',v.uc.mem_read(o+0x194,12))),owner_hex=bytes(v.uc.mem_read(o+0x184,0x28)).hex(),animator_hex=bytes(v.uc.mem_read(a+0xfc,0x13f8-0xfc)).hex(),rng_draws=self.rng_draws,seed=v.rng_seed,clock=v.u32(0x65cb38))
 def render(self):
  self.draws=[];self.software.clear()
  if not(self.vm.u32(self.owner+8)&0x1000):self.vm.call(self.anim_render,this=self.animator,instruction_limit=400000000)
  return self.vm.surface_bytes('screen')

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=90);p.add_argument('--state-only',action='store_true');p.add_argument('--render-stride',type=int,default=1);p.add_argument('--samples',type=int,nargs='+',default=[0,10,20,30,40,60,79,80,89]);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/cataclysm.i3d')
 f=CataclysmScene(a.exe,data);initial=f.state();records=[];print('initial',initial['phase'],initial['rng_draws'],flush=True)
 for tick in range(a.frames):
  f.step();active=not(f.vm.u32(f.owner+8)&0x1000)
  if active:f.animate()
  f.raster_enabled=not a.state_only and tick in a.samples
  if tick%a.render_stride==0:pixels=f.render()
  else:f.draws=[];f.software.clear();pixels=f.vm.surface_bytes('screen')
  if f.raster_enabled:save_rgb565_png(a.output/f'native-{tick:03}.png',pixels,1280,960)
  records.append(dict(tick=tick,state=f.state(),draws=f.draws if tick in a.samples else []));print(tick,f.state()['phase'],f.state()['life'],len(f.draws),f.rng_draws,flush=True)
 (a.output/'trace.json').write_text(json.dumps(dict(initial=initial,records=records,asset_sha256=sha(data),executable_sha256=sha(a.exe.read_bytes()),dispatch=f.dispatch,render_stride=a.render_stride,scope='Null-spell variant0, clean original allocation state, empty character world; actual NextFrame/owner Pulse/custom Animate/full Render/normal lighting and software raster. Resource construction, base animator Initialize, silent audio and map/script notification services remain explicit fixture boundaries.',viewport=[1280,960],center=[640,640],lighting=dict(ambient=[38,38,38],directional=[1,1,1]),owner_variant=0),indent=2)+'\n')
