#!/usr/bin/env python3
"""Unchanged whole native/actual Metal pairs with pinned lifecycle evidence."""
import argparse,json,hashlib,re
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(evidence,output):
 output.mkdir(parents=True,exist_ok=False);native=evidence/'native-water-full-v3';n=json.loads((native/'manifest.json').read_text());assert n['status']=='reference_complete'and n['independent_replays']==2;cases=[];binary=None;width,height=n['viewport']
 for p in n['profiles']:
  name=p['name'];mp=evidence/'metal-water-final'/name/'manifest.json';m=json.loads(mp.read_text());assert m['status']=='pass'and m['exit_code']==0
  for k,v in dict(frames=60,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[640,480],rectangle=[0,0,1280,960],resolution=[1280,960],background='black',native_domain=True,ambient=[32,255,255,255]).items():assert m['scenario'][k]==v
  binary=m['binary_sha256']if binary is None else binary;assert m['binary_sha256']==binary;log=(mp.parent/'run.log').read_text();assert '[native-domain] enabled=1 camera_z=0 owner=RzRxRyT '+name in log;light=[l for l in log.splitlines()if '[vfx-source-light]'in l];assert len(light)==1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in light[0]
  # World submission precedes late-frame Advance. Warmup1 excludes render1;
  # captured frame C is render C+1 at original tick C. Prove this with the
  # actual production RNG-count logs instead of selecting a visual match.
  rng={int(frame):int(draws)for frame,draws in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)};trace=json.loads((native/name/'state-trace.json').read_text());assert rng[1]==trace[0]['random_count']
  maps=evidence/'map-water-final'/name/'manifest.json';r=json.loads(maps.read_text());assert r['status']=='pass'and r['binary_sha256']==binary and r['completed_rows']==10 and r['created_visible']and r['floor_restored']and r['distinct_stationary_frames']>=4
  panel=Image.new('RGB',(2*width,6*(height+25)),(16,16,16));detail=Image.new('RGB',(1024,6*404),(16,16,16));pairs=[];rows=[c for c in n['cases']if c['name']==name];assert len(rows)==6
  for i,c in enumerate(rows):
   assert c['tick']==c['capture']and rng[c['capture']+1]==trace[c['tick']]['random_count'],'Actual capture phase/RNG differs from native lifecycle'
   assert c['whole_geometry_unclipped'];f=m['frames'][c['capture']-1];paths=[Path(c['image']),Path(f['path'])];assert sha(paths[0])==c['image_sha256']and sha(paths[1])==f['sha256'];images=[Image.open(x).convert('RGB')for x in paths];bounds=[im.getbbox()for im in images];assert all(im.size==(width,height)for im in images)
   assert all(b and 0<b[0]<b[2]<width-1 and 0<b[1]<b[3]<height-1 for b in bounds)
   for j,(im,label)in enumerate(zip(images,['Original software','Actual Metal'])):
    panel.paste(im,(j*width,i*(height+25)+25));ImageDraw.Draw(panel).text((j*width+4,i*(height+25)+4),f'{name} {label} capture{c["capture"]} tick{c["tick"]}',fill='white');detail.paste(im.crop((384,288,896,672)),(j*512,i*404+20));ImageDraw.Draw(detail).text((j*512+4,i*404+3),f'{name} {label} capture{c["capture"]}',fill='white')
   channels=ImageChops.difference(*images).split();maximum=ImageChops.lighter(ImageChops.lighter(channels[0],channels[1]),channels[2]);pairs.append(dict(capture=c['capture'],native_tick=c['tick'],production_before_render=c['capture']+1,production_rng_count=rng[c['capture']+1],native=str(paths[0]),metal=str(paths[1]),sha256=[sha(x)for x in paths],draws=c['draws'],geometry_bounds=c['geometry_bounds'],whole_geometry_unclipped=True,effect_bounds=bounds,whole_differing_pixels=width*height-maximum.histogram()[0]))
  whole=output/(name+'-whole.png');fixed=output/(name+'-fixed-center.png');panel.save(whole);detail.save(fixed);cases.append(dict(name=name,type_id=p['id'],pairs=pairs,source_light_log=light[0],native_manifest=dict(path=str(native/'manifest.json'),sha256=sha(native/'manifest.json')),metal_manifest=dict(path=str(mp),sha256=sha(mp)),map_manifest=dict(path=str(maps),sha256=sha(maps)),panels=dict(whole=dict(path=str(whole),sha256=sha(whole)),supplementary_fixed_center=dict(path=str(fixed),sha256=sha(fixed),rectangle=[384,288,896,672],scale=1))))
 result=dict(status='review_ready',accepted=False,cases=cases,binary_sha256=binary,probe_sha256=sha(__file__),scope='Twelve unchanged full1280x960 whole native factory/controller/pool/owner/normal-light/RGB565 additive software frames against independently executed actual Metal typed previews, source seed1/light/origin/camera/24Hz phase declared, allgeometry unclipped. Same-binary actual48frame map create/evolution/MOVE/delete/restoredfloor passes. Native custom CULLMODE1 none maps to existing two-sided helper, TestNoWrite and ONE/ONE retained. No scale/geometry/color/phase/projection fitting. Supplementary fixed crop is no-scale inspection only; exactpixels/quantization/device/naturalcaller gates remain open.');(output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.evidence,a.output)['status'])
