#!/usr/bin/env python3
"""Original Pixie swarm with explicitly empty nearby-character context."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
from fountain_probe import FountainFixture
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='6341fc0e14798943fd9b8e7140e8309135b9f952979f35639aefa617dee8c408'
WIDTH=256;HEIGHT=384


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'inline constexpr int32_t kPixieBespokeNumParts','// ----- X04/X05/X06/X07/X08 TFountainAnimator'),
        initialize=span(source,'    // --- Port of TPixieAnimator::Initialize','    if (attach_runtime_component) {'),
        advance_submit=span(source,'void TPixieEffect_Bespoke::Advance(','void TPixieEffect_Bespoke::TickAndSubmitForTest_BESPOKE('),
        map_dispatch=span(source,'class TPixieReferenceComponent final','void TPixieEffect_Bespoke::Initialize('),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def parse_asset(asset):
    if sha(asset)!=ASSET_SHA:raise ValueError('Wrong exact Pixies asset')
    meshes=[]
    for j in range(2):
        meshes.append(dict(vertices=[list(v)for v in struct.iter_unpack('<8f',asset[0x144+j*128:0x1c4+j*128])],
            indices=list(struct.unpack_from('<6H',asset,0x244+j*12)),texture=asset[0xa30+j*516:0xc30+j*516]))
    return meshes


def build_port(output,meshes,inputs,ticks):
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
enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct SQuadDrawItem{int corner_count=4,retail_texture=0;float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect{World world;S3DPoint position{};TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){throw std::runtime_error("Unexpected Pixie kill");}int GetFace()const{return 0;}void SetCommandDone(bool){};S3DPoint Pos()const{return position;}void SetPos(S3DPoint p){position=p;}const World&Transform()const{return world;}};
struct TObjectInstance:TEffect{TObjectInstance():TEffect(nullptr){}};constexpr int OBJCLASS_CHARACTER=12,OBJCLASS_PLAYER=11;
struct TMapPane{int calls=0;int FindObjectsInRange(S3DPoint,int*,int range,int,int cls){if(range!=90||(cls!=12&&cls!=11))throw std::runtime_error("Changed nearby context");++calls;return 0;}TObjectInstance*GetInstance(int){throw std::runtime_error("Nonempty Pixie character context");}};TMapPane MapPane;
std::ifstream rng;int rng_count=0;int random(int low,int high){int a,b,v;if(!(rng>>a>>b>>v)||a!=low||b!=high){fprintf(stderr,"RNG request %d..%d original %d..%d\n",low,high,a,b);throw std::runtime_error("Pixie native/port RNG order mismatch");}++rng_count;return v;}
'''
    def literal(v):
        s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
    verts=[]
    for m in meshes:verts.append('{'+','.join('{{'+','.join(literal(v)for v in p[:3])+'},{'+','.join(literal(v)for v in p[3:6])+'},{'+','.join(literal(v)for v in p[6:8])+'}}'for p in m['vertices'])+'}')
    topology='{'+','.join('{'+','.join(map(str,m['indices']))+'}'for m in meshes)+'}'
    initializer='void TPixieEffect_Bespoke::Initialize(bool attach_runtime_component){\n'+pieces['initialize']+'}\n'
    trailer=r'''
int main(int argc,char**argv){SMeshVertex vertices[2][4]={VERTICES};uint16_t indices[2][6]=INDICES;
 for(int runtime:{0,1}){rng.clear();rng.open(argv[1]);if(!rng)return 2;rng_count=0;MapPane.calls=0;TPixieEffect_Bespoke effect(nullptr);
  for(int j=0;j<2;++j){effect.vertices_[j].assign(vertices[j],vertices[j]+4);effect.indices_[j].assign(indices[j],indices[j]+6);effect.textures_[j]=j+1;}
  effect.Initialize(bool(runtime));
  for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
   printf("S %d %d %d %d %zu\n",runtime,tick,rng_count,MapPane.calls,renderer.draws.size());
   for(int i=0;i<25;++i){const auto&p=effect.pix_[i];printf("P %d %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",runtime,tick,i,p.time,p.pos.X,p.pos.Y,p.pos.Z,p.vel.X,p.vel.Y,p.vel.Z,p.scale.X,p.scale.Y,p.scale.Z);}
   for(const auto&d:renderer.draws){printf("D %d %d %u",runtime,tick,unsigned(d.key.texture)-1);for(int i=0;i<3;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}
  int a,b,c;if(rng>>a>>b>>c)throw std::runtime_error("Unused original RNG inputs");rng.close();}
}
'''.replace('VERTICES',','.join(verts)).replace('INDICES',topology).replace('TICKS',str(ticks))
    source=output/'pixie-port-component.cpp';source.write_text(prelude+'\n'+types+'\n'+initializer+'\n'+pieces['advance_submit']+'\n'+trailer)
    binary=output/'pixie-port-component';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    r=subprocess.run(cmd,capture_output=True,text=True);(output/'port-compile.log').write_text(r.stdout+r.stderr)
    if r.returncode:raise RuntimeError('Pixie production compile failed')
    r=subprocess.run([str(binary),str(inputs)],capture_output=True,text=True);(output/'port-trace.txt').write_text(r.stdout);(output/'port-run.log').write_text(r.stderr)
    if r.returncode:raise RuntimeError('Pixie production state/RNG trace failed')
    frames={}
    for line in r.stdout.splitlines():
        p=line.split();key=(int(p[1]),int(p[2]))
        if p[0]=='S':frames[key]=dict(random_count=int(p[3]),map_queries=int(p[4]),particles=[],draws=[])
        elif p[0]=='P':frames[key]['particles'].append(dict(index=int(p[3]),time=int(p[4]),values=[float(x)for x in p[5:]]))
        else:frames[key]['draws'].append(dict(mesh=int(p[3]),positions=[[float(x)for x in p[4+i*5:7+i*5]]for i in range(3)],uvs=[[float(x)for x in p[7+i*5:9+i*5]]for i in range(3)],indices=[0,1,2]))
    return frames,dict(command=cmd,generated_source_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),bodies=['Initialize state','Advance','Submit','src/math3d.cpp'],boundaries=['exact authored assets','identity owner/face zero','strict original RNG replay','empty nearby-character query provider','loader/component installation separate'])


class PixieFixture:
    observe_random=FountainFixture.observe_random
    def __init__(self,executable,asset,build=None):
        self.meshes=parse_asset(asset);self.software=SoftwareFixture(executable,WIDTH,HEIGHT,build=build);self.vm=self.software.vm
        self.software.set_texture(16,16,self.meshes[0]['texture'],format='ARGB4444')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.imagery=self.vm.allocate(0x200);self.objects=[self.vm.allocate(0x200)for _ in range(2)]
        self.vm.put_u32(self.animator,0x5ad758);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        vt=self.vm.allocate(0x200);self.vm.put_u32(self.owner,vt);done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vt+0x158,done)
        setpos=self.vm.allocate(64);self.vm.write(setpos,bytes.fromhex('8b4424048b108951108b50048951148b5008895118c20c00'));self.vm.put_u32(vt+8,setpos)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.matrix=self.vm.allocate(64)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x452060:32}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x48332c):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.random_pending=[];self.packets=[];self.map_queries=0;self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:uc.reg_write(UC_X86_REG_EAX,self.objects[self.vm.u32(sp+4)])
        elif address==0x452060:
            if self.vm.u32(sp+16)!=90 or self.vm.u32(sp+24)not in(11,12):raise AssertionError('Changed Pixie nearby-query contract')
            self.map_queries+=1;uc.reg_write(UC_X86_REG_EAX,0)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Pixie blend request changed')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if self.vm.u32(obj)!=0x100:raise AssertionError('Pixie matrix flags changed')
            self.packets.append(dict(mesh=self.objects.index(obj),matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.map_queries=0;self.vm.call(self.vm.u32(0x5ad758+24),this=self.animator)

    def animate(self):self.vm.call(self.vm.u32(0x5ad758+44),this=self.animator)

    def state(self):
        pointer=self.vm.u32(self.animator+0xfc);particles=[]
        for i in range(25):
            v=struct.unpack('<9fI',self.vm.uc.mem_read(pointer+i*40,40));particles.append(dict(index=i,time=v[9],values=list(v[:9])))
        return dict(random_count=len(self.random),map_queries=self.map_queries,particles=particles)

    def original_draws(self):
        self.packets=[];self.vm.call(self.vm.u32(0x5ad758+52),this=self.animator);draws=[]
        for packet in self.packets:
            self.vm.write(self.matrix,struct.pack('<16f',*packet['matrix']));points=[];mesh=self.meshes[packet['mesh']]
            for vertex in mesh['vertices']:
                self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(self.matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            for face in range(0,6,3):
                ids=mesh['indices'][face:face+3];p=[points[i]for i in ids];sx=[x[0]-x[1]for x in p];sy=[.5*(x[0]+x[1])-.867*x[2]for x in p]
                if(sx[1]-sx[0])*(sy[2]-sy[0])<(sx[2]-sx[0])*(sy[1]-sy[0]):continue
                draws.append(dict(mesh=packet['mesh'],positions=p,uvs=[mesh['vertices'][i][6:8]for i in ids],indices=[0,1,2]))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            self.software.set_texture(16,16,self.meshes[draw['mesh']]['texture'],format='ARGB4444');p=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=v[0]<WIDTH and 0<=v[1]<HEIGHT)for v in p):raise AssertionError('Pixie viewport clips geometry')
            self.software.draw([(*v,31,31,31,31,*uv)for v,uv in zip(p,draw['uvs'])],draw['indices'],z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=96);p.add_argument('--repeat',type=int,default=2);p.add_argument('--state-only',action='store_true');a=p.parse_args()
    if a.repeat<2:p.error('At least two exact replays required')
    a.output.mkdir(parents=True,exist_ok=True)
    before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(a.archive)as z:asset=z.read('Imagery/Misc/pixies.i3d')
    fixture=PixieFixture(a.executable,asset);fixture.reset()
    for tick in range(a.ticks):fixture.animate()
    random=fixture.random.copy();inputs=a.output/'native-random-inputs.txt';inputs.write_text(''.join(f'{lo} {hi} {v}\n'for lo,hi,v in random))
    port,compiled=build_port(a.output,fixture.meshes,inputs,a.ticks);samples=[x for x in(0,1,2,12,24,48,72,96)if x<=a.ticks];cases=[];errors=[];times=[];replay={}
    for repeat in range(a.repeat):
        fixture.reset()
        for tick in range(a.ticks+1):
            if tick:fixture.animate()
            original=fixture.state();modern=port[(1,tick)];preview=port[(0,tick)];state_errors=[]
            if repeat and original!=cases[tick]['original_state']:raise AssertionError('Pixie warm native state replay changed')
            if original['random_count']!=modern['random_count']:state_errors.append('RNG count')
            if original['map_queries']!=modern['map_queries']:state_errors.append('empty nearby query count')
            for x,y in zip(original['particles'],modern['particles']):
                if x['time']!=y['time']:state_errors.append(f'particle{x["index"]}.time')
                for field,(v,w)in enumerate(zip(x['values'],y['values'])):
                    if f32(v)!=f32(w):state_errors.append(f'particle{x["index"]}.float{field}')
            if {k:v for k,v in preview.items()if k!='map_queries'}!={k:v for k,v in modern.items()if k!='map_queries'}:state_errors.append('preview/map producer mismatch')
            image=None;diff=None;position_error=0;draws=[]
            if tick in samples and not a.state_only:
                start=time.perf_counter();draws=fixture.original_draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(modern['draws']);times.append((time.perf_counter()-start)*1000);image=(sha(color),sha(mcolor),sha(depth),sha(mdepth))
                if repeat==0:replay[tick]=image
                elif replay[tick]!=image:raise AssertionError('Pixie warm pixel/depth replay changed')
                if len(draws)!=len(modern['draws']):state_errors.append('submission count')
                for x,y in zip(draws,modern['draws']):
                    if x['mesh']!=y['mesh']:state_errors.append('authored object/texture selection')
                    if any(f32(v)!=f32(w)for p,q in zip(x['uvs'],y['uvs'])for v,w in zip(p,q)):state_errors.append('authored UVs')
                position_error=max([abs(v-w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['positions'],y['positions'])for v,w in zip(p,q)]or[0])
                diff=sum(v!=w for v,w in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                if not repeat:save_rgb565_png(a.output/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(a.output/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
            if not repeat:
                if state_errors or position_error>5e-5 or diff:errors.append(dict(tick=tick,state_errors=state_errors,max_position_error=position_error,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,original_state=original,port_state=modern,preview_state=preview,state_errors=state_errors,image_hashes=image,max_position_error=position_error,differing_rgb565_pixels=diff,original_draws=draws))
        if fixture.random!=random:raise AssertionError('Pixie native RNG warm replay changed')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Pixie source spans changed')
    r=dict(status='pass'if not errors else'fail',failures=errors,retail_sha256=RETAIL_SHA,asset_member='Imagery/Misc/pixies.i3d',asset_sha256=sha(asset),type_id='0x89abcde1',source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),sampled_pixel_ticks=[]if a.state_only else samples,replays_per_case=a.repeat,native_rng_calls=len(random),native_rng_input_sha256=sha(inputs.read_bytes()),median_warm_pixel_pair_ms=statistics.median(times)if times else None,
        original_initialize='0x4f3820',original_animate='0x4f39a0',original_render='0x4f3d10',scope='Original 25-particle native swarm state/RNG/pose/render versus compiled shared actual map/preview producer. Exact two authored ARGB4444 textures/geometry. Identity owner, face zero, explicit empty nearby-character context and normal owner SetPos storage. Current triangle-culling formula provided at imagery boundary. Base animator/extents/imagery boundary adapted; original matrices/projection/raster unchanged. Natural character interaction, map/component binding, arbitrary world/face pose, culling/lighting and Metal separate.',guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)

if __name__=='__main__':main()
