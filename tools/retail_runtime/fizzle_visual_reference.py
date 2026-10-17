#!/usr/bin/env python3
"""Fizzle lifecycle through original owner, relative matrix carrier and ARGB4444 raster.

Resource lookup/RenderObject dispatch is a boundary; its original relative world
compose and the software transform/raster are executed. No fitted coordinates.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
from fizzle_probe import FizzleFixture,SYSTEM_OBJECTS,INDICES,ROOT
from firecone_owner_probe import native_owner,native_world
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png


def sha(data):return hashlib.sha256(data).hexdigest()


def run(executable,archive,output,origin=(0,0,0)):
    output.mkdir(parents=True,exist_ok=False)
    with zipfile.ZipFile(archive) as z:asset=z.read('Imagery/Magic/fizzle.i3d')
    f=FizzleFixture(executable,asset);f.reset()
    f.vm.write(f.owner+0x10,struct.pack('<3i',*origin))
    sw=LitSoftwareFixture(executable,640,480);sw.lighting((38,38,38),(1,1,1));v=sw.vm
    for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
        v.put_u32(address,value)
    v.call(0x417d60,(2,1),this=0x65a57c)
    f.animate() # one explicit warmup tick, as in the actual capture
    cases=[]
    for capture in range(1,41):
        rng_before=len(f.random)
        f.animate();owner=native_owner(f);f.original_draws()
        packets=[]
        for packet in f.packets:
            obj=SYSTEM_OBJECTS[packet['system']]
            world=native_world(f,dict(object=obj,matrix=packet['matrix']))
            packets.append((obj,world))
        hashes=[]
        for replay in range(2):
            sw.clear()
            for obj,world in packets:
                offset=(0x8a4,0x10a8,0x18ac)[obj]
                sw.set_texture(32,32,asset[offset:offset+2048],format='ARGB4444')
                projected=sw.project(f.points[obj],world_matrix=world)
                # Original ARGB4444 path ignores per-vertex color; zero sentinel.
                sw.draw([(*point,0,0,0,0,*uv) for point,uv in zip(projected,f.uvs[obj])],
                        INDICES,z_enabled=True,z_write=False)
            rgb=v.surface_bytes('screen');depth=v.surface_bytes('depth')
            hashes.append((sha(rgb),sha(depth)))
        if hashes[0]!=hashes[1]:raise AssertionError('Warm native replay changed')
        path=output/f'native-{capture:03d}.png';save_rgb565_png(path,rgb,640,480)
        cases.append(dict(capture=capture,tick=capture+1,alive=f.state()['alive'],draws=len(packets),
                          rng_draws=len(f.random),rng_before=rng_before,owner_matrix=owner,image=str(path),sha256=sha(path.read_bytes()),
                          rgb565_sha256=hashes[0][0],depth_sha256=hashes[0][1],
                          nonzero_pixels=sum(x!=(0,) for x in struct.iter_unpack('<H',rgb))))
    report=dict(status='reference_complete',origin=list(origin),cases=cases,source_mode=2,
                executable_sha256=sha(Path(executable).read_bytes()),asset_sha256=sha(asset),
                source_ambient=[38]*3,source_directional=[1]*3,color_policy='ARGB4444 ignores vertex color',
                functions=['4f3f90 Init','4f4050 Animate','4f4440 Render','50c220 particle Render(1,0)',
                           '40e470/40e740 owner','40aad5..40ab92 relative world','original software projector and ARGB4444 raster'],
                warm_replays=2,appearance_fitted=False,accepted=False,
                scope='Whole original leaf lifecycle and actual original RNG, owner/world/projector/raster; imagery lookup and complete RenderObject dispatch are explicit boundaries. No natural spell/map/device claim.',
                probe_sha256=sha(Path(__file__).read_bytes()))
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True)
    p.add_argument('--origin',type=int,nargs=3,default=(0,0,0));a=p.parse_args()
    r=run(a.executable,a.archive,a.output,a.origin)
    print(r['status'],len(r['cases']),'frames;',sum(bool(c['nonzero_pixels'])for c in r['cases']),'visible')
