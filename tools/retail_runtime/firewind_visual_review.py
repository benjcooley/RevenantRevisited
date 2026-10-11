"""Matched 400-slot FireWind original-software / actual-Metal lifecycle panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();trace=json.loads((a.native/'trace.json').read_text());samples=[1,4,9,15,24,27,28,34,39,40,44,59,79,99,119,139,140,149];prefix=trace['effect'];pairs=[]
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(3072,len(samples)*792));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail)
for row,i in enumerate(samples):
 files=[a.native/f'native-{i:03}.png',next((a.metal/'common-domain').glob(f'frame-{i:03}-*.png')),next((a.metal/'native-domain').glob(f'frame-{i:03}-*.png'))]
 crop=(0,0,1024,768)
 for col,(f,label)in enumerate(zip(files,['Original full software','Metal common world','Metal native projector'])):
  im=Image.open(f).convert('RGB');whole.paste(im.resize((512,384)),(col*512,row*408+24));detail.paste(im.crop(crop),(col*1024,row*792+24));w.text((col*512+4,row*408+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white');d.text((col*1024+4,row*792+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white')
 pairs.append(dict(capture=i,pulse=i+1,crop=crop,images=[dict(path=str(f.resolve()),sha256=sha(f))for f in files]))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for domain in('common','native'):
 for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(a.metal/(domain+'-domain')/'run.log').read_text()):
  render=int(render);actual=int(count);expect=trace['initial']['rng_draws']if render==1 else trace['records'][render-2]['state']['rng_draws'];rng.append(dict(domain=domain,before_render=render,native=expect,metal=actual,match=actual==expect))
assert all(r['match']for r in rng),rng
r=dict(status='ready_for_independent_review',effect=trace['effect'],pairs=pairs,rng_controls=rng,cadence='24Hz warmup 1; capture N follows original pulse N+1; original null-spell owner/animator pulse then Render',source_policy='Original normal-lit FVF112 executed, actual two RGB565 textures and source additive normal-lit policy; native ambient38/directional1. Metal scene32white. No synthetic white vertex input in original reference.',crop_policy='Fixed full1024x768 detail; same crop in each column; no geometry/color/registration fit',native_trace=dict(path=str((a.native/'trace.json').resolve()),sha256=sha(a.native/'trace.json')),metal_manifests=[dict(path=str((a.metal/(d+'-domain')/'manifest.json').resolve()),sha256=sha(a.metal/(d+'-domain')/'manifest.json'))for d in('common','native')],panels=[dict(path=str((a.output/n).resolve()),sha256=sha(a.output/n))for n in('whole-life.png','detail.png')],limits=['Resource/empty-world/audio/command-reaper interfaces explicit. Natural spell damage/target cleanup and game caller not certified; Only the named effect and its own native registrations are evaluated.','Common world versus native projector and raster/backend quantization differences remain.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('manifest',sha(a.output/'pair-manifest.json'),'RNG controls',len(rng))
