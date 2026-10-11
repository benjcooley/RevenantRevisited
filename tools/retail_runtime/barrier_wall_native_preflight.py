#!/usr/bin/env python3
"""Literal legacy BarrierEW/NS full native factories, keys, flares and raster."""
import argparse,json,struct,zipfile
from pathlib import Path
from speed_controller_preflight import NativeSpeed,ROOT
from generic_mesh_visual_reference import NativeGenericMeshScene
from lit_software_probe import LitSoftwareFixture
from firecone_owner_probe import native_owner
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
PROFILES=(('BarrierEW','0x224a5adf',0x4f6420,0x66cc08,'3a2080c6e8ea8fb79c4e5d19f99d408d4429b8d4f3e2ded43fc879fd092a0798'),('BarrierNS','0x224b5adc',0x4f6530,0x66cc20,'89a8637e5c46b9da0c256762b1c967383e957cf7e188fe07a5c0201caff2306a'))
def read_old(data,name,digest):
 assert sha(data)==digest;u=lambda p:struct.unpack_from('<I',data,p)[0];h=lambda p:struct.unpack_from('<H',data,p)[0];rel=lambda p:p+u(p);b=20+u(16);assert (u(b),u(b+4),u(b+264),u(b+272),u(b+916),u(b+1880),h(70))==(4,40,20,2,2,5,1);vertices=rel(rel(b+8));faces=rel(b+268);keybase=rel(b+6620);objects=[]
 for i in range(5):
  parent=struct.unpack_from('<b',data,rel(b+7132+i*4))[0];assert parent==-1;start=h(b+4060+i*2);count=h(b+4188+i*2);rawkeys=data[keybase+i*8:keybase+(i+1)*8];assert len(rawkeys)==8;objects.append(dict(index=i,name=data[b+1884+i*32:b+1884+(i+1)*32].split(b'\0')[0].decode(),material=h(b+3932+i*2),vertex_start=start,vertex_count=count,vertex_hex=data[vertices+start*32:vertices+(start+count)*32].hex(),texture_runs=[v for t in range(3)for v in(h(b+4316+i*16+t*2),h(b+5340+i*16+t*2))],states=[dict(parent=parent,keys=[0],legacy_key_hex=rawkeys.hex())]))
 textures=[]
 for i in range(2):
  desc=b+920+i*108;hh,w=struct.unpack_from('<2I',data,desc+8);assert u(b+1848+i*4)==1;pointer=rel(b+1784+i*4);pixel=rel(pointer);pf=data[desc+72:desc+104];assert struct.unpack('<8I',pf)[3:]==(16,0xf00,0xf0,0xf,0xf000);textures.append(dict(width=w,height=hh,pixel_format=pf.hex(),raw=data[pixel:pixel+w*hh*2].hex()))
 return dict(name=name,sha256=digest,body_offset=b,flags=4,version=0,vertices=40,faces=20,materials=2,textures=2,states=1,state0_frames=1,objects=objects,tags=[],face_hex=data[faces:faces+120].hex(),vertex_hex=data[vertices:vertices+1280].hex(),material_hex=[data[b+276+i*80:b+276+(i+1)*80].hex()for i in range(2)],texture_data=textures)
class NativeBarrierWall(NativeSpeed):
 def copy_vertices(self,*args):pass
 def setup_resources(self):
  v=self.vm;original=bytes(v.uc.mem_read(self.animator+0x30,40));NativeSpeed.setup_resources(self);v.uc.mem_write(self.animator+0x30,original);a=self.asset
  v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',self.bytes(struct.pack('<I',self.bytes(bytes.fromhex(a['vertex_hex'])))))));v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o in a['objects']:
   p=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in((0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])):v.put_u32(p+off,value)
   v.put_u32(p+0x34,self.bytes(struct.pack('<3I',*o['texture_runs'][::2])));v.put_u32(p+0x38,self.bytes(struct.pack('<3I',*o['texture_runs'][1::2])))
  mats=[]
  for raw in a['material_hex']:
   p=v.allocate(0x58);v.write(p,bytes.fromhex(raw));v.put_u32(p+0x54,0);mats.append(p)
  v.put_u32(self.imagery+0x34,2);v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<2I',*mats)));self.surfaces={};vt=v.allocate(33*4);self.texture_contract=[]
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
  v.put_u32(self.imagery+0x4c,2);v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<2I',*nodes)));v.put_u32(self.imagery+0x10,1)
 def __init__(self,exe,data,profile,asset_input=None,binding_vtable=0x5aa46c,initialize=0x4e7530):
  name,id,factory,builder,digest=profile;NativeSpeed.__init__(self,exe,(asset_input if asset_input is not None else read_old(data,name,digest)),1280,960,software_factory=LitSoftwareFixture,particle_required=False,native_binding=(factory,builder,binding_vtable),eager_pose=False);v=self.vm;self.tick=0;self.draws=[];self.render_states={};self.raster_enabled=True;self.return_address=None;self.software.lighting((38,38,38),(1,1,1))
  for address,value in((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5c619c,1),(0x5e8850,1)):v.put_u32(address,value)
  method=self.bytes(b'\xb8'+struct.pack('<I',self.animator)+b'\xc3');vt=v.allocate(0x200);v.put_u32(vt+0x24,method);v.put_u32(self.owner,vt);flags=self.bytes(b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');v.put_u32(vt+0x40,flags);v.put_u32(self.owner+0x4c,self.imagery);done=self.bytes(b'\x31\xc0\xc2\x04\x00');v.put_u32(vt+0x158,done)
  def rand(uc,a,n,u):self.ret(v.random_msvc(),0)
  v.rng_seed=1;v.uc.hook_add(UC_HOOK_CODE,rand,begin=0x58c582,end=0x58c582)
  device=v.allocate(4);dvt=v.allocate(0xa0);v.put_u32(device,dvt);v.put_u32(0x668f14,device);self.material_state=0
  def getlight(args):v.put_u32(args[2],self.material_state);return 0
  def setlight(args):self.material_state=args[2];return 0
  for slot,name,fn in((23,'BarrierWallGetLight',getlight),(24,'BarrierWallSetLight',setlight)):v.handlers[name]=(3,fn);v.put_u32(dvt+slot*4,v.api_address('kernel32.dll',name))
  def state(uc,a,n,u):sp=uc.reg_read(UC_X86_REG_ESP);self.render_states[v.u32(sp+4)]=v.u32(sp+8)
  def finished(uc,a,n,u):
   if self.raster_enabled:
    rows=list(struct.iter_unpack('<3f4I2f',uc.mem_read(self.software.vertices,self.draws[-1]['args'][3]*36)));self.draws[-1]['geometry_bounds']=[min(x[0]for x in rows),-max(x[1]for x in rows),max(x[0]for x in rows),-min(x[1]for x in rows)]
  def draw(uc,a,n,u):
   sp=uc.reg_read(UC_X86_REG_ESP);self.draws.append(dict(args=[v.u32(sp+4+i*4)for i in range(7)],states=dict(self.render_states)));ret=v.u32(sp)
   if self.raster_enabled:
    if self.return_address is None:self.return_address=ret;v.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
    else:assert ret==self.return_address
   else:self.ret(0,28)
  v.uc.hook_add(UC_HOOK_CODE,state,begin=0x56d400,end=0x56d400);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30);v.call(0x401610);v.call(initialize,this=self.animator,instruction_limit=10000000);self.animobjs=[v.call(0x40eef0,(i,),this=self.animator)for i in range(len(self.asset['objects']))]
  # Exact upstream authored-key cache refresh on the actual SetupObjects records.
  # The resource/owner cache is an explicit boundary; no host pose is supplied.
  for obj in self.animobjs:assert v.call(0x40a420,(obj,0,0,0,0),this=self.imagery)==1
 def advance(self):self.tick+=1;self.vm.put_u32(0x65cb38,self.tick);self.vm.call(0x4e7700,this=self.animator,instruction_limit=10000000)
 def render(self):
  owner=native_owner(self);self.software.clear();self.draws=[];self.render_states={};self.vm.call(0x4e7a10,this=self.animator,instruction_limit=100000000);return self.vm.surface_bytes('screen'),dict(tick=self.tick,rng_seed=self.vm.rng_seed,owner=owner,draws=self.draws)
def run(output,ticks):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for profile in PROFILES:
   name,id,_,_,digest=profile;data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/'+name.lower()+'.i3d'));f=NativeBarrierWall(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,profile);folder=output/name;folder.mkdir();rows=[]
   for tick in range(ticks+1):
    if tick:f.advance()
    f.raster_enabled=tick in(0,1,6,16,30,60,90);raw,r=f.render();r['raster_executed']=f.raster_enabled
    if f.raster_enabled:p=folder/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960);r.update(image=str(p),image_sha256=sha(p.read_bytes()),nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)));print(name,tick,len(r['draws']),r['nonzero_pixels'],flush=True)
    rows.append(r)
   cases.append(dict(name=name,id=id,asset_sha256=digest,constructor=f.constructor,rows=rows,object_keys=[x['states'][0]['legacy_key_hex']for x in f.asset['objects']],object_parents=[x['states'][0]['parent']for x in f.asset['objects']]))
 report=dict(status='native_preflight_complete',accepted=False,cases=cases,scope='Exact legacy0BarrierEW/NS full constructor/customInitialize/SetupObjects/CopyVertices/actual5object authored-key cache40a420/30slot Animate/RenderObject/softwareARGB4444. Own immutable raw5root/40vertex/20face/2texture/key/material resource/COM and nullspellowner flag/command boundaries; no productiongeometry/UV/colors/pose/raster input. Selected full native phases only; all intervening originalRender executes through56eb30devicehandoff. Port/actualMetal/natural/device separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=30);a=p.parse_args();run(a.output,a.ticks)
