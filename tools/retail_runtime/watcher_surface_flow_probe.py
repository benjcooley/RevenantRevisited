"""Read-only original watcher descriptor/upload/alpha-dispatch flow; no retagging."""
import argparse,json,struct,hashlib,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_ESI
import watcher_loaded_visual_reference as reference
from character_native_preflight import AuthenticCharacters,ROOT,sha
from static_texture_probe import parse_frames

class ObservedWorld(AuthenticCharacters):
 def __init__(self,*args,**kwargs):
  super().__init__(*args,**kwargs);self.name='setup';self.events=[];self.pending={};self.texels=[];self.scene_before_resources=False;v=self.vm
  def entry(uc,address,*unused):
   sp=uc.reg_read(UC_X86_REG_ESP);ret=v.u32(sp);count=2 if address==0x4164c0 else 6;args=[v.u32(sp+4+4*i)for i in range(count)];event=dict(name=self.name,function=hex(address),scene_initialized=v.u32(0x65a57c),software=v.u32(0x5d7a28),format_count=v.u32(0x5e8934),source_desc=bytes(v.uc.mem_read(args[0],124)).hex(),destination_desc_before=bytes(v.uc.mem_read(args[1]if count==2 else args[3],124)).hex());self.events.append(event)
   if count==6:
    width,height=v.u32(args[0]+12),v.u32(args[0]+8);event['source_pixels_sha256']=hashlib.sha256(bytes(v.uc.mem_read(args[1],width*height*2))).hexdigest()
   self.pending.setdefault(ret,[]).append((args,event))
   if len(self.pending[ret])==1:
    def returned(uc,*unused,ret=ret):
     if not self.pending.get(ret):return
     args,event=self.pending[ret].pop();dst=args[1]if len(args)==2 else args[3];event['destination_desc_after']=bytes(v.uc.mem_read(dst,124)).hex()
     if len(args)==6:
      width,height=v.u32(dst+12),v.u32(dst+8);event['destination_pixels_sha256']=hashlib.sha256(bytes(v.uc.mem_read(args[4],width*height*2))).hexdigest()
    v.uc.hook_add(UC_HOOK_CODE,returned,begin=ret,end=ret)
  for a in (0x4164c0,0x416790):v.uc.hook_add(UC_HOOK_CODE,entry,begin=a,end=a)
  def selected(uc,*unused):
   pointer=v.u32(0x67604c)
   if pointer and pointer in self.world.ddraw.surfaces:
    surface=self.world.ddraw.surface(pointer);self.events.append(dict(name=self.name,function='SetTexture-return',surface=hex(pointer),pixel_format=list(struct.unpack('<8I',surface.pixel_format)),alpha_branch=v.uc.mem_read(0x676054,1)[0]))
  v.uc.hook_add(UC_HOOK_CODE,selected,begin=0x56d3f8,end=0x56d3f8)
  def sample_texel(uc,address,*unused):
   if sum(x['name']==self.name for x in self.texels)>=16:return
   word=uc.reg_read(UC_X86_REG_ESI)&0xffff
   if not word or any(x['name']==self.name and x['word']==word for x in self.texels):return
   self.texels.append(dict(name=self.name,instruction=hex(address),word=word,actual_alpha4=word>>12,actual_rgb4=[(word>>8)&15,(word>>4)&15,word&15],authored_alpha1=word>>15,authored_rgb5=[(word>>10)&31,(word>>5)&31,word&31]))
  for a in (0x55048f,0x5507b8):v.uc.hook_add(UC_HOOK_CODE,sample_texel,begin=a,end=a)
  self.kernels=[]
  for a in (0x54e0d0,0x54e050,0x54e090,0x54e010,0x54e5e0,0x54e3a0,0x54e4c0,0x54e310,0x54ea30,0x54e810,0x54e920,0x54e700,0x54eb70,0x54eb40):
   v.uc.hook_add(UC_HOOK_CODE,lambda uc,address,*unused:self.kernels.append(dict(name=self.name,entry=hex(address))),begin=a,end=a)
 def new_object(self,name,*args,**kwargs):
  self.name=name
  if self.scene_before_resources:self.world.call(0x416f70,this=0x65a57c)
  return super().new_object(name,*args,**kwargs)

def run(boot,output,scene_before_resources=False):
 worlds=[]
 def factory(*args,**kwargs):
  f=ObservedWorld(*args,**kwargs);f.scene_before_resources=scene_before_resources
  original=f.world.call
  def call(address,*args,**kwargs):
   result=original(address,*args,**kwargs)
   if scene_before_resources and address==0x46e8b0:original(0x416fb0,this=0x65a57c)
   return result
  f.world.call=call;worlds.append(f);return f
 old=reference.AuthenticCharacters;reference.AuthenticCharacters=factory
 try:reference.run(boot,output)
 finally:reference.AuthenticCharacters=old
 f=worlds[0];profiles=json.loads((ROOT/'tools/retail_runtime/static_additional_profiles.json').read_text())['profiles'][1:];assets={}
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  names={n.lower():n for n in z.namelist()}
  for p in profiles:
   data=z.read(names['imagery/'+p['asset'].replace('\\','/').lower()]);m=parse_frames(data,p);assets[p['name']]=dict(asset_sha256=hashlib.sha256(data).hexdigest(),stored_format=m['pixel_format'],stored_frame_sha256=[hashlib.sha256(raw).hexdigest()for raw in m['textures']])
 for name,asset in assets.items():
  events=[x for x in f.events if x['name']==name];closest=[x for x in events if x['function']=='0x4164c0'];copies=[x for x in events if x['function']=='0x416790'];selected=[x for x in events if x['function']=='SetTexture-return']
  assert len(closest)==1 and closest[0]['scene_initialized']==1 and closest[0]['software']==1
  assert list(struct.unpack_from('<8I',bytes.fromhex(closest[0]['destination_desc_after']),72))==asset['stored_format']
  assert len(copies)==len(asset['stored_frame_sha256'])
  assert [x['source_pixels_sha256']for x in copies]==asset['stored_frame_sha256']
  assert all(x['source_pixels_sha256']==x['destination_pixels_sha256']for x in copies)
  assert selected and all(x['pixel_format']==asset['stored_format'] and x['alpha_branch']==1 for x in selected)
  assert set(x['entry']for x in f.kernels if x['name']==name)=={'0x54e050'}
 report=dict(status='observed_no_conversion_policy',accepted=False,scene_before_resources=scene_before_resources,assets=assets,events=f.events,kernels=f.kernels,texel_samples=f.texels,original_full_reference=sha(output/'manifest.json'),probe_sha256=sha(Path(__file__)),scope='Actual loaded source/closest-format/upload/selected-surface/full-raster calls observed only. No descriptor/texel/mask rewrite, no substituted format enumeration or pixel kernel. Correct portable ARGB1555 decoder remains unchanged.')
 (output/'surface-flow.json').write_text(json.dumps(report,indent=2)+'\n');print('FLOW',len(f.events),set(x['entry']for x in f.kernels));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--scene-before-resources',action='store_true');a=p.parse_args();run(a.boot_image,a.output,a.scene_before_resources)
