#!/usr/bin/env python3
"""Literal Snow default factory/whole base Initialize and Render, independent of Iced."""
import argparse,json,struct,zipfile
from pathlib import Path
from generic_mesh_visual_reference import NativeGenericMeshScene
from speed_controller_preflight import ROOT
from firecone_owner_probe import native_owner
from firecone_state_probe import sha
from software_probe import save_rgb565_png
ASSET_SHA='0a2cdd9255322792ecb15f80bec3330a04aac363232c8adf5c40570949ab9ac8'
class NativeSnow(NativeGenericMeshScene):
 def setup_resources(self):
  # Preserve the actual empty factory array. Shared controller fixtures install
  # prototype slots before Initialize; Snow lets original SetupObjects allocate
  # every real draw object instead, avoiding duplicate pre-populated slots.
  original=bytes(self.vm.uc.mem_read(self.animator+0x30,40));super().setup_resources();self.vm.write(self.animator+0x30,original)
 def __init__(self,exe,data):
  assert sha(data)==ASSET_SHA;super().__init__(exe,data,'Snow',1280,960);v=self.vm
  flags=v.allocate(16);v.write_code(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x40,flags);v.put_u32(self.owner+0x4c,self.imagery);done=v.allocate(16);v.write_code(done,b'\x31\xc0\xc2\x04\x00');v.put_u32(v.u32(self.owner)+0x158,done)
  v.call(0x40dd60,this=self.animator,instruction_limit=100000000);assert v.u32(self.animator)==0x5a370c
 def render_whole(self,tick,frame):
  v=self.vm;v.put_u32(0x65cb38,tick);v.write(self.owner+0x5c,struct.pack('<h',frame));v.put_u32(self.animator+0x14,frame);owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};v.call(0x417d60,(0,1),this=0x65a57c)
  for key,value in[(7,1),(14,1),(22,2)]:v.call(0x56d400,(key,value))
  v.call(0x40e8d0,this=self.animator,instruction_limit=100000000);assert len(self.draws)==50;assert all(not s['locked']for s in self.surfaces.values());return v.surface_bytes('screen'),v.surface_bytes('depth'),dict(owner=owner,draws=self.draws,frame=frame)
def run(output):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/iced.i3d'))
 cases=[];previous=[]
 for replay in range(2):
  f=NativeSnow(exe,data);drawlog=[]
  for capture in(1,5,15,29,45,59):
   raw,depth,record=f.render_whole(capture+1,capture+1);pair=sha(raw),sha(depth),sha(json.dumps(record,sort_keys=True).encode());bounds=[d['projected_geometry_bounds']for d in record['draws']];assert all(0<b[0]<=b[2]<1279 and 0<b[1]<=b[3]<959 for b in bounds)
   if replay:assert pair==previous[(1,5,15,29,45,59).index(capture)]
   else:
    previous.append(pair);image=output/f'native-capture{capture:03d}.png';save_rgb565_png(image,raw,1280,960);drawlog.append(record);cases.append(dict(name='Snow',id='0xfacedbed',capture=capture,owner_frame=record['frame'],draws=len(record['draws']),image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=pair[0],depth_sha256=pair[1],draw_sha256=pair[2],geometry_bounds=bounds,whole_geometry_unclipped=True,nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))));print(capture,len(record['draws']),cases[-1]['nonzero_pixels'],flush=True)
  if not replay:(output/'draws.json').write_text(json.dumps(drawlog,indent=2)+'\n')
 report=dict(status='reference_complete',accepted=False,name='Snow',id='0xfacedbed',asset_sha256=ASSET_SHA,cases=cases,constructor=f.constructor,viewport=[1280,960],origin=[0,0,0],camera=[0,0,0],source_ambient=[38]*3,directional=[1]*3,independent_replays=2,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),scope='Literal Snow resolves default40dc00/5a370c, independently of Iced custom actor controller. Whole original empty-factory array/base Initialize/SetupObjects/controller refresh/owner/legacy8byte keys/full baseRender/RenderObject/FVF_VERTEX/two exactARGB4444 textures/software raster across all50 authored roots. Six fullunclipped native image/depth/draw sets agree across two fresh VM replays. All100keys perroot authored constant; no snowflake/shatter/actor animation claim. Readonly decoded resources/resident one-frame COM/nullspell command/material handles explicit. No production geometry/UV/colors/pose/raster injected; actualMetal/map/naturalcaller/device/exactpixel separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.output)['status'])
