#!/usr/bin/env python3
"""Pin original/actual Metal Puke null-spell lifecycle without fitted inputs."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from skin_visual_review import sha

def review(native,common,domain,state,output):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads((native/'manifest.json').read_text());ms=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert json.loads((state/'manifest.json').read_text())['status']=='pass'
 for folder,m in zip((common,domain),ms):
  assert m['status']=='pass' and m['scenario']['effect']=='TPuke_BESPOKE_NULLSPELL' and m['scenario']['frames']==32
  log=(folder/'run.log').read_text();assert log.count('[puke] exact authored meshes=3')==1;assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
  for t,life,scale,draws,x,y,z in re.findall(r'puke-phase\] tick=(\d+) life=(\d+) scale=([^ ]+) draws=(\d+) first=\(([^,]+),([^,]+),([^\)]+)\)',log):
   r=ref['cases'][int(t)];assert int(life)==r['state']['life'] and int(draws)==len(r['draws']);assert abs(float(scale)-r['state']['scale'])<1e-7
   assert max(abs(a-b)for a,b in zip(map(float,(x,y,z)),r['state']['positions'][0]))<1e-6
 assert ms[0]['binary_sha256']==ms[1]['binary_sha256']
 ticks=(2,14,15,16,17,18,19,20,21,22,23,24,25,26,30);panel=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(960,len(ticks)*345));pd=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);cases=[]
 for row,t in enumerate(ticks):
  n=ref['cases'][t];paths=[Path(n['image'])]+[Path(m['frames'][t-2]['path'])for m in ms];hashes=[n['image_sha256']]+[m['frames'][t-2]['sha256']for m in ms];assert list(map(sha,paths))==hashes
  c=dict(tick=t,life=n['state']['life'],alive=n['state']['alive'],draws=len(n['draws']),images=list(map(str,paths)),sha256=hashes,pixels=[],bounds=[])
  for col,(path,label)in enumerate(zip(paths,('Original','Common Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');c['pixels'].append(sum(p!=(0,0,0)for p in im.getdata()));c['bounds'].append(im.getbbox());panel.paste(im.resize((320,240)),(col*320,row*265+25));pd.text((col*320+4,row*265+4),f'{label} tick{t}',fill='white')
   detail.paste(im.crop((300,210,380,290)).resize((320,320),Image.Resampling.NEAREST),(col*320,row*345+25));dd.text((col*320+4,row*345+4),f'{label} tick{t} identical crop x4',fill='white')
  if t<16 or t>=26:assert not any(c['pixels'])
  cases.append(c)
 panel.save(output/'contact.png');detail.save(output/'detail.png');report=dict(status='awaiting_independent_review',accepted=False,retail_name='puke',retail_type_id='0xad92bc11',cases=cases,binary_sha256=ms[0]['binary_sha256'],input_manifest_sha256={str(p):sha(p/'manifest.json')for p in(native,common,domain,state)},contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,scope='Actual null-spell default branch: initial45, source0/0/0,25 literal slots, original grouped three-mesh Render order, delay/birth/fall/shrink/kill26 and blanktail. Native exact original owner NextFrame/Pulse/Kill, leaf Init/Animate/Render/RenderObject/nativeowner/FVF112/ARGB4444/software raster; resource/COM, base animator bookkeeping/extents and script/light/map-flag services are explicit boundaries. Source38/dir1,seed1,owner/camera0,native-domain isolated Metal compared; ordinary-world residual separately retained. Small output follows native null-spell branch, not invented actor-mouth lift. Natural invoker attachment/damage/audio/device/exactpixel/end-to-end caller remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print([(c['tick'],c['pixels'],c['bounds'])for c in cases]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for n in('native','common','domain','state','output'):p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.output)
