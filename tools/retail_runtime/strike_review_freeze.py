#!/usr/bin/env python3
"""Freeze phase-matched Strike whole/fixed-view pairs; this is not approval."""
import argparse,hashlib,json,struct
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont,ImageChops
from strike_whole_views import CENTERS,SAMPLES
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def load(p):return json.loads(Path(p).read_text())
FONT=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',28)
def pair(a,b,label,path):
 canvas=Image.new('RGB',(a.width+b.width,a.height+40));canvas.paste(a,(0,40));canvas.paste(b,(a.width,40));ImageDraw.Draw(canvas).text((8,4),label,fill='white',font=FONT);canvas.save(path);return canvas
def contact(panels,path,width=1920,columns=2):
 tiles=[]
 for p in panels:
  p=p.copy();p.thumbnail((width//columns,800));tiles.append(p)
 h=max(p.height for p in tiles);result=Image.new('RGB',(width,h*((len(tiles)+columns-1)//columns)))
 for i,p in enumerate(tiles):result.paste(p,((i%columns)*(width//columns),(i//columns)*h))
 result.save(path)
def run(base,out):
 out.mkdir(parents=True,exist_ok=False);npath=base/'strike-manager-sampled/manifest.json';n=load(npath);mpath=base/'strike-metal-frozen/manifest.json';m=load(mpath);wpath=base/'strike-whole-native-fixed/manifest.json';w=load(wpath)
 assert m['status']=='pass';binary=m['binary_sha256'];pins=[];panels=[];details=[];pairs=[]
 def pin(p):pins.append(dict(path=str(p),sha256=sha(p)))
 for row in n['records']:
  if 'image'not in row:continue
  t=row['tick'];assert m['sample_simulation_ticks'][t-1]==t
  a=Image.open(row['image']).convert('RGB');p=m['frames'][t-1]['path'];b=Image.open(p).convert('RGB');assert a.size==b.size==(1920,1080);pin(row['image']);pin(p)
  panel=pair(a,b,f'Tick {t}: original | Metal',out/f'pair-{t:03}.png');panels.append(panel);pairs.append(dict(tick=t,native=row['image'],metal=p))
  box=ImageChops.lighter(a,b).getbbox()
  if box:
   x0,y0,x1,y1=box;box=(max(0,x0-12),max(0,y0-12),min(1920,x1+12),min(1080,y1+12));details.append(pair(a.crop(box),b.crop(box),f'Tick {t}: same-pixel crop',out/f'detail-{t:03}.png'))
 contact(panels,out/'contact.png');contact(details,out/'detail.png',1600)
 views={};whole=[];tiles=[];overlap=[]
 for y in CENTERS:
  p=base/f'strike-frozen-y{y}/manifest.json';v=load(p);assert v['status']=='pass'and v['binary_sha256']==binary;views[y]=v;pin(p)
 for row in w['records']:
  if not row['views']:continue
  t=row['tick'];canvases=[Image.new('RGB',(1920,6840))for _ in range(2)];previous=None
  for q in row['views']:
   y=q['center'][1];v=views[y];index=v['sample_simulation_ticks'].index(t);p=v['frames'][index]['path'];a=Image.open(q['image']).convert('RGB');b=Image.open(p).convert('RGB');assert a.size==b.size==(1920,1080);pin(q['image']);pin(p)
   if previous:
    overlap.append(dict(tick=t,centers=[previous[0],y],native_different_pixels=sum(x!=(0,0,0)for x in ImageChops.difference(previous[1].crop((0,960,1920,1080)),a.crop((0,0,1920,120))).getdata()),metal_different_pixels=sum(x!=(0,0,0)for x in ImageChops.difference(previous[2].crop((0,960,1920,1080)),b.crop((0,0,1920,120))).getdata())))
   previous=y,a,b
   for canvas,image in zip(canvases,(a,b)):canvas.paste(image,(0,6460-y))
   box=ImageChops.lighter(a,b).getbbox()
   if box:
    x0,y0,x1,y1=box;box=max(0,x0-12),max(0,y0-12),min(1920,x1+12),min(1080,y1+12);tiles.append(pair(a.crop(box),b.crop(box),f'Tick {t}, centerY {y}: original | Metal',out/f'wide-detail-{t:03}-y{y}.png'))
  whole.append(pair(*canvases,f'Tick {t}: seven fixed views, original | Metal',out/f'whole-{t:03}.png'))
 contact(whole,out/'whole-contact.png',2400,3);contact(tiles,out/'whole-detail.png',1920,2)
 # Conservative all100-phase submitted-geometry coverage. Constants are the
 # independently executed original view/projection, not measured image fits.
 bounds=[]
 for row in n['records']:
  points=[]
  for q in row['draws']:
   world=q['matrix'];vertices=[]
   for v in struct.iter_unpack('<8f',bytes.fromhex(q['vertices_hex'])):
    x,y,z=[sum(v[k]*world[k*4+j]for k in range(3))+world[12+j]for j in range(3)];x-=3185;y-=3185
    vertices.append((960+1.0101525783538818*(x-y),700+.5050762891769409*(x+y)-1.237179160118103*z))
   indices=struct.unpack('<'+'H'*(len(q['indices_hex'])//4),bytes.fromhex(q['indices_hex']))
   for i in range(0,len(indices),3):
    triangle=[vertices[k]for k in indices[i:i+3]]
    if all(abs(triangle[0][0]-p[0])<=640 and abs(triangle[0][1]-p[1])<=480 for p in triangle[1:]):points.extend(triangle)
  if points:
   box=[min(p[0]for p in points),min(p[1]for p in points),max(p[0]for p in points),max(p[1]for p in points)];assert box[0]>1 and box[1]>-5759 and box[2]<1919 and box[3]<1079,box;bounds.append(dict(tick=row['tick'],projected_bounds=box))
 for p in (npath,mpath,wpath,base/'strike-actual-particles-frozen.json',base/'strike-pose-x87/manifest.json',base/'strike-particles-final/manifest.json',base/'strike-comet-default-control/manifest.json'):pin(p)
 report=dict(status='ready_for_independent_review',accepted=False,visual_pass=False,type_id='0xad92bd32',source_sha256=sha(__file__),binary_sha256=binary,whole_pairs=pairs,additional_whole_phases=list(SAMPLES),fixed_centers=list(CENTERS),coverage=dict(width=1920,height=6840,all100_submitted_accepted_bounds=bounds),overlap_differences=overlap,pins=pins,scope='Named Meteor Storm, genuine Acolytes and active empty-map allocation; four real timed owners with Comet override. 24Hz all original Pulse/Animate/Render and RNG preserved. Seven fixed views include genuine initial world-dependent fast particles. No natural scheduler/populated damage/moving actors/audio/hardware/exact-pixel certification.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('ready',len(pairs),len(pins),binary)
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.base,a.output)
