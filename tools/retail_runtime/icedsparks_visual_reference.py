#!/usr/bin/env python3
"""Exact Icedsparks binding and full original default mesh, without Iced actor logic."""
import argparse,json,re,struct,zipfile
from pathlib import Path
from snow_visual_reference import NativeSnow,ASSET_SHA
from speed_controller_preflight import ROOT,read_asset
from firecone_state_probe import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
SAMPLES=(2,6,20,50,98,99,100,101,199,200)

class NativeIcedsparks(NativeSnow):
 def __init__(self,exe,data):
  super().__init__(exe,data,name='Icedsparks');v=self.vm
  v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1))
  v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator)
  v.write(self.owner+0x5c,struct.pack('<hh',0,1));vt=v.u32(self.imagery)
  for offset,value in ((0x8c,struct.unpack_from('<H',data,68)[0]),(0x90,100)):
   p=v.allocate(16);v.write_code(p,b'\xb8'+struct.pack('<I',value)+b'\xc2\x04\x00');v.put_u32(vt+offset,p)
  v.uc.hook_add(UC_HOOK_CODE,lambda *args:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  self.clock=0;self.lifecycle=[]
 def advance(self):
  v=self.vm;self.clock+=1;v.put_u32(0x65cb38,self.clock)
  v.call(0x470cc0,this=self.owner);v.call(0x4de800,this=self.owner)
  assert not v.u32(self.owner+8)&0x1000
  frame=struct.unpack('<h',v.uc.mem_read(self.owner+0x5c,2))[0];assert frame==self.clock%100
  v.call(0x40e2e0,this=self.animator);v.put_u32(self.animator+0x14,frame)
  self.lifecycle.append(dict(tick=self.clock,frame=frame,flags=hex(v.u32(self.owner+8))))

def run(output):
 output.mkdir(parents=True,exist_ok=False);exe=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  cls=z.read('class.def');matches=re.findall(r'^\s*"Icedsparks"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)',cls.decode('latin1'),re.M)
  assert matches==[('Magic\\Iced.i3d','0xacabfada')],matches
  data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/iced.i3d'))
 assert sha(data)==ASSET_SHA
 asset=read_asset(data);keys=[bytes.fromhex(o['states'][0]['legacy_key_hex'])for o in asset['objects']]
 assert len(keys)==50 and asset['state0_frames']==100
 for raw in keys:assert len(raw)==800 and all(raw[i*8:i*8+8]==raw[:8]for i in range(100))
 cases=[];expected=[]
 for replay in range(2):
  f=NativeIcedsparks(exe,data);assert f.constructor['factory']=='0x40dc00'and f.vm.u32(f.animator)==0x5a370c
  builder=f.vm.call(0x40dca0,(f.put_string('Icedsparks'),));assert builder==0x5e8508 and f.vm.u32(f.vm.u32(builder))==0x40dc00 and f.vm.u32(0x5e872c)==93
  for index,tick in enumerate(SAMPLES):
   while f.clock<tick:f.advance()
   raw,depth,record=f.render_whole(tick,f.lifecycle[-1]['frame']);signature=(sha(raw),sha(depth),sha(json.dumps(record,sort_keys=True).encode()))
   bounds=[d['projected_geometry_bounds']for d in record['draws']];assert len(bounds)==50 and all(0<b[0]<=b[2]<1279 and 0<b[1]<=b[3]<959 for b in bounds)
   if replay:assert signature==expected[index]
   else:
    expected.append(signature);p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960)
    cases.append(dict(tick=tick,owner_frame=tick%100,image=str(p.resolve()),image_sha256=sha(p.read_bytes()),rgb565_sha256=signature[0],depth_sha256=signature[1],draw_sha256=signature[2],draws=record['draws'],geometry_bounds=bounds,whole_geometry_unclipped=True))
   print(replay,tick,signature[0],flush=True)
 assert len(set(c['rgb565_sha256']for c in cases))==1
 report=dict(status='reference_complete',accepted=False,name='Icedsparks',type_id='0xacabfada',asset_sha256=ASSET_SHA,class_def_sha256=sha(cls),dispatch=f.dispatch,constructor=f.constructor,owner_lifecycle=f.lifecycle,cases=cases,independent_replays=2,constant_legacy_keys=5000,viewport=[1280,960],origin=[0,0,0],source_ambient=[38]*3,directional=[1]*3,original_executable_sha256=sha(exe.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),shared_fixture_sha256=sha(Path(__file__).with_name('snow_visual_reference.py').read_bytes()),scope='Exact CLASS EFFECT Icedsparks binding; original full93-constructor registry/default factory, empty-array Initialize/SetupObjects, owner/projector,50 roots,5000 literal constant8byte keys, full default Render/RenderObject/Illuminate and two exactARGB4444 software textures. Ten unchanged whole phases across99->0 twice, two fresh VM replays. Original NextFrame470cc0/effectPulse4de800/defaultAnimate execute through200ticks/two wraps; only base4708e0 script/audio is bounded. Decoded resource/COM residency/light/nullspell fixture; no victim/shatter or natural caller claim.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
