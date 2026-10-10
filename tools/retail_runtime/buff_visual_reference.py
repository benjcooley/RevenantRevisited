#!/usr/bin/env python3
"""Original full buff composites for fixed-origin actual Metal visual review.

Authored parser, pose, Pulse, RenderSample, matrices, illumination and raster
execute in the pinned retail binary. No production vertices/colors are inputs.
"""
import argparse
import json
from pathlib import Path
import struct
import zipfile
from buff_base_lighting_probe import PROFILES, punctuation
from speed_controller_preflight import NativeSpeed, read_asset, ROOT
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from firecone_state_probe import sha


def run(executable, output):
    output.mkdir(parents=True, exist_ok=False)
    cases=[]
    state=1;rng_counts={state:0}
    for count in range(1,10001):
        state=(state*214013+2531011)&0xffffffff;rng_counts[state]=count
    for name, member, base_index, base_blend, pinned in PROFILES:
        destination=output/name;destination.mkdir()
        with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
            members={p.lower():p for p in archive.namelist()}
            data=archive.read(members['imagery/magic/'+member+'.i3d'])
        if sha(data)!=pinned:raise ValueError('Shipped asset changed')
        asset=read_asset(data);asset['name']=name
        f=NativeSpeed(executable,asset);punctuation(f);initialization=f.initialize(0)
        # Preview/map NextFrame precedes Pulse. Capture skips render1 as warmup:
        # first recorded image follows Pulse2 at animation frame2.
        trace=f.state_trace(61,frame_offset=1,move_tick=None)
        software=LitSoftwareFixture(executable,640,340)
        software.lighting((38,38,38),(1,1,1));v=software.vm
        for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),(0x5e8740,0),(0x5c61ac,1),(0x5e8790,0)):
            v.put_u32(address,value)
        u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p)
        textures=[]
        for index in range(asset['textures']):
            descriptor=r(asset['body_offset']+40)+index*120
            height,width=struct.unpack_from('<2I',data,descriptor+8);bits=r(r(descriptor+108))
            textures.append((width,height,data[bits:bits+width*height*2]))
        prototype=asset['objects'][f.prototype_index]
        raw=list(struct.iter_unpack('<8f',bytes.fromhex(prototype['vertex_hex'])))
        base=asset['objects'][base_index];vertices=list(struct.iter_unpack('<8f',bytes.fromhex(base['vertex_hex'])))
        indices=list(struct.unpack_from('<'+'H'*(base['texture_runs'][3]*3),bytes.fromhex(asset['face_hex']),base['texture_runs'][2]*6))
        nv=f.vm;obj=f.animobjs[f.prototype_index];point=nv.allocate(12);result=nv.allocate(12)
        for tick in (1,5,15,29,45,59):
            software.clear();software.set_texture(*textures[1 if name != 'Fmastery' else 0]);v.call(0x417d60,(16,1),this=0x65a57c)
            count=0
            for row in trace['rows']:
                if row[0]!=tick or not row[3]:continue
                sample=row[20:]
                nv.write(obj+0x10,struct.pack('<3f',*sample[:3]));nv.write(obj+0x28,struct.pack('<3f',*sample[3:6]));nv.write(obj+0x40,struct.pack('<3f',*[sample[10]]*3))
                if nv.call(0x40a420,(obj,0,0,0,0),this=f.imagery)!=1:continue
                positions=[]
                for vertex in raw:
                    nv.write(point,struct.pack('<3f',*vertex[:3]));nv.call(0x43ad80,(obj+0x58,point,result))
                    positions.append(struct.unpack('<3f',nv.uc.mem_read(result,12)))
                projected=software.project(positions);color=[(int(c)&255)>>3 for c in sample[6:9]]
                software.draw([(*p,*color,int(sample[9]*255)>>3,*t[6:]) for p,t in zip(projected,raw)], [2,3,0,1,2,0],z_enabled=True,z_write=False,instruction_limit=5000000)
                count+=1
            frame=(tick+1)%asset['state0_frames']
            if nv.call(0x40a420,(f.animobjs[base_index],0,frame,0,0),this=f.imagery)!=1:raise AssertionError('Base rejected')
            matrix=struct.unpack('<16f',nv.uc.mem_read(f.animobjs[base_index]+0x58,64))
            software.set_texture(*textures[0]);v.call(0x417d60,(base_blend,1),this=0x65a57c)
            image,depth,transformed=software.draw_authored(vertices,indices,matrix,z_enabled=base_blend!=80)
            path=destination/f'native-capture{tick:03d}.png';save_rgb565_png(path,image,640,340)
            prior_rng=next(row[2] for row in trace['rows'] if row[0]==tick-1)
            cases.append(dict(name=name,capture=tick,animation_frame=frame,particles=count,native_rng_before_capture=prior_rng,native_rng_draws_before_capture=rng_counts[prior_rng],nonzero_pixels=sum(x!=(0,) for x in struct.iter_unpack('<H',image)),image_sha256=sha(path.read_bytes()),base_colors=[list(x[3:7])for x in transformed]))
    report=dict(status='reference_complete',original_executable_sha256=sha(executable.read_bytes()),native_fixture_sha256=sha((ROOT/'tools/retail_runtime/lit_software_probe.py').read_bytes()),cases=cases,accepted=False,metal_compared=False,configuration=dict(origin=[0,0,0],facing=0,viewport=[640,340],center=[320,170],quality=0,seed=1,ambient_rgb=[38]*3,directional_rgb=[1]*3,directional_direction=[0,-.78125,-.625],point_lights=[],fmastery_incoming_blend=16),scope='Complete isolated native particle+normal-lit base at declared origin, independently executed native poses and sample colors. NextFrame-before-Pulse cadence, render1 warmup, captures1/5/15/29/45/59. Literal RGB565 assets, original mode16 then base80 (Speed/Quicksilver) or16 (Fmastery). Modern GPU common projection and scene ownership are assessed separately. No fitted positions, colors or camera.',probe_sha256=sha(Path(__file__).read_bytes()))
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(json.dumps(run(a.executable,a.output),indent=2))
