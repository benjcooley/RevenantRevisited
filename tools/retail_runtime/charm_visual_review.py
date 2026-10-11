#!/usr/bin/env python3
"""Pin whole original Charm/current Metal loop and matched visual phases."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageDraw
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def review(native,common,domain,state,output,cold=None):
 output.mkdir(parents=True,exist_ok=False);ref=json.loads((native/'manifest.json').read_text());proof=json.loads((state/'manifest.json').read_text());captures=[json.loads((p/'manifest.json').read_text())for p in(common,domain)]
 assert proof['status']=='pass' and proof['ticks']==122 and proof['default_zero_identical']
 for index,(path,m)in enumerate(zip((common,domain),captures)):
  assert m['status']=='pass'and m['scenario']['native_domain']==bool(index)
  assert m['scenario']['effect']=='TBuffEffect_Bespoke__charm_BESPOKE'and m['scenario']['frames']==122
  assert (path/'run.log').read_text().count('[charm] authored cached-emitter')==1
  assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in(path/'run.log').read_text()
 ticks=(2,6,16,29,30,31,60,90,119,120,121);panel=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(1200,len(ticks)*375));draw=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail);cases=[]
 for row,tick in enumerate(ticks):
  n=next(c for c in ref['cases']if c['tick']==tick);paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures];hashes=[n['image_sha256']]+[m['frames'][tick-2]['sha256']for m in captures];assert list(map(sha,paths))==hashes
  cases.append(dict(tick=tick,images=list(map(str,paths)),sha256=hashes,native_draws=len(n['draws'])))
  for col,(path,label)in enumerate(zip(paths,('Original','Common-world Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');panel.paste(im.resize((320,240)),(col*320,row*265+25));draw.text((col*320+2,row*265+5),f'{label} tick{tick}',fill='white');detail.paste(im.crop((225,145,425,320)).resize((400,350)),(col*400,row*375+25));dd.text((col*400+2,row*375+5),f'{label} tick{tick} same crop 2x',fill='white')
 panel.save(output/'contact.png');detail.save(output/'detail.png')
 actual=[];log=(domain/'run.log').read_text()
 for t,frame,draws,credit,x,y,z in re.findall(r'charm-phase\] tick=(\d+) frame=(\d+) dust=(\d+) credit=([^ ]+) emitter=\(([^,]+),([^,]+),([^)]+)\)',log):
  n=ref['cases'][int(t)];assert int(draws)+1==len(n['draws']);error=max(abs(a-b)for a,b in zip(map(float,(x,y,z)),n['draws'][-1]['matrix'][12:15]));assert error<1e-5;actual.append(dict(tick=int(t),dust=int(draws),emitter_error=error))
 report=dict(status='awaiting_independent_review',accepted=False,retail_type_id='0xad92bd37',retail_name='charm',cases=cases,actual_metal_emitter_checks=actual,input_manifest_sha256={str(p):sha(p/'manifest.json')for p in(native,common,domain,state)},binary_sha256=[m['binary_sha256']for m in captures],contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,scope='State0 repeated30-frame loop including29-to0 and119-to0 boundaries, exact moving #star and independent #clover, inclusive keys, constant scale jitter and persistent75-slot pool. Cold incomingnormal1 then mode16 after first valid particle; Static preview retains pool without browser reset. Whole original Animate/Render software reference vs actual isolated Metal; source38/dir1, quality0, seed1, flatground0, owner/camera0. Native-domain policy explicit; common-world residual shown. Natural caller/end-state trigger/audio and exact raster equality remain open.')
 if cold:
  m=json.loads((cold/'manifest.json').read_text());assert m['status']=='pass' and m['scenario']['warmup']==1 and m['scenario']['fps']==48 and m['scenario']['native_domain']
  assert m['pixel_summary']['nonblack_frames']==m['scenario']['frames'];assert 'tick=1 frame=1 dust=0' in (cold/'run.log').read_text()
  n=ref['cases'][1];paths=[Path(n['image']),Path(m['frames'][0]['path'])];hashes=[n['image_sha256'],m['frames'][0]['sha256']];assert list(map(sha,paths))==hashes
  cold_panel=Image.new('RGB',(1280,505));cd=ImageDraw.Draw(cold_panel)
  for col,(path,label)in enumerate(zip(paths,('Original cold tick1','Native-domain Metal cold tick1'))):cold_panel.paste(Image.open(path).convert('RGB'),(col*640,25));cd.text((col*640+5,5),label,fill='white')
  cold_panel.save(output/'cold-contact.png');report['cold_start']=dict(tick=1,images=list(map(str,paths)),sha256=hashes,capture_manifest_sha256=sha(cold/'manifest.json'),contact_sha256=sha(output/'cold-contact.png'),limit='Isolated black fixture; original cold base writes depth while helper uses no-write. No surrounding-depth/device parity claimed.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in('native','common','domain','state','output'):p.add_argument('--'+name,type=Path,required=True)
 p.add_argument('--cold',type=Path)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.output,a.cold)
