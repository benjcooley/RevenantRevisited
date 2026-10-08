#!/usr/bin/env python3
"""Linked no-splash Drip/Ripple actors: actual native and production lifecycles."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from drip_probe import DripFixture,ROOT,ASSET_SHA as DRIP_SHA,INDICES,WIDTH,HEIGHT,production_spans as drip_spans
from ripple_probe import RippleFixture,ASSET_SHA as RIPPLE_SHA,production_spans as ripple_spans
from software_probe import save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
PARAMS=(20,128,1);STOP_TICK=121;TICKS=144


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text()
    return {**{'drip_'+k:v for k,v in drip_spans().items()},**{'ripple_'+k:v for k,v in ripple_spans().items()},
        'ripple_factory':span(source,'TRippleEffect* TRippleEffect::SpawnForTest(','void TRippleEffect::Advance('),
        'asset_quad_loader':span(source,'bool LoadWaterQuad(','void SubmitWaterQuad(')}


class NativeChild(RippleFixture):
    def reset_child(self,length):
        if length!=20:raise ValueError('Linked fixture is only the no-splash length20 contract')
        self.software.restore();self.calls={};self.vm.put_u32(self.owner+0x300,length);self.vm.call(0x4f0780,this=self.animator)
    def state(self):return dict(frameon=self.vm.u32(self.animator+0x104),ripframe=self.vm.u32(self.animator+0x100),alive=not bool(self.vm.u32(self.owner+8)&0x1000),splash_count=self.vm.u32(self.animator+0x108))
    def animate(self):
        if self.state()['alive']:self.vm.call(0x4f08b0,this=self.animator)
    def draws(self):
        if not self.state()['alive']:return []
        self.vm.call(0x4f0c10,this=self.animator);points=[]
        for xyz in self.points:
            self.vm.write(self.point,struct.pack('<3f',*xyz));self.vm.call(0x43ad80,(self.obj+0x58,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
        uv=[list(struct.unpack('<2f',self.vm.uc.mem_read(self.lverts+i*32+24,8)))for i in range(4)]
        return [dict(texture=2,positions=points,uvs=uv)]


class CompositeFixture:
    def __init__(self,executable,drip_asset,ripple_asset):
        self.parent=DripFixture(executable,drip_asset);self.child_vm=NativeChild(executable,ripple_asset)
        self.textures={1:(64,64,drip_asset[0x2d4:0x2d4+8192]),2:(256,256,ripple_asset[0x4f0:0x4f0+131072])}
        self.reset()
    def reset(self):self.parent.reset(PARAMS);self.active_child=None;self.births=[];self.deaths=[];self.tick=0
    def step(self):
        self.tick+=1
        # Existing children own the elapsed step before a parent event; a child
        # born by that event starts at age0 and gets its first step next tick.
        if self.active_child:
            self.child_vm.animate()
            if not self.child_vm.state()['alive']:
                self.deaths.append(dict(tick=self.tick,id=self.active_child['id']));self.active_child=None
        if self.tick==STOP_TICK:
            vm=self.parent.vm;vm.call(vm.u32(vm.u32(self.parent.owner)+0x200),(20,128,1000000),this=self.parent.owner)
        before=len(self.parent.events);self.parent.animate()
        for event in self.parent.events[before:]:
            if self.active_child:raise AssertionError('Unexpected child overlap outside bounded pool contract')
            if event['arguments']!=[0,0,0,20]:raise AssertionError('Unexpected linked child parameters/origin')
            self.active_child=dict(id=len(self.births)+1,birth=self.tick);self.births.append(dict(tick=self.tick,id=self.active_child['id'],arguments=event['arguments']))
            self.child_vm.reset_child(event['arguments'][3])
    def state(self):
        children=[]
        if self.active_child:
            c=self.child_vm.state()
            if c['splash_count']:raise AssertionError('No-splash child created particle splash state')
            children.append(dict(id=self.active_child['id'],frameon=c['frameon'],ripframe=c['ripframe'],alive=c['alive']))
        return dict(parent=self.parent.state(),children=children,births=self.births.copy(),deaths=self.deaths.copy())
    def draws(self):
        draws=[dict(texture=1,**d)for d in self.parent.original_draws()]
        if self.active_child:draws+=self.child_vm.draws()
        return draws
    def pixels(self,draws):
        s=self.parent.software;s.clear()
        for d in draws:
            w,h,texture=self.textures[d['texture']];s.set_texture(w,h,texture,format='ARGB4444');points=s.project(d['positions'],camera=(0,0,0),zdist=1925)
            if len(points)!=4 or len(d['uvs'])!=4:raise AssertionError('Linked quad cardinality mismatch')
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in points):raise AssertionError('Linked authored geometry outside declared viewport')
            s.draw([(*p,31,31,31,31,*uv)for p,uv in zip(points,d['uvs'])],INDICES,z_enabled=True,z_write=False)
        return s.vm.surface_bytes('screen'),s.vm.surface_bytes('depth')


def build_port(output,drip_asset,ripple_asset,inputs):
    pieces=production_spans();types=pieces['ripple_declaration'].replace('  private:','  public:')+'\n'+pieces['drip_types'].replace('  private:','  public:')
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal,FullTexture};enum class EFxLightMode:uint8_t{Unlit};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct Journal{int tick,id;};std::vector<Journal>births,deaths;int now=0;
struct TObjectImagery{virtual~TObjectImagery()=default;static void FreeImagery(TObjectImagery*){throw std::runtime_error("Unexpected asset rejection");}};struct SObjectDef{};
struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TObjectInstance{World world;S3DPoint pos{};int mapindex=0;virtual~TObjectInstance(){if(mapindex)deaths.push_back({now,mapindex});}
 const S3DPoint&Pos()const{return pos;}const World&Transform()const{return world;}void ForcePos(S3DPoint p){pos=p;MtxClear(&world.m);hmm_vec3 v={float(p.x),float(p.y),float(p.z)};MtxTranslate(&world.m,&v);}
 void SetMapIndex(int n){mapindex=n;births.push_back({now,n});}int GetMapIndex()const{return mapindex;}void ActivateComponents(){}};
struct TEffect:TObjectInstance{TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual void Pulse(){};virtual void Load(RTInputStream,int32_t,int32_t){};virtual void Save(RTOutputStream){}};
struct TMapPane{int next=0;int MakeIndex(){return ++next;}}MapPane;
void log_info(const char*,...){};void log_error(const char*,...){throw std::runtime_error("Asset binding failed");}
extern "C" int rand(void){throw std::runtime_error("Unexpected raw RNG: native short no-splash physics requires none");}
std::ifstream rng;int rng_count=0;
struct S3DTex{TTextureHandle htexture=2;};struct SMeshVertex{};
struct T3DImagery:TObjectImagery{S3DVertex vertices[2][4];int NumObjects()const{return 2;}int NumObjVerts(int n){return n<2?4:0;}int NumTextures()const{return 1;}
 void GetObjVerts(int n,S3DVertex*out){if(n<0||n>1)throw std::runtime_error("Unexpected object binding");std::memcpy(out,vertices[n],sizeof(vertices[n]));}void GetTexture(int n,S3DTex*out){if(n!=0)throw std::runtime_error("Unexpected texture slot");out->htexture=2;}}imagery;
T3DImagery*LoadWaterImagery(const char*name,const char*){if(std::strcmp(name,"Magic\\ripples.I3D"))throw std::runtime_error("Unexpected linked asset");return &imagery;}
bool ExtractSubMeshTextureSlot(T3DImagery*,int obj,int slot,std::vector<SMeshVertex>&v,std::vector<uint16_t>&i){if(obj<0||obj>1||slot!=1)throw std::runtime_error("Wrong linked authored binding");i={0,1,3,3,2,0};return true;}
'''
    # The real helper executes, wrapped only to verify observed native invocation
    # order, returned value and the explicit absence of a raw CRT draw.
    rng_wrapper=r'''
int WaterRandom(int low,int high){int a,b,value,raw;if(!(rng>>a>>b>>value>>raw)||a!=low||b!=high||raw!=-1)throw std::runtime_error("Linked physics RNG contract changed");int got=production_random::WaterRandom(low,high);if(got!=value)throw std::runtime_error("Linked RNG result differs");++rng_count;return got;}
'''
    boundary=r'''
TDripEffect::~TDripEffect()=default;
void TDripEffect::Initialize(){throw std::runtime_error("Owner component install unsupported");}void TDripEffect::Pulse(){throw std::runtime_error("Owner Pulse unsupported");}
void TDripEffect::Load(RTInputStream,int32_t,int32_t){throw std::runtime_error("Owner serialization unsupported");}void TDripEffect::Save(RTOutputStream){throw std::runtime_error("Owner serialization unsupported");}
void TRippleEffect::Pulse(){throw std::runtime_error("Owner Pulse outside composed component schedule");}
'''
    def vertices(asset,offset):
        records=struct.unpack_from('<32f',asset,offset)
        def literal(v):
            s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
        return '{'+','.join('{{'+','.join(literal(x)for x in records[i:i+3])+'},{'+','.join(literal(x)for x in records[i+3:i+6])+'},'+literal(records[i+6])+','+literal(records[i+7])+'}'for i in range(0,32,8))+'}'
    trailer=r'''
int main(int argc,char**argv){rng.open(argv[1]);if(!rng)return 2;S3DVertex head[4]=HEAD,ring[4]=RING,splash[4]=SPLASH;std::memcpy(imagery.vertices[0],ring,sizeof(ring));std::memcpy(imagery.vertices[1],splash,sizeof(splash));
 TDripEffect parent(nullptr);parent.drop_vertices_.assign(head,head+4);parent.drop_texture_=1;parent.SetParams(20,128,1);bool valid=false;
 for(now=0;now<=144;++now){if(now==121)parent.SetParams(20,128,1000000);if(now)parent.Advance(1.0/24.0);valid|=!parent.dead_;renderer.draws.clear();parent.Submit(EFxDebugMode::Normal);
  printf("P %d %d %d %d %d %zu %.9g %.9g %.9g %.9g %.9g %.9g\n",now,int(parent.dead_),parent.time_,rng_count,int(valid),births.size(),parent.drop_pos_.X,parent.drop_pos_.Y,parent.drop_pos_.Z,parent.drop_vel_.X,parent.drop_vel_.Y,parent.drop_vel_.Z);
  for(const auto&child:parent.spawned_ripples_)printf("C %d %d %d %d %d %d\n",now,child->GetMapIndex(),child->frameon_,child->ripframe_,int(child->IsAlive()),child->GetLength());
  for(const auto&d:renderer.draws){printf("Q %d %u",now,unsigned(d.key.texture));for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}
 for(const auto&b:births)printf("B %d %d\n",b.tick,b.id);for(const auto&d:deaths)printf("D %d %d\n",d.tick,d.id);
 int a,b,c,d;if(rng>>a>>b>>c>>d)throw std::runtime_error("Unused linked physics RNG input");}
'''.replace('HEAD',vertices(drip_asset,0xfc)).replace('RING',vertices(ripple_asset,0x144)).replace('SPLASH',vertices(ripple_asset,0x1c4))
    generated=output/'drip-ripple-production.cpp';generated.write_text(prelude+'\nnamespace production_random {\n'+pieces['drip_random_helper']+'}\n'+rng_wrapper+'\n'+types+'\n'+boundary+'\n'+pieces['drip_constants']+'\n'+pieces['drip_time_helpers']+'\n'+pieces['asset_quad_loader']+'\n'+pieces['drip_quad']+'\n'+pieces['ripple_factory']+'\n'+pieces['ripple_methods']+'\n'+pieces['drip_advance_submit']+'\n'+trailer)
    binary=output/'drip-ripple-production';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Composed production compile failed')
    result=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout);(output/'port-run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('Composed production lifecycle failed')
    rows={};births=[];deaths=[]
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='P':rows[tick]=dict(parent=dict(dead=bool(int(p[2])),time=int(p[3]),random_count=int(p[4]),values_valid=bool(int(p[5])),event_count=int(p[6]),values=[float(x)for x in p[7:]]),children=[],draws=[])
        elif p[0]=='C':
            if int(p[6])!=20:raise AssertionError('Production child length changed')
            rows[tick]['children'].append(dict(id=int(p[2]),frameon=int(p[3]),ripframe=int(p[4]),alive=bool(int(p[5]))))
        elif p[0]=='Q':rows[tick]['draws'].append(dict(texture=int(p[2]),positions=[[float(x)for x in p[3+i*5:6+i*5]]for i in range(4)],uvs=[[float(x)for x in p[6+i*5:8+i*5]]for i in range(4)]))
        elif p[0]=='B':births.append(dict(tick=tick,id=int(p[2]),arguments=[0,0,0,20]))
        else:deaths.append(dict(tick=tick,id=int(p[2])))
    for tick,row in rows.items():row.update(births=[x for x in births if x['tick']<=tick],deaths=[x for x in deaths if x['tick']<=tick])
    return rows,dict(command=cmd,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),actual_bodies=['Drip Advance/Submit','Ripple factory/Advance/Submit','WaterRandom','AdvanceWaterTime','WaterDropPosition','LoadWaterQuad','SubmitWaterQuad','math3d.cpp'],boundaries=['exact asset providers and scene identity methods','base destructor birth/death journals','no original map allocator/sector loop/component installation','audio lookup omitted; native audio raw draws observed separately'])



def state_errors(native,port):
    """Validate full linked state before comparing any float or child record."""
    errors=[]
    for name in ('dead','time','random_count','values_valid','event_count'):
        if native['parent'][name]!=port['parent'][name]:errors.append('parent.'+name)
    if len(native['parent']['values'])!=6 or len(port['parent']['values'])!=6:
        raise AssertionError('Parent field cardinality')
    if native['parent']['values_valid'] and any(f32(a)!=f32(b) for a,b in zip(native['parent']['values'],port['parent']['values'])):
        errors.append('parent pos/vel')
    for key in ('children','births','deaths'):
        if native[key]!=port[key]:errors.append(key)
    return errors


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True);parser.add_argument('--repeat',type=int,default=2);parser.add_argument('--state-only',action='store_true');args=parser.parse_args()
    if args.repeat<2:parser.error('At least two complete repeats required')
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as z:drip=z.read('Imagery/Magic/drip.i3d');ripple=z.read('Imagery/Magic/ripples.i3d')
    if sha(drip)!=DRIP_SHA or sha(ripple)!=RIPPLE_SHA:raise ValueError('Unsupported composite assets')
    fixture=CompositeFixture(args.executable,drip,ripple)
    for _ in range(TICKS):fixture.step()
    random=fixture.parent.random.copy();audio=fixture.parent.audio_random.copy();inputs=args.output/'native-physics-random.txt';inputs.write_text(''.join(f'{a} {b} {c} {d}\n'for a,b,c,d in random));(args.output/'native-audio-random.json').write_text(json.dumps(audio,indent=2)+'\n')
    port,compiled=build_port(args.output,drip,ripple,inputs);samples=sorted(set(range(0,TICKS+1,6))|{23,24,25,26,42,44,45,48,49,68,69,72,73,92,93,96,97,116,117,120,121,140,141,142,144});cases=[];failures=[];times=[];replay={}
    for repeat in range(args.repeat):
        fixture.reset()
        for tick in range(TICKS+1):
            if tick:fixture.step()
            native=fixture.state();row=port[tick];errors=[]
            if repeat and native!=cases[tick]['native']:raise AssertionError('Composed warm state/events changed')
            errors=state_errors(native,row)
            image=None;diff=None;corner=0;draws=[]
            if tick in samples and not args.state_only:
                start=time.perf_counter();draws=fixture.draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(row['draws']);times.append((time.perf_counter()-start)*1000);image=[sha(color),sha(mcolor),sha(depth),sha(mdepth)]
                if not repeat:replay[tick]=image
                elif replay[tick]!=image:raise AssertionError('Composed warm image/depth changed')
                if len(draws)!=len(row['draws']):errors.append('ordered submission cardinality')
                for a,b in zip(draws,row['draws']):
                    if a['texture']!=b['texture']:errors.append('ordered texture binding')
                    if len(a['positions'])!=4 or len(b['positions'])!=4:raise AssertionError('Composed quad cardinality')
                    if any(f32(x)!=f32(y)for p,q in zip(a['uvs'],b['uvs'])for x,y in zip(p,q)):errors.append('authored child/head UVs')
                corner=max([abs(x-y)for a,b in zip(draws,row['draws'])for p,q in zip(a['positions'],b['positions'])for x,y in zip(p,q)]or[0]);diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                if image[2]!=image[3]:errors.append('depth')
                if not repeat:save_rgb565_png(args.output/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
            if not repeat:
                if errors or corner>2e-5 or diff:failures.append(dict(tick=tick,errors=errors,max_corner_error=corner,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,native=native,port=row,errors=errors,image_hashes=image,max_corner_error=corner,differing_rgb565_pixels=diff,native_draws=draws))
        if fixture.parent.random!=random or fixture.parent.audio_random!=audio:raise AssertionError('Composed warm RNG changed')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Composite production spans changed')
    report=dict(status='pass'if not failures else'fail',failures=failures,retail_sha256=RETAIL_SHA,asset_sha256=dict(drip=sha(drip),ripple=sha(ripple)),source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),pixel_samples=[]if args.state_only else samples,replays_per_case=args.repeat,median_warm_composite_pair_ms=statistics.median(times)if times else None,
        order='Existing children advance before parent step; newborn age0 renders on birth tick, first child step next tick. Period changed from1 to1000000 before tick121 to drain the last child.',scope='Actual native Drip emitter events instantiate actual original no-splash Ripple Init/Animate/Render; production independently spawns/advances/removes/renders real TRippleEffect via compiled actual Drip/Ripple methods/factory and asset providers. Compare linked birth/live ages/death and ordered mixed-texture software pixels, not port rings positioned from native traces. Identity owner/fixed camera/white vertices, explicit component-style scheduling; original sector iteration/map allocation/loader/component installation and audio/global RNG coupling remain separate. Length20 excludes splash recursion.',native_births=fixture.births,native_deaths=fixture.deaths,physics_range_invocations=len(random),native_audio_raw_draws=len(audio),guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if failures:raise SystemExit(1)

if __name__=='__main__':main()
