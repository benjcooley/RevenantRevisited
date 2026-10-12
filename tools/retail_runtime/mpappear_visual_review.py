"""Matched MPAppear original-software / actual-Metal lifecycle panels."""
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
trace=json.loads((a.native/'trace.json').read_text());samples=[1,4,9,14,19,24,29,34,39,44,49,59,69,79,88,89,99,109];prefix=trace['effect'];pairs=[]
viewport=trace['policy']['viewport'];wide=viewport==[1280,960];crop=(128,128,1152,896)if wide else(128,256,896,768);cw,ch=crop[2]-crop[0],crop[3]-crop[1]
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(cw*3,len(samples)*(ch+24)));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail)
for row,i in enumerate(samples):
 files=[a.native/f'native-{i:03}.png',next((a.metal/'common-domain').glob(f'frame-{i:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{i:03}-*.png'))]
 for col,(f,label)in enumerate(zip(files,['Original full software','Metal common world','Metal native projector'])):
  im=Image.open(f).convert('RGB');assert list(im.size)==viewport,(f,im.size,viewport);whole.paste(im.resize((512,384)),(col*512,row*408+24));detail.paste(im.crop(crop),(col*cw,row*(ch+24)+24));w.text((col*512+4,row*408+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white');d.text((col*cw+4,row*(ch+24)+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white')
 pairs.append(dict(capture=i,pulse=i+1,crop=crop,images=[dict(path=str(f.resolve()),sha256=sha(f))for f in files]))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for domain in('common','native'):
 for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(a.metal/(domain+'-domain')/'run.log').read_text()):
  render=int(render);actual=int(count);expect=trace['initial']['rng_draws']if render==1 else trace['records'][render-2]['after']['rng_draws'];rng.append(dict(domain=domain,before_render=render,native=expect,metal=actual,match=actual==expect))
assert all(r['match']for r in rng),rng
r=dict(status='ready_for_independent_review',effect=trace['effect'],viewer_depth_ranges=depth_policy,pairs=pairs,rng_controls=rng,cadence='24Hz warmup 1; capture N follows original pulse N+1; original null-spell NextFrame and owner Pulse (which dispatches animator Pulse exactly once), Animate and full Render',source_policy='Original full default Render executes actual normal-lit FVF112 meshes; four literal RGB565 textures, mode80 meshes; native ambient38/directional1. Metal scene32white. No synthetic white vertex input in original reference.',crop_policy=f'Full{viewport} viewport, fixed detail crop{crop}; same crop in each column; no geometry/color/registration fit',native_trace=dict(path=str((a.native/'trace.json').resolve()),sha256=sha(a.native/'trace.json')),metal_manifests=[dict(path=str((a.metal/(d+'-domain')/'manifest.json').resolve()),sha256=sha(a.metal/(d+'-domain')/'manifest.json'))for d in('common','native')],panels=[dict(path=str((a.output/n).resolve()),sha256=sha(a.output/n))for n in('whole-life.png','detail.png')],limits=['Resource/map-notification/audio/script boundaries explicit. Flat terrain height1 is the declared particle world. Null-spell MPAppear leaf only; natural multiplayer/network/spell triggering/audio, saved layered map appearance and complete device caller are not certified.','Common world versus native projector and raster/backend quantization differences remain. Full viewport boundary is recorded in cases.json. Off-screen appearance is unclaimed.','Explicit native-domain plus software policy applies the original640/480 primitive-span limits. The common/hardware path retains full geometry; the original guard comparison is a bounded diagnostic.','Native-domain viewer uses original positive16-bit depth range0..65535; common diagnostic retains2238..3262. No per-effect fitted transform or color.', 'Fixed declared viewport and crop; no off-screen or arbitrary-camera claim.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('manifest',sha(a.output/'pair-manifest.json'),'RNG controls',len(rng))
