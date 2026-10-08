#!/usr/bin/env python3
"""Warm original-x86 partsys state/pose A/B; this does not draw pixels.

Field layout and external boundaries reuse the retained retail oracle at
RevenantRetailLab/research/partsys/verify_retail.py. The production state module
is compiled directly; there is no second Python transcription of its equations.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import statistics
import struct
import subprocess
import time

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP
from runtime import Runtime

RETAIL_SHA = '28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
ROOT = Path(__file__).resolve().parents[2]
FIELDS = ('tick', 'rng', 'alive', 'age', 'x', 'y', 'z', 'vx', 'vy', 'vz',
          'emission_credit', 'render_x', 'render_y', 'render_z', 'rot_x',
          'rot_y', 'rot_z', 'red', 'green', 'blue', 'alpha', 'scale')
IDENTITY = (1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.)


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


class PartsysProbe:
    """One PE load, one checkpoint, independent bounded warm replays."""
    def __init__(self, executable):
        self.vm = Runtime(executable)
        if hashlib.sha256(self.vm.image).hexdigest() != RETAIL_SHA:
            raise ValueError('This fixture requires the verified retail executable')
        vm = self.vm
        self.controller = vm.allocate(0x200)
        self.animator = vm.allocate(0x200)
        self.owner = vm.allocate(0x200)
        self.emitter = vm.allocate(0x200)
        self.emitters = vm.allocate(4)
        self.particles = vm.allocate(10 * 252)
        self.prototype = vm.allocate(0x200)
        self.vertices = vm.allocate(32)
        c, a, o, e, p, t = (self.controller, self.animator, self.owner,
                            self.emitter, self.particles, self.prototype)
        for offset, value in ((12,a), (20,o), (24,1), (40,self.emitters),
                              (44,p), (248,10), (356,t)):
            vm.put_u32(c + offset, value)
        vm.put_u32(self.emitters, e)
        for offset, value in ((16,10), (20,20), (24,30)):
            vm.put_u32(o + offset, value)
        for i in range(3):
            self.put_float(e + 64 + 4*i, 1.)
        for offset, value in ((0x10c,24), (0x110,10), (0x114,10),
                              (0x118,2), (0x11c,2), (0x120,30), (0x124,30)):
            self.put_float(c + offset, value)
        vm.put_u32(c + 0x108, 2)
        for offset, value in ((0,.5), (0x50,106), (0x54,126), (0x58,155),
                              (0x60,1), (0x64,1), (0xac,-10), (0xb0,-10)):
            self.put_float(c + 0x44 + offset, value)
        for slot in range(10):
            vm.put_u32(p + slot*252 + 0xdc, t)
            vm.put_u32(p + slot*252 + 0xe0, o)
        # Only exact external function entries are intercepted. All original
        # Pulse/spawn/update/curve/x87-transform instructions remain executable.
        self.boundaries = {
            0x58c582:'MSVC rand',
            0x40efc0:'emitter origin',
            0x40a420:'emitter matrix',
            0x40ef80:'prototype transform',
            0x452e10:'walk height',
        }
        self.calls = {}
        self.original_calls = {}
        for address in self.boundaries:
            vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x403760,0x4031c0,0x402a20,0x401390,0x43ad80,0x4026d0):
            vm.uc.hook_add(UC_HOOK_CODE,self.original_entry,begin=address,end=address)
        vm.checkpoint()

    def put_float(self,address,value):
        self.vm.write(address,struct.pack('<f',value))

    def read_float(self,address):
        return struct.unpack('<f',self.vm.uc.mem_read(address,4))[0]

    def external(self,uc,address,size,user):
        vm = self.vm
        sp = uc.reg_read(UC_X86_REG_ESP)
        self.calls[hex(address)] = self.calls.get(hex(address),0)+1
        cleanup = 0
        if address == 0x58c582:
            uc.reg_write(UC_X86_REG_EAX,vm.random_msvc())
        elif address == 0x40efc0:
            vm.write(vm.u32(sp+8),struct.pack('<3f',0,0,0));cleanup = 12
        elif address == 0x40a420:
            vm.write(vm.u32(sp+16),struct.pack('<16f',*IDENTITY));cleanup = 20
        elif address == 0x40ef80:
            cleanup = 8  # Explicit identity imagery/animator fixture transform.
        elif address == 0x452e10:
            uc.reg_write(UC_X86_REG_EAX,0);cleanup = 12
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)

    def original_entry(self,uc,address,size,user):
        self.original_calls[hex(address)] = self.original_calls.get(hex(address),0)+1

    def configure(self,shape=0,face=0,render=False,seed=1,owner=(10,20,30)):
        """Prepare the explicitly synthetic emitter fixture without editor input."""
        if type(shape) is not int or shape not in range(4):
            raise ValueError('shape must be an integer from 0 to 3')
        if type(face) is not int or face not in range(256):
            raise ValueError('face must be an integer from 0 to 255')
        if type(seed) is not int or not 0 <= seed <= 0xffffffff:
            raise ValueError('seed must fit uint32')
        if type(render) is not bool:
            raise ValueError('render must be a boolean')
        if len(owner)!=3 or any(type(v) is not int or not -0x80000000 <= v <= 0x7fffffff for v in owner):
            raise ValueError('owner must be three signed 32-bit fixture coordinates')
        vm = self.vm
        self.restored_pages = vm.restore()
        self.calls = {}
        self.original_calls = {}
        self.rows = []
        self.tick = 0
        self.config = dict(shape=shape,face=face,render=render,seed=seed,owner=list(owner))
        vm.rng_seed=seed
        c,p,t = self.controller,self.particles,self.prototype
        vm.put_u32(c+0x104,shape)
        vm.write(self.owner+54,bytes([face]))
        for offset,value in zip((16,20,24),owner):
            vm.put_u32(self.owner+offset,value & 0xffffffff)
        if render:
            for offset,value in ((0x10,1),(0x14,2),(0x18,3),(0x28,10),
                                 (0x2c,20),(0x30,30),(0x60,.3333)):
                self.put_float(c+0x44+offset,value)
            vm.put_u32(t+0xa0,1);vm.put_u32(t+0xa4,self.vertices)
            vm.put_u32(0x5d7a28,1)
        return self.inspect()

    def step(self,ticks=1):
        if type(ticks) is not int or not 0 <= ticks <= 10000:
            raise ValueError('ticks must be an integer from 0 to 10000')
        if not hasattr(self,'config'):
            raise ValueError('Configure the fixture before stepping')
        vm=self.vm;c,p,t=self.controller,self.particles,self.prototype
        for _ in range(ticks):
            tick=self.tick
            vm.put_u32(self.animator+20,tick)
            vm.call(0x403760,this=c)
            sample = []
            if self.config['render']:
                # Stop after ORIGINAL pose/color setup, before device submission.
                vm.call(0x4026d0,this=p,stop_address=0x4028e2)
                packed = vm.u32(self.vertices+16)
                sample = [*[self.read_float(t+0x10+4*i) for i in range(3)],
                          *[self.read_float(t+0x28+4*i) for i in range(3)],
                          (packed>>16)&255,(packed>>8)&255,packed&255,
                          (packed>>24)/255,self.read_float(t+0x40)]
            self.rows.append([tick,vm.rng_seed,vm.u32(p+0xe4),vm.u32(p+0xd0),
                         *[self.read_float(p+0xb8+4*i) for i in range(3)],
                         *[self.read_float(p+0xc4+4*i) for i in range(3)],
                         self.read_float(c+0x16c),*sample])
            vm.advance()
            self.tick+=1
        return self.inspect()

    def inspect(self):
        vm=self.vm;c,p=self.controller,self.particles
        # Covers complete pool influence/replay, beyond compared first-slot fields.
        state = bytes(vm.uc.mem_read(c,0x200)) + bytes(vm.uc.mem_read(p,10*252))
        return dict(rows=copy.deepcopy(self.rows),external_calls=dict(self.calls),
                    original_function_calls=dict(self.original_calls),
                    state_sha256=hashlib.sha256(state).hexdigest(),
                    milliseconds=vm.milliseconds,restored_pages=self.restored_pages)

    def checkpoint(self):
        self.vm.checkpoint()
        self.control_snapshot=copy.deepcopy(dict(calls=self.calls,original_calls=self.original_calls,
            rows=self.rows,tick=self.tick,config=self.config))

    def reset(self):
        if not hasattr(self,'control_snapshot'):
            raise ValueError('Fixture has no control checkpoint')
        self.restored_pages=self.vm.restore()
        for name,value in copy.deepcopy(self.control_snapshot).items():
            setattr(self,name,value)
        return self.inspect()

    def replay(self,shape,face,render):
        self.configure(shape,face,render)
        return self.step(30)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--repeat',type=int,default=3)
    args=parser.parse_args()
    if args.repeat<2:parser.error('--repeat must be at least two to check reset')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    source_paths=[ROOT/'src'/name for name in ('authoredpartsys.h','authoredpartsys.cpp',
                  'partsysdefinition.h','partsysdefinition.cpp')]
    before={str(p.relative_to(ROOT)):sha(p) for p in source_paths}
    driver=args.output.parent/(args.output.stem+'.port-driver')
    command=['clang++','-std=c++17','-iquote',str(ROOT/'src'),
             str(Path(__file__).with_name('effect_probe_driver.cpp')),
             str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),
             '-o',str(driver)]
    subprocess.run(command,check=True,capture_output=True,text=True)
    start=time.perf_counter();probe=PartsysProbe(args.executable)
    load_ms=(time.perf_counter()-start)*1000
    timings=[];cases=[];errors=[];checks=0
    for render in (False,True):
        for shape in range(4):
            for face in (0,32,64,127,255):
                expected=None
                for repeat in range(args.repeat):
                    start=time.perf_counter();run=probe.replay(shape,face,render)
                    timings.append((time.perf_counter()-start)*1000)
                    content={k:v for k,v in run.items() if k!='restored_pages'}
                    if expected is None:expected=content
                    elif content!=expected:
                        raise AssertionError(f'Warm reset mismatch: {(shape,face,render,repeat)}')
                port_text=subprocess.check_output([str(driver),str(shape),str(face),
                        *(['render'] if render else [])],text=True)
                port_rows=[[float(v) for v in line.split()] for line in port_text.splitlines()]
                if len(port_rows)!=30:raise AssertionError('Port driver did not emit 30 ticks')
                for tick,(retail,port) in enumerate(zip(expected['rows'],port_rows)):
                    if len(retail)!=len(port):raise AssertionError('Trace schema mismatch')
                    for index,(a,b) in enumerate(zip(retail,port)):
                        checks+=1
                        tolerance=0 if index<4 else 2e-5
                        if abs(a-b)>tolerance:
                            errors.append(dict(shape=shape,face=face,render=render,tick=tick,
                                field=FIELDS[index],retail=a,port=b,absolute_error=abs(a-b)))
                cases.append(dict(shape=shape,face=face,render=render,**expected))
    after={str(p.relative_to(ROOT)):sha(p) for p in source_paths}
    if after!=before:raise AssertionError('Production source changed during the A/B run')
    report=dict(status='pass' if not errors else 'fail',retail_sha256=RETAIL_SHA,
        fixture='authored partsys bounded state and pose setup',case_count=len(cases),
        ticks_per_case=30,warm_replays=len(timings),replays_per_case=args.repeat,
        checks=checks,error_count=len(errors),errors=errors[:100],
        pe_load_ms=load_ms,median_warm_30_tick_ms=statistics.median(timings),
        p95_warm_30_tick_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],
        original_functions=sorted({address for case in cases for address in case['original_function_calls']}),
        render_stop='0x4028e2',source_sha256=after,driver_sha256=sha(driver),
        driver_source_sha256=sha(Path(__file__).with_name('effect_probe_driver.cpp')),
        probe_source_sha256=sha(Path(__file__)),
        compile_command=command,integer_and_rng_exact=True,float_absolute_tolerance=2e-5,
        fields=list(FIELDS),cases=cases,guest_os_boots=0,retail_processes=0,
        full_pool_replay_checked=True,full_pool_port_field_comparison=False,
        pixel_comparison=False,ledger_acceptance_changed=False,
        scope='Original CPU state and render-pose/color setup versus compiled production state module. '
              'Synthetic emitter/owner, explicit identity bone/prototype transforms, ground zero and '
              'matched MSVC RNG; device submission is not executed. No retail pixels or map acceptance.')
    args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k not in ('cases','errors')},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
