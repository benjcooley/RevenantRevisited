"""Freeze CSV- and RNG-matched moving QueenArrow original/Metal panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
pin=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
trace=json.loads((a.native/'trace.json').read_text());samples=list(range(1,19))+[22];crop=(100,100,550,320);manifests={};rng=[]
for domain in('native','common'):
 p=a.metal/(domain+'-domain')/'manifest.json';m=json.loads(p.read_text());assert m['status']=='pass'and m['scenario']['resolution']==[640,480]
 for t in m['timing']:assert abs(float(t['simulation_seconds'])-(int(t['frame'])+1)/24)<1e-8
 for tick,grow,down,draws in re.findall(r'\[arrow-phase\] tick=(\d+) grow=(\d+) countdown=(-?\d+) random=(\d+)',(p.parent/'run.log').read_text()):
  tick,grow,down,draws=map(int,(tick,grow,down,draws));n=trace['records'][tick-1]['state'];assert(grow,down,draws)==(n['grow'],n['countdown'],n['rng_draws']);rng.append(dict(domain=domain,pulse=tick,draws=draws))
 manifests[domain]=m
assert manifests['native']['binary_sha256']==manifests['common']['binary_sha256']
whole=Image.new('RGB',(960,len(samples)*264));detail=Image.new('RGB',(900,len(samples)*244));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);pairs=[]
for row,t in enumerate(samples):
 paths=[a.native/f'native-{t:03}.png',next((a.metal/'common-domain').glob(f'frame-{t:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{t:03}-*.png'))];bounds=[]
 for col,(path,label)in enumerate(zip(paths,['Original software','Metal common diagnostic','Metal native domain'])):
  im=Image.open(path).convert('RGB');assert im.size==(640,480);bbox=im.getbbox();bounds.append(bbox)
  if bbox and col!=1:assert bbox[0]>0 and bbox[1]>0 and bbox[2]<640 and bbox[3]<480
  whole.paste(im.resize((320,240)),(col*320,row*264+24));wd.text((col*320+4,row*264+4),f'original index {t} / pulse {t+1}: {label}',fill='white')
  if col in(0,2):
   dcol=col//2;detail.paste(im.crop(crop),(dcol*450,row*244+24));dd.text((dcol*450+4,row*244+4),f'pulse {t+1}: {label}',fill='white')
 pairs.append(dict(native_record=t,pulse=t+1,images=[pin(p)for p in paths],visible_bounds=bounds))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png')
r=dict(status='ready_for_independent_review',effect='QueenArrow',type_id='0xad92bd1e',pairs=pairs,binary_sha256=manifests['native']['binary_sha256'],native_trace=pin(a.native/'trace.json'),metal_manifests=[pin(a.metal/(d+'-domain')/'manifest.json')for d in('native','common')],rng_controls=rng,panels=[pin(a.output/n)for n in('whole-life.png','detail.png')],viewport=[640,480],center=[320,320],source=[-180,0,38],target=[180,0,78],cadence='24Hz Pulse and Render; warmup1. One-based file N follows simulation pulse N+1 and native zero-based record N, verified from CSV and complete active RNG logs.',source_policy='Distinct original QueenArrow constructor, moving owner Pulse504d20 and full custom Render505000/RenderObject/normal lighting/software raster. Actual23-unit launch and collision countdown; exact asset object2 green selected by original constructor, with original untextured software ignoring material diffuse. Ambient38white/directional1; Metal ambient32white source policy. Native untextured flat-depth/opaqueRGB5 helper is reused; hardware retains authored additive scene policy.',limits=['Non-character source face64 and target-plane character query are declared caller inputs; no natural queen/bow, actor selection or live damage claim.','Damage/reaction, resource/COM, base animator Initialize and map/script/audio boundaries remain explicit.','Natural actor/terrain, device, scanline/raster precision and exact framebuffer equality remain open.','Full declared original/native sample bounds are within640x480. Detail crop is supplemental; common projection remains a diagnostic.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'phases',len(pairs),'RNG',len(rng))
