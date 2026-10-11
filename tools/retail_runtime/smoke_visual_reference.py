#!/usr/bin/env python3
"""Exact named Smoke custom animator, old resource and declared quality3 renderer."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from vortex_native_preflight import NativeVortex
from barrier_wall_native_preflight import NativeBarrierWall,ROOT
from speed_profile_contract import sha
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
ASSET_SHA='5029b94d06420c8beacc253d88f75fbf8d167306a7950be3d6a37330b2bb72e9'
def read_smoke(data):
 assert sha(data)==ASSET_SHA;u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];rel=lambda p:p+u(p);b=20+u(16)
 assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),u(24))==(0,4,2,1,1,1,3)
 vertices=rel(rel(b+8));faces=rel(b+268);tracks=[]
 for state in range(3):
  count=h(70+76*state);keys=rel(b+6620+state*4);raw=data[keys:keys+count*8];assert count==(8 if state==2 else 1) and all(raw[i*8:i*8+8]==raw[:8]for i in range(count))
  tracks.append(dict(parent=-1,keys=[0]*count,legacy_key_hex=raw.hex()))
 o=dict(index=0,name=data[b+1884:b+1916].split(b'\0')[0].decode(),material=h(b+3932),vertex_start=h(b+4060),vertex_count=h(b+4188),vertex_hex=data[vertices:vertices+128].hex(),texture_runs=[v for t in range(2)for v in(h(b+4316+t*2),h(b+5340+t*2))],states=tracks);assert o['name']=='photon'
 desc=b+920;height,width=struct.unpack_from('<2I',data,desc+8);assert u(b+1848)==1;pixel=rel(rel(b+1784));pf=data[desc+72:desc+104];assert struct.unpack('<8I',pf)[3:]==(16,0xf00,0xf0,0xf,0xf000)
 return dict(name='Smoke',sha256=ASSET_SHA,body_offset=b,flags=0,version=0,vertices=4,faces=2,materials=1,textures=1,states=3,state0_frames=1,objects=[o],tags=[],face_hex=data[faces:faces+12].hex(),vertex_hex=data[vertices:vertices+128].hex(),material_hex=[data[b+276:b+356].hex()],texture_data=[dict(width=width,height=height,pixel_format=pf.hex(),raw=data[pixel:pixel+width*height*2].hex())])
class NativeSmoke(NativeVortex):
 def setup_resources(self):
  super().setup_resources();v=self.vm;v.put_u32(0x5d79e4,self.quality_input)
  resource=v.u32(v.u32(self.imagery+0x74));tracks=self.asset['objects'][0]['states'];v.put_u32(resource+0x40,self.bytes(struct.pack('<3I',1,1,8)));v.put_u32(resource+0x44,self.bytes(struct.pack('<3I',*[self.bytes(bytes.fromhex(t['legacy_key_hex']))for t in tracks])))
  method=self.bytes(bytes.fromhex('8b44240483f8027508b808000000c20400b801000000c20400'));v.put_u32(v.u32(self.imagery)+0x90,method)
 def __init__(self,exe,data,quality=3):
  self.quality_input=quality;NativeBarrierWall.__init__(self,exe,data,('Smoke','0x224a5ccc',0x4f65d0,0x66ce10,ASSET_SHA),read_smoke(data),0x5aa4e4,0x4e7d80)
  v=self.vm;old=v.u32(self.owner);vt=v.allocate(0x200);v.write(vt,bytes(v.uc.mem_read(0x5a85ac,0x184)));v.put_u32(vt+0x40,v.u32(old+0x40));v.put_u32(self.owner,vt);v.write(self.owner+0x7c,struct.pack('<h',-1));v.put_u32(self.owner+0x54,self.imagery);v.put_u32(self.owner+0x58,self.animator);v.write(self.owner+0x5c,struct.pack('<hh',0,1))
  fn=self.bytes(b'\xb8\x00\x20\0\0\xc2\x04\0');v.put_u32(v.u32(self.imagery)+0x8c,fn)
  v.uc.hook_add(UC_HOOK_CODE,lambda *_:self.ret(0,0),begin=0x4708e0,end=0x4708e0)
  self.packets=[]
  def packet(uc,*_):
   a=self.draws[-1]['args'];self.packets.append(dict(arguments=a,vertices_hex=bytes(v.uc.mem_read(a[2],a[3]*32)).hex(),matrix=list(struct.unpack('<16f',v.uc.mem_read(0x675ed0,64)))))
  v.uc.hook_add(UC_HOOK_CODE,packet,begin=0x56eb30,end=0x56eb30)
 def state(self):
  v=self.vm;a=self.animator
  return dict(tick=v.u32(a+0xfc),alive=not bool(v.u32(self.owner+8)&0x1000),owner_flags=hex(v.u32(self.owner+8)),center=list(struct.unpack('<4f',v.uc.mem_read(a+0x100,16))),slots=[list(struct.unpack('<9fi',v.uc.mem_read(a+0x110+i*40,40)))for i in range(50)],seed=v.rng_seed)
 def advance(self):
  self.tick+=1;v=self.vm
  if not self.state()['alive']:return
  v.call(0x4de800,this=self.owner)
  if not self.state()['alive']:return
  v.call(0x470cc0,this=self.owner);v.call(0x4e7e50,this=self.animator);v.put_u32(self.animator+0x14,0)
 def render(self):
  native_owner(self);self.software.clear();self.draws=[];self.packets=[];self.render_states={}
  if self.state()['alive']:self.vm.call(0x4e7fa0,this=self.animator,instruction_limit=100000000)
  return self.vm.surface_bytes('screen')
def run(output,ticks):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/smoke.i3d'))
 f=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);rows=[]
 for tick in range(ticks+1):
  if tick:f.advance()
  f.raster_enabled=tick in(0,2,20,80,140,199,200,250,300,500,600);before=f.state();raw=f.render();assert before==f.state();r=dict(tick=tick,state=before,draw_count=len(f.draws))
  if f.raster_enabled:
   p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960);r.update(image=str(p),image_sha256=sha(p.read_bytes()),pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),draws=f.draws,packets=f.packets);print(tick,r['pixels'],len(f.draws),flush=True)
  rows.append(r)
 low=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,quality=1);assert not low.state()['alive'];assert not any(low.render())
 report=dict(status='native_preflight',accepted=False,name='Smoke',type_id='0x224a5ccc',asset_sha256=ASSET_SHA,quality=3,low_quality_kills=True,constructor=f.constructor,cases=rows,scope='Exact capitalized shipped custom Smoke factory/Init/Animate/Render plus default effect owner NextFrame/Pulse; quality3 visible, quality1 original immediate kill. Own old-body fourvertices/twofaces/singleARGB4444 material/texture/state0key. Decoded resource/COM, base bookkeeping/script/flag notification/quality/light interfaces; no actor or device caller acceptance.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=300);a=p.parse_args();run(a.output,a.ticks)
