#!/usr/bin/env python3
"""Pin full Smoke original/GPU phases and genuine quality controls without fitting."""
import argparse,json,re
from pathlib import Path
from PIL import Image,ImageDraw
from speed_profile_contract import sha

def digest(p):return sha(Path(p).read_bytes())
def review(native,common,domain,state,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((native/'manifest.json').read_text());ms=[json.loads((p/'manifest.json').read_text())for p in(common,domain)];proof=json.loads((state/'manifest.json').read_text());assert proof['status']=='pass';assert n['quality']==3 and n['low_quality_kills'];maximum=0
 for folder,m in zip((common,domain),ms):
  assert m['status']=='pass'and m['scenario']['effect']=='Smoke_ORIGINAL_QUALITY'and m['scenario']['partsys_quality']==3 and m['scenario']['frames']==600;log=(folder/'run.log').read_text();assert log.count('[smoke] typed_id=224a5ccc quality=3 alive=1')==1;assert 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'in log
  phases=re.findall(r'\[smoke-phase\] tick=(\d+) center=\(([^,]+),([^\)]+)\) first=\(([^,]+),([^,]+),([^\)]+)\) size=([^ ]+) life=(\d+)',log);assert len(phases)>=600
  for raw in phases:
   tick=int(raw[0])
   if tick>600:continue
   ref=n['cases'][tick]['state'];expected=ref['center'][:2]+ref['slots'][0][:4]+[ref['slots'][0][-1]];actual=list(map(float,raw[1:]));assert len(expected)==len(actual);maximum=max(maximum,max(abs(a-b)for a,b in zip(actual,expected)));assert maximum<2e-5
 assert ms[0]['binary_sha256']==ms[1]['binary_sha256'];ticks=(2,20,80,140,199,200,250,300,500,600);whole=Image.new('RGB',(960,len(ticks)*265));detail=Image.new('RGB',(1200,len(ticks)*425));wd=ImageDraw.Draw(whole);dd=ImageDraw.Draw(detail);rows=[]
 for row,tick in enumerate(ticks):
  ref=n['cases'][tick];paths=[Path(ref['image'])]+[Path(m['frames'][tick-2]['path'])for m in ms];hashes=[ref['image_sha256']]+[m['frames'][tick-2]['sha256']for m in ms];assert list(map(digest,paths))==hashes;assert ref['draw_count']==50;images=[Image.open(p).convert('RGB')for p in paths];assert all(im.size==(1280,960)for im in images);rows.append(dict(tick=tick,paths=list(map(str,paths)),sha256=hashes,visible_bbox=[im.getbbox()for im in images],original_draws=50,original_geometry_bounds=[d['geometry_bounds']for d in ref['draws']]))
  for col,(im,label)in enumerate(zip(images,('Original','Common Metal','Native-domain Metal'))):
   whole.paste(im.resize((320,240)),(col*320,row*265+25));wd.text((col*320+4,row*265+4),f'{label} tick{tick}',fill='white');detail.paste(im.crop((440,280,840,680)),(col*400,row*425+25));dd.text((col*400+4,row*425+4),f'{label} same actual-size crop tick{tick}',fill='white')
 whole.save(output/'contact.png');detail.save(output/'detail.png');report=dict(status='awaiting_independent_review',accepted=False,name='Smoke',type_id='0x224a5ccc',binary_sha256=ms[0]['binary_sha256'],asset_sha256=n['asset_sha256'],cases=rows,actual_tick_sample_max_error=maximum,input_manifest_sha256={str(p/'manifest.json'):digest(p/'manifest.json')for p in(native,common,domain,state)},contact_sha256=digest(output/'contact.png'),detail_sha256=digest(output/'detail.png'),appearance_fitted=False,scope='Exact capitalized custom Smoke, own oldbody/key/ARGB4444,50 original ordered RenderObject calls and whole software raster at ten phases through600 ticks versus actual600-frame Metal films. Declared quality3,seed1,sourceambient38/directional1,owner/camera0,1280x960 native-domain; common projection retained separately. Pulse/NextFrame/Animate clear-done owner handshake retained. Original own-texel source contribution floor enabled only in native-domain through existing helper policy; low quality0/1/2 original kills, global default unchanged. Base script/bookkeeping, immutable decoded resource/COM and declared light/camera are boundaries. Natural map/caller/device/destination precision/exact pixels remain open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['binary_sha256'],maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser()
 for name in('native','common','domain','state','output'):p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();review(a.native,a.common,a.domain,a.state,a.output)
