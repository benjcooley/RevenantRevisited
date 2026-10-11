#!/usr/bin/env python3
"""Unclipped whole original/actual Metal water panels, clocks, RNG and map proof."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()

def review(native,metal,maps,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());width,height=n['viewport'];cases=[];binary=None;state=1;counts={1:0}
 for count in range(1,10001):state=(state*214013+2531011)&0xffffffff;counts[state]=count
 for p in n['profiles']:
  name=p['name'];m=json.loads((metal/name/'manifest.json').read_text());s=m['scenario'];assert m['status']=='pass'and m['exit_code']==0
  for k,v in dict(frames=96,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[width//2,height//2],rectangle=[0,0,width,height],resolution=[width,height],background='black',native_domain=True,ambient=[32,255,255,255],partsys_quality=0).items():assert s[k]==v
  binary=m['binary_sha256']if binary is None else binary;assert binary==m['binary_sha256'];log=(metal/name/'run.log').read_text();light=[l for l in log.splitlines()if '[vfx-source-light]'in l];assert len(light)==1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in light[0]
  cadence=json.loads((native/name/'cadence.json').read_text());rng=[]
  for frame,draws in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log):
   tick=int(frame)-1;original=counts[cadence[tick]['rng_before']];assert original==int(draws),(name,frame,original,draws);rng.append(dict(before_render=int(frame),random_draws=original))
  assert rng
  init=json.loads((native/name/'initialization.json').read_text());assert init['initialization']['controllers'][0]['emitter_count']==6
  rows=[c for c in n['cases']if c['name']==name];assert len(rows)==6;sheet=Image.new('RGB',(width*2,(height+25)*6),(16,16,16));labels=ImageDraw.Draw(sheet);pairs=[]
  for row,c in enumerate(rows):
   assert c['whole_geometry_unclipped'];b=c['geometry_bounds'];assert 0<b[0]<b[2]<width-1 and 0<b[1]<b[3]<height-1
   q=m['frames'][c['capture']-1];paths=[Path(c['image']),Path(q['path'])];assert sha(paths[0])==c['image_sha256']and sha(paths[1])==q['sha256'];images=[Image.open(path).convert('RGB')for path in paths];assert all(im.size==(width,height)for im in images);boxes=[im.getbbox()for im in images]
   for box in boxes:assert box and 0<box[0]<box[2]<width-1 and 0<box[1]<box[3]<height-1
   for col,(im,label)in enumerate(zip(images,['Original software','Actual Metal'])):sheet.paste(im,(col*width,row*(height+25)+25));labels.text((col*width+4,row*(height+25)+4),f'{name} {label} capture{c["capture"]} ownerframe{c["owner_frame"]}',fill='white')
   diff=ImageChops.difference(*images);channels=diff.split();maximum=ImageChops.lighter(ImageChops.lighter(channels[0],channels[1]),channels[2]);different=width*height-maximum.histogram()[0]
   pairs.append(dict(capture=c['capture'],owner_frame=c['owner_frame'],cached_frame=c['cached_frame'],native=str(paths[0]),metal=str(paths[1]),sha256=[sha(path)for path in paths],bbox=boxes,native_geometry_bounds=b,all_geometry_unclipped=True,differing_pixels=different))
  panel=output/(name+'-native-metal.png');sheet.save(panel);mp=maps/name/'manifest.json';maprecord=json.loads(mp.read_text());assert maprecord['status']=='pass'and maprecord['binary_sha256']==binary and maprecord['created_visible']and maprecord['floor_restored']and maprecord['completed_rows']==10
  cases.append(dict(name=name,type_id=p['id'],pairs=pairs,panel=dict(path=str(panel),sha256=sha(panel)),rng_checks=rng,source_light_log=light[0],metal_manifest=dict(path=str(metal/name/'manifest.json'),sha256=sha(metal/name/'manifest.json')),map_manifest=dict(path=str(mp),sha256=sha(mp)),map_observations=10,map_frames=48))
 report=dict(status='review_ready',accepted=False,binary_sha256=binary,native_manifest=dict(path=str(native/'manifest.json'),sha256=sha(native/'manifest.json')),profiles=n['profiles'],cases=cases,probe_sha256=sha(__file__),viewport=[width,height],strict_pixel_parity=all(p['differing_pixels']==0 for c in cases for p in c['pairs']),scope='24 unchanged whole original PartSys/default/RenderObject/software versus actual typed-owner Metal frames: four exactRGB565/six-emitter assets, seed1/24Hz/warmup1, integer owner/cached-frame cadence and before-render RNG counts, sourceambient32 white/ground0 and fixed native-domain projection. Every emitted original quad and both imagebounds fit1280x960. Real map ground16 create/birth/evolution/move/delete/floor is separately pinned. No crops/registration/geometry/color/phase fitting; software/GPU quantization/edges/exactpixels/naturalcaller/device/depth remainopen.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for k in ['native','metal','maps','output']:p.add_argument('--'+k,type=Path,required=True)
 a=p.parse_args();r=review(a.native,a.metal,a.maps,a.output);print(r['status'],len(r['cases']))
