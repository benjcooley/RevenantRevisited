#!/usr/bin/env python3
"""Exact oldbody Fire default renderer; bounded blank reference, no acceptance."""
import argparse,sys,struct,zipfile,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools/retail_runtime'))
from vortex_native_preflight import NativeVortex
from barrier_wall_native_preflight import NativeBarrierWall
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
from speed_profile_contract import sha
from unicorn import UC_HOOK_CODE
parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True);out=parser.parse_args().output;out.mkdir(parents=True,exist_ok=False)
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:d=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fire.i3d'))
assert sha(d)=='70574ac537f9ec6f6a39df1adb449e969afa15691a9190c5b4450114d0ae3fee'
u=lambda p:struct.unpack_from('<I',d,p)[0];h=lambda p:struct.unpack_from('<H',d,p)[0];r=lambda p:p+u(p);b=20+u(16);vp=r(r(b+8));fp=r(b+268);kp=r(b+6620);desc=b+920;hh,w=struct.unpack_from('<2I',d,desc+8);frames=[d[r(r(b+1784)+i*4):r(r(b+1784)+i*4)+w*hh*2].hex()for i in range(u(b+1848))];o=dict(index=0,name='box01',material=h(b+3932),vertex_start=h(b+4060),vertex_count=h(b+4188),vertex_hex=d[vp:vp+128].hex(),texture_runs=[v for i in range(2)for v in(h(b+4316+i*2),h(b+5340+i*2))],states=[dict(parent=-1,keys=[0],legacy_key_hex=d[kp:kp+8].hex())]);a=dict(name='Fire',sha256=sha(d),body_offset=b,flags=0,version=0,vertices=4,faces=2,materials=1,textures=1,states=1,state0_frames=1,objects=[o],tags=[],face_hex=d[fp:fp+12].hex(),vertex_hex=d[vp:vp+128].hex(),material_hex=[d[b+276:b+356].hex()],texture_data=[dict(width=w,height=hh,pixel_format=d[desc+72:desc+104].hex(),raw_frames=frames)])
class Fire(NativeVortex):
 def __init__(self):
  NativeBarrierWall.__init__(self,ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',d,('Fire','0x119f01e3',0x40dc00,0x5e8508,sha(d)),a,0x5a370c,0x40dd60);v=self.vm;v.put_u32(self.owner,0x5a85ac);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1));fn=self.bytes(b'\xb8\x01\x20\0\0\xc2\x04\0');v.put_u32(v.u32(self.imagery)+0x8c,fn);v.uc.hook_add(UC_HOOK_CODE,lambda *_:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
f=Fire();v=f.vm;rows=[]
for t in range(67):
 if t:v.call(0x4de800,this=f.owner);v.call(0x470cc0,this=f.owner);v.call(0x40e2e0,this=f.animator)
 v.put_u32(0x65cb38,t);f.raster_enabled=t in(0,1,2,16,32,33,66);native_owner(f);f.software.clear();f.draws=[];f.render_states={};v.call(0x417d60,(0,1),this=0x65a57c)
 for key,value in((7,1),(14,1),(22,2)):v.call(0x56d400,(key,value))
 v.call(0x40e8d0,this=f.animator,instruction_limit=100000000);row=dict(tick=t,owner_frame=struct.unpack('<h',v.uc.mem_read(f.owner+0x5c,2))[0],animator_frame=v.u32(f.animator+0x14),draws=len(f.draws),textureframe=v.u32(v.u32(v.u32(f.imagery+0x5c))+0x80))
 if f.raster_enabled:
  raw=v.surface_bytes('screen');p=out/f'native-{t:03}.png';save_rgb565_png(p,raw,1280,960);row.update(pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(p),sha256=sha(p.read_bytes()),draws=f.draws);print(row,flush=True)
 rows.append(row)
(out/'manifest.json').write_text(json.dumps(dict(status='default_reference_blank_deferred',accepted=False,visual_pass=False,name='Fire',type_id='0x119f01e3',asset_sha256=sha(d),executable_sha256=sha((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()),constructor=f.constructor,cases=rows,scope='Exact oldbody33ARGB4444textures/allfourvertices/twofaces/singlekey; whole original defaultfactory/Initialize/ownerPulse/NextFrame/Animate/Render through66ticks. Declared origin/camera0,ambient38dir1,Scene0/cull2/depthwrite. Every actual owner frame and texture selection remains0; seven sampled software images blank. No forced atlas timing/scale/cull change and no source/runtime/appearance acceptance. Immutable decoded resource/COM surfaces, base owner script and declared light/camera are boundaries.'),indent=2)+'\n')
