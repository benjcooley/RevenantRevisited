#!/usr/bin/env python3
"""Original WindStrip factory/leaf lifecycle/full RenderObject and authored software pixels."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from fireball_visual_reference import FireBallScene
from speed_controller_preflight import NativeSpeed,read_asset,ROOT,sha
from simple_effect_preflight import native_dispatch
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner

def read_windstrip(data):
 assert sha(data)=='d33eeaa85ebcad2ef0ef55c258ae259bd92bb3cb2f204589cb7724f502fb941a'
 a=read_asset(data);assert a['version']==1 and a['states']==3 and a['vertices']==24 and a['faces']==24
 for o in a['objects']:
  for t in o['states']:t['frames']=len(bytes.fromhex(t['legacy_key_hex']))//8;t['raw_keys']=bytes.fromhex(t['legacy_key_hex'])

 return a

class WindStripScene(FireBallScene):
 def __init__(self,exe,data):
  self.data=data;self.asset=read_windstrip(data);self.software=LitSoftwareFixture(exe,640,480);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  native_dispatch(v);builder=v.call(0x40dca0,(self.put_string('WindStrip'),));factory=v.u32(v.u32(builder));assert(builder,factory)==(0x66dea8,0x5237d0)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h);assert v.u32(self.animator)==0x5b6124
  self.dispatch=dict(builder=hex(builder),factory=hex(factory),animator_vtable=hex(v.u32(self.animator)),owner_init='0x5227f0',owner_pulse='0x522800',init='0x5229c0',animate='0x522bc0',render='0x522fa0')
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.eager_pose=False;NativeSpeed.setup_resources(self);self.setup_resources()
  # Original generic owner virtual methods, null spell and non-inventory frame.
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1))
  method=self.bytes(b'\xb8'+struct.pack('<I',struct.unpack_from('<H',data,68)[0])+b'\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x8c,method)
  vt_owner=v.allocate(0x200);v.write(vt_owner,bytes(v.uc.mem_read(0x5b5f20,0x184)));v.put_u32(self.owner,vt_owner)
  def flags(args):v.put_u32(self.owner+8,args[0]);return 0
  v.handlers['WindStripFlags']=(1,flags);v.put_u32(vt_owner+0x40,v.api_address('kernel32.dll','WindStripFlags'))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x40c960:0,0x40c9c0:4,0x4708e0:0,0x4713c0:0,0x49c430:4}
  for a in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=a,end=a)
  self.rng_draws=0;v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582);v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in ((23,'WindStripGetLight',getlight),(24,'WindStripSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True;self.tick=0
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  v.rng_seed=1;v.call(0x5227f0,this=self.owner);assert v.u32(self.owner+0xd8)==0;v.put_u32(self.animator+0x44,0);v.call(0x522af0,this=self.animator);self.animobjs=[v.u32(v.u32(self.animator+0x54)+i*4)for i in range(v.u32(self.animator+0x44))];v.call(0x5229c0,this=self.animator);native_owner(self)
 def observe_object(self,uc,*_):
  from unicorn.x86_const import UC_X86_REG_ESP
  obj=self.vm.u32(uc.reg_read(UC_X86_REG_ESP)+4);self.current_object=self.vm.u32(obj+4)
 def state(self):
  v=self.vm;a=self.animator
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),frame=v.u32(a+0x39c),halo=struct.unpack('<f',v.uc.mem_read(a+0x398,4))[0],center=list(struct.unpack('<3f',v.uc.mem_read(a+0x10c,12))),drop_pointer=v.u32(a+0x3a4),seed=v.rng_seed,rng_draws=self.rng_draws)
 def full_state(self):
  v=self.vm;a=self.animator;n=self.state();n['strips']=[];n['drops']=[]
  for i in range(20):
   fields=[struct.unpack('<f',v.uc.mem_read(a+offset+i*4,4))[0]for offset in(0x118,0x168,0x1b8,0x208,0x258,0x2a8,0x2f8,0x348)]
   strip=v.u32(a+0x3a8+i*4);count=v.u32(strip+0x18);points=list(struct.unpack('<'+str(count*3)+'f',v.uc.mem_read(v.u32(strip+0x1c),count*12)))
   n['strips'].append(fields+points+[0.]*(30-len(points)))
  if n['drop_pointer']:
   for i in range(400):
    p=n['drop_pointer']+i*32;used=v.u32(p+28);n['drops'].append([used]+(list(struct.unpack('<7f',v.uc.mem_read(p,28)))if used else [0.]*7))
  else:n['drops']=[[0]*8 for _ in range(400)]
  return n
 def advance(self):
  self.tick+=1;v=self.vm
  if not self.state()['alive']:return
  v.call(0x522800,this=self.owner)
  if not self.state()['alive']:return
  v.call(0x470cc0,this=self.owner)
  v.call(0x522bc0,(1,),this=self.animator);v.put_u32(self.animator+0x14,struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0]);native_owner(self)
 def render(self):
  self.draws=[];self.software.clear()
  if self.state()['alive']:self.vm.call(0x522fa0,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def run(exe,archive,output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/wind.i3d'))
 f=WindStripScene(exe,data);rows=[];samples={2,10,25,50,75,99,100,125,150,151,175,200,225,251,252,255}
 for tick in range(256):
  if tick:f.advance()
  f.raster_enabled=tick in samples
  raw=f.render()if tick else f.vm.surface_bytes('screen')
  row=dict(tick=tick,state=f.state(),draws=f.draws)
  if tick in samples:
   p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,640,480);row.update(image=str(p),image_sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)))
  rows.append(row)
  if tick%25==0:print(tick,f.state(),len(f.draws),flush=True)
 report=dict(retail_name='WindStrip',retail_type_id='0xded3dbed',asset_sha256=sha(data),original_executable_sha256=sha(exe.read_bytes()),dispatch=f.dispatch,cases=rows,viewport=[640,480],source_ambient=[38]*3,source_directional=[1]*3,seed=1,null_spell=True,probe_sha256=sha(Path(__file__).read_bytes()),scope='Original complete factory, SetupObjects, Init, owner Pulse then NextFrame then custom Animate, full RenderObject/native owner/original software. All256 ticks/full Render RNG; raster enabled for16 declared phases. Immutable decoded resources/COM texture residency and base animator/script/scene-extents services are boundaries. Equal-width GenerateStrip reads uninitialized tiny incoming stack residue, integer width contribution0; source reproduces observed7-unit halfwidth, no broader caller-stack assertion. Natural spell/invoker orbit, damage, nonzero facing and actor caller remain open.',accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
