#!/usr/bin/env python3
"""Freeze phase-checked MistFog whole-effect native/Metal review evidence."""
import argparse
import hashlib
import json
import re
from pathlib import Path
from PIL import Image,ImageDraw


def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def review(native,metal,output):
    output.mkdir(parents=True,exist_ok=False)
    n=json.loads((native/'manifest.json').read_text())
    m=json.loads((metal/'manifest.json').read_text())
    if n['status']!='reference_complete' or m['status']!='pass':raise AssertionError('Incomplete input run')
    for key,value in dict(frames=240,fps=24,warmup=1,seed=1,origin=[0,0,0],
        camera=[320,170],rectangle=[0,0,640,340],resolution=[640,340],
        ambient=[32,255,255,255],background='black').items():
        if m['scenario'][key]!=value:raise AssertionError('Different scenario: '+key)
    log=(metal/'run.log').read_text()
    lights=[line for line in log.splitlines() if '[vfx-source-light]' in line]
    if len(lights)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' not in lights[0]:
        raise AssertionError('Different source lighting')
    rng={int(t):int(c) for t,c in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
    panel=Image.new('RGB',(768,len(n['cases'])*249));labels=ImageDraw.Draw(panel)
    cases=[];checks=0
    for row,c in enumerate(n['cases']):
        if c['packets']!=25:raise AssertionError('Incomplete effect')
        if c['tick']<=60:
            if rng[c['tick']]!=c['rng_before']:raise AssertionError('Live native/Metal RNG phase differs')
            checks+=1
        p=Path(c['path']);q=Path(m['frames'][c['capture']-1]['path'])
        if sha(p)!=c['sha256'] or sha(q)!=m['frames'][c['capture']-1]['sha256']:
            raise AssertionError('Captured image changed')
        for column,(path,label) in enumerate(((p,'Original'),(q,'Metal'))):
            im=Image.open(path).convert('RGB')
            panel.paste(im.crop((128,64,512,288)),(column*384,row*249+25))
            labels.text((column*384+3,row*249+3),f'{label} tick {c["tick"]}',fill='white')
        cases.append(dict(capture=c['capture'],tick=c['tick'],native=str(p),metal=str(q),
                          sha256=[sha(p),sha(q)],rng_before=c['rng_before']))
    if checks!=6:raise AssertionError('Missing live RNG checkpoints')
    panel.save(output/'contact.png')
    report=dict(status='review_ready',accepted=False,cases=cases,live_rng_checkpoints=checks,
        source_light_log=lights[0],source_ambient=[38]*3,source_directional=[1]*3,
        native_manifest_sha256=sha(native/'manifest.json'),metal_manifest_sha256=sha(metal/'manifest.json'),
        binary_sha256=m['binary_sha256'],panel_sha256=sha(output/'contact.png'),crop=[128,64,512,288],
        appearance_fitted=False,scope='Eleven whole25-puff lifecycle phases through source resets. Native fullnormal-light RGB565 renderer versus actual Metal. Common projector/framebuffer precision retain residuals; no original full-map/device/natural-creation acceptance.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--native',type=Path,required=True);p.add_argument('--metal',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=review(a.native,a.metal,a.output)
    print('PASS',len(r['cases']),'whole-effect visual pairs;',r['live_rng_checkpoints'],'live RNG checkpoints')
