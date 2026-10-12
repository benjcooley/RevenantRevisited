#!/usr/bin/env python3
"""Unchanged full baseRibbon native/Metal pairs, source cadence and whole coverage."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(p):return sha(Path(p).read_bytes())
def review(evidence,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((evidence/'native-final/manifest.json').read_text());repeat=json.loads((evidence/'native-final-repeat/manifest.json').read_text());m=json.loads((evidence/'metal-final/manifest.json').read_text());state=json.loads((evidence/'state-animate-first-final/manifest.json').read_text());keys=json.loads((evidence/'legacy-contract-final/manifest.json').read_text());assert m['status']==state['status']==keys['status']=='pass';assert n['first_render_tick']==repeat['first_render_tick']==state['first_render_tick']==1;assert n['cases']==[{**c,'image':c.get('image','').replace('native-final-repeat','native-final')}if 'image'in c else c for c in repeat['cases']]
 assert m['scenario']['native_domain'] and m['scenario']['effect']=='TRibbonAnimator_BESPOKE'and m['scenario']['frames']==90 and m['pixel_summary']['distinct_frames']==90
 ticks=(2,6,16,20,30,60,90);full=Image.new('RGB',(1280,len(ticks)*505));detail=Image.new('RGB',(1040,len(ticks)*625));d=ImageDraw.Draw(full);dd=ImageDraw.Draw(detail);cases=[]
 for row,tick in enumerate(ticks):
  nc=next(c for c in n['cases']if c['tick']==tick);mc=m['frames'][tick-2];paths=[Path(nc['image']),Path(mc['path'])];hashes=[nc['image_sha256'],mc['sha256']];assert list(map(digest,paths))==hashes;images=[Image.open(p).convert('RGB')for p in paths];assert all(im.getbbox() for im in images)
  bounds=[c['geometry_bounds']for c in nc['draws']];assert all(0<b[0]<=b[2]<1280 and 0<b[1]<=b[3]<960 for b in bounds);cases.append(dict(tick=tick,native_draws=len(nc['draws']),rng_after=nc['rng_after'],native_geometry_bounds=bounds,whole_geometry_unclipped=True,visible_bounds=[i.getbbox()for i in images],images=list(map(str,paths)),sha256=hashes))
  for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):full.paste(im.resize((640,480)),(col*640,row*505+25));d.text((col*640+5,row*505+5),label+' Ribbon tick'+str(tick),fill='white');detail.paste(im.crop((380,190,900,790)),(col*520,row*625+25));dd.text((col*520+4,row*625+5),label+' same actual-size crop tick'+str(tick),fill='white')
 full.save(output/'whole.png');detail.save(output/'fixed-crop.png');log=(evidence/'metal-final/run.log').read_text();checks=[]
 for t,scale,sparks,floor in re.findall(r'ribbon-phase\] tick=(\d+) scale=([^ ]+) sparks=(\d+) floor=([^\n]+)',log):
  t=int(t);nc=next((c for c in n['cases']if c['tick']==t),None)
  if not nc or not nc['raster_executed']:continue
  assert int(sparks)+4==len(nc['draws']);checks.append(dict(tick=t,source_draws=int(sparks)+4,source_scale=float(scale),source_floor=float(floor)))
 assert len(checks)>=7
 report=dict(status='awaiting_independent_review',accepted=False,name='Ribbon',type_id='0x562c7439',asset_sha256=n['asset_sha256'],binary_sha256=m['binary_sha256'],cases=cases,actual_draw_checks=checks,fresh_original_replays_identical=True,source_state_fields=state['fields'],source_state_max_error=state['max_error'],native_pose_channels=keys['native_pose_channels'],native_pose_max_error=keys['max_pose_error'],source_legacy_guard_positive=keys['accept'],source_legacy_guard_negative=keys['reject'],input_manifest_sha256={str(p):digest(p)for p in(evidence/'native-final/manifest.json',evidence/'native-final-repeat/manifest.json',evidence/'metal-final/manifest.json',evidence/'state-animate-first-final/manifest.json',evidence/'legacy-contract-final/manifest.json')},whole_sha256=digest(output/'whole.png'),fixed_crop_sha256=digest(output/'fixed-crop.png'),appearance_fitted=False,scope='Literal baseRibbon0x562c7439 own legacy asset/defaultnulltargetbranch; full original factory/allconstructors/Initialize/30sparks/3tubes20tickgrowth/real5frameTexture0Selector/Render/RNG. Initialize→Animate1→Render1 caller cadence exactly matches actual source; no insertedFrame0Render RNG. Source12vertex12triangle sparks/floor and174vertex192triangle tube geometry complete;18legacy posechannels/raw16bytes restored narrowly. All90actualMetal frames distinct, seven fullunclipped native/Metal pairs with real cached sourcekeys/CalcObjectMatrix and existing exactARGB helper; no pose/scale/color/camera/phase fit. Explicit1280×960 camera/owner0,ambient38/dir1,seed1,nullspell/target/immutableresource/COM/keycache/devicehandoffbounds. Natural revive actor/decay,targetmovement,device/exactpixel/color and normal-map trigger integration remain open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['binary_sha256']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();review(a.evidence,a.output)
