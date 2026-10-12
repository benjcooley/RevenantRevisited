"""Explicit non-identical device realization: native1555 upload into supported4444.

This is not an unchanged retail device or primary appearance reference. Original
ConvertTexture performs every texel conversion; no host color formula writes pixels.
"""
import argparse,json,struct,hashlib,zipfile
from pathlib import Path
import watcher_loaded_visual_reference as reference
from watcher_surface_flow_probe import ObservedWorld
from character_native_preflight import ROOT,sha
from static_texture_probe import parse_frames

FMT1555=struct.pack('<8I',32,65,0,16,0x7c00,0x3e0,0x1f,0x8000)
FMT4444=struct.pack('<8I',32,65,0,16,0xf00,0xf0,0xf,0xf000)
def run(boot,output,convert=True,semantic_alpha=False):
 assert not semantic_alpha or convert
 worlds=[];owners=[];realizations=[];conversions=[]
 def factory(*args,**kwargs):
  f=ObservedWorld(*args,**kwargs);worlds.append(f);new_surface=f.world.ddraw._new_surface
  def realize(width,height,caps,pixel_format,memory=0,pitch=0):
   if caps&0x1000 and pixel_format==FMT1555:
    assert not memory
    pointer=new_surface(width,height,caps,FMT4444,memory,pitch);realizations.append(dict(name=f.name,pointer=hex(pointer),requested=list(struct.unpack('<8I',pixel_format)),realized=list(struct.unpack('<8I',FMT4444))));return pointer
   return new_surface(width,height,caps,pixel_format,memory,pitch)
  f.world.ddraw._new_surface=realize;new_object=f.new_object
  def track(name,*args,**kwargs):
   owner=new_object(name,*args,**kwargs);owners.append((name,owner));return owner
  f.new_object=track;original_call=f.world.call
  def call(address,*args,**kwargs):
   result=original_call(address,*args,**kwargs)
   if convert and address==0x46e8b0:
    v=f.vm;owner=kwargs['this'];im=v.u32(owner+0x54);node=v.u32(v.u32(im+0x5c));table=v.u32(node+0x84)
    pointers=[v.u32(table+i*4)for i in range(v.u32(node+0x7c))]
    current=v.u32(node+0x90)
    if current not in pointers:pointers.append(current)
    for pointer in dict.fromkeys(pointers):
     surface=f.world.ddraw.surface(pointer);assert surface.pixel_format==FMT4444
     raw=bytes(v.uc.mem_read(surface.memory,surface.pitch*surface.height));source=v.allocate(len(raw));v.uc.mem_write(source,raw)
     desc=bytearray(124);struct.pack_into('<5I',desc,0,124,0,surface.height,surface.width,surface.pitch//2);desc[72:104]=FMT1555
     source_desc=v.allocate(124);v.uc.mem_write(source_desc,bytes(desc));desc[72:104]=FMT4444
     dest_desc=v.allocate(124);v.uc.mem_write(dest_desc,bytes(desc))
     original_call(0x416790,(source_desc,source,0,dest_desc,surface.memory,0),this=0x65a57c,instruction_limit=100000000)
     converted=bytes(v.uc.mem_read(surface.memory,len(raw)))
     native_hash=hashlib.sha256(converted).hexdigest()
     if semantic_alpha:
      # Explicit adapted-device rule only: preserve binary alpha semantics.
      # RGB is still produced by the original converter; no tint/brightness fit.
      source_words=struct.unpack('<16384H',raw);native_words=struct.unpack('<16384H',converted)
      converted=struct.pack('<16384H',*[((n&0x0fff)|((15 if w&0x8000 else 0)<<12))for w,n in zip(source_words,native_words)])
      v.uc.mem_write(surface.memory,converted)
     conversions.append(dict(name=f.name,surface=hex(pointer),source_sha256=hashlib.sha256(raw).hexdigest(),original_converter_sha256=native_hash,converted_sha256=hashlib.sha256(converted).hexdigest(),changed_texels=sum(a!=b for a,b in zip(struct.unpack('<16384H',raw),struct.unpack('<16384H',converted)))))
   return result
  f.world.call=call;return f
 old=reference.AuthenticCharacters;reference.AuthenticCharacters=factory
 try:reference.run(boot,output)
 finally:reference.AuthenticCharacters=old
 f=worlds[0];v=f.vm;profiles={p['name']:p for p in json.loads((ROOT/'tools/retail_runtime/static_additional_profiles.json').read_text())['profiles'][1:]};frames=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  names={n.lower():n for n in z.namelist()}
  for name,owner in owners:
   p=profiles[name];data=z.read(names['imagery/'+p['asset'].replace('\\','/').lower()]);m=parse_frames(data,p);im=v.u32(owner+0x54);node=v.u32(v.u32(im+0x5c));surfaces=v.u32(node+0x84);assert v.u32(node+0x7c)==len(m['textures'])
   for i,source in enumerate(m['textures']):
    surface=f.world.ddraw.surface(v.u32(surfaces+i*4));assert surface.pixel_format==FMT4444
    converted=bytes(v.uc.mem_read(surface.memory,surface.pitch*surface.height));src=struct.unpack('<16384H',source);dst=struct.unpack('<16384H',converted)
    expected=[((w>>15)*(15 if semantic_alpha else 8)<<12)|(((w>>10)&31)>>1<<8)|(((w>>5)&31)>>1<<4)|((w&31)>>1)for w in src]
    mismatches=sum(a!=b for a,b in zip(expected,dst))
    if convert:assert mismatches==0,(name,i,mismatches)
    else:assert source==converted
    samples=[(a,b)for a,b in zip(src,dst)if a][:16]
    frames.append(dict(name=name,frame=i,source_sha256=hashlib.sha256(source).hexdigest(),converted_sha256=hashlib.sha256(converted).hexdigest(),expected_native_shift_mismatches=mismatches,opaque_source_texels=sum(w>>15 for w in src),converted_alpha_values=sorted(set(w>>12 for w in dst)),samples=samples))
 manifest=json.loads((output/'manifest.json').read_text());manifest['status']='qualified_device_render_complete';manifest['device_identical']=False;manifest['semantic_alpha_adaptation']=semantic_alpha;manifest['scope']='Declared device4444 realization with optional extra original ConvertTexture call after loading; original frame selection/fullrender otherwise unchanged. This supersedes the imported fixture no-substitution scope. No primary appearance acceptance.';(output/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
 report=dict(semantic_alpha_adaptation=semantic_alpha,executable_sha256=sha(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'),status='qualified_device_conversion' if convert else 'qualified_realization_negative_control',extra_original_conversion=convert,accepted=False,primary_visual_pass=False,device_identical=False,conversions=conversions,realizations=realizations,frames=frames,events=f.events,kernels=f.kernels,texel_samples=f.texels,original_full_reference=sha(output/'manifest.json'),probe_sha256=sha(Path(__file__)),scope=('Declared COM allocation realizes requested ARGB1555 as ARGB4444. '+('After loading, an extra original ConvertTexture call converts each resident frame using declared1555 source/4444 destination descriptors. Its alpha1 expands to alpha4=8, not15; RGB5 reduces to RGB4. ' if convert else 'Realization-only negative control: original upload still copies1555 bytes unchanged despite the different realized descriptor. ' )+('Explicit additional device adaptation maps original alpha1 to full alpha4=15 while retaining original-converted RGB4. ' if semantic_alpha else '')+'Original SetTexture/fullrenderer consume the resident surfaces. Device-identical and primary appearance acceptance remain false. No host color formula, geometry/controller/kernel replacement, or port decoder change. Hardware expectations and quantization remain open.'))
 (output/'qualified-device.json').write_text(json.dumps(report,indent=2)+'\n');print('QUALIFIED',len(frames),[x['expected_native_shift_mismatches']for x in frames]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--boot-image',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--realization-only',action='store_true');p.add_argument('--semantic-alpha',action='store_true');a=p.parse_args();run(a.boot_image,a.output,not a.realization_only,a.semantic_alpha)
