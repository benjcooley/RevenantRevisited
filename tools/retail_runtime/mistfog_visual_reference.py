#!/usr/bin/env python3
"""Whole MistFog native owner/normal-light/software lifecycle reference.

Asset-provider and base-registration interfaces are explicit fixture boundaries;
leaf state/Render, owner matrix, relative RenderObject carrier, Illuminate and
RGB565 raster execute original instructions. No candidate colors/poses enter.
"""
import argparse
import json
import struct
import zipfile
from pathlib import Path
from mistfog_probe import MistFogFixture, INDICES, ROOT, sha
from firecone_owner_probe import native_owner, native_world
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png


def run(executable, archive, output):
    output.mkdir(parents=True, exist_ok=False)
    with zipfile.ZipFile(archive) as z:
        asset=z.read('Imagery/Misc/mistfog.i3d')
    f=MistFogFixture(executable,asset);f.reset();f.objects=[f.obj]
    vertices=list(struct.iter_unpack('<8f',asset[0xfc:0x17c]))
    sw=LitSoftwareFixture(executable,640,340)
    sw.set_texture(64,64,asset[0x2d4:0x22d4],format='RGB565')
    v=sw.vm
    for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),
                          (0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
        v.put_u32(address,value)
    v.call(0x417d60,(8,1),this=0x65a57c)
    cases=[];lifecycle=[]
    f.animate() # One complete render/update warmup, matching FrameSnap.
    for capture in range(1,241):
        before=f.state()['random_count'];f.animate()
        lifecycle.append(dict(capture=capture,tick=capture+1,rng_before=before,
                              rng_after=f.state()['random_count']))
        if capture not in (1,5,15,29,45,59,99,149,199,239,240):continue
        owner=native_owner(f);f.original_draws();packets=[]
        for p in f.packets:
            f.vm.put_u32(f.obj,p['flags'])
            f.vm.write(f.obj+0x10,struct.pack('<3f',*p['position']))
            f.vm.write(f.obj+0x40,struct.pack('<3f',*p['scale']))
            f.vm.call(0x40a420,(f.obj,0,0,0,0),this=f.imagery)
            matrix=struct.unpack('<16f',f.vm.uc.mem_read(f.obj+0x58,64))
            packets.append(native_world(f,dict(object=0,matrix=matrix)))
        def raster(ambient=(38,38,38),directional=(1,1,1)):
            sw.lighting(ambient,directional);sw.clear();colors=set()
            for matrix in packets:
                _,_,transformed=sw.draw_authored(vertices,INDICES,matrix)
                colors.update(tuple(p[3:7]) for p in transformed)
            return v.surface_bytes('screen'),v.surface_bytes('depth'),sorted(colors)
        pixels,depth,colors=raster()
        if raster()!=(pixels,depth,colors):raise AssertionError('Native warm replay differs')
        path=output/f'native-capture{capture:03}.png'
        save_rgb565_png(path,pixels,640,340)
        case=dict(capture=capture,tick=capture+1,rng_before=before,owner_matrix=owner,
                  packets=len(packets),rgb5=colors,path=str(path.resolve()),
                  sha256=sha(path.read_bytes()),depth_sha256=sha(depth),
                  nonzero_pixels=sum(x!=(0,) for x in struct.iter_unpack('<H',pixels)))
        if capture==1:
            dark,_,dark_colors=raster((8,8,8),(0,0,0))
            bright,_,bright_colors=raster((96,96,96),(1,1,1))
            case['light_controls']=dict(dark_ambient=[8]*3,dark_directional=[0]*3,
                dark_rgb5=dark_colors,dark_nonzero=sum(x!=(0,) for x in struct.iter_unpack('<H',dark)),
                brighter_ambient=[96]*3,brighter_directional=[1]*3,brighter_rgb5=bright_colors,
                brighter_nonzero=sum(x!=(0,) for x in struct.iter_unpack('<H',bright)))
        cases.append(case)
    report=dict(status='reference_complete',accepted=False,name='MistFog',type_id='0x180674ba',
        executable_sha256=sha(executable.read_bytes()),asset_sha256=sha(asset),
        probe_sha256=sha(Path(__file__).read_bytes()),
        fixture_sha256=sha((ROOT/'tools/retail_runtime/lit_software_probe.py').read_bytes()),
        configuration=dict(origin=[0,0,0],camera=[0,0,0],viewport=[640,340],source_ambient=[38]*3,
            source_directional=[1]*3,scene_ambient=[32,255,255,255],point_lights=[],
            blend=8,cull=3,z_test=True,z_write=False,fps=24,seed=1,warmup=1),
        cases=cases,lifecycle=lifecycle,scope=__doc__)
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    r=run(a.executable,a.archive,a.output)
    print('Original MistFog normal-lit lifecycle:',len(r['cases']),'samples',flush=True)
