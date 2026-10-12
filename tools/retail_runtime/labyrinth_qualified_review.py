"""Unclipped whole qualified-native/actualGPU phases plus lifecycle/cadence checks."""
import argparse,json,re,struct,zipfile
from pathlib import Path
from PIL import Image,ImageDraw
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha

def run(native,metal,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());assert n['patched_original']and not n['widened_raster_constants']and m['status']=='pass';assert(n['width'],n['height'],n['center'])==(1920,1080,[960,600]);pairs=[];panels=[];errors=[];checks=0;roi=(470,25,1450,1045)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_asset(z.read('Imagery/Misc/starfield.i3d'))
 vertices=[list(struct.iter_unpack('<8f',bytes.fromhex(o['vertex_hex'])))for o in asset['objects']];geometry=[]
 for r in n['rows']:
  points=[]
  for d in r['draws']:
   w=d['world']
   for v in vertices[d['object']]:
    x,y,z=[sum(v[k]*w[k*4+j]for k in range(3))+w[12+j]for j in range(3)];points.append((960+x-y,600+.5050762891769409*(x+y)-1.237179160118103*z))
  box=[min(p[0]for p in points),min(p[1]for p in points),max(p[0]for p in points),max(p[1]for p in points)];geometry.append(dict(tick=r['tick'],bbox=box));assert 0<box[0]<box[2]<1920 and 0<box[1]<box[3]<1080,geometry[-1]
 for case in m['cases']:
  t=case['tick'];r=n['rows'][t-1];p=Path(case['path']);assert sha(p.read_bytes())==case['sha256'];c=json.loads(p.read_text());assert c['status']=='pass'and c['sample_simulation_ticks']==[t]and c['binary_sha256']==m['binary_sha256'];f=c['frames'][0];paths=[Path(r['image']),Path(f['path'])];hashes=[r['image_sha256'],f['sha256']];assert[sha(p.read_bytes())for p in paths]==hashes;images=[Image.open(p).convert('RGB')for p in paths];boxes=[im.getbbox()for im in images];assert all(b and 0<b[0]<b[2]<1920 and 0<b[1]<b[3]<1080 for b in boxes);whole=Image.new('RGB',(3840,1105));fixed=Image.new('RGB',(1960,1045))
  for i,im in enumerate(images):whole.paste(im,(i*1920,25));fixed.paste(im.crop(roi),(i*980,25));ImageDraw.Draw(whole).text((i*1920+4,5),f'{"Qualified original"if i==0 else"Actual Metal"} Labyrinth tick{t}',fill='white');ImageDraw.Draw(fixed).text((i*980+4,5),f'{"Mod"if i==0 else"GPU"} tick{t} samefixedROI',fill='white')
  wp=out/f'whole-{t:03d}.png';cp=out/f'fixed-{t:03d}.png';whole.save(wp);fixed.save(cp);panels.append(dict(tick=t,whole=str(wp),whole_sha256=sha(wp.read_bytes()),crop=str(cp),crop_sha256=sha(cp.read_bytes())));pairs.append(dict(tick=t,images=list(map(str,paths)),sha256=hashes,bbox=boxes,draws=len(r['draws']),frame=r['owner_frame']))
  rows=re.findall(r'\[labyrinth-qualified-phase\] tick=(\d+) draws=(\d+) first_radius=([0-9.eE+-]+) first_angle=([0-9.eE+-]+) owner_frame=(\d+)',(p.parent/'run.log').read_text());assert len(rows)==t
  for tick,draws,radius,angle,frame in rows:
   tick=int(tick);nr=n['rows'][tick-1];state=struct.unpack('<600f',bytes.fromhex(nr['state_hex'])[:2400]);checks+=5
   if int(draws)!=len(nr['draws'])or int(frame)!=nr['owner_frame']or abs(float(radius)-state[0])>2e-5 or abs(float(angle)-state[150])>2e-5:errors.append(dict(case=t,tick=tick))
 assert not errors;r=dict(status='awaiting_independent_qualified_review',accepted=False,unmodified_visual_pass=False,type_id='0xad92bd20',patch=n['patch'],binary_sha256=m['binary_sha256'],pairs=pairs,panels=panels,geometry_bounds_all170=geometry,full_phase_checks=checks,errors=errors,source_fitted=False,native_manifest_sha256=sha((native/'manifest.json').read_bytes()),metal_manifest_sha256=sha((metal/'manifest.json').read_bytes()),scope='Complete150stars/trails plusauthoredbackground retained. Qualifiednative selector4→3 only; originalsoftware rejectsbackground viaactual640/480 primitivebounds, sourcecopiesonlyexplicitnative-domain/software gate. Actualowner−360/legacy24Hz100framewrap, originalRender/RNG through170; freshsourcecasescapturedeclaredsamephases withoutgiantfilmstrip. Full1920x1080center960,600 geometrybounds checkedall170 includinghiddenbackground. Unmodified fourthprototypefault/superseded doubleAnimatefixture retained. Sampling/internaldepth/quant/device/naturalcaller/exactpixels open.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],len(pairs),checks);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.output)
