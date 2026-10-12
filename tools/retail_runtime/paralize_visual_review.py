#!/usr/bin/env python3
"""Freeze supporting fixed-view Paralize1 pairs; huge trajectory holds appearance."""
import argparse,json
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(path):return sha(Path(path).read_bytes())
def run(native,common,domain,state,bounds,gallery,output):
 output.mkdir(parents=True,exist_ok=False)
 n=json.loads((native/'trace.json').read_text()); films=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 for p in(state,gallery):assert json.loads((p/'manifest.json').read_text())['status']=='pass'
 b=json.loads((bounds/'manifest.json').read_text());assert b['span'][0]>600000 and b['span'][1]>160000
 assert all(m['status']=='pass'and m['scenario']['effect']=='TParalize1_NAMED_NO_TARGET'and len(m['frames'])==32 for m in films)
 assert films[0]['binary_sha256']==films[1]['binary_sha256']
 ticks=(2,5,9,10,11,13,16,19,20,21,22,25,30,32);contact=Image.new('RGB',(960,len(ticks)*265));draw=ImageDraw.Draw(contact);rows=[]
 for row,tick in enumerate(ticks):
  ref=n['records'][tick-1];paths=[Path(ref['image'])]+[Path(m['frames'][tick-2]['path'])for m in films];hashes=[ref['image_sha256']]+[m['frames'][tick-2]['sha256']for m in films];assert list(map(digest,paths))==hashes
  images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images)
  for col,(im,label)in enumerate(zip(images,('Original','Common Metal','Native-domain Metal'))):
   contact.paste(im.resize((320,240)),(320*col,265*row+25));draw.text((320*col+3,265*row+4),f'{label} tick {tick}',fill='white')
  rows.append(dict(tick=tick,paths=list(map(str,paths)),sha256=hashes,bounds=[im.getbbox()for im in images],native_draws=ref['draw_count']))
 contact.save(output/'contact.png');inputs=[native/'trace.json']+[p/'manifest.json'for p in(common,domain,state,bounds,gallery)]
 report=dict(status='held_incomplete_trajectory_coverage',accepted=False,visual_pass=False,type_id='0xad92bc14',name='paralize1',binary_sha256=films[0]['binary_sha256'],asset_sha256=n['asset_sha256'],cases=rows,projected_trajectory_bounds=b['bounds'],projected_trajectory_span=b['span'],input_manifest_sha256={str(p):digest(p)for p in inputs},contact_sha256=digest(output/'contact.png'),appearance_fitted=False,scope='Supporting early fixed-view original/fullRender versus actual Metal. Named Physical Paralysis, null invoker, zero targets, seed1, source ambient38/dir1, quality0, origin/camera0, 1280x960 center640/640. Actual kill21 and later no-op ticks. The enormous real no-target trajectory is not covered by these views, so full appearance remains HOLD; no target eggs/actor/damage/audio/device/exact-pixel claim. Gallery proxy at a nonzero station remains explicitly partial.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for name in('native','common','domain','state','bounds','gallery','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();run(a.native,a.common,a.domain,a.state,a.bounds,a.gallery,a.output)
