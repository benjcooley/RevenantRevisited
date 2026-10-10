#!/usr/bin/env python3
"""Original speed punctuation, # matrices and per-object blend carrier proof.

Executes unmodified NewObject punctuation and RenderObject blend fragments plus
whole CalcObjectMatrix and software SetBlendMode. Mesh/device and raster remain
unexecuted; this does not establish rendered acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile

from unicorn.x86_const import UC_X86_REG_EBX,UC_X86_REG_ECX,UC_X86_REG_ESP,UC_X86_REG_EIP
from speed_controller_preflight import NativeSpeed, read_asset, ROOT, SPEED_SHA


def punctuation(f):
    v=f.vm;sp=v.STACK+v.STACK_SIZE-0x1000;rows=[]
    for index,o in enumerate(f.asset['objects']):
        name=f.put_string(o['name']);obj=f.animobjs[index]
        v.put_u32(sp+0x1c,name)
        v.uc.reg_write(UC_X86_REG_ESP,sp);v.uc.reg_write(UC_X86_REG_ECX,name)
        v.uc.reg_write(UC_X86_REG_EBX,obj)
        v.uc.emu_start(0x409f61,0x409fe0,count=10000)
        if v.uc.reg_read(UC_X86_REG_EIP)!=0x409fe0:raise AssertionError('Punctuation fragment did not finish')
        rows.append(dict(object_index=index,name=o['name'],flags=hex(v.u32(obj)),blend=v.u32(obj+0x348)))
    if [v.u32(p)&0x20000000 for p in f.animobjs]!=[0,0x20000000,0x20000000]:
        raise AssertionError('Exact Speed punctuation flags differ')
    return rows


def carrier(f,index):
    v=f.vm;sp=v.STACK+v.STACK_SIZE-0x1000
    for address,value in ((0x5d7a28,1),(0x66818c,0),(0x5e91c0,0),
        (0x5e8740,0),(0x5c61ac,1),(0x5e8790,0),(0x5e88b4,0x13579bdf)):
        v.put_u32(address,value)
    v.put_u32(sp+0xc8,f.animobjs[index]);v.uc.reg_write(UC_X86_REG_ESP,sp)
    v.uc.emu_start(0x40adfc,0x40aeba,count=100000)
    if v.uc.reg_read(UC_X86_REG_EIP)!=0x40aeba:raise AssertionError('Blend carrier did not finish')
    row=dict(object_index=index,bone_mode=v.u32(f.animobjs[index]+0x348),
        requested_mode=v.u32(0x5e88b4),srcblend=v.u32(0x675fc8),dstblend=v.u32(0x675f10),
        z_enabled=v.uc.mem_read(0x5e595d,1)[0],z_write=v.uc.mem_read(0x5e595c,1)[0])
    if row['srcblend']!=2 or row['dstblend']!=2 or row['z_write']!=0:
        raise AssertionError('Original Speed carrier is not additive/no-write')
    return row


def run(executable,archive,output):
    output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(archive)as z:asset=read_asset(z.read('Imagery/Magic/speed.I3D'))
    if asset['sha256']!=SPEED_SHA:raise ValueError('Speed asset changed')
    asset['name']='speed';f=NativeSpeed(executable,asset)
    names=punctuation(f);initialization=f.initialize(0)
    cold=carrier(f,1)
    if cold['requested_mode']!=80 or cold['z_enabled']!=0:
        raise AssertionError('Explicit native cold base mode80 changed')
    trace=f.state_trace(90)
    particle=carrier(f,2);base=carrier(f,1)
    if particle['requested_mode']!=16 or particle['z_enabled']!=1 or base!=cold:
        raise AssertionError('Particle16/base80 per-object native policy changed')
    matrices=[]
    v=f.vm
    for frame in range(asset['state0_frames']):
        if v.call(0x40a420,(f.animobjs[1],0,frame,0,0),this=f.imagery)!=1:
            raise AssertionError('Actual #speedflare matrix rejected')
        matrices.append(dict(frame=frame,position=list(struct.unpack('<3f',v.uc.mem_read(f.animobjs[1]+0x10,12))),
            rotation=list(struct.unpack('<3f',v.uc.mem_read(f.animobjs[1]+0x28,12))),
            scale=list(struct.unpack('<3f',v.uc.mem_read(f.animobjs[1]+0x40,12))),
            matrix=list(struct.unpack('<16f',v.uc.mem_read(f.animobjs[1]+0x58,64)))))
    (output/'asset-triage.json').write_text(json.dumps([asset],indent=2)+'\n')
    (output/'retail-state-trace.json').write_text(json.dumps(trace,indent=2)+'\n')
    (output/'retail-camera-matrices.json').write_text(json.dumps(matrices,indent=2)+'\n')
    report=dict(status='pass',native_punctuation=names,native_initialization=initialization,
        cold_base_carrier=cold,live_particle_carrier=particle,post_particle_base_carrier=base,
        camera_matrix_frames=len(matrices),asset_sha256=SPEED_SHA,
        probe_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        original_executable_sha256=hashlib.sha256(executable.read_bytes()).hexdigest(),
        actual_code=['NewObject409f61..409fe0 punctuation','wholeCalcObjectMatrix40a420',
            'RenderObject40adfc..40aeba blendcarrier','wholeSetBlendMode417d60/SceneSetState417060/software56d400'],
        no_blend_or_matrix_intercepts=True,accepted=False,new_rendered_ab_cases=0,
        scope='Exact native Speed punctuation, #emitter/base matrix and explicit per-object softwareblend '
            'carrier. Decoder/provider and copied exact prototype boundaries explicit; full mesh/material '
            'dispatch/projector/raster/runtime/Metal/caster not yet compared.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();r=run(a.executable,a.archive,a.output)
    print(json.dumps({k:v for k,v in r.items()if k not in('native_initialization','native_punctuation')},indent=2))
