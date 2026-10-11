#!/usr/bin/env python3
"""Freeze finite Fairy2 whole frames, fixed crop, repeat and blank-tail controls."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(p):return sha(Path(p).read_bytes())
def review(evidence,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((evidence/'native-finite-final/manifest.json').read_text());repeat=json.loads((evidence/'native-finite-repeat/manifest.json').read_text());state=json.loads((evidence/'state-finite-freeze/manifest.json').read_text());guard=json.loads((evidence/'profile-contract-v2/manifest.json').read_text())
 assert state['discrete_controls_pass'] and state['ticks']==401 and state['status']=='fail' and state['max_error']<1e-3
 assert guard['status']=='pass' and guard['reject']==8529
 assert [(c['tick'],c['rgb565_sha256'],c['rng_after'],c['draws'])for c in n['cases']]==[(c['tick'],c['rgb565_sha256'],c['rng_after'],c['draws'])for c in repeat['cases']]
 captures={label:json.loads((evidence/('metal-final-'+label)/'manifest.json').read_text())for label in('early','rise','high','fade','end')}
 binaries={c['binary_sha256']for c in captures.values()};assert len(binaries)==1
 for label,c in captures.items():
  assert c['status']=='pass' and c['scenario']['effect']=='TFairy2_AUTHORED' and c['scenario']['native_domain'] and c['scenario']['resolution']==[1280,960] and c['scenario']['seed']==1
  log=(evidence/('metal-final-'+label)/'run.log').read_text();assert 'capacity=208' in log and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' in log
 ticks=(6,30,60,120,200,300,398,399,400);panel=Image.new('RGB',(1280,len(ticks)*505));detail=Image.new('RGB',(820,len(ticks)*420));d=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);cases=[];checks=[]
 for row,tick in enumerate(ticks):
  c=next(c for c in n['cases']if c['tick']==tick);label='early'if tick<=60 else'rise'if tick==120 else'high'if tick==200 else'fade'if tick==300 else'end';m=captures[label];index=tick-m['scenario']['warmup']-1;frame=m['frames'][index];paths=[Path(c['image']),Path(frame['path'])];hashes=[c['image_sha256'],frame['sha256']];assert [digest(p)for p in paths]==hashes
  images=[Image.open(p).convert('RGB')for p in paths];bounds=[im.getbbox()for im in images]
  assert (all(b is None for b in bounds) and len(c['draws'])==0 and int(c['owner_flags'],16)&0x1000)if tick>=399 else all(b is not None for b in bounds)
  for packet in c['draws']:
   b=packet['geometry_bounds'];assert 0<b[0]<=b[2]<1280 and 0<b[1]<=b[3]<960
  cases.append(dict(tick=tick,native_owner_frame=c['frame'],owner_flags=c['owner_flags'],native_draws=len(c['draws']),images=list(map(str,paths)),sha256=hashes,visible_bounds=bounds,native_geometry_unclipped=True))
  for col,(im,label)in enumerate(zip(images,('Original','Actual Metal'))):panel.paste(im.resize((640,480)),(col*640,row*505+25));d.text((col*640+5,row*505+5),label+' tick'+str(tick),fill='white');detail.paste(im.crop((260,80,1080,870)).resize((410,395)),(col*410,row*420+25));dd.text((col*410+4,row*420+5),label+' same fixed crop tick'+str(tick),fill='white')
 for label in captures:
  log=(evidence/('metal-final-'+label)/'run.log').read_text()
  for tick,frame,draws,credit,x,y,z in re.findall(r'fairy2-phase\] tick=(\d+) frame=(\d+) dust=(\d+) credit=([^ ]+) emitter=\(([^,]+),([^,]+),([^)]+)\)',log):
   tick=int(tick);matches=[c for c in n['cases']if c['tick']==tick]
   if not matches:continue
   c=matches[0];assert int(draws)+1==len(c['draws']);error=max(abs(a-b)for a,b in zip(map(float,(x,y,z)),c['emitter_matrix'][12:15]));assert error<1e-5;checks.append(dict(label=label,tick=tick,draws=int(draws)+1,emitter_max_error=error))
 panel.save(output/'whole.png');detail.save(output/'fixed-crop.png')
 report=dict(status='awaiting_independent_review',accepted=False,name='Fairy2',retail_type_id='0xad92bd26',asset_sha256=n['asset_sha256'],original_executable_sha256=n['original_executable_sha256'],binary_sha256=next(iter(binaries)),cases=cases,actual_emitter_checks=checks,repeat_images_and_packets_identical=True,source_state_strict_status=state['status'],source_state_max_error=state['max_error'],source_state_discrete_controls_pass=True,numerical_residual_open=True,full_actual_metal=False,natural_map_profile_supported=False,appearance_fitted=False,input_manifest_sha256={str(p):digest(p)for p in[evidence/'native-finite-final/manifest.json',evidence/'native-finite-repeat/manifest.json',evidence/'state-finite-freeze/manifest.json',evidence/'profile-contract-v2/manifest.json',*[evidence/('metal-final-'+label)/'manifest.json'for label in captures],evidence/'map-lifecycle/manifest.json']},whole_sha256=digest(output/'whole.png'),fixed_crop_sha256=digest(output/'fixed-crop.png'),scope='Exact own Fairy2 finite399 state0 flags0x2000. Original full factory/controllers/parser/NextFrame/Pulse/Render/RNG on all401ticks; selected full raster only, device56eb30 boundary otherwise. Source typed-owner real399 lifecycle, normal-lit authored base and own dust/variablepps/gravity/relvel/jitter; unchanged actual Metal9phases at ticks6/30/60/120/200/300/398 plus399/400 zero draws/blanktail, no wraps. Declared1280x960 owner/camera0, ambient38/dir1, seed1, quality0, flatground0 and native-domain projection; no fitting. Exact pixels/color, tiny0.000366 world position/velocity residual, original depth-write/internal-edge difference, native device, natural caller/audio/state1 and unsupported normal map particle profile remain open. Historical modulo399 fixtures explicitly superseded.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['binary_sha256']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();review(a.evidence,a.output)
