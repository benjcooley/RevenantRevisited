"""Matched moving Photon original-software / actual-Metal lifecycle panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--charge',action='store_true');a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();trace=json.loads((a.native/'trace.json').read_text());samples=[1,5,10,20,29,30,35,40,50,60,65,70,80,99];prefix='charge'if a.charge else'flight';pairs=[]
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(1536,len(samples)*536));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail)
for row,i in enumerate(samples):
 files=[a.native/f'native-{i:03}.png',next((a.metal/(prefix+'-common')).glob(f'frame-{i:03}-*.png')),next((a.metal/(prefix+'-native')).glob(f'frame-{i:03}-*.png'))]
 x,y,z=trace['records'][i]['motion']['position'];nz=z/(1.4600000381469727-z*.0033333334140479565*.009999999776482582)*1.0379999876022339;sx=512+1.010152578*(x-y);sy=384+.505076289*(x+y)-1.23717916*nz;crop=(round(sx)-256,round(sy)-256,round(sx)+256,round(sy)+256)
 for col,(f,label)in enumerate(zip(files,['Original full software','Metal common world','Metal native projector'])):
  im=Image.open(f).convert('RGB');whole.paste(im.resize((512,384)),(col*512,row*408+24));detail.paste(im.crop(crop),(col*512,row*536+24));w.text((col*512+4,row*408+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white');d.text((col*512+4,row*536+5),f'{prefix} capture {i} / pulse {i+1}: {label}',fill='white')
 pairs.append(dict(capture=i,pulse=i+1,crop=crop,images=[dict(path=str(f.resolve()),sha256=sha(f))for f in files]))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for domain in('common','native'):
 for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(a.metal/(prefix+'-'+domain)/'run.log').read_text()):
  render=int(render);actual=int(count);expect=trace['initial']['rng_draws']if render==1 else trace['records'][render-2]['state']['rng_draws'];rng.append(dict(domain=domain,before_render=render,native=expect,metal=actual,match=actual==expect))
assert all(r['match']for r in rng),rng
r=dict(status='ready_for_independent_review',effect='Photon',animate_first=a.charge,pairs=pairs,rng_controls=rng,cadence='24Hz warmup 1; capture N follows original pulse N+1; optional single prelaunch Animate declared',source_policy='Original normal-lit FVF112 executed, actual five ARGB4444 textures and strict software alpha path; native ambient38/directional1. Metal scene32white. No synthetic white vertex input in original reference.',crop_policy='Fixed512 square at original owner projection; same crop in each column; no geometry/color/registration fit',native_trace=dict(path=str((a.native/'trace.json').resolve()),sha256=sha(a.native/'trace.json')),metal_manifests=[dict(path=str((a.metal/(prefix+'-'+d)/'manifest.json').resolve()),sha256=sha(a.metal/(prefix+'-'+d)/'manifest.json'))for d in('common','native')],panels=[dict(path=str((a.output/n).resolve()),sha256=sha(a.output/n))for n in('whole-life.png','detail.png')],limits=['Resource/world/audio/command-reaper interfaces explicit. Natural spell/game caller not certified.','Common world versus native projector and raster/backend quantization differences remain.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('manifest',sha(a.output/'pair-manifest.json'),'RNG controls',len(rng))
