#!/usr/bin/env python3
"""Execute real hook/C instructions with explicit Win32 I/O boundary stubs."""
import argparse
import json
from pathlib import Path
import struct
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import (UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ECX,
    UC_X86_REG_EDX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP,
    UC_X86_REG_ESP,UC_X86_REG_EFLAGS)
from reconstruct import pe_layout


REGS=[UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ECX,UC_X86_REG_EDX,
      UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP,UC_X86_REG_ESP,UC_X86_REG_EFLAGS]


def machine(data):
    layout=pe_layout(data);base=layout['image_base'];opt=layout['optional_offset']
    image_size,headers=struct.unpack_from('<II',data,opt+56)
    uc=Uc(UC_ARCH_X86,UC_MODE_32);uc.mem_map(base,(image_size+4095)//4096*4096)
    uc.mem_write(base,data[:headers])
    for s in layout['sections']:
        if s['size']:uc.mem_write(base+s['rva'],data[s['offset']:s['offset']+s['size']])
    uc.mem_map(0x10000000,0x20000);uc.mem_map(0x0f000000,0x1000)
    uc.mem_write(0x0f000f00,b'\xcc')  # Bounded sentinel block; stop before executing it.
    for i,r in enumerate(REGS[:-2]):uc.reg_write(r,0x11223300+i)
    uc.reg_write(UC_X86_REG_ESP,0x10010000);uc.reg_write(UC_X86_REG_EFLAGS,0x246)
    uc.mem_write(0x10010000,struct.pack('<IIIII',0x0f000f00,1,2,3,4))
    return uc


def verify(root,baseline):
    m=json.loads((root/'manifest.json').read_text());original=baseline.read_bytes()
    data=(root/'Revenant.hooked.exe').read_bytes();uc=machine(data);api={};events=[];messages=[]
    sizes={'CreateFileA':7,'SetFilePointer':4,'WriteFile':5,'CloseHandle':1}
    for i,(name,address) in enumerate(m['iat'].items()):
        stub=0x0f000000+i*16;api[stub]=name
        uc.mem_write(stub,b'\xc2'+struct.pack('<H',sizes[name]*4))
        uc.mem_write(address,struct.pack('<I',stub))
    target=m['target_va'];endpoint=target+m['stolen_size'] if m['mode']=='trace' else 0x0f000f00
    reached=[]
    def read_string(pointer):
        result=bytearray()
        for i in range(256):
            b=bytes(uc.mem_read(pointer+i,1))
            if b==b'\0':return result.decode('ascii')
            result+=b
        raise ValueError('Unterminated logger filename')
    def callback(u,address,size,user):
        if address==endpoint:
            reached.append(address);u.emu_stop();return
        if address not in api:return
        name=api[address];sp=u.reg_read(UC_X86_REG_ESP)
        args=struct.unpack('<'+'I'*sizes[name],u.mem_read(sp+4,4*sizes[name]));events.append(name)
        if name=='CreateFileA':
            assert read_string(args[0])=='retail-asm.log';assert args[1:]==(0x40000000,3,0,4,0x80,0)
            u.reg_write(UC_X86_REG_EAX,0x77)
        elif name=='SetFilePointer':
            assert args==(0x77,0,0,2);u.reg_write(UC_X86_REG_EAX,0)
        elif name=='WriteFile':
            assert args[0]==0x77 and args[4]==0
            messages.append(bytes(u.mem_read(args[1],args[2])).decode('ascii'))
            u.mem_write(args[3],struct.pack('<I',args[2]));u.reg_write(UC_X86_REG_EAX,1)
        else:
            assert args==(0x77,);u.reg_write(UC_X86_REG_EAX,1)
    uc.hook_add(UC_HOOK_CODE,callback);uc.emu_start(target,0,count=1000)
    final={r:uc.reg_read(r) for r in REGS}
    if m['mode']=='trace':
        plain=machine(original);done=[]
        def stop(u,a,s,v):
            if a==endpoint:done.append(a);u.emu_stop()
        plain.hook_add(UC_HOOK_CODE,stop);plain.emu_start(target,0,count=32)
        equivalent=bool(done) and final=={r:plain.reg_read(r) for r in REGS}
    else:
        expected=machine(original)
        equivalent=all(final[r]==expected.reg_read(r) for r in REGS if r not in (UC_X86_REG_EAX,UC_X86_REG_ESP))
        equivalent=equivalent and final[UC_X86_REG_EAX]==m['return_value'] and final[UC_X86_REG_ESP]==0x10010000+4+m['stack_cleanup']
    passed=bool(reached) and equivalent and messages==[m['log_message']]
    report=dict(status='pass' if passed else 'fail',mode=m['mode'],api_calls=events,logged_messages=messages,
        intended_control_transfer=bool(reached),abi_state_matches_expected=equivalent,
        scope='Added machine-code wrapper and compiled C executed. CreateFile/seek/write/close boundaries explicitly stubbed; actual C filename/message/arguments and stdcall cleanup checked. Trace compared against execution of original stolen prologue. Remaining original game body and guest I/O not executed.')
    (root/'execution-verification.json').write_text(json.dumps(report,indent=2)+'\n')
    if not passed:raise SystemExit('Hook verification failed')
    print(f'PASS: {m["mode"]} hook, C logging call sequence and ABI/control transfer.')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('directory',type=Path);p.add_argument('--baseline',type=Path,required=True)
    a=p.parse_args();verify(a.directory.resolve(),a.baseline.resolve())
