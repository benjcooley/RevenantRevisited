#!/usr/bin/env python3
"""Run the original HUD bar kernel; record its draws at the raster boundary.

No bar calculation is reimplemented. Pixel drawing is deliberately intercepted;
this proves original slice decisions, not screenshots or a complete HUD.
"""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import time
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX
from runtime import Runtime

RETAIL_SHA='28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'


class BarTrace:
    def __init__(self,executable):
        self.vm=Runtime(executable)
        if hashlib.sha256(self.vm.image).hexdigest()!=RETAIL_SHA:
            raise ValueError('HUD function addresses require the verified retail image')
        self.panel=self.vm.allocate(0xe0);self.bitmap=self.vm.allocate(0x80)
        self.vm.put_u32(self.panel+0x68,self.bitmap)
        self.vm.put_u32(0x6680c8,0)  # Explicit Classic branch fixture.
        self.draws=[]
        self.vm.uc.hook_add(UC_HOOK_CODE,self.capture,begin=0x414d70,end=0x414d70)
        self.vm.checkpoint()

    def capture(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        args=struct.unpack('<14I',uc.mem_read(sp+4,56))
        self.draws.append(dict(dst_x=args[0],dst_y=args[1],z=args[2],source=args[3],
            source_z=args[4],width=args[5],height=args[6],tint=args[7],src_x=args[8],
            src_y=args[9],src_width=args[10],src_height=args[11],matrix=args[12],mode=args[13]))
        # Original 0x414d70 ABI: thiscall, 14 stack arguments, ret 0x38.
        # Its raster work is skipped; the original 0x54a5d0 decisions execute.
        uc.reg_write(UC_X86_REG_EAX,1)
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP,sp+60)

    def run(self,value,maximum,direction=1,fade=6):
        self.vm.restore();self.draws=[]
        self.vm.put_u32(self.panel+0xd4,fade);self.vm.put_u32(self.panel+0xdc,fade)
        # HP literals from the actual retail status bar call sites. The kernel
        # clamps value/max, computes fill, chooses slices and packs alpha.
        args=(value&0xffffffff,maximum&0xffffffff,0x44,0x0f,2,1,2,0x31,0x7d,0x11,0x77,6,direction&0xffffffff)
        self.vm.call(0x54a5d0,args,this=self.panel)
        return list(self.draws)


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--repeat',type=int,default=20)
    args=parser.parse_args()
    if args.repeat<1:parser.error('--repeat must be positive')
    trace=BarTrace(args.executable);cases=[];timings=[]
    for maximum,values in [(100,[-10,0,25,50,100,125]),(0,[0,1])]:
        for value in values:
            for direction in (1,-1):
                for fade in (0,3,6):
                    reference=None
                    for _ in range(args.repeat):
                        start=time.perf_counter();draws=trace.run(value,maximum,direction,fade)
                        timings.append((time.perf_counter()-start)*1000)
                        if reference is None:reference=draws
                        elif reference!=draws:raise AssertionError('Retail bar trace did not replay exactly')
                    cases.append(dict(value=value,maximum=maximum,direction=direction,fade=fade,draws=reference))
    report=dict(status='pass',retail_sha256=RETAIL_SHA,original_kernel='0x54a5d0',
        intercepted_draw_boundary='0x414d70',cases=len(cases),replays_per_case=args.repeat,
        total_replays=len(timings),mean_loop_ms=statistics.mean(timings),median_loop_ms=statistics.median(timings),
        guest_os_boots=0,results=cases,
        scope='Original Classic HP bar slice/clamp/alpha decisions run in process. Draw boundary intercepted and recorded; no retail pixels, assets, full HUD or Revisited comparison yet.')
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='results'},indent=2))


if __name__=='__main__':main()
