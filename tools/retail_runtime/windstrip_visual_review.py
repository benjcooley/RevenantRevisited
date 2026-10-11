#!/usr/bin/env python3
"""Freeze full WindStrip phase/RNG/actual GPU appearance comparisons."""
import json,re,hashlib
from pathlib import Path
from PIL import Image,ImageDraw
import argparse
p=argparse.ArgumentParser();p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();b=a.base;o=a.output;o.mkdir(parents=True,exist_ok=False);sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest();native=json.loads((b/'wind-native-first/manifest.json').read_text());metal=[json.loads((b/f'wind-metal-{p}-first/manifest.json').read_text())for p in ('common','domain')]
for p,m in zip(('common','domain'),metal):
 assert m['status']=='pass' and m['exit_code']==0 and m['scenario']['frames']==255
 rows=re.findall(r'windstrip-phase\] tick=(\d+) frame=(\d+) halo=([^ ]+) drops=(\d+) rng=(\d+)',(b/f'wind-metal-{p}-first/run.log').read_text());assert len(rows)==251
 for t,frame,halo,drops,rng in rows:
  n=native['cases'][int(t)];assert int(frame)==n['state']['frame'] and int(rng)==n['state']['rng_draws'];assert abs(float(halo)-n['state']['halo'])<1e-6;assert len(n['draws'])==20+int(drops)+int(float(halo)>0)
assert metal[0]['binary_sha256']==metal[1]['binary_sha256']
samples=[n for n in native['cases']if 'image'in n];contact=Image.new('RGB',(960,265*len(samples)));detail=Image.new('RGB',(960,345*len(samples)));d=ImageDraw.Draw(contact);dd=ImageDraw.Draw(detail);cases=[]
for row,n in enumerate(samples):
 tick=n['tick'];paths=[Path(n['image'])]+[Path(m['frames'][tick-2]['path'])for m in metal];case=dict(tick=tick,paths=list(map(str,paths)),sha256=list(map(sha,paths)),pixels=[],bounds=[])
 for col,(p,label)in enumerate(zip(paths,('Original','Common','Native domain'))):
  im=Image.open(p).convert('RGB');assert im.size==(640,480);case['pixels'].append(sum(x!=(0,0,0)for x in im.getdata()));case['bounds'].append(im.getbbox());contact.paste(im.resize((320,240)),(320*col,row*265+25));d.text((320*col+4,row*265+4),f'{label} tick{tick}',fill='white');detail.paste(im.crop((160,60,480,380)),(320*col,row*345+25));dd.text((320*col+4,row*345+4),f'{label} tick{tick} actual size',fill='white')
 if tick>=252:assert not any(case['pixels'])
 cases.append(case)
contact.save(o/'contact.png');detail.save(o/'detail.png');m=dict(status='awaiting_review',accepted=False,retail_type_id='0xded3dbed',name='WindStrip',binary_sha256=metal[0]['binary_sha256'],cases=cases,scope=native['scope'],inputs={str(b/p/'manifest.json'):sha(b/p/'manifest.json')for p in('wind-native-first','wind-metal-common-first','wind-metal-domain-first','wind-state-first','wind-geometry-first')},contact_sha256=sha(o/'contact.png'),detail_sha256=sha(o/'detail.png'));(o/'manifest.json').write_text(json.dumps(m,indent=2)+'\n');print(sha(o/'manifest.json'));print([(c['tick'],c['pixels'])for c in cases])
