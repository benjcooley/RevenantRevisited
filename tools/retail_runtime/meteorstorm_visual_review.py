"""Matched MeteorStorm Comet original-software / actual-Metal lifecycle panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
depth_policy={}
for domain,expected in [('native',[0,65535]),('common',[2238,3262])]:
 manifest=json.loads((a.metal/(domain+'-domain')/'manifest.json').read_text())
 assert manifest['status']=='pass' and manifest.get('depth_range')==expected,manifest
 depth_policy[domain]=manifest['depth_range']
trace=json.loads((a.native/'trace.json').read_text());samples=[1,3,5,8,11,14,17,20,23,26,29,32,35,38,41,44,59];prefix=trace['effect'];pairs=[]
whole=Image.new('RGB',(1920,len(samples)*384));detail=Image.new('RGB',(2400,len(samples)*1104));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail)
for row,i in enumerate(samples):
 files=[a.native/f'native-{i:03}.png',next((a.metal/'common-domain').glob(f'frame-{i:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{i:03}-*.png'))]
 crop=(600,0,1400,1080)
 for col,(f,label)in enumerate(zip(files,['Original full software','Metal common world','Metal native projector'])):
  im=Image.open(f).convert('RGB');whole.paste(im.resize((640,360)),(col*640,row*384+24));detail.paste(im.crop(crop),(col*800,row*1104+24));w.text((col*640+4,row*384+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white');d.text((col*800+4,row*1104+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white')
 pairs.append(dict(capture=i,pulse=i+1,crop=crop,images=[dict(path=str(f.resolve()),sha256=sha(f))for f in files]))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for domain in('common','native'):
 for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(a.metal/(domain+'-domain')/'run.log').read_text()):
  render=int(render);actual=int(count);expect=trace['initial']['rng_draws']if render==1 else trace['records'][render-2]['after']['rng_draws'];rng.append(dict(domain=domain,before_render=render,native=expect,metal=actual,match=actual==expect))
assert all(r['match']for r in rng),rng
r=dict(status='ready_for_independent_review',effect=trace['effect'],viewer_depth_ranges=depth_policy,pairs=pairs,rng_controls=rng,cadence='24Hz warmup 1; capture N follows original pulse N+1; original null-spell NextFrame and owner Pulse (which dispatches animator Pulse exactly once), Animate and full Render',source_policy='Original normal-lit FVF112 base plus full FVF1e2 PartSys executed; actual two RGB565 textures, mode80 bases/mode16 particles; native ambient38/directional1. Metal scene32white. No synthetic white vertex input in original reference.',crop_policy='Full1920x1080 viewport, fixed detail crop600,0,1400,1080; same crop in each column; no geometry/color/registration fit',native_trace=dict(path=str((a.native/'trace.json').resolve()),sha256=sha(a.native/'trace.json')),metal_manifests=[dict(path=str((a.metal/(d+'-domain')/'manifest.json').resolve()),sha256=sha(a.metal/(d+'-domain')/'manifest.json'))for d in('common','native')],panels=[dict(path=str((a.output/n).resolve()),sha256=sha(a.output/n))for n in('whole-life.png','detail.png')],limits=['Resource/map-notification/audio/script boundaries explicit. Flat terrain height1 is the declared particle world. Null-spell authored Comet leaf only; natural StrikeEffect/Strike spell orchestration, damage, audio and actor caller are not certified.','Common world versus native projector and raster/backend quantization differences remain.','Explicit native-domain plus software policy applies original640/480 projected primitive-span rejection. The common path preserves the full authored impact ring; hardware appearance is not certified by this software comparison.','Native-domain viewer uses original positive16-bit depth range0..65535; common diagnostic retains2238..3262 and clips high launch geometry. Only near/far differ; no XY/camera/geometry/color change.', 'Fixed1920x1080 viewport and crop; no off-screen or arbitrary-camera claim.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('manifest',sha(a.output/'pair-manifest.json'),'RNG controls',len(rng))
