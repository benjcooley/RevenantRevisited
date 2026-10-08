#!/usr/bin/env python3
"""Original Fizzle burst versus compiled production state/mesh and SW pixels.

Native RNG is observed, not replaced. The compiled port consumes those same
explicit random inputs with strict range/order checks. Both frontends submit
raw local particle Z: actual retail Fizzle passes abs_pos=0 to particle Render.
"""
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
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EBP,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA

ROOT=Path(__file__).resolve().parents[2]
WIDTH=192;HEIGHT=384;PIXELS=WIDTH*HEIGHT
ASSET_SHA='35588dc99f7a0b1b1cd52e0a783b02b3ef6c8ff9a8fd34b60fb575d6466d9cd4'
SYSTEM_OBJECTS=(0,2,1)
INDICES=(2,0,3,1,3,0)

def sha(data):return hashlib.sha256(data).hexdigest()
def span(source,start,end):
    first=source.index(start);return source[first:source.index(end,first)].rstrip()
def f32(value):return struct.pack('<f',value)


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(constants=span(source,'constexpr double kFizzleSimTickMs','// Resolve which texture slot a sub-object'),
        slot=span(source,'int32_t FizzleFindFreeSlot','}   // namespace'),
        method=span(source,'void TFizzleEffect::TickAndSubmitForTest','// *************************************************************************\n// * TRippleEffect'),
        types=span(header,'inline constexpr int32_t kFizzleParticlesPerSystem','// ********************\n// * TFireBallEffect'),
        math_cpp=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset,inputs):
    pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
    prelude=r'''
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t {Alpha,Additive};
enum class EFxDepthMode:uint8_t {TestNoWrite};
enum class EFxDebugMode:uint8_t {Normal,SolidColor,FullTexture};
enum class EFxLightMode:uint8_t {Lit,Unlit};
struct SQuadDrawItem {float world_pos[4][3]{};float uv[4][2]{};float color_rgba[4]{};
 struct {TTextureHandle texture{};uint8_t blend{},depth_mode{};} key;
 EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer {std::vector<SQuadDrawItem> draws;void SubmitFxQuad(const SQuadDrawItem& i){draws.push_back(i);}};
TRenderer renderer;TRenderer* Renderer=&renderer;
struct TObjectImagery {};struct SObjectDef {};
struct TEffect {S3DPoint position{};TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};
 virtual ~TEffect()=default;const S3DPoint& Pos()const{return position;}};
struct TTime {static double dt;static double DeltaTime(){return dt;}};double TTime::dt=0;
std::ifstream random_inputs;int random_count=0;
int random(int low,int high){int a,b,value;if(!(random_inputs>>a>>b>>value)||a!=low||b!=high)
 throw std::runtime_error("Native/ported RNG call range/order mismatch");++random_count;return value;}
'''
    initializer=[]
    for obj in SYSTEM_OBJECTS:
        data=struct.unpack_from('<32f',asset,0x360+obj*128)
        def literal(v):
            s=format(v,'.9g');return s+'f' if '.' in s or 'e' in s else s+'.0f'
        initializer.append('{'+','.join('{{'+','.join(literal(v) for v in data[i:i+3])+'},{0,0,1},'+literal(data[i+6])+','+literal(data[i+7])+'}' for i in range(0,32,8))+'}')
    trailer=r'''
int main(int argc,char**argv){random_inputs.open(argv[1]);if(!random_inputs)return 2;
 TFizzleEffect effect(nullptr);S3DVertex vertices[3][4]={VERTICES};
 for(int s=0;s<3;++s){effect.subobjs_[s].texture=s+1;std::memcpy(effect.subobjs_[s].vertices,vertices[s],sizeof(vertices[s]));}
 for(int tick=0;tick<40;++tick){TTime::dt=tick?1.0/24.0:0;renderer.draws.clear();
  effect.TickAndSubmitForTest(EFxDebugMode::Normal);
  std::printf("S %d %d %d %.9g %d %zu\n",tick,int(effect.alive_),effect.frame_count_,effect.emit_add_,random_count,renderer.draws.size());
  for(int i=0;i<90;++i){const auto&p=effect.particles_[i];
   std::printf("P %d %d %d %d %d",tick,i,int(p.used),p.phase,int(p.flicker));
   std::printf(" %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",p.pos.X,p.pos.Y,p.pos.Z,
    p.vel.X,p.vel.Y,p.vel.Z,p.scl.X,p.scl.Y,p.scl.Z,p.rot_z_deg,p.spin_rate,p.max_scl);}
  for(const auto&d:renderer.draws){std::printf("D %d %u",tick,unsigned(d.key.texture));
   for(int i=0;i<4;++i)std::printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);std::puts("");}
 }
 int a,b,v;if(random_inputs>>a>>b>>v)throw std::runtime_error("Port failed to consume all native RNG inputs");
}
'''.replace('VERTICES',','.join(initializer))
    generated=output/'fizzle-port-component.cpp'
    generated.write_text(prelude+'\n'+types+'\n'+pieces['constants']+'\n'+pieces['slot']+'\n'+pieces['method']+'\n'+trailer)
    binary=output/'fizzle-port-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),
        '-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Fizzle production-body compile failed; see port-compile.log')
    trace=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True)
    (output/'port-trace.txt').write_text(trace.stdout);(output/'port-run.log').write_text(trace.stderr)
    if trace.returncode:raise RuntimeError('Production Fizzle trace failed; see port-run.log')
    frames={}
    for line in trace.stdout.splitlines():
        fields=line.split();tag=fields[0];tick=int(fields[1])
        if tag=='S':frames[tick]=dict(alive=bool(int(fields[2])),frame_count=int(fields[3]),add=float(fields[4]),random_count=int(fields[5]),particles=[],draws=[])
        elif tag=='P':frames[tick]['particles'].append(dict(index=int(fields[2]),used=bool(int(fields[3])),phase=int(fields[4]),flicker=bool(int(fields[5])),values=[float(v) for v in fields[6:]]))
        elif tag=='D':frames[tick]['draws'].append(dict(system=int(fields[2])-1,positions=[[float(v) for v in fields[3+i*5:6+i*5]] for i in range(4)],uvs=[[float(v) for v in fields[6+i*5:8+i*5]] for i in range(4)]))
    return frames,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        compiled_production_bodies=['TFizzleEffect::TickAndSubmitForTest','FizzleFindFreeSlot','src/math3d.cpp'],
        boundaries=['exact asset vertices/UVs/texture handles','renderer captures production SubmitFxQuad',
                    'time steps exactly 1/24 seconds','RNG consumes unchanged retail outputs with strict range/order validation'])


class FizzleFixture:
    def __init__(self,executable,asset):
        if sha(asset)!=ASSET_SHA:raise ValueError('Unsupported Fizzle asset revision')
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT);self.vm=self.software.vm
        self.asset=asset;self.points=[];self.uvs=[];self.textures=[]
        for obj in range(3):
            records=struct.unpack_from('<32f',asset,0x360+obj*128)
            self.points.append([records[i:i+3] for i in range(0,32,8)]);self.uvs.append([records[i+6:i+8] for i in range(0,32,8)])
            desc=0x738+obj*120;pointer=desc+108+struct.unpack_from('<I',asset,desc+108)[0]
            bits=pointer+struct.unpack_from('<I',asset,pointer)[0]
            if struct.unpack_from('<2I',asset,desc+8)!=(32,32):raise ValueError('Wrong texture dimensions')
            surface=self.vm.surface(f'fizzle_texture_{obj}',32,32);self.vm.write(surface.address,asset[bits:bits+2048]);self.textures.append(surface)
        self.software.set_texture(32,32,asset[0x8a4:0x8a4+2048],format='ARGB4444')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);vtable=self.vm.allocate(0x204)
        self.vm.put_u32(self.owner,vtable);self.vm.put_u32(self.animator,0x5ad9d4)
        self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.vm.allocate(0x200))
        done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00')
        flags=self.vm.allocate(16);self.vm.write(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00')
        self.vm.put_u32(vtable+0x158,done);self.vm.put_u32(vtable+0x40,flags)
        self.objects=[self.vm.allocate(0x200) for _ in range(3)]
        self.systems=[]
        for offset in (0xfc,0x124,0x14c):
            system=self.animator+offset;self.systems.append(system)
            self.vm.put_u32(system,0x5a96d8);self.vm.put_u32(system+4,self.vm.call(0x482fb0,(30*88,)));self.vm.put_u32(system+0x1c,30)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.calls={};self.packets=[];self.random=[];self.pending_random=[]
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,
                         0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x48332c):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_particle_render,begin=0x50c220,end=0x50c220)
        self.vm.call(0x4f3f90,this=self.animator)
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);self.calls[hex(address)]=self.calls.get(hex(address),0)+1
        if address==0x40eef0:
            obj=self.vm.u32(sp+4)
            if obj not in range(3):raise AssertionError('Wrong Fizzle object request')
            uc.reg_write(UC_X86_REG_EAX,self.objects[obj])
        elif address==0x417d60:
            if (self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Unexpected Fizzle blend')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4);object_index=self.objects.index(obj)
            self.packets.append(dict(system=SYSTEM_OBJECTS.index(object_index),slot=uc.reg_read(UC_X86_REG_EBP)//88,
                matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def observe_random(self,uc,address,size,user):
        if address==0x483300:
            sp=uc.reg_read(UC_X86_REG_ESP);self.pending_random.append(struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8)))
        else:
            result=struct.unpack('<i',struct.pack('<I',uc.reg_read(UC_X86_REG_EAX)))[0]
            self.random.append((*self.pending_random.pop(),result))

    def observe_particle_render(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if struct.unpack('<2I',self.vm.uc.mem_read(sp+4,8))!=(1,0):
            raise AssertionError('Fizzle particle Render changed flicker/abs_pos arguments')

    def reset(self):
        self.software.restore();self.calls={};self.packets=[];self.random=[];self.pending_random=[]

    def state(self):
        particles=[]
        for s,system in enumerate(self.systems):
            base=self.vm.u32(system+4)
            for slot in range(30):
                address=base+slot*88;raw=bytes(self.vm.uc.mem_read(address,88))
                values=struct.unpack_from('<3f',raw,0)+struct.unpack_from('<3f',raw,0x30)+struct.unpack_from('<3f',raw,0x0c)
                values+=struct.unpack_from('<f',raw,0x20)+struct.unpack_from('<2f',raw,0x40)
                particles.append(dict(index=s*30+slot,used=bool(struct.unpack_from('<I',raw,0x54)[0]),
                    phase=struct.unpack_from('<I',raw,0x50)[0],flicker=bool(struct.unpack_from('<I',raw,0x48)[0]),values=list(values)))
        return dict(alive=not bool(self.vm.u32(self.owner+8)&0x1000),frame_count=self.vm.u32(self.animator+0x178),
            add=struct.unpack('<f',self.vm.uc.mem_read(self.animator+0x174,4))[0],random_count=len(self.random),particles=particles)

    def animate(self):
        if self.state()['alive']:self.vm.call(0x4f4050,this=self.animator)

    def original_draws(self):
        self.packets=[]
        if self.state()['alive']:self.vm.call(0x4f4440,this=self.animator)
        draws=[]
        for packet in self.packets:
            object_index=SYSTEM_OBJECTS[packet['system']];matrix=self.vm.allocate(64);self.vm.write(matrix,struct.pack('<16f',*packet['matrix']))
            positions=[]
            for point in self.points[object_index]:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(matrix,self.point,self.result))
                positions.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(system=packet['system'],slot=packet['slot'],positions=positions,uvs=self.uvs[object_index],matrix=packet['matrix']))
        return draws

    def identify_draws(self,draws,state):
        active=[p for p in state['particles'] if p['used'] and p['values'][6]>0]
        if len(active)!=len(draws):raise AssertionError('Unexpected production draw count')
        identified=[]
        for draw,particle in zip(draws,active):
            if draw['system']!=particle['index']//30:raise AssertionError('Production system traversal mismatch')
            identified.append(dict(draw,slot=particle['index']%30))
        return identified

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            self.software.texture=self.textures[SYSTEM_OBJECTS[draw['system']]]
            self.vm.call(0x56d3a0,(draw['system']+1,self.software.texture_object))
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT) for p in projected):
                raise AssertionError('Selected Fizzle fixture viewport clips authored geometry')
            vertices=[(*p,31,31,31,31,*uv) for p,uv in zip(projected,draw['uvs'])]
            self.software.draw(vertices,INDICES,z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--repeat',type=int,default=2);args=parser.parse_args()
    if args.repeat<2:parser.error('--repeat must be at least two')
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode()) for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive) as archive:asset=archive.read('Imagery/Magic/fizzle.i3d')
    start=time.perf_counter();fixture=FizzleFixture(args.executable,asset);setup_ms=(time.perf_counter()-start)*1000
    # Capture the actual complete native random stream once, without replacing it.
    fixture.reset()
    for tick in range(1,40):fixture.animate()
    random=fixture.random.copy();random_file=args.output/'native-random-inputs.txt'
    random_file.write_text(''.join(f'{a} {b} {v}\n' for a,b,v in random))
    port,compiled=build_port(args.output,asset,random_file)
    timings=[];cases=[];failures=[];first_hashes={}
    for repeat in range(args.repeat):
        fixture.reset()
        for tick in range(40):
            start=time.perf_counter()
            if tick:fixture.animate()
            original=fixture.state();ported=port[tick]
            original_draws=fixture.original_draws();port_draws=fixture.identify_draws(ported['draws'],ported)
            retail,retail_depth=fixture.pixels(original_draws);modern,modern_depth=fixture.pixels(port_draws)
            timings.append((time.perf_counter()-start)*1000);pair=(sha(retail),sha(modern),sha(retail_depth),sha(modern_depth))
            if repeat==0:first_hashes[tick]=pair
            elif pair!=first_hashes[tick]:raise AssertionError('Native Fizzle warm replay changed pixels/depth')
            if repeat:continue
            state_errors=[]
            for name in ('alive','frame_count','random_count'):
                if original[name]!=ported[name]:state_errors.append(name)
            if f32(original['add'])!=f32(ported['add']):state_errors.append('emission accumulator')
            for a,b in zip(original['particles'],ported['particles']):
                if a['used']!=b['used']:state_errors.append(f'particle{a["index"]}.used')
                if not a['used']:continue
                for field in ('phase','flicker'):
                    if a[field]!=b[field]:state_errors.append(f'particle{a["index"]}.{field}')
                for field,(x,y) in enumerate(zip(a['values'],b['values'])):
                    if f32(x)!=f32(y):state_errors.append(f'particle{a["index"]}.float{field}')
            nonzero_original=[draw for draw in original_draws if original['particles'][draw['system']*30+draw['slot']]['values'][6]>0]
            geometry_error=max([abs(x-y) for a,b in zip(nonzero_original,port_draws) for p,q in zip(a['positions'],b['positions']) for x,y in zip(p,q)] or [0])
            uv_error=max([abs(x-y) for a,b in zip(nonzero_original,port_draws) for p,q in zip(a['uvs'],b['uvs']) for x,y in zip(p,q)] or [0])
            diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(PIXELS)+'H',retail),struct.unpack('<'+str(PIXELS)+'H',modern)))
            if state_errors or geometry_error>3e-5 or uv_error>1e-7 or diff:
                failures.append(dict(tick=tick,state_errors=state_errors,max_position_error=geometry_error,max_uv_error=uv_error,differing_rgb565_pixels=diff))
            save_rgb565_png(args.output/f'retail-{tick:02d}.png',retail,WIDTH,HEIGHT)
            save_rgb565_png(args.output/f'port-{tick:02d}.png',modern,WIDTH,HEIGHT)
            cases.append(dict(tick=tick,original_state=original,port_state=ported,
                original_draws=original_draws,port_draws=port_draws,
                original_submissions=len(original_draws),port_submissions=len(port_draws),
                state_errors=state_errors,max_position_error=geometry_error,max_uv_error=uv_error,
                differing_rgb565_pixels=diff,retail_rgb565_sha256=pair[0],port_rgb565_sha256=pair[1],
                retail_depth_sha256=pair[2],port_depth_sha256=pair[3],nonzero_retail_pixels=sum(v!=0 for v in struct.unpack('<'+str(PIXELS)+'H',retail))))
        if fixture.random!=random:raise AssertionError('Original RNG did not replay exactly')
    after={k:sha(v.encode()) for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production Fizzle source spans changed during A/B')
    report=dict(status='pass' if not failures else 'fail',failures=failures,retail_sha256=RETAIL_SHA,
        asset_member='Imagery/Magic/fizzle.i3d',asset_sha256=sha(asset),asset_bytes=len(asset),
        vertex_offsets=['0x360','0x3e0','0x460'],texture_offsets=['0x8a4','0x10a8','0x18ac'],
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_component=compiled,
        original_initialize='0x4f3f90',original_animate='0x4f4050',original_render='0x4f4440',
        original_particle_functions=['0x50c170','0x50c1b0','0x50c220'],original_random='0x483300',
        original_rng_calls=len(random),native_random_input_sha256=sha(random_file.read_bytes()),
        case_count=len(cases),replays_per_case=args.repeat,setup_ms=setup_ms,median_warm_frame_pair_ms=statistics.median(timings),
        p95_warm_frame_pair_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],
        camera=dict(position=[0,0,0],zdist=1925,viewport=[WIDTH,HEIGHT],owner_world='identity',all_authored_corners_inside_viewport=True),
        coordinate_policy='Actual original Fizzle calls TParticleSystem::Render(flicker=1,abs_pos=0); the binary skips its conditional FIX_Z_VALUE branch. Raw local particle Z is shared by original and production port; no compensating coordinate conversion is applied.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,
        scope='Actual original complete one-shot Fizzle particle birth/motion/scale/spin/flicker/reap/death and original mesh/raster vs compiled current production effect, exact shipped textures, actual original RNG replayed as common inputs. Base animator, owner task/flags, imagery/extents/mesh submission are explicit boundaries. No spell/map light/audio or modern GPU integration.',cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='cases'},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
