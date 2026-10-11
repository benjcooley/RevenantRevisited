#!/usr/bin/env python3
"""Whole original Charm default controller/render caller and authored star/clover."""
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
class NativeCharm(NativeBlast):
 def __init__(self,exe,data):
  if hashlib.sha256(data).hexdigest()!='680c30d3d07cf4204fed4391c6b04fd2b3824f0472b49930b0b05bfff3e9b478':raise ValueError('Exact Charm required')
  NativeScrollScene.__init__(self,exe,data,'charm',640,480,particle_required=True);v=self.vm;self.tick=0;self.raster_enabled=True
  def boundary(uc,address,size,user):self.ret(v.random_msvc(),0)if address==0x58c582 else self.ret(0,12)
  for address in(0x58c582,0x452e10):v.uc.hook_add(UC_HOOK_CODE,boundary,begin=address,end=address)
  v.rng_seed=1
  def observe_object(uc,address,size,user):self.current_object=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def observe_packet(uc,address,size,user):
   self.draws[-1].update(object=v.u32(self.current_object+4),matrix=list(struct.unpack('<16f',uc.mem_read(self.current_object+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
   if not self.raster_enabled:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,observe_object,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,observe_packet,begin=0x56eb30,end=0x56eb30)
 def initialize_scene(self):
  dispatch=registry(self);punctuation(self);r=NativeSpeed.initialize(self,0);r['dispatch']=dispatch;v=self.vm;self.controllers=[v.u32(v.u32(self.animator+0x68)+i*4)for i in range(v.u32(self.animator+0x58))];return r
 def advance(self):
  v=self.vm;self.tick+=1;frame=self.tick%30;v.put_u32(0x65cb38,self.tick);v.write(self.owner+0x5c,struct.pack('<h',frame));v.call(0x40e2e0,this=self.animator,instruction_limit=5000000);v.put_u32(self.animator+0x14,frame)
 def render(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};v.call(0x417d60,(1,1),this=0x65a57c);before=v.rng_seed;v.call(0x40e8d0,this=self.animator,instruction_limit=100000000)
  return v.surface_bytes('screen'),dict(tick=self.tick,frame=v.u32(self.animator+0x14),owner=owner,draws=self.draws,rng_before=before,rng_after=v.rng_seed)
def run(exe,archive,output,ticks=121):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/charm.i3d'))
 f=NativeCharm(exe,data);init=copy.deepcopy(f.initialize_scene());rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  f.raster_enabled=tick in(0,1,2,6,16,29,30,31,60,90,119,120,121)
  raw,r=f.render();r['raster_executed']=f.raster_enabled;r['rgb565_sha256']=hashlib.sha256(raw).hexdigest()
  if f.raster_enabled:
   path=output/f'native-{tick:03d}.png';save_rgb565_png(path,raw,640,480);r.update(image=str(path),image_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',raw)));print(tick,len(r['draws']),r['nonzero_pixels'],flush=True)
  assert r['rng_before']==r['rng_after']
  rows.append(r)
 report=dict(accepted=False,retail_type_id='0xad92bd37',initialization=init,cases=rows,asset_sha256=hashlib.sha256(data).hexdigest(),original_executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,seed=1,quality=0,scope='Whole original default Animate/Render on every tick; original raster only selected frames, device draw boundary56eb30 otherwise. All draw callers, keys, cached poses and particle render samples execute; zero Render RNG consumption checked. Immutable decoded resource/texture COM residency, flatground0 and owner frame clock are explicit boundaries; no actor substitute.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=121);a=p.parse_args();run(a.executable,a.archive,a.output,a.ticks)
