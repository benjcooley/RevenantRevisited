#!/usr/bin/env python3
"""Original moving trail update/render, through range impact and history drain.

Original Animate common prefix runs; spark/ring/impact subsystems are excluded.
The oracle never transcribes trail equations. Original512150 full dispatch and
complete animator are separate gates from this substantial trail fixture.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EBX,UC_X86_REG_EIP,UC_X86_REG_ESP
from fireball_head_probe import HeadFixture,port_driver,particle_card,ROOT,ASSET_SHA,VERTEX_OFFSET,INDICES
from software_probe import save_rgb565_png


def position_tolerance(value):
    # Existing authored-transform contract accounts for x87 intermediates
    # versus native binary32 arithmetic: max(3e-5 absolute,8binary32ULPs).
    bits=struct.unpack('<I',struct.pack('<f',abs(value)))[0]
    current=struct.unpack('<f',struct.pack('<I',bits))[0]
    next_value=struct.unpack('<f',struct.pack('<I',bits+1))[0]
    return max(3e-5,8*(next_value-current))


class TrailFixture(HeadFixture):
    def __init__(self,executable,asset,**asset_offsets):
        self.raw=[];super().__init__(executable,asset,960,540,(240,0,128),**asset_offsets)
        self.animate_address=0x510e40;self.animate_stop=0x511179
        self.commit_address=0x511530;self.commit_stop=0x51153b;self.trail_address=0x511950
        vm=self.vm
        # Explicit excluded spark parameter/pulse methods do not consume RNG.
        table=vm.allocate(16);ret=vm.allocate(4);ret4=vm.allocate(4)
        vm.write(ret,b'\xc3');vm.write(ret4,b'\xc2\x04\x00')
        vm.put_u32(table+4,ret4);vm.put_u32(table+12,ret)
        vm.put_u32(self.animator+0x248,table)
        for address in (0x40e460,0x471b50,0x58c582):
            vm.uc.hook_add(UC_HOOK_CODE,self.animate_boundary,begin=address,end=address)
        vm.put_u32(self.animator+0x240,16)
        vm.write(self.animator+0x4b8,struct.pack('<fif',.6,0,0.))
        vm.write(self.animator+0x4c4,struct.pack('<f',1.5))
        self.surface.checkpoint()

    def animate_boundary(self,uc,address,size,user):
        vm=self.vm;sp=uc.reg_read(UC_X86_REG_ESP);cleanup=4
        if address==0x40e460:
            vm.put_u32(self.animator+0xc,vm.u32(self.missile.obj+0xc)&0xffff)
        elif address==0x58c582:
            uc.reg_write(UC_X86_REG_EAX,vm.random_msvc());cleanup=0
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)

    def boundary(self,uc,address,size,user):
        if address==0x40a8f0:
            self.raw.append(dict(matrix=bytes(self.vm.uc.mem_read(self.obj+0x58,64)).hex(),
                uvs=[struct.unpack('<2f',self.vm.uc.mem_read(self.lverts+i*32+24,8))for i in range(4)],
                scale=struct.unpack('<f',self.vm.uc.mem_read(self.obj+0x40,4))[0]))
        super().boundary(uc,address,size,user)

    def advance_original_prefix(self):
        # Actual shared instructions copy/decay ten slots, add owner XYZ,
        # sample glow RNG, advance/wrap frame and integer spin. Stop before
        # state-specific spark/burst/ring choreography, then execute original
        # state-history store separately. No copied update equations.
        self.vm.call(self.animate_address,(0,),this=self.animator,stop_address=self.animate_stop)
        self.vm.uc.reg_write(UC_X86_REG_EBX,self.animator)
        self.vm.call(self.commit_address,stop_address=self.commit_stop)

    def trail_state(self):
        records=[]
        for slot in range(10):
            raw=bytes(self.vm.uc.mem_read(self.animator+0xfc+slot*32,28))
            x,y,z,scale,rotation,frame,glow=struct.unpack('<4fiff',raw)
            records.append(dict(position=[x,y,z],scale=scale,rotation=rotation,frame=frame,glow=glow))
        return records

    def trail_packets(self):
        self.raw=[];self.vm.call(self.trail_address,this=self.animator)
        position=self.missile.inspect()['position']
        owner=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.5,0.,*map(float,position),1.]
        self.vm.write(self.owner_matrix,struct.pack('<16f',*owner));cards=[]
        for raw in self.raw:
            self.vm.write(self.obj+0x58,bytes.fromhex(raw['matrix']))
            self.vm.call(0x43aa90,(self.combined_matrix,self.obj+0x58,self.owner_matrix))
            points=[]
            for p in self.points:
                self.vm.write(self.point,struct.pack('<3f',*p))
                self.vm.call(0x43ad80,(self.combined_matrix,self.point,self.result))
                points.append(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12)))
            cards.append(dict(positions=points,uvs=raw['uvs'],count=4,scale=raw['scale']))
        return cards


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--producer',type=Path);parser.add_argument('--producer-manifest',type=Path)
    parser.add_argument('--output',type=Path,required=True);args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/newfireball.i3d')
    if hashlib.sha256(asset).hexdigest()!=ASSET_SHA:raise ValueError('Wrong authored asset')
    if args.producer:
        if not args.producer_manifest:parser.error('--producer requires --producer-manifest')
        previous=json.loads(args.producer_manifest.read_text());compiled=dict(previous['compiled_port']);binary=args.producer.resolve()
        if hashlib.sha256(binary.read_bytes()).hexdigest()!=compiled['binary_sha256']:raise ValueError('Retained producer changed')
        vertices=args.output/'authored-box01.bin';vertices.write_bytes(asset[VERTEX_OFFSET:VERTEX_OFFSET+128])
        compiled['retained_manifest']=str(args.producer_manifest.resolve())
    else:binary,vertices,compiled=port_driver(args.output,asset)
    sources=[ROOT/p for p in ('src/effect.cpp','src/effect.h','src/fireballquad.cpp','src/fireballquad.h','src/math3d.cpp')]
    source_hashes={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest()for p in sources}
    fixture=TrailFixture(args.executable,asset)
    fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    cases=[];geometry_errors=0;pixel_errors=0;checks=0;timings=[]
    selected={1,2,5,10,20,37,50,60,61,62,65,69,70,72}
    for tick in range(1,73):
        motion=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if tick not in selected:continue
        started=time.perf_counter();trail=fixture.trail_state();original=fixture.trail_packets()
        file=args.output/'trail-state.bin'
        file.write_bytes(b''.join(struct.pack('<4fiff',*p['position'],p['scale'],p['rotation'],p['frame'],p['glow'])for p in trail))
        # Capture actual production Submit branch in FLY to isolate its trail
        # adapter independently of the still-pending burst/spark components.
        data=json.loads(subprocess.check_output([str(binary),str(vertices),*[str(p)for p in motion['position']],
            '0',str(motion['angle']),'0','0','.6','1.5',str(file)],text=True))
        port=[particle_card(p)for p in data['particles']]+data['quads'][1:-1]
        # Zero-area draws produce no pixels; original retains their calls.
        original_visible=[c for c in original if c['scale']!=0]
        port_visible=[c for c in port if any(p!=c['positions'][0]for p in c['positions'][1:])]
        card_count_error=len(original_visible)!=len(port_visible)
        if card_count_error:geometry_errors+=1
        deltas=[]
        for a,b in zip(original_visible,port_visible):
            for kind in ('positions','uvs'):
                for p,q in zip(a[kind],b[kind]):
                    for x,y in zip(p,q):
                        checks+=1;deltas.append(abs(x-y))
                        if abs(x-y)>(position_tolerance(x) if kind=='positions' else 1e-7):geometry_errors+=1
        # Fixed camera at midrange and960x540 viewport retain actual late
        # trajectory/impact/drain pixels; the missile is never relocated.
        ref=fixture.pixels(original_visible);candidate=fixture.pixels(port_visible)
        pixels=fixture.surface.width*fixture.surface.height
        error=sum(a!=b for a,b in zip(struct.unpack('<'+'H'*pixels,ref),struct.unpack('<'+'H'*pixels,candidate)));pixel_errors+=error
        timings.append((time.perf_counter()-started)*1000)
        save_rgb565_png(args.output/f'retail-{tick:03d}.png',ref,fixture.surface.width,fixture.surface.height)
        save_rgb565_png(args.output/f'port-{tick:03d}.png',candidate,fixture.surface.width,fixture.surface.height)
        cases.append(dict(tick=tick,motion=motion,original_trail=trail,original_card_count=len(original),
            original_visible_cards=len(original_visible),port_visible_cards=len(port_visible),card_count_error=card_count_error,
            original_cards=original_visible,port_cards=port_visible,max_field_error=max(deltas,default=0),
            original_visible_pixels=sum(p!=0 for p in struct.unpack('<'+'H'*pixels,ref)),
            pixel_difference=error,retail_sha256=hashlib.sha256(ref).hexdigest(),port_sha256=hashlib.sha256(candidate).hexdigest()))
    if cases[-1]['original_card_count']!=0:raise AssertionError('Original trail did not drain after impact')
    fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    retained={c['tick']:c for c in cases};replay_checks=0
    for tick in range(1,73):
        motion=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if tick not in retained:continue
        prior=retained[tick]
        if motion!=prior['motion']or fixture.trail_state()!=prior['original_trail']:
            raise AssertionError('Original trail state failed warm replay')
        cards=fixture.trail_packets();visible=[c for c in cards if c['scale']!=0]
        if hashlib.sha256(fixture.pixels(visible)).hexdigest()!=prior['retail_sha256']:
            raise AssertionError('Original trail pixels failed warm replay')
        replay_checks+=1
    if source_hashes!={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest()for p in sources}:
        raise AssertionError('Production source changed during trail comparison')
    report=dict(status='pass'if not geometry_errors and not pixel_errors else'differences_found',
        original_functions=['0x510220','0x470920','0x510e40..0x511179','0x511530..0x51153b','0x511950','0x43aa90','0x43ad80','0x56d960'],
        compiled_port=compiled,cases=cases,geometry_errors=geometry_errors,pixel_differences=pixel_errors,field_checks=checks,
        source_sha256=source_hashes,viewport=[960,540],camera=[240,0,128],
        exact_original_warm_replays=replay_checks,
        source_destination=[[0,0,128],[1000,0,128]],moving_tick_count=72,range_impact_tick=61,trail_drained=True,
        median_pair_ms=sorted(timings)[len(timings)//2],host_trajectory_forcing=False,full_animator=False,
        float_position_bound='max(3e-5 absolute,8binary32ULPs), established authored-transform bound',
        strict_absolute_earlier_misses='4coordinates differed by one binary32ULP at worldX~380..430; retained strict_absolute_manifest.json',
        scope='Original moving missile through range impact and real trail history drain. Original Animate common prefix and trail renderer execute; '
            'spark callbacks isolated/noRNG, state-specific burst/ring behavior excluded, steady head scale.6 selected. '
            'Actual production Submit body receives original trail state for strict rendering comparison; trail simulation port equivalence is a separate gate. '
            'Fixed camera240,0,128 and960x540viewport retain actual impact/drain visibility. Shared original raster, not Metal/backend parity.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if geometry_errors or pixel_errors:raise SystemExit(1)


if __name__=='__main__':main()
