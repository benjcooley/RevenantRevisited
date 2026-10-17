#!/usr/bin/env python3
"""Whole custom Waterfall/Water factory, pool, FVF normals and native raster.

Immutable decoded resources and resident COM textures come from the shared
generic fixture. Original constructors, initialization, animation, matrices,
lighting and complete Render execute; no source pool or geometry is injected.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from generic_mesh_visual_reference import NativeGenericMeshScene
from speed_controller_preflight import ROOT
from fountain_probe import FountainFixture
from firecone_owner_probe import native_owner
from firecone_state_probe import sha
from software_probe import save_rgb565_png

BINDINGS={'Waterfall':(0x4f9a40,0x66cc10,0x5ad26c,100),'Water':(0x4f9c30,0x66ce98,0x5ad4e4,25)}
CAPTURES=(1,5,15,29,45,59)
class NativeLiteralWater(NativeGenericMeshScene):
 observe_random=FountainFixture.observe_random
 def __init__(self,exe,data,name,width=1280,height=960):
  factory,builder,vt,self.pool_size=BINDINGS[name]
  super().__init__(exe,data,name,width,height,(factory,builder,vt),False)
  v=self.vm;self.random=[];self.random_pending=[];self.crt_draws=0;v.rng_seed=1
  def rand(uc,address,size,user):self.crt_draws+=1;self.ret(v.random_msvc(),0)
  v.uc.hook_add(UC_HOOK_CODE,rand,begin=0x58c582,end=0x58c582)
  for address in (0x483300,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
  # Declared nullspell owner methods: flags, animator identity and command
  # liveness. Resource link is the exact fixture imagery, never a substitute.
  flags=v.allocate(16);v.write_code(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x40,flags)
  v.put_u32(self.owner+0x4c,self.imagery)
  done=v.allocate(16);v.write_code(done,b'\x31\xc0\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x158,done)
  self.vtable=vt;v.call(v.u32(vt+24),this=self.animator,instruction_limit=100000000)
  assert v.u32(self.animator+0x100)==self.pool_size
 def state(self):
  v=self.vm;pointer=v.u32(self.animator+0xfc)
  return dict(rng_seed=v.rng_seed,crt_draws=self.crt_draws,random_count=len(self.random),particles=[list(struct.unpack('<9fi',v.uc.mem_read(pointer+i*40,40)))for i in range(self.pool_size)])
 def animate(self,tick):
  self.vm.put_u32(0x65cb38,tick);self.vm.call(self.vm.u32(self.vtable+44),this=self.animator,instruction_limit=100000000)
 def render_whole(self):
  owner=native_owner(self);before=self.state();self.software.clear();self.draws=[];self.render_states={};self.vm.call(self.vm.u32(self.vtable+52),this=self.animator,instruction_limit=100000000)
  assert before==self.state(),'Render mutated pool/RNG'
  assert all(not s['locked']for s in self.surfaces.values())
  return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth'),dict(owner=owner,draws=self.draws,state=before)

def run(executable,archive,output,names=('Waterfall','Water')):
 output.mkdir(parents=True,exist_ok=False);profiles=[];cases=[];states=[];width,height=1280,960
 rows=json.loads((ROOT/'docs/vfx/EFFECT_BURNDOWN.json').read_text())['retail_effects']
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()}
  for name in names:
   row=next(r for r in rows if r['retail_name']==name);path=row['asset'].replace('\\','/');data=z.read(ns['imagery/'+path.lower()]);assert sha(data)=='ece24f00eab927662fea2027dde203afec2525471eb6cde7b2a06f69c9b9aa75'
   profile=dict(name=name,id=row['retail_type_id'],asset=path,asset_sha256=sha(data),binding=[hex(x)for x in BINDINGS[name][:3]],pool=BINDINGS[name][3]);profiles.append(profile);folder=output/name;folder.mkdir();previous=[]
   for replay in range(2):
    fixture=NativeLiteralWater(executable,data,name,width,height);trace=[];drawlog=[]
    for tick in range(61):
     if tick:fixture.animate(tick)
     trace.append(dict(tick=tick,**fixture.state()))
     if tick not in CAPTURES:continue
     raw,depth,record=fixture.render_whole();bounds=[d['projected_geometry_bounds']for d in record['draws']];assert bounds and all(0<b[0]<=b[2]<width-1 and 0<b[1]<=b[3]<height-1 for b in bounds),'Authored geometry clipped'
     pair=(sha(raw),sha(depth),sha(json.dumps(record,sort_keys=True).encode()))
     if replay:assert pair==previous[CAPTURES.index(tick)],'Independent fresh native replay differed'
     else:
      previous.append(pair);image=folder/f'native-capture{tick:03d}.png';save_rgb565_png(image,raw,width,height);drawlog.append(record);case=dict(name=name,type_id=profile['id'],capture=tick,tick=tick,draws=len(record['draws']),image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=pair[0],depth_sha256=pair[1],draw_sha256=pair[2],geometry_bounds=bounds,whole_geometry_unclipped=True,nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)));cases.append(case);print(name,tick,case['draws'],case['nonzero_pixels'],flush=True)
    encoded=json.dumps(trace,sort_keys=True).encode()
    if replay:assert sha(encoded)==state_sha,'Independent full pool/RNG trace differed'
    else:
     state_sha=sha(encoded);states.append(dict(name=name,state_sha256=state_sha,initialization=fixture.constructor,texture_contract=fixture.texture_contract));(folder/'state-trace.json').write_text(json.dumps(trace,indent=2)+'\n');(folder/'draws.json').write_text(json.dumps(drawlog,indent=2)+'\n');(folder/'rng-inputs.txt').write_text(''.join(f'{a} {b} {v}\n'for a,b,v in fixture.random))
 report=dict(status='reference_complete',accepted=False,profiles=profiles,cases=cases,states=states,viewport=[width,height],origin=[0,0,0],camera=[0,0,0],source_ambient=[38]*3,directional=[1]*3,seed=1,independent_replays=2,original_executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Exact full custom factory/all constructors/base SetupObjects/Initialize/warmup/Animate/pool/RNG/full Render/owner matrix/FVF_VERTEX/normal lighting/RGB565 additive software raster. Two fresh VM replays agree on all61 complete pool/RNG states and six whole unclipped image/depth/draw sets. Readonly resource/one-frame COM residency/nullspell owner command interface and null volume explicit; full archive loader/material handle creation/natural caller/device/exactpixel acceptance separate. No geometry/pose/color/UV fitting or production inputs in native.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--name',action='append',choices=list(BINDINGS));p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',a.output,a.name or tuple(BINDINGS));print(r['status'],len(r['cases']))
