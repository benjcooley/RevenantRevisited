#!/usr/bin/env python3
"""Bounded native FireWind null-spell motion; no port or rendering acceptance."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,RETAIL_SHA
from fountain_probe import FountainFixture

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='ab55c2c0bacb33c2dad3d5a84aa51f57fe502fec498045ded9aebd10862266ea'
POOL_COUNT=400
RECORD_BYTES=92


def sha(data):return hashlib.sha256(data).hexdigest()


def preflight(executable,archive):
    with zipfile.ZipFile(archive)as assets:
        member=next(n for n in assets.namelist()if n.lower()=='imagery/magic/firewind.i3d')
        asset=assets.read(member)
    if sha(asset)!=ASSET_SHA:raise ValueError('Literal FireWind asset revision changed')
    software=SoftwareFixture(executable);vm=software.vm
    if sha(vm.image)!=RETAIL_SHA:raise ValueError('This fixed-address preflight requires unchanged retail')
    vm.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
    animator=vm.allocate(0x9300);owner=vm.allocate(0x400);imagery=vm.allocate(0x200);vtable=vm.allocate(0x200)
    vm.put_u32(animator,0x5a9404);vm.put_u32(animator+4,owner);vm.put_u32(animator+8,imagery);vm.put_u32(owner,vtable)
    if vm.u32(0x5a9404+6*4)!=0x4e2400 or vm.u32(0x5a9404+11*4)!=0x4e2620:
        raise AssertionError('Pinned original animator dispatch changed')
    done=vm.allocate(16);vm.write_code(done,b'\x31\xc0\xc2\x04\x00');vm.put_u32(vtable+0x158,done)
    boundaries={0x40dd60:0,0x40e2e0:0,0x49c430:4}
    def external(uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff)
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+boundaries[address])
    for address in boundaries:vm.uc.hook_add(UC_HOOK_CODE,external,begin=address,end=address)
    class RangeObserver:
        observe_random=FountainFixture.observe_random
    observer=RangeObserver();observer.vm=vm;observer.random=[];observer.random_pending=[]
    for address in(0x483300,0x483312,0x48332c):
        vm.uc.hook_add(UC_HOOK_CODE,observer.observe_random,begin=address,end=address)
    vm.call(0x4e2400,this=animator)
    samples=[];failure=None
    for tick in range(101):
        if tick:
            try:vm.call(0x4e2620,this=animator)
            except Exception as error:
                failure=dict(tick=tick,error=str(error),pc=hex(vm.uc.reg_read(UC_X86_REG_EIP)));break
        if tick in(0,1,10,28,39,40,60,80,100):
            active=sum(vm.u32(animator+0x13c+i*RECORD_BYTES+60)!=0 for i in range(POOL_COUNT))
            samples.append(dict(tick=tick,frame=vm.u32(animator+0x118),active_slots=active,
                random_calls=len(observer.random),slot0_position=list(struct.unpack('<3f',vm.uc.mem_read(animator+0x13c,12))),
                complete_pool_sha256=sha(vm.uc.mem_read(animator+0x13c,POOL_COUNT*RECORD_BYTES))))
    moving=any(s['slot0_position']!=samples[0]['slot0_position']for s in samples[1:])
    if failure is None:
        if samples[0]['active_slots']!=100 or samples[0]['random_calls']!=300:
            raise AssertionError('Original birth metrics changed')
        if samples[-1]['frame']!=100 or samples[-1]['active_slots']!=387 or samples[-1]['random_calls']!=4404 or not moving:
            raise AssertionError('Original null-spell motion metrics changed')
    return dict(status='native_nullspell_feasible_port_reconstruction_deferred'if failure is None else'native_boundary_failed',
        retail_sha256=RETAIL_SHA,asset_member=member,asset_sha256=ASSET_SHA,
        original_registration='0x4e23ea',builder_vtable='0x5a9400',animator_vtable='0x5a9404',
        original_initialize='0x4e2400',original_animate='0x4e2620',original_render='0x4e2c80',
        pool_count=POOL_COUNT,record_bytes=RECORD_BYTES,pool_offset='0x13c',initial_sphere_slots=100,
        samples=samples,failure=failure,moving_particles_executed=moving,
        explicit_boundaries=['base animator Init/Animate services','silent audio lookup','owner SetCommandDone facade'],
        source_sha256={name:sha(Path(__file__).with_name(name).read_bytes())for name in('firewind_preflight.py','software_probe.py','fountain_probe.py')},
        current_port='Single2second billboard placeholder with invented sine scale/fade; no literal pool to correct arithmetically.',
        spell_target_damage_tested=False,rendered_ab_cases=0,engine_changes=False,dosbox_used=False,
        next_step='Separate source-grounded400slot controller/render port task; no arithmetic-only equivalence claim.')


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();report=preflight(args.executable,args.archive)
    args.output.mkdir(parents=True,exist_ok=False)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))
    if report['failure']:raise SystemExit(1)
