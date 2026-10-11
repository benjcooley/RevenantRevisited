"""Freeze fixed-phase Funnel original/Metal native-domain and common diagnostic panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
ref=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
trace=json.loads((a.native/'trace.json').read_text());samples=[1,5,9,13,21,33,49,65,81,97,101,105,117,121,129,133,137,141,145];crop=(256,288,1152,896);manifests={};rng=[]
for domain,depth in [('native',[0,65535]),('common',[2238,3262])]:
 path=a.metal/(domain+'-domain')/'manifest.json';manifest=json.loads(path.read_text());assert manifest['status']=='pass'and manifest['scenario']['resolution']==[1280,960] and manifest['depth_range']==depth
 for t in manifest['timing']:assert abs(float(t['simulation_seconds'])-((int(t['frame'])-1)*4+2)/24)<1e-8
 for tick,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(path.parent/'run.log').read_text()):
  tick,count=int(tick),int(count);expected=trace['initial']['rng_draws']if tick==1 else trace['records'][min(tick-2,148)]['after']['rng_draws'];assert count==expected,(domain,tick,count,expected);rng.append(dict(domain=domain,before_render=tick,draws=count))
 manifests[domain]=manifest
assert manifests['native']['binary_sha256']==manifests['common']['binary_sha256']
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(1792,len(samples)*632));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);pairs=[]
for row,t in enumerate(samples):
 paths=[a.native/f'native-{t:03}.png',next((a.metal/'common-domain').glob(f'frame-{(t+3)//4:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{(t+3)//4:03}-*.png'))];bounds=[]
 for col,(path,label)in enumerate(zip(paths,['Original software','Metal common diagnostic','Metal native domain'])):
  im=Image.open(path).convert('RGB');assert im.size==(1280,960);bbox=im.getbbox();bounds.append(bbox)
  if bbox and col!=1:assert bbox[0]>0 and bbox[1]>0 and bbox[2]<1280 and bbox[3]<960,(t,label,bbox)
  whole.paste(im.resize((512,384)),(col*512,row*408+24));wd.text((col*512+4,row*408+4),f'tick {t} / pulse {t+1}: {label}',fill='white')
  if col in(0,2):
   dcol=col//2;detail.paste(im.crop(crop),(dcol*896,row*632+24));dd.text((dcol*896+4,row*632+4),f'tick {t}: {label} / fixed crop {crop}',fill='white')
 pairs.append(dict(capture=t,pulse=t+1,stage=trace['records'][t]['after']['state'],images=[ref(p)for p in paths],visible_bounds=bounds))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png')
r=dict(status='ready_for_independent_review',effect='Funnel',type_id='0xad92bc17',pairs=pairs,binary_sha256=manifests['native']['binary_sha256'],native_trace=ref(a.native/'trace.json'),metal_manifests=[ref(a.metal/(d+'-domain')/'manifest.json')for d in('native','common')],rng_controls=rng,panels=[ref(a.output/n)for n in('whole-life.png','detail.png')],cadence='24Hz Pulse and Render; warmup1; one-based screenshot ordinal N follows pulse4N-2 and native trace record4N-3. This follows logged simulation time and RNG, without phase fitting.',viewport=[1280,960],center=[640,640],detail_crop=crop,source_policy='Original Funnel spell-backed owner, ground1 for x<180 and0 beyond, source(-180,0,78), face64. Actual NextFrame/SetState/Pulse, both full PartSys controllers, default Animate/full Render/normal lighting and software raster. Native ambient38white/directional1; Metal ambient32white, native projector and positive16-bit depth. Damage calls observed only.',limits=['Decoded resource/COM setup, base animator Initialize, silent audio, map/script notifications, explicit invoker/target health and terrain are fixture boundaries. No natural spell/actor selection, damage, audio or arbitrary-terrain claim.','Common/hardware retains its ordinary projector; native-domain uses the proven original projector. No fitted color, scale, geometry or timing.','RGB565 destination blend precision, texture sampling, raster interpolation and device backend remain open; no exact framebuffer claim. The earlier quick panel used incorrect sparse-file phase arithmetic and is rejected; these phases follow CSV simulation timestamps.','All original and native-domain nonzero pixels of the declared phases lie strictly within the complete1280x960 viewport. Common-domain rendering remains a separate diagnostic. The fixed detail crop is supplemental, not whole-frame coverage.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'phases',len(pairs),'RNG',len(rng))
