#!/usr/bin/env python3
"""Check memory lifetime, protection, rollback and thread-local isolation."""
import argparse
import json
from pathlib import Path
import time
import unittest
from unicorn import UcError
from runtime import Runtime
from test_threads import api,slot,assemble,BASELINE


class MemoryTests(unittest.TestCase):
    def test_virtual_reserve_commit_protection_decommit_and_restore(self):
        vm=Runtime(BASELINE);address=vm.call(api(vm,'VirtualAlloc'),(0,8192,0x2000,4))
        load=assemble(vm,'mov eax,[esp+4]\nmov eax,[eax]\nret 4\n')
        with self.assertRaises(UcError):vm.call(load,(address,))
        self.assertEqual(vm.call(api(vm,'VirtualAlloc'),(address,4096,0x1000,4)),address)
        self.assertEqual(vm.call(load,(address,)),0)
        vm.put_u32(address,123);vm.checkpoint()
        self.assertEqual(vm.call(api(vm,'VirtualFree'),(address,4096,0x4000)),1)
        with self.assertRaises(UcError):vm.call(load,(address,))
        vm.restore();self.assertEqual(vm.call(load,(address,)),123)
        vm.call(api(vm,'VirtualAlloc'),(address+4096,4096,0x1000,4));vm.restore()
        with self.assertRaises(UcError):vm.call(load,(address+4096,))

    def test_virtual_release_and_new_mappings_roll_back(self):
        vm=Runtime(BASELINE);old=vm.call(api(vm,'VirtualAlloc'),(0,4096,0x3000,4));vm.put_u32(old,321)
        vm.checkpoint();vm.call(api(vm,'VirtualFree'),(old,0,0x8000))
        new=vm.call(api(vm,'VirtualAlloc'),(0,4096,0x3000,4));vm.put_u32(new,777)
        vm.call(api(vm,'VirtualFree'),(new,0,0x8000))
        vm.restore();self.assertEqual(vm.u32(old),321);self.assertEqual(set(vm.virtual_memory.regions),{old})
        again=vm.call(api(vm,'VirtualAlloc'),(0,4096,0x3000,4));self.assertEqual(again,new)

    def test_heap_reallocation_zeroing_free_and_metadata_restore(self):
        vm=Runtime(BASELINE);heap=vm.call(api(vm,'HeapCreate'),(0,0,0))
        address=vm.call(api(vm,'HeapAlloc'),(heap,8,8));vm.write(address,b'original');vm.checkpoint()
        larger=vm.call(api(vm,'HeapReAlloc'),(heap,8,address,16))
        self.assertEqual(bytes(vm.uc.mem_read(larger,16)),b'original'+bytes(8))
        self.assertEqual(vm.call(api(vm,'HeapSize'),(heap,0,larger)),16)
        self.assertEqual(vm.call(api(vm,'HeapFree'),(heap,0,larger)),1)
        self.assertEqual(vm.call(api(vm,'HeapFree'),(heap,0,larger)),0)
        vm.restore();self.assertEqual(vm.call(api(vm,'HeapSize'),(heap,0,address)),8)
        self.assertEqual(bytes(vm.uc.mem_read(address,8)),b'original')

    def test_guest_stores_restore_with_native_or_python_tracking(self):
        # dirtypages.c tracks guest stores when it builds; the Python hook
        # otherwise. Either way: cross-page stores and a store to memory
        # mapped and released after the checkpoint restore exactly.
        counts={}
        for native in (True,False):
            vm=Runtime(BASELINE)
            if not native and vm._native_dirty is not None:
                for hook in vm._write_hooks:vm._del_write_hook(hook)
                vm._native_dirty=None;vm._write_hooks=[vm._add_write_hook()]
            if native and vm._native_dirty is None:continue
            buffer=vm.allocate(8192);vm.write(buffer+4094,b'abcd')
            store=assemble(vm,'mov eax,[esp+4]\nmov edx,[esp+8]\nmov [eax],edx\nret 8\n')
            vm.checkpoint()
            for _ in range(3):
                vm.call(store,(buffer+4094,0x11223344))
                temporary=vm.call(api(vm,'VirtualAlloc'),(0,4096,0x3000,4))
                vm.call(store,(temporary,7));vm.call(api(vm,'VirtualFree'),(temporary,0,0x8000))
                counts.setdefault(native,set()).add(vm.restore())
                self.assertEqual(bytes(vm.uc.mem_read(buffer+4094,4)),b'abcd')
                self.assertEqual(set(vm.virtual_memory.regions),set())
        self.assertEqual(len(counts[False]),1)
        if True in counts:self.assertEqual(counts[True],counts[False])

    def test_tls_and_last_error_are_thread_local_and_replayable(self):
        vm=Runtime(BASELINE);index=vm.call(api(vm,'TlsAlloc'))
        vm.call(api(vm,'TlsSetValue'),(index,111));vm.call(api(vm,'SetLastError'),(234,))
        result=vm.allocate(16)
        code=assemble(vm,f'''
    push ebx
    mov ebx,[esp+8]
    push 343
    call [0x{slot(vm,'SetLastError'):x}]
    call [0x{slot(vm,'GetLastError'):x}]
    mov [ebx],eax
    push {index}
    call [0x{slot(vm,'TlsGetValue'):x}]
    mov [ebx+4],eax
    push 222
    push {index}
    call [0x{slot(vm,'TlsSetValue'):x}]
    mov eax,1
    pop ebx
    ret 4
''')
        vm.checkpoint()
        for _ in range(10):
            vm.restore();handle=vm.call(api(vm,'CreateThread'),(0,0x10000,code,result,0,0))
            self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(handle,0xffffffff)),0)
            self.assertEqual(vm.u32(result),343);self.assertEqual(vm.u32(result+4),0)
            self.assertEqual(vm.call(api(vm,'GetLastError')),234)
            self.assertEqual(vm.call(api(vm,'TlsGetValue'),(index,)),111)
            self.assertEqual(vm.scheduler.threads[2].tls[index],222)


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    start=time.perf_counter();result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(MemoryTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,
        errors=len(result.errors),failures=len(result.failures),elapsed_seconds=time.perf_counter()-start,
        scope='Guest reservation/commit/protection/decommit/release, heap lifetime/realloc, TLS/last-error isolation and rollback. Allocation layout differs from Windows heap internals.')
    if args.output:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
