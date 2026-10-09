#!/usr/bin/env python3
"""Exercise guest scheduling with real retail helpers and x86 Win32 callers."""
import argparse
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import time
import unittest
from runtime import Runtime
from threads import GuestDeadlock
from function_hook import imports

BASELINE=Path(__file__).resolve().parents[2]/'recon/retail_asm/baseline/Revenant.rebuilt.exe'


def assemble(vm,body):
    address=vm.allocate(4096)
    with tempfile.TemporaryDirectory() as temp:
        source=Path(temp)/'probe.asm';output=Path(temp)/'probe.bin'
        source.write_text(f'BITS 32\nORG 0x{address:x}\n'+body)
        subprocess.run([shutil.which('nasm'),'-f','bin',str(source),'-o',str(output)],check=True,capture_output=True)
        vm.write(address,output.read_bytes())
    return address


def slot(vm,name):return imports(vm.image,vm.layout)['kernel32.dll',name]
def api(vm,name):return vm.api_address('kernel32.dll',name)


class ThreadTests(unittest.TestCase):
    def test_original_retail_functions_worker_sleep_event_and_replay(self):
        vm=Runtime(BASELINE);result=vm.allocate(32)
        code=assemble(vm,f'''
main:
    push ebx
    mov ebx,[esp+8]
    push 0
    push 0
    push 0
    push 0
    call [0x{slot(vm,'CreateEventA'):x}]
    mov [ebx],eax
    lea eax,[ebx+4]
    push eax
    push 0
    push ebx
    push worker
    push 0x10000
    push 0
    call [0x{slot(vm,'CreateThread'):x}]
    mov [ebx+8],eax
    push 0xffffffff
    push dword [ebx]
    call [0x{slot(vm,'WaitForSingleObject'):x}]
    mov [ebx+12],eax
    push 0xffffffff
    push dword [ebx+8]
    call [0x{slot(vm,'WaitForSingleObject'):x}]
    mov [ebx+16],eax
    mov eax,[ebx+20]
    pop ebx
    ret 4
worker:
    push ebx
    mov ebx,[esp+8]
    mov eax,[fs:0x24]
    mov [ebx+24],eax
    push 6
    push 20
    call 0x56d400
    push 10
    call [0x{slot(vm,'Sleep'):x}]
    lea eax,[ebx+20]
    push eax
    push 20
    call 0x56d4b0
    push dword [ebx]
    call [0x{slot(vm,'SetEvent'):x}]
    mov eax,42
    pop ebx
    ret 4
''')
        vm.checkpoint();reference=None
        for _ in range(30):
            vm.restore();self.assertEqual(vm.call(code,(result,)),6)
            state=[vm.u32(result+i*4) for i in range(7)]
            self.assertEqual(state[1],2);self.assertEqual(state[3:7],[0,0,6,2])
            self.assertEqual(vm.milliseconds,10)
            self.assertEqual(vm.scheduler.threads[2].exit_code,42)
            replay=(state,vm.scheduler.trace)
            if reference is None:reference=replay
            else:self.assertEqual(replay,reference)
        vm.restore();self.assertEqual(set(vm.scheduler.threads),{1})

    def test_thread_environment_and_suspended_thread(self):
        vm=Runtime(BASELINE);tid=vm.allocate(4)
        worker=assemble(vm,'mov eax,[fs:0x24]\nret 4\n')
        handle=vm.call(api(vm,'CreateThread'),(0,0x10000,worker,0,4,tid))
        self.assertEqual(vm.u32(tid),2)
        self.assertEqual(vm.scheduler.threads[2].state,'suspended')
        self.assertEqual(vm.call(api(vm,'ResumeThread'),(handle,)),1)
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(handle,0xffffffff)),0)
        self.assertEqual(vm.scheduler.threads[2].exit_code,2)
        self.assertNotEqual(vm.scheduler.threads[1].stack_low,vm.scheduler.threads[2].stack_low)
        main=assemble(vm,'mov eax,[fs:0x24]\nret\n')
        self.assertEqual(vm.call(main),1)

    def test_events_timeout_wait_any_wait_all_and_aliases_restore(self):
        vm=Runtime(BASELINE);name=vm.allocate(16);vm.write(name,b'named-event\0')
        first=vm.call(api(vm,'CreateEventA'),(0,1,0,name))
        alias=vm.call(api(vm,'CreateEventA'),(0,0,1,name))
        other=vm.call(api(vm,'CreateEventA'),(0,0,1,0))
        handles=vm.allocate(8);vm.put_u32(handles,first);vm.put_u32(handles+4,other)
        vm.checkpoint()
        self.assertEqual(vm.call(api(vm,'WaitForMultipleObjects'),(2,handles,0,0)),1)
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(other,5)),258)
        self.assertEqual(vm.milliseconds,5)
        vm.call(api(vm,'SetEvent'),(alias,))
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(first,0)),0)
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(first,0)),0)
        vm.restore();vm.call(api(vm,'SetEvent'),(alias,))
        self.assertEqual(vm.call(api(vm,'WaitForMultipleObjects'),(2,handles,1,0)),0)
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(other,0)),258)
        alias2=vm.call(api(vm,'CreateEventA'),(0,1,0,name))
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(alias2,0)),0)

    def test_critical_section_reentrancy_and_unowned_leave_rejected(self):
        vm=Runtime(BASELINE);section=vm.allocate(24)
        vm.call(api(vm,'InitializeCriticalSection'),(section,))
        vm.call(api(vm,'EnterCriticalSection'),(section,));vm.call(api(vm,'EnterCriticalSection'),(section,))
        self.assertEqual(vm.scheduler.critical[section]['depth'],2)
        vm.call(api(vm,'LeaveCriticalSection'),(section,));vm.call(api(vm,'LeaveCriticalSection'),(section,))
        with self.assertRaises(ValueError):vm.call(api(vm,'LeaveCriticalSection'),(section,))

    def test_create_thread_ignores_undefined_flag_bits(self):
        vm=Runtime(BASELINE);entry=assemble(vm,'xor eax, eax\nret 4\n')
        handle=vm.call(api(vm,'CreateThread'),(0,0,entry,0,1,0))
        tid=vm.scheduler.objects[handle]['tid']
        self.assertEqual(vm.scheduler.threads[tid].state,'ready')
        suspended=vm.call(api(vm,'CreateThread'),(0,0,entry,0,4|1,0))
        self.assertEqual(vm.scheduler.threads[vm.scheduler.objects[suspended]['tid']].state,'suspended')
        with self.assertRaises(ValueError):vm.call(api(vm,'CreateThread'),(vm.allocate(12),0,entry,0,0,0))

    def test_mutex_recursive_ownership_release_and_checkpoint(self):
        vm=Runtime(BASELINE)
        mutex=vm.call(api(vm,'CreateMutexA'),(0,0,0))
        vm.checkpoint()
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(mutex,0)),0)
        self.assertEqual(vm.call(api(vm,'WaitForSingleObject'),(mutex,0)),0)
        self.assertEqual(vm.scheduler.objects[mutex]['depth'],2)
        self.assertEqual(vm.call(api(vm,'ReleaseMutex'),(mutex,)),1)
        self.assertEqual(vm.call(api(vm,'ReleaseMutex'),(mutex,)),1)
        self.assertEqual(vm.call(api(vm,'ReleaseMutex'),(mutex,)),0)
        self.assertEqual(vm.call(api(vm,'GetLastError')),288)
        vm.restore()
        self.assertIsNone(vm.scheduler.objects[mutex]['owner'])
        owned=vm.call(api(vm,'CreateMutexA'),(0,1,0))
        self.assertEqual(vm.scheduler.objects[owned]['owner'],vm.scheduler.current)
        with self.assertRaises(ValueError):vm.call(api(vm,'ReleaseMutex'),(0x7fff,))

    def test_named_event_final_close_recreates_and_checkpoint_preserves_live_aliases(self):
        vm=Runtime(BASELINE);name=vm.allocate(32);vm.write(name,b'close-recreate-event\0')
        create=lambda manual,initial:vm.call(api(vm,'CreateEventA'),(0,manual,initial,name))
        wait=lambda handle:vm.call(api(vm,'WaitForSingleObject'),(handle,0))
        close=lambda handle:vm.call(api(vm,'CloseHandle'),(handle,))
        first=create(1,1);alias=create(0,0)
        self.assertEqual(vm.call(api(vm,'GetLastError')),183)
        # Opening an existing named event preserves its manual-reset behavior.
        self.assertEqual(wait(alias),0);self.assertEqual(wait(alias),0)
        vm.call(api(vm,'ResetEvent'),(first,));self.assertEqual(close(first),1)
        second_alias=create(0,1)
        self.assertEqual(vm.call(api(vm,'GetLastError')),183)
        self.assertEqual(wait(second_alias),258) # The surviving alias retains state.
        vm.checkpoint()
        for _ in range(3):
            vm.restore()
            self.assertEqual(wait(alias),258);self.assertEqual(wait(second_alias),258)
            vm.call(api(vm,'SetEvent'),(alias,))
            self.assertEqual(wait(second_alias),0);self.assertEqual(wait(second_alias),0)
            vm.call(api(vm,'ResetEvent'),(alias,))
            self.assertEqual(close(alias),1);self.assertEqual(wait(second_alias),258)
            self.assertEqual(close(second_alias),1)
            fresh=create(0,1)
            self.assertEqual(vm.call(api(vm,'GetLastError')),0)
            self.assertEqual(wait(fresh),0);self.assertEqual(wait(fresh),258)
            self.assertEqual(close(fresh),1);self.assertEqual(close(fresh),0)

    def test_deadlock_and_busy_worker_budget_are_explicit(self):
        vm=Runtime(BASELINE);event=vm.call(api(vm,'CreateEventA'),(0,0,0,0))
        with self.assertRaises(GuestDeadlock):vm.call(api(vm,'WaitForSingleObject'),(event,0xffffffff))
        vm=Runtime(BASELINE);worker=assemble(vm,'jmp $\n')
        handle=vm.call(api(vm,'CreateThread'),(0,0x10000,worker,0,0,0))
        with self.assertRaisesRegex(RuntimeError,'budget exhausted'):
            vm.call(api(vm,'WaitForSingleObject'),(handle,0xffffffff),instruction_limit=30000)


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    start=time.perf_counter();result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(ThreadTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,
        errors=len(result.errors),failures=len(result.failures),elapsed_seconds=time.perf_counter()-start,
        scope='Guest TIB/FS, original retail setter/getter on a worker thread, sleep, event waits, suspension, exit and replay. Deterministic scheduling policy, not Windows scheduling equivalence or full game startup.')
    if args.output:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
