#!/usr/bin/env python3
"""Literal Waterfall native warmup, drops and authored software pixels."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
from fountain_probe import FountainFixture
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='ece24f00eab927662fea2027dde203afec2525471eb6cde7b2a06f69c9b9aa75'
WIDTH=320;HEIGHT=512;INDICES=(1,0,2,2,3,1)


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'class TWaterFallEffect_Bespoke :','// *************************************************************************\n// * TWaterEffect_Bespoke'),
        particle=span(header,'struct SWaterParticle','_CLASSDEF(TWaterFallAnimator)'),
        constants=span(source,'constexpr int32_t kWaterFallMaxDrops','// Snapshot constants — direct from effect_old.cpp:11903'),
        initialize=span(source,'    numdrops_ = kWaterFallMaxDrops;','    if (attach_runtime_component)\n    {'),
        helpers=span(source,'TWaterFallEffect_Bespoke::~TWaterFallEffect_Bespoke()','void TWaterFallEffect_Bespoke::Initialize('),
        advance_submit=span(source,'void TWaterFallEffect_Bespoke::Advance(','void TWaterFallEffect_Bespoke::TickAndSubmitForTest_BESPOKE('),
        map_dispatch=span(source,'class TWaterfallReferenceComponent final','TWaterFallEffect_Bespoke::~TWaterFallEffect_Bespoke()'),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset,inputs,ticks):
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
enum class EFxBlend:uint8_t{Alpha,AdditiveStraight};constexpr uint32_t OF_KILL=0x1000;enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};enum class EFxLightMode:uint8_t{Unlit};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TObjectInstance{virtual void OffScreen(){}};struct TEffect:TObjectInstance{World world;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;uint32_t Flags()const{return 0;}void SetCommandDone(bool){};uint32_t ObjId()const{return 0xa907dabf;}int GetMapIndex()const{return 1;}int GetFace()const{return 0;}const World&Transform()const{return world;}};
struct TTime{static constexpr double LegacyFrameSeconds=1.0/24.0;};void log_info(const char*,...){};
std::ifstream rng;int rng_count=0;int random(int low,int high){int a,b,v;if(!(rng>>a>>b>>v)||a!=low||b!=high)throw std::runtime_error("Waterfall native/port RNG order mismatch");++rng_count;return v;}
'''
    records=struct.unpack_from('<32f',asset,0xfc)
    def literal(v):
        s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
    vertices='{'+','.join('{{'+','.join(literal(v)for v in records[i:i+3])+'},{'+','.join(literal(v)for v in records[i+3:i+6])+'},'+literal(records[i+6])+','+literal(records[i+7])+'}'for i in range(0,32,8))+'}'
    initializer='void TWaterFallEffect_Bespoke::Initialize(bool attach_runtime_component){\n'+pieces['initialize']+'}\n'
    trailer=r'''
int main(int argc,char**argv){rng.open(argv[1]);if(!rng)return 2;TWaterFallEffect_Bespoke effect(nullptr);S3DVertex vertices[4]=VERTICES;
 effect.authored_vertices_.assign(vertices,vertices+4);effect.texture_=1;effect.Initialize(true);
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
  printf("S %d %d %zu\n",tick,rng_count,renderer.draws.size());
  for(int i=0;i<100;++i){const auto&d=effect.drops_[i];printf("P %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",tick,i,d.time,d.pos.X,d.pos.Y,d.pos.Z,d.vel.X,d.vel.Y,d.vel.Z,d.scale.X,d.scale.Y,d.scale.Z);}
  for(const auto&d:renderer.draws){printf("D %d",tick);for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);for(float c:d.color_rgba)printf(" %.9g",c);puts("");}}
 int a,b,c;if(rng>>a>>b>>c)throw std::runtime_error("Unused original RNG inputs");}
'''.replace('VERTICES',vertices).replace('TICKS',str(ticks))
    source=output/'waterfall-port-component.cpp';source.write_text(prelude+'\n'+pieces['particle']+'\n'+types+'\n'+pieces['constants']+'\n'+pieces['helpers']+'\n'+initializer+'\n'+pieces['advance_submit']+'\n'+trailer)
    binary=output/'waterfall-port-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Waterfall production compile failed')
    result=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout);(output/'port-run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('Waterfall production trace failed')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(random_count=int(p[2]),particles=[],draws=[])
        elif p[0]=='P':frames[tick]['particles'].append(dict(index=int(p[2]),time=int(p[3]),values=[float(x)for x in p[4:]]))
        else:frames[tick]['draws'].append(dict(positions=[[float(x)for x in p[2+i*5:5+i*5]]for i in range(4)],uvs=[[float(x)for x in p[5+i*5:7+i*5]]for i in range(4)],material_diffuse=[float(x)for x in p[22:26]]))
    return frames,dict(command=command,generated_source_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        bodies=['Initialize actual state/warmup loop','InitParticle','UpdateStuff','Advance','Submit','src/math3d.cpp'],boundaries=['exact authored geometry/texture','identity owner, face zero','unchanged original range-RNG result replay','asset/component binding separate'])


class WaterfallFixture:
    observe_random=FountainFixture.observe_random
    def __init__(self,executable,asset,build=None):
        if sha(asset)!=ASSET_SHA:raise ValueError('Wrong exact Waterfall asset')
        if struct.unpack_from('<4I',asset,0x258+88)!=(0xf800,0x7e0,0x1f,0):raise ValueError('Waterfall RGB565 masks changed')
        if struct.unpack_from('<4f',asset,0x208+4)!=(1,1,1,1):raise ValueError('Waterfall authored diffuse changed')
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT,build=build);self.vm=self.software.vm
        self.software.set_texture(64,64,asset[0x2d4:0x2d4+8192],format='RGB565')
        records=struct.unpack_from('<32f',asset,0xfc);self.points=[records[i:i+3]for i in range(0,32,8)];self.uvs=[records[i+6:i+8]for i in range(0,32,8)]
        if struct.unpack_from('<6H',asset,0x17c)!=INDICES:raise ValueError('Waterfall topology changed')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200)
        self.vm.put_u32(self.animator,0x5ad26c);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        vt=self.vm.allocate(0x200);self.vm.put_u32(self.owner,vt);done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vt+0x158,done)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.matrix=self.vm.allocate(64)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x40a0c0:8,0x4f2f20:16}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x48332c):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.random_pending=[];self.packets=[];self.ignored_light_calls=0;self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Waterfall wrong mesh')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(8,1):raise AssertionError('Waterfall blend request changed')
        elif address==0x40a0c0:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(self.obj,0x1e2):raise AssertionError('Waterfall copied vertex format changed')
        elif address==0x4f2f20:
            if self.vm.u32(sp+16)!=self.obj:raise AssertionError('Waterfall ignored-lighting object changed')
            self.ignored_light_calls+=1
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if self.vm.u32(obj)!=0x100:raise AssertionError('Waterfall matrix poseflags changed')
            self.packets.append(list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.ignored_light_calls=0;self.vm.call(self.vm.u32(0x5ad26c+6*4),this=self.animator)

    def animate(self):self.vm.call(self.vm.u32(0x5ad26c+11*4),this=self.animator)

    def state(self):
        if self.vm.u32(self.animator+0x100)!=100:raise AssertionError('Waterfall native drop count changed')
        pointer=self.vm.u32(self.animator+0xfc)
        particles=[]
        for i in range(100):
            values=struct.unpack('<9fi',self.vm.uc.mem_read(pointer+i*40,40));particles.append(dict(index=i,time=values[9],values=list(values[:9])))
        return dict(random_count=len(self.random),particles=particles)

    def original_draws(self):
        self.packets=[];self.ignored_light_calls=0;self.vm.call(self.vm.u32(0x5ad26c+13*4),this=self.animator);draws=[]
        for matrix in self.packets:
            self.vm.write(self.matrix,struct.pack('<16f',*matrix));points=[]
            for point in self.points:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(self.matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=points,uvs=self.uvs))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Waterfall viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],INDICES,z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--ticks',type=int,default=96);parser.add_argument('--repeat',type=int,default=2);parser.add_argument('--state-only',action='store_true');args=parser.parse_args()
    if args.repeat<2:parser.error('At least two exact replays required')
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as a:asset=a.read('Imagery/Misc/water.i3d')
    fixture=WaterfallFixture(args.executable,asset);fixture.reset()
    for tick in range(args.ticks):fixture.animate()
    random=fixture.random.copy();inputs=args.output/'native-random-inputs.txt';inputs.write_text(''.join(f'{a} {b} {v}\n'for a,b,v in random))
    port,compiled=build_port(args.output,asset,inputs,args.ticks);samples=[x for x in(0,1,12,24,25,48,72,96)if x<=args.ticks];cases=[];errors=[];times=[];replay={}
    for repeat in range(args.repeat):
        fixture.reset()
        for tick in range(args.ticks+1):
            if tick:fixture.animate()
            original=fixture.state();modern=port[tick];state_errors=[]
            if len(original['particles'])!=100 or len(modern['particles'])!=100:raise AssertionError('Waterfall must compare the full100-drop pool')
            if repeat and original!=cases[tick]['original_state']:raise AssertionError('Waterfall warm native state replay changed')
            if original['random_count']!=modern['random_count']:state_errors.append('RNG count')
            for p,q in zip(original['particles'],modern['particles']):
                if p['time']!=q['time']:state_errors.append(f'drop{p["index"]}.time')
                for field,(a,b)in enumerate(zip(p['values'],q['values'])):
                    if f32(a)!=f32(b):state_errors.append(f'drop{p["index"]}.float{field}')
            image=None;diff=None;position_error=0;draws=[]
            if tick in samples and not args.state_only:
                start=time.perf_counter();draws=fixture.original_draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(modern['draws']);times.append((time.perf_counter()-start)*1000)
                image=(sha(color),sha(mcolor),sha(depth),sha(mdepth))
                if repeat==0:replay[tick]=image
                elif replay[tick]!=image:raise AssertionError('Waterfall warm pixel/depth replay changed')
                if len(draws)!=len(modern['draws']):state_errors.append('submission count')
                if fixture.ignored_light_calls!=len(draws):state_errors.append('unused copied-lighting callback count')
                if any(d['material_diffuse']!=[1,1,1,1]for d in modern['draws']):state_errors.append('authored material diffuse')
                if any(f32(v)!=f32(w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['uvs'],y['uvs'])for v,w in zip(p,q)):state_errors.append('authored UVs')
                position_error=max([abs(a-b)for p,q in zip(draws,modern['draws'])for x,y in zip(p['positions'],q['positions'])for a,b in zip(x,y)]or[0])
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                if not repeat:save_rgb565_png(args.output/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
            if not repeat:
                if state_errors or position_error>5e-5 or diff:errors.append(dict(tick=tick,state_errors=state_errors,max_position_error=position_error,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,original_state=original,port_state=modern,state_errors=state_errors,image_hashes=image,max_position_error=position_error,differing_rgb565_pixels=diff,original_draws=draws))
        if fixture.random!=random:raise AssertionError('Native RNG replay changed')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Waterfall production source spans changed')
    report=dict(status='pass'if not errors else'fail',failures=errors,retail_sha256=RETAIL_SHA,asset_member='Imagery/Misc/water.i3d',asset_sha256=sha(asset),type_id='0xa907dabf',source_span_sha256=after,
        probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),sampled_pixel_ticks=[]if args.state_only else samples,replays_per_case=args.repeat,native_rng_calls=len(random),native_rng_input_sha256=sha(inputs.read_bytes()),
        median_warm_pixel_pair_ms=statistics.median(times)if times else None,original_initialize='0x4f2e60',original_animate='0x4f2f00',original_render='0x4f3060',
        scope='Original continuous 100-drop native state/RNG/100-tick warmup/respawn/render against shared actual map/preview Initialize state, Advance and Submit. Exact authored geometry/UVs/RGB565 texture and diffuse material; identity owner, face zero, white vertices, no source shader fitting. Original base animator/extents/imagery-copy/submission boundary adapters; unused copied-LVERTEX DoLighting output omitted explicitly. Render clears VERTS and uses authored normals/materials, white vertex input is controlled. The common software target uses retail authored indices; modern quad triangulation and normal/device lighting are separate. Original matrix/projection/raster unchanged. Real map/component binding, arbitrary owner face/world pose, lighting/culling/blending and Metal backend separate.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)

if __name__=='__main__':main()
