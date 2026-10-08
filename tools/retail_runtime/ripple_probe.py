#!/usr/bin/env python3
"""Original no-splash Ripple versus compiled production effect and SW pixels."""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='86639bfb7a7880a717ae0f67bbda270cc13e730eff0fe85fcc14d82d8b49df07'
VERTEX_OFFSET=0x144;INDEX_OFFSET=0x244;TEXTURE_OFFSET=0x4f0
INDICES=(0,1,3,3,2,0)
LENGTHS=(4,20,24)

def sha(data):return hashlib.sha256(data).hexdigest()

def source_span(source,start,end):
    first=source.index(start);return source[first:source.index(end,first)].rstrip()


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(helpers=source_span(source,'constexpr double kWaterTicksPerSecond','\nT3DImagery* LoadWaterImagery'),
        submit=source_span(source,'void SubmitWaterQuad(','\ntemplate <typename Effect>'),
        methods=source_span(source,'void TRippleEffect::Advance(double seconds)','\nvoid TRippleEffect::TickAndSubmitForTest'),
        declaration=source_span(header,'class TRippleEffect :','\n// *******************\n// * Ripple Animator'),
        math_cpp=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset):
    pieces=production_spans()
    helpers,submit,methods,declaration=(pieces[k] for k in ('helpers','submit','methods','declaration'))
    # Expose fixture inputs/state while leaving all compiled method bodies intact.
    declaration=declaration.replace('  private:','  public:')
    prelude=r'''
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t {Alpha,Additive};
enum class EFxDepthMode:uint8_t {TestNoWrite};
enum class EFxDebugMode:uint8_t {Normal,SolidColor,FullTexture};
enum class EFxLightMode:uint8_t {Lit,Unlit};
struct SQuadDrawItem {float world_pos[4][3]{};float uv[4][2]{};
 struct {TTextureHandle texture{};uint8_t blend{},depth_mode{};} key;
 EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer {std::vector<SQuadDrawItem> draws;void SubmitFxQuad(const SQuadDrawItem& i){draws.push_back(i);}};
TRenderer renderer;TRenderer* Renderer=&renderer;
struct TObjectImagery {};struct SObjectDef {};
struct World {hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4& Matrix()const{return m;}};
struct TObjectInstance {World world;S3DPoint pos;virtual ~TObjectInstance()=default;
 const World& Transform()const{return world;}const S3DPoint& Pos()const{return pos;}};
struct TEffect:TObjectInstance {TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};
 virtual void Pulse(){};};
'''
    records=struct.unpack_from('<32f',asset,VERTEX_OFFSET)
    initializer=','.join('{{'+','.join(format(v,'.9g')+'f' if '.' in format(v,'.9g') or 'e' in format(v,'.9g') else format(v,'.9g')+'.0f' for v in records[i:i+3])+'},{0,0,1},0,0}' for i in range(0,32,8))
    trailer=r'''
void TRippleEffect::Pulse(){}
TRippleEffect* TRippleEffect::SpawnForTest(const S3DPoint&){throw std::runtime_error("Unexpected child spawn in no-splash fixture");}
int main(){
    for(int length:{4,20,24}){
        TRippleEffect effect(nullptr);effect.SetLength(length);
        S3DVertex raw[]={VERTICES};effect.ring_vertices_.assign(raw,raw+4);effect.ring_texture_=1;
        for(int tick=0;tick<=length+2;++tick){
            renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
            std::printf("%d %d %d %d %d %zu",length,tick,effect.frameon_,effect.ripframe_,int(effect.IsAlive()),renderer.draws.size());
            if(!renderer.draws.empty())for(int i=0;i<4;++i){const auto& d=renderer.draws[0];
                std::printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);}
            std::puts("");effect.Advance(1.0/24.0);
        }
    }
}
'''.replace('VERTICES',initializer)
    generated=output/'ripple-port-component.cpp'
    generated.write_text(prelude+'\n'+declaration+'\n'+helpers+'\n'+submit+'\n'+methods+'\n'+trailer)
    binary=output/'ripple-port-component'
    command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),
             str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Compiled Ripple production bodies failed; see port-compile.log')
    rows={}
    for line in subprocess.check_output([str(binary)],text=True).splitlines():
        values=[float(x) for x in line.split()];length,tick=map(int,values[:2]);rows[(length,tick)]=values
        if len(values)!=(26 if values[5] else 6):raise AssertionError('Unexpected port packet trace')
    return rows,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        production_methods=['TRippleEffect::Advance','TRippleEffect::Submit','SubmitWaterQuad','AdvanceWaterTime'],
        boundaries=['exact asset vertex input','identity owner matrix','renderer captures production SubmitFxQuad',
                    'length<=24 excludes random splash/recursive child branch','child spawn rejects execution'])


class RippleFixture:
    def __init__(self,executable,asset):
        if sha(asset)!=ASSET_SHA:raise ValueError('Unsupported Ripple asset revision')
        if struct.unpack_from('<6H',asset,INDEX_OFFSET)!=INDICES:raise ValueError('Wrong triangle topology')
        # Follow the self-relative pointer field and frame pointer, not a PNG.
        frame_offsets=0x468+struct.unpack_from('<I',asset,0x468)[0]
        texture=frame_offsets+struct.unpack_from('<I',asset,frame_offsets)[0]
        if texture!=TEXTURE_OFFSET or struct.unpack_from('<2I',asset,0x404)!=(256,256):raise ValueError('Wrong atlas layout')
        self.software=SoftwareFixture(executable,128,128);self.vm=self.software.vm
        self.software.set_texture(256,256,asset[texture:texture+131072],format='ARGB4444')
        self.points=[struct.unpack_from('<3f',asset,VERTEX_OFFSET+i*32) for i in range(4)]
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.obj=self.vm.allocate(0x200)
        self.lverts=self.vm.allocate(128);self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.vm.allocate(0x200))
        self.vm.put_u32(self.obj+0xa0,4);self.vm.put_u32(self.obj+0xa4,self.lverts)
        for i,xyz in enumerate(self.points):self.vm.write(self.lverts+i*32,struct.pack('<3f3I2f',*xyz,0,0xffffffff,0,0,0))
        owner_vtable=self.vm.allocate(0x204);self.vm.put_u32(self.owner,owner_vtable)
        # Explicit owner interface adapters, with real x86 stack cleanup.
        get_length=self.vm.allocate(16);self.vm.write(get_length,b'\x8b\x81\x00\x03\x00\x00\xc3')
        set_flags=self.vm.allocate(16);self.vm.write(set_flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00')
        command_done=self.vm.allocate(16);self.vm.write(command_done,b'\x31\xc0\xc2\x04\x00')
        for offset,address in [(0x200,get_length),(0x40,set_flags),(0x158,command_done)]:self.vm.put_u32(owner_vtable+offset,address)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40a0c0:8,0x4178e0:0,0x417d60:8,
                         0x417b00:0,0x40eef0:4,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        self.calls={}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);self.calls[hex(address)]=self.calls.get(hex(address),0)+1
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Unexpected splash object in no-splash fixture')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if (self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Unexpected Ripple blend state')
        elif address==0x40a8f0:
            if self.vm.u32(sp+4)!=self.obj:raise AssertionError('Wrong Ripple submission')
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def packet(self,length,tick):
        if length not in LENGTHS:raise ValueError('Only verified no-splash lengths are supported')
        self.software.restore();self.calls={};self.vm.put_u32(self.owner+0x300,length)
        self.vm.call(0x4f0780,this=self.animator)
        for _ in range(tick):
            # The object lifecycle removes OF_KILL objects. Do not call an
            # already killed retail animator merely to satisfy a fixture tick.
            if self.vm.u32(self.owner+8)&0x1000:break
            self.vm.call(0x4f08b0,this=self.animator)
        alive=not bool(self.vm.u32(self.owner+8)&0x1000)
        state=dict(frameon=self.vm.u32(self.animator+0x104),ripframe=self.vm.u32(self.animator+0x100),
                   scale=struct.unpack('<f',self.vm.uc.mem_read(self.animator+0x10c,4))[0],alive=alive,
                   splash_count=self.vm.u32(self.animator+0x108))
        if not alive:return state,[],[]
        self.vm.call(0x4f0c10,this=self.animator);positions=[]
        for xyz in self.points:
            self.vm.write(self.point,struct.pack('<3f',*xyz));self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result))
            positions.append(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12)))
        uvs=[struct.unpack('<2f',self.vm.uc.mem_read(self.lverts+i*32+24,8)) for i in range(4)]
        return state,positions,uvs

    def pixels(self,positions,uvs):
        self.software.clear()
        if not positions:return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth'),[]
        projected=self.software.project(positions,camera=(0,0,0),zdist=1925)
        vertices=[(*p,31,31,31,31,*uv) for p,uv in zip(projected,uvs)]
        color,depth=self.software.draw(vertices,INDICES,z_enabled=True,z_write=False)
        return color,depth,projected


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--repeat',type=int,default=2);args=parser.parse_args()
    if args.repeat<2:parser.error('--repeat must be at least two')
    args.output.mkdir(parents=True,exist_ok=True)
    paths=[ROOT/p for p in ('src/effect.cpp','src/effect.h','src/math3d.cpp','src/math3d.h')]
    before={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in paths}
    before_spans={name:sha(text.encode()) for name,text in production_spans().items()}
    with zipfile.ZipFile(args.archive) as archive:asset=archive.read('Imagery/Magic/ripples.i3d')
    port_rows,compiled=build_port(args.output,asset)
    start=time.perf_counter();fixture=RippleFixture(args.executable,asset);setup_ms=(time.perf_counter()-start)*1000
    cases=[];timings=[];failures=[]
    for length in LENGTHS:
        for tick in range(length+3):
            row=port_rows[(length,tick)];first=None
            for repeat in range(args.repeat):
                start=time.perf_counter();state,positions,uvs=fixture.packet(length,tick)
                retail,retail_depth,projected=fixture.pixels(positions,uvs)
                port_positions=[row[6+i*5:9+i*5] for i in range(4)] if row[5] else []
                port_uvs=[row[9+i*5:11+i*5] for i in range(4)] if row[5] else []
                port,port_depth,port_projected=fixture.pixels(port_positions,port_uvs)
                timings.append((time.perf_counter()-start)*1000)
                pair=(sha(retail),sha(port),sha(retail_depth),sha(port_depth))
                if first is None:first=pair
                elif first!=pair:raise AssertionError('Ripple pixels/depth failed exact warm replay')
                if repeat==0:
                    position_error=max([abs(a-b) for p,q in zip(positions,port_positions) for a,b in zip(p,q)] or [0])
                    uv_error=max([abs(a-b) for p,q in zip(uvs,port_uvs) for a,b in zip(p,q)] or [0])
                    diff=sum(a!=b for a,b in zip(struct.unpack('<16384H',retail),struct.unpack('<16384H',port)))
                    state_equal=(state['frameon'],state['ripframe'],state['alive'])==(int(row[2]),int(row[3]),bool(row[4]))
                    if not state_equal or position_error>2e-5 or uv_error>1e-7 or diff:
                        failures.append(dict(length=length,tick=tick,state_equal=state_equal,max_position_error=position_error,
                                             max_uv_error=uv_error,differing_rgb565_pixels=diff))
                    save_rgb565_png(args.output/f'retail-L{length:02d}-T{tick:02d}.png',retail,128,128)
                    save_rgb565_png(args.output/f'port-L{length:02d}-T{tick:02d}.png',port,128,128)
                    cases.append(dict(length=length,tick=tick,original_state=state,port_state=dict(frameon=int(row[2]),ripframe=int(row[3]),alive=bool(row[4])),
                        original_vertices=positions,port_vertices=port_positions,original_uvs=uvs,port_uvs=port_uvs,
                        max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff,
                        original_projected=projected,port_projected=port_projected,retail_rgb565_sha256=pair[0],port_rgb565_sha256=pair[1],
                        retail_depth_sha256=pair[2],port_depth_sha256=pair[3],nonzero_retail_pixels=sum(p!=0 for p in struct.unpack('<16384H',retail)),
                        external_call_counts=dict(fixture.calls)))
    after={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in paths}
    after_spans={name:sha(text.encode()) for name,text in production_spans().items()}
    if after_spans!=before_spans:raise AssertionError('Compiled Ripple production bodies changed during probe')
    report=dict(status='pass' if not failures else 'fail',failures=failures,retail_sha256=RETAIL_SHA,
        asset_member='Imagery/Magic/ripples.i3d',asset_sha256=sha(asset),asset_bytes=len(asset),vertex_offset=hex(VERTEX_OFFSET),
        index_offset=hex(INDEX_OFFSET),texture_offset=hex(TEXTURE_OFFSET),texture_sha256=sha(asset[TEXTURE_OFFSET:TEXTURE_OFFSET+131072]),
        source_sha256=after,source_span_sha256=after_spans,
        source_contract='Exact compiled production bodies/helpers; whole-file hashes are provenance, not unrelated-edit invalidation.',
        probe_sha256=sha(Path(__file__).read_bytes()),port_component=compiled,
        registration='0x4f0760 -> builder0x5ac5ac -> factory0x4f8df0 -> animatorvtable0x5ac5b0',
        original_initialize='0x4f0780',original_animate='0x4f08b0',original_render='0x4f0c10',original_raster='0x56d960',
        lengths=list(LENGTHS),case_count=len(cases),replays_per_case=args.repeat,setup_ms=setup_ms,median_warm_pair_ms=statistics.median(timings),
        p95_warm_pair_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],guest_os_boots=0,dosbox_used=False,
        pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,ledger_acceptance_changed=False,
        camera=dict(position=[0,0,0],zdist=1925,viewport=[128,128],owner_world='identity'),
        scope='Actual original no-splash Ripple Initialize/Animate/Render and exact atlas vs compiled production Advance/Submit on authored tick boundaries. Shared original projection/software raster isolates effect state, lifespan, scale, UV and geometry. Owner/task/baseanimator/imagery/extents interfaces are explicit fixtures. No RNG, splash children, map lighting/background or modern GPU comparison.',cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='cases'},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
