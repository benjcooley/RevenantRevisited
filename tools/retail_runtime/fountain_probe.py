#!/usr/bin/env python3
"""Four registered Fountain leaves: native controller versus production port."""
import argparse
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
from fizzle_probe import span,sha,f32

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='a4a0564e778ea00286edc6169c7d6dba3309f2ef83c339727f9b8c5d06dad21b'
WIDTH=192;HEIGHT=192;PIXELS=WIDTH*HEIGHT
INDICES=(0,1,3,3,2,0)
NAMES=('CyanFont','RedFont','GreenFont','BlueFont')
IDS=('0x22491405','0x335a2516','0x446b3627','0x557c4738')
VTABLES=(0x5a9964,0x5a99cc,0x5a9a34,0x5a9a9c)


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'inline constexpr int32_t kFountainBespokeNumBubbles','// =========================================================================\n// W2D batch'),
        initialize=span(source,'    // --- Port of TFountainAnimator::Initialize','    log_info("[fountain] SpawnForTest:'),
        tick_submit=span(source,'void TFountainAnimator_Bespoke::TickAndSubmitForTest_BESPOKE','// --- end TFountainAnimator_Bespoke'),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text(),
        runtime=(ROOT/'src/effects/fountain.cpp').read_text())


def build_port(output,asset,inputs,ticks,color):
    pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t {Alpha};
enum class EFxDepthMode:uint8_t {TestNoWrite};
enum class EFxDebugMode:uint8_t {Normal,FullTexture};
enum class EFxLightMode:uint8_t {Unlit};
struct SQuadDrawItem {float world_pos[4][3]{};float uv[4][2]{};float color_rgba[4]{};
 struct {TTextureHandle texture{};uint8_t blend{},depth_mode{};} key;
 EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer {std::vector<SQuadDrawItem> draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};
TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery {};struct SObjectDef {};
struct World {hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect {World world;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};
 virtual ~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){throw std::runtime_error("Unexpected owner deletion");}
 const World&Transform()const{return world;}};
struct TTime {static double dt;static double DeltaTime(){return dt;}};double TTime::dt=0;
void log_info(const char*,...){}
std::ifstream random_inputs;int random_count=0;
int random(int low,int high){int a,b,value;if(!(random_inputs>>a>>b>>value)||a!=low||b!=high)
 throw std::runtime_error("Fountain original/production RNG call order mismatch");++random_count;return value;}
'''
    initializer=[]
    def literal(v):
        s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
    for obj in range(4):
        record=struct.unpack_from('<32f',asset,0x1be4+obj*128)
        initializer.append('{'+','.join('{{'+','.join(literal(v)for v in record[i:i+3])+'},{0,0,1},'+literal(record[i+6])+','+literal(record[i+7])+'}'for i in range(0,32,8))+'}')
    trailer=r'''
int main(int argc,char**argv){random_inputs.open(argv[1]);if(!random_inputs)return 2;
 TFountainAnimator_Bespoke effect(nullptr);auto*fount=&effect;S3DVertex vertices[4][4]={VERTICES};
 effect.colorobj_=std::atoi(argv[2]);
 for(int c=0;c<4;++c){effect.textures_[c]=1;std::memcpy(effect.vertices_[c],vertices[c],sizeof(vertices[c]));}
 INITIALIZE
 for(int tick=0;tick<=TICKS;++tick){TTime::dt=tick?1.0/24.0:0;renderer.draws.clear();effect.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);
  std::printf("S %d %d %d %zu\n",tick,effect.colorobj_,random_count,renderer.draws.size());
  for(int i=0;i<10;++i)std::printf("P %d %d %d %.9g %.9g %.9g %.9g %.9g\n",tick,i,effect.framenum_[i],effect.p_[i].X,effect.p_[i].Y,effect.p_[i].Z,effect.scale_[i],effect.rise_[i]);
  for(const auto&d:renderer.draws){std::printf("D %d",tick);for(int i=0;i<4;++i)std::printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);std::puts("");}
 }
 int a,b,c;if(random_inputs>>a>>b>>c)throw std::runtime_error("Unconsumed native Fountain RNG inputs");
}
'''.replace('VERTICES',','.join(initializer)).replace('INITIALIZE',pieces['initialize']).replace('TICKS',str(ticks))
    generated=output/'fountain-port-component.cpp';generated.write_text(prelude+'\n'+types+'\n'+pieces['tick_submit']+'\n'+trailer)
    binary=output/'fountain-port-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    if not binary.exists():
        result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
        if result.returncode:raise RuntimeError('Production Fountain compile failed; see port-compile.log')
    result=subprocess.run([str(binary),str(inputs),str(color)],capture_output=True,text=True)
    (output/(NAMES[color]+'-port-trace.txt')).write_text(result.stdout);(output/(NAMES[color]+'-port-run.log')).write_text(result.stderr)
    if result.returncode:raise RuntimeError('Production Fountain state/RNG trace failed')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(color=int(p[2]),random_count=int(p[3]),particles=[],draws=[])
        elif p[0]=='P':frames[tick]['particles'].append(dict(index=int(p[2]),frame=int(p[3]),values=[float(v)for v in p[4:]]))
        else:frames[tick]['draws'].append(dict(positions=[[float(v)for v in p[2+i*5:5+i*5]]for i in range(4)],uvs=[[float(v)for v in p[5+i*5:7+i*5]]for i in range(4)]))
    return frames,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        bodies=['production SpawnForTest initialization loop','TickAndSubmitForTest_BESPOKE','src/math3d.cpp'],
        boundaries=['exactSparklevertices/UVs/texture','identityowner','rendererrecordsactualSubmitFxQuad','strictunchangednativeRNGoutputs'])


def build_runtime(output,asset,inputs,ticks,color):
    """Compile the actual map producer, with RNG/asset/owner inputs explicit."""
    source=(ROOT/'src/effects/fountain.cpp').read_text()
    prelude=(output/'fountain-port-component.cpp').read_text().split('inline constexpr int32_t kFountainBespokeNumBubbles')[0]
    prelude=prelude.replace('struct TObjectImagery {};','struct T3DImagery;struct TObjectImagery {static void FreeImagery(T3DImagery*){}};')
    prelude+='''
struct TObjectInstance:TEffect {TObjectInstance():TEffect(nullptr){}};
struct S3DTex {TTextureHandle htexture=1;};
struct T3DImagery {int NumTextures()const{return 1;}void GetTexture(int,S3DTex*){throw std::runtime_error("Unexpected lazy texture load");}};
namespace d3d {struct BlendStateGuard{};void SetBlendState(){}}
namespace fountain_shim {
'''
    constants=span(source,'constexpr int32_t kNumFountainBubbles','// Inclusive range random')
    bodies=span(source,'struct State','// --------------------------------------------------------------------------\n// Spawn — load the asset')
    update=span(source,'void Tick(State* st)','void Submit(State* st)')
    vertices=[]
    for c in range(4):
        record=struct.unpack_from('<32f',asset,0x1be4+c*128)
        def literal(v):
            s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
        vertices.append('{'+','.join('{{'+','.join(literal(v)for v in record[i:i+3])+'},{0,0,1},'+literal(record[i+6])+','+literal(record[i+7])+'}'for i in range(0,32,8))+'}')
    trailer=r'''
}
int main(int argc,char**argv){random_inputs.open(argv[1]);if(!random_inputs)return 2;
 fountain_shim::State state;S3DVertex vertices[4][4]={VERTICES};state.colorobj=std::atoi(argv[2]);state.texture=1;
 std::memcpy(state.vertices,vertices[state.colorobj],sizeof(state.vertices));fountain_shim::Initialize(&state);TObjectInstance owner;
 for(int tick=0;tick<=TICKS;++tick){TTime::dt=tick?1.0/24.0:0;renderer.draws.clear();fountain_shim::Tick(&state);
  // This is the actual runtime component's owner-aware render dispatch.
  fountain_shim::Render(&state,&owner,EFxDebugMode::Normal);
  std::printf("S %d %d %d %zu\n",tick,state.colorobj,random_count,renderer.draws.size());
  for(int i=0;i<10;++i)std::printf("P %d %d %d %.9g %.9g %.9g %.9g %.9g\n",tick,i,state.framenum[i],state.p[i][0],state.p[i][1],state.p[i][2],state.scale[i],state.rise[i]);
  for(const auto&d:renderer.draws){std::printf("D %d",tick);for(int i=0;i<4;++i)std::printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);std::puts("");}
 }
 int a,b,c;if(random_inputs>>a>>b>>c)throw std::runtime_error("Unconsumed runtime Fountain RNG inputs");
}
'''.replace('VERTICES',','.join(vertices)).replace('TICKS',str(ticks))
    generated=output/'fountain-runtime-component.cpp'
    generated.write_text(prelude+constants+'\nstatic int32_t snap_random(int32_t lo,int32_t hi){return random(lo,hi);}\n'+bodies+'\n'+update+'\n'+trailer)
    binary=output/'fountain-runtime-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    if not binary.exists():
        result=subprocess.run(command,capture_output=True,text=True);(output/'runtime-compile.log').write_text(result.stdout+result.stderr)
        if result.returncode:raise RuntimeError('Actual runtime Fountain compile failed; see runtime-compile.log')
    result=subprocess.run([str(binary),str(inputs),str(color)],capture_output=True,text=True)
    (output/(NAMES[color]+'-runtime-trace.txt')).write_text(result.stdout);(output/(NAMES[color]+'-runtime-run.log')).write_text(result.stderr)
    if result.returncode:raise RuntimeError('Actual runtime Fountain trace failed')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(color=int(p[2]),random_count=int(p[3]),particles=[],draws=[])
        elif p[0]=='P':frames[tick]['particles'].append(dict(index=int(p[2]),frame=int(p[3]),values=[float(v)for v in p[4:]]))
        else:frames[tick]['draws'].append(dict(positions=[[float(v)for v in p[2+i*5:5+i*5]]for i in range(4)],uvs=[[float(v)for v in p[5+i*5:7+i*5]]for i in range(4)]))
    return frames,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        bodies=['fountain_shim::State','Initialize','Animate','Tick','Render'],
        boundaries=['actualmapcomponentowner-awareRenderdispatch','exactassetvertices/UV/texture','strictnativeRNGinputsreplacesnap_randomprovideronly',
                    'owneridentity;realmapcreation/componentregistry/lazyassetbindingremainseparate'])


class FountainFixture:
    def __init__(self,executable,asset):
        if sha(asset)!=ASSET_SHA:raise ValueError('Wrong shipped Sparkle asset')
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT);self.vm=self.software.vm
        self.software.set_texture(64,64,asset[0x1e78:0x1e78+8192],format='ARGB4444')
        self.points=[];self.uvs=[]
        for color in range(4):
            records=struct.unpack_from('<32f',asset,0x1be4+color*128)
            self.points.append([records[i:i+3]for i in range(0,32,8)]);self.uvs.append([records[i+6:i+8]for i in range(0,32,8)])
            if struct.unpack_from('<6H',asset,0x1de4+color*12)!=INDICES:raise ValueError('Unexpected Fountain topology')
        self.animator=self.vm.allocate(0x240);self.owner=self.vm.allocate(0x400);self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);vtable=self.vm.allocate(0x200)
        self.vm.put_u32(self.owner,vtable);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vtable+0x158,done)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x48332c):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.random_pending=[];self.packets=[];self.color=0
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=self.color:raise AssertionError('Wrong Fountain color object')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Wrong Fountain blend request')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if obj!=self.obj or self.vm.u32(obj)!=0x48:raise AssertionError('Fountain poseflags changed')
            self.packets.append(dict(position=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x10,12))),scale=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x40,12)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def observe_random(self,uc,address,size,user):
        if address==0x483300:
            sp=uc.reg_read(UC_X86_REG_ESP);self.random_pending.append(struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8)))
        else:
            value=struct.unpack('<i',struct.pack('<I',uc.reg_read(UC_X86_REG_EAX)))[0]
            self.random.append((*self.random_pending.pop(),value))

    def reset(self,color):
        self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.color=color
        self.vm.put_u32(self.animator,VTABLES[color]);self.vm.call(0x4e4080,this=self.animator)
        if self.vm.u32(self.animator+0x1ec)!=color:raise AssertionError('Actual leaf SetColorObject changed')

    def animate(self):self.vm.call(self.vm.u32(VTABLES[self.color]+11*4),this=self.animator)

    def state(self):
        particles=[]
        for i in range(10):
            particles.append(dict(index=i,frame=struct.unpack('<i',self.vm.uc.mem_read(self.animator+0x1c4+i*4,4))[0],
                values=list(struct.unpack('<3f',self.vm.uc.mem_read(self.animator+0xfc+i*12,12)))+
                [struct.unpack('<f',self.vm.uc.mem_read(self.animator+0x174+i*4,4))[0],struct.unpack('<f',self.vm.uc.mem_read(self.animator+0x19c+i*4,4))[0]]))
        return dict(color=self.vm.u32(self.animator+0x1ec),random_count=len(self.random),particles=particles)

    def original_draws(self):
        self.packets=[];self.vm.call(0x4e41f0,this=self.animator);draws=[]
        for packet in self.packets:
            self.vm.put_u32(self.obj,0x48);self.vm.write(self.obj+0x10,struct.pack('<3f',*packet['position']));self.vm.write(self.obj+0x40,struct.pack('<3f',*packet['scale']))
            self.vm.call(0x40a420,(self.obj,0,0,0,0),this=self.imagery);points=[]
            for point in self.points[self.color]:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result))
                points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=points,uvs=self.uvs[self.color],**packet))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Fountain viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],INDICES,z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--ticks',type=int,default=96);parser.add_argument('--repeat',type=int,default=2);parser.add_argument('--state-only',action='store_true');args=parser.parse_args()
    if args.repeat<2:parser.error('At least two replays required')
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as archive:asset=archive.read('Imagery/Misc/sparkle.i3d')
    fixture=FountainFixture(args.executable,asset);cases=[];errors=[];times=[];samples=sorted({x for x in (0,1,3,6,13,14,20,24,48,72,args.ticks)if x<=args.ticks})
    compiled=None;runtime_compiled=None;inputs=[]
    # Rebuild once each invocation; later colors share the same compiled body.
    for binary in (args.output/'fountain-port-component',args.output/'fountain-runtime-component'):
        if binary.exists():binary.unlink()
    for color,name in enumerate(NAMES):
        fixture.reset(color)
        for tick in range(args.ticks):fixture.animate()
        random=fixture.random.copy();random_file=args.output/(name+'-native-random.txt');random_file.write_text(''.join(f'{a} {b} {v}\n'for a,b,v in random))
        inputs.append(dict(name=name,calls=len(random),sha256=sha(random_file.read_bytes())))
        port,compiled=build_port(args.output,asset,random_file,args.ticks,color)
        runtime,runtime_compiled=build_runtime(args.output,asset,random_file,args.ticks,color);first={}
        for repeat in range(args.repeat):
            fixture.reset(color)
            for tick in range(args.ticks+1):
                if tick:fixture.animate()
                original=fixture.state();ported=port[tick];runtime_frame=runtime[tick];state_errors=[]
                # Both compiled production paths are compared independently.
                # Exact packet equality lets the same native raster comparison
                # cover runtime images without repeating a third identical draw.
                runtime_equal=runtime_frame==ported
                if not runtime_equal:state_errors.append('actual runtime producer differs from canonical production')
                if original['color']!=ported['color']or original['random_count']!=ported['random_count']:state_errors.append('color or random count')
                for p,q in zip(original['particles'],ported['particles']):
                    if p['frame']!=q['frame']:state_errors.append(f'bubble{p["index"]}.frame')
                    for field,(x,y)in enumerate(zip(p['values'],q['values'])):
                        if f32(x)!=f32(y):state_errors.append(f'bubble{p["index"]}.float{field}')
                position_error=uv_error=0;diff=None;pair=None;draws=[]
                if tick in samples and not args.state_only:
                    start=time.perf_counter();draws=fixture.original_draws();native,z=fixture.pixels(draws);modern,mz=fixture.pixels(ported['draws']);times.append((time.perf_counter()-start)*1000)
                    pair=(sha(native),sha(modern),sha(z),sha(mz))
                    if repeat==0:first[tick]=pair
                    elif first[tick]!=pair:raise AssertionError('Fountain warm image/depth replay changed')
                    if len(draws)!=len(ported['draws']):state_errors.append('visible bubble submission count')
                    position_error=max([abs(a-b)for p,q in zip(draws,ported['draws'])for x,y in zip(p['positions'],q['positions'])for a,b in zip(x,y)]or[0])
                    uv_error=max([abs(a-b)for p,q in zip(draws,ported['draws'])for x,y in zip(p['uvs'],q['uvs'])for a,b in zip(x,y)]or[0])
                    diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(PIXELS)+'H',native),struct.unpack('<'+str(PIXELS)+'H',modern)))
                    if repeat==0:
                        save_rgb565_png(args.output/f'{name}-retail-{tick:03d}.png',native,WIDTH,HEIGHT)
                        save_rgb565_png(args.output/f'{name}-port-{tick:03d}.png',modern,WIDTH,HEIGHT)
                if repeat==0:
                    if state_errors or position_error>3e-5 or uv_error>1e-7 or diff:
                        errors.append(dict(name=name,tick=tick,state_errors=state_errors,max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff))
                    cases.append(dict(name=name,type_id=IDS[color],tick=tick,original_state=original,port_state=ported,runtime_state=runtime_frame,
                        runtime_equals_canonical=runtime_equal,runtime_color_depth_hashes=(pair[1],pair[3])if runtime_equal and pair else None,
                        original_draws=draws,state_errors=state_errors,max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff,image_hashes=pair))
            if fixture.random!=random:raise AssertionError('Native Fountain RNG did not replay exactly')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Fountain production source spans changed during comparison')
    if not args.state_only and 6 in samples:
        colors={c['image_hashes'][0]for c in cases if c['tick']==6}
        if len(colors)!=4:raise AssertionError('The four registered colors did not produce distinct visible images')
    report=dict(status='pass'if not errors else'fail',failures=errors,retail_sha256=RETAIL_SHA,asset_member='Imagery/Misc/sparkle.i3d',asset_sha256=sha(asset),asset_bytes=len(asset),
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_component=compiled,runtime_component=runtime_compiled,
        runtime_producer_compared=True,runtime_packets_equal_canonical=all(c['runtime_equals_canonical']for c in cases),
        variants=list(NAMES),profile_count=4,case_count=len(cases),replays_per_case=args.repeat,
        sampled_pixel_ticks=[]if args.state_only else samples,median_warm_pixel_pair_ms=statistics.median(times)if times else None,native_rng_inputs=inputs,
        original_initialize='0x4e4080',original_animate='0x4e4120 via actual leaf vtable wrappers',original_render='0x4e41f0',original_matrix='0x40a420',original_random='0x483300',
        original_leaf_vtables=[hex(v)for v in VTABLES],vertex_offsets=['0x1be4','0x1c64','0x1ce4','0x1d64'],texture_offset='0x1e78',
        camera=dict(position=[0,0,0],zdist=1925,viewport=[WIDTH,HEIGHT],owner_world='identity',lighting='explicitwhite31pervertex'),
        scope='Original registered four Fountain leaves and complete delay/rise/shrink/respawn state with unchanged native RNG inputs vs compiled current production initialization loop and TickAndSubmit body. Exact legacySparkle authoredmeshes/UVs/ARGBatlas, originalCalcObjectMatrix/projection/softwarepixels. Baseanimator/owner/imagery/extents/renderershell areexplicitboundaries; natural scriptedmapownership/light/modernGPU remainseparate.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
