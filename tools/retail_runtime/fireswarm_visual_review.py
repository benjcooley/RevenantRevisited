#!/usr/bin/env python3
"""Freeze unfitted whole FireSwarm original/common/native-domain lifecycle."""
import argparse,json,hashlib,re
from pathlib import Path
from PIL import Image,ImageDraw

def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(native,common,domain,baseline,out):
 out.mkdir(parents=True,exist_ok=False);n=json.load(open(native/'manifest.json'));cm=json.load(open(common/'manifest.json'));dm=json.load(open(domain/'manifest.json'));bm=json.load(open(baseline/'manifest.json'));assert n['status']=='reference_complete'and cm['status']==dm['status']==bm['status']=='pass';assert cm['binary_sha256']==dm['binary_sha256']
 for index,(folder,m)in enumerate(((common,cm),(domain,dm))):
  s=m['scenario'];expected=dict(effect='TFireSwarmEffect_BESPOKE',frames=80,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[960,540],rectangle=[0,0,1920,1080],resolution=[1920,1080],native_domain=bool(index))
  assert all(s[k]==v for k,v in expected.items());log=(folder/'run.log').read_text();assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
  assert f'[native-domain] enabled={index} camera_z=0 owner=RzRxRyT TFireSwarmEffect_Bespoke'in log
  for frame in m['frames']:assert sha(frame['path'])==frame['sha256']and Image.open(frame['path']).size==(1920,1080)
 before=[x['sha256']for x in bm['frames']];after=[x['sha256']for x in cm['frames']];assert before==after,'Ordinary common movie changed'
 panel=Image.new('RGB',(1920,390*len(n['cases'])));d=ImageDraw.Draw(panel);rows=[]
 for row,c in enumerate(n['cases']):
  t=c['tick'];paths=[Path(c['path']),Path(cm['frames'][t-2]['path']),Path(dm['frames'][t-2]['path'])];images=[Image.open(p).convert('RGB')for p in paths];hashes=[sha(p)for p in paths];assert hashes[0]==c['sha256']
  if t in(2,15,25,75,76,80):assert images[0].getbbox()is None and images[2].getbbox()is None,(t,'source empty phase not reproduced')
  rows.append(dict(tick=t,paths=list(map(str,paths)),sha256=hashes,bbox=[im.getbbox()for im in images],guard_accepted=c['guard_accepted'],state=c['state']))
  for col,(im,label)in enumerate(zip(images,('Original software','Common world','Native domain'))):panel.paste(im.resize((640,360),Image.Resampling.NEAREST),(col*640,row*390+25));d.text((col*640+4,row*390+4),f'{label} tick{t}',fill='white')
 panel.save(out/'contact.png');selected=(27,28,36,60,72,74);detail=Image.new('RGB',(1680,720*len(selected)));d=ImageDraw.Draw(detail)
 for row,t in enumerate(selected):
  c=next(c for c in rows if c['tick']==t)
  for col,path in enumerate(c['paths']):
   im=Image.open(path).convert('RGB').crop((680,0,1240,700));detail.paste(im,(col*560,row*720+20));d.text((col*560+4,row*720+3),f'{("Original","Common","Native-domain")[col]} tick{t}',fill='white')
 detail.save(out/'detail.png');report=dict(status='review_ready',accepted=False,type_id='0x582c1e78',cases=rows,binary_sha256=cm['binary_sha256'],native_manifest_sha256=sha(native/'manifest.json'),metal_manifests=[dict(path=str(p/'manifest.json'),sha256=sha(p/'manifest.json'))for p in(common,domain,baseline)],default_identical_frames=80,contact_sha256=sha(out/'contact.png'),detail_sha256=sha(out/'detail.png'),configuration=n['configuration'],appearance_fitted=False,scope='Actual complete authored cylinder lifecycle in supported1920x1080viewport. Native-domain software primitive admission matches original literal640/480limits; early software-empty phases do not establish hardware appearance. Ordinary geometry retained byte-identical. Original leaf/owner/carrier/normal/projector/raster execute with resource/base/GetObject/RenderObject dispatch boundaries. Native sample/raster/destination quantization, naturalcaller and device limits remain; no exactpixels claim.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('review_ready',len(rows),cm['binary_sha256']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for n in('native','common','domain','baseline','output'):p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.baseline,a.output)
