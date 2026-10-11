"""Original GVortex default owner/controllers and full normal-lit Render.

The null-spell owner plays the authored 240-frame state. Native NextFrame,
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
class GVortexWorld(MissileProbe):
 def boundary(self,uc,address,size,user):
  if address==0x4de800:return # Execute original generic effect Pulse.
  super().boundary(uc,address,size,user)

class GVortexScene(FireBallScene):
 owner_pulse,anim_factory,anim_vtable,anim_init,anim_pulse,anim_render,builder=(0x4de800,0x40dc00,0x5a370c,0x40dd60,0x40e2e0,0x40e8d0,0x5e8508)
 def __init__(self,exe,data,width=1280,height=960,center=(640,640)):
  self.put_string=lambda s:self.bytes(s.encode('latin1')+b'\0');self.data=data;self.asset=read_fireball(data);
  self.rng_draws=0;self.software=LitSoftwareFixture(exe,width,height,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1));v.put_u32(0x675fc0,center[0]);v.put_u32(0x675fc4,center[1])
  self.missile=GVortexWorld(exe,vm=v);self.missile.configure([0,0,78],[0,-500,78],target_hp=0,animator_present=True);self.owner=self.missile.obj;self.imagery=v.allocate(0x200)
  # Generic EFFECT owner vtable, not the missile subclass used to allocate world services.
  v.write(self.missile.vtable,bytes(v.uc.mem_read(0x5a85ac,0x184)))
  v.put_u32(self.owner+0xd8,0);v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
  self.boundaries={0x49c430:4,0x40b2e0:12,0x40b5c0:12,0x40c7d0:28,0x471020:4,0x4548a0:8,0x452750:12,0x456790:8,0x4567c0:0}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  v.write(self.owner+0xd8,bytes(0xac));v.put_u32(self.owner+8,0x48001)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(self.anim_factory,(self.owner,),this=self.builder);v.uc.hook_del(h);assert v.u32(self.animator)==self.anim_vtable;v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'GVortexGetLight',getlight),(24,'GVortexSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1 # Resource construction above supplies complete immutable animator resources.
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  self.pulse_inputs=[]
  v.uc.hook_add(UC_HOOK_CODE,self.observe_pulse,begin=0x403760,end=0x403760)
  v.put_u32(0x5d79e4,0)
  self.prototype_index=0
  from default_static_probe import original_registry
  self.dispatch=original_registry(v,("gvortex",))
  for o in self.asset['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   v.put_u32(resource+0x20,self.bytes(struct.pack('<'+'i'*len(o['states']),*[t['parent']for t in o['states']])))
  punctuation(self);v.call(0x403300);v.call(0x405750)
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
  v.call(0x40df90,(0,),this=self.animator,instruction_limit=5000000)
  v.put_u32(self.animator+0x14,0);native_owner(self)
 def observe_pulse(self,uc,*_):
  v=self.vm;c=uc.reg_read(UC_X86_REG_ECX);emitters=[]
  for i in range(v.u32(c+0x18)):
   e=v.u32(v.u32(c+0x28)+4*i)
   emitters.append(dict(position=list(struct.unpack('<3f',v.uc.mem_read(e+0x10,12))),scale=list(struct.unpack('<3f',v.uc.mem_read(e+0x40,12))),matrix=list(struct.unpack('<16f',v.uc.mem_read(e+0x58,64)))))
  self.pulse_inputs.append(dict(controller=c,frame=v.u32(self.animator+0x14),position=list(struct.unpack('<3i',v.uc.mem_read(self.owner+0x10,12))),origin=list(struct.unpack('<3f',v.uc.mem_read(c+0x170,12))),emitters=emitters))
 def controller_state(self):
  v=self.vm;result=[]
  for i in range(v.u32(self.animator+0x58)):
   c=v.u32(v.u32(self.animator+0x68)+4*i);vt=v.u32(c);assert vt==0x5a357c
   result.append(dict(vtable=hex(vt),state=v.u32(c+4),tag_frame=v.u32(c+8),blend=v.u32(c+0x2c)))
  return result
 def step(self):
  v=self.vm
  if v.u32(self.owner+8)&0x1000:return
  self.pulse_inputs=[]
  v.call(0x470cc0,this=self.owner,instruction_limit=5000000)
  v.call(self.owner_pulse,this=self.owner,instruction_limit=5000000)
 def animate(self):
  self.vm.call(0x40e460,(0,),this=self.animator,instruction_limit=5000000);native_owner(self)
 def state(self):
  v=self.vm;o=self.owner;a=self.animator
  return dict(state=v.u32(o+0xc)&65535,frame=struct.unpack('<h',v.uc.mem_read(o+0x5c,2))[0],
   position=list(struct.unpack('<3i',v.uc.mem_read(o+0x10,12))),velocity=list(struct.unpack('<3i',v.uc.mem_read(o+0x184,12))),
   counter=v.u32(o+0x190),ground_z=v.u32(o+0x194),command_done=v.u32(o+0x80),flags=v.u32(o+8),
   cached_state=v.u32(a+0xc),cached_frame=v.u32(a+0x14),seed=v.rng_seed,rng_draws=self.rng_draws,
   controllers=self.controller_state(),pulse_inputs=self.pulse_inputs,object_matrices=[list(struct.unpack('<16f',v.uc.mem_read(p+0x58,64)))for p in self.animobjs])
 def render(self):
  self.draws=[];self.software.clear()
  if not(self.vm.u32(self.owner+8)&0x1000):self.vm.call(self.anim_render,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=260);p.add_argument('--render-stride',type=int,default=1);p.add_argument('--width',type=int,default=1280);p.add_argument('--height',type=int,default=960);p.add_argument('--center',type=int,nargs=2,default=(640,640));p.add_argument('--sample-stride',type=int,default=0);p.add_argument('--state-only',action='store_true');args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
 assert sha(args.executable.read_bytes())=='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/gvortex.I3D')
 assert sha(data)=='8ae869f7c4005dd74aaf080008677b53cc6a96c2f500d6c6573ef52e19c1e759'
 f=GVortexScene(args.executable,data,args.width,args.height,args.center);initial=f.state();records=[]
 selected=(0,1,4,9,19,29,39,59,79,99,119,139,159,179,199,219,238,239,249,259)
 if args.sample_stride>0:selected=tuple(range(1,args.frames,args.sample_stride))
 for tick in range(args.frames):
  f.step();before=f.state();draw=args.render_stride>0 and (tick+1)%args.render_stride==0
  f.raster_enabled=not args.state_only and tick in selected and args.render_stride==1
  if draw:f.animate();pixels=f.render()
  else:f.draws=[];f.software.clear();pixels=f.vm.surface_bytes('screen')
  if f.raster_enabled:save_rgb565_png(args.output/f'native-{tick:03}.png',pixels,args.width,args.height)
  records.append(dict(tick=tick,before=before,after=f.state(),rendered=draw,draws=f.draws if tick in selected else []));print(tick,before['state'],before['frame'],before['position'],len(f.draws),flush=True)
 report=dict(dispatch=f.dispatch,effect='GVortex',type_id='0xad99bd33',initial=initial,records=records,render_stride=args.render_stride,sampled_captures=list(selected),
  executable_sha256=sha(args.executable.read_bytes()),asset_sha256=sha(data),source_sha256=sha(Path(__file__).read_bytes()),
  policy=dict(null_spell=True,source=[0,0,0],facing=0,ground_height=1,camera=[0,0,0],viewport=[args.width,args.height],screen_center=list(args.center),ambient=[38,38,38],directional=[1,1,1],seed=1,
   executed=['470cc0','4de800','40df90','40e2e0','40e460','40e8d0','40a8f0','56eb30'],
   boundaries=['decoded literal I3D resource/COM surfaces','silent audio','flat walk-height 1 for particle queries','map redraw/lock notifications','base script callback'],
   limits=['No natural spell targeting/damage/audio acceptance','Only original live-owner leaf, not natural scene/device caller']))
 (args.output/'trace.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
