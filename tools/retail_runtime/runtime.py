"""In-process retail PE32 execution. No native window/device/guest OS required.

Unknown APIs stop execution: returning invented success would invalidate an oracle.
"""
from __future__ import annotations
import copy
from contextlib import contextmanager
from dataclasses import dataclass
from fractions import Fraction
from pathlib import Path
import struct
import sys

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP, UC_X86_REG_EIP
from threads import Scheduler,Yield
from memory import Heaps,VirtualMemory
import dirtypages
from process import Process

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'retail_asm'))
from reconstruct import pe_layout
from function_hook import file_offset


class MissingAPI(RuntimeError):
    pass


@dataclass
class GuestCall:
    address: int
    args: tuple


def import_slots(image,layout):
    """Keep every IAT slot, including ordinal imports and duplicate names."""
    base=layout['image_base']
    rva,size=struct.unpack_from('<II',image,layout['optional_offset']+104)
    if not rva:return
    descriptor=file_offset(layout,base+rva)
    def string(rva):
        start=file_offset(layout,base+rva)
        return image[start:image.index(0,start)].decode('ascii')
    while any(image[descriptor:descriptor+20]):
        lookup,_,_,name,iat=struct.unpack_from('<IIIII',image,descriptor)
        dll=string(name).lower();cursor=file_offset(layout,base+(lookup or iat));i=0
        while True:
            value=struct.unpack_from('<I',image,cursor+i*4)[0]
            if not value:break
            symbol=f'#{value&0xffff}' if value&0x80000000 else string(value+2)
            yield dll,symbol,base+iat+i*4
            i+=1
        descriptor+=20


@dataclass
class Surface:
    address: int
    width: int
    height: int
    stride: int
    format: str = 'RGB565'


class Runtime:
    STACK=0x0e000000
    STACK_SIZE=0x200000
    HEAP=0x10000000
    HEAP_SIZE=0x4000000
    STUBS=0x70000000
    STUB_SIZE=0x100000
    RETURN=0x700ff000

    def __init__(self, executable):
        self.image=Path(executable).read_bytes()
        self.layout=pe_layout(self.image)
        if struct.unpack_from('<H',self.image,self.layout['pe_offset']+4)[0]!=0x14c:
            raise ValueError('Only i386 PE32 images are supported')
        self.base=self.layout['image_base']
        # Host memory writes do not reliably invalidate Unicorn's translated
        # blocks. PE code is known; allocated instruction stubs opt in via
        # write_code so ordinary data/raster writes avoid cache work.
        self.code_ranges=[(self.base+s['rva'],self.base+s['rva']+max(s['virtual_size'],s['size']))
                          for s in self.layout['sections'] if s['executable']]
        opt=self.layout['optional_offset']
        image_size,headers=struct.unpack_from('<II',self.image,opt+56)
        self.uc=Uc(UC_ARCH_X86,UC_MODE_32)
        self.uc.mem_map(self.base,(image_size+4095)//4096*4096)
        self.uc.mem_write(self.base,self.image[:headers])
        for section in self.layout['sections']:
            if section['size']:
                self.uc.mem_write(self.base+section['rva'],self.image[section['offset']:section['offset']+section['size']])
        self.uc.mem_map(self.STACK,self.STACK_SIZE)
        self.uc.mem_map(self.HEAP,self.HEAP_SIZE)
        self.uc.mem_map(self.STUBS,self.STUB_SIZE)
        self.uc.mem_write(self.RETURN,b'\xcc')
        self.heap_next=self.HEAP
        self.ticks=0
        self.tick_rate=24
        self.sleep_ms=0
        self.rng_seed=1
        self.files={}
        self.handles={}
        self.next_handle=256
        self.file_handle_limit=20
        self.surfaces={}
        self.messages=[]
        self.windows={}
        self.timers={}
        self.next_timer=1
        self.api_trace=[]
        self.stubs={}
        self.error=None
        self.reached_return=False
        self.snapshot=None
        self.dirty={}
        self._restoring=False
        self.handlers={
            'GetTickCount':(0,lambda a:self.milliseconds),
            'timeGetTime':(0,lambda a:self.milliseconds),
            'QueryPerformanceCounter':(1,self._counter),
            'QueryPerformanceFrequency':(1,self._frequency),
            'GetCurrentProcessId':(0,lambda a:1),
            'GetCurrentThreadId':(0,lambda a:1),
            'GetVersion':(0,lambda a:0x87ce0a04),
            'Sleep':(1,self._sleep),
            'CreateFileA':(7,self._create),
            'SetFilePointer':(4,self._seek),
            'ReadFile':(5,self._read),
            'WriteFile':(5,self._write),
            'CloseHandle':(1,self._close),
            'GetFileSize':(2,self._file_size),
            'GetProcessHeap':(0,lambda a:self.HEAP),
            'HeapAlloc':(3,lambda a:self.allocate(a[2],bool(a[1]&8))),
            'PeekMessageA':(5,self._peek),
            'PostQuitMessage':(1,self._quit),
            'SetTimer':(4,self._set_timer),
            'KillTimer':(2,self._kill_timer),
            'DispatchMessageA':(1,self._dispatch),
        }
        self.user_apis={'PeekMessageA','PostQuitMessage','SetTimer','KillTimer','DispatchMessageA'}
        # Import name -> DLLs whose import of it a slot-registered handler
        # serves (e.g. a slot's ddraw.dll/gdi32.dll adapters). Empty: the
        # kernel32/user32/winmm defaults above apply unchanged.
        self.api_dlls={}
        # Where host calls start their stack. A fixture that keeps a guest
        # frame alive (a WinMain stopped mid-way) moves it below that frame.
        self.call_sp=self.STACK+self.STACK_SIZE-0x100
        for i,(dll,name,iat) in enumerate(import_slots(self.image,self.layout)):
            address=self.STUBS+i*16
            if address>=self.RETURN:raise ValueError('Import stubs overlap return sentinel')
            self.uc.mem_write(address,b'\xcc')
            self.uc.mem_write(iat,struct.pack('<I',address))
            self.stubs[address]=(dll,name)
        # Trap only the import/sentinel range. Dispatching every retail
        # instruction back through Python defeats the fast native CPU loop.
        self.uc.hook_add(UC_HOOK_CODE,self._code,begin=self.STUBS,end=self.STUBS+self.STUB_SIZE-1)
        # Pages written since the checkpoint, with their content before:
        # tracked in C (dirtypages.c) when it builds, else by a Python hook.
        self._native_dirty=dirtypages.DirtyPages(self.uc) if dirtypages.available() else None
        self._write_hooks=[self._add_write_hook()]
        self._bulk_active=False
        self.scheduler=Scheduler(self)
        self.heaps=Heaps(self)
        self.virtual_memory=VirtualMemory(self)
        self.process=Process(self)
        self.handlers.update({
            'GetStartupInfoA':(1,self.process.startup_info),
            'GetStdHandle':(1,self.process.std_handle),
            'GetFileType':(1,self.process.file_type),
            'GetModuleHandleA':(1,self.process.module),
            'GetCommandLineA':(0,lambda a:self.process.command_pointer),
            'GetEnvironmentStrings':(0,self.process.environment),
            'GetEnvironmentStringsA':(0,self.process.environment),
            'GetEnvironmentStringsW':(0,self.process.environment),
            'FreeEnvironmentStringsA':(1,self.process.free_environment),
            'FreeEnvironmentStringsW':(1,self.process.free_environment),
            'SetHandleCount':(1,self.process.handle_count),
            'WideCharToMultiByte':(8,self.process.wide_to_bytes),
            'GetACP':(0,lambda a:1252),
            'GetCPInfo':(2,self.process.cp_info),
            'VirtualAlloc':(4,self.virtual_memory.alloc),
            'VirtualFree':(3,self.virtual_memory.free),
            'HeapCreate':(3,self.heaps.create),
            'HeapAlloc':(3,self.heaps.alloc),
            'HeapFree':(3,self.heaps.free),
            'HeapReAlloc':(4,self.heaps.realloc),
            'HeapSize':(3,self.heaps.size),
            'HeapDestroy':(1,self.heaps.destroy),
            'TlsAlloc':(0,self.scheduler.tls_alloc),
            'TlsFree':(1,self.scheduler.tls_free),
            'TlsGetValue':(1,self.scheduler.tls_get),
            'TlsSetValue':(2,self.scheduler.tls_set),
            'GetLastError':(0,self.scheduler.get_error),
            'SetLastError':(1,self.scheduler.set_error),
            'GetCurrentThreadId':(0,lambda a:self.scheduler.current),
            'GetCurrentThread':(0,lambda a:0xfffffffe),
            'CreateThread':(6,self.scheduler.create_thread),
            'ResumeThread':(1,self.scheduler.resume_thread),
            'ExitThread':(1,self.scheduler.exit_thread),
            'SetThreadPriority':(2,self._thread_priority),
            'GetExitCodeThread':(2,self._thread_exit_code),
            'CreateEventA':(4,self.scheduler.create_event),
            'CreateMutexA':(3,self.scheduler.create_mutex),
            'ReleaseMutex':(1,self.scheduler.release_mutex),
            'SetEvent':(1,self.scheduler.set_event),
            'ResetEvent':(1,self.scheduler.reset_event),
            'WaitForSingleObject':(2,self.scheduler.wait_single),
            'WaitForMultipleObjects':(4,self.scheduler.wait_multiple),
            'InitializeCriticalSection':(1,self.scheduler.initialize_critical),
            'EnterCriticalSection':(1,self.scheduler.enter_critical),
            'LeaveCriticalSection':(1,self.scheduler.leave_critical),
            'DeleteCriticalSection':(1,self.scheduler.delete_critical),
        })

    @property
    def milliseconds(self):
        return int(self.elapsed_ms) & 0xffffffff

    @property
    def elapsed_ms(self):
        return Fraction(self.ticks*1000,self.tick_rate)+self.sleep_ms

    def advance(self,ticks=1):
        if ticks<0:raise ValueError('Negative clock advance')
        self.ticks+=ticks

    def api_address(self,dll,name):
        """Expose a virtual API to explicit probes/dynamic import adapters."""
        identity=(dll.lower(),name)
        for address,existing in self.stubs.items():
            if existing==identity:return address
        address=self.STUBS+len(self.stubs)*16
        if address>=self.RETURN:raise MemoryError('Virtual import stub capacity exhausted')
        self.write(address,b'\xcc');self.stubs[address]=identity;return address

    def random_msvc(self):
        # Used only when a scenario explicitly selects the MSVC CRT stream.
        # Retail's internal RNG functions may instead run as original code.
        self.rng_seed=(214013*self.rng_seed+2531011)&0xffffffff
        return (self.rng_seed>>16)&0x7fff

    def _add_write_hook(self,begin=1,end=0):
        if self._native_dirty is not None:return self._native_dirty.hook_add(begin,end)
        return self.uc.hook_add(UC_HOOK_MEM_WRITE,self._memory_write,begin=begin,end=end)

    def _del_write_hook(self,hook):
        if self._native_dirty is not None:self._native_dirty.hook_del(hook)
        else:self.uc.hook_del(hook)

    @property
    def dirty_pages(self):
        """Pages written since the checkpoint."""
        return len(self._native_dirty) if self._native_dirty is not None else len(self.dirty)

    def forget_page(self,page):
        """A page whose mapping went away: nothing to restore."""
        self.dirty.pop(page,None)
        if self._native_dirty is not None:self._native_dirty.forget(page)

    def _touch(self,address,size):
        if self.snapshot is None or self._restoring or not size:return
        if self._native_dirty is not None:
            self._native_dirty.touch(address,size);return
        first=address&~4095;last=(address+size-1)&~4095
        for page in range(first,last+1,4096):
            if page not in self.dirty:self.dirty[page]=bytes(self.uc.mem_read(page,4096))

    def _memory_write(self,uc,access,address,size,value,user):
        self._touch(address,size)

    @contextmanager
    def bulk_writes(self,ranges):
        """Snapshot declared output buffers once; avoid a callback per pixel.

        All other guest writes remain tracked. Capturing full baseline pages
        before entering makes reset correct even if rendering faults halfway.
        """
        if self._bulk_active:raise RuntimeError('Nested bulk-write scopes are not supported')
        spans=sorted((address,address+size) for address,size in ranges if size>0)
        merged=[]
        for start,end in spans:
            if start<0 or end>0x100000000:raise ValueError('Invalid guest buffer range')
            self._touch(start,end-start)
            if merged and start<=merged[-1][1]:merged[-1]=(merged[-1][0],max(end,merged[-1][1]))
            else:merged.append((start,end))
        if not merged:
            yield;return
        self._bulk_active=True
        for hook in self._write_hooks:self._del_write_hook(hook)
        self._write_hooks=[]
        try:
            cursor=0
            for start,end in merged:
                if cursor<start:self._write_hooks.append(self._add_write_hook(cursor,start-1))
                cursor=end
            if cursor<0x100000000:
                self._write_hooks.append(self._add_write_hook(cursor,0xffffffff))
            yield
        finally:
            for hook in self._write_hooks:self._del_write_hook(hook)
            self._write_hooks=[self._add_write_hook()]
            self._bulk_active=False

    def write(self,address,data):
        self._touch(address,len(data));self.uc.mem_write(address,data)
        self._invalidate_written_code(address,len(data))

    def _invalidate_written_code(self,address,size):
        end=address+size
        for start,stop in self.code_ranges:
            if address<stop and start<end:
                self.uc.ctl_remove_cache(max(address,start),min(end,stop))

    def write_code(self,address,data):
        """Write explicitly executable bytes, including heap-allocated stubs."""
        if data:
            interval=(address,address+len(data))
            if not any(start<=interval[0] and interval[1]<=stop for start,stop in self.code_ranges):
                self.code_ranges.append(interval)
        self.write(address,data)

    def u32(self,address):
        return struct.unpack('<I',self.uc.mem_read(address,4))[0]

    def put_u32(self,address,value):
        self.write(address,struct.pack('<I',value&0xffffffff))

    def string(self,address,limit=32768):
        result=bytearray()
        for i in range(limit):
            value=bytes(self.uc.mem_read(address+i,1))
            if value==b'\0':return result.decode('cp1252')
            result+=value
        raise ValueError('Unterminated emulated string')

    def allocate(self,size,zero=True):
        if size<0:raise ValueError('Negative allocation size')
        address=(self.heap_next+15)&~15;end=address+max(size,1)
        if end>self.HEAP+self.HEAP_SIZE:raise MemoryError('Virtual heap exhausted')
        self.heap_next=end
        if zero:self.write(address,bytes(size))
        return address

    def surface(self,name,width,height):
        if width<=0 or height<=0:raise ValueError('Surface dimensions must be positive')
        stride=(width*2+3)&~3
        s=Surface(self.allocate(stride*height),width,height,stride)
        self.surfaces[name]=s;return s

    def surface_bytes(self,name):
        s=self.surfaces[name];return bytes(self.uc.mem_read(s.address,s.stride*s.height))

    def checkpoint(self):
        self.snapshot=dict(cpu=self.uc.context_save(),heap_next=self.heap_next,ticks=self.ticks,
            sleep_ms=self.sleep_ms,messages=copy.deepcopy(self.messages),windows=copy.deepcopy(self.windows),
            timers=copy.deepcopy(self.timers),next_timer=self.next_timer,
            rng_seed=self.rng_seed,files=copy.deepcopy(self.files),handles=copy.deepcopy(self.handles),
            next_handle=self.next_handle,file_handle_limit=self.file_handle_limit,surfaces=copy.deepcopy(self.surfaces))
        self.snapshot['scheduler']=self.scheduler.snapshot()
        self.snapshot['heaps']=copy.deepcopy(dict(heaps=self.heaps.heaps,blocks=self.heaps.blocks))
        self.snapshot['virtual_memory']=self.virtual_memory.snapshot()
        self.dirty.clear()
        if self._native_dirty is not None:
            self._native_dirty.clear();self._native_dirty.set_active(True)

    def restore(self):
        if self.snapshot is None:raise RuntimeError('No checkpoint')
        self._restoring=True
        if self._native_dirty is not None:self._native_dirty.set_active(False)
        count=self.dirty_pages
        self.virtual_memory.restore_mappings(self.snapshot['virtual_memory'])
        pages=self._native_dirty.items() if self._native_dirty is not None else self.dirty.items()
        for address,data in pages:
            self.uc.mem_write(address,data)
            self._invalidate_written_code(address,len(data))
        self.uc.context_restore(self.snapshot['cpu'])
        self.scheduler.restore(self.snapshot['scheduler'])
        heaps=copy.deepcopy(self.snapshot['heaps']);self.heaps.heaps=heaps['heaps'];self.heaps.blocks=heaps['blocks']
        for name in ['heap_next','ticks','sleep_ms','messages','windows','timers','next_timer',
                     'rng_seed','files','handles','next_handle','file_handle_limit','surfaces']:
            setattr(self,name,copy.deepcopy(self.snapshot[name]))
        self.dirty.clear();self.api_trace.clear();self._restoring=False
        if self._native_dirty is not None:
            self._native_dirty.clear();self._native_dirty.set_active(True)
        return count

    def _code(self,uc,address,size,user):
        if address==self.RETURN:
            if self.scheduler.current!=1:raise RuntimeError('Worker reached host-call return sentinel')
            self.scheduler.threads[1].state='returned'
            self.reached_return=True;uc.emu_stop();return
        if address==self.scheduler.THREAD_RETURN:
            self.scheduler.exit_thread((uc.reg_read(UC_X86_REG_EAX),));uc.emu_stop();return
        if address not in self.stubs:return
        dll,name=self.stubs[address]
        supported_dlls=self.api_dlls.get(name) or ({'winmm.dll','_inmm.dll'} if name=='timeGetTime' else ({'user32.dll'} if name in self.user_apis else {'kernel32.dll'}))
        handler=self.handlers.get(name) if dll in supported_dlls else None
        if handler is None:
            self.error=MissingAPI(f'Unimplemented {dll}!{name} at 0x{address:08x}; caller 0x{self.u32(uc.reg_read(UC_X86_REG_ESP)):08x}')
            uc.emu_stop();return
        argc,implementation=handler
        sp=uc.reg_read(UC_X86_REG_ESP)
        args=struct.unpack('<'+'I'*argc,uc.mem_read(sp+4,argc*4)) if argc else ()
        try:result=implementation(args)
        except Exception as error:self.error=error;uc.emu_stop();return
        target=self.u32(sp)
        yielded=isinstance(result,Yield)
        if yielded and result.result is None:
            self.api_trace.append(dict(dll=dll,function=name,args=list(args),blocked=True,
                tid=self.scheduler.current,clock=self.milliseconds))
            uc.emu_stop();return
        if yielded:result=result.result
        if isinstance(result,GuestCall):
            # Delegate to the original stdcall WndProc/TIMERPROC. Its return
            # cleans its own arguments and resumes the API's original caller.
            frame=sp+4+4*argc-4*(len(result.args)+1)
            self.write(frame,struct.pack('<'+'I'*(len(result.args)+1),target,*result.args))
            self.api_trace.append(dict(dll=dll,function=name,args=list(args),delegated_to=result.address,clock=self.milliseconds))
            uc.reg_write(UC_X86_REG_ESP,frame);uc.reg_write(UC_X86_REG_EIP,result.address);return
        self.api_trace.append(dict(dll=dll,function=name,args=list(args),result=result,clock=self.milliseconds))
        uc.reg_write(UC_X86_REG_EAX,result&0xffffffff)
        uc.reg_write(UC_X86_REG_ESP,sp+4+4*argc)
        uc.reg_write(UC_X86_REG_EIP,target)
        if yielded:uc.emu_stop()

    def call(self,address,args=(),this=None,instruction_limit=1000000,stop_address=None):
        self.error=None;self.reached_return=False
        # Previous successful calls finish with the main thread context active.
        # Rejected/faulted scenarios are rolled back by the session wrapper.
        self.scheduler.current=1
        sp=self.call_sp
        self.write(sp,struct.pack('<'+'I'*(len(args)+1),self.RETURN,*args))
        self.uc.reg_write(UC_X86_REG_ESP,sp)
        if this is not None:self.uc.reg_write(UC_X86_REG_ECX,this)
        hook=None;stopped=[]
        if stop_address is not None:
            def stop(uc,a,s,u):
                if a==stop_address and self.scheduler.current==1:
                    stopped.append(a);self.scheduler.threads[1].state='stopped';uc.emu_stop()
            hook=self.uc.hook_add(UC_HOOK_CODE,stop,begin=stop_address,end=stop_address)
        try:result=self.scheduler.execute(address,instruction_limit)
        finally:
            if hook is not None:self.uc.hook_del(hook)
        if self.error:raise self.error
        if not self.reached_return and not stopped and self.scheduler.threads[1].state!='exited':
            raise RuntimeError('Instruction budget exhausted before expected boundary')
        return result

    def _counter(self,args):
        self.write(args[0],struct.pack('<Q',int(self.elapsed_ms*1000)));return 1

    def _sleep(self,args):
        return self.scheduler.sleep(args)

    def _thread_priority(self,args):
        self.scheduler.thread_handle(args[0]).priority=struct.unpack('<i',struct.pack('<I',args[1]))[0]
        return 1  # Recorded priority; deterministic round-robin policy remains.

    def _thread_exit_code(self,args):
        self.put_u32(args[1],self.scheduler.thread_handle(args[0]).exit_code);return 1

    def post_message(self,hwnd,message,wparam=0,lparam=0,x=0,y=0):
        self.messages.append((hwnd,message,wparam&0xffffffff,lparam&0xffffffff,self.milliseconds,x,y))

    def _quit(self,args):
        self.post_message(0,0x12,args[0]);return 0

    @staticmethod
    def _matches(message,hwnd,minimum,maximum):
        window,kind=message[:2]
        if hwnd==0xffffffff and window:return False
        if hwnd not in (0,0xffffffff) and window!=hwnd:return False
        return kind==0x12 or (minimum==maximum==0) or minimum<=kind<=maximum

    def _peek(self,args):
        output,hwnd,minimum,maximum,flags=args
        if flags&~3:raise MissingAPI('PeekMessage queue-status flags are not implemented')
        minimum&=0xffff;maximum&=0xffff
        for index,message in enumerate(self.messages):
            if self._matches(message,hwnd,minimum,maximum):break
        else:
            # WM_TIMER is generated lazily after posted messages, with one
            # notification per overdue timer rather than a burst for missed ticks.
            for (window,identifier),timer in self.timers.items():
                message=(window,0x113,identifier,timer['callback'],self.milliseconds,0,0)
                if timer['due']<=self.elapsed_ms and self._matches(message,hwnd,minimum,maximum):
                    timer['due']=self.elapsed_ms+timer['period'];self.messages.append(message)
                    index=len(self.messages)-1;break
            else:return 0
        self.write(output,struct.pack('<5I2i',*message))
        if flags&1:self.messages.pop(index)
        return 1

    def _set_timer(self,args):
        window,identifier,period,callback=args
        if window and window not in self.windows:raise MissingAPI('SetTimer requires a registered virtual window')
        if not window:
            identifier=self.next_timer;self.next_timer+=1
        period=min(max(period,10),0x7fffffff)
        self.timers[window,identifier]=dict(period=period,due=self.elapsed_ms+period,callback=callback)
        return identifier

    def _kill_timer(self,args):
        return int(self.timers.pop(tuple(args),None) is not None)

    def _dispatch(self,args):
        hwnd,message,wparam,lparam,when,x,y=struct.unpack('<5I2i',self.uc.mem_read(args[0],28))
        if message==0x113 and lparam:return GuestCall(lparam,(hwnd,message,wparam,self.milliseconds))
        if not hwnd:return 0
        if hwnd not in self.windows:raise MissingAPI('DispatchMessageA needs a registered original WndProc')
        return GuestCall(self.windows[hwnd],(hwnd,message,wparam,lparam))

    def _frequency(self,args):
        self.write(args[0],struct.pack('<Q',1000000));return 1

    def _create(self,args):
        if len(self.handles)>=self.file_handle_limit:
            self.scheduler.set_error((4,));return 0xffffffff
        if args[3] or args[5]&0x40000000 or args[6]:
            raise MissingAPI('CreateFileA security/overlapped/template semantics are not implemented')
        name=self.string(args[0]).replace('\\','/').lower()
        disposition=args[4]
        if disposition not in (1,2,3,4,5):raise ValueError('Invalid creation disposition')
        exists=name in self.files
        if disposition in (3,5) and not exists:return 0xffffffff
        if disposition==1 and exists:return 0xffffffff
        if disposition in (1,2,5) or not exists:self.files[name]=bytearray()
        handle=self.next_handle;self.next_handle+=1;self.handles[handle]=[name,0];return handle

    def _seek(self,args):
        if args[2]:raise MissingAPI('64-bit SetFilePointer is not implemented')
        name,pos=self.handles[args[0]]
        delta=struct.unpack('<i',struct.pack('<I',args[1]))[0]
        origin=(0,pos,len(self.files[name]))[args[3]]
        new=origin+delta
        # Before the start: fails with ERROR_NEGATIVE_SEEK, which the CRT's
        # text-mode append open expects on an empty file.
        if new<0:self.scheduler.set_error((131,));return 0xffffffff
        self.handles[args[0]][1]=new;return new

    def _read(self,args):
        if args[4]:raise MissingAPI('Overlapped ReadFile is not implemented')
        name,pos=self.handles[args[0]];data=self.files[name][pos:pos+args[2]]
        self.write(args[1],bytes(data));self.put_u32(args[3],len(data));self.handles[args[0]][1]+=len(data);return 1

    def _write(self,args):
        if args[4]:raise MissingAPI('Overlapped WriteFile is not implemented')
        name,pos=self.handles[args[0]];data=bytes(self.uc.mem_read(args[1],args[2]));f=self.files[name]
        if len(f)<pos+len(data):f.extend(bytes(pos+len(data)-len(f)))
        f[pos:pos+len(data)]=data;self.handles[args[0]][1]+=len(data);self.put_u32(args[3],len(data));return 1

    def _close(self,args):
        if args[0] in self.scheduler.objects:
            return self.scheduler.close_handle(args[0])
        return int(self.handles.pop(args[0],None) is not None)

    def _file_size(self,args):
        if args[1]:self.put_u32(args[1],0)
        return len(self.files[self.handles[args[0]][0]])
