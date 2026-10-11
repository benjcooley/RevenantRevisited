#!/usr/bin/env python3
"""Whole generic multi-texture native/actual Metal comparisons; no fitted inputs."""
import argparse,hashlib,json
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()
def bounds(image):
 background=Image.new('RGB',image.size,image.getpixel((0,0)));return ImageChops.difference(image,background).getbbox()
def review(evidence,output):
 output.mkdir(parents=True,exist_ok=False);cases=[];binary=None
 for bg in ['black','ltgray']:
  source=evidence/('native-'+bg+'-actual-selector');n=json.loads((source/'manifest.json').read_text());width,height=n['viewport']
  for profile in n['profiles']:
   name=profile['name'];metal=evidence/'metal-final'/(name+'-'+bg);m=json.loads((metal/'manifest.json').read_text());assert m['status']=='pass'and m['exit_code']==0
   for k,v in dict(frames=60,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[640,480],rectangle=[0,0,1280,960],resolution=[1280,960],background=bg,native_domain=True,ambient=[32,255,255,255]).items():assert m['scenario'][k]==v
   binary=m['binary_sha256']if binary is None else binary;assert m['binary_sha256']==binary;log=(metal/'run.log').read_text();light=[l for l in log.splitlines()if '[vfx-source-light]'in l];assert len(light)==1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in light[0];assert '[generic-static-preview] actual owner id='+profile['id'][2:]in log
   rows=[c for c in n['cases']if c['name']==name];assert len(rows)==6;panel=Image.new('RGB',(width*2,6*(height+25)),(16,16,16));detail=Image.new('RGB',(1024,6*404),(16,16,16));pairs=[]
   for i,c in enumerate(rows):
    assert c['whole_geometry_unclipped']and c['draws']==(5 if name=='CharUtility'else 2);f=m['frames'][c['capture']-1];paths=[Path(c['image']),Path(f['path'])];assert sha(paths[0])==c['image_sha256']and sha(paths[1])==f['sha256'];images=[Image.open(p).convert('RGB')for p in paths];boxes=[bounds(im)for im in images];assert all(im.size==(width,height)for im in images)
    for box in boxes:assert box and 0<box[0]<box[2]<width-1 and 0<box[1]<box[3]<height-1
    for j,(im,label)in enumerate(zip(images,['Original software','Actual Metal'])):
     panel.paste(im,(j*width,i*(height+25)+25));ImageDraw.Draw(panel).text((j*width+4,i*(height+25)+4),f'{name} {bg} {label} capture{c["capture"]} state0frame{c["owner_frame"]}',fill='white');detail.paste(im.crop((384,288,896,672)),(j*512,i*404+20));ImageDraw.Draw(detail).text((j*512+4,i*404+3),f'{name} {bg} {label} capture{c["capture"]}',fill='white')
    dif=ImageChops.difference(*images).split();maximum=ImageChops.lighter(ImageChops.lighter(dif[0],dif[1]),dif[2]);pairs.append(dict(capture=c['capture'],owner_frame=c['owner_frame'],native=str(paths[0]),metal=str(paths[1]),sha256=[sha(p)for p in paths],geometry_bounds=c['geometry_bounds'],all_geometry_unclipped=True,effect_bounds=boxes,background_rgb=[im.getpixel((0,0))for im in images],whole_differing_pixels=width*height-maximum.histogram()[0]))
   whole=output/(name+'-'+bg+'-whole.png');diag=output/(name+'-'+bg+'-fixed-center.png');panel.save(whole);detail.save(diag);mp=evidence/'map-final'/name/'manifest.json';mapr=json.loads(mp.read_text());assert mapr['status']=='pass'and mapr['binary_sha256']==binary and mapr['completed_rows']==10 and mapr['created_visible']and mapr['floor_restored']
   cases.append(dict(name=name,type_id=profile['id'],background=bg,pairs=pairs,source_light_log=light[0],native_manifest=dict(path=str(source/'manifest.json'),sha256=sha(source/'manifest.json')),metal_manifest=dict(path=str(metal/'manifest.json'),sha256=sha(metal/'manifest.json')),map_manifest=dict(path=str(mp),sha256=sha(mp)),panels=dict(whole=dict(path=str(whole),sha256=sha(whole)),supplementary_fixed_center=dict(path=str(diag),sha256=sha(diag),rectangle=[384,288,896,672],scale=1))))
 report=dict(status='review_ready',accepted=False,cases=cases,binary_sha256=binary,probe_sha256=sha(__file__),scope='18 unchanged whole1280x960 state0 native default/owner/integer key/RenderObject/FVF_VERTEX/normal light565+unlit4444 versus actual typed-owner Metal samples. All5CharUtility roots/texture slots and bothDoor texture slots; allgeometry/effectbounds unclipped. Native normal-mode depthtest/write1 compared with existing native-domain helper depthtest/no-write; internalface/depth residual explicitlyopen, ordinary mapopaque path unchanged. Sameorigin/camera/light, fixed integer frames, no geometry/color/phase/projection fitting. CharUtility ltgray includes black authoredalpha shadow; destination565 versusRGBA8 clear quantization is explicit. Supplementary fixedcenter rectangle is no-scale and never replaces wholeframes. Natural actorutility/otherDoorstates/device/exactpixel gates remainopen.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.evidence,a.output);print(r['status'],len(r['cases']))
