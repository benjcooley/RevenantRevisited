"""Deterministic guest threads. No host threads or wall-clock waits.

Scheduling is a selected test policy, not a reproduction of Windows priorities.
The CPU runs bounded instruction slices and switches at waits/yields.
"""
import copy
from dataclasses import dataclass,replace,field
from fractions import Fraction
import struct
from unicorn.x86_const import (UC_X86_REG_GDTR,UC_X86_REG_CS,UC_X86_REG_DS,
    UC_X86_REG_ES,UC_X86_REG_SS,UC_X86_REG_FS,UC_X86_REG_EIP,UC_X86_REG_ESP,
    UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ECX,UC_X86_REG_EDX,
    UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP,UC_X86_REG_EFLAGS,UC_X86_REG_FPCW)


class GuestDeadlock(RuntimeError):
    pass


@dataclass
class Yield:
    # None keeps the API call on the stack for retry after waking.
    result: int | None = 0


@dataclass
class Thread:
    tid: int
    context: object
    stack_low: int
    stack_high: int
    state: str = 'ready'
    wake: object = None
    wait: object = None
    exit_code: int = 259
    priority: int = 0
    suspend_count: int = 0
    tls: dict = field(default_factory=dict)


def descriptor(base,limit,access,flags):
    return struct.pack('<Q',(limit&0xffff)|((base&0xffffff)<<16)|(access<<40)|
        (((limit>>16)&15)<<48)|(flags<<52)|(((base>>24)&255)<<56))


class Scheduler:
    GDT=0x7f000000
    TIB=0x7f100000
    MAX_THREADS=120
    THREAD_RETURN=0x700ff010
    QUANTUM=10000

    def __init__(self,vm):
        self.vm=vm;self.current=1;self.next_tid=2;self.running=False
        self.ready=[];self.objects={};self.critical={};self.named_events={}
        self.trace=[];self.threads={}
        self.tls_slots=set()
        vm.uc.mem_map(self.GDT,4096);vm.uc.mem_map(self.TIB,self.MAX_THREADS*4096)
        vm.uc.mem_write(self.THREAD_RETURN,b'\xcc')
        vm.write(self.GDT+8,descriptor(0,0xfffff,0x9b,12))
        vm.write(self.GDT+16,descriptor(0,0xfffff,0x93,12))
        vm.uc.reg_write(UC_X86_REG_GDTR,(0,self.GDT,4095,0))
        for reg in (UC_X86_REG_DS,UC_X86_REG_ES,UC_X86_REG_SS):vm.uc.reg_write(reg,16)
        vm.uc.reg_write(UC_X86_REG_CS,8)
        self.environment(1,vm.STACK,vm.STACK+vm.STACK_SIZE)
        vm.uc.reg_write(UC_X86_REG_FS,24)
        # Hardware x87 reset precision. CRT/game code may change it explicitly.
        vm.uc.reg_write(UC_X86_REG_FPCW,0x37f)
        self.threads[1]=Thread(1,vm.uc.context_save(),vm.STACK,vm.STACK+vm.STACK_SIZE,'idle')

    def environment(self,tid,low,high):
        tib=self.TIB+(tid-1)*4096;selector=(tid+2)*8
        self.vm.write(self.GDT+selector,descriptor(tib,4095,0x93,4))
        self.vm.write(tib,struct.pack('<7I',0xffffffff,high,low,0,0,0,tib))
        self.vm.put_u32(tib+0x20,1);self.vm.put_u32(tib+0x24,tid)
        return selector

    def snapshot(self):
        # Unicorn contexts are immutable saved objects, not deepcopy/pickleable.
        threads={tid:replace(t,wait=copy.deepcopy(t.wait),tls=dict(t.tls)) for tid,t in self.threads.items()}
        shared=copy.deepcopy(dict(objects=self.objects,critical=self.critical,named_events=self.named_events))
        return dict(threads=threads,current=self.current,next_tid=self.next_tid,ready=list(self.ready),tls_slots=set(self.tls_slots),**shared)

    def restore(self,state):
        self.threads={tid:replace(t,wait=copy.deepcopy(t.wait),tls=dict(t.tls)) for tid,t in state['threads'].items()}
        shared=copy.deepcopy({name:state[name] for name in ('current','next_tid','ready','objects','critical','named_events')})
        for name in ('current','next_tid','ready','tls_slots'):setattr(self,name,copy.deepcopy(state[name]))
        for name,value in shared.items():setattr(self,name,value)
        self.trace.clear();self.running=False

    def handle(self,obj):
        handle=self.vm.next_handle;self.vm.next_handle+=1;self.objects[handle]=obj;return handle

    def thread_handle(self,handle):
        if handle==0xfffffffe:return self.threads[self.current]
        obj=self.objects.get(handle)
        if obj is None or obj['type']!='thread':raise ValueError('Invalid thread handle')
        return self.threads[obj['tid']]

    @property
    def tib(self):return self.TIB+(self.current-1)*4096
    def get_error(self,args):return self.vm.u32(self.tib+0x34)
    def set_error(self,args):self.vm.put_u32(self.tib+0x34,args[0]);return 0

    def tls_alloc(self,args):
        for index in range(64):
            if index not in self.tls_slots:self.tls_slots.add(index);return index
        self.set_error((8,));return 0xffffffff

    def tls_get(self,args):
        index=args[0]
        if index not in self.tls_slots:self.set_error((87,));return 0
        self.set_error((0,));return self.threads[self.current].tls.get(index,0)

    def tls_set(self,args):
        index,value=args
        if index not in self.tls_slots:self.set_error((87,));return 0
        self.threads[self.current].tls[index]=value
        self.vm.put_u32(self.tib+0xe10+4*index,value);return 1

    def tls_free(self,args):
        index=args[0]
        if index not in self.tls_slots:self.set_error((87,));return 0
        self.tls_slots.remove(index)
        for thread in self.threads.values():
            thread.tls.pop(index,None);self.vm.put_u32(self.TIB+(thread.tid-1)*4096+0xe10+4*index,0)
        return 1

    def create_thread(self,args):
        security,size,entry,param,flags,out_tid=args
        if security or flags&~4:raise ValueError('Unsupported CreateThread flags/security')
        tid=self.next_tid
        if tid>self.MAX_THREADS:raise MemoryError('Guest thread capacity exhausted')
        self.next_tid+=1
        size=size or struct.unpack_from('<I',self.vm.image,self.vm.layout['optional_offset']+72)[0]
        size=(max(size,4096)+4095)&~4095
        low=self.vm.allocate(size);high=low+size
        parent=self.vm.uc.context_save()
        try:
            selector=self.environment(tid,low,high)
            for reg in (UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_ECX,UC_X86_REG_EDX,
                        UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP):self.vm.uc.reg_write(reg,0)
            self.vm.uc.reg_write(UC_X86_REG_FS,selector)
            self.vm.uc.reg_write(UC_X86_REG_EFLAGS,0x202);self.vm.uc.reg_write(UC_X86_REG_FPCW,0x37f)
            sp=high-0x100;self.vm.write(sp,struct.pack('<II',self.THREAD_RETURN,param))
            self.vm.uc.reg_write(UC_X86_REG_ESP,sp);self.vm.uc.reg_write(UC_X86_REG_EIP,entry)
            suspended=bool(flags&4)
            self.threads[tid]=Thread(tid,self.vm.uc.context_save(),low,high,
                'suspended' if suspended else 'ready',suspend_count=int(suspended))
        finally:self.vm.uc.context_restore(parent)
        if not suspended:self.ready.append(tid)
        if out_tid:self.vm.put_u32(out_tid,tid)
        return self.handle(dict(type='thread',tid=tid))

    def resume_thread(self,args):
        thread=self.thread_handle(args[0]);previous=thread.suspend_count
        if previous:
            thread.suspend_count-=1
            if not thread.suspend_count:thread.state='ready';self.ready.append(thread.tid)
        return previous

    def sleep(self,args):
        thread=self.threads[self.current];duration=args[0]
        thread.state='ready' if not duration else 'sleeping'
        thread.wake=None if duration==0xffffffff else self.vm.elapsed_ms+duration
        return Yield(0)

    def exit_thread(self,args):
        thread=self.threads[self.current];thread.state='exited';thread.exit_code=args[0]
        return Yield(args[0])

    def create_event(self,args):
        security,manual,initial,name=args
        if security:raise ValueError('Event security attributes unsupported')
        text=self.vm.string(name) if name else None
        if text and text in self.named_events:
            obj=self.named_events[text]
            self.set_error((183,)) # ERROR_ALREADY_EXISTS; existing event parameters win.
        else:
            obj=dict(type='event',manual=bool(manual),signaled=bool(initial))
            if text:self.named_events[text]=obj
            self.set_error((0,))
        return self.handle(obj)

    def close_handle(self,handle):
        obj=self.objects.pop(handle,None)
        if obj is None:return 0
        # Names identify a live kernel object, not a permanent cache. Retain
        # aliases while another handle refers to it, then release its name.
        if obj['type']=='event' and not any(other is obj for other in self.objects.values()):
            for name,event in list(self.named_events.items()):
                if event is obj:del self.named_events[name]
        return 1

    def event(self,handle):
        obj=self.objects.get(handle)
        if obj is None or obj['type']!='event':raise ValueError('Invalid event handle')
        return obj

    def set_event(self,args):self.event(args[0])['signaled']=True;return 1
    def reset_event(self,args):self.event(args[0])['signaled']=False;return 1

    def signaled(self,handle):
        obj=self.objects.get(handle)
        if obj is None:raise ValueError('Invalid wait handle')
        if obj['type']=='event':return obj['signaled']
        return self.threads[obj['tid']].state=='exited'

    def consume(self,handles):
        # Duplicate handles in wait-all are rejected before reaching this path.
        for handle in handles:
            obj=self.objects[handle]
            if obj['type']=='event' and not obj['manual']:obj['signaled']=False

    def wait(self,handles,all_objects,timeout):
        if not handles or len(handles)>64 or (all_objects and len(set(handles))!=len(handles)):
            raise ValueError('Unsupported/invalid wait handle list')
        thread=self.threads[self.current]
        states=[self.signaled(handle) for handle in handles]
        if (all(states) if all_objects else any(states)):
            index=0 if all_objects else states.index(True)
            self.consume(handles if all_objects else [handles[index]])
            thread.wait=None;return index
        deadline=thread.wait['deadline'] if thread.wait else (None if timeout==0xffffffff else self.vm.elapsed_ms+timeout)
        if deadline is not None and deadline<=self.vm.elapsed_ms:
            thread.wait=None;return 258
        thread.wait=dict(kind='objects',handles=list(handles),all=bool(all_objects),deadline=deadline)
        thread.state='waiting';return Yield(None)

    def wait_single(self,args):return self.wait([args[0]],False,args[1])

    def wait_multiple(self,args):
        count,pointer,all_objects,timeout=args
        if count<1 or count>64:raise ValueError('Invalid WaitForMultipleObjects count')
        handles=struct.unpack('<'+'I'*count,self.vm.uc.mem_read(pointer,count*4))
        return self.wait(handles,bool(all_objects),timeout)

    def initialize_critical(self,args):
        self.critical[args[0]]=dict(owner=None,depth=0);return 0

    def enter_critical(self,args):
        address=args[0];critical=self.critical[address];thread=self.threads[self.current]
        if critical['owner'] in (None,self.current):
            critical['owner']=self.current;critical['depth']+=1;thread.wait=None;return 0
        thread.wait=dict(kind='critical',address=address,deadline=None);thread.state='waiting';return Yield(None)

    def leave_critical(self,args):
        critical=self.critical[args[0]]
        if critical['owner']!=self.current or not critical['depth']:raise ValueError('Critical section not owned by this thread')
        critical['depth']-=1
        if not critical['depth']:critical['owner']=None
        return 0

    def delete_critical(self,args):
        if self.critical[args[0]]['owner'] is not None:raise ValueError('Cannot delete an owned critical section')
        del self.critical[args[0]];return 0

    def refresh(self):
        for tid,thread in self.threads.items():
            wake=False
            if thread.state=='sleeping':wake=thread.wake is not None and thread.wake<=self.vm.elapsed_ms
            elif thread.state=='waiting':
                wait=thread.wait
                if wait['kind']=='critical':wake=self.critical[wait['address']]['owner'] is None
                else:
                    states=[self.signaled(h) for h in wait['handles']]
                    wake=all(states) if wait['all'] else any(states)
                wake=wake or (wait['deadline'] is not None and wait['deadline']<=self.vm.elapsed_ms)
            if wake:thread.state='ready';self.ready.append(tid)

    def execute(self,address,instruction_limit):
        root=self.threads[1]
        root.context=self.vm.uc.context_save();root.state='ready';root.wait=None;root.wake=None
        self.vm.uc.reg_write(UC_X86_REG_EIP,address);root.context=self.vm.uc.context_save()
        self.ready=[1]+[tid for tid in self.ready if tid!=1]
        self.running=True;remaining=instruction_limit
        try:
            while root.state not in ('returned','exited','stopped'):
                self.refresh()
                if not self.ready:
                    deadlines=[t.wake for t in self.threads.values() if t.state=='sleeping' and t.wake is not None]
                    deadlines+=[t.wait['deadline'] for t in self.threads.values() if t.state=='waiting' and t.wait['deadline'] is not None]
                    if not deadlines:raise GuestDeadlock('No runnable guest thread and no finite wake/timeout')
                    self.vm.sleep_ms+=max(Fraction(0),min(deadlines)-self.vm.elapsed_ms)
                    continue
                tid=self.ready.pop(0);thread=self.threads[tid]
                if thread.state!='ready':continue
                self.current=tid;self.vm.uc.context_restore(thread.context);thread.state='running'
                budget=min(self.QUANTUM,remaining)
                if budget<=0:raise RuntimeError('Guest scheduler instruction-slice budget exhausted')
                self.trace.append(dict(tid=tid,clock=self.vm.milliseconds,pc=self.vm.uc.reg_read(UC_X86_REG_EIP)))
                self.vm.uc.emu_start(self.vm.uc.reg_read(UC_X86_REG_EIP),0,count=budget)
                remaining-=budget;thread.context=self.vm.uc.context_save()
                if self.vm.error:raise self.vm.error
                if thread.state=='running':thread.state='ready'
                if thread.state=='ready':self.ready.append(tid)
            self.current=1;self.vm.uc.context_restore(root.context)
            return self.vm.uc.reg_read(UC_X86_REG_EAX)
        finally:self.running=False
