"""Exact-time three-window original/Metal Maelstrom comparison, without registration."""
import argparse,json,hashlib,re
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--evidence',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--variant',choices=['maelstrom','ymaelstrom'],default='maelstrom');a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();pin=lambda p:dict(path=str(p.resolve()),sha256=sha(p));prefix=a.variant;label='YMaelstrom'if prefix=='ymaelstrom'else'Maelstrom';views=('left','right','upper');centres=((1200,550),(720,550),(960,850));traces={v:json.loads((a.evidence/f'{prefix}-native-{v}/trace.json').read_text())for v in views};manifests={}
for view,centre in zip(views,centres):
 assert traces[view]['policy']['viewport']==[1920,1080]and traces[view]['policy']['screen_center']==list(centre)
 for domain,depth in [('native',[0,65535]),('common',[2238,3262])]:
  path=a.evidence/('ymaelstrom-metal'if prefix=='ymaelstrom'else'maelstrom-metal-corrected')/view/(domain+'-domain')/'manifest.json';m=json.loads(path.read_text());assert m['status']=='pass'and m['depth_range']==depth
  assert m['simulation_fps']==24 and m['capture_stride']==8 and m['sample_simulation_ticks']==list(range(2,259,8));manifests[(view,domain)]=(path,m)
assert len({m['binary_sha256']for p,m in manifests.values()})==1
samples=list(range(1,260,16))
def metal_frame(view,domain,t):
 m=manifests[(view,domain)][1];indices=[i for i,r in enumerate(m['timing'])if round(float(r['simulation_seconds'])*24)-1==t];assert len(indices)==1;return Path(m['frames'][indices[0]]['path'])
def differing(x,y):
 r,g,b=ImageChops.difference(x,y).split();return x.width*x.height-ImageChops.lighter(ImageChops.lighter(r,g),b).histogram()[0]
def composite(left,right,upper):
 assert left.size==right.size==upper.size==(1920,1080)
 overlaps=dict(left_right=differing(left.crop((480,0,1920,1080)),right.crop((0,0,1440,1080))),upper_left=differing(upper.crop((0,300,1680,1080)),left.crop((240,0,1920,780))),upper_right=differing(upper.crop((240,300,1920,1080)),right.crop((0,0,1680,780))))
 out=Image.new('RGB',(2400,1380));out.paste(upper.crop((0,0,1920,300)),(240,0));out.paste(left.crop((0,0,1200,1080)),(0,300));out.paste(right.crop((720,0,1920,1080)),(1200,300));return out,overlaps
whole=Image.new('RGB',(1800,len(samples)*369));detail=Image.new('RGB',(2400,len(samples)*714));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail);pairs=[]
for row,t in enumerate(samples):
 columns=[];images=[]
 for col,domain in enumerate(('original','common','native')):
  files=[a.evidence/f'{prefix}-native-{v}'/f'native-{t:03}.png'if domain=='original'else metal_frame(v,domain,t)for v in views]
  im,overlaps=composite(*[Image.open(f).convert('RGB')for f in files]);images.append(im);bbox=im.getbbox();assert bbox is None or(0<bbox[0]<bbox[2]<2400 and 0<bbox[1]<bbox[3]<1380)
  columns.append(dict(domain=domain,views=[pin(f)for f in files],bbox=bbox,overlap_differing_pixels=overlaps));whole.paste(im.resize((600,345)),(col*600,row*369+24));w.text((col*600+4,row*369+4),f'{label} pulse{t+1}: {domain}',fill='white')
 for col,index in enumerate((0,2)):
  detail.paste(images[index].resize((1200,690)),(col*1200,row*714+24));d.text((col*1200+4,row*714+4),f'Pulse{t+1}: '+('Original software'if index==0 else'Actual Metal native-domain'),fill='white')
 phase=Image.new('RGB',(4800,1380));phase.paste(images[0],(0,0));phase.paste(images[2],(2400,0));phase.save(a.output/f'phase-{t:03}.png');pairs.append(dict(capture=t,pulse=t+1,columns=columns,actual_size_pair=pin(a.output/f'phase-{t:03}.png')))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for (view,domain),(path,m)in manifests.items():
 log=(path.parent/'run.log').read_text();assert int(re.search(r'\[vfx\] close \(frames=(\d+)\)',log)[1])==258
 for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log):
  render=int(render);original=traces[view]['initial']['rng_draws']if render==1 else traces[view]['records'][render-2]['after']['rng_draws'];assert original==int(count);rng.append(dict(view=view,domain=domain,render=render,draws=original))
r=dict(status='ready_for_independent_review',effect=prefix,type_id='0xae5eeb26'if prefix=='ymaelstrom'else'0xaeaeeb26',pairs=pairs,rng_controls=rng,native_traces=[pin(a.evidence/f'{prefix}-native-{v}/trace.json')for v in views],metal_manifests=[pin(p)for p,m in manifests.values()],binary_sha256=next(iter(manifests.values()))[1]['binary_sha256'],panels=[pin(a.output/f)for f in('whole-life.png','detail.png')],scope='Own default245-END owner, three actual registered scrolltex controllers, blend16/80 and full normal-lit original Render. Null spell, silent play and fixed original-domain coordinates/light/seed. Actual OF_KILL plus blank tail.',composition='Three independent supported1920x1080 fixed windows: centres1200,550;720,550;960,850. Combined2400x1380 copies top300 upper rows atX240, lower-left1200 columns atY300, then lower-right columns720..1919 atX1200/Y300. All full triangles fit one window. Overlaps are measured, not blended or fitted; raw windows remain pinned.',sampling='All native260ticks and actual258renders retain24Hz cadence;Native pixels sampled every16ticks; actual33 PNGs every8ticks. Original capture = round(actual simulation_seconds*24)-1.',limits=['No natural boss/spell/map/audio/device caller or exact framebuffer equality.','Independent view clipping/raster differences remain explicit; no claim of one exact giant framebuffer.','Ordinary/common projection and full hardware geometry remain diagnostics; only native/software opts into the original primitive-span guard.','Original x87 UV update versus float GPU offset/add precision is explicitly measured.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'))
