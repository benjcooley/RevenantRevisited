#!/usr/bin/env python3
"""Literal Water25-drop native lifecycle and authored pixels vs current production."""
import argparse
import json
from pathlib import Path
import statistics
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EIP,UC_X86_REG_ESP
from waterfall_probe import WaterfallFixture,ASSET_SHA,INDICES,WIDTH,HEIGHT
from software_probe import RETAIL_SHA,save_rgb565_png
from fizzle_probe import span,sha,f32

ROOT=Path(__file__).resolve().parents[2]
VTABLE=0x5ad4e4


class WaterFixture(WaterfallFixture):
    def __init__(self,executable,asset,build=None):
        super().__init__(executable,asset,build)
        self.vm.put_u32(self.animator,VTABLE)
        expected={6:0x4f3410,11:0x4f34b0,13:0x4f3610,24:0x4f32c0,25:0x4f3380}
        if any(self.vm.u32(VTABLE+slot*4)!=entry for slot,entry in expected.items()):
            raise AssertionError('Pinned native Water leaf dispatch changed')
        self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=0x4f34d0,end=0x4f34d0)
        self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address in (0x417d60,0x4f34d0):
            if address==0x417d60:
                if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(16,1):raise AssertionError('Native Water mode16 changed')
                cleanup=8
            else:
                if self.vm.u32(sp+16)!=self.obj:raise AssertionError('Water copied-light object changed')
                self.ignored_light_calls+=1;cleanup=16
            uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
        else:super().external(uc,address,size,user)

    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.ignored_light_calls=0
        self.vm.call(0x4f3410,this=self.animator)

    def animate(self):self.vm.call(0x4f34b0,this=self.animator)

    def state(self):
        if self.vm.u32(self.animator+0x100)!=25:raise AssertionError('Literal Water native pool must contain25 records')
        pointer=self.vm.u32(self.animator+0xfc);particles=[]
        for index in range(25):
            values=struct.unpack('<9fi',self.vm.uc.mem_read(pointer+index*40,40))
            particles.append(dict(index=index,time=values[9],values=list(values[:9])))
        return dict(random_count=len(self.random),particles=particles)

    def original_draws(self):
        self.packets=[];self.ignored_light_calls=0;self.vm.call(0x4f3610,this=self.animator);draws=[]
        for matrix in self.packets:
            self.vm.write(self.matrix,struct.pack('<16f',*matrix));points=[]
            for point in self.points:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(self.matrix,self.point,self.result))
                points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=points,uvs=[list(v)for v in self.uvs]))
        return draws


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    initialize=span(source,'void TWaterEffect_Bespoke::Initialize(','TWaterEffect_Bespoke*\nTWaterEffect_Bespoke::SpawnForTest_BESPOKE(') if 'void TWaterEffect_Bespoke::Initialize('in source else span(source,'    w->numdrops_ = kWaterMaxDrops;','    log_info("[water] SpawnForTest_BESPOKE:')
    tick=span(source,'void TWaterEffect_Bespoke::TickAndSubmitForTest_BESPOKE(','// wave-bespoke-W2A')
    advance=span(source,'void TWaterEffect_Bespoke::Advance(','void TWaterEffect_Bespoke::TickAndSubmitForTest_BESPOKE(') if 'void TWaterEffect_Bespoke::Advance('in source else ''
    return dict(types=span(header,'class TWaterEffect_Bespoke :','// * TPixieEffect *'),
        particle=span(header,'struct SWaterParticle','_CLASSDEF(TWaterFallAnimator)'),
        constants=span(source,'constexpr int32_t kWaterMaxDrops','// First-pass billboard size'),
        helpers=span(source,'TWaterEffect_Bespoke::~TWaterEffect_Bespoke()',
            'void TWaterEffect_Bespoke::Initialize('if 'void TWaterEffect_Bespoke::Initialize('in source else 'TWaterEffect_Bespoke::SpawnForTest_BESPOKE('),
        runtime_component=span(source,'class TLiteralWaterReferenceComponent final','class TLiteralWaterAnimatorBuilder final')if'class TLiteralWaterReferenceComponent final'in source else'',
        initialize=initialize,advance_submit=advance,tick_submit=tick,
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset,inputs,ticks):
    parts=production_spans();types=parts['types'].replace('  private:','  public:')
    # The function return type immediately before Spawn is outside its body.
    helpers=parts['helpers'].rstrip().removesuffix('TWaterEffect_Bespoke*').rstrip()
    prelude=r'''
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t{Alpha,AdditiveStraight};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};enum class EFxLightMode:uint8_t{Unlit,LitFlat};enum class EFxPipeline:uint16_t{Billboard};enum class EFxBillboardOrientation:uint8_t{ScreenAligned};constexpr uint32_t OF_KILL=0x1000;
struct Key{TTextureHandle texture{};uint8_t blend{},depth_mode{};uint16_t pipeline_id{};};
struct SBillboardDrawItem{float size_wu[2]{},color_rgba[4]{},uv_rect[4]{},world_pos[3]{};Key key;EFxLightMode light_mode{};EFxBillboardOrientation orientation{};EFxDebugMode debug_mode{};};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{};Key key;EFxLightMode light_mode{};EFxDebugMode debug_mode{};int retail_texture{};};
struct TRenderer{std::vector<SBillboardDrawItem>billboards;std::vector<SQuadDrawItem>quads;void SubmitFxBillboard(const SBillboardDrawItem&i){billboards.push_back(i);}void SubmitFxQuad(const SQuadDrawItem&i){quads.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{virtual~TObjectImagery()=default;};struct SObjectDef{};struct World{hmm_mat4 matrix;World(){MtxClear(&matrix);}const hmm_mat4&Matrix()const{return matrix;}};
constexpr int OBJCLASS_EFFECT=25;constexpr uint32_t kLiteralWaterId=0x1903abcd;
struct TObjectInstance{int offscreen_calls=0;virtual void OffScreen(){++offscreen_calls;}};
struct TFlipbookBillboardComponent{TObjectInstance*owner=nullptr;virtual~TFlipbookBillboardComponent()=default;virtual const char*ComponentName()const{return"stub";}virtual void Submit(TRenderer&,const TObjectInstance&)const{};virtual void OnUpdate(){};TObjectInstance*Owner()const{return owner;}EFxDebugMode DebugMode()const{return EFxDebugMode::Normal;}void Tick(){OnUpdate();}void Configure(TTextureHandle,int,int,int,int,int,float,float,bool,bool){}};
struct S3DObj{int material=0;};struct S3DMat{struct{struct{float r=1,g=1,b=1,a=1;}diffuse;}matdesc;};struct S3DTex{struct{int width=64,height=64;}desc;};
struct T3DImagery:TObjectImagery{int NumMaterials()const{return1;}void GetObject(int,S3DObj*){};void GetMaterial(int,S3DMat*){};void GetTexture(int,S3DTex*){}};
T3DImagery imagery;bool imagery_ready=true;std::vector<S3DVertex>authored;
bool LoadWaterQuad(T3DImagery*,int,std::vector<S3DVertex>&vertices,TTextureHandle&texture){if(!imagery_ready)return false;vertices=authored;texture=1;return true;}
struct TEffect:TObjectInstance{World world;bool has_identity=false;uint32_t object_id=kLiteralWaterId,flags=0;int map_index=1,objclass=-1;std::vector<std::unique_ptr<TFlipbookBillboardComponent>>components;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;void KillThisEffect(){flags|=OF_KILL;}void SetCommandDone(bool){};uint32_t Flags()const{return flags;}uint32_t ObjId()const{if(!has_identity)throw std::runtime_error("Preview has no class/type identity");return object_id;}int GetMapIndex()const{return map_index;}int ObjClass()const{return objclass;}TObjectImagery*GetImagery(){return&imagery;}void AddComponent(std::unique_ptr<TFlipbookBillboardComponent>c){c->owner=this;components.push_back(std::move(c));}const S3DPoint&Pos()const{static S3DPoint p{};return p;}const World&Transform()const{return world;}};
struct TTime{static constexpr double LegacyFrameSeconds=1.0/24.0;static double DeltaTime(){return LegacyFrameSeconds;}};
void log_info(const char*,...){};
std::ifstream rng;int rng_count=0;int random(int low,int high){int a,b,v;if(!(rng>>a>>b>>v)||a!=low||b!=high)throw std::runtime_error("Water native/production RNG request order mismatch");++rng_count;return v;}
'''
    records=struct.unpack_from('<32f',asset,0xfc)
    def literal(v):
        text=format(v,'.9g');return text+'f'if'.'in text or'e'in text else text+'.0f'
    vertices='{'+','.join('{{'+','.join(literal(v)for v in records[i:i+3])+'},{'+','.join(literal(v)for v in records[i+3:i+6])+'},'+literal(records[i+6])+','+literal(records[i+7])+'}'for i in range(0,32,8))+'}'
    has_initialize=parts['initialize'].startswith('void TWaterEffect_Bespoke::Initialize(')
    setup='S3DVertex vertices[4]='+vertices+';authored.assign(vertices,vertices+4);'if has_initialize else 'S3DVertex vertices[4]='+vertices+';effect.authored_vertices_.assign(vertices,vertices+4);effect.literal_geometry_=true;' if 'authored_vertices_'in types else ''
    initializer=r'''
TWaterEffect_Bespoke alternate(nullptr);alternate.OffScreen();if(!(alternate.Flags()&OF_KILL))return 14;
bool runtime=argc>2 && std::string(argv[2])=="runtime";
if(runtime){
 effect.has_identity=true;effect.objclass=OBJCLASS_EFFECT;
 effect.object_id=0xadbcef15;effect.Initialize(true);effect.object_id=kLiteralWaterId;
 effect.map_index=0;effect.Initialize(true);effect.map_index=1;
 effect.objclass=0;effect.Initialize(true);effect.objclass=OBJCLASS_EFFECT;
 effect.flags=OF_KILL;effect.Initialize(true);effect.flags=0;
 if(effect.initialized_||rng_count||!effect.components.empty())return 13;
 imagery_ready=false;effect.Initialize(true);if(effect.initialized_||rng_count||!effect.components.empty())return 10;imagery_ready=true;
}
effect.Initialize(runtime);int initial_rng=rng_count;effect.Initialize(runtime);
if(rng_count!=initial_rng||effect.components.size()!=size_t(runtime))return 11;
if(runtime){effect.OffScreen();effect.Initialize(true);if(effect.Flags()&OF_KILL||effect.offscreen_calls!=1||effect.components.size()!=1||rng_count!=initial_rng)return 12;}
''' if has_initialize else parts['initialize'].replace('w->','effect.')
    clock_check=r'''
if(argc>2 && std::string(argv[2])=="fraction"){
 int fps=std::atoi(argv[3]);effect.Advance(1.01/fps);
 size_t size=size_t(effect.numdrops_)*sizeof(SWaterParticle);std::vector<unsigned char>saved(size);std::memcpy(saved.data(),effect.drops_,size);
 int before_rng=rng_count;effect.sim_accum_seconds_=0;effect.Submit(EFxDebugMode::Normal);
 if(renderer.quads.empty())return 20;auto before=renderer.quads.front();renderer.quads.clear();
 double remaining=std::fmod(1.01/fps,TTime::LegacyFrameSeconds);effect.sim_accum_seconds_=remaining;
 effect.Submit(EFxDebugMode::Normal);if(renderer.quads.empty())return 21;auto after=renderer.quads.front();
 float fraction=float(remaining/TTime::LegacyFrameSeconds);int active=0;while(effect.drops_[active].time!=0)++active;
 printf("F %d %.9g %.9g %.9g %d\n",fps,fraction,after.world_pos[0][0]-before.world_pos[0][0],effect.drops_[active].vel.X*fraction,
  int(rng_count==before_rng && std::memcmp(saved.data(),effect.drops_,size)==0));
 return 0;}
if(argc>2 && std::string(argv[2])!="runtime"){int fps=std::atoi(argv[2]);
 effect.Advance(std::numeric_limits<double>::quiet_NaN());effect.Advance(std::numeric_limits<double>::infinity());effect.Advance(-1);effect.Advance(0);
 printf("G %lld %d\n",(long long)effect.ticks_,rng_count);
 for(int frame=0;frame<fps*4;++frame)effect.Advance(1.0/fps);
 printf("C %d %lld %d\n",fps,(long long)effect.ticks_,rng_count);
 for(int i=0;i<effect.numdrops_;++i){const auto&d=effect.drops_[i];printf("P 96 %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",i,d.time,d.pos.X,d.pos.Y,d.pos.Z,d.vel.X,d.vel.Y,d.vel.Z,d.scale.X,d.scale.Y,d.scale.Z);}return0;}
'''.replace('return0','return 0')if parts['advance_submit']else''
    trailer=r'''
int main(int argc,char**argv){rng.open(argv[1]);TWaterEffect_Bespoke effect(nullptr);effect.texture_=1;SETUP INITIALIZE
CLOCK_CHECK
for(int tick=0;tick<=TICKS;++tick){renderer.billboards.clear();renderer.quads.clear();
 if(RUNTIME){if(tick)effect.components[0]->Tick();effect.components[0]->Submit(renderer,effect);}
 else if(tick)effect.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);else{FIRST_SUBMIT}
 printf("S %d %d\n",tick,rng_count);
 for(int i=0;i<effect.numdrops_;++i){const auto&d=effect.drops_[i];printf("P %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",tick,i,d.time,d.pos.X,d.pos.Y,d.pos.Z,d.vel.X,d.vel.Y,d.vel.Z,d.scale.X,d.scale.Y,d.scale.Z);}
 for(const auto&d:renderer.billboards)printf("B %d %.9g %.9g %.9g %.9g %.9g\n",tick,d.world_pos[0],d.world_pos[1],d.world_pos[2],d.size_wu[0],d.size_wu[1]);
 for(const auto&d:renderer.quads){printf("D %d",tick);for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);printf(" %u\n",unsigned(d.key.blend));}}
}
'''.replace('SETUP',setup).replace('INITIALIZE',initializer).replace('CLOCK_CHECK',clock_check).replace('RUNTIME','runtime'if has_initialize else'false').replace('TICKS',str(ticks)).replace('FIRST_SUBMIT','effect.Submit(EFxDebugMode::Normal);'if parts['advance_submit']else'')
    prelude=prelude.replace('return1','return 1')
    generated=output/'water-port-component.cpp';generated.write_text('\n'.join((prelude,parts['particle'],types,parts['constants'],helpers,parts['runtime_component'],parts['initialize']if has_initialize else'',parts['advance_submit'],parts['tick_submit'],trailer)))
    binary=output/'water-port-component';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    compile_result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(compile_result.stdout+compile_result.stderr)
    if compile_result.returncode:raise RuntimeError('Water production compile failed; see port-compile.log')
    result=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout);(output/'port-run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('Water production trace failed; see port-run.log')
    if has_initialize:
        runtime_result=subprocess.run([str(binary),str(inputs),'runtime'],capture_output=True,text=True)
        (output/'runtime-port-trace.txt').write_text(runtime_result.stdout);(output/'runtime-port-run.log').write_text(runtime_result.stderr)
        if runtime_result.returncode or runtime_result.stdout!=result.stdout:
            raise AssertionError('Actual runtime component/late initialization/offscreen replay differs from preview')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(random_count=int(p[2]),particles=[],draws=[],billboards=[])
        elif p[0]=='P':frames[tick]['particles'].append(dict(index=int(p[2]),time=int(p[3]),values=list(map(float,p[4:]))))
        elif p[0]=='B':frames[tick]['billboards'].append(dict(position=list(map(float,p[2:5])),size=list(map(float,p[5:7]))))
        else:frames[tick]['draws'].append(dict(positions=[list(map(float,p[2+i*5:5+i*5]))for i in range(4)],uvs=[list(map(float,p[5+i*5:7+i*5]))for i in range(4)],blend=int(p[22])))
    return frames,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        geometry='authored_quad'if parts['advance_submit']else'invented_billboard',runtime_preview_equal=has_initialize,
        compiled_bodies=['InitParticle','UpdateStuff','TickAndSubmit',*(['Advance','Submit']if parts['advance_submit']else[]),*(['Initialize','TLiteralWaterReferenceComponent::OnUpdate/Submit']if has_initialize else[])],
        boundaries=['source initialization loop supplied from actual Spawn','identity owner/source renderer packet interception','native ordered range-RNG replay'])


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--ticks',type=int,default=96);args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=False)
    before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as archive:asset=archive.read('Imagery/Misc/water.i3d')
    fixture=WaterFixture(args.executable,asset);fixture.reset();birth_count=fixture.state()['random_count']
    # Extra native inputs permit the old fast millisecond clock to expose its
    # state divergence instead of truncating the comparison at RNG EOF.
    for _ in range(args.ticks+32):fixture.animate()
    random=fixture.random.copy();inputs=args.output/'native-random-inputs.txt';inputs.write_text(''.join(f'{a} {b} {v}\n'for a,b,v in random))
    port,compiled=build_port(args.output,asset,inputs,args.ticks);samples=[n for n in(0,1,12,24,25,48,72,96)if n<=args.ticks]
    cases=[];failures=[];images={};times=[];field_checks=0
    for repeat in range(2):
        fixture.reset()
        for tick in range(args.ticks+1):
            if tick:fixture.animate()
            native=fixture.state();modern=port[tick];errors=[]
            if len(native['particles'])!=25 or len(modern['particles'])!=25:raise AssertionError('All25 Water drops must be compared')
            if repeat and native!=cases[tick]['original_state']:raise AssertionError('Complete native state/RNG replay changed')
            if native['random_count']!=modern['random_count']:errors.append('RNG count')
            for p,q in zip(native['particles'],modern['particles']):
                if not repeat:field_checks+=10
                if p['time']!=q['time']:errors.append(f'drop{p["index"]}.time')
                for field,(a,b)in enumerate(zip(p['values'],q['values'])):
                    if f32(a)!=f32(b):errors.append(f'drop{p["index"]}.float{field}')
            diff=None;pair=None;position_error=None
            if tick in samples:
                start=time.perf_counter();draws=fixture.original_draws();native_color,z=fixture.pixels(draws)
                if compiled['geometry']=='authored_quad':
                    if len(draws)!=len(modern['draws']):errors.append('draw count')
                    revised,rz=fixture.pixels(modern['draws']);pair=[sha(native_color),sha(revised),sha(z),sha(rz)]
                    position_error=max([abs(a-b)for p,q in zip(draws,modern['draws'])for x,y in zip(p['positions'],q['positions'])for a,b in zip(x,y)]or[0])
                    diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',native_color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',revised)))
                    if position_error>5e-5 or diff or z!=rz:errors.append('geometry/pixels/depth')
                    if any(d['blend']!=1 for d in modern['draws']):errors.append('production additive blend metadata')
                    if repeat and images[tick]!=pair:raise AssertionError('Original/production pixels or depth replay changed')
                    images[tick]=pair;save_rgb565_png(args.output/f'port-{tick:03d}.png',revised,WIDTH,HEIGHT)
                else:errors.append('unsupported invented16wu screen-aligned billboard geometry')
                save_rgb565_png(args.output/f'retail-{tick:03d}.png',native_color,WIDTH,HEIGHT);times.append((time.perf_counter()-start)*1000)
            if not repeat:
                if errors:failures.append(dict(tick=tick,errors=errors))
                cases.append(dict(tick=tick,original_state=native,port_state=modern,state_errors=errors,
                    max_position_error=position_error,differing_rgb565_pixels=diff,image_hashes=pair))
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Owned production Water source changed during comparison')
    report=dict(status='fail'if failures else'pass',failures=failures,type_id='0x1903abcd',asset_member='Imagery/Misc/water.i3d',asset_sha256=sha(asset),retail_sha256=RETAIL_SHA,
        original_initialize='0x4f3410',original_animate='0x4f34b0',original_render='0x4f3610',native_animator_vtable=hex(VTABLE),native_pool_records=25,native_warmup_ticks=25,native_birth_rng_calls=birth_count,
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),field_checks=field_checks,sampled_pixel_ticks=samples,replays_per_case=2,
        median_warm_pair_ms=statistics.median(times),scope='Literal Water25drop original Init/warmup/Animate/RNG/Render matrices vs compiled actual production methods and packets. Exact authored XYZ/UV/RGB565/original indices; identity owner/face0/white common software raster. Copied lighting callback boundary omitted; source initialization/assets/renderer adapted. Actual additive device arithmetic, normal illumination, modern triangulation, map/component and Metal separate; no FlowWater mapping claim.',
        dosbox_used=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in('cases','source_span_sha256','port_components','failures')},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
