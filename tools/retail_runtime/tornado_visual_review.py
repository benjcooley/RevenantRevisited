"""Freeze sampled whole Tornado original/actual Metal phases, without phase fitting."""
import argparse,csv,json,hashlib,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
from software_probe import SoftwareFixture
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native,metal,state,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());s=json.loads((state/'manifest.json').read_text());assert m['status']==s['status']=='pass' and m['exit_code']==0
 timing=list(csv.DictReader((metal/'frame-timing.csv').open()));by={round(float(r['simulation_seconds'])*24):f for r,f in zip(timing,m['frames'])};assert list(by)==m['sample_simulation_ticks']
 log=(metal/'run.log').read_text();phases=re.findall(r'\[tornado-phase\] tick=(\d+) rng=(\d+) flame_on=(\d+) first=\(([^)]+)\) scale=([^ ]+) alive=(\d+)',log);assert len(phases)==360
 error=0
 for t,rng,flame,pos,scale,alive in phases:
  r=n['records'][int(t)-1]['state'];a=struct.unpack_from('<4f',bytes.fromhex(r['tornado_hex']));assert [int(rng),int(flame),int(alive)]==[r['rng_calls'],r['flameon'],int(not r['flags']&0x1000)]
  error=max(error,max(abs(x-y)for x,y in zip([*map(float,pos.split(',')),float(scale)],a)))
 assert error<1e-5
 selected=[r for r in n['records']if 'image'in r and r['tick']in by];whole=Image.new('RGB',(1280,500*len(selected)),(18,18,18));detail=Image.new('RGB',(1280,520*len(selected)),(18,18,18));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);cases=[]
 projector=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 all_bounds=[]
 for i,r in enumerate(selected):
  bounds=[];zero=0
  for d in r['draws']:
   if max(abs(x)for x in d['matrix'][:12])<1e-8:zero+=1;continue
   vertices=[v[:3]for v in struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex']))];points=projector.project(vertices,d['matrix'],camera=n['camera']);xs=[p[0]for p in points];ys=[p[1]+160 for p in points];b=[min(xs),min(ys),max(xs),max(ys)];bounds.append(b);all_bounds.append(b)
  paths=[Path(r['image']),Path(by[r['tick']]['path'])];images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for col,im in enumerate(images):
   label=f"tick {r['tick']} "+('original software'if col==0 else'Metal native-domain');whole.paste(im.resize((640,480)),(col*640,i*500+20));detail.paste(im.crop((320,280,960,780)),(col*640,i*520+20));wd.text((col*640+6,i*500+3),label,fill='white');dd.text((col*640+6,i*520+3),label,fill='white')
  cases.append(dict(tick=r['tick'],native_image=str(paths[0]),native_sha256=sha(paths[0]),metal_image=str(paths[1]),metal_sha256=sha(paths[1]),projected_geometry_bounds=bounds,zero_scale_draws=zero,pixel_bounds=[im.getbbox()for im in images]))
 whole.save(out/'contact.png');detail.save(out/'detail.png');extent=[min(b[0]for b in all_bounds),min(b[1]for b in all_bounds),max(b[2]for b in all_bounds),max(b[3]for b in all_bounds)]
 report=dict(status='awaiting_independent_review',accepted=False,type_id='0x2784abde',fixture='Explicit exact Tornado owner associated with genuine named Tornado spell/caster/fighting target; natural variant effect remains Funnel',binary_sha256=m['binary_sha256'],inputs={str(p):sha(p)for p in(native/'manifest.json',metal/'manifest.json',state/'manifest.json',metal/'frame-timing.csv')},actual_gpu_phase_error=error,actual_gpu_rng_counts_exact=True,cases=cases,whole_geometry_extent=extent,whole_geometry_contained=extent[0]>=0 and extent[1]>=0 and extent[2]<1280 and extent[3]<960,contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),timing='CSV simulation ticks, initial unnumbered capture separate; all24Hz simulation/Render calls retained. Original raster executes only declared sampled phases after first10 all-frame control.',scope='Complete original constructors/owner/customPulse/baseAnimate/fullRender/resources; genuine actors not map-sector residents, real damage query empty four times. Original camera RAM/material-getter/owner-matrix boundaries; source38/dir1. Exact source core/pools/RNG/ordered matrices; native-domain original projection and existing RGB565/ARGB4444 policies. Natural cast, mapped damage recipients/player modifiers, audio/device, destination framebuffer precision and exact pixels remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(len(cases),error,extent,report['whole_geometry_contained'])
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--state',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.state,a.output)
