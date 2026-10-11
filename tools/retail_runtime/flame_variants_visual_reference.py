#!/usr/bin/env python3
"""Exact FlameB/G CLASS bindings through full original custom animator/render."""
import argparse,json,struct,zipfile
from pathlib import Path
from scroll_water_visual_reference import NativeScrollScene
from static_particles_probe import punctuation
from firecone_owner_probe import native_owner
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
ROOT=Path(__file__).resolve().parents[2]
PROFILES=(('FlameB','0x50ba373c','1e94317594421341b8b3d9c9b41be01ce724b63c9702deb0fcd6bd2773241718'),('FlameG','0x50ba373d','a60243384bcfb5700e543d15c543cfcc25cca3d34baf4d37811cef386254a778'))
class NativeFlameVariant(NativeScrollScene):
 def __init__(self,exe,data):
  super().__init__(exe,data,'Flame',640,480,particle_required=False,native_binding=(0x4f5d70,0x66ccc0,0x5a9dec));self.tick=0
  v=self.vm;flags=v.allocate(16);v.write_code(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x40,flags);v.put_u32(self.owner+0x4c,self.imagery);done=v.allocate(16);v.write_code(done,b'\x31\xc0\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x158,done)
  def observe(uc,a,n,u):self.current_object=self.vm.u32(uc.reg_read(UC_X86_REG_ESP)+4)
  def packet(uc,a,n,u):self.draws[-1].update(matrix=list(struct.unpack('<16f',uc.mem_read(self.current_object+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))),uvs=[list(struct.unpack('<2f',uc.mem_read(self.draws[-1]['args'][2]+i*32+24,8)))for i in range(4)])
  self.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=0x40a8f0,end=0x40a8f0);self.vm.uc.hook_add(UC_HOOK_CODE,packet,begin=0x56eb30,end=0x56eb30)
 def setup_resources(self):
  original=bytes(self.vm.uc.mem_read(self.animator+0x30,40));super().setup_resources();self.vm.write(self.animator+0x30,original)
 def initialize_scene(self):
  punctuation(self);self.vm.call(0x401610);self.vm.call(0x4e4ec0,this=self.animator,instruction_limit=10000000);self.animobjs=[self.vm.call(0x40eef0,(0,),this=self.animator)];return dict(constructor=self.constructor,dispatch=self.dispatch,initialize='0x4e4ec0',animate='0x4e4ef0',render='0x4e4f20',initial_internal_frame=self.vm.u32(self.animator+0xfc),flags=[hex(self.vm.u32(o))for o in self.animobjs])
 def advance(self):self.tick+=1;self.vm.put_u32(0x65cb38,self.tick);self.vm.call(0x4e4ef0,this=self.animator,instruction_limit=10000000)
 def render(self):
  self.software.clear();self.draws=[];self.render_states={};owner=native_owner(self);self.vm.call(0x4e4f20,this=self.animator,instruction_limit=100000000);vertices=list(struct.iter_unpack('<3f4I2f',self.vm.uc.mem_read(self.software.vertices,4*36)));bounds=[min(x[0]for x in vertices),-max(x[1]for x in vertices),max(x[0]for x in vertices),-min(x[1]for x in vertices)];assert 0<bounds[0]<bounds[2]<640 and 0<bounds[1]<bounds[3]<480;return self.vm.surface_bytes('screen'),dict(tick=self.tick,internal_frame=self.vm.u32(self.animator+0xfc),owner=owner,draws=self.draws,geometry_bounds=bounds)
def run(output,ticks=36):
 output.mkdir(parents=True,exist_ok=False);cases=[];exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  classdef=z.read('class.def').decode('latin1')
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name.lower()+'.i3d'));assert sha(data)==digest;assert any('"Flame"'in l and id in l and name+'.I3D'in l for l in classdef.splitlines());f=NativeFlameVariant(exe,data);init=f.initialize_scene();folder=output/name;folder.mkdir();rows=[]
   for tick in range(ticks+1):
    if tick:f.advance()
    raw,r=f.render();r.update(rgb565_sha256=sha(raw),nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)));p=folder/f'native-{tick:03d}.png';save_rgb565_png(p,raw,640,480);r.update(image=str(p),image_sha256=sha(p.read_bytes()));rows.append(r)
   cases.append(dict(name=name,id=id,asset_sha256=digest,initialization=init,rows=rows));print(name,'complete',[(r['tick'],r['internal_frame'],r['nonzero_pixels'],len(r['draws']))for r in rows[:3]],flush=True)
 report=dict(status='reference_complete',accepted=False,viewport=[640,480],owner=[0,0,0],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,quality=0,original_executable_sha256=sha(exe.read_bytes()),classdef_sha256=sha(classdef.encode('latin1')),probe_sha256=sha(Path(__file__).read_bytes()),cases=cases,scope='Exact two literal CLASS EFFECT Flame records with own Blue/Green imagery and IDs; complete all93 original constructors/custom4f5d70 factory/4e4ec0Initialize/4e4ef0Animate/4e4f20Render/RenderObject/vertex transform/ARGB4444 software pixel kernel. Original empty factory object-array and full SetupObjects/CopyVertices are retained; owner flag/command accessors, decoded immutable resource and textureCOM residency are explicit boundaries. Prelit whites are original CopyVertices output, not host inputs. Native scene input ambient38/dir1; no injected production poses/UVs/vertexcolors/raster. Actual Metal and map acceptance separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=36);a=p.parse_args();run(a.output,a.ticks)
