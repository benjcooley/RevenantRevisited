#!/usr/bin/env python3
"""FireSwarm: native one-shot scales/yaw and actual tube01 mesh/software pixels."""
import argparse,json,statistics,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha,f32
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='858011f22ae4b4e17be08667a8ea2cc018c71b672889cef822e12c07a57676c0'
WIDTH=1024;HEIGHT=2048
def q32(value):return struct.unpack('<f',f32(value))[0]


def parse_asset(asset):
    if sha(asset)!=ASSET_SHA:raise ValueError('Wrong exact FireSwarm asset')
    u=lambda a:struct.unpack_from('<I',asset,a)[0];r=lambda a:a+u(a);body=20+u(16)
    if(u(body),u(body+4),u(body+12),u(body+20),u(body+36),u(body+44))!=(0x1c,1,186,204,2,2):raise ValueError('FireSwarm v1 topology changed')
    vs=r(r(r(body+16)));fs=r(body+24);obj=r(body+48)+48;tex=r(body+40)+120
    if asset[obj:obj+32].split(b'\0')[0]!=b'tube01':raise ValueError('Wrong FireSwarm object')
    material,start,count,_=struct.unpack_from('<4H',asset,obj+32);runs=struct.unpack_from('<6H',asset,r(obj+40));first,nfaces=runs[4:]
    if(material,start,count,first,nfaces)!=(1,12,174,12,192):raise ValueError('FireSwarm local mesh/texture run changed')
    masks=struct.unpack_from('<4I',asset,tex+88)
    if masks!=(0xf00,0xf0,0xf,0xf000):raise ValueError('FireSwarm ARGB4444 masks changed')
    bits=r(r(tex+108));vertex_bytes=asset[vs+start*32:vs+(start+count)*32];index_bytes=asset[fs+first*6:fs+(first+nfaces)*6];indices=list(struct.unpack('<576H',index_bytes))
    if max(indices)>=174 or set(indices)!=set(range(174)):raise ValueError('FireSwarm local vertex coverage changed')
    return dict(vertices=[list(x)for x in struct.iter_unpack('<8f',vertex_bytes)],indices=indices,vertex_bytes=vertex_bytes,index_bytes=index_bytes,texture=asset[bits:bits+8192],vertex_offset=vs+start*32,index_offset=fs+first*6,texture_offset=bits)


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'class TFireSwarmEffect_Bespoke :','// *************************************************************************\n// * TBurnEffect_Bespoke'),
        constants=span(source,'constexpr const char* kFireSwarmBespokeImageryPath','}  // namespace'),
        initialize=span(source,'    texture_=tex.htexture; frameon_=0; cylth_=0.0f;','    if (attach_runtime_component) {'),
        advance_submit=span(source,'void TFireSwarmEffect_Bespoke::Advance(','void TFireSwarmEffect_Bespoke::TickAndSubmitForTest_BESPOKE('),
        map_dispatch=span(source,'class TFireSwarmReferenceComponent final','void TFireSwarmEffect_Bespoke::Initialize('),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,mesh,ticks):
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
struct SQuadDrawItem{bool retail_argb4444=false,retail_software_projection=false;int corner_count=4,retail_texture=0;float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};struct S3DTex{TTextureHandle htexture=2;};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect{World world;int kills=0;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){++kills;}void SetCommandDone(bool){};const World&Transform()const{return world;}};
'''
    initialize='void TFireSwarmEffect_Bespoke::Initialize(bool attach_runtime_component){S3DTex tex;\n'+pieces['initialize']+'}\n'
    trailer=r'''
int main(int argc,char**argv){std::ifstream input(argv[1],std::ios::binary);uint32_t nv,ni;input.read((char*)&nv,4);input.read((char*)&ni,4);std::vector<SMeshVertex>vertices(nv);std::vector<uint16_t>indices(ni);input.read((char*)vertices.data(),nv*32);input.read((char*)indices.data(),ni*2);
 if(!input||nv!=174||ni!=576)return 2;
 for(int runtime:{0,1}){TFireSwarmEffect_Bespoke effect(nullptr);effect.vertices_=vertices;effect.indices_=indices;effect.Initialize(bool(runtime));
  for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
   printf("S %d %d %d %d %d %.9g %.9g %.9g\n",runtime,tick,effect.frameon_,int(effect.alive_),effect.kills,effect.cylhscl_,effect.cylvscl_,effect.cylth_);
   for(const auto&d:renderer.draws){if(d.key.texture!=2||d.corner_count!=3)throw std::runtime_error("Incorrect authored FireSwarm binding");printf("D %d %d",runtime,tick);for(int i=0;i<3;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}}}
}
'''.replace('TICKS',str(ticks))
    src=output/'fireswarm-port-components.cpp';src.write_text(prelude+'\n'+types+'\n'+pieces['constants']+'\n'+initialize+'\n'+pieces['advance_submit']+'\n'+trailer)
    data=output/'tube01-mesh.bin';data.write_bytes(struct.pack('<2I',174,576)+mesh['vertex_bytes']+mesh['index_bytes'])
    binary=output/'fireswarm-port-components';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(src),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('FireSwarm production compile failed')
    result=subprocess.run([str(binary),str(data)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout);(output/'port-run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('FireSwarm production trace failed')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();key=(int(p[1]),int(p[2]))
        if p[0]=='S':frames[key]=dict(frame=int(p[3]),alive=bool(int(p[4])),kills=int(p[5]),values=[float(x)for x in p[6:]],draws=[])
        else:frames[key]['draws'].append(dict(positions=[[float(x)for x in p[3+i*5:6+i*5]]for i in range(3)],uvs=[[float(x)for x in p[6+i*5:8+i*5]]for i in range(3)]))
    return frames,dict(command=cmd,generated_source_sha256=sha(src.read_bytes()),binary_sha256=sha(binary.read_bytes()),mesh_input_sha256=sha(data.read_bytes()),bodies=['actual Initialize state','Advance','Submit','src/math3d.cpp'],ownership_modes=['preview','actual map owner'],boundaries=['exact tube01 vertices/indices/texture1','identity owner/no spell','normal kill request recorded','asset loading and component installation separate'])


class FireSwarmFixture:
    def __init__(self,executable,asset,build=None):
        self.mesh=parse_asset(asset);self.software=SoftwareFixture(executable,WIDTH,HEIGHT,build=build);self.vm=self.software.vm
        self.software.set_texture(64,64,self.mesh['texture'],format='ARGB4444')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200)
        self.vm.put_u32(self.animator,0x5ac0cc);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        vt=self.vm.allocate(0x200);self.vm.put_u32(self.owner,vt);done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vt+0x158,done)
        flags=self.vm.allocate(16);self.vm.write(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');self.vm.put_u32(vt+0x40,flags)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.matrix=self.vm.allocate(64)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        self.packets=[];self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=1:raise AssertionError('Native FireSwarm did not select tube01/object1')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('FireSwarm alpha request changed')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4)
            if self.vm.u32(obj)!=0x100:raise AssertionError('FireSwarm matrix flags changed')
            self.packets.append(list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def reset(self):self.software.restore();self.packets=[];self.vm.call(self.vm.u32(0x5ac0cc+24),this=self.animator)

    def animate(self):
        if not self.vm.u32(self.owner+8)&0x1000:self.vm.call(self.vm.u32(0x5ac0cc+44),this=self.animator)

    def state(self):return dict(frame=self.vm.u32(self.animator+0xfc),alive=not bool(self.vm.u32(self.owner+8)&0x1000),values=list(struct.unpack('<3f',self.vm.uc.mem_read(self.animator+0x100,12))))

    def original_draws(self):
        self.packets=[]
        if self.state()['alive']:self.vm.call(self.vm.u32(0x5ac0cc+52),this=self.animator)
        draws=[]
        for matrix in self.packets:
            self.vm.write(self.matrix,struct.pack('<16f',*matrix));points=[]
            for vertex in self.mesh['vertices']:
                self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(self.matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            for face in range(0,576,3):
                ids=self.mesh['indices'][face:face+3];p=[points[i]for i in ids]
                # Explicit current-frontend culling provider; independent device
                # culling is not asserted by this shared-raster comparison.
                sx=[q32(x[0]-x[1])for x in p];sy=[q32(q32(.5*q32(x[0]+x[1]))-q32(q32(.867)*x[2]))for x in p]
                if q32(q32(sx[1]-sx[0])*q32(sy[2]-sy[0]))<q32(q32(sx[2]-sx[0])*q32(sy[1]-sy[0])):continue
                draws.append(dict(positions=p,uvs=[self.mesh['vertices'][i][6:8]for i in ids]))
        return draws

    def pixels(self,draws):
        self.software.clear()
        if draws:
            points=[p for d in draws for p in d['positions']];uvs=[u for d in draws for u in d['uvs']];projected=self.software.project(points,camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('FireSwarm viewport clips original authored shape')
            self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,uvs)],list(range(len(points))),z_enabled=True,z_write=False,instruction_limit=50000000)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=78);p.add_argument('--repeat',type=int,default=2);p.add_argument('--state-only',action='store_true');a=p.parse_args()
    if a.repeat<2:p.error('At least two complete replays required')
    a.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(a.archive)as z:asset=z.read('Imagery/Magic/fireswarm.i3d')
    fixture=FireSwarmFixture(a.executable,asset);port,compiled=build_port(a.output,fixture.mesh,a.ticks);samples=[x for x in(0,1,12,24,30,36,48,60,72,75,76,78)if x<=a.ticks];cases=[];failures=[];times=[];replay={}
    for repeat in range(a.repeat):
        fixture.reset()
        for tick in range(a.ticks+1):
            if tick:fixture.animate()
            original=fixture.state();modern=port[(1,tick)];preview=port[(0,tick)];errors=[]
            if len(original['values'])!=3 or len(modern['values'])!=3:raise AssertionError('FireSwarm must compare all three scale/yaw fields')
            if repeat and original!=cases[tick]['original_state']:raise AssertionError('FireSwarm warm state changed')
            if(original['frame'],original['alive'])!=(modern['frame'],modern['alive']):errors.append('frame/lifetime')
            for field,(v,w)in enumerate(zip(original['values'],modern['values'])):
                if f32(v)!=f32(w):errors.append(f'float{field}')
            if {k:v for k,v in modern.items()if k!='kills'}!={k:v for k,v in preview.items()if k!='kills'}:errors.append('preview/map producer mismatch')
            if modern['kills']!=int(not modern['alive']):errors.append('normal owner kill request')
            image=None;diff=None;corner=0;draws=[]
            if tick in samples and not a.state_only:
                start=time.perf_counter();draws=fixture.original_draws();color,depth=fixture.pixels(draws);mcolor,mdepth=fixture.pixels(modern['draws']);times.append((time.perf_counter()-start)*1000);image=(sha(color),sha(mcolor),sha(depth),sha(mdepth))
                if repeat==0:replay[tick]=image
                elif replay[tick]!=image:raise AssertionError('FireSwarm warm pixel/depth changed')
                if len(draws)!=len(modern['draws']):errors.append('triangle submission count')
                if any(f32(v)!=f32(w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['uvs'],y['uvs'])for v,w in zip(p,q)):errors.append('authored UVs/indices')
                corner=max([abs(v-w)for x,y in zip(draws,modern['draws'])for p,q in zip(x['positions'],y['positions'])for v,w in zip(p,q)]or[0]);diff=sum(v!=w for v,w in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',mcolor)))
                if not repeat:save_rgb565_png(a.output/f'retail-{tick:03d}.png',color,WIDTH,HEIGHT);save_rgb565_png(a.output/f'port-{tick:03d}.png',mcolor,WIDTH,HEIGHT)
            if not repeat:
                if errors or corner>5e-5 or diff:failures.append(dict(tick=tick,state_errors=errors,max_corner_error=corner,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,original_state=original,port_state=modern,preview_state=preview,state_errors=errors,image_hashes=image,max_corner_error=corner,differing_rgb565_pixels=diff,original_draws=draws))
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('FireSwarm source spans changed')
    r=dict(status='pass'if not failures else'fail',failures=failures,retail_sha256=RETAIL_SHA,asset_member='Imagery/Magic/fireswarm.i3d',asset_sha256=sha(asset),type_id='0x582c1e78',source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),sampled_pixel_ticks=[]if a.state_only else samples,replays_per_case=a.repeat,median_warm_pixel_pair_ms=statistics.median(times)if times else None,
        original_initialize='0x4f0130',original_animate='0x4f0160',original_render='0x4f0270',scope='Original one-shot scale/yaw/lifecycle and tube01 object1 authored mesh/texture versus compiled shared preview/map producer. No RNG/audio; owner spell null, identity world. Current frontend triangle-culling formula provided at imagery boundary; independent device culling not asserted. Original matrix/vertex/projection/raster executes unmodified with selected 1024x2048 viewport, camera zero/zdist1925/white input vertices, no shape or camera fitting. Original software skips triangle edges above640px X/480px Y: early tall-cylinder samples are empty, so early visible appearance is not verified. Later nonempty samples, lifecycle/pose/mesh and size-rejection diagnostic are retained separately. Real loader/component/map context, normals/lighting/blend/modern GPU remain separate.',
        original_software_limits=dict(oversized_triangle_skip_branch='0x56d9fd..0x56da7f ->0x56dba7',maximum_edge_x=640,maximum_edge_y=480,larger_viewport_does_not_remove_limit=True,early_nonempty_appearance_verified=False),guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps({k:v for k,v in r.items()if k!='cases'},indent=2))
    if failures:raise SystemExit(1)

if __name__=='__main__':main()
