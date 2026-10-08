"""Actual YFireBall original prefix and authored moving head/trail/spark subset.

The Y methods execute from their own retail addresses and use the exact green
asset. Compiled shared production prefix/submission is the candidate. Native
NPC-specific world behavior, growth, burst/ring and full backend remain open.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import zipfile
from fireball_spark_render_probe import SparkRenderFixture,float32
from fireball_spark_probe import compile_prefix
from fireball_head_probe import ROOT,port_driver,function
from fireball_trail_probe import position_tolerance
from software_probe import save_rgb565_png

ASSET='Imagery/Magic/Yfireball.I3D'
SHA='1f5f85d4477435b2c0bc4e6486e4a070795fa9366b18fb7fc7eb49392dedea98'
HEAD=0x294;SPARK=0x314;HEAD_TEX=0x1610;SPARK_TEX=0x21614


class YFixture(SparkRenderFixture):
    def __init__(self,executable,asset):
        super().__init__(executable,asset,spark_vertices=SPARK,spark_texture=SPARK_TEX,vertex_offset=HEAD,texture_offset=HEAD_TEX)
        self.head_address=0x514350;self.glow_address=0x514520;self.trail_address=0x513ed0
        self.animate_address=0x5133c0;self.animate_stop=0x5136f9
        self.commit_address=0x513ab0;self.commit_stop=0x513abb

    def all_pixels(self,cards,asset):
        self.surface.clear();last=None
        for card in cards:
            texture=card['texture']
            if texture!=last:
                if texture==2:self.surface.set_texture(64,64,asset[SPARK_TEX:SPARK_TEX+8192],format='ARGB4444')
                else:self.surface.set_texture(256,256,asset[HEAD_TEX:HEAD_TEX+131072],format='ARGB4444')
                last=texture
            vertices=[(*p,31,31,31,31,*uv)for p,uv in zip(self.surface.project(card['positions'],camera=self.camera),card['uvs'])]
            self.surface.draw(vertices,(2,0,3,1,3,0),True,False,instruction_limit=5000000)
        return self.vm.surface_bytes('screen')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=z.read(ASSET)
    if hashlib.sha256(asset).hexdigest()!=SHA:raise ValueError('Wrong Y authored asset')
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    if 'class TFireBallEffect_Bespoke__YFireBall : public TFireBallEffect'not in header:raise AssertionError('Y preview no longer shares verified producer')
    wrapper=function(source,'void TFireBallEffect_Bespoke__YFireBall::TickAndSubmitForTest_BESPOKE(')
    if 'TickAndSubmit(debug_mode);'not in wrapper:raise AssertionError('Y preview no longer delegates actual shared runtime')
    factory=function(source,'TFireBallEffect_Bespoke__YFireBall::SpawnForTest_BESPOKE(')
    if 'SpawnForTestWithAsset'not in factory or 'YFireBall.I3D'not in factory:raise AssertionError('Y asset factory changed')
    files=['src/effect.cpp','src/effect.h','src/fireballquad.cpp','src/fireballquad.h','src/fireballcore.h','src/fireballspark.h']
    source_sha={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest()for p in files}
    prefix,compiled=compile_prefix(args.output);render,vertices,render_compiled=port_driver(args.output,asset)
    vertices.write_bytes(asset[HEAD:HEAD+256]);fixture=YFixture(args.executable,asset)
    # Execute the Y leaf's original initializer independently of the selected
    # upstream animator .6 steady-head fixture, then retain exact defaults.
    fixture.vm.call(0x513100,this=fixture.missile.obj)
    defaults={hex(o):fixture.vm.u32(fixture.missile.obj+o)for o in (0xc,0x184,0x188,0x18c,0x194)}
    if defaults!={'0xc':0,'0x184':32768,'0x188':524288,'0x18c':1,'0x194':1}:raise AssertionError('Unexpected Y missile defaults')
    fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    native=[];motion=[]
    for tick in range(1,97):
        motion.append(fixture.missile.step(1)[-1]);fixture.advance_original_prefix();native.append(fixture.state())
    file=args.output/'motion.txt';file.write_text(''.join(' '.join(map(str,[m['state'],*m['position']]))+'\n'for m in motion))
    port=json.loads(subprocess.check_output([str(prefix),str(file),'1'],text=True));state_checks=0;state_errors=[]
    for tick,(a,b)in enumerate(zip(native,port),1):
        for key in('seed','explode','rng_calls'):
            state_checks+=1
            if a[key]!=b[key]:state_errors.append([tick,key])
        for group in('head','trail','sparks'):
            aa=[a[group]]if group=='head'else a[group];bb=[b[group]]if group=='head'else b[group]
            for slot,(u,v)in enumerate(zip(aa,bb)):
                for field in u:
                    x=u[field]if isinstance(u[field],list)else[u[field]];y=v[field]if isinstance(v[field],list)else[v[field]]
                    for rx,px in zip(x,y):
                        state_checks+=1;exact=(rx==px)if field in('used','life','flicker_status','rotation','frame')else struct.pack('<f',rx)==struct.pack('<f',px)
                        if not exact:state_errors.append([tick,group,slot,field,rx,px])
    fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    selected={1,2,5,10,20,37,50,60,61,62,65,69,70,72,80,81,96};cases=[];packet_checks=0;packet_errors=0;pixel_errors=0
    for tick in range(1,97):
        row=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if fixture.state()!=native[tick-1]or row!=motion[tick-1]:raise AssertionError('Native Y state warm replay changed')
        if tick not in selected:continue
        cards=fixture.spark_packets();trail=[dict(c,texture=1)for c in fixture.trail_packets()if c['scale']!=0]
        if row['state']in(0,1):
            h=native[tick-1]['head'];glow=fixture.packet(True,int(h['frame']),h['rotation'],h['scale'],h['glow'],0);head=fixture.packet(False,int(h['frame']),h['rotation'],h['scale'],h['glow'],0)
            cards+=[dict(glow,texture=1)]+trail+[dict(head,texture=1)]
        else:cards+=trail
        state=port[tick-1];trailfile=args.output/'trail.bin';trailfile.write_bytes(b''.join(struct.pack('<4fiff',*p['position'],p['scale'],p['rotation'],p['frame'],p['glow'])for p in state['trail']))
        sparkfile=args.output/'sparks.bin';sparkfile.write_bytes(b''.join(struct.pack('<7fiII',*p['position'],*p['velocity'],p['scale'],p['life'],p['used'],p['flicker_status'])for p in state['sparks']))
        h=state['head'];packet=json.loads(subprocess.check_output([str(render),str(vertices),*map(str,row['position']),'0',str(row['angle']),str(h['frame']),str(h['rotation']),str(h['scale']),str(h['glow']),str(trailfile),str(sparkfile)],text=True))
        if packet['particles']:raise AssertionError('Y tested subset unexpectedly falls back to particles')
        if row['state']in(0,1):candidate=packet['quads']
        else:candidate=[c for c in packet['quads']if c['texture']==2]+[c for c in packet['quads']if c['texture']==1][1:-1]
        candidate=[c for c in candidate if any(v!=c['positions'][0]for v in c['positions'][1:])]
        if len(cards)!=len(candidate):packet_errors+=1
        for a,b in zip(cards,candidate):
            packet_checks+=1
            if a['texture']!=b['texture']:packet_errors+=1
            for field in('positions','uvs'):
                for x,y in zip(a[field],b[field]):
                    for u,v in zip(x,y):
                        packet_checks+=1
                        if abs(u-v)>(position_tolerance(u)if field=='positions'else 1e-7):packet_errors+=1
        original=fixture.all_pixels(cards,asset);rendered=fixture.all_pixels(candidate,asset);count=960*540
        different=sum(x!=y for x,y in zip(struct.unpack('<'+'H'*count,original),struct.unpack('<'+'H'*count,rendered)));pixel_errors+=different
        save_rgb565_png(args.output/f'retail-{tick:03d}.png',original,960,540);save_rgb565_png(args.output/f'port-{tick:03d}.png',rendered,960,540)
        cases.append(dict(tick=tick,motion=row,native_cards=cards,port_cards=candidate,different_pixels=different,
            original_visible_pixels=sum(x!=0 for x in struct.unpack('<'+'H'*count,original)),retail_sha256=hashlib.sha256(original).hexdigest(),port_sha256=hashlib.sha256(rendered).hexdigest()))
    fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0,animator_present=True)
    retained={c['tick']:c for c in cases};warm_images=0
    for tick in range(1,97):
        row=fixture.missile.step(1)[-1];fixture.advance_original_prefix()
        if fixture.state()!=native[tick-1]or row!=motion[tick-1]:raise AssertionError('Second native Y warm state changed')
        if tick not in retained:continue
        cards=fixture.spark_packets();trail=[dict(c,texture=1)for c in fixture.trail_packets()if c['scale']!=0]
        if row['state']in(0,1):
            h=native[tick-1]['head'];glow=fixture.packet(True,int(h['frame']),h['rotation'],h['scale'],h['glow'],0);head=fixture.packet(False,int(h['frame']),h['rotation'],h['scale'],h['glow'],0)
            cards+=[dict(glow,texture=1)]+trail+[dict(head,texture=1)]
        else:cards+=trail
        if cards!=retained[tick]['native_cards']:raise AssertionError('Native Y packet replay changed')
        if hashlib.sha256(fixture.all_pixels(cards,asset)).hexdigest()!=retained[tick]['retail_sha256']:raise AssertionError('Native Y mixed texture replay changed')
        warm_images+=1
    if source_sha!={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest()for p in files}:raise AssertionError('Production changed during Y comparison')
    report=dict(status='pass'if not state_errors and not packet_errors and not pixel_errors else'differences_found',
        source_sha256=source_sha,compiled_state=compiled,compiled_submit=render_compiled,actual_y_preview_delegate=wrapper,actual_y_factory=factory,
        native_leaf_defaults=defaults,original_methods=['0x513100','0x510220','0x470920','0x5133c0..0x5136f9','0x513ab0..0x513abb','0x514350','0x514520','0x513ed0','0x50adb0','0x50b2b0'],
        asset=ASSET,asset_sha256=SHA,asset_offsets=dict(head=hex(HEAD),spark=hex(SPARK),head_texture=hex(HEAD_TEX),spark_texture=hex(SPARK_TEX)),
        source=[0,0,128],destination=[1000,0,128],moving_ticks=96,state_checks=state_checks,state_errors=state_errors,
        ordered_random_range_calls=sum(len(s['rng_calls'])for s in native),exact_original_state_warm_replay_ticks=96,
        packet_checks=packet_checks,packet_errors=packet_errors,pixel_pairs=len(cases),pixel_differences=pixel_errors,exact_original_packet_pixel_warm_replays=warm_images,cases=cases,
        viewport=[960,540],camera=[240,0,128],full_yfireball_accepted=False,
        scope='Original Y native functions execute with exact green asset and actual source/destination missile trajectory. Candidate compiles verified shared production prefix/submission; actual Y preview derives canonical FireBall and delegates TickAndSubmit with explicit Y asset factory. Steady .6 head, white/unlit, selected identity owner+Zstretch1.5, native ABS spark raw-to-common Z bridge retained. No Yhagoro hit/world binding, natural growth/burst/ring/damage/map reaping or independent device/Metal parity.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in('cases','actual_y_preview_delegate','actual_y_factory')},indent=2))
    if state_errors or packet_errors or pixel_errors:raise SystemExit(1)


if __name__=='__main__':main()
