#!/usr/bin/env python3
"""Freeze independently produced whole EnergySpray lifecycle images and controls."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from skin_visual_review import sha

def review(base,output,current_tag=""):
 names=('energyspray-native-final','energy-metal-common-final'+current_tag,'energy-metal-domain-final'+current_tag,'energyspray-state-first','energyspray-matrix-first','energy-bounds-final','energy-prelit-native-final2','energy-argb-native-final')
 inputs={name:json.loads((base/name/'manifest.json').read_text())for name in names}
 native=inputs[names[0]];metal=[inputs[name]for name in names[1:3]];gpu=json.loads((base/('energy-prelit-gpu-final2'+current_tag)/'review.json').read_text())
 assert native['warm_replays']==2 and not native['render_initial']
 for name in names[3:]:assert inputs[name]['status']=='pass',name
 assert gpu['status']=='pass'
 for name,m in zip(names[1:3],metal):
  assert m['status']=='pass' and m['exit_code']==0 and m['scenario']['frames']==140
  assert m['scenario']['effect']=='TEnergySpray_BESPOKE_NULLSPELL'
  assert m['binary_sha256']==gpu['binary_sha256']
  log=(base/name/'run.log').read_text();assert sha(base/name/'run.log')==m['log_sha256']
  rows=re.findall(r'energyspray-phase\] tick=(\d+) phase=(\d+) countdown=(\d+) core=([^ ]+) rendered_scale=([^ ]+) draws=(\d+) rng=(\d+)',log)
  assert len(rows)==135
  for tick,phase,countdown,core,scale,draws,rng in rows:
   n=native['cases'][int(tick)];s=n['state']
   assert (int(phase),int(countdown),int(rng),int(draws))==(s['phase'],s['countdown'],s['rng_draws'],len(n['draws']))
   assert abs(float(core)-s['core_scale'])<1e-6
 output.mkdir(parents=True,exist_ok=False)
 ticks=(2,10,30,59,60,61,70,90,110,111,120,130,135,136,140)
 contact=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(960,len(ticks)*345));pd=ImageDraw.Draw(contact);dd=ImageDraw.Draw(detail);cases=[]
 for row,tick in enumerate(ticks):
  n=native['cases'][tick];paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in metal];hashes=[n['image_sha256']]+[m['frames'][tick-2]['sha256']for m in metal];assert list(map(sha,paths))==hashes
  c=dict(tick=tick,alive=n['state']['alive'],phase=n['state']['phase'],images=list(map(str,paths)),sha256=hashes,pixels=[],bounds=[])
  for col,(path,label)in enumerate(zip(paths,('Original','Common Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');assert im.size==(640,480);c['pixels'].append(sum(p!=(0,0,0)for p in im.getdata()));c['bounds'].append(im.getbbox())
   contact.paste(im.resize((320,240)),(320*col,265*row+25));pd.text((320*col+4,265*row+4),f'{label} tick {tick}',fill='white')
   detail.paste(im.crop((160,80,480,400)),(320*col,345*row+25));dd.text((320*col+4,345*row+4),f'{label} tick {tick} actual-size crop',fill='white')
  if tick>=136:assert not any(c['pixels'])
  if tick==111:assert c['pixels'][0]==c['pixels'][2]==0 and c['pixels'][1]>0
  cases.append(c)
 contact.save(output/'contact.png');detail.save(output/'detail.png')
 report=dict(status='awaiting_independent_review',accepted=False,retail_name='energyspray',retail_type_id='0xad92bc1c',asset_sha256=native['asset_sha256'],binary_sha256=gpu['binary_sha256'],original_executable_sha256=native['original_executable_sha256'],cases=cases,contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),input_manifest_sha256={str(base/name/'manifest.json'):sha(base/name/'manifest.json')for name in names},gpu_review_sha256=sha(base/('energy-prelit-gpu-final2'+current_tag)/'review.json'),source_native_phase_rng_matches=270,appearance_fitted=False,scope='Null-spell facing0, origin/camera world0, seed1, source ambient38/directional1; actual registry, owner Init/Pulse, leaf Init/Animate/Render, full RenderObject and original raster. Original two complete141-tick replays; actual Metal140 frames at24fps, common and explicit native-domain, identical binary. Twenty-five authored radial cards, central Render-RNG flare, twenty colored triangles, phases60/50/25 and real owner kill136. Independent source state87984 fields, matrices82096 fields and2220 packed colors. Existing per-source ARGB contribution floor and literal primitive-span admission only native-domain/software policy; common retains complete geometry. New explicit prelitRGBA5 helper uses literal packed colors/alpha, no invented normals. Native source contribution and depth controls pass; general destination RGB565 rounding/interpolation/framebuffer precision are not exact. Frozen640x480 viewport clips outer fan tips in both; no full offscreen geometry visibility claim. Original triangle ABSPOS at zero anchor is preserved by fixture; gallery translates the complete standalone fixture to its station. Natural actor-hand attachment, spell target/damage, audio, nonzero facing, natural caller and other devices remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(sha(output/'manifest.json'));print([(c['tick'],c['pixels'])for c in cases]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--current-tag',default='');a=p.parse_args();review(a.base,a.output,a.current_tag)
