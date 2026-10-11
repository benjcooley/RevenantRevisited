#!/usr/bin/env python3
"""Freeze original/Metal Iced reveal, shatter, bounce and exhausted-tail evidence."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from heal_visual_review import sha


def review(native,common,domain,output):
 output.mkdir(parents=True,exist_ok=False)
 n=json.loads((native/'manifest.json').read_text());captures=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert n['status']=='reference_complete' and n['warm_replays']==2 and n['null_victim_control']['draws']==0
 checks=[];lights=[]
 for index,(path,m)in enumerate(zip((common,domain),captures)):
  assert m['status']=='pass'
  expected=dict(frames=420,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[320,240],rectangle=[0,0,640,480],resolution=[640,480],background='black',ambient=[32,255,255,255],native_domain=bool(index),effect='TIcedEffect_BESPOKE_SINGLE_BURST')
  assert all(m['scenario'].get(k)==v for k,v in expected.items())
  log=(path/'run.log').read_text()
  assert f'[native-domain] enabled={index} camera_z=0 owner=RzRxRyT Iced victim=stationary-live' in log
  light=[line for line in log.splitlines()if '[vfx-source-light]'in line]
  assert len(light)==1 and 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in light[0]
  lights.append(light[0]);counts={int(t):int(c)for t,c in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
  assert set(counts)=={1,2,6,16,30,46,60}
  for tick,count in counts.items():assert n['cases'][tick-1]['state']['rng_calls']==count
  phases={int(t):(int(a),int(r))for t,a,r in re.findall(r'\[iced-runtime-phase\] frame=(\d+) alive=(\d+) rng_draws=(\d+)',log)}
  assert set(phases)=={239,240,241,322}
  for tick,(alive,rng)in phases.items():assert (bool(alive),rng)==(n['cases'][tick]['state']['alive'],n['cases'][tick]['state']['rng_calls'])
  checks.append(dict(before_render=counts,leaf_phases=phases))
 selected=(2,4,13,239,240,250,280,300,320,322,421)
 panel=Image.new('RGB',(900,len(selected)*250));draw=ImageDraw.Draw(panel);cases=[]
 for c in n['cases']:
  tick=c['tick']
  if tick<2 or 'image'not in c:continue
  paths=[Path(c['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures]
  hashes=[c['sha256']]+[m['frames'][tick-2]['sha256']for m in captures]
  assert [sha(p)for p in paths]==hashes
  images=[Image.open(p).convert('RGB')for p in paths]
  cases.append(dict(tick=tick,capture=tick-1,alive=c['state']['alive'],draws=len(c['packets']),rng_calls=c['state']['rng_calls'],images=list(map(str,paths)),sha256=hashes,nonzero_pixels=[sum(any(rgb)for rgb in im.getdata())for im in images]))
  if tick in selected:
   row=selected.index(tick)
   for col,(im,label)in enumerate(zip(images,('Original','Common-world Metal','Native-domain Metal'))):
    panel.paste(im.resize((300,225),Image.Resampling.NEAREST),(col*300,row*250+25));draw.text((col*300+2,row*250+4),f'{label} tick{tick}',fill='white')
 panel.save(output/'contact.png')
 detail=Image.new('RGB',(960,750));draw=ImageDraw.Draw(detail)
 for row,tick in enumerate((13,250)):
  c=next(c for c in cases if c['tick']==tick)
  for col,path in enumerate(c['images']):
   im=Image.open(path).convert('RGB').crop((220,55,435,295)).resize((320,350),Image.Resampling.NEAREST)
   detail.paste(im,(col*320,row*375+25));draw.text((col*320,row*375),('Original','Common-world','Native-domain')[col]+f' tick{tick}')
 detail.save(output/'detail.png')
 report=dict(status='review_ready',accepted=False,retail_name='Iced',retail_type_id='0xfacecabb',cases=cases,selected_ticks=selected,live_checks=checks,source_light_logs=lights,binary_sha256=[m['binary_sha256']for m in captures],
  input_manifest_sha256=[sha(p/'manifest.json')for p in(native,common,domain)],contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,
  port_capture_counts=[m['pixel_summary']for m in captures],
  scope='Sixteen whole-effect native/Metal phase triples covering reveal, hold, shatter, gravity/spin/bounces and death. Each actual Metal route contains420frames; original422states and18sampledsoftwareframes repeat exactly. Native ownerInit/InitIced/Pulse and wholeanimator with explicit stationary living victimhealth100, origin0/facing0; sourceARGB4444,declaredcull2/depthtest/no-write. Native-domain fixture cameraZ0; common-world projector residual retained. BasePulse/Stop/audio/resource dispatch, actual character attachment, early-unfreeze trigger and exactpixels remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'whole-effect phase triples')

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','common','domain','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.output)
