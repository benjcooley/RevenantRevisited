#!/usr/bin/env python3
"""Full-rate default Vortex Render with six selected whole native raster frames."""
import argparse,json,struct,zipfile
from pathlib import Path
from vortex_native_preflight import NativeVortex,ROOT,ASSET_SHA
from software_probe import save_rgb565_png
from speed_profile_contract import sha

def run(output):
 output.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/vortex.i3d'))
 f=NativeVortex(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);rows=[]
 for tick in range(1,61):
  f.vm.put_u32(0x65cb38,tick);f.raster_enabled=tick in(2,6,16,30,46,60);raw,draw=f.render_default();r=dict(tick=tick,owner_frame=0,raster_executed=f.raster_enabled,draw=draw,rng_after=f.vm.rng_seed)
  if f.raster_enabled:
   assert len(draw['draws'])==3;bounds=[d['geometry_bounds']for d in draw['draws']];assert all(0<b[0]<b[2]<1279 and 0<b[1]<b[3]<959 for b in bounds);p=output/f'native-{tick:03d}.png';save_rgb565_png(p,raw,1280,960);r.update(image=str(p),image_sha256=sha(p.read_bytes()),rgb565_sha256=sha(raw),nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw)),whole_geometry_unclipped=True)
  rows.append(r)
 report=dict(status='reference_complete',accepted=False,name='Vortex',type_id='0x452dade0',asset_sha256=ASSET_SHA,constructor=f.constructor,dispatch=f.dispatch,full_constructors=93,cases=rows,source_ambient=[38]*3,directional=[1]*3,origin=[0,0,0],camera=[0,0,0],viewport=[1280,960],scope='Literal shipped Vortex default full animatorfactory/all93constructors/Initialize; everytick real owner-matrix/keycache/RenderObject/material/cull/texture dispatch over3roots, six selected full original software frames and explicitdevicehandoff on remainingticks. Single authored state/frame, no registered customVortex/controller. Immutable legacyprovider/singleARGB4444COM/normal ambient38dir1/cull2/depthwrite boundaries; no hostgeometry/pose/lighting/raster fit. Owner lifecycle independently validated on real port; natural/device/exactpixels separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.output)['status'])
