#!/usr/bin/env python3
"""Freeze exact-phase original Fog vs actual common/native-domain Metal."""
import argparse,json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()

def review(native,common,domain,output):
 output.mkdir(parents=True,exist_ok=False)
 ref=json.loads((native/'manifest.json').read_text());assert ref['warm_replays']==2
 captures=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 checkpoints=[]
 for index,(path,m)in enumerate(zip((common,domain),captures)):
  expected=dict(effect='TFogEffect_BESPOKE',frames=120,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[320,240],rectangle=[0,0,640,480],resolution=[640,480],background='black',native_domain=bool(index))
  assert m['status']=='pass'and all(m['scenario'][k]==v for k,v in expected.items())
  log=(path/'run.log').read_text();assert f'[native-domain] enabled={index} camera_z=0 owner=RzRxRyT Fog'in log
  rows=re.findall(r'\[fog-phase\] tick=(\d+) rng_draws=(\d+) c2=([^ ]+) a2=([^ ]+) offset2=\(([^)]+)\)',log)
  assert len(rows)==3
  checks=[]
  for tick,count,color,alpha,offset in rows:
   t=int(tick);s=ref['cases'][t]['state'];assert int(count)==s['rng_calls']
   values=[float(color),float(alpha)]+list(map(float,offset.split(',')));expected=s['colors'][4:6]+s['offsets'][6:9]
   assert max(abs(a-b)for a,b in zip(values,expected))<5e-9
   checks.append(dict(tick=t,rng_calls=int(count),vertex2=values))
  checkpoints.append(checks)
 selected=(2,5,10,20,40,60,80,100,120);panel=Image.new('RGB',(960,270*len(selected)));draw=ImageDraw.Draw(panel);cases=[]
 for row,tick in enumerate(selected):
  n=ref['cases'][tick];paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures]
  hashes=[n['sha256']]+[m['frames'][tick-2]['sha256']for m in captures];assert [sha(p)for p in paths]==hashes
  images=[Image.open(p).convert('RGB')for p in paths]
  counts=[sum(any(c)for c in im.getdata())for im in images]
  cases.append(dict(tick=tick,images=list(map(str,paths)),sha256=hashes,nonzero_pixels=counts,rng_calls=n['state']['rng_calls']))
  for col,(im,label)in enumerate(zip(images,('Original','Common-world Metal','Native-domain Metal'))):
   panel.paste(im.resize((320,240),Image.Resampling.NEAREST),(col*320,row*270+26));draw.text((col*320+3,row*270+5),f'{label} tick{tick}',fill='white')
 panel.save(output/'contact.png')
 detail=Image.new('RGB',(960,330));d=ImageDraw.Draw(detail)
 for col,path in enumerate(cases[0]['images']):
  im=Image.open(path).convert('RGB').crop((200,175,440,305)).resize((960//3,260),Image.Resampling.NEAREST)
  detail.paste(im,(col*320,35));d.text((col*320+3,8),('Original','Common-world','Native-domain')[col]+' tick2',fill='white')
 detail.save(output/'detail.png')
 report=dict(status='review_ready',accepted=False,retail_name='Fog',retail_type_id='0xc24b2a85',cases=cases,live_rng_checkpoints=checkpoints,
  input_manifest_sha256=[sha(p/'manifest.json')for p in(native,common,domain)],binary_sha256=[m['binary_sha256']for m in captures],
  contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,
  scope='Whole36-vertex/50-triangle untextured Fog; exact original state/RNG range/order, source packed vertex RGB5/A5, native owner/projector fixture. Real120-frame films per projection. Original prelit alpha lookup uses(A5+1)/32 and writes depth; GPU vertex interpolation/blend/fragmentdepth retains higher precision than original scanlines/RGB565 and flat triangledepth. Ordinary world projection/natural caller and map integration remain separate. Ambient is irrelevant to original prelit vertex colors.')
 # Actual draw log distinguishes literal dynamicRGBA5 from the historical
 # normalized-color carrier, without inferring policy from an output filename.
 if 'rgba0=(22,22,22,3)' in (domain/'run.log').read_text():
  report['native_domain_dynamic_rgba5']=True
  report['scope']='Whole36-vertex/50-triangle Fog; actual original leaf/owner/FVF_LVERTEX/raster with declared resource/base-animator/extents/RenderObject dispatch boundaries. Real120-frame films per projection and original RNG checkpoints. Native-domain carries literalRGBA5, truncated source contribution, integer screen samples and flat maximum triangledepth. Original fixed8 scanline interpolation, destinationRGB565 rounding and triangleedge ownership remain residuals. Common rendering remains normalized-color policy. Naturalcaller/device/exactpixels are open; ambient is irrelevant to prelit colors.'
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'phase triples')

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','common','domain','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.output)
