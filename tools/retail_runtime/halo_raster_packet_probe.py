"""Observe complete public Halo integer packets and preserve prior sampled pixels."""
import argparse,json,struct
from pathlib import Path
import halo_native_visual_reference as native
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP

def run(output,baseline,totalframes,step):
    output.mkdir(parents=True,exist_ok=False);packets=[];floating=[]
    base=native.LitSoftwareFixture
    class Observed(base):
        def __init__(self,*a,**kw):
            super().__init__(*a,**kw);vm=self.vm
            def transformed(uc,*_):
                sp=uc.reg_read(UC_X86_REG_ESP);n=vm.u32(sp+4)
                floating[:]=list(struct.iter_unpack('<3f4I2f',uc.mem_read(vm.u32(0x675fb4),n*36)))
            def packet(uc,*_):
                sp=uc.reg_read(UC_X86_REG_ESP);args=list(struct.unpack('<20i',uc.mem_read(sp+4,80)))
                actual=[args[1+i*5:6+i*5]for i in range(3)];w,h=args[18]+1,args[19]+1
                expected=[[int(v[0]),int(-v[1]),int(v[2]),int(v[7]*w),int(v[8]*h)]for v in floating]
                assert all(v in expected for v in actual),(actual,expected)
                packets.append(dict(tick=vm.u32(0x65cb38),floating=list(floating),integer=actual,texture_size=[w,h]))
            vm.uc.hook_add(UC_HOOK_CODE,transformed,begin=0x56d960,end=0x56d960)
            vm.uc.hook_add(UC_HOOK_CODE,packet,begin=0x54e050,end=0x54e050)
    native.LitSoftwareFixture=Observed
    ticks=totalframes+10;native.PHASES=tuple(range(1,ticks+1))
    try:current=native.run(output/'native',ticks,True,totalframes,step)
    finally:native.LitSoftwareFixture=base
    current=json.loads(json.dumps(current));old=json.loads(baseline.read_text());pixels=0
    assert len(old['rows'])==len(current['rows'])==ticks
    for a,b in zip(old['rows'],current['rows']):
        for key in ('tick','dead_no_calls','flags','scale','frame','draws'):assert a[key]==b[key],(a['tick'],key)
        if a['image']:
            assert a['image_sha256']==b['image_sha256'];pixels+=1
    report=dict(status='pass',accepted=False,visual_pass=False,totalframes=totalframes,step=step,frames=ticks,
                packets=packets,packet_count=len(packets),integer_channels=len(packets)*15,
                prior_sampled_pixels_identical=pixels,baseline_sha256=native.sha(baseline),
                native_sha256=native.sha(output/'native/manifest.json'),probe_sha256=native.sha(Path(__file__)),
                limits='Genuine public parameters, full software raster on every frame and read-only54e050 packet hooks. Prior complete owner/controller/draw traces and selected framebuffer samples remain unchanged. No source policy selection or exact GPU coverage claim.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print('PASS',ticks,len(packets),pixels,flush=True)

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--baseline',type=Path,required=True);p.add_argument('--totalframes',type=int,default=60);p.add_argument('--step',type=float,default=.04);a=p.parse_args();run(a.output,a.baseline,a.totalframes,a.step)
