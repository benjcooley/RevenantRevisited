#!/usr/bin/env python3
"""Literal Vortex legacy mesh and executed original default factory/render."""
import argparse,json,struct,zipfile
from pathlib import Path
from barrier_wall_native_preflight import NativeBarrierWall,ROOT
from speed_controller_preflight import NativeSpeed
from speed_profile_contract import sha
from firecone_owner_probe import native_owner
from software_probe import save_rgb565_png
ASSET_SHA='e11ceb080fa6aa0b3066d71f4b5439d897ef2aa38309a2b6d6c4139411cd4f4a'
def read_vortex(data):
 assert sha(data)==ASSET_SHA;u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];rel=lambda p:p+u(p);b=20+u(16);assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),h(70))==(0,205,304,2,1,3,1);vertices=rel(rel(b+8));faces=rel(b+268);keys=rel(b+6620);objects=[]
 for i in range(3):
  start=h(b+4060+i*2);count=h(b+4188+i*2);objects.append(dict(index=i,name=data[b+1884+i*32:b+1916+i*32].split(b'\0')[0].decode(),material=h(b+3932+i*2),vertex_start=start,vertex_count=count,vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex(),texture_runs=[v for t in range(2)for v in(h(b+4316+i*16+t*2),h(b+5340+i*16+t*2))],states=[dict(parent=-1,keys=[0],legacy_key_hex=data[keys+i*8:keys+(i+1)*8].hex())]))
 desc=b+920;hh,w=struct.unpack_from('<2I',data,desc+8);assert u(b+1848)==1;pixel=rel(rel(b+1784));pf=data[desc+72:desc+104];texture=dict(width=w,height=hh,pixel_format=pf.hex(),raw=data[pixel:pixel+w*hh*2].hex());assert len(bytes.fromhex(texture['raw']))==w*hh*2
 return dict(name='Vortex',sha256=ASSET_SHA,body_offset=b,flags=0,version=0,vertices=205,faces=304,materials=2,textures=1,states=1,state0_frames=1,objects=objects,tags=[],face_hex=data[faces:faces+304*6].hex(),vertex_hex=data[vertices:vertices+205*32].hex(),material_hex=[data[b+276+i*80:b+356+i*80].hex()for i in range(2)],texture_data=[texture])
class NativeVortex(NativeBarrierWall):
 def setup_resources(self):
  v=self.vm;original=bytes(v.uc.mem_read(self.animator+0x30,40));NativeSpeed.setup_resources(self);v.uc.mem_write(self.animator+0x30,original);a=self.asset
  v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',self.bytes(struct.pack('<I',self.bytes(bytes.fromhex(a['vertex_hex'])))))));v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o in a['objects']:
   p=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in((0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])):v.put_u32(p+off,value)
   v.put_u32(p+0x34,self.bytes(struct.pack('<'+'I'*(a['textures']+1),*o['texture_runs'][::2])));v.put_u32(p+0x38,self.bytes(struct.pack('<'+'I'*(a['textures']+1),*o['texture_runs'][1::2])))
  mats=[]
  for raw in a['material_hex']:
   p=v.allocate(0x58);v.write(p,bytes.fromhex(raw));v.put_u32(p+0x54,0);mats.append(p)
  v.put_u32(self.imagery+0x34,len(mats));v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<'+'I'*len(mats),*mats)));self.surfaces={};vt=v.allocate(33*4);self.texture_contract=[]
  def addref(args):self.surfaces[args[0]]['refs']+=1;return self.surfaces[args[0]]['refs']
  def release(args):self.surfaces[args[0]]['refs']-=1;return self.surfaces[args[0]]['refs']
  def pixel_format(args):v.write(args[1],self.surfaces[args[0]]['pf']);return 0
  def lock(args):
   s=self.surfaces[args[0]];assert not s['locked'];s['locked']=True;d=bytearray(124);struct.pack_into('<5I',d,0,124,0x100f,s['height'],s['width'],s['width']*2);struct.pack_into('<I',d,36,s['address']);d[72:104]=s['pf'];v.write(args[2],bytes(d));return 0
  def unlock(args):s=self.surfaces[args[0]];assert s['locked'];s['locked']=False;return 0
  for slot,label,count,fn in((1,'AddRef',1,addref),(2,'Release',1,release),(21,'GetPixelFormat',2,pixel_format),(25,'Lock',5,lock),(32,'Unlock',2,unlock)):
   name='BarrierWallSurface'+label;v.handlers[name]=(count,fn);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',name))
  nodes=[]
  for i,t in enumerate(a['texture_data']):
   frames=[]
   for f,raw in enumerate(t.get('raw_frames',[t.get('raw')])):
    tex=v.surface('old-controller-texture'+str(i)+'-'+str(f),t['width'],t['height']);v.write(tex.address,bytes.fromhex(raw));p=v.allocate(4);v.put_u32(p,vt);self.surfaces[p]=dict(width=t['width'],height=t['height'],pf=bytes.fromhex(t['pixel_format']),address=tex.address,refs=1,locked=False);frames.append(p)
   node=v.allocate(0xa0);v.put_u32(node+0x7c,len(frames));v.put_u32(node+0x84,self.bytes(struct.pack('<'+'I'*len(frames),*frames)));v.put_u32(node+0x88,self.bytes(struct.pack('<'+'I'*len(frames),*(i*16+f+1 for f in range(len(frames))))));v.put_u32(node+0x90,frames[0]);v.put_u32(node+0x94,i*16+1);nodes.append(node)
  v.put_u32(self.imagery+0x4c,len(nodes));v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<'+'I'*len(nodes),*nodes)));v.put_u32(self.imagery+0x10,1)
 def __init__(self,exe,data):
  super().__init__(exe,data,('Vortex','0x452dade0',0x40dc00,0x5e8508,ASSET_SHA),read_vortex(data),0x5a370c,0x40dd60)
 def render_default(self):
  v=self.vm;owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};v.call(0x417d60,(0,1),this=0x65a57c)
  for key,value in ((7,1),(14,1),(22,2)):v.call(0x56d400,(key,value))
  for obj in self.animobjs:
   assert v.call(0x40a420,(obj,0,0,0,0),this=self.imagery)==1
   v.call(0x40a8f0,(obj,0,0,self.animator+0xac,0xffffffff,1),this=self.imagery,instruction_limit=100000000)
  return v.surface_bytes('screen'),dict(owner=owner,draws=self.draws)

def run(output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/vortex.i3d'))
 f=NativeVortex(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);raw,draw=f.render_default();repeat=f.render_default();assert raw==repeat[0];save_rgb565_png(output/'native-frame0.png',raw,1280,960);report=dict(status='native_preflight_complete',accepted=False,name='Vortex',type_id='0x452dade0',asset_sha256=ASSET_SHA,constructor=f.constructor,asset={k:f.asset[k]for k in ('flags','version','vertices','faces','materials','textures','states','state0_frames')},draw=draw,nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),warm_replay_identical=True,image_sha256=sha((output/'native-frame0.png').read_bytes()),scope='Exact shipped oldbody Vortex default full factory/all93constructors/Initialize and complete actual RenderObject over3authoredroots. Source snapshot customVortex is not its shipped binding. Immutable readonly legacy resource/singleCOMtexture/material and originalkeycache/owner/camera/ambient38dir1/cull2/depthwrite bounds; no fakegeometry/pose/raster. ActualMetal/naturaldevice/exactpixel separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['nonzero_pixels'],len(draw['draws']),report['constructor'],[d.get('geometry_bounds')for d in draw['draws']]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
