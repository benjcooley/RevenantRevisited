#!/usr/bin/env python3
"""Warm-reset benchmark running two original retail machine-code functions."""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import time
from runtime import Runtime


def benchmark(executable,output,iterations):
    begin=time.perf_counter();vm=Runtime(executable);load=time.perf_counter()-begin
    result=vm.allocate(4);vm.surface('screen',640,480);vm.checkpoint()
    timings=[];pages=[];values=[]
    # Original SW SetRenderState and GetRenderState, whose opcode ABI was
    # independently verified. No reimplemented game/raster logic is called.
    for i in range(iterations):
        start=time.perf_counter();pages.append(vm.restore())
        desired=2 if i%2 else 6
        vm.call(0x56d400,(20,desired));vm.call(0x56d4b0,(20,result))
        value=vm.u32(result)
        if value!=desired:raise AssertionError('Original setter/getter mismatch')
        vm.advance(24)
        if vm.milliseconds!=1000:raise AssertionError('Clock reset failed')
        values.append(value);timings.append(time.perf_counter()-start)
    report=dict(status='pass',input_sha256=hashlib.sha256(Path(executable).read_bytes()).hexdigest(),
        host_process='Python+Unicorn in process',guest_os_boots=0,native_windows=0,real_device_calls=0,
        pe_load_seconds=load,iterations=iterations,
        original_calls_per_iteration=2,mean_loop_ms=statistics.mean(timings)*1000,
        median_loop_ms=statistics.median(timings)*1000,p95_loop_ms=sorted(timings)[int(.95*(iterations-1))]*1000,
        maximum_dirty_pages=max(pages),original_function_addresses=['0x56d400','0x56d4b0'],
        scope='Original retail setter/getter machine code + warm dirty-page reset + deterministic clock. Not combat, HUD drawing, complete startup or DirectDraw compatibility.')
    output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--iterations',type=int,default=500)
    a=p.parse_args();benchmark(a.executable,a.output,a.iterations)
