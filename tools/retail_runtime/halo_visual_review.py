#!/usr/bin/env python3
"""Unfitted whole original/Metal Halo public-API comparison for exact own mesh."""
import argparse,json
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def run(native,metal,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());m=json.loads((metal/'manifest.json').read_text());assert m['status']=='pass';assert m['sample_simulation_ticks']==list(range(1,len(m['frames'])+1));selected=[r for r in n['rows']if r['image']];whole=Image.new('RGB',(1280,505*len(selected)));crop=Image.new('RGB',(128,89*len(selected)));details=Image.new('RGB',(256,153*len(selected)));w=ImageDraw.Draw(whole);c=ImageDraw.Draw(crop);d=ImageDraw.Draw(details);pairs=[]
 for row,r in enumerate(selected):
  t=r['tick'];paths=[Path(r['image']),Path(m['frames'][t-1]['path'])];hashes=[r['image_sha256'],m['frames'][t-1]['sha256']];assert [sha(p.read_bytes())for p in paths]==hashes;images=[Image.open(p).convert('RGB')for p in paths];boxes=[im.getbbox()for im in images];assert all(b is None or (b[0]>0 and b[1]>0 and b[2]<640 and b[3]<480)for b in boxes);pairs.append(dict(tick=t,images=list(map(str,paths)),sha256=hashes,visible_bbox=boxes,scale=r['scale'],frame=r['frame'],flags=r['flags'],native_draws=len(r['draws'])))
  for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):
   whole.paste(im,(640*col,505*row+25));w.text((640*col+4,505*row+5),f'{label} Halo tick{t}',fill='white');fixed=im.crop((288,263,352,327));crop.paste(fixed,(64*col,89*row+25));c.text((64*col+2,89*row+5),f'{label} {t}',fill='white');details.paste(fixed.resize((128,128),Image.Resampling.NEAREST),(128*col,153*row+25));d.text((128*col+2,153*row+5),f'{label} {t} 2xnearest',fill='white')
 for name,im in [('whole.png',whole),('fixed-crop.png',crop),('nearest2x-details.png',details)]:im.save(output/name)
 report=dict(status='awaiting_independent_review',accepted=False,type_id='0xdedbdbed',parameters=n['initialization']['parameters'],binary_sha256=m['binary_sha256'],pairs=pairs,native_manifest_sha256=sha((native/'manifest.json').read_bytes()),metal_manifest_sha256=sha((metal/'manifest.json').read_bytes()),panels={f:sha((output/f).read_bytes())for f in('whole.png','fixed-crop.png','nearest2x-details.png')},configuration=m['scenario'],appearance_fitted=False,scope='Actual public InitParams exact authored12vertices/12faces original/fullsoftwareRender versus actualMetal, two explicit parametercases distinct from discoveredretailcaller constants. Whole640x480 camera320,320 owner0,0,16; fixed64x64 ROI288,263,352,327 unchanged, optional2xnearestdetail clearlydisplay-only. Originaltiny blue glow grows/shrinks and blanktail; every visiblebbox strictly interior. Nearzero coverage/sampling/ARGB4 lookup/destinationquantization/device/naturalcaller remain open. No geometry/scale/phase/color fitting.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(pairs),report['binary_sha256']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.metal,a.output)
