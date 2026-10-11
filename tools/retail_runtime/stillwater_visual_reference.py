#!/usr/bin/env python3
"""Whole default-renderer StillWater reference and actual Metal comparison.

Requires the shared scroll_water_visual_reference fixture. StillWater has no
controller tags and one constant active frame; it does not simulate scrolling.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
from PIL import Image,ImageDraw
import scroll_water_visual_reference as shared
from software_probe import save_rgb565_png

ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def run(executable,archive,metal,output):
    output.mkdir(parents=True,exist_ok=False)
    profiles=json.loads((ROOT/'tools/retail_runtime/static_additional_profiles.json').read_text())['profiles']
    profile=next(p for p in profiles if p['name']=='StillWater')
    with zipfile.ZipFile(archive)as z:
        member=next(n for n in z.namelist() if n.lower()=='imagery/misc/stillwater.i3d')
        asset=z.read(member)
    if hashlib.sha256(asset).hexdigest()!=profile['asset_sha256']:raise AssertionError('Different asset')
    f=shared.NativeScrollScene(executable,asset,'StillWater');init=f.initialize_scene()
    if init['controllers'] or f.asset['state0_frames']!=1:raise AssertionError('StillWater contract changed')
    m=json.loads((metal/'manifest.json').read_text())
    if m['status']!='pass':raise AssertionError('Metal capture failed')
    for key,value in dict(effect='TWaterEffect_BESPOKE__StillWater',frames=60,fps=24,
        warmup=1,seed=1,origin=[0,0,0],camera=[320,170],rectangle=[0,0,640,340],
        resolution=[640,340],background='black',ambient=[32,255,255,255]).items():
        if m['scenario'][key]!=value:raise AssertionError('Different Metal scenario: '+key)
    log=(metal/'run.log').read_text();light=[l for l in log.splitlines()if '[vfx-source-light]'in l]
    if len(light)!=1 or 'ambient=(0.149019614,0.149019614,0.149019614) directional=(1,1,1) point_lights=0'not in light[0]:
        raise AssertionError('Different source lighting')
    cases=[];native_hashes=set();metal_hashes=set();panel=Image.new('RGB',(1280,6*365));labels=ImageDraw.Draw(panel)
    for row,capture in enumerate((1,5,15,29,45,59)):
        raw,depth,draw=f.render(capture+1,0);repeat=f.render(capture+1,0)
        if raw!=repeat[0]or depth!=repeat[1]:raise AssertionError('Native warm replay differs')
        if len(draw['draws'])!=1 or draw['draws'][0]['args'][1]!=0x112:raise AssertionError('Whole RenderObject dispatch differs')
        p=output/f'native-capture{capture:03}.png';save_rgb565_png(p,raw,640,340)
        q=Path(m['frames'][capture-1]['path'])
        if sha(q)!=m['frames'][capture-1]['sha256']:raise AssertionError('Metal capture changed')
        native_hashes.add(hashlib.sha256(raw).hexdigest());metal_hashes.add(Image.open(q).convert('RGB').tobytes())
        for col,(path,label)in enumerate(((p,'Original'),(q,'Metal'))):
            panel.paste(Image.open(path).convert('RGB'),(col*640,row*365+25));labels.text((col*640+3,row*365+3),f'{label} tick{capture+1}',fill='white')
        cases.append(dict(capture=capture,global_tick=capture+1,owner_frame=0,draw=draw,
            native=str(p.resolve()),metal=str(q),sha256=[sha(p),sha(q)],depth_sha256=hashlib.sha256(depth).hexdigest(),
            native_nonzero_pixels=sum(x!=(0,)for x in struct.iter_unpack('<H',raw))))
    if len(native_hashes)!=1 or len(metal_hashes)!=1:raise AssertionError('Fixed STILL frame changed')
    panel.save(output/'contact.png')
    report=dict(status='review_ready',accepted=False,name='StillWater',type_id='0xadbcef14',
        asset_sha256=profile['asset_sha256'],original_executable_sha256=sha(executable),
        probe_sha256=sha(__file__),shared_fixture_sha256=sha(shared.__file__),initialization=init,
        binary_sha256=m['binary_sha256'],metal_manifest_sha256=sha(metal/'manifest.json'),
        source_light_log=light[0],configuration=dict(origin=[0,0,0],camera=[0,0,0],viewport=[640,340],
            source_ambient=[38]*3,directional=[1]*3,initial_mode=0,fps=24,seed=1,warmup=1),
        cases=cases,panel_sha256=sha(output/'contact.png'),appearance_fitted=False,
        scope='Whole original default factory/owner/key/RenderObject/FVF_VERTEX/ARGB4444 software render versus actual Metal registered authored preview. No tags, one constant frame. Shared decoded resource/COM material/texture residency boundaries; framebuffer/projection residual and original map context remain separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--metal',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();r=run(a.executable,a.archive,a.metal,a.output);print('PASS',len(r['cases']),'StillWater whole-frame pairs')
