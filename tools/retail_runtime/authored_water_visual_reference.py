#!/usr/bin/env python3
"""Whole six-emitter original water draw, with observed unclipped FVF output.

Reuse the full particle resource/factory/parser/RenderObject fixture. No host
geometry, UV, color or matrices feed the original draw. Source-ground0 and
immutable RGB565 texture residency are declared API inputs.
"""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from static_particles_visual_reference import NativeParticleScene
from static_particles_probe import registry,punctuation
from speed_controller_preflight import ROOT
from software_probe import save_rgb565_png
from firecone_state_probe import sha

PROFILES=json.loads((ROOT/'tools/retail_runtime/authored_water_profiles.json').read_text())['profiles']
SAMPLES=(5,10,20,35,65,95)
class NativeWaterScene(NativeParticleScene):
 def __init__(self,exe,data,name,width,height):
  super().__init__(exe,data,name,width,height);self.draw_return=None
  def finished(uc,address,size,user):
   rows=list(struct.iter_unpack('<3f4I2f',self.vm.uc.mem_read(self.software.vertices,4*36)))
   self.draws[-1]['projected_geometry_bounds']=[min(r[0]for r in rows),-max(r[1]for r in rows),max(r[0]for r in rows),-min(r[1]for r in rows)]
  def entered(uc,address,size,user):
   if not self.raster_enabled:return
   ret=self.vm.u32(uc.reg_read(UC_X86_REG_ESP))
   if self.draw_return is None:self.draw_return=ret;self.vm.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
   elif ret!=self.draw_return:raise AssertionError('Original particle FVF caller changed')
  self.vm.uc.hook_add(UC_HOOK_CODE,entered,begin=0x56eb30,end=0x56eb30)
 def initialize_scene(self):
  self.dispatch=registry(self);punctuation(self);initialization=self.initialize(0)
  if len(initialization['controllers'])!=1 or initialization['controllers'][0]['vtable']!='0x5a3544' or initialization['controllers'][0]['emitter_count']!=6:raise AssertionError('Expected one complete six-emitter native PartSys')
  self.state_trace(0,frame_offset=1,move_tick=None)
  return dict(dispatch=self.dispatch,initialization=initialization)
 def step(self,tick,raster):
  v=self.vm;before=v.rng_seed;cached=v.u32(self.animator+0x14)
  v.call(0x403760,this=self.controller,instruction_limit=5000000)
  frame=(tick+1)%self.asset['state0_frames'];v.put_u32(self.animator+0x14,frame)
  for index,obj in enumerate(self.animobjs):
   if index==self.prototype_index:continue
   if v.call(0x40a420,(obj,0,frame,0,0),this=self.imagery)!=1:raise AssertionError('Original emitter matrix rejected')
  self.software.clear();self.draws=[];self.raster_enabled=raster
  v.call(0x404270,this=self.controller,instruction_limit=100000000)
  if self.software.locked or not(v.u32(self.animobjs[0])&1):raise AssertionError('Original surface/prototype lifecycle changed')
  return v.surface_bytes('screen'),v.surface_bytes('depth'),dict(capture=tick,owner_frame=frame,cached_frame=cached,rng_before=before,rng_after=v.rng_seed,draws=self.draws)

def run(executable,archive,output,width=1280,height=960):
 output.mkdir(parents=True,exist_ok=False);cases=[];initializations=[]
 with zipfile.ZipFile(archive)as z:
  names={n.lower():n for n in z.namelist()}
  for p in PROFILES:
   data=z.read(names['imagery/'+p['asset']]);assert sha(data)==p['sha256'];folder=output/p['name'];folder.mkdir()
   first=NativeWaterScene(executable,data,p['name'],width,height);second=NativeWaterScene(executable,data,p['name'],width,height);init=first.initialize_scene();assert init==second.initialize_scene();(folder/'initialization.json').write_text(json.dumps(init,indent=2)+'\n');cadence=[];drawlog=[]
   for tick in range(96):
    raw,depth,record=first.step(tick,tick in SAMPLES);replay=second.step(tick,tick in SAMPLES)
    if record['rng_before']!=replay[2]['rng_before'] or record['rng_after']!=replay[2]['rng_after'] or len(record['draws'])!=len(replay[2]['draws']):raise AssertionError('Independent native cadence replay differs')
    cadence.append({k:record[k]for k in ['capture','owner_frame','cached_frame','rng_before','rng_after']}|dict(draws=len(record['draws'])))
    if tick not in SAMPLES:continue
    if raw!=replay[0]or depth!=replay[1]:raise AssertionError('Independent whole native image/depth replay differs')
    if not record['draws']:raise AssertionError('Selected native frame is empty')
    bounds=[d['projected_geometry_bounds']for d in record['draws']];assert all(0<b[0]<b[2]<width-1 and 0<b[1]<b[3]<height-1 for b in bounds)
    image=folder/f'native-capture{tick:03d}.png';save_rgb565_png(image,raw,width,height);case=dict(name=p['name'],type_id=p['id'],capture=tick,owner_frame=record['owner_frame'],cached_frame=record['cached_frame'],image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=sha(raw),depth_sha256=sha(depth),draws=len(record['draws']),geometry_bounds=[min(b[0]for b in bounds),min(b[1]for b in bounds),max(b[2]for b in bounds),max(b[3]for b in bounds)],whole_geometry_unclipped=True,nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)));cases.append(case);drawlog.append(record);print(p['name'],tick,case['draws'],case['nonzero_pixels'],flush=True)
   (folder/'cadence.json').write_text(json.dumps(cadence,indent=2)+'\n');(folder/'draws.json').write_text(json.dumps(drawlog,indent=2)+'\n');initializations.append(dict(name=p['name'],manifest=str(folder/'initialization.json'),sha256=sha((folder/'initialization.json').read_bytes())))
 report=dict(status='reference_complete',accepted=False,profiles=PROFILES,cases=cases,viewport=[width,height],origin=[0,0,0],camera=[0,0,0],source_ambient=[38]*3,directional=[1]*3,seed=1,ground_height=0,quality=0,frames=96,samples=SAMPLES,independent_fresh_replays=2,original_executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),shared_fixture_sha256=sha(Path(__file__).with_name('static_particles_visual_reference.py').read_bytes()),initializations=initializations,scope='Actual complete original default factory/registration/parser/Initialize/six literal emitters/cached-frame Pulse/RenderSample/RenderObject/FVF_LVERTEX/software raster. Immutable decoded resource, owner GetAnimator, material COM, exact RGB565 texture residency, seed1 and ground0 boundaries. Every emitted transformed quad fits the fixed viewport; no host geometry/UV/color/matrix/raster replacement. Actual Metal/map and natural caller/device/depth acceptance separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe');p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--width',type=int,default=1280);p.add_argument('--height',type=int,default=960);a=p.parse_args();r=run(a.executable,a.archive,a.output,a.width,a.height);print(r['status'],len(r['cases']))
