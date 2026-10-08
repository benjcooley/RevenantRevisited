#!/usr/bin/env python3
"""Check replay isolation against actual retail instructions and compiled hooks."""
import argparse
import hashlib
import json
from pathlib import Path
import time
import subprocess
import sys
import unittest
from unicorn import UcError
from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_EAX
from runtime import Runtime,MissingAPI
from run import Session
from threads import GuestDeadlock


ROOT=Path(__file__).resolve().parents[2]
BASELINE=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
EXPERIMENTS=ROOT/'recon/retail_asm/experiments'


class ReplayTests(unittest.TestCase):
    def test_warm_instruction_patch_and_restore_cross_page(self):
        vm=Runtime(BASELINE);area=vm.allocate(8192);address=area+4093
        vm.write_code(address,b'\xb8\x01\x00\x00\x00\xc3')
        vm.checkpoint();self.assertEqual(vm.call(address),1)
        # The immediate spans pages and has already been translated.
        vm.put_u32(address+1,2);self.assertEqual(vm.call(address),2)
        vm.restore();self.assertEqual(vm.call(address),1)
        vm.write_code(address,b'\xb8\x03\x00\x00\x00\xc3');self.assertEqual(vm.call(address),3)
        vm.restore();self.assertEqual(vm.call(address),1)

    def test_json_code_patch_replays_without_worker_restart(self):
        session=Session(BASELINE,[dict(op='allocate',name='function',size=16),
            dict(op='write_code',address='$function',hex='b801000000c3')])
        patch=dict(operations=[dict(op='call',address='$function',expect_eax=1),
            dict(op='write_code',address='$function',hex='b802000000c3'),
            dict(op='call',address='$function',expect_eax=2)])
        for _ in range(3):self.assertEqual(session.execute(patch)['status'],'pass')
        self.assertEqual(session.execute(dict(operations=[dict(op='call',address='$function',expect_eax=1)]))['status'],'pass')

    def test_existing_pe_instruction_writes_invalidate_automatically(self):
        vm=Runtime(BASELINE);vm.checkpoint();original=vm.call(0x56d400,(20,6))
        vm.write(0x56d400,b'\xb8\x7b\x00\x00\x00\xc3')
        self.assertEqual(vm.call(0x56d400,(20,6)),123)
        vm.restore();self.assertEqual(vm.call(0x56d400,(20,6)),original)

    def test_original_machine_code_scenario_repeats(self):
        scenario=json.loads((Path(__file__).parent/'scenarios/render_state.json').read_text())
        session=Session(BASELINE,scenario['setup'])
        results=[session.execute(scenario)['results'] for _ in range(100)]
        self.assertTrue(all(r==results[0] for r in results))
        self.assertEqual(results[0][2]['value'],6)
        self.assertEqual(results[0][5]['value'],2)
        self.assertEqual(hashlib.sha256(session.vm.image).hexdigest(),scenario['executable_sha256'])

    def test_context_memory_heap_clock_rng_and_files_reset(self):
        vm=Runtime(BASELINE);address=vm.allocate(8192)
        vm.write(address+4090,b'baseline-cross-page')
        vm.files['fixture']=bytearray(b'original')
        vm.uc.reg_write(UC_X86_REG_EAX,123);vm.checkpoint()
        sequence=[vm.random_msvc() for _ in range(20)]
        vm.write(address+4090,b'overwritten-state!')
        vm.advance(24);vm.files['fixture'][:]=b'mutated';vm.allocate(123)
        vm.uc.reg_write(UC_X86_REG_EAX,999)
        self.assertEqual(vm.restore(),3)
        self.assertEqual(bytes(vm.uc.mem_read(address+4090,19)),b'baseline-cross-page')
        self.assertEqual(vm.uc.reg_read(UC_X86_REG_EAX),123)
        self.assertEqual(vm.milliseconds,0)
        self.assertEqual(vm.heap_next,address+8192)
        self.assertEqual(vm.files['fixture'],b'original')
        self.assertEqual([vm.random_msvc() for _ in range(20)],sequence)

    def test_compiled_c_logger_uses_virtual_file_system(self):
        root=EXPERIMENTS/'winmain_skip';manifest=json.loads((root/'manifest.json').read_text())
        vm=Runtime(root/'Revenant.hooked.exe');vm.checkpoint()
        for _ in range(30):
            vm.restore();result=vm.call(manifest['target_va'],(1,2,3,4))
            self.assertEqual(result,0)
            self.assertEqual(vm.files['retail-asm.log'],manifest['log_message'].encode())
            self.assertEqual([x['function'] for x in vm.api_trace],['CreateFileA','SetFilePointer','WriteFile','CloseHandle'])
            self.assertEqual(set(vm.handles),set(vm.process.standard.values()))

    def test_clock_import_and_missing_api_stop(self):
        vm=Runtime(BASELINE);vm.checkpoint();vm.advance(24)
        tick=next(a for a,name in vm.stubs.items() if name==('kernel32.dll','GetTickCount'))
        self.assertEqual(vm.call(tick),1000)
        vm.restore();self.assertEqual(vm.call(tick),0)
        missing=next(a for a,(dll,name) in vm.stubs.items() if name not in vm.handlers)
        with self.assertRaises(MissingAPI):vm.call(missing)

    def test_faulted_scenario_rolls_back_and_next_request_works(self):
        session=Session(BASELINE)
        session.vm.put_u32(0x675f10,123);session.checkpoint()
        with self.assertRaises(UcError):session.execute(dict(operations=[
            dict(op='write_u32',address='0x675f10',value=456),
            dict(op='call',address='0x12345678')]))
        self.assertEqual(session.vm.u32(0x675f10),123)
        result=session.execute(dict(operations=[dict(op='read_u32',address='0x675f10',expect=123)]))
        self.assertEqual(result['status'],'pass')

    def test_offscreen_surface_capture_replays(self):
        session=Session(BASELINE,[dict(op='surface',name='screen',width=3,height=2)])
        scenario=dict(operations=[dict(op='write',address='$screen',hex='00f8e0071f00'),dict(op='capture',name='screen')])
        first=session.execute(scenario)['results'][-1]
        self.assertEqual(first,session.execute(scenario)['results'][-1])
        self.assertEqual(first['stride'],8)
        session.vm.restore();self.assertEqual(session.vm.surface_bytes('screen'),bytes(16))

    def test_persistent_jsonl_process_survives_invalid_request(self):
        requests=[dict(id=1,command='status'),dict(id=2,scenario=dict(operations=[
            dict(op='call',address='0x56d400',args=[20,6]),
            dict(op='read_u32',address='0x675f10',expect=6)])),
            dict(id=3,command='checkpoint'),None,
            dict(id=4,scenario=dict(operations=[dict(op='read_u32',address='0x675f10',expect=6)]))]
        process=subprocess.run([sys.executable,str(Path(__file__).parent/'run.py'),str(BASELINE)],
            input='\n'.join(json.dumps(r) for r in requests)+'\n',capture_output=True,text=True,check=True)
        responses=[json.loads(line) for line in process.stdout.splitlines()]
        self.assertEqual([r['status'] for r in responses],['ready','pass','checkpointed','error','pass'])
        self.assertEqual(responses[-1]['id'],4)
        self.assertEqual(responses[0]['guest_os_boots'],0)

    def test_sleep_and_real_imported_multimedia_clock(self):
        vm=Runtime(BASELINE);vm.checkpoint()
        api=lambda dll,name:next(a for a,n in vm.stubs.items() if n==(dll,name))
        vm.call(api('kernel32.dll','Sleep'),(10,))
        self.assertEqual(vm.call(api('_inmm.dll','timeGetTime')),10)
        vm.advance(24);self.assertEqual(vm.milliseconds,1010)
        vm.restore();self.assertEqual(vm.milliseconds,0)
        with self.assertRaises(GuestDeadlock):vm.call(api('kernel32.dll','Sleep'),(0xffffffff,))

    def test_message_filters_timer_coalescing_and_checkpoint(self):
        vm=Runtime(BASELINE);msg=vm.allocate(28);vm.checkpoint()
        api=lambda name:next(a for a,n in vm.stubs.items() if n==('user32.dll',name))
        peek=lambda flags=1,low=0,high=0:vm.call(api('PeekMessageA'),(msg,0,low,high,flags))
        identifier=vm.call(api('SetTimer'),(0,999,10,0))
        vm.post_message(0,0x100,65)
        self.assertEqual(peek(0),1);self.assertEqual(len(vm.messages),1)
        self.assertEqual(peek(),1);self.assertEqual(vm.u32(msg+4),0x100)
        self.assertEqual(peek(),0)
        vm.advance(24)
        self.assertEqual(peek(),1);self.assertEqual(vm.u32(msg+4),0x113)
        self.assertEqual(vm.u32(msg+8),identifier)
        self.assertEqual(peek(),0)  # No 100-message burst after missed intervals.
        vm.call(api('PostQuitMessage'),(7,))
        self.assertEqual(peek(1,0x200,0x209),1);self.assertEqual(vm.u32(msg+4),0x12)
        self.assertEqual(vm.u32(msg+8),7)
        vm.restore();self.assertEqual(vm.timers,{});self.assertEqual(vm.messages,[])

    def test_dispatch_executes_guest_callback_with_correct_stdcall_abi(self):
        vm=Runtime(BASELINE);msg=vm.allocate(28);callback=vm.allocate(16)
        vm.write(callback,bytes.fromhex('8b44240cc21000'))  # Return third callback argument.
        vm.windows[123]=callback;vm.post_message(123,0x100,65,999)
        api=lambda name:next(a for a,n in vm.stubs.items() if n==('user32.dll',name))
        self.assertEqual(vm.call(api('PeekMessageA'),(msg,0,0,0,1)),1)
        self.assertEqual(vm.call(api('DispatchMessageA'),(msg,)),65)
        self.assertEqual(vm.api_trace[-1]['delegated_to'],callback)


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    started=time.perf_counter();result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(ReplayTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,
        failures=len(result.failures),errors=len(result.errors),elapsed_seconds=time.perf_counter()-started,
        baseline_sha256=hashlib.sha256(BASELINE.read_bytes()).hexdigest(),
        scope='Original render-state functions, compiled C hook with virtual I/O, replay isolation and surface buffers. No retail HUD/combat or full startup yet.')
    if args.output:args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
