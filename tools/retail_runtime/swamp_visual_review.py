#!/usr/bin/env python3
"""Freeze whole original Swamp against actual same-cadence Metal films."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(p):return sha(Path(p).read_bytes())
def review(native,common,domain,state,poses,stride,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((native/'trace.json').read_text());ms=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 for p in(state,poses,stride):assert json.loads((p/'manifest.json').read_text())['status']=='pass'
 assert ms[0]['binary_sha256']==ms[1]['binary_sha256'];actual=[]
 for folder,m in zip((common,domain),ms):
  assert m['status']=='pass'and m['scenario']['effect']=='TSwamp_AUTHORED_TAGS'and m['scenario']['frames']==60
  log=(folder/'run.log').read_text();assert log.count('[swamp] authored cached-emitter')==1;assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
  for values in re.findall(r'\[swamp-phase\] tick=(\d+) frame=(\d+) dust=(\d+) credit=([^ ]+) emitter=\(([^,]+),([^,]+),([^\)]+)\)',log):
   t,frame,dust=map(int,values[:3]);ref=n['records'][t-1]['after'];assert frame==ref['frame'];assert abs(float(values[3])-ref['controllers'][0]['credit'])<1e-5;actual.append(dict(policy=folder.name,tick=t,frame=frame,particle_draws=dust))
  for i,f in enumerate(m['frames']):assert bool(Image.open(f['path']).convert('RGB').getbbox())==(i+2<56),(folder,i)
 ticks=(2,4,9,10,16,26,41,54,55,56,60);whole=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(960,len(ticks)*245));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);rows=[]
 for row,tick in enumerate(ticks):
  ref=n['records'][tick-1];paths=[Path(ref['image'])]+[Path(m['frames'][tick-2]['path'])for m in ms];hashes=[ref['image_sha256']]+[m['frames'][tick-2]['sha256']for m in ms];assert list(map(digest,paths))==hashes;images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1024,768)for im in images);rows.append(dict(tick=tick,paths=list(map(str,paths)),sha256=hashes,bbox=[im.getbbox()for im in images],original_draws=ref['draw_count']))
  for col,(im,label)in enumerate(zip(images,('Original','Common Metal','Native-domain Metal'))):
   whole.paste(im.resize((320,240)),(col*320,row*265+25));wd.text((col*320+4,row*265+4),f'{label} tick{tick}',fill='white');detail.paste(im.crop((352,500,672,720)),(col*320,row*245+25));dd.text((col*320+4,row*245+4),f'{label} actual-size tick{tick}',fill='white')
 whole.save(output/'contact.png');detail.save(output/'detail.png');inputs=[native/'trace.json']+[p/'manifest.json'for p in(common,domain,state,poses,stride)];report=dict(status='awaiting_independent_review',accepted=False,name='Swamp',type_id='0xbecffeda',binary_sha256=ms[0]['binary_sha256'],asset_sha256=n['asset_sha256'],cases=rows,actual_tick_checks=actual,input_manifest_sha256={str(p):digest(p)for p in inputs},contact_sha256=digest(output/'contact.png'),detail_sha256=digest(output/'detail.png'),appearance_fitted=False,scope='Own state0/null-spell default owner, actual NextFrame/effectPulse/controller/completeRender/RenderObject/software; 31-slot fgeys emitter and27 authored meshes, exact source draw order and56-tick kill. Declared sourceambient38/dir1,seed1,quality0,flatground1,owner/camera0,1024x768 screen center512/640. Decoded immutable resources/COM, script/map notifications are explicit boundaries. Existing native-domain source-alpha quantization; common projection remains separate. Natural caller/audio/device/destination precision/exact pixels remain open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for name in('native','common','domain','state','poses','stride','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.poses,a.stride,a.output)
