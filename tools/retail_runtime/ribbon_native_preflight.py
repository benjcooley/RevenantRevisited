#!/usr/bin/env python3
"""Exact legacy base Ribbon full original animator and five real texture frames."""
import argparse,json,struct,zipfile
from pathlib import Path
from barrier_wall_native_preflight import NativeBarrierWall,ROOT
from firecone_owner_probe import native_owner
from speed_profile_contract import sha
from software_probe import save_rgb565_png
ASSET_SHA='5b0544d54b21b00a8aa2856fe9d8b4abeb3ff5b0f348ccb45a67c39b86d25acf'
def read_ribbon(data):
 assert sha(data)==ASSET_SHA;u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];rel=lambda p:p+u(p);b=20+u(16);assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),h(70))==(0,186,204,2,2,2,1);vertices=rel(rel(b+8));faces=rel(b+268);keybase=rel(b+6620);objects=[]
 for i in range(2):
  start=h(b+4060+i*2);count=h(b+4188+i*2);rawkeys=data[keybase+i*8:keybase+(i+1)*8];objects.append(dict(index=i,name=data[b+1884+i*32:b+1884+(i+1)*32].split(b'\0')[0].decode(),material=h(b+3932+i*2),vertex_start=start,vertex_count=count,vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex(),texture_runs=[v for t in range(3)for v in(h(b+4316+i*16+t*2),h(b+5340+i*16+t*2))],states=[dict(parent=-1,keys=[0],legacy_key_hex=rawkeys.hex())]))
 textures=[]
 for i in range(2):
  desc=b+920+i*108;hh,w=struct.unpack_from('<2I',data,desc+8);frames=u(b+1848+i*4);assert frames==(5 if i==0 else 1);pointer=rel(b+1784+i*4);raw=[data[rel(pointer+f*4):rel(pointer+f*4)+w*hh*2].hex()for f in range(frames)];pf=data[desc+72:desc+104];assert struct.unpack('<8I',pf)[3:]==(16,0xf00,0xf0,0xf,0xf000);textures.append(dict(width=w,height=hh,pixel_format=pf.hex(),raw_frames=raw))
 return dict(name='Ribbon',sha256=ASSET_SHA,body_offset=b,flags=0,version=0,vertices=186,faces=204,materials=2,textures=2,states=1,state0_frames=1,objects=objects,tags=[],face_hex=data[faces:faces+1224].hex(),vertex_hex=data[vertices:vertices+5952].hex(),material_hex=[data[b+276+i*80:b+276+(i+1)*80].hex()for i in range(2)],texture_data=textures)
class NativeRibbon(NativeBarrierWall):
 def __init__(self,exe,data):
  super().__init__(exe,data,('Ribbon','0x562c7439',0x4f5c30,0x66ce18,ASSET_SHA),read_ribbon(data),0x5a9d04,0x4e46f0)
 def advance(self):self.tick+=1;self.vm.put_u32(0x65cb38,self.tick);self.vm.call(0x4e4860,this=self.animator,instruction_limit=10000000)
 def render(self):
  owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};before=self.vm.rng_seed;self.vm.call(0x4e4a70,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen'),dict(tick=self.tick,rng_before=before,rng_after=self.vm.rng_seed,owner=owner,draws=self.draws,texture_frames=[self.vm.u32(self.vm.u32(self.vm.u32(self.imagery+0x5c)+i*4)+0x80)for i in range(2)])
def run(output,ticks,animate_first=False):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/ribbon.i3d'))
 f=NativeRibbon(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);rows=[]
 for tick in range(1 if animate_first else 0,ticks+1):
  if tick:f.advance()
  f.raster_enabled=tick in(0,1,2,6,16,20,30,60,90);raw,r=f.render();r['raster_executed']=f.raster_enabled
  if f.raster_enabled:p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960);r.update(image=str(p),image_sha256=sha(p.read_bytes()),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(tick,len(r['draws']),r['nonzero_pixels'],r['texture_frames'],flush=True)
  rows.append(r)
 report=dict(status='native_preflight_complete',accepted=False,first_render_tick=1 if animate_first else 0,name='Ribbon',id='0x562c7439',asset_sha256=ASSET_SHA,constructor=f.constructor,cases=rows,scope='Exact legacy base Ribbon full constructor/customInitialize/Pulse/RenderObject and all5actual authored texture0 frames through original40c520selector, singletexture1. Null original target, immutable decoded resource/COM/owner flag/command/seed1 and authored-key cache40a420 explicit bounds; no fake actor/pose/geometry/UV/color/texture or raster. Selected full raster only, originalRender/RNG allintervening ticks. ActualMetal/source/map/device separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=30);p.add_argument('--animate-first',action='store_true');a=p.parse_args();run(a.output,a.ticks,a.animate_first)
