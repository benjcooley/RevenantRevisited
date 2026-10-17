"""Freeze Pulp original/actual Metal public-Set phases using logged state hashes."""
import argparse,json,struct,re,csv
from pathlib import Path
from PIL import Image,ImageDraw
from character_native_preflight import ROOT,sha
from software_probe import SoftwareFixture

def fnv(raw):
 h=1469598103934665603
 for x in raw:h=((h^x)*1099511628211)&0xffffffffffffffff
 return h

def run(native,metal,cold,state,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());c=json.loads((cold/'manifest.json').read_text());s=json.loads((state/'manifest.json').read_text())
 assert m['status']==c['status']==s['status']=='pass' and m['binary_sha256']==c['binary_sha256'] and m['exit_code']==c['exit_code']==0
 log=(metal/'run.log').read_text();cold_log=(cold/'run.log').read_text();assert '[pulp-phase]'not in cold_log and '[pulp-state] tick=0 'in cold_log
 hashes={int(t):(int(b,16),int(d,16))for t,b,d in re.findall(r'\[pulp-state\] tick=(\d+) body=([0-9a-f]+) blood=([0-9a-f]+)',log)};rng={int(t):int(r)for t,r in re.findall(r'\[pulp-phase\] tick=(\d+) rng=(\d+)',log)}
 for row in n['records']:
  t=row['tick'];st=row['state'];assert hashes[t]==(fnv(bytes.fromhex(st['body_hex'])),fnv(bytes.fromhex(st['blood_hex']))),(t,hashes[t])
  if t:assert rng[t]==st['rng_calls']
 assert c['scenario']['fps']==1000000 and c['scenario']['frames']==1 and len(c['frames'])==1
 by=dict(zip(m['sample_simulation_ticks'],m['frames']));by[0]=c['frames'][0];selected=[r for r in n['records']if 'image'in r and r['tick']in by]
 whole=Image.new('RGB',(1280,500*len(selected)),(18,18,18));detail=Image.new('RGB',(800,340*len(selected)),(18,18,18));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);cases=[];bounds=[]
 projector=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 for i,r in enumerate(selected):
  for d in r['draws']:
   vertices=[v[:3]for v in struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex']))];points=projector.project(vertices,d['matrix'],camera=n['camera']);xs=[p[0]for p in points];ys=[p[1]+160 for p in points];bounds.append([min(xs),min(ys),max(xs),max(ys)])
  paths=[Path(r['image']),Path(by[r['tick']]['path'])];images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for col,im in enumerate(images):
   label=f"tick {r['tick']} "+('original software'if col==0 else'Metal native-domain');whole.paste(im.resize((640,480)),(col*640,i*500+20));detail.paste(im.crop((440,390,840,710)),(col*400,i*340+20));wd.text((col*640+5,i*500+3),label,fill='white');dd.text((col*400+5,i*340+3),label,fill='white')
  cases.append(dict(tick=r['tick'],native_image=str(paths[0]),native_sha256=sha(paths[0]),metal_image=str(paths[1]),metal_sha256=sha(paths[1]),pixel_bounds=[im.getbbox()for im in images]))
 whole.save(out/'contact.png');detail.save(out/'detail.png');extent=[min(x[0]for x in bounds),min(x[1]for x in bounds),max(x[2]for x in bounds),max(x[3]for x in bounds)];assert extent[0]>=0 and extent[1]>=0 and extent[2]<1280 and extent[3]<960
 report=dict(status='awaiting_independent_review',accepted=False,visual_pass=False,type_id='0x152dafdd',binary_sha256=m['binary_sha256'],cases=cases,whole_geometry_extent=extent,whole_geometry_contained=True,actual_gpu_pool_hash_rows=len(n['records']),actual_gpu_rng_counts_exact=True,inputs={str(p):sha(p)for p in(native/'manifest.json',metal/'manifest.json',cold/'manifest.json',state/'manifest.json',metal/'frame-timing.csv')},contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),scope='Genuine Acolyte constructor/animator; actual public Set(velocity0,0,8; counts5/5/5), original empty terrain and ten failed blood-tile allocations. Declared GetTickCount0, including intrinsic rand/srand/rand MakeIndex side effects. Full original80-tick Pulse/Animate/Render/raster, exact source pools/RNG/ordered matrices. Cold Metal uses1microsecond step and proves zero simulation ticks; active Metal retains24Hz. Five settled chunks persist after blood expires; no invented lifetime. Source38/dir1, original Scene1, native-domain projection. Natural actor death caller, mapped tile success/live clock, sound/device, native light selection outside declared inputs and exact framebuffer pixels remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(len(cases),extent,report['binary_sha256'])
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--cold',type=Path,required=True);p.add_argument('--state',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.cold,a.state,a.output)
