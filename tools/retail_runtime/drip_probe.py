#!/usr/bin/env python3
"""Drip emitter: native state/head pixels and emitted Ripple request boundary."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='497ea6ba74c800c43e63ab509c12e2298ad4fbdce4089fddb3bc888d6bd195d2'
WIDTH=320;HEIGHT=768;INDICES=(0,1,3,3,2,0)
PROFILES={'short_no_splash':(20,128,1),'caverns_parameters':(20,200,35),'default_parameters':(64,128,48)}


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'class TDripEffect :','// *******************\n// * Drip Animator'),
        constants=span(source,'constexpr double kWaterTicksPerSecond','int32_t WaterRandom('),
        random_helper=span(source,'int32_t WaterRandom(','// Split time at authored event boundaries.'),
        time_helpers=span(source,'template <typename AdvanceChildren, typename Tick>','T3DImagery* LoadWaterImagery('),
        quad=span(source,'void SubmitWaterQuad(','template <typename Effect>\nclass TWaterReferenceComponent'),
        advance_submit=span(source,'void TDripEffect::Advance(','void TDripEffect::TickAndSubmitForTest('),
        map_dispatch=span(source,'template <typename Effect>\nclass TWaterReferenceComponent','template <typename Effect>\nclass TWaterReferenceBuilder'),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,asset,inputs,ticks,params):
    pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
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
struct TObjectImagery{};struct SObjectDef{};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TObjectInstance{World world;S3DPoint pos{};virtual~TObjectInstance()=default;const S3DPoint&Pos()const{return pos;}const World&Transform()const{return world;}};
struct TEffect:TObjectInstance{TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual void Pulse(){};virtual void Load(RTInputStream,int32_t,int32_t){};virtual void Save(RTOutputStream){}};
void log_info(const char*,...){};
std::ifstream rng;int rng_count=0,current_raw=-1;bool raw_consumed=false;
extern "C" int rand(void){if(current_raw<0||raw_consumed)throw std::runtime_error("Production requested a raw RNG draw absent in retail");raw_consumed=true;return current_raw;}
struct Event{int tick,x,y,z,length;};std::vector<Event>events;int now=0;
// Explicit child factory/lifecycle boundary: capture request, no child rendering
// or child initialization RNG claim. Independent Ripple probe verifies length20.
struct TRippleEffect{size_t event;static TRippleEffect*SpawnForTest(const S3DPoint&p){events.push_back({now,p.x,p.y,p.z,0});auto*c=new TRippleEffect;c->event=events.size()-1;return c;}
 void SetLength(int n){events[event].length=n;}void Advance(double){};void Submit(EFxDebugMode){};bool IsAlive()const{return false;}};
'''
    records=struct.unpack_from('<32f',asset,0xfc)
    def literal(v):
        s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
    vertices='{'+','.join('{{'+','.join(literal(v)for v in records[i:i+3])+'},{'+','.join(literal(v)for v in records[i+3:i+6])+'},'+literal(records[i+6])+','+literal(records[i+7])+'}'for i in range(0,32,8))+'}'
    unsupported=r'''
TDripEffect::~TDripEffect()=default;
void TDripEffect::Initialize(){throw std::runtime_error("Real asset/component installation is outside fixture");}
void TDripEffect::Pulse(){throw std::runtime_error("Owner Pulse is outside fixture");}
void TDripEffect::Load(RTInputStream,int32_t,int32_t){throw std::runtime_error("Serialized owner load is outside fixture");}
void TDripEffect::Save(RTOutputStream){throw std::runtime_error("Serialized owner save is outside fixture");}
'''
    rng_wrapper=r'''
int WaterRandom(int low,int high){int a,b,v,raw;if(!(rng>>a>>b>>v>>raw)||a!=low||b!=high)throw std::runtime_error("Drip native physics RNG order mismatch");
 current_raw=raw;raw_consumed=false;int value=production_random::WaterRandom(low,high);
 if(value!=v||raw_consumed!=(raw>=0))throw std::runtime_error("Production WaterRandom value/raw draw count mismatch");++rng_count;return value;}
'''
    trailer=r'''
int main(int argc,char**argv){rng.open(argv[1]);if(!rng)return 2;TDripEffect effect(nullptr);S3DVertex vertices[4]=VERTICES;effect.drop_vertices_.assign(vertices,vertices+4);effect.drop_texture_=1;
 effect.SetParams(std::atoi(argv[2]),std::atoi(argv[3]),std::atoi(argv[4]));bool valid=false;
 for(now=0;now<=TICKS;++now){if(now)effect.Advance(1.0/24.0);valid|=!effect.dead_;renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
  printf("S %d %d %d %d %d %zu %.9g %.9g %.9g %.9g %.9g %.9g\n",now,int(effect.dead_),effect.time_,rng_count,int(valid),events.size(),effect.drop_pos_.X,effect.drop_pos_.Y,effect.drop_pos_.Z,effect.drop_vel_.X,effect.drop_vel_.Y,effect.drop_vel_.Z);
  for(const auto&d:renderer.draws){printf("D %d",now);for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}
 for(const auto&e:events)printf("E %d %d %d %d %d\n",e.tick,e.x,e.y,e.z,e.length);
 int a,b,c;if(rng>>a>>b>>c)throw std::runtime_error("Unused Drip native physics RNG inputs");}
'''.replace('VERTICES',vertices).replace('TICKS',str(ticks))
    src=output/'drip-port-components.cpp';src.write_text(prelude+'\nnamespace production_random {\n'+pieces['random_helper']+'}\n'+rng_wrapper+'\n'+types+'\n'+unsupported+'\n'+pieces['constants']+'\n'+pieces['time_helpers']+'\n'+pieces['quad']+'\n'+pieces['advance_submit']+'\n'+trailer)
    binary=output/'drip-port-components';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(src),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    r=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError('Drip production compile failed')
    r=subprocess.run([str(binary),str(inputs),*map(str,params)],capture_output=True,text=True);(output/'port-trace.txt').write_text(r.stdout);(output/'port-run.log').write_text(r.stderr)
    if r.returncode:raise RuntimeError('Drip production state/RNG trace failed')
    frames={};events=[]
    for line in r.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(dead=bool(int(p[2])),time=int(p[3]),random_count=int(p[4]),values_valid=bool(int(p[5])),event_count=int(p[6]),values=[float(x)for x in p[7:]],draws=[])
        elif p[0]=='D':frames[tick]['draws'].append(dict(positions=[[float(x)for x in p[2+i*5:5+i*5]]for i in range(4)],uvs=[[float(x)for x in p[5+i*5:7+i*5]]for i in range(4)]))
        else:events.append(dict(tick=tick,arguments=[int(x)for x in p[2:]]))
    return frames,events,dict(command=cmd,generated_source_sha256=sha(src.read_bytes()),binary_sha256=sha(binary.read_bytes()),bodies=['TDripEffect inline constructor/SetParams','WaterRandom actual helper','Advance','Submit','AdvanceWaterTime','WaterDropPosition','SubmitWaterQuad','src/math3d.cpp'],boundaries=['raw authored asset/identity owner','native physics raw CRT RNG subset replay; native audio RNG separately observed','child factory/lifecycle/submission captured only','owner Initialize/Pulse/Load/Save unsupported','map component dispatch source retained, installation separate'])


class DripFixture:
    def __init__(self,executable,asset,build=None):
        if sha(asset)!=ASSET_SHA:raise ValueError('Wrong exact Drip asset')
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT,build=build);self.vm=self.software.vm;self.software.set_texture(64,64,asset[0x2d4:0x2d4+8192],format='ARGB4444')
        records=struct.unpack_from('<32f',asset,0xfc);self.points=[records[i:i+3]for i in range(0,32,8)];self.uvs=[records[i+6:i+8]for i in range(0,32,8)]
        if struct.unpack_from('<6H',asset,0x17c)!=INDICES:raise ValueError('Drip authored topology changed')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200)
        self.vm.put_u32(self.animator,0x5ac83c);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        vt=self.vm.allocate(0x240);self.vm.put_u32(self.owner,vt)
        # Execute actual retail object GetParams/SetParams with original owner layout.
        self.vm.put_u32(vt+0x204,self.vm.u32(0x5ac630+0x204));self.vm.put_u32(vt+0x200,self.vm.u32(0x5ac630+0x200))
        done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vt+0x158,done)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.matrix=self.vm.allocate(64)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x4f1140:16,0x49c430:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in(0x483300,0x483312,0x48332c,0x58c5a3):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.audio_random=[];self.random_pending=[];self.events=[];self.packets=[];self.tick=0;self.values_valid=False;self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Drip authored object changed')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Drip blend request changed')
        elif address==0x4f1140:self.events.append(dict(tick=self.tick,arguments=list(struct.unpack('<4i',self.vm.uc.mem_read(sp+4,16)))))
        elif address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff) # Explicit unavailable sound; native audio selection RNG still executes.
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if self.vm.u32(obj)!=0x100:raise AssertionError('Drip matrix poseflags changed')
            self.packets.append(list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def observe_random(self,uc,address,size,user):
        if address==0x483300:
            sp=uc.reg_read(UC_X86_REG_ESP);lo,hi=struct.unpack('<2i',self.vm.uc.mem_read(sp+4,8));self.random_pending.append([lo,hi,self.vm.u32(sp),-1])
        elif address==0x58c5a3:
            if not self.random_pending:raise AssertionError('Unexpected raw RNG outside Drip range helper')
            self.random_pending[-1][3]=uc.reg_read(UC_X86_REG_EAX)
        else:
            lo,hi,site,raw=self.random_pending.pop();value=struct.unpack('<i',struct.pack('<I',uc.reg_read(UC_X86_REG_EAX)))[0]
            if site==0x4f135c:
                if(lo,hi)!=(49,51):raise AssertionError('Drip audio selection range changed')
                self.audio_random.append((lo,hi,value,raw))
            else:
                if site!=0x4f125e:raise AssertionError(f'Unknown native Drip RNG site{site:x}')
                self.random.append((lo,hi,value,raw))

    def reset(self,params):
        self.software.restore();self.random=[];self.audio_random=[];self.random_pending=[];self.events=[];self.packets=[];self.tick=0;self.values_valid=False
        self.vm.write(self.owner+0x184,struct.pack('<3i',*params));self.vm.write(self.animator+0x11c,struct.pack('<3i',64,128,48));self.vm.call(self.vm.u32(0x5ac83c+24),this=self.animator)

    def animate(self):
        self.tick+=1;self.vm.call(self.vm.u32(0x5ac83c+44),this=self.animator);self.values_valid|=not bool(self.vm.u32(self.animator+0x114))

    def state(self):return dict(dead=bool(self.vm.u32(self.animator+0x114)),time=self.vm.u32(self.animator+0x118),random_count=len(self.random),values_valid=self.values_valid,event_count=len(self.events),values=list(struct.unpack('<6f',self.vm.uc.mem_read(self.animator+0xfc,24))))

    def original_draws(self):
        self.packets=[];self.vm.call(self.vm.u32(0x5ac83c+52),this=self.animator);draws=[]
        for matrix in self.packets:
            self.vm.write(self.matrix,struct.pack('<16f',*matrix));points=[]
            for point in self.points:
                self.vm.write(self.point,struct.pack('<3f',*point));self.vm.call(0x43ad80,(self.matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=points,uvs=self.uvs))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            p=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=v[0]<WIDTH and 0<=v[1]<HEIGHT)for v in p):raise AssertionError('Drip viewport clips geometry')
            self.software.draw([(*v,31,31,31,31,*uv)for v,uv in zip(p,draw['uvs'])],INDICES,z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=192);p.add_argument('--repeat',type=int,default=2);p.add_argument('--state-only',action='store_true');a=p.parse_args()
    if a.repeat<2:p.error('At least two exact replays required')
    a.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(a.archive)as z:asset=z.read('Imagery/Magic/drip.i3d')
    fixture=DripFixture(a.executable,asset);allcases=[];failures=[];profiles=[];times=[]
    for name,params in PROFILES.items():
        out=a.output/name;out.mkdir(exist_ok=True);fixture.reset(params)
        for tick in range(a.ticks):fixture.animate()
        random=fixture.random.copy();audio=fixture.audio_random.copy();events=fixture.events.copy();inputs=out/'native-physics-random.txt';inputs.write_text(''.join(f'{lo} {hi} {v} {raw}\n'for lo,hi,v,raw in random));(out/'native-audio-random.json').write_text(json.dumps(audio,indent=2)+'\n')
        port,pevents,compiled=build_port(out,asset,inputs,a.ticks,params);samples=sorted({x for x in(0,1,2,3,12,20,24,50,96,160,a.ticks)if x<=a.ticks}|{e['tick']-1 for e in events}|{e['tick']for e in events});replay={};cases=[]
        if events!=pevents:failures.append(dict(profile=name,error='emitted ripple request mismatch',native=events,port=pevents))
        for repeat in range(a.repeat):
            fixture.reset(params)
            for tick in range(a.ticks+1):
                if tick:fixture.animate()
                original=fixture.state();modern=port[tick];state_errors=[]
                if len(original['values'])!=6 or len(modern['values'])!=6:raise AssertionError('Drip must compare all six valid state fields')
                if repeat and original!=cases[tick]['original_state']:raise AssertionError('Drip warm native state replay changed')
                for field in('dead','time','random_count','values_valid','event_count'):
                    if original[field]!=modern[field]:state_errors.append(field)
                if original['values_valid']:
                    for field,(v,w)in enumerate(zip(original['values'],modern['values'])):
                        if f32(v)!=f32(w):state_errors.append(f'float{field}')
                image=None;diff=None;corner=0;draws=[]
                if tick in samples and not a.state_only:
                    start=time.perf_counter();draws=fixture.original_draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(modern['draws']);times.append((time.perf_counter()-start)*1000);image=(sha(color),sha(mcolor),sha(depth),sha(mdepth))
                    if repeat==0:replay[tick]=image
                    elif replay[tick]!=image:raise AssertionError('Drip warm pixels/depth replay changed')
                    if len(draws)!=len(modern['draws']):state_errors.append('head submission count')
                    if any(f32(v)!=f32(w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['uvs'],y['uvs'])for v,w in zip(p,q)):state_errors.append('authored UVs')
                    corner=max([abs(v-w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['positions'],y['positions'])for v,w in zip(p,q)]or[0]);diff=sum(v!=w for v,w in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                    if not repeat:save_rgb565_png(out/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(out/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
                if not repeat:
                    if state_errors or corner>5e-5 or diff:failures.append(dict(profile=name,tick=tick,state_errors=state_errors,max_corner_error=corner,differing_rgb565_pixels=diff))
                    cases.append(dict(tick=tick,original_state=original,port_state=modern,state_errors=state_errors,image_hashes=image,max_corner_error=corner,differing_rgb565_pixels=diff,original_draws=draws))
            if fixture.random!=random or fixture.audio_random!=audio or fixture.events!=events:raise AssertionError('Drip warm RNG/event replay changed')
        profiles.append(dict(name=name,params=list(params),physics_rng_calls=len(random),audio_rng_calls=len(audio),emitted_requests=events,sampled_pixel_ticks=[]if a.state_only else samples,port_components=compiled));allcases.append(dict(profile=name,cases=cases))
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Drip source spans changed')
    r=dict(status='pass'if not failures else'fail',failures=failures,retail_sha256=RETAIL_SHA,asset_member='Imagery/Magic/drip.i3d',asset_sha256=sha(asset),type_id='0x5975abde',source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),case_count=sum(len(x['cases'])for x in allcases),replays_per_case=a.repeat,profiles=profiles,median_warm_head_pixel_pair_ms=statistics.median(times)if times else None,
        original_initialize='0x4f10c0',original_animate='0x4f11e0',original_render='0x4f13c0',scope='Native Drip emitter state, physics RNG subset, falling authored head and emitted Ripple parameters against compiled actual shared Advance/Submit and time/quad helpers. Three explicit parameter sets, identity owner and fixed declared camera. Native audio selection RNG49..51 executes and is observed separately; port has no audio draw, full audio/global RNG parity remains open. Child factory/lifecycle/rendering/initialization-RNG is an explicit request boundary, not verified composite; independent no-splash Ripple probe covers length20. Native unused pos/vel before first spawn are not claimed. Asset/component/serialization/map installation, arbitrary poses, lighting/blend/culling/modern GPU separate.',guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=allcases)
    (a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items()if k!='cases'},indent=2))
    if failures:raise SystemExit(1)

if __name__=='__main__':main()
