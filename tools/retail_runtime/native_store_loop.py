"""Instruction-equivalent acceleration of seven pinned x86 copy/fill loops.

The decoder, stream, geometry and pixels remain original. Bulk memory writes
preserve the loop's exact memory/register/flag results and checkpoint tracking.
"""
import struct
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_EDI,UC_X86_REG_ESI,UC_X86_REG_EBP,UC_X86_REG_EIP,UC_X86_REG_EFLAGS

START,END=0x41d158,0x41d162
OPCODES=bytes.fromhex('890783c704ff4df875f6')
COPY_START,COPY_END=0x41d19f,0x41d1ad
COPY_OPCODES=bytes.fromhex('8a064666890783c702ff4df875f2')
ARITHMETIC_FLAGS=0x8d5
REGISTER_LOOPS=(
    (0x41ced0,0x41cedc,'c7070000000083c7044975f4',UC_X86_REG_ECX,4,'zero'),
    (0x41cede,0x41ceea,'c707ffffffff83c7044975f4',UC_X86_REG_ECX,4,'ones'),
    (0x41cf4b,0x41cf53,'890783c7044a75f8',UC_X86_REG_EDX,4,'fill'),
    (0x41cf8a,0x41cf93,'8a06468807474a75f7',UC_X86_REG_EDX,1,'copy'),
    (0x41cfa3,0x41cfb0,'8b0683c604890783c7044a75f3',UC_X86_REG_EDX,4,'copy'),
)

def install(vm):
    if bytes(vm.uc.mem_read(START,len(OPCODES)))!=OPCODES:
        raise ValueError('Original repeated-store opcodes differ')
    if bytes(vm.uc.mem_read(COPY_START,len(COPY_OPCODES)))!=COPY_OPCODES:
        raise ValueError('Original word-copy opcodes differ')
    # Validate the whole set before adding any hooks. A modified named retail
    # build must fail cleanly, rather than retain a partially installed set.
    for start,end,hexcode,_,_,_ in REGISTER_LOOPS:
        if bytes(vm.uc.mem_read(start,end-start))!=bytes.fromhex(hexcode):
            raise ValueError('Original register-store opcodes differ')
    stats={'loops':0,'words':0,'copy_loops':0,'copied_words':0}
    def execute(uc,address,size,user):
        frame=uc.reg_read(UC_X86_REG_EBP);count=vm.u32(frame-8)
        # Zero would underflow in the original loop: do not reinterpret it.
        if not 1<=count<=262144:return
        destination=uc.reg_read(UC_X86_REG_EDI);end=destination+count*4
        if end>0x100000000:return
        # Runtime's bump heap is permanently mapped writable; allocations
        # there cannot be VirtualFree'd. Avoid querying every region per run.
        covered=end if vm.HEAP<=destination and end<=vm.heap_next else destination
        if covered!=end:
            for lo,hi,permissions in sorted(uc.mem_regions()):
                if hi<covered:continue
                if lo>covered or not permissions&2:return
                covered=min(end,hi+1)
                if covered==end:break
        if covered!=end:return
        value=uc.reg_read(UC_X86_REG_EAX)
        vm.write(destination,struct.pack('<I',value)*count)
        vm.put_u32(frame-8,0)
        uc.reg_write(UC_X86_REG_EDI,end&0xffffffff)
        # Last DEC is1->0: ZF/PF set, SF/OF/AF clear. CF is the last ADD's.
        flags=(uc.reg_read(UC_X86_REG_EFLAGS)&~ARITHMETIC_FLAGS)|0x44|int(end>0xffffffff)
        uc.reg_write(UC_X86_REG_EFLAGS,flags);uc.reg_write(UC_X86_REG_EIP,END)
        stats['loops']+=1;stats['words']+=count
    stats['hook']=vm.uc.hook_add(UC_HOOK_CODE,execute,begin=START,end=START)
    def copy(uc,address,size,user):
        frame=uc.reg_read(UC_X86_REG_EBP);count=vm.u32(frame-8)
        if not 1<=count<=262144:return
        source=uc.reg_read(UC_X86_REG_ESI);destination=uc.reg_read(UC_X86_REG_EDI);end=destination+2*count
        # In this decoder references are to the compressed stream. If a
        # different caller overlaps its buffers, leave the original loop active.
        if source<end and destination<source+count:return
        if not (vm.HEAP<=source and source+count<=vm.heap_next and vm.HEAP<=destination and end<=vm.heap_next):return
        data=bytes(uc.mem_read(source,count));value=uc.reg_read(UC_X86_REG_EAX)
        words=bytearray(count*2);words[::2]=data;words[1::2]=bytes([(value>>8)&255])*count
        vm.write(destination,bytes(words));vm.put_u32(frame-8,0)
        uc.reg_write(UC_X86_REG_ESI,(source+count)&0xffffffff);uc.reg_write(UC_X86_REG_EDI,end&0xffffffff)
        uc.reg_write(UC_X86_REG_EAX,(value&0xffffff00)|data[-1])
        flags=(uc.reg_read(UC_X86_REG_EFLAGS)&~ARITHMETIC_FLAGS)|0x44|int(end>0xffffffff)
        uc.reg_write(UC_X86_REG_EFLAGS,flags);uc.reg_write(UC_X86_REG_EIP,COPY_END)
        stats['copy_loops']+=1;stats['copied_words']+=count
    stats['copy_hook']=vm.uc.hook_add(UC_HOOK_CODE,copy,begin=COPY_START,end=COPY_START)
    stats['register_loops']=0;stats['register_bytes']=0
    def register_handler(start,end,register,width,kind):
        def run(uc,address,size,user):
            count=uc.reg_read(register);destination=uc.reg_read(UC_X86_REG_EDI);finish=destination+count*width
            if not 1<=count<=262144 or not vm.HEAP<=destination or finish>vm.heap_next:return
            value=uc.reg_read(UC_X86_REG_EAX);source=uc.reg_read(UC_X86_REG_ESI)
            if kind=='copy':
                source_end=source+count*width
                if not vm.HEAP<=source or source_end>vm.heap_next or (source<finish and destination<source_end):return
                data=bytes(uc.mem_read(source,count*width));uc.reg_write(UC_X86_REG_ESI,source_end&0xffffffff)
                value=(value&0xffffff00)|data[-1]if width==1 else struct.unpack('<I',data[-4:])[0]
                uc.reg_write(UC_X86_REG_EAX,value)
            else:data=struct.pack('<I',0 if kind=='zero'else 0xffffffff if kind=='ones'else value)*count
            flags=uc.reg_read(UC_X86_REG_EFLAGS)
            carry=(flags&1)if width==1 else int(finish>0xffffffff)
            vm.write(destination,data);uc.reg_write(UC_X86_REG_EDI,finish&0xffffffff);uc.reg_write(register,0)
            uc.reg_write(UC_X86_REG_EFLAGS,(flags&~ARITHMETIC_FLAGS)|0x44|carry);uc.reg_write(UC_X86_REG_EIP,end)
            stats['register_loops']+=1;stats['register_bytes']+=len(data)
        return run
    for start,end,hexcode,register,width,kind in REGISTER_LOOPS:
        stats[f'{start:x}_hook']=vm.uc.hook_add(UC_HOOK_CODE,register_handler(start,end,register,width,kind),begin=start,end=start)
    return stats
