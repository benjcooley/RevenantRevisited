#!/usr/bin/env python3
"""Original Streamer controller, four authored meshes and both ownership modes."""
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
ASSET_SHA='297a224bb097d1993aa4b85d9a31e719a17d1dd013271630b1de28a12a6af9cb'
WIDTH=256;HEIGHT=384;PIXELS=WIDTH*HEIGHT


def production_spans():
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    return dict(types=span(header,'class TStreamerEffect_Bespoke :','// --- X11 TRibbonAnimator_Bespoke'),
        helpers=span(source,'void TStreamerEffect_Bespoke::InitStreamerSnap(','// =========================================================================\n// X11 TRibbonAnimator_Bespoke'),
        advance_submit=span(source,'void TStreamerEffect_Bespoke::Advance(','void TStreamerEffect_Bespoke::TickAndSubmitForTest_BESPOKE'),
        initialize_state=span(source,'        scl_init_[j]=0.15f*float(j+1);','    runtime_owned_=attach_runtime_component;')+'    runtime_owned_=attach_runtime_component;',
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def parse_asset(data):
    if sha(data)!=ASSET_SHA:raise ValueError('Wrong Streamer imagery revision')
    u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a);body=20+u(16)
    verts=r(r(r(body+16)));faces=r(body+24);objects=r(body+48);textures=r(body+40);meshes=[]
    if(u(body+44),u(body+36))!=(4,4):raise ValueError('Streamer object/texture count changed')
    for j in range(4):
        obj=objects+j*48;start,count=struct.unpack_from('<2H',data,obj+34);runs=struct.unpack_from('<10H',data,r(obj+40))
        face_start,face_count=runs[(j+1)*2:(j+2)*2];desc=textures+j*120;offsets=r(desc+108);bits=r(offsets)
        if(count,face_count)!= (4,2)or struct.unpack_from('<2I',data,desc+8)!=(64,64):raise ValueError('Streamer geometry/texture dimensions changed')
        meshes.append(dict(vertices=[list(x)for x in struct.iter_unpack('<8f',data[verts+start*32:verts+(start+count)*32])],
            indices=list(struct.unpack_from('<6H',data,faces+face_start*6)),texture=data[bits:bits+8192],
            vertex_offset=hex(verts+start*32),faces_offset=hex(faces+face_start*6),texture_offset=hex(bits)))
    return meshes


def build_port(output,meshes,ticks):
    pieces=production_spans();types=pieces['types'].replace('  private:','  public:')
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxBlend:uint8_t{AdditiveStraight};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxDebugMode:uint8_t{Normal};
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct SQuadDrawItem{int corner_count=4,retail_texture=0;float world_pos[4][3]{},uv[4][2]{};
 struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};
TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{};struct SObjectDef{};struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TEffect{World world;int kill_count=0;TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;
 virtual void OffScreen(){};void KillThisEffect(){++kill_count;}void SetCommandDone(bool){};int GetFace()const{return 0;}
 const World&Transform()const{return world;}};
'''
    initializers=[]
    def lit(v):
        s=format(v,'.9g');return s+'f'if'.'in s or'e'in s else s+'.0f'
    for mesh in meshes:
        initializers.append('{'+','.join('{{'+','.join(lit(v)for v in p[:3])+'},{'+','.join(lit(v)for v in p[3:6])+'},{'+','.join(lit(v)for v in p[6:8])+'}}'for p in mesh['vertices'])+'}')
    topology='{'+','.join('{'+','.join(map(str,m['indices']))+'}'for m in meshes)+'}'
    trailer=r'''
int main(){SMeshVertex vertices[4][4]={VERTICES};uint16_t indices[4][6]=INDICES;
 for(int runtime:{0,1}){TStreamerEffect_Bespoke effect(nullptr);
  for(int j=0;j<4;++j){effect.authored_[j].vertices.assign(vertices[j],vertices[j]+4);effect.authored_[j].indices.assign(indices[j],indices[j]+6);effect.authored_[j].texture=j+1;
  }
  effect.Initialize(bool(runtime));
  for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
   printf("S %d %d %d %d %d %zu\n",runtime,tick,effect.frameon_,int(effect.alive_),effect.kill_count,renderer.draws.size());
   for(int j=0;j<4;++j){printf("A %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g\n",runtime,tick,j,effect.dscl_[j],effect.scl_init_[j],effect.th_[j],effect.dth_[j],effect.h_[j],effect.dh_[j]);
    for(int i=0;i<50;++i){const auto&p=effect.stream_[j][i];printf("P %d %d %d %d %d %.9g %.9g %.9g %.9g\n",runtime,tick,j,i,p.count,p.pos.X,p.pos.Y,p.pos.Z,p.scl);}}
   for(const auto&d:renderer.draws){printf("D %d %d %u",runtime,tick,unsigned(d.key.texture)-1);for(int k=0;k<3;++k)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[k][0],d.world_pos[k][1],d.world_pos[k][2],d.uv[k][0],d.uv[k][1]);puts("");}
  }
 }
}
'''.replace('VERTICES',','.join(initializers)).replace('INDICES',topology).replace('TICKS',str(ticks))
    # Asset/component providers are explicit boundaries; the initialization
    # state loop below is extracted unchanged from the actual map producer.
    initialize='void TStreamerEffect_Bespoke::Initialize(bool attach_runtime_component) {\nfor(int j=0;j<kStreamerMaxStreams;++j) {\n'+pieces['initialize_state']+'\n}\n'
    source=output/'streamer-port-components.cpp';source.write_text(prelude+'\n'+types+'\n'+initialize+'\n'+pieces['helpers']+'\n'+pieces['advance_submit']+'\n'+trailer)
    binary=output/'streamer-port-components';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Streamer production compile failed; see port-compile.log')
    text=subprocess.check_output([str(binary)],text=True);(output/'port-trace.txt').write_text(text);frames={}
    for line in text.splitlines():
        p=line.split();key=(int(p[1]),int(p[2]))
        if p[0]=='S':frames[key]=dict(frameon=int(p[3]),alive=bool(int(p[4])),kill_count=int(p[5]),streams=[],particles=[],draws=[])
        elif p[0]=='A':frames[key]['streams'].append([float(x)for x in p[4:]])
        elif p[0]=='P':frames[key]['particles'].append(dict(stream=int(p[3]),slot=int(p[4]),count=int(p[5]),values=[float(x)for x in p[6:]]))
        else:frames[key]['draws'].append(dict(stream=int(p[3]),positions=[[float(x)for x in p[4+k*5:7+k*5]]for k in range(3)],uvs=[[float(x)for x in p[7+k*5:9+k*5]]for k in range(3)],indices=[0,1,2]))
    return frames,dict(command=command,generated_source_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        bodies=['Initialize state loop','InitStreamerSnap','AddStreamerSnap','Advance','Submit','src/math3d.cpp'],
        ownership_modes=['preview runtime_owned=false','actual map component runtime_owned=true'],
        boundaries=['authoredreadygeometry/texture','identityowner/no spell/face0','normal KillThisEffect request recorded','asset/component binding remains separate'])


class StreamerFixture:
    def __init__(self,executable,meshes):
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT);self.vm=self.software.vm;self.meshes=meshes
        self.software.set_texture(64,64,meshes[0]['texture'],format='RGB565')
        self.animator=self.vm.allocate(0x200);self.owner=self.vm.allocate(0x400);self.imagery=self.vm.allocate(0x200)
        self.objects=[self.vm.allocate(0x200)for _ in range(4)];self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.vm.put_u32(self.animator,0x5abe4c);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.animator+8,self.imagery)
        vt=self.vm.allocate(0x200);self.vm.put_u32(self.owner,vt);done=self.vm.allocate(16);self.vm.write(done,b'\x31\xc0\xc2\x04\x00');self.vm.put_u32(vt+0x158,done)
        flags=self.vm.allocate(16);self.vm.write(flags,b'\x8b\x44\x24\x04\x89\x41\x08\xc2\x04\x00');self.vm.put_u32(vt+0x40,flags)
        self.packets=[];self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x49c430:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        self.software.clear();self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff)  # Explicit unavailable sound; no sound parity claim.
        elif address==0x40eef0:uc.reg_write(UC_X86_REG_EAX,self.objects[self.vm.u32(sp+4)])
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(8,1):raise AssertionError('Unexpected Streamer blend request')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4);self.packets.append(dict(stream=self.objects.index(obj),matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def reset(self):
        self.software.restore();self.packets=[];self.vm.call(0x4efbd0,this=self.animator)

    def animate(self):
        if not self.vm.u32(self.owner+8)&0x1000:self.vm.call(0x4efd50,this=self.animator)

    def state(self):
        streams=[];particles=[]
        alive=not bool(self.vm.u32(self.owner+8)&0x1000)
        for j in range(4):
            streams.append([struct.unpack('<f',self.vm.uc.mem_read(self.animator+offset+j*4,4))[0]for offset in(0x100,0x110,0x120,0x130,0x140,0x150)])
            pointer=self.vm.u32(self.animator+0x160+j*4)
            if not pointer:continue
            for i in range(50):
                values=struct.unpack('<4fi',self.vm.uc.mem_read(pointer+i*20,20));particles.append(dict(stream=j,slot=i,count=values[4],values=list(values[:4])))
        return dict(frameon=self.vm.u32(self.animator+0xfc),alive=alive,streams=streams,particles=particles)

    def original_draws(self):
        self.packets=[]
        if self.state()['alive']:self.vm.call(0x4efe50,this=self.animator)
        draws=[]
        for packet in self.packets:
            matrix=self.vm.allocate(64);self.vm.write(matrix,struct.pack('<16f',*packet['matrix']));mesh=self.meshes[packet['stream']];points=[]
            for v in mesh['vertices']:
                self.vm.write(self.point,struct.pack('<3f',*v[:3]));self.vm.call(0x43ad80,(matrix,self.point,self.result));points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            # Match the explicit frontend culling boundary with the port's
            # current triangle submission; source vertex/index data is retained.
            for face in range(0,6,3):
                ids=mesh['indices'][face:face+3];p=[points[i]for i in ids];sx=[x[0]-x[1]for x in p];sy=[.5*(x[0]+x[1])-.867*x[2]for x in p]
                if(sx[1]-sx[0])*(sy[2]-sy[0])<(sx[2]-sx[0])*(sy[1]-sy[0]):continue
                draws.append(dict(stream=packet['stream'],positions=p,uvs=[mesh['vertices'][i][6:8]for i in ids],indices=[0,1,2]))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            self.software.set_texture(64,64,self.meshes[draw['stream']]['texture'],format='RGB565')
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Streamer viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],draw['indices'],z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--ticks',type=int,default=103);parser.add_argument('--repeat',type=int,default=2);parser.add_argument('--state-only',action='store_true');args=parser.parse_args()
    args.output.mkdir(parents=True,exist_ok=True);before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(args.archive)as a:asset=a.read('Imagery/Magic/streamer.i3d')
    meshes=parse_asset(asset);port,compiled=build_port(args.output,meshes,args.ticks);fixture=StreamerFixture(args.executable,meshes)
    samples=[x for x in(0,1,2,12,24,48,72,99,100,101,103)if x<=args.ticks];cases=[];errors=[];times=[];first={}
    for repeat in range(args.repeat):
        fixture.reset()
        for tick in range(args.ticks+1):
            if tick:fixture.animate()
            original=fixture.state();preview=port[(0,tick)];runtime=port[(1,tick)];state_errors=[]
            if(original['frameon'],original['alive'])!=(runtime['frameon'],runtime['alive']):state_errors.append('clock/lifetime')
            for j,(a,b)in enumerate(zip(original['streams'],runtime['streams'])):
                for field,(x,y)in enumerate(zip(a,b)):
                    if f32(x)!=f32(y):state_errors.append(f'stream{j}.float{field}')
            if original['alive']:
                for p,q in zip(original['particles'],runtime['particles']):
                    if p['count']!=q['count']:state_errors.append(f'particle{p["stream"]}/{p["slot"]}.count')
                    if p['count']<=0:continue
                    for field,(x,y)in enumerate(zip(p['values'],q['values'])):
                        if f32(x)!=f32(y):state_errors.append(f'particle{p["stream"]}/{p["slot"]}.float{field}')
            same_producers={k:v for k,v in preview.items()if k!='kill_count'}=={k:v for k,v in runtime.items()if k!='kill_count'}
            if not same_producers:state_errors.append('preview/runtime producer mismatch')
            if runtime['kill_count']!=int(not runtime['alive']):state_errors.append('runtime kill request')
            pair=None;diff=None;position_error=0;draws=[]
            if tick in samples and not args.state_only:
                start=time.perf_counter();draws=fixture.original_draws();native,z=fixture.pixels(draws);modern,mz=fixture.pixels(runtime['draws']);times.append((time.perf_counter()-start)*1000)
                pair=(sha(native),sha(modern),sha(z),sha(mz))
                if repeat==0:first[tick]=pair
                elif first[tick]!=pair:raise AssertionError('Streamer warm pixel/depth replay changed')
                if len(draws)!=len(runtime['draws']):state_errors.append('triangle submission count')
                position_error=max([abs(a-b)for p,q in zip(draws,runtime['draws'])for x,y in zip(p['positions'],q['positions'])for a,b in zip(x,y)]or[0])
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(PIXELS)+'H',native),struct.unpack('<'+str(PIXELS)+'H',modern)))
                if not repeat:save_rgb565_png(args.output/f'retail-{tick:03d}.png',native,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',modern,WIDTH,HEIGHT)
            if not repeat:
                if state_errors or position_error>5e-5 or diff:errors.append(dict(tick=tick,state_errors=state_errors,max_position_error=position_error,differing_rgb565_pixels=diff))
                cases.append(dict(tick=tick,original_state=original,preview_state=preview,runtime_state=runtime,producer_state_equal=same_producers,state_errors=state_errors,image_hashes=pair,max_position_error=position_error,differing_rgb565_pixels=diff,original_draws=draws))
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Streamer source spans changed during comparison')
    report=dict(status='pass'if not errors else'fail',failures=errors,retail_sha256=RETAIL_SHA,asset_member='Imagery/Magic/streamer.i3d',asset_sha256=sha(asset),source_span_sha256=after,
        probe_sha256=sha(Path(__file__).read_bytes()),port_components=compiled,case_count=len(cases),sampled_pixel_ticks=[]if args.state_only else samples,replays_per_case=args.repeat,
        median_warm_pixel_pair_ms=statistics.median(times)if times else None,original_initialize='0x4efbd0',original_animate='0x4efd50',original_render='0x4efe50',original_parametric='0x4efa90',original_insertion='0x4efcc0',
        scope='Original four-stream one-shot state/trig/mesh/raster versus actual shared preview/map production methods in both ownership modes. Exact authored meshes/textures, face0/identityowner, no spell and explicitly unavailable optional sound. Current frontend culling formula supplied at mesh-submission boundary; actual authored-normal illumination/deviceculling/map/poison-cure/audio/backend remain separate.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if errors:raise SystemExit(1)


if __name__=='__main__':main()
