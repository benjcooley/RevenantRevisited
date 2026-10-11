#!/usr/bin/env python3
"""Full original Flies constructors/pool/RenderObject over its exact legacy0 mesh."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from speed_controller_preflight import NativeSpeed,ROOT
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from firecone_state_probe import sha
from fountain_probe import FountainFixture
from software_probe import save_rgb565_png
ASSET_SHA='f878ea797286e80d4f90ebe5454be2d57f2ecd84f83a7af4da323c9d815c1797'
CAPTURES=(5,15,29,45,59,89)
def read_mesh(data):
 assert sha(data)==ASSET_SHA;u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];r=lambda p:p+u(p);b=20+u(16);verts=r(r(b+8));faces=r(b+268);keys=r(b+6620);texture=r(r(b+1784))
 assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),u(b+1848))==(0,4,2,1,1,1,1)
 assert struct.unpack_from('<8I',data,b+920+72)==(32,65,0,16,0xf00,0xf0,0xf,0xf000)
 assert struct.unpack_from('<2I',data,b+920+8)==(8,8);frames=h(70);assert frames==100
 rawkeys=data[keys:keys+frames*8];assert len(rawkeys)==frames*8
 obj=dict(index=0,name=data[b+1884:data.index(0,b+1884)].decode(),material=h(b+3932),vertex_start=h(b+4060),vertex_count=h(b+4188),texture_runs=[h(b+4316),h(b+5340),h(b+4318),h(b+5342)],vertex_hex=data[verts:verts+128].hex(),states=[dict(parent=-1,keys=[0]*frames,legacy_key_hex=rawkeys.hex())])
 assert obj['name']=='box01'and obj['material']==obj['vertex_start']==0 and obj['vertex_count']==4 and obj['texture_runs']==[0,0,0,2]
 return dict(name='Flies',sha256=ASSET_SHA,body_offset=b,flags=0,version=0,vertices=4,faces=2,materials=1,textures=1,states=1,state0_frames=frames,objects=[obj],tags=[],face_hex=data[faces:faces+12].hex(),vertex_hex=obj['vertex_hex'],material_hex=data[b+276:b+356].hex(),texture_hex=data[texture:texture+128].hex(),texture_pixel_format_hex=data[b+992:b+1024].hex())

class NativeFlies(NativeSpeed):
 observe_random=FountainFixture.observe_random
 def setup_resources(self):
  NativeSpeed.setup_resources(self);v=self.vm;a=self.asset;v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',self.bytes(struct.pack('<I',self.bytes(bytes.fromhex(a['vertex_hex'])))))));v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  resource=v.u32(v.u32(self.imagery+0x74));
  for off,value in [(0x24,4),(0x28,0),(0x30,0),(0x3c,0)]:v.put_u32(resource+off,value)
  v.put_u32(resource+0x34,self.bytes(struct.pack('<2I',0,0)));v.put_u32(resource+0x38,self.bytes(struct.pack('<2I',0,2)))
  material=v.allocate(0x58);v.write(material,bytes.fromhex(a['material_hex']));v.put_u32(material+0x54,0);v.put_u32(self.imagery+0x34,1);v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<I',material)))
  self.software.set_texture(8,8,bytes.fromhex(a['texture_hex']),format='ARGB4444');node=v.allocate(0xa0);v.put_u32(node+0x7c,1);v.put_u32(node+0x90,self.software.texture_object);v.put_u32(node+0x94,1);v.put_u32(self.imagery+0x4c,1);v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<I',node)));v.put_u32(self.imagery+0x10,1)
 def copy_vertices(self,*args):pass
 def __init__(self,exe,data,width=1280,height=960):
  a=read_mesh(data);NativeSpeed.__init__(self,exe,a,width,height,software_factory=LitSoftwareFixture,particle_required=False,native_binding=(0x4f6790,0x66ce40,0x5aa754),eager_pose=False);v=self.vm;self.random=[];self.random_pending=[];self.crt_draws=0;v.rng_seed=1;self.draws=[];self.render_states={};self.draw_return=None
  self.software.lighting((38,38,38),(1,1,1))
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  method=v.allocate(16);v.write_code(method,b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');vt=v.allocate(0x200);v.put_u32(vt+0x24,method);v.put_u32(self.owner,vt)
  flags=v.allocate(16);v.write_code(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');v.put_u32(vt+0x40,flags);v.put_u32(self.owner+0x4c,self.imagery);done=v.allocate(16);v.write_code(done,b'\x31\xc0\xc2\x04\x00');v.put_u32(vt+0x158,done)
  device=v.allocate(4);dvt=v.allocate(0xa0);v.put_u32(device,dvt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in [(23,'FliesGetLightState',getlight),(24,'FliesSetLightState',setlight)]:v.handlers[name]=(3,method);v.put_u32(dvt+slot*4,v.api_address('kernel32.dll',name))
  def rand(uc,address,size,user):self.crt_draws+=1;self.ret(v.random_msvc(),0)
  v.uc.hook_add(UC_HOOK_CODE,rand,begin=0x58c582,end=0x58c582)
  for address in (0x483300,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
  def state(uc,address,size,user):sp=uc.reg_read(UC_X86_REG_ESP);self.render_states[v.u32(sp+4)]=v.u32(sp+8)
  v.uc.hook_add(UC_HOOK_CODE,state,begin=0x56d400,end=0x56d400)
  def finished(uc,address,size,user):rows=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(self.software.vertices,self.draws[-1]['args'][3]*36)));self.draws[-1]['geometry_bounds']=[min(p[0]for p in rows),-max(p[1]for p in rows),max(p[0]for p in rows),-min(p[1]for p in rows)]
  def draw(uc,address,size,user):
   sp=uc.reg_read(UC_X86_REG_ESP);self.draws.append(dict(args=[v.u32(sp+4+i*4)for i in range(7)],states=dict(self.render_states)));ret=v.u32(sp)
   if self.draw_return is None:self.draw_return=ret;v.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
   else:assert ret==self.draw_return
  v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
  v.call(0x4e8180,this=self.animator,instruction_limit=100000000)
  assert v.u32(self.animator+0xfc+0x1c)==20
 def state(self):
  v=self.vm;pointer=v.u32(self.animator+0xfc+4);return dict(random_count=len(self.random),crt_draws=self.crt_draws,rng_seed=v.rng_seed,particles=[dict(values=list(struct.unpack('<9f',v.uc.mem_read(pointer+i*0x58,36))),used=v.u32(pointer+i*0x58+0x54))for i in range(20)])
 def animate(self,tick):self.vm.put_u32(0x65cb38,tick);self.vm.call(0x4e8260,this=self.animator,instruction_limit=100000000)
 def render(self):
  before=self.state();owner=native_owner(self);clear=((int(.45*255)>>3)<<11)|((int(.46*255)>>2)<<5)|(int(.50*255)>>3);self.software.clear(clear);self.draws=[];self.render_states={};self.vm.call(0x4e83d0,this=self.animator,instruction_limit=100000000);assert before==self.state();return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth'),dict(owner=owner,draws=self.draws,state=before,clear_rgb565=clear)
def run(output):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/flies.i3d'))
 cases=[];controls=[];previous=[];states_hash=None
 for replay in range(2):
  f=NativeFlies(exe,data);trace=[];drawlog=[]
  for tick in range(91):
   if tick:f.animate(tick)
   trace.append(dict(tick=tick,**f.state()))
   if tick-1 not in CAPTURES and tick not in (0,1,2):continue
   raw,depth,record=f.render();pair=(sha(raw),sha(depth),sha(json.dumps(record,sort_keys=True).encode()));
   if tick in (0,1,2):
    assert not record['draws'];assert all(x==(record['clear_rgb565'],)for x in struct.iter_unpack('<H',raw));
    if not replay:controls.append(dict(tick=tick,draws=0,random_count=f.state()['random_count'],rgb565_sha256=pair[0],depth_sha256=pair[1]));
    continue
   bounds=[d['geometry_bounds']for d in record['draws']];assert bounds and all(0<b[0]<=b[2]<1279 and 0<b[1]<=b[3]<959 for b in bounds)
   if replay:assert pair==previous[CAPTURES.index(tick-1)],'Fresh original replay differs'
   else:
    previous.append(pair);image=output/f'native-capture{tick-1:03d}.png';save_rgb565_png(image,raw,1280,960);drawlog.append(record);cases.append(dict(name='Flies',id='0xa21d9d00',capture=tick-1,tick=tick,draws=len(record['draws']),image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=pair[0],depth_sha256=pair[1],draw_sha256=pair[2],geometry_bounds=bounds,whole_geometry_unclipped=True,changed_pixels=sum(x!=(record['clear_rgb565'],)for x in struct.iter_unpack('<H',raw))));print(tick,cases[-1]['draws'],cases[-1]['changed_pixels'],flush=True)
  encoded=json.dumps(trace,sort_keys=True).encode()
  if replay:assert sha(encoded)==states_hash
  else:
   states_hash=sha(encoded);(output/'state-trace.json').write_text(json.dumps(trace,indent=2)+'\n');(output/'draws.json').write_text(json.dumps(drawlog,indent=2)+'\n');(output/'rng-inputs.txt').write_text(''.join(f'{a} {b} {v}\n'for a,b,v in f.random))
 r=dict(status='reference_complete',accepted=False,name='Flies',id='0xa21d9d00',cases=cases,controls=controls,asset_sha256=ASSET_SHA,constructor=f.constructor,viewport=[1280,960],origin=[0,0,0],camera=[0,0,0],background='ltgray',clear_rgb565=record['clear_rgb565'],seed=1,independent_replays=2,state_sha256=states_hash,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Complete actual Flies factory/base/Initialize/TParticleSystem/all91poolRNGstates/owner/fullRenderObject/FVF_VERTEX/8x8ARGB4444/software blend/raster. Actual firstthree empty/RNG controls retained. Two fresh replays of allstate plus six fullunclipped image/depth/draw sets. Gray565clear reveals dark authored sprites; native blackbackground visibility limitation retained. Exact immutable legacy0 resource/singleframeCOM/nullspellowner/materialhandle boundaries; no production geometry/UV/poses/colors/raster supplied. Actual Metal/map/naturalcaller/device/exactpixel gates separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--whole',action='store_true');a=p.parse_args()
 if a.whole:print(run(a.output)['status']);raise SystemExit
 a.output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/flies.i3d'))
 f=NativeFlies(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data)
 for tick in range(1,61):f.animate(tick)
 raw,depth,record=f.render();save_rgb565_png(a.output/'native-tick060.png',raw,1280,960);clear=record['clear_rgb565'];r=dict(status='full_native_preflight',accepted=False,asset_sha256=ASSET_SHA,constructor=f.constructor,state=f.state(),draw=record,changed_pixels=sum(x!=(clear,)for x in struct.iter_unpack('<H',raw)),image_sha256=sha((a.output/'native-tick060.png').read_bytes()),depth_sha256=sha(depth),scope='Exact legacy0 shipped quad/material8x8ARGB4444/frame residency; full constructor/base/Initialize/TParticleSystem/60Animate/owner/fullRenderObject/FVF/software draw. Explicit nullspell/readonly resource/COM/material-handle interfaces; gray565 background reveals authored darkflies. No production geometry, pose, UV, colors or raster supplied to native.');(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],len(record['draws']),r['changed_pixels'])
