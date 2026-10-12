"""Freeze exact-tick whole ManaDrain/native GPU pairs and executed bounds."""
import argparse,csv,hashlib,json,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
from software_probe import SoftwareFixture
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native,metal,state,out):
 out.mkdir(parents=True,exist_ok=False)
 n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());s=json.loads((state/'manifest.json').read_text())
 assert m['status']==s['status']=='pass' and m['exit_code']==0 and len(m['frames'])==100
 timing=list(csv.DictReader((metal/'frame-timing.csv').open()));by={round(float(r['simulation_seconds'])*24):f for r,f in zip(timing,m['frames'])};assert list(by)==m['sample_simulation_ticks']==list(range(2,102))
 log=(metal/'run.log').read_text();phases=re.findall(r'\[manadrain-phase\] tick=(\d+) stage=(-?\d+) remaining=(-?\d+) amount=(\d+) scale=([^ ]+) mana=(\d+)/(\d+) alive=(\d+)',log)
 draws=re.findall(r'\[manadrain-draw\] tick=(\d+) rng=(\d+) first=\(([^)]+)\) scale=([^ ]+) rotation=([^ ]+) glow=([^\s]+)',log)
 assert len(phases)==91 and len(draws)==90
 phase_error=0
 for p in phases:
  r=n['records'][int(p[0])-1]['state'];owner=bytes.fromhex(r['owner_hex']);a=bytes.fromhex(r['animator_hex'])
  assert list(map(int,p[1:4]))==[struct.unpack_from('<i',owner,x)[0]for x in(20,24,16)]
  assert list(map(int,p[5:8]))==[r['caster_mana'],r['victim_mana'],int(not r['flags']&0x1000)]
  phase_error=max(phase_error,abs(float(p[4])-struct.unpack_from('<f',a,0x24)[0]))
 for d in draws:
  r=n['records'][int(d[0])-1]['state'];a=bytes.fromhex(r['animator_hex']);assert int(d[1])==r['rng_calls'];assert list(map(int,d[2].split(',')))==list(struct.unpack_from('<3i',a,0x64))
  phase_error=max(phase_error,abs(float(d[3])-struct.unpack_from('<f',a,0x24)[0]),abs(float(d[4])-struct.unpack_from('<f',a,0x20)[0]))
 assert phase_error<1e-6
 selected=[r for r in n['records']if 'image'in r and r['tick']in by];whole=Image.new('RGB',(1280,500*len(selected)),(18,18,18));detail=Image.new('RGB',(900,290*len(selected)),(18,18,18));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);cases=[]
 projector=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 for i,r in enumerate(selected):
  bounds=[];degenerate=0
  for draw in r['draws']:
   if max(abs(x)for x in draw['matrix'][:12])<1e-8:degenerate+=1;continue
   vertices=[v[:3]for v in struct.iter_unpack('<8f',bytes.fromhex(draw['vertices_hex']))];points=projector.project(vertices,draw['matrix'],camera=n['camera']);xs=[p[0]for p in points];ys=[p[1]+160 for p in points];bound=[min(xs),min(ys),max(xs),max(ys)];bounds.append(bound)
   assert 0<bound[0]<=bound[2]<1279 and 0<bound[1]<=bound[3]<959,(r['tick'],bound)
  paths=[Path(r['image']),Path(by[r['tick']]['path'])];images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for col,im in enumerate(images):
   label=f"tick {r['tick']} "+('original'if col==0 else'Metal');whole.paste(im.resize((640,480)),(col*640,i*500+20));detail.paste(im.crop((550,480,1000,750)),(col*450,i*290+20));wd.text((col*640+6,i*500+3),label,fill='white');dd.text((col*450+6,i*290+3),label,fill='white')
  cases.append(dict(tick=r['tick'],native_image=str(paths[0]),native_sha256=sha(paths[0]),metal_image=str(paths[1]),metal_sha256=sha(paths[1]),projected_geometry_bounds=bounds,zero_scale_draws=degenerate,pixel_bounds=[im.getbbox()for im in images]))
 whole.save(out/'contact.png');detail.save(out/'detail.png')
 report=dict(status='awaiting_independent_review',accepted=False,type_id='0xad92bc13',fixture='Mana Drain, genuine stationary Acolyte caster/fighting target, original62-point mana transfer',binary_sha256=m['binary_sha256'],executable_sha256=n['executable_sha256'],inputs={str(p):sha(p)for p in(native/'manifest.json',metal/'manifest.json',state/'manifest.json',metal/'frame-timing.csv')},actual_gpu_phase_error=phase_error,actual_gpu_rng_counts_exact=True,cases=cases,contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),timing='100 numbered captures correspond to actual CSV ticks2..101; contact sheet is not an extra simulation sample.',scope='Full original named owner/animator/render/resources in booted actor VM; standalone map residency association, executed base owner/pre matrix fragments, declared1280x960 camera3185,3185,0 centered640,640/source38+dir1, material getter and RAM output boundary. Actual native mana getters/setters and Spell::Kill execute. Port uses real source actors/loaded named variant and explicit core; arbitrary actor motion, natural map insertion/scheduling, audio/device and exact destination/framebuffer pixels remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(cases),phase_error);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--state',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.state,a.output)
