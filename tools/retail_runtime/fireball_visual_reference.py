"""Original FireBall factory, complete Animate/Render and software draw fixture.

Executes complete leaf Animate/Render and original RenderObject/software raster.
Resource decoding, base animation bookkeeping, audio and world queries remain
explicit interfaces; this is not a natural map/spell/device caller claim.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_ECX
from lit_software_probe import LitSoftwareFixture
from software_probe import SoftwareFixture,save_rgb565_png
from projectile_probe import MissileProbe
from speed_controller_preflight import NativeSpeed,ROOT,sha

def read_fireball(data):
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);bo=20+u(16);raw=r(r(r(bo+16)));faces=r(bo+24)
 a=dict(body_offset=bo,flags=u(bo),version=u(bo+4),vertices=u(bo+12),faces=u(bo+20),materials=u(bo+28),textures=u(bo+36),states=u(24),objects=[],tags=[],state0_frames=struct.unpack_from('<h',data,70)[0])
 assert a['version']==1 and a['states']==3
 for i in range(u(bo+44)):
  p=r(bo+48)+48*i;material,start,count=struct.unpack_from('<3H',data,p+32);tracks=[]
  for state in range(a['states']):
   st=r(p+44)+state*8;n=struct.unpack_from('<h',data,70+76*state)[0];tracks.append(dict(parent=struct.unpack_from('<i',data,st)[0],keys=[],raw_keys=data[r(st+4):r(st+4)+n*36],frames=n))
  runs=list(struct.unpack_from('<'+'H'*(2*(a['textures']+1)),data,r(p+40)))
  a['objects'].append(dict(index=i,name=data[p:p+32].split(b'\0')[0].decode(),material=material,vertex_start=start,vertex_count=count,states=tracks,texture_runs=runs,vertex_hex=data[raw+start*32:raw+(start+count)*32].hex()))
 a['face_hex']=data[faces:faces+6*a['faces']].hex();return a

class FireBallScene:
 bytes=NativeSpeed.bytes
 ret=NativeSpeed.ret
 imagery_lookup=NativeSpeed.imagery_lookup
 def __init__(self,exe,data,width=1024,height=768,visual_replay=False):
  self.rng_draws=0;self.data=data;self.asset=read_fireball(data);self.software=LitSoftwareFixture(exe,width,height,camera=(0,0,0) if visual_replay else (240,0,128));self.vm=self.software.vm;v=self.vm
  self.software.lighting((38,38,38),(1,1,1))
  self.missile=MissileProbe(exe,vm=v);self.missile.configure([-240,0,78] if visual_replay else [0,0,78],[760,0,128] if visual_replay else [1000,0,128],target_hp=0,launch_ready=True,animator_present=True)
  self.owner=self.missile.obj;v.call(0x510bd0,this=self.owner);self.imagery=v.allocate(0x200)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0)
  self.animator=v.call(0x514bf0,(self.owner,),this=0x66d460);v.uc.hook_del(h)
  assert v.u32(self.animator)==0x5b4290
  v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices=set(range(3));self.unused_rejected_pose_indices=set();self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  NativeSpeed.setup_resources(self)
  self.setup_resources()
  self.boundaries={0x40dd60:0,0x49c430:4,0x471b50:4}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  v.uc.hook_add(UC_HOOK_CODE,self.animate_base,begin=0x40e460,end=0x40e460)
  v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):assert tuple(args[:2])==(device,1);v.put_u32(args[2],self.material_state);return 0
  def setlight(args):assert tuple(args[:2])==(device,1);self.material_state=args[2];return 0
  for slot,name,method in [(23,'FireBallGetLight',getlight),(24,'FireBallSetLight',setlight)]:v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1;v.call(0x510d00,this=self.animator)
  if visual_replay:v.random_msvc() # SpawnForTest consumes one outer aim draw before endpoints override.
  self.draws=[];self.raster_enabled=True;self.states={};self.current_object=-1
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0)
  v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30)
  v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
 def external(self,uc,address,size,user):self.ret(0xffffffff if address==0x49c430 else 0,self.boundaries[address])
 def rng(self,*_):self.rng_draws+=1;self.ret(self.vm.random_msvc(),0)
 def animate_base(self,*_):self.vm.put_u32(self.animator+0xc,self.vm.u32(self.owner+0xc)&65535);self.ret(0,4)
 def texture_frame(self,*_):self.ret(1,8)
 def observe_state(self,uc,*_):sp=uc.reg_read(UC_X86_REG_ESP);self.states[self.vm.u32(sp+4)]=self.vm.u32(sp+8)
 def observe_object(self,uc,*_):
  obj=self.vm.u32(uc.reg_read(UC_X86_REG_ESP)+4);self.current_object=self.animobjs.index(obj)
 def observe_draw(self,uc,*_):
  v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);a=[v.u32(sp+4+i*4)for i in range(7)]
  self.draws.append(dict(object=self.current_object,arguments=a,states=dict(self.states),vertices_hex=bytes(v.uc.mem_read(a[2],a[3]*32)).hex(),indices_hex=bytes(v.uc.mem_read(a[4],a[5]*2)).hex(),matrix=list(struct.unpack('<16f',v.uc.mem_read(0x675ed0,64)))))
  if not self.raster_enabled:self.ret(0,28)
 def setup_resources(self):
  v=self.vm;data=self.data;a=self.asset;u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);bo=a['body_offset']
  raw=r(r(r(bo+16)));pointer=self.bytes(data[raw:raw+a['vertices']*32]);pointer=self.bytes(struct.pack('<I',pointer));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',pointer)))
  v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o,obj in zip(a['objects'],self.animobjs):
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in [(0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])]:v.put_u32(resource+off,value)
   runs=o['texture_runs'];v.put_u32(resource+0x34,self.bytes(struct.pack('<'+'I'*len(runs[::2]),*runs[::2])));v.put_u32(resource+0x38,self.bytes(struct.pack('<'+'I'*len(runs[1::2]),*runs[1::2])))
   v.put_u32(resource+0x40,self.bytes(struct.pack('<'+'I'*len(o['states']),*[t['frames']for t in o['states']])))
   keys=[self.bytes(t['raw_keys'])for t in o['states']];v.put_u32(resource+0x44,self.bytes(struct.pack('<'+'I'*len(keys),*keys)))
  materials=[];rawmat=r(bo+32)
  for i in range(a['materials']):
   mat=v.allocate(0x58);v.write(mat,data[rawmat+i*80:rawmat+i*80+80]);v.put_u32(mat+0x54,0);materials.append(mat)
  v.put_u32(self.imagery+0x34,len(materials));v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<'+'I'*len(materials),*materials)))
  self.surfaces=[];textures=[]
  for i in range(a['textures']):
   tex=r(bo+40)+120*i;h,w=struct.unpack_from('<2I',data,tex+8);pixels=r(r(tex+108));pf=struct.unpack_from('<8I',data,tex+72)
   s=object.__new__(SoftwareFixture);s.vm=v;s.texture_object=v.allocate(4);s.vtable=v.allocate(33*4);v.put_u32(s.texture_object,s.vtable);s.com_trace=[];s.locked=False;s.references=1
   for slot,name,argc,method in [(1,'AddRef',1,s._addref),(2,'Release',1,s._release),(21,'GetPixelFormat',2,s._pixel_format),(25,'Lock',5,s._lock),(32,'Unlock',2,s._unlock)]:
    label=f'FireBallSurface{i}{name}';v.handlers[label]=(argc,method);v.put_u32(s.vtable+slot*4,v.api_address('kernel32.dll',label))
   fmt='ARGB4444'if pf[4:8]==(0xf00,0xf0,0xf,0xf000)else'RGB565';s.set_texture(w,h,data[pixels:pixels+w*h*2],format=fmt);self.surfaces.append(s)
   t=v.allocate(0xa0);v.put_u32(t+0x94,1);v.put_u32(t+0x90,s.texture_object);textures.append(t)
  v.put_u32(self.imagery+0x4c,len(textures));v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<'+'I'*len(textures),*textures)));v.put_u32(self.imagery+0x10,1)
 def step(self):
  motion=self.missile.step(1)[-1];self.vm.call(0x510e40,(0,),this=self.animator,instruction_limit=5000000)
  from firecone_owner_probe import native_owner
  native_owner(self);return motion
 def state(self):
  from fireball_spark_probe import SparkFixture
  from fireball_trail_probe import TrailFixture
  self.pool=self.vm.u32(self.animator+0x2c8);self.rng_calls=[];self.trail_state=lambda:TrailFixture.trail_state(self)
  result=SparkFixture.state(self);v=self.vm
  result['burst']=[]
  for i in range(10):
   x,y,z,scale,rot,frame,glow,used=struct.unpack('<4fiffI',v.uc.mem_read(self.animator+0x364+i*32,32));result['burst'].append(dict(position=[x,y,z],scale=scale,rotation=rot,frame=frame,glow=glow,used=bool(used)))
  result['ring_scale']=struct.unpack('<f',v.uc.mem_read(self.animator+0x300,4))[0];result['ring_done']=bool(v.u32(self.animator+0x354));result['rng_draws']=self.rng_draws;return result
 def render(self):
  self.draws=[];self.software.clear();self.vm.call(0x512150,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen')

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--frames',type=int,default=90);p.add_argument('--state-only',action='store_true');p.add_argument('--visual-replay',action='store_true');args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
 assert sha(args.executable.read_bytes())=='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/newfireball.i3d')
 assert sha(data)=='3c78b8e2dbb1251e9267b65caa92b038a026c8d1fa2919cc75e1d8ec7cd7a6a1'
 f=FireBallScene(args.executable,data,visual_replay=args.visual_replay);records=[]
 for tick in range(args.frames):
  motion=f.step();f.raster_enabled=not args.state_only and tick in(0,1,5,10,20,40,59,60,65,70,80,89);pixels=f.render();print(tick,motion['state'],motion['position'],len(f.draws),sum(x!=(0,)for x in struct.iter_unpack('<H',pixels)),flush=True)
  if f.raster_enabled:save_rgb565_png(args.output/f'native-{tick:03d}.png',pixels,1024,768)
  records.append(dict(tick=tick,motion=motion,draws=f.draws if f.raster_enabled else [],state=f.state()))
 trace=args.output/'trace.json';trace.write_text(json.dumps(records,indent=2))
 report=dict(retail_sha256=sha(args.executable.read_bytes()),asset_sha256=sha(data),trace_sha256=sha(trace.read_bytes()),source_sha256=sha(Path(__file__).read_bytes()),frames=args.frames,visual_replay=args.visual_replay,outer_preview_rng_draws=int(args.visual_replay),original_functions=['514bf0','510d00','510220','470920','510e40','512150','40a8f0','4174b0','56eb30'],boundaries=['decoded I3D resource/COM surfaces','silent audio','base animation state bookkeeping; native owner matrix executed separately','MissileProbe no live target; ground height 1','single texture frame'],camera=[0,0,0]if args.visual_replay else[240,0,128],source=[-240,0,78]if args.visual_replay else[0,0,78],ambient=[38,38,38],directional=[1,1,1],images=[dict(path=str(p),sha256=sha(p.read_bytes()))for p in sorted(args.output.glob('native-*.png'))],full_natural_caller=False,actual_metal=False)
 (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
