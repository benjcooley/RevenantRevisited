#!/usr/bin/env python3
"""Complete default RenderObject with immutable multi-texture resource surfaces."""
import argparse,json,struct,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from scroll_water_visual_reference import NativeScrollScene
from speed_controller_preflight import NativeSpeed,ROOT,read_asset
from software_probe import save_rgb565_png
from firecone_state_probe import sha

class NativeGenericMeshScene(NativeScrollScene):
 def setup_resources(self):
  NativeSpeed.setup_resources(self);v=self.vm;a=self.asset;data=self.data;u=lambda p:struct.unpack_from('<I',data,p)[0];rel=lambda p:p+u(p);bo=a['body_offset']
  raw=rel(rel(rel(bo+16)));pointer=self.bytes(data[raw:raw+a['vertices']*32]);v.put_u32(self.imagery+0x28,self.bytes(struct.pack('<I',self.bytes(struct.pack('<I',pointer)))));v.put_u32(self.imagery+0x30,self.bytes(bytes.fromhex(a['face_hex'])))
  for o in a['objects']:
   resource=v.u32(v.u32(self.imagery+0x74)+o['index']*4)
   for off,value in [(0x24,o['vertex_count']),(0x28,o['vertex_start']),(0x30,0),(0x3c,o['material'])]:v.put_u32(resource+off,value)
   v.put_u32(resource+0x34,self.bytes(struct.pack('<'+'I'*(a['textures']+1),*o['texture_runs'][::2])));v.put_u32(resource+0x38,self.bytes(struct.pack('<'+'I'*(a['textures']+1),*o['texture_runs'][1::2])))
  mats=[];rawmat=rel(bo+32)
  for i in range(a['materials']):
   p=v.allocate(0x58);v.write(p,data[rawmat+i*80:rawmat+(i+1)*80]);v.put_u32(p+0x54,0);mats.append(p)
  v.put_u32(self.imagery+0x34,len(mats));v.put_u32(self.imagery+0x44,self.bytes(struct.pack('<'+'I'*len(mats),*mats)))
  self.surfaces={};self.surface_trace=[];vt=v.allocate(33*4)
  def record(args,name):
   s=self.surfaces[args[0]];self.surface_trace.append(dict(surface=s['index'],method=name));return s
  def addref(args):s=record(args,'AddRef');s['references']+=1;return s['references']
  def release(args):s=record(args,'Release');s['references']-=1;assert s['references']>=0;return s['references']
  def pixel_format(args):s=record(args,'GetPixelFormat');v.write(args[1],s['pixel_format']);return 0
  def lock(args):
   s=record(args,'Lock');assert not s['locked']and args[1]==args[3]==args[4]==0 and v.u32(args[2])==124;s['locked']=True;desc=bytearray(124);struct.pack_into('<5I',desc,0,124,0x100f,s['height'],s['width'],s['width']*2);struct.pack_into('<I',desc,36,s['address']);desc[72:104]=s['pixel_format'];v.write(args[2],bytes(desc));return 0
  def unlock(args):s=record(args,'Unlock');assert s['locked']and args[1]==0;s['locked']=False;return 0
  for slot,name,count,method in [(1,'AddRef',1,addref),(2,'Release',1,release),(21,'GetPixelFormat',2,pixel_format),(25,'Lock',5,lock),(32,'Unlock',2,unlock)]:
   label='GenericMeshSurface'+name;v.handlers[label]=(count,method);v.put_u32(vt+slot*4,v.api_address('kernel32.dll',label))
  tex=rel(bo+40);nodes=[];self.texture_contract=[]
  for i in range(a['textures']):
   p=tex+i*120;h,w=struct.unpack_from('<2I',data,p+8);pf=data[p+72:p+104];masks=struct.unpack('<8I',pf);assert masks[3:]==(16,0xf800,0x7e0,0x1f,0)or masks[3:]==(16,0xf00,0xf0,0xf,0xf000);assert u(p+116)==1
   pixel=rel(rel(p+108));raw=data[pixel:pixel+w*h*2];assert len(raw)==w*h*2;surface=v.surface('generic-texture-'+str(i),w,h);v.write(surface.address,raw);obj=v.allocate(4);v.put_u32(obj,vt);self.surfaces[obj]=dict(index=i,width=w,height=h,address=surface.address,pixel_format=pf,references=1,locked=False)
   node=v.allocate(0xa0);v.put_u32(node+0x7c,1);v.put_u32(node+0x90,obj);v.put_u32(node+0x94,i+1);nodes.append(node);self.texture_contract.append(dict(index=i,width=w,height=h,pixel_format=masks,raw_sha256=sha(raw)))
  v.put_u32(self.imagery+0x4c,len(nodes));v.put_u32(self.imagery+0x5c,self.bytes(struct.pack('<'+'I'*len(nodes),*nodes)));v.put_u32(self.imagery+0x10,1)
 def __init__(self,exe,data,name,width=1280,height=960):
  super().__init__(exe,data,name,width,height);self.return_address=None;self.clear_color=0
  # Every supplied node declares one real resident frame. Execute the original
  # one-frame selector instead of inheriting the scroll fixture's boundary.
  self.vm.uc.hook_del(self.texture_frame_hook);self.texture_selector_calls=0
  def selected(uc,address,size,user):self.texture_selector_calls+=1
  self.vm.uc.hook_add(UC_HOOK_CODE,selected,begin=0x40c520,end=0x40c520)
  original_clear=self.software.clear;self.software.clear=lambda:original_clear(self.clear_color)
  def finished(uc,address,size,user):
   count=self.draws[-1]['args'][3];rows=list(struct.iter_unpack('<3f4I2f',self.vm.uc.mem_read(self.software.vertices,count*36)));self.draws[-1]['projected_geometry_bounds']=[min(r[0]for r in rows),-max(r[1]for r in rows),max(r[0]for r in rows),-min(r[1]for r in rows)]
  def entered(uc,address,size,user):
   ret=self.vm.u32(uc.reg_read(UC_X86_REG_ESP))
   if self.return_address is None:self.return_address=ret;self.vm.uc.hook_add(UC_HOOK_CODE,finished,begin=ret,end=ret)
   elif ret!=self.return_address:raise AssertionError('Original generic FVF caller changed')
  self.vm.uc.hook_add(UC_HOOK_CODE,entered,begin=0x56eb30,end=0x56eb30)
 def render(self,tick,frame,mode=0,cull=2):
  raw,depth,record=super().render(tick,frame,mode,cull);assert all(not s['locked']for s in self.surfaces.values());return raw,depth,record

def run(executable,archive,output,background='black'):
 output.mkdir(parents=True,exist_ok=False);profiles=json.loads((ROOT/'tools/retail_runtime/generic_static_profiles.json').read_text())['profiles'];cases=[];initializations=[];width,height=1280,960
 # Literal ltgray gallery backdrop quantized to the original565 destination.
 clear=0 if background=='black'else ((int(.45*255)>>3)<<11)|((int(.46*255)>>2)<<5)|(int(.50*255)>>3)
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()}
  for profile in profiles:
   if background=='ltgray'and profile['name']!='CharUtility':continue
   name=profile['name'];data=z.read(ns['imagery/'+profile['asset']]);assert sha(data)==profile['asset_sha256'];folder=output/name;folder.mkdir();f=NativeGenericMeshScene(executable,data,name,width,height);f.clear_color=clear;init=f.initialize_scene();assert not init['controllers'];initializations.append(dict(name=name,initialization=init,texture_contract=f.texture_contract));drawlog=[]
   for capture in [1,5,15,29,45,59]:
    tick=capture+1;frame=tick%profile['frames'];raw,depth,record=f.render(tick,frame);replay=f.render(tick,frame);assert raw==replay[0]and depth==replay[1];assert len(record['draws'])==(5 if name=='CharUtility'else 2)
    bounds=[d['projected_geometry_bounds']for d in record['draws']];assert all(0<b[0]<b[2]<width-1 and 0<b[1]<b[3]<height-1 for b in bounds)
    image=folder/f'native-capture{capture:03d}.png';save_rgb565_png(image,raw,width,height);drawlog.append(record);cases.append(dict(name=name,type_id=profile['id'],capture=capture,owner_frame=frame,draws=len(record['draws']),image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=sha(raw),depth_sha256=sha(depth),geometry_bounds=bounds,whole_geometry_unclipped=True,changed_pixels=sum(v!=(clear,)for v in struct.iter_unpack('<H',raw))))
   (folder/'draws.json').write_text(json.dumps(drawlog,indent=2)+'\n');(folder/'initialization.json').write_text(json.dumps(initializations[-1],indent=2)+'\n')
 report=dict(status='reference_complete',accepted=False,profiles=[p for p in profiles if background=='black'or p['name']=='CharUtility'],cases=cases,viewport=[width,height],origin=[0,0,0],camera=[0,0,0],source_ambient=[38]*3,directional=[1]*3,background=background,clear_rgb565=clear,mode=0,cull=2,depth_test=True,depth_write=True,original_executable_sha256=sha(executable.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),shared_fixture_sha256=sha(Path(__file__).with_name('scroll_water_visual_reference.py').read_bytes()),scope='Whole state0 default factory/controller refresh/owner/integer keys/RenderObject/FVF_VERTEX/normal lighting/software raster over every exact root and565/4444 texture slot. Immutable decoded mesh/material resources, resident single-frame texture COM and source scene clear/light/depth/cull inputs explicit. All transformed vertices fitfixed1280x960 and complete RGB565/depth warm replays match. FullLoadTexture/materialhandlecreation/otherDoorstates/naturaldevice/caller separate. No hostgeometry/pose/UV/color/raster replacement.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--name',required=True,choices=['CharUtility','KinSecretDoor']);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=False)
 row=next(x for x in json.loads((ROOT/'docs/vfx/EFFECT_BURNDOWN.json').read_text())['retail_effects']if x['retail_name']==a.name)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/'+row['asset'].replace('\\','/').lower()))
 f=NativeGenericMeshScene(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,a.name);init=f.initialize_scene();raw,depth,record=f.render(2,0);repeat=f.render(2,0);assert raw==repeat[0]and depth==repeat[1];save_rgb565_png(a.output/'native-frame0.png',raw,1280,960);report=dict(status='native_preflight',name=a.name,type_id=row['retail_type_id'],asset_sha256=sha(data),initialization=init,texture_contract=f.texture_contract,draw=record,nonzero_pixels=sum(v!=(0,)for v in struct.iter_unpack('<H',raw)),image_sha256=sha((a.output/'native-frame0.png').read_bytes()),depth_sha256=sha(depth),accepted=False,scope='Whole state0 default/owner/key/RenderObject/software with decoded immutable roots/materials and all single-frame exact565/4444 resident COM surfaces. Explicit origin/camera0/source light/cull2/depthtest+write/mode0; fullLoadTexture/materialhandlecreation/naturaldevice/caller separate. No hostgeometry/color/UV/pose injection.');(a.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(a.name,report['nonzero_pixels'],len(record['draws']),[d.get('projected_geometry_bounds')for d in record['draws']])
