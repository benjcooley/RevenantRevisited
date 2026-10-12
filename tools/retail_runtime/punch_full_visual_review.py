"""Match complete actual PunchAndJudy owner loop/poses and selected whole images."""
import argparse,json,re,struct
from pathlib import Path
from PIL import Image,ImageDraw
from character_native_preflight import ROOT,sha
from software_probe import SoftwareFixture

def run(native,metal,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());assert m['status']=='pass' and m['exit_code']==0
 log=(metal/'run.log').read_text();phases={int(t):int(f)for t,f in re.findall(r'\[punch-phase\] tick=(\d+) frame=(\d+)',log)}
 poses={(int(t),int(o)):(int(f),[float(x)for x in values.split(',')])for t,f,o,values in re.findall(r'\[punch-pose\] tick=(\d+) frame=(\d+) object=(\d+) matrix=([^\n]+)',log)}
 maximum=0;fields=0
 for row in n['records']:
  t=row['tick'];assert row['state']['frame']==t%581 and row['state']['rng_calls']==0 and row['state']['controller_count']==0
  if not t:continue
  assert phases[t]==row['state']['frame'];assert len(row['draws'])==8
  for d in row['draws']:
   f,current=poses[(t,d['object'])];assert f==t%581 and len(current)==16
   # Declared equivalent relative port owner: translate to the original map
   # camera domain and round at the same float matrix store, not an image fit.
   current[12]=struct.unpack('<f',struct.pack('<f',current[12]+3185))[0];current[13]=struct.unpack('<f',struct.pack('<f',current[13]+3185))[0]
   delta=max(abs(a-b)for a,b in zip(current,d['matrix']));maximum=max(maximum,delta);fields+=16
   assert delta<.0003,(t,d['object'],delta,current,d['matrix'])
 by=dict(zip(m['sample_simulation_ticks'],m['frames']));selected=[r for r in n['records']if'image'in r and r['tick']in by]
 contact=Image.new('RGB',(1280,500*len(selected)),(18,18,18));detail=Image.new('RGB',(800,300*len(selected)),(18,18,18));dc=ImageDraw.Draw(contact);dd=ImageDraw.Draw(detail);cases=[];bounds=[]
 projector=SoftwareFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',1280,960)
 for r in n['records']:
  for d in r['draws']:
   points=projector.project([p[:3]for p in struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex']))],d['matrix'],camera=n['camera']);xs=[p[0]for p in points];ys=[p[1]+160 for p in points];bounds.append([min(xs),min(ys),max(xs),max(ys)])
 for i,r in enumerate(selected):
  paths=[Path(r['image']),Path(by[r['tick']]['path'])];images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for c,im in enumerate(images):
   label=f"tick {r['tick']} frame {r['state']['frame']} "+('original software'if c==0 else'Metal native-domain');contact.paste(im.resize((640,480)),(c*640,i*500+20));detail.paste(im.crop((440,440,840,720)),(c*400,i*300+20));dc.text((c*640+5,i*500+3),label,fill='white');dd.text((c*400+5,i*300+3),label,fill='white')
  actual_size=None
  if r['tick']in(100,300,500,580,581):
   actual_size=out/f'actual-size-{r["tick"]:03}.png';pair=Image.new('RGB',(800,280));pair.paste(images[0].crop((440,440,840,720)),(0,0));pair.paste(images[1].crop((440,440,840,720)),(400,0));pair.save(actual_size)
  cases.append(dict(tick=r['tick'],frame=r['state']['frame'],native_image=str(paths[0]),native_sha256=sha(paths[0]),metal_image=str(paths[1]),metal_sha256=sha(paths[1]),bounds=[im.getbbox()for im in images],actual_size=str(actual_size)if actual_size else None,actual_size_sha256=sha(actual_size)if actual_size else None))
 contact.save(out/'contact.png');detail.save(out/'detail.png');extent=[min(x[0]for x in bounds),min(x[1]for x in bounds),max(x[2]for x in bounds),max(x[3]for x in bounds)];assert extent[0]>=0 and extent[1]>=0 and extent[2]<1280 and extent[3]<960
 report=dict(status='awaiting_independent_review',accepted=False,visual_pass=False,type_id='0x0c05263a',binary_sha256=m['binary_sha256'],cases=cases,actual_gpu_pose_fields=fields,max_pose_error=maximum,actual_gpu_owner_frames=600,whole_geometry_extent=extent,whole_geometry_contained=True,inputs={str(p):sha(p)for p in(native/'manifest.json',metal/'manifest.json',metal/'frame-timing.csv')},contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),scope='Exact loaded PunchAndJudy owner/default animator, eight roots/twoRGB565 textures, source38/dir1 facing0 and explicit scene1. Complete600 original Pulse/Animate/PreRender/Render/Post/raster updates, full581-frame wrap and zeroRNG. Actual GPU fullinteger owner/ordered pose comparison; fixed1280x960 native-domain whole geometry. Natural caller, fractional animation/state transitions, terrain/device and exact framebuffer equality remain open.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(len(cases),fields,maximum,extent)
if __name__=='__main__':
 p=argparse.ArgumentParser();[p.add_argument('--'+x,type=Path,required=True)for x in('native','metal','output')];a=p.parse_args();run(a.native,a.metal,a.output)
