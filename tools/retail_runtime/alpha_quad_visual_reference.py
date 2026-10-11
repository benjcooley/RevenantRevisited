#!/usr/bin/env python3
"""Original Mist/SymGlow ARGB4444 lifecycle and unfitted Metal comparisons."""
import argparse
import json
from pathlib import Path
import re
import zipfile
from PIL import Image,ImageDraw
from unicorn import UC_HOOK_CODE
from mist_probe import MistFixture,INDICES
from symglow_probe import SymGlowFixture,parse_asset
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_owner_probe import native_owner
from fireflash_visual_reference import sha,ROOT


def reference(executable,archive,output):
    output.mkdir(parents=True,exist_ok=False)
    with zipfile.ZipFile(archive) as z:
        assets={n:z.read(p) for n,p in [('Mist','Imagery/Magic/mist.i3d'),
                                       ('SymGlow','Imagery/Misc/symglow.i3d')]}
    reports={}
    for name in ('Mist','SymGlow'):
        folder=output/name;folder.mkdir()
        if name=='Mist':
            f=MistFixture(executable,assets[name]);texture=assets[name][0x2d4:0x22d4]
            blend=8;indices=INDICES
        else:
            mesh=parse_asset(assets[name]);f=SymGlowFixture(executable,mesh)
            texture=mesh['texture'];blend=2;indices=(0,1,2)
        count=[0]
        def observe(uc,address,size,user): count[0]+=1
        f.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=0x58c582,end=0x58c582)
        f.reset()
        if name=='SymGlow':
            f.owner=f.vm.allocate(0x400);f.vm.put_u32(f.animator+4,f.owner)
        owner=native_owner(f)
        sw=LitSoftwareFixture(executable,512,512)
        sw.lighting((38,38,38),(1,1,1))
        sw.set_texture(64,64,texture,format='ARGB4444')
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),
            (0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
            sw.vm.put_u32(address,value)
        sw.vm.call(0x417d60,(blend,1),this=0x65a57c)
        cases=[]
        for tick in range(1,62):
            prior=count[0]
            if name=='Mist': f.animate();draws=f.original_draws()
            else: f.advance();f.render_state();draws=f.original_draws()
            if tick not in (2,6,16,30,46,60): continue
            sw.clear()
            for draw in draws:
                projected=sw.project(draw['positions'],world_matrix=owner)
                # Original4444 ignores RGB and alpha vertex fields. The executed
                # zero/9/31 controls in test_alpha_scene_state pin this property;
                # zero is a sentinel, not fabricated illumination/material color.
                sw.draw([(*p,0,0,0,0,*uv) for p,uv in zip(projected,draw['uvs'])],
                        indices,z_enabled=True,z_write=False)
            image=folder/f'tick-{tick:03}.png'
            save_rgb565_png(image,sw.vm.surface_bytes('screen'),512,512)
            cases.append(dict(tick=tick,capture=tick-1,draws=len(draws),
                crt_draws_before=prior,image=str(image.resolve()),sha256=sha(image)))
        report=dict(name=name,cases=cases,owner=owner,blend=blend,
            executable_sha256=sha(executable),probe_sha256=sha(__file__),
            source_color_policy='Original ARGB4444 ignores vertex RGBA; zero sentinel plus executed zero/9/31 equivalence control.',
            source_ambient=[38]*3,source_directional=[1]*3,viewport=[512,512],
            scope='Original Init/Animate/Render/pose, original owner atorigin and original software mode/raster. Decoded resource/provider and silent audio boundaries. No normal-light or whole-scene claim is needed for the color-independent4444 kernel.')
        (folder/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
        reports[name]=report
    return reports


def review(reference_dir,metal_dir,output):
    output.mkdir(parents=True,exist_ok=False);reports={}
    for name in ('Mist','SymGlow'):
        original=json.loads((reference_dir/name/'manifest.json').read_text())
        metal=metal_dir/name/'metal-final';capture=json.loads((metal/'manifest.json').read_text())
        if capture['status']!='pass': raise AssertionError('Metal recording failed')
        scenario=capture['scenario']
        for key,value in dict(frames=60,fps=24,warmup=1,seed=1,origin=[0,0,0],
            camera=[256,256],rectangle=[0,0,512,512],background='black').items():
            if scenario[key]!=value: raise AssertionError('Different Metal scenario: '+key)
        log=(metal/'run.log').read_text()
        light=[line for line in log.splitlines() if '[vfx-source-light]' in line]
        if len(light)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0' not in light[0]:
            raise AssertionError('Different declared source lighting')
        rng={int(t):int(n) for t,n in re.findall(r'before_render=(\d+) global_random_draws=(\d+)',log)}
        panel=Image.new('RGB',(512,6*281),(20,20,20));labels=ImageDraw.Draw(panel);cases=[]
        for row,case in enumerate(original['cases']):
            if case['crt_draws_before']!=rng[case['tick']]: raise AssertionError('Native/Metal phase differs')
            native=Path(case['image']);modern=Path(capture['frames'][case['capture']-1]['path'])
            if sha(native)!=case['sha256']: raise AssertionError('Native image changed')
            for column,(path,label) in enumerate(((native,'Native'),(modern,'Metal'))):
                im=Image.open(path).convert('RGB').crop((128,64,384,320))
                panel.paste(im,(column*256,row*281+25))
                labels.text((column*256+4,row*281+4),f'{name} {label} tick{case["tick"]}',fill='white')
            cases.append(dict(tick=case['tick'],capture=case['capture'],native=str(native),metal=str(modern),
                sha256=[sha(native),sha(modern)],crt_draws_before=case['crt_draws_before']))
        panel.save(output/(name+'.png'))
        reports[name]=dict(status='review_ready',accepted=False,cases=cases,source_light_log=light[0],
            binary_sha256=capture['binary_sha256'],native_manifest_sha256=sha(reference_dir/name/'manifest.json'),
            metal_manifest_sha256=sha(metal/'manifest.json'),panel_sha256=sha(output/(name+'.png')),
            crop=[128,64,384,320],appearance_fitted=False,
            scope='Six elapsed phases, same original RNG cadence, literal geometry/texture and software alpha kernel versus actual Metal. Common projector and destination RGB565 quantization differ; natural caller/lighting scene/device parity are separate.')
    (output/'manifest.json').write_text(json.dumps(reports,indent=2)+'\n')
    return reports


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--reference',type=Path);p.add_argument('--metal',type=Path)
    a=p.parse_args()
    if bool(a.reference)!=bool(a.metal): p.error('Review needs --reference and --metal together')
    result=review(a.reference,a.metal,a.output) if a.reference else reference(a.executable,a.archive,a.output)
    print('PASS',', '.join(result),'native/Metal phase artifacts')
