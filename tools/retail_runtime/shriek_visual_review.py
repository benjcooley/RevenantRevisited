#!/usr/bin/env python3
"""Freeze original/actual Metal null-spell Shriek, no fitted poses or colors."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from skin_visual_review import sha

def review(native,common,domain,state,previous,alpha,output):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads((native/'manifest.json').read_text());ms=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert json.loads((state/'manifest.json').read_text())['status']=='pass';assert json.loads((alpha/'review.json').read_text())['status']=='pass'
 for folder,m in zip((common,domain),ms):
  assert m['status']=='pass' and m['scenario']['effect']=='TShriek_BESPOKE_NULLSPELL' and m['scenario']['frames']==50
  log=(folder/'run.log').read_text();assert log.count('[shriek] exact authored meshes=4')==1
  for t,counter,draws,scale,pos in re.findall(r'shriek-phase\] tick=(\d+) counter=(\d+) draws=(\d+) first_scale=([^ ]+) first_position=([^\n]+)',log):
   r=ref['cases'][int(t)];assert int(counter)==r['state']['counter'];assert abs(float(scale)-r['state']['scales'][0])<1e-7 and abs(float(pos)-r['state']['positions'][0])<1e-6
 assert ms[0]['binary_sha256']==ms[1]['binary_sha256']==json.loads((alpha/'review.json').read_text())['binary_sha256']
 old=json.loads((previous/'manifest.json').read_text());assert [f['sha256']for f in old['frames']]==[f['sha256']for f in ms[0]['frames']]
 ticks=(2,14,15,16,18,20,25,30,35,40,44,45,46,48,50);panel=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(960,len(ticks)*265));pd=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);cases=[]
 for row,t in enumerate(ticks):
  n=ref['cases'][t];paths=[Path(n['image'])]+[Path(m['frames'][t-2]['path'])for m in ms];hashes=[n['image_sha256']]+[m['frames'][t-2]['sha256']for m in ms];assert list(map(sha,paths))==hashes
  c=dict(tick=t,alive=n['state']['alive'],draws=len(n['draws']),images=list(map(str,paths)),sha256=hashes,pixels=[],bounds=[])
  for col,(path,label)in enumerate(zip(paths,('Original','Common Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');c['pixels'].append(sum(p!=(0,0,0)for p in im.getdata()));c['bounds'].append(im.getbbox());panel.paste(im.resize((320,240)),(col*320,row*265+25));pd.text((col*320+4,row*265+4),f'{label} tick{t}',fill='white')
   detail.paste(im.crop((260,150,480,280)).resize((320,240),Image.Resampling.NEAREST),(col*320,row*265+25));dd.text((col*320+4,row*265+4),f'{label} tick{t} fixed crop',fill='white')
  if t<=15 or t>=46:assert not any(c['pixels'])
  cases.append(c)
 panel.save(output/'contact.png');detail.save(output/'detail.png');report=dict(status='awaiting_independent_review',accepted=False,retail_name='shriek',retail_type_id='0xad92bc12',cases=cases,binary_sha256=ms[0]['binary_sha256'],input_manifest_sha256={str(p):sha(p/'manifest.json')for p in(native,common,domain,state,previous,alpha)},alpha_review_sha256=sha(alpha/'review.json'),default_common_regression_identical_frames=50,contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,scope='Null-spell original owner counter0..46,8rings cycling4authored meshes, literal Init RNG/start delays, scale .12 and position3 steps, birth cutoff45/kill46. Full original registry/owner/leaf/RenderObject/CopyVertices FVF_LVERTEX/ARGB4444 raster; resource/COM and base bookkeeping/script/light/map notification boundaries. Native-domain Metal opts into proven perRGB565 source-contribution floor only; overlapping destination rounding, framebuffer precision and exact pixels remain open. Common-world50frames byte-identical before/after shared opt-in addition; common projection and subquantum skirt retained. Natural invoker/bone placement, damage/audio/device/end-to-end caller remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print([(c['tick'],c['pixels'],c['bounds'])for c in cases]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for n in('native','common','domain','state','previous','alpha','output'):p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.previous,a.alpha,a.output)
