#!/usr/bin/env python3
"""Original EnergySpray factory/leaf lifecycle/full RenderObject and authored software pixels."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from fireball_visual_reference import FireBallScene
from speed_controller_preflight import NativeSpeed,read_asset,ROOT,sha
from simple_effect_preflight import native_dispatch
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner

def read_energyspray(data):
 assert sha(data)=='bdca48f08b388c385c78acf6e3b01e35ca5d7969ff910b8a9dd2dbd66f1ac75f'
 a=read_asset(data);assert a['version']==2 and a['states']==1 and a['vertices']==11 and a['faces']==5
 for o in a['objects']:
  for t in o['states']:t['frames']=len(t['keys']);t['raw_keys']=struct.pack('<'+'I'*len(t['keys']),*t['keys'])
 return a

class EnergySprayScene(FireBallScene):
 def __init__(self,exe,data,width=640,height=480,screen_origin=None):
  self.data=data;self.asset=read_energyspray(data);self.software=LitSoftwareFixture(exe,width,height);self.software.lighting((38,38,38),(1,1,1));v=self.vm=self.software.vm
  if screen_origin is not None:
   v.put_u32(0x675fc0,screen_origin[0]);v.put_u32(0x675fc4,screen_origin[1])
  self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);self.put_string=lambda s:self.bytes(s.encode()+b'\0')
  native_dispatch(v);builder=v.call(0x40dca0,(self.put_string('energyspray'),));factory=v.u32(v.u32(builder));assert(builder,factory)==(0x66cff8,0x509390)
  h=v.uc.hook_add(UC_HOOK_CODE,self.imagery_lookup,begin=0x46e8a0,end=0x46e8a0);self.animator=v.call(factory,(self.owner,),this=builder);v.uc.hook_del(h);assert v.u32(self.animator)==0x5b0a28
  self.dispatch=dict(builder=hex(builder),factory=hex(factory),animator_vtable=hex(v.u32(self.animator)),owner_init='0x502b40',owner_pulse='0x502b70',init='0x502c50',animate='0x502da0',render='0x503110')
  self.required_pose_indices=set();self.unused_rejected_pose_indices=set();self.eager_pose=False;NativeSpeed.setup_resources(self);self.setup_resources()
  # Original generic owner virtual methods, null spell and non-inventory frame.
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1))
  method=self.bytes(b'\xb8'+struct.pack('<I',struct.unpack_from('<H',data,68)[0])+b'\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x8c,method)
  vt_owner=v.allocate(0x200);v.write(vt_owner,bytes(v.uc.mem_read(0x5a85ac,0x184)));v.put_u32(self.owner,vt_owner)
  def flags(args):v.put_u32(self.owner+8,args[0]);return 0
  v.handlers['EnergySprayFlags']=(1,flags);v.put_u32(vt_owner+0x40,v.api_address('kernel32.dll','EnergySprayFlags'))
  self.boundaries={0x40dd60:0,0x40e2e0:0,0x40c960:0,0x40c9c0:4,0x4708e0:0,0x4713c0:0,0x49c430:4}
  for a in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=a,end=a)
  self.rng_draws=0;v.uc.hook_add(UC_HOOK_CODE,self.rng,begin=0x58c582,end=0x58c582);v.uc.hook_add(UC_HOOK_CODE,self.texture_frame,begin=0x40c520,end=0x40c520)
  for addr,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(addr,value)
  device=v.allocate(4);vt=v.allocate(0xa0);v.put_u32(device,vt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,method in ((23,'EnergySprayGetLight',getlight),(24,'EnergySpraySetLight',setlight)):v.handlers[name]=(3,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  self.draws=[];self.states={};self.current_object=-1;self.raster_enabled=True;self.tick=0
  v.uc.hook_add(UC_HOOK_CODE,self.observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,self.observe_draw,begin=0x56eb30,end=0x56eb30);v.uc.hook_add(UC_HOOK_CODE,self.observe_state,begin=0x56d400,end=0x56d400)
  v.rng_seed=1;v.call(0x502b40,this=self.owner);assert v.u32(self.owner+0x184)==0 and v.u32(self.owner+0x80)==0;v.call(0x502c50,this=self.animator);native_owner(self)
 def state(self):
  v=self.vm;a=self.animator;floats=lambda p,n:list(struct.unpack('<'+str(n)+'f',v.uc.mem_read(a+p,n*4)))
  return dict(alive=not bool(v.u32(self.owner+8)&0x1000),phase=v.u32(self.owner+0x184),countdown=v.u32(self.owner+0x188),anchor=list(struct.unpack('<6i',v.uc.mem_read(a+0xfc,24))),angles_a=floats(0x114,20),angles_b=floats(0x164,20),unused=floats(0x1b4,20),lengths=floats(0x204,20),colors=list(v.uc.mem_read(a+0x254,60)),directions=list(struct.unpack('<60i',v.uc.mem_read(a+0x290,240))),radii=floats(0x380,25),radial_angles=floats(0x3e4,25),velocities=floats(0x448,25),scales=floats(0x4ac,25),status=list(struct.unpack('<25i',v.uc.mem_read(a+0x510,100))),core_scale=floats(0x574,1)[0],seed=v.rng_seed,rng_draws=self.rng_draws)

 def advance(self):
  self.tick+=1;v=self.vm
  if not self.state()['alive']:return
  v.call(0x470cc0,this=self.owner);v.call(0x502b70,this=self.owner)
  if not self.state()['alive']:return
  v.call(0x502da0,this=self.animator);v.put_u32(self.animator+0x14,struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0]);native_owner(self)
 def render(self):
  self.draws=[];self.software.clear()
  if self.state()['alive']:self.vm.call(0x503110,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')

def run(exe,archive,output,ticks=140,render_initial=False):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/energyspray.i3d'))
 f=EnergySprayScene(exe,data);rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  raw=f.render() if tick or render_initial else f.vm.surface_bytes('screen');p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,640,480)
  rows.append(dict(tick=tick,state=f.state(),draws=f.draws,image=str(p),image_sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))));print(tick,rows[-1]['state']['phase'],len(f.draws),rows[-1]['nonzero_pixels'],flush=True)
 # Independent fresh actual factory replay checks every state, packet and pixel.
 replay=EnergySprayScene(exe,data)
 for tick,row in enumerate(rows):
  if tick:replay.advance()
  raw=replay.render() if tick or render_initial else replay.vm.surface_bytes('screen');assert row['state']==replay.state() and row['draws']==replay.draws and row['rgb565_sha256']==sha(raw), f'Replay differs at {tick}'
 report=dict(warm_replays=2,retail_type_id='0xad92bc1c',asset_sha256=sha(data),dispatch=f.dispatch,cases=rows,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,seed=1,null_spell=True,render_initial=render_initial,accepted=False,scope='Actual complete registered factory, original leaf Init/Animate/Render, original owner NextFrame/Pulse/KillThisEffect, full RenderObject/native owner/matrix/FVF_VERTEX twoARGB passes and CopyVertices FVF_LVERTEX prelitRGBA5 triangles/software raster. Exact immutable V2 compressed keys, geometry/material/texture residency; base animator bookkeeping/extents and owner script/light/map-flag notification services are boundaries. No live spell, invoker, damage or natural actor-hand attachment.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
