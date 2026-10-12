"""Freeze genuine loaded sgeyser / actual Metal lifecycle pairs, not approval."""
import argparse,json,hashlib,struct
from pathlib import Path
from PIL import Image,ImageChops,ImageDraw,ImageFont
FONT=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',24)
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(base,out):
 out.mkdir(parents=True,exist_ok=False);np=base/'sgeyser-native-whole/manifest.json';mp=base/'sgeyser-metal-whole/manifest.json';n=json.loads(np.read_text());m=json.loads(mp.read_text());assert m['status']=='pass';pins=[];panels=[];details=[];pairs=[];bounds=[]
 def pin(p):pins.append(dict(path=str(p),sha256=sha(p)))
 def pair(a,b,t,path):
  c=Image.new('RGB',(max(400,a.width+b.width),a.height+34));c.paste(a,(0,34));c.paste(b,(a.width,34));ImageDraw.Draw(c).text((5,3),f'Tick{t}: original | Metal',fill='white',font=FONT);c.save(path);return c
 def contact(tiles,path,width):
  rows=[];columns=3;cell=width//columns
  for t in tiles:t=t.copy();t.thumbnail((cell,800));rows.append(t)
  height=max(t.height for t in rows);c=Image.new('RGB',(width,height*((len(rows)+columns-1)//columns)))
  for i,t in enumerate(rows):c.paste(t,((i%columns)*cell,(i//columns)*height))
  c.save(path)
 for row in n['rows']:
  points=[]
  for q in row['draws']:
   w=q['matrix']
   for v in struct.iter_unpack('<8f',bytes.fromhex(q['vertices_hex'])):
    x,y,z=[sum(v[k]*w[k*4+j]for k in range(3))+w[12+j]for j in range(3)];x-=1019;y-=11935;points.append((640+1.0101525783538818*(x-y),640+.5050762891769409*(x+y)-1.237179160118103*z))
  if points:
   box=[min(p[0]for p in points),min(p[1]for p in points),max(p[0]for p in points),max(p[1]for p in points)];assert box[0]>0 and box[1]>0 and box[2]<1280 and box[3]<960,box;bounds.append(dict(tick=row['tick'],bounds=box))
  if not row['image_sha256']:continue
  t=row['tick'];assert m['sample_simulation_ticks'][t-1]==t;metal=m['frames'][t-1]['path'];a=Image.open(row['image']).convert('RGB');b=Image.open(metal).convert('RGB');assert a.size==b.size==(1280,960);pin(row['image']);pin(metal);pairs.append(dict(tick=t,native=row['image'],metal=metal));panels.append(pair(a,b,t,out/f'pair-{t:03}.png'))
  box=ImageChops.lighter(a,b).getbbox()
  if box:
   x0,y0,x1,y1=box;box=(max(0,x0-12),max(0,y0-12),min(1280,x1+12),min(960,y1+12));details.append(pair(a.crop(box),b.crop(box),t,out/f'detail-{t:03}.png'))
 contact(panels,out/'contact.png',1920);contact(details,out/'detail.png',1500)
 for p in (np,mp,base/'sgeyser-particles-final/manifest.json',base/'sgeyser-actual-whole.json',base/'sgeyser-pose-first/manifest.json',base/'sgeyser-gallery-final/manifest.json',base/'sgeyser-observer-controls.json'):pin(p)
 result=dict(status='ready_for_independent_review',accepted=False,visual_pass=False,type_id='0xad92bd30',binary_sha256=m['binary_sha256'],pairs=pairs,pins=pins,complete_submitted_bounds=bounds,source_sha256=sha(__file__),scope='Genuine generic factory at declared saved placement1019,11935,289; one104-slot inlinecontroller and original failed vapor open. Post-init seed1, quality0, empty ground0, fixed1280x960 center(640,640), camera(sourceXY,0), original full Pulse/Animate/Render everytick through45kill;120sampledGPUframes. Natural trap/map layers/audio/device/exactpixels not certified.')
 (out/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print('ready',len(pairs),len(pins),m['binary_sha256'])
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.base,a.output)
