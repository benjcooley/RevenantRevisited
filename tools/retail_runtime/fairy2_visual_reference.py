#!/usr/bin/env python3
"""Exact Fairy2 default factory, original399-frame emitter and full PartSys render."""
import argparse,copy,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from fairy_visual_reference import NativeFairy
from blast_visual_reference import NativeBlast
from scroll_water_visual_reference import NativeScrollScene
from speed_controller_preflight import NativeSpeed,ROOT
from static_particles_probe import registry,punctuation
from firecone_state_probe import sha
from software_probe import save_rgb565_png
ASSET_SHA='4dff6217434f50c1026116b9af4f3cc2b9a61547cd0e7d179efbe0dcea3841b5'
class NativeFairy2(NativeFairy):
 def __init__(self,exe,data,width=1280,height=960):
  assert sha(data)==ASSET_SHA;NativeScrollScene.__init__(self,exe,data,'Fairy2',width,height,particle_required=True);v=self.vm;self.tick=0;self.state=0;self.raster_enabled=True
  def boundary(uc,address,size,user):self.ret(v.random_msvc(),0)if address==0x58c582 else self.ret(0,12)
  for address in(0x58c582,0x452e10):v.uc.hook_add(UC_HOOK_CODE,boundary,begin=address,end=address)
  v.rng_seed=1
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery)
  for offset,value in((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,self.asset['state0_frames'])):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
  v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  def obj(uc,address,size,user):self.current_object=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def packet(uc,address,size,user):
   self.draws[-1].update(object=v.u32(self.current_object+4),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,obj,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,packet,begin=0x56eb30,end=0x56eb30)
  self.return_address=None
  def finished(uc,address,size,user):rows=list(struct.iter_unpack('<3f4I2f',v.uc.mem_read(self.software.vertices,self.draws[-1]['args'][3]*36)));self.draws[-1]['geometry_bounds']=[min(r[0]for r in rows),-max(r[1]for r in rows),max(r[0]for r in rows),-min(r[1]for r in rows)]
  def entered(uc,address,size,user):
   if not self.raster_enabled:return
   ret=v.u32(uc.reg_read(UC_X86_REG_ESP))
   if self.return_address is None:self.return_address=ret;v.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
   else:assert self.return_address==ret
  v.uc.hook_add(UC_HOOK_CODE,entered,begin=0x56eb30,end=0x56eb30)
 def setup_resources(self):
  NativeBlast.setup_resources(self);v=self.vm
  assert self.asset['states']==2 and self.asset['state0_frames']==399
  for o in self.asset['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4);tracks=o['states'];v.put_u32(resource+0x20,self.bytes(struct.pack('<2i',*(t['parent']for t in tracks))));v.put_u32(resource+0x40,self.bytes(struct.pack('<2I',*(len(t['keys'])for t in tracks))));pointers=[self.bytes(struct.pack('<'+'I'*len(t['keys']),*t['keys']))for t in tracks];v.put_u32(resource+0x44,self.bytes(struct.pack('<2I',*pointers)))
  frames=v.u32(v.u32(self.imagery+0x28));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<2I',frames,frames)));method=v.allocate(32);v.write_code(method,b'\x83\x7c\x24\x04\x01\x74\x08\xb8'+struct.pack('<I',399)+b'\xc2\x04\x00\xb8\x01\x00\x00\x00\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x90,method)
 def initialize_scene(self):
  dispatch=registry(self);punctuation(self);record=NativeSpeed.initialize(self,0);record['dispatch']=dispatch;assert dispatch['actual_builder']=='0x5e8508'and dispatch['actual_factory']=='0x40dc00';self.controllers=[self.vm.u32(self.vm.u32(self.animator+0x68)+i*4)for i in range(2)];assert len(record['controllers'])==2;return record
 def advance(self):
  v=self.vm;self.tick+=1;v.put_u32(0x65cb38,self.tick)
  if v.u32(self.owner+8)&0x1000:return
  v.call(0x470cc0,this=self.owner);v.call(0x4de800,this=self.owner)
  if v.u32(self.owner+8)&0x1000:return
  frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0];v.call(0x40e2e0,this=self.animator,instruction_limit=10000000);v.put_u32(self.animator+0x14,frame)
 def render(self):
  from firecone_owner_probe import native_owner
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};v.call(0x417d60,(1,1),this=0x65a57c);before=v.rng_seed
  if not v.u32(self.owner+8)&0x1000:v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  return v.surface_bytes('screen'),dict(tick=self.tick,state=self.state,frame=v.u32(self.animator+0x14),owner_flags=hex(v.u32(self.owner+8)),owner=owner,draws=self.draws,rng_before=before,rng_after=v.rng_seed,emitter_matrix=list(struct.unpack('<16f',v.uc.mem_read(self.animobjs[1]+0x58,64))))

def run(output,ticks=60):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fairy2.i3d'))
 f=NativeFairy2(exe,data);init=copy.deepcopy(f.initialize_scene());cases=[];rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  f.raster_enabled=tick in(0,1,2,6,30,60,120,200,300,398,399,400)
  raw,r=f.render();r.update(raster_executed=f.raster_enabled,rgb565_sha256=sha(raw));rows.append(r)
  if tick in(0,1,2,6,30,60,120,200,300,398,399,400):
   p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960);r.update(image=str(p),image_sha256=sha(p.read_bytes()),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));cases.append(r);print(tick,len(r['draws']),r['nonzero_pixels'],flush=True)
 report=dict(status='reference_complete',accepted=False,name='Fairy2',retail_type_id='0xad92bd26',viewport=[1280,960],origin=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,quality=0,seed=1,asset_sha256=ASSET_SHA,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),initialization=init,cases=cases,scope='Exact own Fairy2 asset/defaultfullregistry/factory/controller parser/Initialize/wholeAnimate/Render, original cached399frame finite emitter/variablepps/jitter/relvel/gravity/colors/normal-lit RGB565base+prelitdust and native raster. Immutable decoded resources/oneframeCOM/seed1/flatground0 areexplicit APIbounds; no productiongeometry/pose/color/raster injected. Ownasset andbinding independently verified, no Fairy alias equivalence. Original NextFrame470cc0 and effectPulse4de800 execute; script/tag-audio4708e0 is bounded. Original raster only selected declared phases; original Render/RNG executes through56eb30device boundary otherwise. ActualMetal/map/naturalcaller/device/exactpixel separate.');(output/'draws.json').write_text(json.dumps(rows)+'\n');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=60);a=p.parse_args();run(a.output,a.ticks)
