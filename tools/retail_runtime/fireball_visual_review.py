"""Freeze matched FireBall full-software/common-Metal/native-Metal panels."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--native',required=True,type=Path);parser.add_argument('--metal',required=True,type=Path);parser.add_argument('--output',required=True,type=Path);args=parser.parse_args()
n=args.native.resolve();m=args.metal.resolve();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
trace=json.loads((n/'trace.json').read_text());samples=(1,5,10,20,40,59,60,65,70,80,89)
whole=Image.new('RGB',(1536,len(samples)*408));detail=Image.new('RGB',(1536,len(samples)*536));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail);pairs=[]
for row,i in enumerate(samples):
 files=[n/f'native-{i:03}.png',next((m/'common-domain').glob(f'frame-{i:03}-*.png')),next((m/'native-domain').glob(f'frame-{i:03}-*.png'))]
 x,y,z=trace[i]['motion']['position'];nz=z/(1.4600000381469727-z*.0033333334140479565*.009999999776482582)*1.0379999876022339
 sx=512+1.010152578*(x-y);sy=384+.505076289*(x+y)-1.23717916*nz
 crop=(round(sx)-256,round(sy)-256,round(sx)+256,round(sy)+256)
 for col,(f,label)in enumerate(zip(files,('Original full software','Metal common world','Metal native projector'))):
  im=Image.open(f).convert('RGB');whole.paste(im.resize((512,384)),(col*512,row*408+24));detail.paste(im.crop(crop),(col*512,row*536+24));w.text((col*512+4,row*408+5),f'capture {i} / pulse {i+1}: {label}',fill='white');d.text((col*512+4,row*536+5),f'capture {i} / pulse {i+1}: {label}',fill='white')
 pairs.append(dict(capture=i,native_tick=i,pulse=i+1,crop=crop,images=[dict(path=str(f),sha256=sha(f))for f in files]))
whole.save(out/'whole-life.png');detail.save(out/'detail.png')
rng=[]
for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(m/'common-domain/run.log').read_text()):
 render=int(render);actual=int(count);expect=1 if render==1 else trace[render-2]['state']['rng_draws']+1;rng.append(dict(before_render=render,native_rng_draws=expect,metal_rng_draws=actual,match=actual==expect))
assert all(r['match'] for r in rng),rng
r=dict(status='ready_for_independent_review',effect='FireBall',pairs=pairs,rng_controls=rng,cadence='24Hz; warmup 1; capture N follows original pulse N+1',source_policy='32 white scene input; native ambient 38/255 and directional 1; exact ARGB4444 format/software alpha policy; authored LVERTEX unlit source colors',crop_policy='512-square centered on original owner projection, same rectangle for all three images; no image registration or fitted geometry/color',native_manifest=dict(path=str(n/'manifest.json'),sha256=sha(n/'manifest.json')),metal_manifests=[dict(path=str(m/k/'manifest.json'),sha256=sha(m/k/'manifest.json'))for k in('common-domain','native-domain')],panels=[dict(path=str(f),sha256=sha(f))for f in(out/'whole-life.png',out/'detail.png')],limits=['resource/audio/world interfaces are explicit fixture boundaries','full natural map/spell caller and exact device framebuffer remain open','shared common-world/native projection and software quantization differences remain'])
(out/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('RNG controls',rng);print('manifest',sha(out/'pair-manifest.json'))
