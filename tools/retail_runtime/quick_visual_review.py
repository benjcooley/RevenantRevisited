#!/usr/bin/env python3
"""Freeze whole Quick original/actual Metal phases without fitting inputs."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image,ImageDraw

def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def review(native,common,domain,state,poses,control,output):
 output.mkdir(parents=True,exist_ok=False)
 ref=json.loads((native/'manifest.json').read_text());captures=[json.loads((p/'manifest.json').read_text())for p in (common,domain)]
 common_tail=[]
 for p,m in zip((common,domain),captures):
  assert m['status']=='pass'and m['scenario']['effect']=='TQuick_AUTHORED_TAGS'and m['scenario']['frames']==60
  assert m['pixel_summary']['nonblack_frames']==(55 if p==common else 54)
  log=(p/'run.log').read_text();assert log.count('[quick] authored cached-emitter')==1
  assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
  for i,f in enumerate(m['frames']):
   visible=bool(Image.open(f['path']).convert('RGB').getbbox())
   if p==common and i==54:
    assert visible and m['pixel_summary']['pixel_sha256'][i]==m['pixel_summary']['pixel_sha256'][i-1];common_tail.append(dict(tick=i+2,reason='One-frame retained prior image after owner completed; not phase-adjusted'))
   else:assert visible==(i<54)
 assert json.loads((state/'manifest.json').read_text())['status']=='pass'
 assert json.loads((poses/'manifest.json').read_text())['status']=='pass'
 for c in ref['cases']:assert bool(int(c['owner_flags'],16)&0x1000)==(c['tick']>=56)
 ticks=(2,6,9,16,28,40,50,55,56,60);cases=[];panel=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(1320,len(ticks)*305));pd=ImageDraw.Draw(panel);dd=ImageDraw.Draw(detail)
 for row,tick in enumerate(ticks):
  n=next(c for c in ref['cases']if c['tick']==tick);paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in captures];hashes=[n['image_sha256']]+[m['frames'][tick-2]['sha256']for m in captures];assert list(map(sha,paths))==hashes
  cases.append(dict(tick=tick,images=list(map(str,paths)),sha256=hashes,native_draws=len(n['draws']),owner_flags=n['owner_flags']))
  for col,(path,label)in enumerate(zip(paths,('Original','Common-world Metal','Native-domain Metal'))):
   im=Image.open(path).convert('RGB');panel.paste(im.resize((320,240)),(col*320,row*265+25));pd.text((col*320+4,row*265+4),f'{label} tick{tick}',fill='white');detail.paste(im.crop((210,175,430,315)).resize((440,280)),(col*440,row*305+25));dd.text((col*440+4,row*305+4),f'{label} tick{tick} same crop x2',fill='white')
 panel.save(output/'contact.png');detail.save(output/'detail.png')
 actual=[];log=(domain/'run.log').read_text()
 for t,frame,draws,credit,x,y,z in re.findall(r'quick-phase\] tick=(\d+) frame=(\d+) dust=(\d+) credit=([^ ]+) emitter=\(([^,]+),([^,]+),([^)]+)\)',log):
  c=ref['cases'][int(t)];assert int(frame)==c['frame'];assert int(draws)+6==len(c['draws']);actual.append(dict(tick=int(t),frame=int(frame),particle_draws=int(draws)))
 order=json.loads((control/'differential.json').read_text());assert any(c['different_pixels']for c in order['cases'])
 report=dict(status='awaiting_independent_review',accepted=False,retail_name='Quick',retail_type_id='0xbabfface',cases=cases,actual_metal_draw_checks=actual,input_manifest_sha256={str(p):sha(p/'manifest.json')for p in(native,common,domain,state,poses,control)},order_negative_control_sha256=sha(control/'differential.json'),binary_sha256=[m['binary_sha256']for m in captures],contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),appearance_fitted=False,common_tail_diagnostic=common_tail,scope='State0 exact56-frame null-spell owner lifecycle, actual original NextFrame470cc0/effectPulse4de800 and full controller/RenderObject/normal producer/software raster. Immutable state-header and decoded resources/COM residency; script/tag-audio base4708e0 and flatground0 are explicit boundaries. Six authored ARGB4444 layers follow RGB565 sparks in native material order. Actual isolated Metal, source38/dir1, seed1, quality0, owner/camera0, explicitnative-domain; common-world residual separately retained. Natural spell/attachment/audio/scene and exact raster/device parity remain open.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in ('native','common','domain','state','poses','control','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.poses,a.control,a.output)
