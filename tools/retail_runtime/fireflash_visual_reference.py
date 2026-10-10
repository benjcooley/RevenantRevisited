#!/usr/bin/env python3
"""Original normal-lit FireFlash lifetime reference and unfitted Metal review.

Uses exact authored resources and original animation/owner/normal/raster code.
Audio/resource interfaces remain explicit boundaries; this is not a full scene.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import zipfile
from PIL import Image, ImageDraw
from unicorn import UC_HOOK_CODE
from fireflash_probe import FireFlashFixture
from firecone_owner_probe import native_owner, native_world
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png

ROOT=Path(__file__).resolve().parents[2]
SELECTED=(2,6,16,26,27,30,31,35,36,39,46,60,70)
sha=lambda p:hashlib.sha256(Path(p).read_bytes()).hexdigest()


def reference(executable,archive,output):
    output.mkdir(parents=True,exist_ok=False)
    with zipfile.ZipFile(archive) as z:
        asset=z.read('Imagery/Magic/fireflash.i3d')
    fixture=FireFlashFixture(executable,asset)
    count=[0]
    def observe(uc,address,size,user): count[0]+=1
    # Observe the unmodified CRT implementation; do not substitute an RNG.
    fixture.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=0x58c582,end=0x58c582)
    fixture.reset()
    owner=native_owner(fixture)
    software=LitSoftwareFixture(executable,512,512)
    software.lighting((38,38,38),(1,1,1))
    for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),
        (0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
        software.vm.put_u32(address,value)
    software.vm.call(0x417d60,(8,1),this=0x65a57c)
    cases=[]
    for tick in range(1,72):
        prior=count[0]
        fixture.step()
        # Render mutates particle scale, so execute every frame, not only samples.
        packets=fixture.render()
        if tick not in SELECTED: continue
        software.clear();colors=[]
        for packet in packets:
            part=fixture.parts[packet['object']]
            world=native_world(fixture,packet)
            software.set_texture(64,64,part['texture'])
            _,_,vertices=software.draw_authored(part['vertices'],part['indices'],world,
                                                cull=1,z_enabled=True,z_write=False)
            colors.extend(tuple(v[3:6]) for v in vertices)
        path=output/f'tick-{tick:03}.png'
        save_rgb565_png(path,software.vm.surface_bytes('screen'),512,512)
        cases.append(dict(tick=tick,capture=tick-1,draws=len(packets),
            crt_draws_before=prior,colors=sorted(set(colors)),image=str(path.resolve()),sha256=sha(path)))
    report=dict(cases=cases,owner=owner,executable_sha256=sha(executable),
        asset_sha256=hashlib.sha256(asset).hexdigest(),probe_sha256=sha(__file__),
        viewport=[512,512],camera=[0,0,0],ambient=[38]*3,directional=[1]*3,
        original_functions=['Init4e18c0','Animate4e1ad0','Render4e1fa0',
            'Owner40e470/40e740','RenderObject40aad5','SceneBlend417d60','VertexIlluminateRaster56eb30'],
        scope='Null-spell owner at origin, facing0. Exact authored resources, silent audio/base traversal boundaries; actual original owner/world composition and normal-lit raster. No fabricated vertex light or whole-scene claim.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


def review(reference_dir,metal,output):
    output.mkdir(parents=True,exist_ok=False)
    original=json.loads((reference_dir/'manifest.json').read_text())
    capture=json.loads((metal/'manifest.json').read_text())
    scenario=capture['scenario']
    if capture['status']!='pass' or scenario['rectangle']!=[0,0,512,512] or scenario['camera']!=[256,256]:
        raise AssertionError('Expected unmodified512-square Metal capture')
    if scenario['origin']!=[0,0,0] or scenario['background']!='black' or scenario.get('native_domain',False):
        raise AssertionError('Expected null-origin common-domain black capture')
    if scenario['frames']!=70 or scenario['fps']!=24 or scenario['warmup']!=1 or scenario['seed']!=1:
        raise AssertionError('Capture cadence/seed differs')
    log=(metal/'run.log').read_text()
    light=[line for line in log.splitlines() if '[vfx-source-light]' in line]
    if len(light)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' not in light[0]:
        raise AssertionError('Actual Metal source lighting differs')
    rng={int(t):int(n) for t,n in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
    cases=[];verified=0
    for case in original['cases']:
        tick=case['tick'];number=case['capture']
        native=Path(case['image']);modern=Path(capture['frames'][number-1]['path'])
        if sha(native)!=case['sha256']: raise AssertionError('Reference changed')
        if tick in rng:
            if rng[tick]!=case['crt_draws_before']: raise AssertionError('Native/Metal RNG phase differs')
            verified+=1
        images=[Image.open(p).convert('RGB') for p in (native,modern)]
        cases.append(dict(tick=tick,capture=number,native=str(native),metal=str(modern),
            sha256=[sha(p) for p in (native,modern)],bbox=[im.getbbox() for im in images],
            differing_pixels=sum(a!=b for a,b in zip(images[0].getdata(),images[1].getdata()))))
    if verified!=6: raise AssertionError('Missing six independent RNG phase controls')
    for name,ticks in (('annulus',(26,30,35,36,39,46)),('ends',(2,6,16,60,70)),('transitions',(27,31))):
        panel=Image.new('RGB',(384,len(ticks)*305),(20,20,20));draw=ImageDraw.Draw(panel)
        for row,tick in enumerate(ticks):
            case=next(c for c in cases if c['tick']==tick)
            for column,(key,label) in enumerate((('native','Retail'),('metal','Metal'))):
                # Identical fixed pixel crop; no image scaling or registration.
                im=Image.open(case[key]).convert('RGB').crop((160,0,352,280))
                panel.paste(im,(column*192,row*305+25))
                draw.text((column*192+4,row*305+4),f'{label} tick{tick} capture{tick-1}',fill='white')
        panel.save(output/(name+'.png'))
    report=dict(status='review_ready',accepted=False,cases=cases,rng_phase_controls=verified,
        binary_sha256=capture['binary_sha256'],source_light_log=light[0],appearance_fitted=False,
        native_manifest_sha256=sha(reference_dir/'manifest.json'),metal_manifest_sha256=sha(metal/'manifest.json'),
        scope='Same elapsed ticks at24Hz, null spell, original owner, normal light and authored textures versus actual Metal. Exact device/pixel parity, target spell branch and natural caller remain separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--reference',type=Path);p.add_argument('--metal',type=Path)
    a=p.parse_args()
    if bool(a.reference)!=bool(a.metal): p.error('Review needs both --reference and --metal')
    r=review(a.reference,a.metal,a.output) if a.reference else reference(a.executable,a.archive,a.output)
    print(len(r['cases']),'FireFlash elapsed-time cases')
