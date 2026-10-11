#!/usr/bin/env python3
"""Whole unchanged Flies/Snow original versus actual Metal visual panels."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()
def bbox(image):return ImageChops.difference(image,Image.new('RGB',image.size,image.getpixel((0,0)))).getbbox()
def run(evidence,output):
 output.mkdir(parents=True,exist_ok=False);cases=[];binary=None
 for name,native_folder,metal_folder,bg,frames in [('Flies','native-flies-full-v2','metal-flies-final-v3','ltgray',90),('Snow','native-snow-full','metal-snow-final','black',60)]:
  np=evidence/native_folder/'manifest.json';mp=evidence/metal_folder/'manifest.json';n=json.loads(np.read_text());m=json.loads(mp.read_text());assert n['status']=='reference_complete'and n['independent_replays']==2 and m['status']=='pass'and m['exit_code']==0
  for k,v in dict(frames=frames,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[640,480],rectangle=[0,0,1280,960],resolution=[1280,960],background=bg,native_domain=True,ambient=[32,255,255,255]).items():assert m['scenario'][k]==v
  binary=m['binary_sha256']if binary is None else binary;assert binary==m['binary_sha256'];map_path=evidence/'map-final'/name/'manifest.json';mapped=json.loads(map_path.read_text());assert mapped['status']=='pass'and mapped['binary_sha256']==binary and mapped['completed_rows']==10 and mapped['created_visible']and mapped['floor_restored'];log=(mp.parent/'run.log').read_text();rng={int(a):int(b)for a,b in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
  if name=='Flies':trace=json.loads((np.parent/'state-trace.json').read_text());assert rng[1]==trace[0]['random_count']
  else:assert '[generic-static-preview] actual owner id=facedbed'in log and 'meshes=50 exact_profile=1'in log
  panel=Image.new('RGB',(2560,6*985),(16,16,16));fixed=Image.new('RGB',(1024,6*404),(16,16,16));pairs=[]
  for i,c in enumerate(n['cases']):
   if name=='Flies':assert c['tick']==c['capture']+1 and (c['capture']+1 not in rng or rng[c['capture']+1]==trace[c['capture']]['random_count'])
   f=m['frames'][c['capture']-1];paths=[Path(c['image']),Path(f['path'])];assert sha(paths[0])==c['image_sha256']and sha(paths[1])==f['sha256'];images=[Image.open(p).convert('RGB')for p in paths];boxes=[bbox(im)for im in images];assert all(im.size==(1280,960)for im in images)and c['whole_geometry_unclipped'];assert all(b and 0<b[0]<b[2]<1279 and 0<b[1]<b[3]<959 for b in boxes)
   for j,(im,title)in enumerate(zip(images,['Original software','Actual Metal'])):
    panel.paste(im,(1280*j,985*i+25));ImageDraw.Draw(panel).text((1280*j+5,985*i+5),f'{name} {title} capture{c["capture"]}',fill='white');fixed.paste(im.crop((384,288,896,672)),(512*j,404*i+20));ImageDraw.Draw(fixed).text((512*j+4,404*i+3),f'{name} {title} capture{c["capture"]}',fill='white')
   pairs.append(dict(capture=c['capture'],native_tick=c.get('tick'),native_owner_frame=c.get('owner_frame'),native=str(paths[0]),metal=str(paths[1]),sha256=[sha(p)for p in paths],draws=c['draws'],geometry_bounds=c['geometry_bounds'],whole_geometry_unclipped=True,effect_bounds=boxes,background_rgb=[im.getpixel((0,0))for im in images]))
  whole=output/(name+'-whole.png');detail=output/(name+'-fixed-center.png');panel.save(whole);fixed.save(detail);case=dict(name=name,type_id=n['id'],pairs=pairs,native_manifest=dict(path=str(np),sha256=sha(np)),metal_manifest=dict(path=str(mp),sha256=sha(mp)),map_manifest=dict(path=str(map_path),sha256=sha(map_path)),panels=dict(whole=dict(path=str(whole),sha256=sha(whole)),supplementary_fixed_center=dict(path=str(detail),sha256=sha(detail),rectangle=[384,288,896,672],scale=1)))
  if name=='Flies':
   controls=[]
   for capture in(1,):
    image=Image.open(m['frames'][capture-1]['path']).convert('RGB');assert bbox(image)is None;controls.append(dict(capture=capture,tick=2,draws=0,image=m['frames'][capture-1]['path'],sha256=m['frames'][capture-1]['sha256']))
   case['native_empty_controls']=n['controls'];case['metal_empty_controls']=controls
  cases.append(case)
 report=dict(status='review_ready',accepted=False,cases=cases,binary_sha256=binary,probe_sha256=sha(__file__),scope='Twelve unchanged full1280x960 whole native/actualMetal pairs; full ctor/base/pool/original matrix/light/blend/raster and typed own Metal pipeline. Native geometry and observed effect bounds unclipped. Flies actual sparse dark8x8 sprite20-pool24Hz staging/wander; gray565 versusRGBA8 backdrop quantization and black visibility limitation explicit, firstempty phase proved. Snow default50-root static authored ice mesh, independently bound from Iced actor/shatter; alllegacy posechannels verified separately. Samebinary realmap create/MOVE/delete passes. Existing helper depthwrite residual and GPU/software pixel/quantization/naturalcaller/nativehardware/fullacceptance remainopen; no geometry/color/phase/projection fitting.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],sha(output/'manifest.json'));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.evidence,a.output)
