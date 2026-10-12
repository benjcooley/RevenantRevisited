#!/usr/bin/env python3
"""Whole qualified-original/source panels; never an unmodified-retail verdict."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def run(native,metal,out):
 out.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());assert n['patched_original']and m['status']=='pass'and m['sample_simulation_ticks']==list(range(1,105));pairs=[];rows=[r for r in n['records']if 'image'in r];crop=Image.new('RGB',(180,len(rows)*90));detail=Image.new('RGB',(720,len(rows)*265));whole=[];roi=(595,450,685,510)
 for row,r in enumerate(rows):
  t=r['tick'];f=m['frames'][t-1];paths=[native/r['image'],Path(f['path'])];hashes=[r['sha256'],f['sha256']];assert[sha(p.read_bytes())for p in paths]==hashes;images=[Image.open(p).convert('RGB')for p in paths];boxes=[im.getbbox()for im in images];assert all(b is None or b[0]>0 and b[1]>0 and b[2]<1280 and b[3]<960 for b in boxes);page=Image.new('RGB',(2560,985))
  for c,(im,label)in enumerate(zip(images,('Qualified modified original','Actual Metal qualified port'))):
   page.paste(im,(c*1280,25));ImageDraw.Draw(page).text((c*1280+4,5),f'{label} Rift1 tick{t}',fill='white');p=im.crop(roi);crop.paste(p,(c*90,row*90+25));ImageDraw.Draw(crop).text((c*90+2,row*90+5),f'{"Mod"if c==0 else"GPU"} {t}',fill='white');detail.paste(p.resize((360,240),Image.Resampling.NEAREST),(c*360,row*265+25));ImageDraw.Draw(detail).text((c*360+2,row*265+5),f'{label} {t} 4xnearest',fill='white')
  p=out/f'whole-{t:03d}.png';page.save(p);whole.append(dict(path=str(p),sha256=sha(p.read_bytes())));pairs.append(dict(tick=t,images=list(map(str,paths)),sha256=hashes,bbox=boxes,flags=r['state']['flags'],owner_frame=r['state']['frame'],draws=len(r['draws'])))
 crop.save(out/'fixed-crop.png');detail.save(out/'nearest4x-details.png');errors=[];checks=0
 for t,g,f,u,v in re.findall(r'\[rift-qualified-phase\] tick=(\d+) global=(\d+) frame=(\d+) u=([0-9.eE+-]+) v=([0-9.eE+-]+)',(metal/'run.log').read_text()):
  t,g,f=int(t),int(g),int(f);r=n['records'][t-1];checks+=5
  if t!=g or f!=r['state']['frame']or float(u)!=(g%4)*.25 or float(v)!=((g%16)-4*(g%4))*.25:errors.append(dict(tick=t))
 assert '[rift-qualified-kill] tick=100 frame=99'in(metal/'run.log').read_text();r=dict(status='awaiting_independent_qualified_review',accepted=False,unmodified_visual_pass=False,type_id='0xadbcef18',patch=n['patch'],binary_sha256=m['binary_sha256'],pairs=pairs,whole_panels=whole,detail_sha256=sha((out/'nearest4x-details.png').read_bytes()),crop_sha256=sha((out/'fixed-crop.png').read_bytes()),source_lifetime_uv_checks=checks,errors=errors,native_manifest_sha256=sha((native/'manifest.json').read_bytes()),metal_manifest_sha256=sha((metal/'manifest.json').read_bytes()),appearance_fitted=False,scope='Qualified original one parser literal v→d only, unchanged actualRift asset/genericfactory/caller and original row arithmetic/4wrapped appearances16rawUVcycle/kill100. Source own strictprofile/UVcontroller/realowner APIs, whole1280x960 bounds, fixed595,450,685,510 ROI/4xnearest display only. Unmodified Init fault retained; never an unmodified appearancepass. Naturalcaller/otherrevisions/device/sampling/edge/quant/exactpixels open.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],len(pairs),checks,len(errors));return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.output)
