#!/usr/bin/env python3
"""Original Puke factory/leaf lifecycle/full RenderObject and authored software pixels."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from fireball_visual_reference import FireBallScene
from speed_controller_preflight import NativeSpeed,read_asset,ROOT,sha
from simple_effect_preflight import native_dispatch
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner

def read_puke(data):
 assert sha(data)=='9319c4d31f2c57beea63a066352b6d6fefd5b8148253a574d983102abfa93f8c'
 a=read_asset(data);assert a['version']==1 and a['states']==1 and a['vertices']==36 and a['faces']==36
 u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p)
 for o in a['objects']:
  p=r(a['body_offset']+48)+48*o['index'];st=r(p+44);n=a['state0_frames']
  o['states']=[dict(parent=struct.unpack_from('<i',data,st)[0],keys=[],raw_keys=data[r(st+4):r(st+4)+n*8],frames=n)]
 return a

class PukeScene(FireBallScene):
 def __init__(self,exe,data):
  self.data=data;self.asset=read_puke(data);self.software=LitSoftwareFixture(exe,640,480);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  native_dispatch(v);builder=v.call(0x40dca0,(self.put_string('puke'),));factory=v.u32(v.u32(builder));assert(builder,factory)==(0x66cf80,0x500510)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h);assert v.u32(self.animator)==0x5ae610
  self.dispatch=dict(builder=hex(builder),factory=hex(factory),animator_vtable=hex(v.u32(self.animator)),owner_init='0x4fcd20',owner_pulse='0x4fcd40',init='0x4fcd70',animate='0x4fcda0',render='0x4fd1c0')
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.eager_pose=False;NativeSpeed.setup_resources(self);self.setup_resources()
  # Original generic owner virtual methods, null spell and non-inventory frame.
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1))
  method=self.bytes(b'\xb8'+struct.pack('<I',struct.unpack_from('<H',data,68)[0])+b'\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x8c,method)
  vt_owner=v.allocate(0x200);v.write(vt_owner,bytes(v.uc.mem_read(0x5a85ac,0x184)));v.put_u32(self.owner,vt_owner)
  def flags(args):v.put_u32(self.owner+8,args[0]);return 0
  v.handlers['PukeFlags']=(1,flags);v.put_u32(vt_owner+0x40,v.api_address('kernel32.dll','PukeFlags'))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x40c960:0,0x40c9c0:4,0x4708e0:0,0x4713c0:0}
  for a in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=a,end=a)
  self.rng_draws=0;v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582);v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in ((23,'PukeGetLight',getlight),(24,'PukeSetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True;self.tick=0
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  v.rng_seed=1;v.call(0x4fcd20,this=self.owner);assert v.u32(self.owner+0x184)==1 and v.u32(self.owner+0x80)==0;v.call(0x4fcd70,this=self.animator);native_owner(self)
 def state(self):
  v=self.vm;a=self.animator;floats=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),life=v.u32(a+0x3c8),scale=floats(0x3c4,1)[0],positions=[floats(0xfc+i*12,3)for i in range(25)],velocities=[floats(0x228+i*12,3)for i in range(25)],starts=list(struct.unpack('<25i',v.uc.mem_read(a+0x360,100))),firsttime=v.u32(a+0x3cc),seed=v.rng_seed,rng_draws=self.rng_draws)
 def advance(self):
  self.tick+=1;v=self.vm
  if not self.state()['alive']:return
  v.call(0x470cc0,this=self.owner);v.call(0x4fcd40,this=self.owner)
  if not self.state()['alive']:return
  v.call(0x4fcda0,this=self.animator);v.put_u32(self.animator+0x14,struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0]);native_owner(self)
 def render(self):
  self.draws=[];self.software.clear()
  if self.state()['alive']:self.vm.call(0x4fd1c0,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def run(exe,archive,output,ticks=32):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/puke.i3d'))
 f=PukeScene(exe,data);rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  raw=f.render();p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,640,480)
  rows.append(dict(tick=tick,state=f.state(),draws=f.draws,image=str(p),image_sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))));print(tick,rows[-1]['state']['life'],len(f.draws),rows[-1]['nonzero_pixels'],flush=True)
 # Independent fresh actual factory replay checks every state, packet and pixel.
 replay=PukeScene(exe,data)
 for tick,row in enumerate(rows):
  if tick:replay.advance()
  raw=replay.render();assert row['state']==replay.state() and row['draws']==replay.draws and row['rgb565_sha256']==sha(raw), f'Replay differs at {tick}'
 report=dict(warm_replays=2,retail_type_id='0xad92bc11',asset_sha256=sha(data),dispatch=f.dispatch,cases=rows,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,seed=1,null_spell=True,accepted=False,scope='Actual complete registered factory, original leaf Init/Animate/Render, original owner NextFrame/Pulse/KillThisEffect, full RenderObject/native owner/matrix/Illuminate/software raster. Exact immutable V1 eight-byte keys, geometry/material/texture residency; base animator bookkeeping/extents and owner script/light/map-flag notification services are boundaries. No live spell, invoker, damage or natural actor-mouth attachment.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
