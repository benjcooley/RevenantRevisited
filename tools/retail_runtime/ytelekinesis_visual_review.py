"""Freeze six fixed-view whole YTelekinesis coverage without camera/geometry fitting."""
import argparse,json,hashlib,re,struct
from pathlib import Path
from PIL import Image,ImageDraw,ImageChops
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True);p.add_argument('--coverage',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();pin=lambda p:dict(path=str(p.resolve()),sha256=sha(p))
views=[(f'r{r}c{c}',x,y)for r,y in enumerate([960,420])for c,x in enumerate([2700,1560,420])];samples=[1+8*i for i in range(24)];traces={n:json.loads((a.native/n/'trace.json').read_text())for n,x,y in views};manifests={};rng=[]
for n,x,y in views:
 assert traces[n]['viewport']==[1920,1080]and traces[n]['center']==[x,y]
 assert [r['state']for r in traces[n]['records']]==[r['state']for r in traces['r0c0']['records']]
 for domain,depth in [('native',[0,65535]),('common',[2238,3262])]:
  folder=a.metal/n/(domain+'-domain');m=json.loads((folder/'manifest.json').read_text());assert m['status']=='pass'and m['depth_range']==depth and m['actual_viewport']==[1920,1080]and m['actual_camera']==[x,y];manifests[n,domain]=m
  log=(folder/'run.log').read_text();assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1)' in log
  phases=[round(float(t['simulation_seconds'])*24)-1 for t in m['timing']];assert phases==samples,(n,domain,phases)
  assert len(m['frames'])==24 and len(m['timing'])==24
  for tick,count,draws in re.findall(r'\[ytelekinesis-phase\] tick=(\d+) rocks=(\d+) random=(\d+)',(folder/'run.log').read_text()):
   tick,count,draws=map(int,(tick,count,draws));assert 1<=tick<=165 and count==4 and draws==45;rng.append(dict(view=n,domain=domain,pulse=tick,draws=draws))
  assert len(re.findall(r'\[ytelekinesis-phase\]',(folder/'run.log').read_text()))==165
assert len({m['binary_sha256']for m in manifests.values()})==1
coverage=json.loads(a.coverage.read_text());assert coverage['status']=='pass'and coverage['all_native_triangles']==39600

def compose(images):
 out=Image.new('RGB',(4200,1620));overlap=[]
 for r in range(2):
  for c in range(3):
   im=images[f'r{r}c{c}'];assert im.size==(1920,1080);w=1140 if c<2 else 1920;h=540 if r==0 else 1080
   out.paste(im.crop((0,0,w,h)),(1140*c,540*r))
   others=[]
   if c<2:others.append(('horizontal',im.crop((1140,0,1920,1080)),images[f'r{r}c{c+1}'].crop((0,0,780,1080))))
   if r==0:others.append(('vertical',im.crop((0,540,1920,1080)),images[f'r1c{c}'].crop((0,0,1920,540))))
   for label,one,two in others:
    diff=ImageChops.difference(one,two);red,green,blue=diff.split();mask=ImageChops.lighter(ImageChops.lighter(red,green),blue);overlap.append(dict(view=f'r{r}c{c}',direction=label,different_pixels=one.width*one.height-mask.histogram()[0]))
 return out,overlap
whole=Image.new('RGB',(2520,len(samples)*348));detail=Image.new('RGB',(1280,len(samples)*184));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);pairs=[]
for row,t in enumerate(samples):
 composites=[];columns=[]
 for col,domain in enumerate(['original','common','native']):
  files={n:a.native/n/f'native-{t:03}.png'if domain=='original'else Path(manifests[n,domain]['frames'][row]['path'])for n,x,y in views};images={n:Image.open(f).convert('RGB')for n,f in files.items()};im,overlap=compose(images);composites.append(im)
  whole.paste(im.resize((840,324)),(col*840,row*348+24));wd.text((col*840+4,row*348+4),f'YTelekinesis pulse{t+1}: {domain}, fixed3x2 views',fill='white');columns.append(dict(domain=domain,views=[pin(f)for f in files.values()],overlap=overlap,bounds=im.getbbox()))
 crops=[]
 for i,r in enumerate(traces['r0c0']['records'][t]['state']['rocks']):
  x,y,z=r['position'];cx=round(2700+1.010152578*(x-y));cy=round(960+.505076289*(x+y)-1.23717916*z);crop=(cx-80,cy-80,cx+80,cy+80);crops.append(crop)
  for dcol,index in enumerate([0,2]):detail.paste(composites[index].crop(crop),(i*320+dcol*160,row*184+24));dd.text((i*320+dcol*160+3,row*184+4),f'{t}: rock{i} '+('original'if index==0 else'Metal'),fill='white')
 raw=Image.new('RGB',(8400,1620));raw.paste(composites[0]);raw.paste(composites[2],(4200,0));path=a.output/f'phase-{t:03}.png';raw.save(path)
 pairs.append(dict(native_record=t,pulse=t+1,columns=columns,actual_size_pair=pin(path),detail_crops=crops))
whole.save(a.output/'whole-life.png');detail.save(a.output/'detail.png')
report=dict(status='ready_for_independent_review',effect='YTelekinesis',type_id='0xaeaeeb28',pairs=pairs,binary_sha256=next(iter(manifests.values()))['binary_sha256'],coverage=pin(a.coverage),native_traces=[pin(a.native/n/'trace.json')for n,x,y in views],metal_manifests=[pin(a.metal/n/(d+'-domain')/'manifest.json')for n,x,y in views for d in ['native','common']],rng_controls=rng,panels=[pin(a.output/f)for f in ['whole-life.png','detail.png']],configuration=dict(viewport=[1920,1080],x_centers=[2700,1560,420],y_centers=[960,420],union=[4200,1620],render_hz=24,snapshot_stride=8,seed=1,player_position=[0,0,0],spell=None,lighting=dict(source_ambient_rgb=[38,38,38],source_directional_rgb=[1,1,1],actual_metal_ambient=[0.149019614]*3,point_lights=0)),composition='Regular3x2 immutable windows, horizontal step1140/overlap780, vertical step540/overlap540. Composite uses top-left fixed ownership cuts; all raw windows and per-overlap pixel differences retained. Detail uses identical160px crops centered from native state, with coordinates recorded; full composites remain authoritative. No geometry, color, scale, seed or phase fit.',scope='Own original registration/Init/ownerPulse/customAnimate/fullRender/FVF112 normal lighting/RGB565 software raster against actual Metal. Explicit stationary global player position and null spell. Four moving/accelerating rotating chunks cross the player center, expire individually but remain rendered, then actual kill165 and blank tail.',limits=['Natural player movement, spell damage/actor reactions/audio and map dispatch are open.','Resource/COM residency, base animator initialization and map notifications are explicit boundaries.','Overlap raster differences, UV/light quantization, device backend and exact framebuffer equality remain open.','Unsupported large viewport and clipped1280x960 references remain diagnostic; only supported fixed grid establishes whole trajectory coverage.'])
(a.output/'pair-manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('pair',sha(a.output/'pair-manifest.json'),'phases',len(pairs),'rng',len(rng))
