#!/usr/bin/env python3
"""Original moving Fairy base and dust: full default controller/render caller."""
import argparse,copy,hashlib,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from blast_visual_reference import NativeBlast,ROOT
from scroll_water_visual_reference import NativeScrollScene
from speed_controller_preflight import NativeSpeed
from static_particles_probe import registry,punctuation
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png

class NativeFairy(NativeBlast):
 def __init__(self,exe,data):
  if hashlib.sha256(data).hexdigest()!='45ba6f8b7ace0f070fc1928a3af4c051d8b3af84a64a976d5383e8c72551eb8a':raise ValueError('Exact Fairy asset required')
  NativeScrollScene.__init__(self,exe,data,'Fairy',640,480,particle_required=True)
  v=self.vm;self.tick=0;self.state=0
  def boundary(uc,address,size,user):
   if address==0x58c582:self.ret(v.random_msvc(),0)
   else:self.ret(0,12)
  for address in (0x58c582,0x452e10):v.uc.hook_add(UC_HOOK_CODE,boundary,begin=address,end=address)
  v.rng_seed=1
  def observe_object(uc,address,size,user):self.current_object=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def observe_packet(uc,address,size,user):
   self.draws[-1].update(object=v.u32(self.current_object+4),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
  v.uc.hook_add(UC_HOOK_CODE,observe_object,begin=0x40a8f0,end=0x40a8f0)
  v.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x56eb30,end=0x56eb30)
 def setup_resources(self):
  NativeBlast.setup_resources(self);v=self.vm
  for o in self.asset['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4);states=o['states']
   v.put_u32(resource+0x20,self.bytes(struct.pack('<2i',*[s['parent']for s in states])))
   v.put_u32(resource+0x40,self.bytes(struct.pack('<2I',*[len(s['keys'])for s in states])))
   pointers=[self.bytes(struct.pack('<'+'I'*len(s['keys']),*s['keys']))for s in states]
   v.put_u32(resource+0x44,self.bytes(struct.pack('<2I',*pointers)))
  frames=v.u32(v.u32(self.imagery+0x28));v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<2I',frames,frames)))
  method=v.allocate(32);v.write_code(method,b'\x83\x7c\x24\x04\x01\x74\x08\xb8\xc8\x00\x00\x00\xc2\x04\x00\xb8\x01\x00\x00\x00\xc2\x04\x00');v.put_u32(v.u32(self.imagery)+0x90,method)
 def initialize_scene(self):
  dispatch=registry(self);punctuation(self);record=NativeSpeed.initialize(self,0)
  record['dispatch']=dispatch;assert len(record['controllers'])==2
  self.controllers=[self.vm.u32(self.vm.u32(self.animator+0x68)+i*4)for i in range(2)]
  return record
 def advance(self):
  v=self.vm;self.tick+=1;frame=self.tick%200 if self.state==0 else 0
  v.put_u32(0x65cb38,self.tick);v.write(self.owner+0x5c,struct.pack('<h',frame))
  # Actual default Animate invokes all controller Pulses before cached frame/
  # emitter poses are refreshed for rendering. No host emitter positions.
  v.call(0x40e2e0,this=self.animator,instruction_limit=5000000)
  v.put_u32(self.animator+0x14,frame)
 def render(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={}
  v.call(0x417d60,(1,1),this=0x65a57c);rng_before=v.rng_seed
  v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  return v.surface_bytes('screen'),dict(tick=self.tick,state=self.state,frame=v.u32(self.animator+0x14),owner=owner,draws=self.draws,rng_before=rng_before,rng_after=v.rng_seed,emitter_matrix=list(struct.unpack('<16f',v.uc.mem_read(self.animobjs[1]+0x58,64))))

def run(exe,archive,output,ticks=60):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fairy.i3d'))
 f=NativeFairy(exe,data);init=copy.deepcopy(f.initialize_scene());rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  raw,record=f.render()
  record.update(rgb565_sha256=hashlib.sha256(raw).hexdigest())
  if tick in(0,1,2,6,16,20,30,46,60,90,100,150,199,200,201):
   path=output/f'native-{tick:03d}.png';save_rgb565_png(path,raw,640,480);record.update(image=str(path),image_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',raw)));print(tick,len(record['draws']),record['nonzero_pixels'],flush=True)
  rows.append(record)
 (output/'manifest.json').write_text(json.dumps(dict(accepted=False,retail_type_id='0xad92bd25',viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,quality=0,seed=1,asset_sha256=hashlib.sha256(data).hexdigest(),original_executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),scope='Full default Animate40e2e0 and Render40e8d0, cached moving emitter, original particle pulse/render, normal-lit fairy plus prelit dust, exact RGB565 textures, native owner/projector/raster. Declared looping owner clock; no live actor or natural caller fabricated. Immutable decoded resources, texture COM residency, seed1 and flat ground0 are explicit boundaries.',initialization=init,cases=rows),indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=60);a=p.parse_args();run(a.executable,a.archive,a.output,a.ticks)
