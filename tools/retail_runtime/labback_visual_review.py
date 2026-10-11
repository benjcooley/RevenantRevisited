#!/usr/bin/env python3
"""Pin whole original Labback/current Metal loop and matched visual phases."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageDraw
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(native,common,domain,state,output):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads((native/'manifest.json').read_text());proof=json.loads((state/'manifest.json').read_text());captures=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert proof['status']=='pass' and proof['ticks']==402 and proof['default_zero_identical']
 for index,(path,m)in enumerate(zip((common,domain),captures)):
  assert m['status']=='pass'and m['scenario']['native_domain']==bool(index)
  assert m['scenario']['effect']=='TLabback_AUTHORED_TAGS'and m['scenario']['frames']==402
  assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in(path/'run.log').read_text()
 ticks=(2,16,60,100,200,300,399,400,401);panel=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(1200,len(ticks)*375));draw=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);cases=[]
 for row,tick in enumerate(ticks):
  n=next(c for c in ref['cases']if c['tick']==tick);paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures];hashes=[n['image_sha256']]+[m['frames'][tick-2]['sha256']for m in captures];assert list(map(sha,paths))==hashes
  cases.append(dict(tick=tick,images=list(map(str,paths)),sha256=hashes,native_draws=len(n['draws'])))
  for col,(path,label)in enumerate(zip(paths,('Original','Common-world Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');panel.paste(im.resize((320,240)),(col*320,row*265+25));draw.text((col*320+2,row*265+5),f'{label} tick{tick}',fill='white');detail.paste(im.crop((225,145,425,320)).resize((400,350)),(col*400,row*375+25));dd.text((col*400+2,row*375+5),f'{label} tick{tick} same crop 2x',fill='white')
 panel.save(output/'contact.png');detail.save(output/'detail.png')
 actual=[];log=(domain/'run.log').read_text()
 for t,frame,draws,credit,x,y,z in re.findall(r'labback-phase\] tick=(\d+) frame=(\d+) dust=(\d+) credit=([^ ]+) emitter=\(([^,]+),([^,]+),([^)]+)\)',log):
  n=ref['cases'][int(t)];assert int(draws)+3==len(n['draws']);actual.append(dict(tick=int(t),sparks=int(draws)))
 report=dict(status='awaiting_independent_review',accepted=False,retail_type_id='0xdcc4011d',retail_name='Labback',cases=cases,actual_metal_draw_checks=actual,input_manifest_sha256={str(p):sha(p/'manifest.json')for p in(native,common,domain,state)},binary_sha256=[m['binary_sha256']for m in captures],contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,scope='State0 complete400-frame loop including399-to0 boundary, three independently rotating authored spheres and separate #sparks, inclusive keys and persistent50-slot pool. Original mode16 CULL_NONE; positive-face-only negative control omits all spheres. Whole original Animate/Render software reference vs actual isolated Metal; source38/dir1, quality0, seed1, flatground0, owner/camera0. Native-domain policy explicit; common-world residual shown. Natural caller/end-state trigger/audio and exact raster equality remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','common','domain','state','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.output)
