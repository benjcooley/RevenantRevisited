"""Freeze complete Dragonfire lifecycle reference/current pixels and phase proof."""
import argparse,json,struct,re
from pathlib import Path
from PIL import Image,ImageDraw
from character_native_preflight import ROOT,sha
from software_probe import SoftwareFixture

def pool_hash(raw):
 h=1469598103934665603
 for slot in range(300):
  p=raw[slot*88:(slot+1)*88];used=struct.unpack_from('<i',p,84)[0]
  for x in p[84:88]+(p[:72]+p[76:84]if used else b''):h=((h^x)*1099511628211)&0xffffffffffffffff
 return h

def run(native,metal,state,pose,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());s=json.loads((state/'manifest.json').read_text());p=json.loads((pose/'manifest.json').read_text());assert m['status']==s['status']==p['status']=='pass' and m['exit_code']==0
 log=(metal/'run.log').read_text();phases={int(t):(int(fr),int(alive),int(rng),int(fire,16),int(smoke,16))for t,fr,alive,rng,fire,smoke in re.findall(r'\[dragonfire-phase\] tick=(\d+) frame=(\d+) alive=(\d+) rng=(\d+) fire=([0-9a-f]+) smoke=([0-9a-f]+)',log)}
 for r in n['records']:
  t=r['tick'];st=r['state']
  if t not in phases:assert st['flags']&0x1000;continue
  expected=(st['frame'],int(not(st['flags']&0x1000)),st['rng_calls'],*(pool_hash(bytes.fromhex(st['pools'][x]))for x in('fire','smoke')))
  assert phases[t]==expected,(t,phases[t],expected)
 by=dict(zip(m['sample_simulation_ticks'],m['frames']));selected=[r for r in n['records']if 'image'in r and r['tick']in by]
 contact=Image.new('RGB',(1280,500*len(selected)),(18,18,18));detail=Image.new('RGB',(960,420*len(selected)),(18,18,18));dc=ImageDraw.Draw(contact);dd=ImageDraw.Draw(detail);cases=[];bounds=[]
 projector=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 for i,r in enumerate(selected):
  for draw in r['draws']:
   vertices=[v[:3]for v in struct.iter_unpack('<8f',bytes.fromhex(draw['vertices_hex']))];points=projector.project(vertices,draw['matrix'],camera=n['camera']);xs=[q[0]for q in points];ys=[q[1]+160 for q in points];bounds.append([min(xs),min(ys),max(xs),max(ys)])
  paths=[Path(r['image']),Path(by[r['tick']]['path'])];images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for c,im in enumerate(images):
   label=f"tick {r['tick']} "+('original software'if c==0 else'Metal native-domain');contact.paste(im.resize((640,480)),(c*640,i*500+20));detail.paste(im.crop((420,300,900,700)),(c*480,i*420+20));dc.text((c*640+5,i*500+3),label,fill='white');dd.text((c*480+5,i*420+3),label,fill='white')
  cases.append(dict(tick=r['tick'],native_image=str(paths[0]),native_sha256=sha(paths[0]),metal_image=str(paths[1]),metal_sha256=sha(paths[1]),bounds=[im.getbbox()for im in images]))
 contact.save(out/'contact.png');detail.save(out/'detail.png');extent=[min(x[0]for x in bounds),min(x[1]for x in bounds),max(x[2]for x in bounds),max(x[3]for x in bounds)];assert extent[0]>=0 and extent[1]>=0 and extent[2]<1280 and extent[3]<960
 report=dict(status='awaiting_independent_review',accepted=False,visual_pass=False,type_id='0xad92bc1a',binary_sha256=m['binary_sha256'],cases=cases,whole_geometry_extent=extent,whole_geometry_contained=True,actual_gpu_pool_rng_rows=len(phases),inputs={str(p):sha(p)for p in(native/'manifest.json',metal/'manifest.json',state/'manifest.json',pose/'manifest.json',metal/'frame-timing.csv')},contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),scope='Exact DragonFire null-spell owner and two300-slot pools, source seed1/facing0/ambient38/dir1. Original full100-update lifecycle with34emission ticks and kill93, every Render/raster retained; actual compiled state/RNG and all19626 ordered matrices proven. No FireCone controller alias or snapshot position offsets. Uninitialized flicker ignored by Render(false,false). Whole fixed1280x960 native-domain view; common projection, live-spell/caster facing, audio/device and exact framebuffer pixels remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(len(cases),extent)
if __name__=='__main__':
 p=argparse.ArgumentParser();[p.add_argument('--'+x,type=Path,required=True)for x in('native','metal','state','pose','output')];a=p.parse_args();run(a.native,a.metal,a.state,a.pose,a.output)
