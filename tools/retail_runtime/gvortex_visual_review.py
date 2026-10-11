"""Two fixed overlapping camera windows cover the full authored GVortex extent."""
import argparse,json,hashlib,re
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native-lower',type=Path,required=True);p.add_argument('--native-upper',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--sparse-metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();pin=lambda p:dict(path=str(p.resolve()),sha256=sha(p));traces={v:json.loads(getattr(a,'native_'+v).joinpath('trace.json').read_text())for v in('lower','upper')};samples=[1,9,17,25,33,49,65,81,97,113,129,145,161,177,193,209,225,233,241,257]
def metal_dir(view,domain):return (a.metal if view=='lower' else a.sparse_metal)/view/(domain+'-domain')
def metal_frame(view,domain,capture):
 folder=metal_dir(view,domain);m=json.loads((folder/'manifest.json').read_text())
 rows=[i for i,t in enumerate(m['timing']) if round(float(t['simulation_seconds'])*24)-1==capture]
 assert len(rows)==1,(view,domain,capture,rows)
 return Path(m['frames'][rows[0]]['path'])
for v,cy in [('lower',800),('upper',1500)]:
 assert traces[v]['policy']['viewport']==[1920,1080]and traces[v]['policy']['screen_center']==[960,cy]
 for d,depth in [('native',[0,65535]),('common',[2238,3262])]:
  m=json.loads((metal_dir(v,d)/'manifest.json').read_text());assert m['status']=='pass'and m['depth_range']==depth
whole=Image.new('RGB',(1536,len(samples)*499));detail=Image.new('RGB',(1920,len(samples)*914));w=ImageDraw.Draw(whole);d=ImageDraw.Draw(detail);pairs=[];overlap=[]
def composite(lo,hi):
 assert lo.size==hi.size==(1920,1080)
 difference=ImageChops.difference(hi.crop((0,700,1920,1080)),lo.crop((0,0,1920,380)));r,g,b=difference.split();mask=ImageChops.lighter(ImageChops.lighter(r,g),b);nonzero=1920*380-mask.histogram()[0]
 out=Image.new('RGB',(1920,1780));out.paste(hi.crop((0,0,1920,700)),(0,0));out.paste(lo,(0,700));return out,nonzero
for row,t in enumerate(samples):
 views=[];composites=[]
 for col,domain in enumerate(('original','common','native')):
  files=[getattr(a,'native_'+v)/f'native-{t:03}.png'if domain=='original'else metal_frame(v,domain,t)for v in('lower','upper')]
  im,n=composite(*[Image.open(f).convert('RGB')for f in files]);composites.append(im);views.append(dict(domain=domain,views=[pin(f)for f in files],composite_bbox=im.getbbox(),overlap_different_pixels=n));whole.paste(im.resize((512,475)),(col*512,row*499+24));w.text((col*512+4,row*499+4),f'GVortex pulse{t+1}: {domain} / two views',fill='white')
 for col,index in enumerate((0,2)):
  detail.paste(composites[index].resize((960,890)),(col*960,row*914+24));d.text((col*960+4,row*914+4),f'GVortex pulse{t+1}: '+('Original software'if index==0 else'Actual Metal native projector'),fill='white')
 phase=Image.new('RGB',(3840,1780));phase.paste(composites[0],(0,0));phase.paste(composites[2],(1920,0));phase.save(a.output/f'phase-{t:03}.png');pairs.append(dict(capture=t,pulse=t+1,columns=views,actual_size_pair=pin(a.output/f'phase-{t:03}.png')))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png');rng=[]
for v in('lower','upper'):
 for domain in('common','native'):
  for render,count in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',(metal_dir(v,domain)/'run.log').read_text()):
   render=int(render);expect=traces[v]['initial']['rng_draws']if render==1 else traces[v]['records'][render-2]['after']['rng_draws'];rng.append(dict(view=v,domain=domain,before_render=render,original=expect,metal=int(count),match=int(count)==expect))
assert all(c['match']for c in rng)
r=dict(status='ready_for_independent_review',effect='gvortex',type_id='0xad99bd33',pairs=pairs,sample_mapping='Original capture = round(recorded simulation_seconds * 24) - 1; lower full film and upper stride8 film use identical 24Hz rendering.',rng_controls=rng,native_traces=[pin(getattr(a,'native_'+v)/'trace.json')for v in('lower','upper')],metal_manifests=[pin(metal_dir(v,d)/'manifest.json')for v in('lower','upper')for d in('common','native')],panels=[pin(a.output/f)for f in('whole-life.png','detail.png')],composition='Two independently rendered1920x1080 windows, centres960,800 and960,1500. Exact integer camera delta700; upper rows0..699 followed by entire lower window at offset700 creates1920x1780 coverage. Overlap is separately measured, not blended or aligned. No geometry, color, UV, scale or time fit. Whole panel downsamples512x475, detail960x890; individual phase pairs retain actual pixels.',scope='Full original default owner, blend controller, normal-lit meshes and raster against actual Metal. Explicit null spell and silent play service. Original source mode16 for ordinary meshes and mode80 for four dollar flares. Complete default240-frame END sequence through actual OF_KILL239 and blank tail.',limits=['No natural trap/map/actor/audio/device caller or exact framebuffer acceptance.','Ordinary/common projection and narrower-depth diagnostic differences retained; common full geometry is not altered to imitate software triangle rejection.','Per-window clipping is intentional framing; combined coverage and overlap checks are recorded. Floating/fixed-point raster precision differences remain.','Initial1280x960 clipped native recording is retained as a diagnostic.'])
(a.output/'pair-manifest.json').write_text(json.dumps(r,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'RNG',len(rng))
