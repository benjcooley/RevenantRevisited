#!/usr/bin/env python3
"""Finite actual default Jtele/Jteled owner, parser, controllers and full raster."""
import argparse,json,struct,zipfile
from pathlib import Path
from jtele_native_preflight import NativeJtele,ROOT,PROFILES
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EFLAGS

def run(output):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name+'.i3d'));f=NativeJtele(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name,False);init=f.initialize_scene();v=f.vm;current=[0];returned=[None];folder=output/name;folder.mkdir();rows=[]
   def obj(uc,a,n,u):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
   def finished(uc,a,n,u):
    if not f.raster_enabled:return
    d=f.draws[-1];verts=list(struct.iter_unpack('<3f4I2f',uc.mem_read(f.software.vertices,d['args'][3]*36)));d['geometry_bounds']=[min(r[0]for r in verts),-max(r[1]for r in verts),max(r[0]for r in verts),-min(r[1]for r in verts)]
   def draw(uc,a,n,u):
    d=f.draws[-1];d.update(object=v.u32(current[0]+4),vertex_sha256=sha(bytes(uc.mem_read(d['args'][2],d['args'][3]*32))),matrix=list(struct.unpack('<16f',uc.mem_read(current[0]+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
    if returned[0]is None:returned[0]=v.u32(uc.reg_read(UC_X86_REG_ESP));v.uc.hook_add(UC_HOOK_CODE,finished,begin=returned[0],end=returned[0])
   def guard(uc,a,n,u):
    if uc.reg_read(UC_X86_REG_EFLAGS)&64:f.draws[-1]['oversized_triangles']=f.draws[-1].get('oversized_triangles',0)+1
   v.uc.hook_add(UC_HOOK_CODE,obj,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
   for a in(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):v.uc.hook_add(UC_HOOK_CODE,guard,begin=a,end=a)
   for tick in range(1,54):
    f.step();f.raster_enabled=tick in(2,6,16,25,40,48,49,50,53);raw,r=f.render();r['raster_executed']=f.raster_enabled
    if f.raster_enabled:
     image=folder/f'native-{tick:03d}.png';save_rgb565_png(image,raw,1280,960);r.update(image=str(image),image_sha256=sha(image.read_bytes()),rgb565_sha256=sha(raw),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)));print(name,tick,len(r['draws']),r['nonzero_pixels'],flush=True)
    rows.append(r)
   assert all(not(int(r['owner_flags'],16)&0x1000)for r in rows[:49]);assert all(int(r['owner_flags'],16)&0x1000 and not r['draws']for r in rows[49:]);cases.append(dict(name=name,type_id=id,asset_sha256=digest,initialization=init,rows=rows))
 report=dict(status='reference_complete',accepted=False,cases=cases,first_render_tick=1,original_executable_sha256=sha((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()),viewport=[1280,960],camera=[0,0,0],origin=[0,0,0],ambient=[38]*3,directional=[1]*3,quality=0,initial_blendmode=1,seed=1,scope='Exact own Jtele/Jteled actualdefaultfactory/all93constructors/wholeInitialize/8originalSetupObjects. OriginalownerNextFrame/Pulse +defaultanimatorPulse/Animate/Render everytick; complete selected nativeRGB565/normal-lit andLVERTEXparticle raster, deviceboundaryotherwise. Initial constructorcacheandactuallazyemittermatrices preserved; literal tag15notactivation, finite50blanktail. Script/tag-audio ownerbase, readonlyresources/multtextureCOM/flatground0/sourceinitialmode1 explicitbounds. Shared640x480primitive-spanrejection retained; constants-onlywidening separate, no ordinarygeometrydrop. Source/actualMetal/map/device/exactpixels separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
