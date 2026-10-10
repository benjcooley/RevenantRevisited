#!/usr/bin/env python3
"""Retain unmoved native/Metal panels and map lifecycle evidence; never auto-accept."""
import argparse
import csv
import json
import re
from pathlib import Path
from PIL import Image, ImageChops, ImageDraw
from firecone_state_probe import sha


def pixel_hash(path):
    with Image.open(path) as image:return sha(image.convert('RGB').tobytes())


def review(reference, metal, maps, output, metal_suffix=""):
    output.mkdir(parents=True,exist_ok=False)
    original=json.loads((reference/'manifest.json').read_text())
    panel=Image.new('RGB',(1100,720),(28,28,28));labels=ImageDraw.Draw(panel)
    cases=[];map_reports=[]
    for row,name in enumerate(('Speed','Quicksilver','Fmastery')):
        folder='Fmastery' if name=='Fmastery' else name.lower()
        capture=json.loads((metal/(name+metal_suffix)/'manifest.json').read_text())
        if capture['status']!='pass' or capture['scenario']['frames']!=60:raise AssertionError('Metal capture failed')
        frames=sorted((metal/(name+metal_suffix)).glob('frame-[0-9][0-9][0-9]-*.png'))
        capture_log=(metal/(name+metal_suffix)/'run.log').read_text()
        lights=[line for line in capture_log.splitlines() if '[vfx-source-light]' in line]
        if len(lights)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' not in lights[0]:raise AssertionError('Actual source lighting differs')
        rng={int(frame):int(count) for frame,count in re.findall(r'\[vfx-rng-scope\] before_render=(\d+) global_random_draws=(\d+)',capture_log)}
        for case in (c for c in original['cases'] if c['name']==folder):
            number=case['capture'];native=reference/folder/f'native-capture{number:03d}.png';port=frames[number-1]
            images=[Image.open(p).convert('RGB')for p in(native,port)]
            if rng[number+1]!=case['native_rng_draws_before_capture']:raise AssertionError('Independent native/Metal RNG draw counts differ')
            cases.append(dict(name=name,capture=number,source_light_log=lights[0],native_rng_draws_before_capture=case['native_rng_draws_before_capture'],metal_rng_draws_before_capture=rng[number+1],animation_frame=case['animation_frame'],native=str(native),metal=str(port),file_sha256=[sha(p.read_bytes())for p in(native,port)],bbox=[im.getbbox()for im in images],rgb_sums=[tuple(sum(p[i]for p in im.getdata())for i in range(3))for im in images],differing_pixels=sum(a!=b for a,b in zip(images[0].getdata(),images[1].getdata()))))
            if number in(5,15,29):
                column=(5,15,29).index(number)
                for j,(im,label)in enumerate(zip(images,('Retail','Metal'))):
                    x=column*365+j*175;y=row*240
                    panel.paste(im.crop((275,115,365,215)).resize((162,180),Image.Resampling.NEAREST),(x,y+35))
                    labels.text((x,y+5),f'{name} {label} cap{number}',fill='white')
        directory=maps/folder;manifest=json.loads((directory/'capture.json').read_text())
        files=sorted(directory.glob('frame-[0-9][0-9][0-9]-*.png'));log=(directory/'run.log').read_text()
        hashes=[pixel_hash(p)for p in files];timing=list(csv.DictReader((directory/'frame-timing.csv').open()))
        commands=[l for l in log.splitlines()if '[scene-command] row 'in l]
        okay=(manifest['exit']==0 and len(files)==58 and len(commands)==6 and all('status=PASS'in l for l in commands) and '[scene-command] COMPLETE rows=6 failures=0'in log and len(timing)==58 and all(abs(float(t['relative_seconds'])-i/24)<1e-6 for i,t in enumerate(timing)) and all(h==hashes[0]for h in hashes[45:]) and hashes[15]!=hashes[0])
        if not okay:raise AssertionError('Map lifecycle capture failed: '+name)
        before=ImageChops.difference(Image.open(files[32]).convert('RGB'),Image.open(files[0]).convert('RGB')).getbbox()
        after=ImageChops.difference(Image.open(files[37]).convert('RGB'),Image.open(files[0]).convert('RGB')).getbbox()
        map_reports.append(dict(name=name,status='pass',binary_sha256=manifest['binary_sha256'],command=manifest['command'],frames=58,commands=commands,distinct_active_frames=len(set(hashes[2:44])),floor_tail_equal=True,movement_bbox_before=before,movement_bbox_after=after,frame_sha256=[sha(p.read_bytes())for p in files],log_sha256=sha((directory/'run.log').read_bytes()),commands_sha256=sha((directory/'commands.txt').read_bytes())))
    panel.save(output/'native-metal.png')
    report=dict(native_domain=capture['scenario'].get('native_domain',False),status='review_ready',accepted=False,strict_pixel_parity=False,native_manifest_sha256=sha((reference/'manifest.json').read_bytes()),metal_binary_sha256=capture['binary_sha256'],cases=cases,map_lifecycle=map_reports,panel_sha256=sha((output/'native-metal.png').read_bytes()),scope='Three complete native isolated effects versus fresh actual Metal, declared identical source lighting/quality/owner origin and documented animation cadence. Raw frames remain unchanged. Panel uses the same fixed crop and nearest resize for both branches, with no image registration or fitting. Map ADDAT/MOVE/DELETE and exact floor restoration are separate real-world-owner evidence. Native common projection, RGB565 raster/modulation and GPU sampling differ visibly; overall appearance requires human review, not an automatic pixel pass. Natural caller, caster, audio, scene point lights and device edge cases remain outside this configuration.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for name in('reference','metal','maps','output'):p.add_argument('--'+name,type=Path,required=True)
    a=p.parse_args();r=review(a.reference,a.metal,a.maps,a.output);print(r['status'],len(r['cases']),'native/Metal comparisons',len(r['map_lifecycle']),'map lifecycles')
