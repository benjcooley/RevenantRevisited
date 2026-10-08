#!/usr/bin/env python3
"""Experimental fixed-layout function wrapper/replacement using original Win32 IAT."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import struct
import subprocess
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_GRP_BRANCH_RELATIVE, CS_GRP_RET
from reconstruct import pe_layout
from add_c_probe import coff_text, aligned


def file_offset(layout, va):
    for section in layout['sections']:
        delta=va-layout['image_base']-section['rva']
        if 0<=delta<section['size']:
            return section['offset']+delta
    raise ValueError(f'Address 0x{va:x} has no raw section')


def imports(data, layout):
    base=layout['image_base']
    rva,size=struct.unpack_from('<II',data,layout['optional_offset']+104)
    p=file_offset(layout,base+rva);result={}
    def string(rva):
        start=file_offset(layout,base+rva)
        return data[start:data.index(0,start)].decode('ascii')
    while any(data[p:p+20]):
        lookup,_,_,name,iat=struct.unpack_from('<IIIII',data,p)
        dll=string(name);cursor=file_offset(layout,base+(lookup or iat));i=0
        while True:
            value=struct.unpack_from('<I',data,cursor+4*i)[0]
            if not value:break
            if not value&0x80000000:
                result[(dll.lower(),string(value+2))]=base+iat+4*i
            i+=1
        p+=20
    return result


def stack_string(name,value):
    raw=value.encode('ascii')+b'\0';raw+=bytes((-len(raw))%4)
    lines=[f'    unsigned int {name}[{len(raw)//4}];',f'    volatile unsigned int *{name}_words = {name};']
    for i in range(0,len(raw),4):
        lines.append(f'    {name}_words[{i//4}] = 0x{int.from_bytes(raw[i:i+4],"little"):08x}u;')
    return '\n'.join(lines)


def hook(baseline, output, target, mode, cleanup, return_value, clang, nasm):
    data=baseline.read_bytes();layout=pe_layout(data);base=layout['image_base']
    image_chars=struct.unpack_from('<H',data,layout['pe_offset']+22)[0]
    if not image_chars&1:
        raise ValueError('This direct-IAT example requires the fixed-base, relocation-stripped retail image')
    entries=imports(data,layout)
    iat={n:entries[('kernel32.dll',n)] for n in ['CreateFileA','SetFilePointer','WriteFile','CloseHandle']}
    cs=Cs(CS_ARCH_X86,CS_MODE_32);cs.detail=True
    offset=file_offset(layout,target);stolen=[];length=0
    for insn in cs.disasm(data[offset:offset+32],target):
        if insn.group(CS_GRP_BRANCH_RELATIVE) or insn.group(CS_GRP_RET):
            raise ValueError('Prologue contains PC-relative control flow/return; requires a relocating trampoline')
        stolen.append(f'{insn.mnemonic} {insn.op_str}');length+=insn.size
        if length>=5:break
    if length<5:raise ValueError('Cannot recover a complete five-byte prologue')
    candidates=[]
    for s in layout['sections']:
        local=aligned(s['virtual_size'],16)
        if s['size']-local>=512 and not any(data[s['offset']+local:s['offset']+s['size']]):
            candidates.append((s,local))
    if not candidates:raise ValueError('Insufficient verified-zero section tail for small logger')
    section,local=candidates[-1];new_va=base+section['rva']+local;new_offset=section['offset']+local
    output.mkdir(parents=True,exist_ok=False)
    message=f'{mode} hook 0x{target:08x}\r\n'
    c='''typedef unsigned int U32;
typedef U32 (__stdcall *Create)(const char*,U32,U32,void*,U32,U32,U32);
typedef U32 (__stdcall *Seek)(U32,int,void*,U32);
typedef int (__stdcall *Write)(U32,const void*,U32,U32*,void*);
typedef int (__stdcall *Close)(U32);
__declspec(noinline) void retail_hook_log(void) {
'''
    c+=stack_string('path','retail-asm.log')+'\n'+stack_string('message',message)+'\n'
    c+=f'''    U32 written=0;
    U32 h=((Create)*(volatile U32*)0x{iat['CreateFileA']:08x}u)((const char*)path,0x40000000u,3,0,4,0x80,0);
    if(h==0xffffffffu)return;
    ((Seek)*(volatile U32*)0x{iat['SetFilePointer']:08x}u)(h,0,0,2);
    ((Write)*(volatile U32*)0x{iat['WriteFile']:08x}u)(h,(const char*)message,{len(message)},&written,0);
    ((Close)*(volatile U32*)0x{iat['CloseHandle']:08x}u)(h);
}}
'''
    source=output/'hook.c';source.write_text(c);obj=output/'hook.obj'
    command=[clang,'--target=i686-pc-windows-msvc','-march=i586','-mno-sse','-mno-sse2',
        '-mno-mmx','-Os','-ffreestanding','-fno-builtin','-fno-stack-protector','-fomit-frame-pointer',
        '-fno-unwind-tables','-fno-asynchronous-unwind-tables','-c',str(source),'-o',str(obj)]
    subprocess.run(command,check=True);code=coff_text(obj);(output/'hook.text.bin').write_bytes(code)
    suffix=('    db '+','.join(f'0x{x:02x}' for x in data[offset:offset+length])+f'\n    jmp 0x{target+length:x}\n') if mode=='trace' else f'    mov eax,0x{return_value&0xffffffff:x}\n    ret {cleanup}\n'
    asm=f'''BITS 32
ORG 0x{new_va:x}
function_wrapper:
    pushfd
    pushad
    call c_logger
    popad
    popfd
{suffix}
    align 16, db 0x90
c_logger:
    incbin "hook.text.bin"
'''
    (output/'hook.asm').write_text(asm)
    subprocess.run([nasm,'-f','bin','-Ox','hook.asm','-o','hook.bin'],cwd=output,check=True)
    added=(output/'hook.bin').read_bytes()
    if local+len(added)>section['size']:raise ValueError('Logger exceeds available zero padding')
    result=bytearray(data);result[new_offset:new_offset+len(added)]=added
    result[offset:offset+length]=b'\xe9'+struct.pack('<i',new_va-(target+5))+b'\x90'*(length-5)
    sh=layout['optional_offset']+layout['optional_size']+40*section['index']
    struct.pack_into('<I',result,sh+8,local+len(added))
    flags=struct.unpack_from('<I',result,sh+36)[0]
    struct.pack_into('<I',result,sh+36,flags|0xe0000000)
    struct.pack_into('<I',result,layout['optional_offset']+64,0)
    allowed=[(offset,offset+length),(new_offset,new_offset+len(added)),(sh+8,sh+12),(sh+36,sh+40),
             (layout['optional_offset']+64,layout['optional_offset']+68)]
    changed=[i for i,(a,b) in enumerate(zip(data,result)) if a!=b]
    if any(not any(a<=i<b for a,b in allowed) for i in changed):raise AssertionError('Unexpected image modification')
    exe=output/'Revenant.hooked.exe';exe.write_bytes(result)
    report=dict(status='built_not_guest_run',mode=mode,target_va=target,target_file_offset=offset,
        stolen_size=length,stolen_instructions=stolen,stolen_bytes=data[offset:offset+length].hex(),
        wrapper_va=new_va,wrapper_file_offset=new_offset,wrapper_size=len(added),iat=iat,
        log_file='retail-asm.log',log_message=message,compiler_command=command,
        stack_cleanup=cleanup if mode=='return' else None,return_value=return_value if mode=='return' else None,
        baseline_sha256=hashlib.sha256(data).hexdigest(),variant_sha256=hashlib.sha256(result).hexdigest(),
        allowed_modified_spans=allowed,actual_changed_byte_count=len(changed),
        scope='Experimental function-entry detour. Trace restores entry registers/flags then executes complete stolen instructions and rejoins original function. Return logs and bypasses body with explicit ABI cleanup. No original game behavior/Win98 startup validation yet.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'{mode} logging hook created at 0x{target:08x}: {exe}')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('baseline',type=Path);p.add_argument('--output',type=Path,required=True)
    p.add_argument('--target',type=lambda x:int(x,0),default=0x4865a0);p.add_argument('--mode',choices=['trace','return'],default='trace')
    p.add_argument('--stack-cleanup',type=int,default=0);p.add_argument('--return-value',type=lambda x:int(x,0),default=0)
    p.add_argument('--clang',default=shutil.which('clang'));p.add_argument('--nasm',default=shutil.which('nasm'))
    a=p.parse_args()
    if not a.clang or not a.nasm:p.error('clang and NASM are required')
    if a.stack_cleanup<0 or a.stack_cleanup>65535:p.error('Stack cleanup must fit RET imm16')
    hook(a.baseline.resolve(),a.output.resolve(),a.target,a.mode,a.stack_cleanup,a.return_value,a.clang,a.nasm)
