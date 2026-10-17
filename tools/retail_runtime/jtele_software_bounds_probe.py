#!/usr/bin/env python3
"""Only original raster-span constants widen; all Jtele draw inputs unchanged."""
import argparse,json,struct,zipfile
from pathlib import Path
from jtele_native_preflight import NativeJtele,ROOT,PROFILES
from speed_profile_contract import sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EFLAGS,UC_X86_REG_ESP

def run(output):
 output.mkdir(parents=True,exist_ok=False);cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name+'.i3d'));f=NativeJtele(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name,False);f.initialize_scene();v=f.vm;current=[0];guards=[]
   def obj(uc,a,n,u):current[0]=v.u32(uc.reg_read(UC_X86_REG_ESP)+4)
   def draw(uc,a,n,u):
    row=f.draws[-1];row.update(object=v.u32(current[0]+4),vertex_sha256=sha(bytes(uc.mem_read(row['args'][2],row['args'][3]*32))),matrix=list(struct.unpack('<16f',uc.mem_read(current[0]+0x58,64))),world=list(struct.unpack('<16f',uc.mem_read(0x675ed0,64))))
   def guard(uc,a,n,u):guards.append(dict(object=v.u32(current[0]+4),address=hex(a),reject=bool(uc.reg_read(UC_X86_REG_EFLAGS)&64)))
   v.uc.hook_add(UC_HOOK_CODE,obj,begin=0x40a8f0,end=0x40a8f0);v.uc.hook_add(UC_HOOK_CODE,draw,begin=0x56eb30,end=0x56eb30)
   for a in(0x56e006,0x56e036,0x56e05c,0x56e082,0x56e36a,0x56e395,0x56e3bb,0x56e3e1,0x56e6f1,0x56e71c,0x56e742,0x56e768):v.uc.hook_add(UC_HOOK_CODE,guard,begin=a,end=a)
   f.step();f.raster_enabled=False;f.render();f.step();v.checkpoint();rows=[];folder=output/name;folder.mkdir()
   for label,limits in(('original',(640.,480.)),('widened_constants_only',(6400.,4800.))):
    v.restore();guards.clear();v.write(0x5b9cc0,struct.pack('<f',limits[0]));v.write(0x5b9cbc,struct.pack('<f',limits[1]));f.raster_enabled=True;raw,record=f.render();image=folder/(label+'.png');save_rgb565_png(image,raw,1280,960);rows.append(dict(label=label,limits=limits,guards=list(guards),record=record,nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),image=str(image),image_sha256=sha(image.read_bytes())))
   assert rows[0]['record']==rows[1]['record'];assert any(g['reject']for g in rows[0]['guards'])and not any(g['reject']for g in rows[1]['guards']);assert rows[1]['nonzero_pixels']>rows[0]['nonzero_pixels'];cases.append(dict(name=name,type_id=id,asset_sha256=digest,capture_tick=2,state_rng_vertices_matrices_unchanged=True,controls=rows));print(name,rows[0]['nonzero_pixels'],rows[1]['nonzero_pixels'],flush=True)
 report=dict(status='diagnostic_negative_control',accepted=False,cases=cases,original_sha256=sha((ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe').read_bytes()),source_helper_sha256=sha((ROOT/'src/cometsoftwarebounds.h').read_bytes()),scope='Only original literalspanlimits640/480→6400/4800 changed in widenedbranch. The whole original default caller at tick 2 has identical draw packets, vertices, matrices, material states, and RNG. Wideningrestorestallbeam; notoriginalvisualacceptance. Narrowexistinghelper mayapplyonlyexplicitnative-domain softwareconfig; ordinarygeometryretained.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
