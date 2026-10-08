"""Scoped Win32 memory. Allocation policy is deterministic, not a Windows heap clone."""
import copy
from unicorn import UC_PROT_NONE,UC_PROT_READ,UC_PROT_WRITE,UC_PROT_EXEC


class VirtualMemory:
    PROTECTION={1:UC_PROT_NONE,2:UC_PROT_READ,4:UC_PROT_READ|UC_PROT_WRITE,
                0x10:UC_PROT_EXEC,0x20:UC_PROT_READ|UC_PROT_EXEC,0x40:UC_PROT_READ|UC_PROT_WRITE|UC_PROT_EXEC}

    def __init__(self,vm):self.vm=vm;self.next_address=0x20000000;self.regions={}

    def snapshot(self):return copy.deepcopy(dict(next_address=self.next_address,regions=self.regions))

    def restore_mappings(self,state):
        for base,region in self.regions.items():
            if base not in state['regions']:
                self.vm.uc.mem_unmap(base,region['size'])
                for page in range(base,base+region['size'],4096):self.vm.forget_page(page)
        for base,region in state['regions'].items():
            if base not in self.regions:self.vm.uc.mem_map(base,region['size'],UC_PROT_NONE)
            self.vm.uc.mem_protect(base,region['size'],UC_PROT_NONE)
            for page,protection in region['committed'].items():
                self.vm.uc.mem_protect(page,4096,self.PROTECTION[protection])
        self.next_address=state['next_address'];self.regions=copy.deepcopy(state['regions'])

    def find(self,address,size):
        for base,region in self.regions.items():
            if base<=address and address+size<=base+region['size']:return base,region
        raise ValueError('Virtual memory range is outside a reservation')

    def alloc(self,args):
        address,size,flags,protection=args
        if not size or flags&~0x3000 or not flags&0x3000:raise ValueError('Unsupported VirtualAlloc size/flags')
        if protection not in self.PROTECTION:raise ValueError('Unsupported virtual memory protection')
        start=address&~4095;end=(address+size+4095)&~4095;rounded=(size+4095)&~4095
        if flags&0x2000 or not address:
            if address:raise ValueError('Explicit-address reservation is not implemented')
            start=(self.next_address+65535)&~65535;end=start+rounded
            if end>=0x60000000:return 0
            self.vm.uc.mem_map(start,rounded,UC_PROT_NONE)
            self.regions[start]=dict(size=rounded,committed={});self.next_address=end
        _,region=self.find(start,end-start)
        if flags&0x1000:
            for page in range(start,end,4096):
                if page not in region['committed']:
                    self.vm.write(page,bytes(4096));region['committed'][page]=protection
                    self.vm.uc.mem_protect(page,4096,self.PROTECTION[protection])
        return start

    def free(self,args):
        address,size,flags=args
        if flags==0x8000:
            region=self.regions.get(address)
            if region is None or size:return 0
            self.vm._touch(address,region['size'])
            self.vm.uc.mem_unmap(address,region['size']);del self.regions[address]
            baseline=self.vm.snapshot['virtual_memory']['regions'] if self.vm.snapshot else {}
            if address not in baseline:
                # A reservation created and released after the checkpoint has
                # no baseline pages to restore, and is currently unmapped.
                for page in range(address,address+region['size'],4096):self.vm.forget_page(page)
            return 1
        if flags!=0x4000:raise ValueError('Unsupported VirtualFree type')
        base,region=self.find(address,size or 1)
        start=address&~4095;end=(address+size+4095)&~4095 if size else base+region['size']
        for page in range(start,end,4096):
            if page in region['committed']:
                self.vm.write(page,bytes(4096));del region['committed'][page]
                self.vm.uc.mem_protect(page,4096,UC_PROT_NONE)
        return 1
class Heaps:
    def __init__(self,vm):
        self.vm=vm;self.heaps={vm.HEAP:dict(maximum=0,flags=0,used=0)};self.blocks={}

    def create(self,args):
        flags,initial,maximum=args
        if flags&~1:raise ValueError('Exception-generating/executable heap options unsupported')
        if maximum:raise ValueError('Fixed-size heap overhead semantics not implemented')
        handle=self.vm.next_handle;self.vm.next_handle+=1
        self.heaps[handle]=dict(maximum=maximum,flags=flags,used=0);return handle

    def alloc(self,args):
        heap,flags,size=args
        if heap not in self.heaps:raise ValueError('Invalid heap handle')
        if flags&~9:raise ValueError('Unsupported heap allocation flags')
        try:address=self.vm.allocate(size,bool(flags&8))
        except MemoryError:return 0
        self.blocks[address]=dict(heap=heap,size=size)
        self.heaps[heap]['used']+=size;return address

    def free(self,args):
        heap,flags,address=args
        if flags&~1:raise ValueError('Unsupported HeapFree flags')
        block=self.blocks.get(address)
        if not block or block['heap']!=heap:return 0
        self.heaps[heap]['used']-=block['size'];del self.blocks[address];return 1

    def size(self,args):
        heap,flags,address=args
        if flags&~1:raise ValueError('Unsupported HeapSize flags')
        block=self.blocks.get(address)
        return block['size'] if block and block['heap']==heap else 0xffffffff

    def realloc(self,args):
        heap,flags,address,size=args
        if flags&~0x19:raise ValueError('Unsupported HeapReAlloc flags')
        block=self.blocks.get(address)
        if not block or block['heap']!=heap:return 0
        old=block['size']
        if size<=old:
            self.heaps[heap]['used']-=old-size;block['size']=size;return address
        if flags&0x10:return 0  # Cannot grow this monotonic block in place.
        replacement=self.alloc((heap,flags&9,size))
        if replacement:
            self.vm.write(replacement,bytes(self.vm.uc.mem_read(address,old)))
            self.free((heap,0,address))
        return replacement

    def destroy(self,args):
        handle=args[0]
        if handle==self.vm.HEAP or handle not in self.heaps:return 0
        for address in [a for a,b in self.blocks.items() if b['heap']==handle]:del self.blocks[address]
        del self.heaps[handle];return 1
