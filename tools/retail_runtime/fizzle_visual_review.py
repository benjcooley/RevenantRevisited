#!/usr/bin/env python3
"""Freeze phase/RNG checked Fizzle original/common/native-domain Metal evidence."""
import argparse
import hashlib
import json
from pathlib import Path
import re
from PIL import Image,ImageDraw


def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def review(native,common,domain,output):
    output.mkdir(parents=True,exist_ok=False)
    n=json.loads((native/'manifest.json').read_text())
    captures=[json.loads((p/'manifest.json').read_text())for p in (common,domain)]
    if n['status']!='reference_complete':raise AssertionError('Native incomplete')
    checkpoints=[];light_logs=[]
    for index,(path,m) in enumerate(zip((common,domain),captures)):
        if m['status']!='pass':raise AssertionError('Metal incomplete')
        expected=dict(frames=40,fps=24,warmup=1,seed=1,origin=[0,0,0],camera=[320,240],
                      rectangle=[0,0,640,480],resolution=[640,480],background='black',
                      ambient=[32,255,255,255],native_domain=bool(index),effect='TFizzleEffect_SINGLE_BURST')
        for key,value in expected.items():
            if m['scenario'].get(key)!=value:raise AssertionError('Scenario differs: '+key)
        log=(path/'run.log').read_text()
        if f'[native-domain] enabled={index} camera_z=0 owner=RzRxRyT particle=relative Fizzle' not in log:
            raise AssertionError('Owner/projector route not proven')
        lights=[line for line in log.splitlines()if '[vfx-source-light]' in line]
        if len(lights)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' not in lights[0]:
            raise AssertionError('Unexpected source light configuration')
        light_logs.append(lights[0])
        counts={int(t):int(c)for t,c in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
        if counts.get(1)!=0:raise AssertionError('Nonzero initial RNG count')
        checks=[dict(tick=1,rng_before=0)]
        for c in n['cases']:
            if c['tick'] in counts:
                if counts[c['tick']]!=c['rng_before']:raise AssertionError('Native/Metal RNG phase differs')
                checks.append(dict(tick=c['tick'],rng_before=c['rng_before']))
        if len(checks)!=5:raise AssertionError('Missing RNG checkpoints')
        checkpoints.append(checks)
    selected=(1,3,5,9,14,19,24,29,39)
    panel=Image.new('RGB',(900,220*len(selected)));draw=ImageDraw.Draw(panel)
    cases=[]
    for c in n['cases']:
        paths=[Path(c['image'])]+[Path(m['frames'][c['capture']-1]['path'])for m in captures]
        hashes=[c['sha256']]+[m['frames'][c['capture']-1]['sha256']for m in captures]
        if [sha(p)for p in paths]!=hashes:raise AssertionError('Frozen frame changed')
        images=[Image.open(p).convert('RGB')for p in paths]
        counts=[sum(any(rgb)for rgb in im.getdata())for im in images]
        cases.append(dict(capture=c['capture'],tick=c['tick'],alive=c['alive'],draws=c['draws'],
                          rng_before=c['rng_before'],images=list(map(str,paths)),sha256=hashes,
                          nonzero_pixels=counts))
        if c['capture'] in selected:
            row=selected.index(c['capture'])
            for col,(im,label)in enumerate(zip(images,('Original','Common-world Metal','Native-domain Metal'))):
                panel.paste(im.crop((170,45,470,240)),(col*300,row*220+25))
                draw.text((col*300+2,row*220+4),f'{label} tick {c["tick"]}',fill='white')
    panel.save(output/'contact.png')
    # Exact integer enlargement exposes sampling/precision residuals without rescaling geometry.
    detail=Image.new('RGB',(960,650));draw=ImageDraw.Draw(detail)
    for col,path in enumerate(cases[13]['images']):
        im=Image.open(path).convert('RGB').crop((270,90,350,245)).resize((320,620),Image.Resampling.NEAREST)
        detail.paste(im,(col*320,30));draw.text((col*320+3,5),('Original','Common-world','Native-domain')[col]+' tick15 (4x)',fill='white')
    detail.save(output/'detail.png')
    report=dict(status='review_ready',accepted=False,retail_name='Fizzle',retail_type_id='0xab8800dd',
                cases=cases,selected_captures=selected,live_rng_checkpoints=checkpoints,
                source_light_logs=light_logs,source_color_policy='ARGB4444 ignores vertex color',
                binary_sha256=[m['binary_sha256']for m in captures],
                input_manifest_sha256=[sha(p/'manifest.json')for p in (native,common,domain)],
                contact_sha256=sha(output/'contact.png'),detail_sha256=sha(output/'detail.png'),
                source_changes=['compose relative particle corners with owner matrix','exact source ARGB4444 quad policy',
                                'explicit native-domain isolated preview only'],appearance_fitted=False,
                scope='Forty whole leaf lifecycle captures, five live RNG checkpoints per Metal route; original owner/world/projector/ARGB4444 software versus actual Metal. Native-domain isolated camera Z=0; ordinary common-world retains shared projector residual. Raster/framebuffer precision differs; natural caller/full original RenderObject resource dispatch/full-map/device remain open.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for name in ('native','common','domain','output'):p.add_argument('--'+name,type=Path,required=True)
    a=p.parse_args();r=review(a.native,a.common,a.domain,a.output)
    print('PASS',len(r['cases']),'phase pairs,',len(r['live_rng_checkpoints'][0]),'RNG checkpoints per route')
