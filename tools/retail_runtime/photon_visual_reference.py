"""Original Photon full leaf Animate/Render with explicit missile/world interfaces."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from fireball_visual_reference import FireBallScene
from speed_controller_preflight import NativeSpeed,ROOT,sha
from lit_software_probe import LitSoftwareFixture
from software_probe import SoftwareFixture,save_rgb565_png
from projectile_probe import MissileProbe

ASSET_SHA='c06262a0ab7029ab8580ae71edfd31f0dcb98691269076cf2654447634516a11'

def read_photon(data):
 assert sha(data)==ASSET_SHA,'Photon authored asset changed'
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);b=20+u(16)
 assert u(b)==0 and u(b+4)==4 and u(b+264)==2 and u(b+1880)==1 and u(b+916)==1
 assert u(b+1848)==5 and struct.unpack_from('<8I',data,b+920+72)==(32,65,0,16,0xf00,0xf0,0xf,0xf000)
 raw=r(r(b+8));faces=r(b+268);frames=[struct.unpack_from('<h',data,70+76*i)[0]for i in range(3)]
 return dict(flags=0,version=0,tags=[],states=3,state0_frames=frames[0],vertices=4,faces=2,materials=1,textures=1,body_offset=b,
  material=data[b+276:b+356],texture_desc=data[b+920:b+1028],texture_frames=[data[r(r(b+1784)+i*4):r(r(b+1784)+i*4)+8192]for i in range(u(b+1848))],
  face_hex=data[faces:faces+12].hex(),objects=[dict(index=0,name='photon',material=0,vertex_start=0,vertex_count=4,texture_runs=[0,0,0,2],vertex_hex=data[raw:raw+128].hex(),states=[dict(parent=-1,keys=[],frames=frames[i],raw_keys=data[r(b+6620+i*4):r(b+6620+i*4)+frames[i]*8])for i in range(3)])])

class PhotonScene(FireBallScene):
 def __init__(self,exe,data,animate_first=False):
  self.data=data;self.asset=read_photon(data);self.rng_draws=0;self.software=LitSoftwareFixture(exe,1024,768,camera=(0,0,0));self.vm=self.software.vm;v=self.vm;self.software.lighting((38,38,38),(1,1,1))
  self.missile=MissileProbe(exe,vm=v);self.missile.configure([-240,0,128],[760,0,128],speed=16,target_hp=0,animator_present=True)
  self.owner=self.missile.obj;self.imagery=v.allocate(0x200);self.boundaries={0x40dd60:0,0x49c430:4,0x49b880:4,0x49b990:24,0x471b50:4}
  for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
  v.call(0x510400,this=self.owner)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(0x514a50,(self.owner,),this=0x66d440);v.uc.hook_del(h);assert v.u32(self.animator)==0x5b401c;v.put_u32(self.owner+0x58,self.animator)
  self.required_pose_indices={0};self.unused_rejected_pose_indices=set();NativeSpeed.setup_resources(self);self.setup_resources()
  v.uc.hook_add(UC_HOOK_CODE,self.animate_base,begin=0x40e460,end=0x40e460);v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582)
  v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in((23,'PhotonGetLight',getlight),(24,'PhotonSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  v.rng_seed=1;v.call(0x5104c0,this=self.animator)
  self.current_texture_frame=0;self.draws=[];self.raster_enabled=True;self.states={};self.current_object=-1
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  if animate_first:v.call(0x510600,(0,),this=self.animator)
  self.initial=self.state()
 def setup_resources(self):
  v=self.vm;a=self.asset;o=a['objects'][0];raw=self.bytes(bytes.fromhex(o['vertex_hex']));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',self.bytes(struct.pack('<I',raw)))));v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  resource=v.u32(v.u32(self.imagery+0x74));
  for off,value in((0x24,4),(0x28,0),(0x30,0),(0x3c,0)):v.put_u32(resource+off,value)
  v.put_u32(resource+0x34,self.bytes(struct.pack('<2I',0,0)));v.put_u32(resource+0x38,self.bytes(struct.pack('<2I',0,2)));v.put_u32(resource+0x40,self.bytes(struct.pack('<3I',*[t['frames']for t in o['states']])));v.put_u32(resource+0x44,self.bytes(struct.pack('<3I',*[self.bytes(t['raw_keys'])for t in o['states']])))
  mat=v.allocate(0x58);v.write(mat,a['material']);v.put_u32(mat+0x54,0);v.put_u32(self.imagery+0x34,1);v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<I',mat)))
  self.surfaces=[]
  for i,pixels in enumerate(a['texture_frames']):
   s=object.__new__(SoftwareFixture);s.vm=v;s.texture_object=v.allocate(4);s.vtable=v.allocate(33*4);v.put_u32(s.texture_object,s.vtable);s.com_trace=[];s.locked=False;s.references=1
   for slot,name,argc,method in((1,'AddRef',1,s._addref),(2,'Release',1,s._release),(21,'GetPixelFormat',2,s._pixel_format),(25,'Lock',5,s._lock),(32,'Unlock',2,s._unlock)):
    label=f'PhotonSurface{i}{name}';v.handlers[label]=(argc,method);v.put_u32(s.vtable+slot*4,v.api_address('kernel32.dll',label))
   s.set_texture(64,64,pixels,format='ARGB4444');self.surfaces.append(s)
  t=v.allocate(0xa0);self.texture=t;v.write(t,a['texture_desc']);v.put_u32(t+0x7c,len(self.surfaces));v.put_u32(t+0x90,self.surfaces[0].texture_object);v.put_u32(t+0x94,1);v.put_u32(self.imagery+0x4c,1);v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<I',t)));v.put_u32(self.imagery+0x10,1)
 def texture_frame(self,uc,*_):
  sp=uc.reg_read(UC_X86_REG_ESP);slot=self.vm.u32(sp+4);frame=self.vm.u32(sp+8);assert slot==0 and frame<len(self.surfaces);self.current_texture_frame=frame;self.vm.put_u32(self.texture+0x90,self.surfaces[frame].texture_object);self.ret(1,8)
 def observe_draw(self,uc,*args):
  super().observe_draw(uc,*args);self.draws[-1]['texture_frame']=self.current_texture_frame
 def step(self):
  self.vm.call(0x510450,this=self.owner);self.missile.tick+=1;self.vm.advance();motion=self.missile.inspect();self.vm.call(0x510600,(0,),this=self.animator)
  from firecone_owner_probe import native_owner
  native_owner(self);return motion
 def state(self):
  v=self.vm;a=self.animator;read=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(p=[read(0xfc+i*12,3)for i in range(32)],v=[read(0x27c+i*12,3)for i in range(32)],scale=read(0x3fc,32),frame=list(struct.unpack('<32i',v.uc.mem_read(a+0x47c,128))),active=v.u32(a+0x4fc),numtexframes=v.u32(a+0x500),oldstate=v.u32(a+0x504),ready=v.u32(self.owner+0x18c),seed=v.rng_seed,rng_draws=self.rng_draws)
 def render(self):
  self.draws=[];self.software.clear();self.vm.call(0x5109c0,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen')

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',required=True,type=Path);p.add_argument('--frames',type=int,default=100);p.add_argument('--animate-first',action='store_true');p.add_argument('--state-only',action='store_true');args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/photon.i3d')
 f=PhotonScene(args.executable,data,args.animate_first);records=[]
 for tick in range(args.frames):
  motion=f.step();f.raster_enabled=not args.state_only and tick in(0,1,5,10,20,29,30,35,40,50,60,65,70,80,99,100);pixels=f.render();print(tick,motion['state'],motion['position'],len(f.draws),flush=True)
  if f.raster_enabled:save_rgb565_png(args.output/f'native-{tick:03d}.png',pixels,1024,768)
  records.append(dict(tick=tick,motion=motion,state=f.state(),draws=f.draws if f.raster_enabled else[]))
 (args.output/'trace.json').write_text(json.dumps(dict(initial=f.initial,records=records,executable_sha256=sha(args.executable.read_bytes()),asset_sha256=sha(data),animate_first=args.animate_first,scope='Original Photon Initialize/Animate/Render, original RenderObject/normal Illuminate and software raster; explicit owner/camera/world/resource interfaces. Pulse-first or one Animate-before-Pulse ordering. Audio and command-reaper intercepted; natural spell/device caller not certified.'),indent=2)+'\n')
