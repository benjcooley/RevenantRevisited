#!/usr/bin/env python3
"""Executed Labback mode16 cull negative control, unchanged geometry/light."""
import argparse,json,struct,zipfile,hashlib
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from labback_visual_reference import NativeLabback,ROOT
from software_probe import save_rgb565_png
def run(exe,archive,output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/labback.i3d'))
 cases=[]
 for cull in(1,2,3):
  f=NativeLabback(exe,data);f.initialize_scene();v=f.vm
  def force(uc,address,size,user):
   sp=uc.reg_read(UC_X86_REG_ESP)
   if v.u32(sp+4)==22:v.put_u32(sp+8,cull);f.render_states[22]=cull
  if cull!=1:v.uc.hook_add(UC_HOOK_CODE,force,begin=0x56d400,end=0x56d400)
  raw,r=f.render();path=output/f'cull-{cull}.png';save_rgb565_png(path,raw,640,480);count=sum(p!=(0,)for p in struct.iter_unpack('<H',raw));cases.append(dict(cull=cull,pixels=count,image=str(path),image_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),rgb565_sha256=hashlib.sha256(raw).hexdigest(),draws=r['draws']));print(cull,count,flush=True)
 report=dict(status='measured',accepted=False,cases=cases,scope='Only original D3D cull state overridden at56d400 for modes2/3; mode1 is unchanged original fullRender caller. Identical authored frame0/owner/light/texture; no geometry or raster bypass.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.archive,a.output)
