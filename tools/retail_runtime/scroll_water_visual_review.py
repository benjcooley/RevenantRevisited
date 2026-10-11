#!/usr/bin/env python3
"""Unfitted source-clock native/Metal water panels and separate map evidence."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageDraw
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()

def review(native,metal,maps,output,require_unclipped=False):
 output.mkdir(parents=True,exist_ok=False);reference=json.load(open(native/'manifest.json'));cases=[];lifecycle=[];binary=None;width,height=reference['viewport']
 for profile in reference['profiles']:
  name=profile['name'];capture=json.load(open(metal/name/'manifest.json'));scenario=capture['scenario'];assert capture['status']=='pass'
  for k,v in dict(frames=60,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[width//2,height//2],rectangle=[0,0,width,height],background='black',native_domain=True).items():assert scenario[k]==v
  binary=capture['binary_sha256']if binary is None else binary;assert capture['binary_sha256']==binary
  log=(metal/name/'run.log').read_text();clock={int(t):(int(g),int(f),float(u),float(v))for t,g,f,u,v in re.findall(r'\[scroll-water-clock\] tick=(\d+) global=(\d+) owner_frame=(\d+) du=([^ ]+) dv=([^\n]+)',log)}
  lights=[l for l in log.splitlines()if '[vfx-source-light]'in l];assert len(lights)==1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in lights[0]
  initial=json.load(open(native/name/'initialization.json'));controllers=initial['controllers'];assert len(controllers)<=1
  rates=controllers[0]['rates']if controllers else [0,0];first_uvs=None;first_offset=None
  sheet=Image.new('RGB',(2*width,6*(height+25)),(20,20,20));labels=ImageDraw.Draw(sheet)
  for row,c in enumerate(x for x in reference['cases']if x['name']==name):
   record=clock[c['capture']+1];assert record[:2]==(c['global_tick'],c['owner_frame'])
   for i in range(2):assert abs(record[i+2]-rates[i]*c['global_tick'])<1e-6
   # Every original copied vertex must change by the same independently logged source offset.
   if first_uvs is None:first_uvs=c['uvs'];first_offset=record[2:]
   assert len(c['uvs'])==len(first_uvs)
   for uv,first in zip(c['uvs'],first_uvs):
    for i in range(2):assert abs((uv[i]-first[i])-(record[i+2]-first_offset[i]))<1e-6
   files=[Path(c['image']),Path(capture['frames'][c['capture']-1]['path'])];assert sha(files[0])==c['image_sha256'];images=[Image.open(f).convert('RGB')for f in files]
   assert all(im.size==(width,height)for im in images)
   bounds=c.get('projected_geometry_bounds');unclipped=bool(bounds and 0<bounds[0]<bounds[2]<width-1 and 0<bounds[1]<bounds[3]<height-1)
   if require_unclipped:
    assert unclipped
    for im in images:
     box=im.getbbox();assert box and 0<box[0]<box[2]<width-1 and 0<box[1]<box[3]<height-1
   for col,(im,label)in enumerate(zip(images,['Original software','Actual Metal'])):sheet.paste(im,(col*width,row*(height+25)+25));labels.text((col*width+5,row*(height+25)+5),f'{name} {label} capture{c["capture"]} global{c["global_tick"]}',fill='white')
   cases.append(dict(name=name,type_id=profile['id'],capture=c['capture'],global_tick=c['global_tick'],owner_frame=c['owner_frame'],source_uv_offset=list(record[2:]),native=str(files[0]),metal=str(files[1]),sha256=[sha(f)for f in files],bbox=[im.getbbox()for im in images],projected_geometry_bounds=bounds,whole_geometry_unclipped=unclipped,pixel_sha256=[hashlib.sha256(im.tobytes()).hexdigest()for im in images],differing_pixels=sum(a!=b for a,b in zip(images[0].getdata(),images[1].getdata())),source_light_log=lights[0],clock_equal=True))
  sheet.save(output/(name+'-native-metal.png'))
  mpath=maps/name/'manifest.json';m=json.load(open(mpath));assert m['status']=='pass'and m['binary_sha256']==binary and m['floor_restored']and m['created_visible'];lifecycle.append(dict(name=name,manifest=str(mpath),sha256=sha(mpath),observations=m['completed_rows'],frames=len(m['frames']),distinct_stationary=m['distinct_stationary_animation_frames'],floor_restored=True))
 report=dict(status='review_ready',accepted=False,binary_sha256=binary,native_manifest=dict(path=str(native/'manifest.json'),sha256=sha(native/'manifest.json')),cases=cases,map_lifecycle=lifecycle,probe_sha256=sha(__file__),strict_pixel_parity=all(c['differing_pixels']==0 for c in cases),scope=f'{len(cases)} whole original controller/object/ARGB4444 software-raster versus actual typed-owner Metal samples. Exact assets, global clock/frame, fixed origin/source light/native-domain projector. Full{width}x{height} panels, no crops/image registration/geometry/color/phase fitting. Original software/GPU coverage and quantization differences retained. Real map create/frame/move/delete/restored floor is separate; original natural water placement/device/collision/scene acceptance open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for k in ['native','metal','maps','output']:p.add_argument('--'+k,type=Path,required=True)
 p.add_argument('--require-unclipped',action='store_true');a=p.parse_args();r=review(a.native,a.metal,a.maps,a.output,a.require_unclipped);print(r['status'],len(r['cases']))
