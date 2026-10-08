#!/usr/bin/env python3
"""Original standalone Sparks fallback versus the current production burst."""
import argparse
import json
import struct
import subprocess
import statistics
import time
import zipfile
from pathlib import Path

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP
from software_probe import SoftwareFixture, RETAIL_SHA, save_rgb565_png
from fountain_probe import FountainFixture
from fizzle_probe import span, sha, f32

ROOT = Path(__file__).resolve().parents[2]
ASSET_SHA='ea9240b037b80eb9b29b475e6fe4278f640124bd6128a4f4a20636bd9628daab'
WIDTH,HEIGHT=384,384


def parse_asset(asset):
    if sha(asset)!=ASSET_SHA:raise ValueError('Unexpected Sparks imagery')
    u=lambda o:struct.unpack_from('<I',asset,o)[0];r=lambda o:o+u(o)
    if(u(108),u(368),u(0x3fc),u(0x7c0))!=(16,8,1,4):raise ValueError('Sparks mesh counts changed')
    vertices=r(r(112));faces=r(372);texture=r(r(0x760))
    if struct.unpack_from('<8I',asset,0x448)!=(32,0x41,0,16,0xf00,0xf0,0xf,0xf000):raise ValueError('Sparks texture format changed')
    parts=[]
    for j in range(4):
        parts.append(dict(vertices=list(struct.iter_unpack('<8f',asset[vertices+j*128:vertices+(j+1)*128])),indices=list(struct.unpack_from('<6H',asset,faces+j*12))))
    return dict(parts=parts,texture=asset[texture:texture+8192],offsets=[vertices,faces,texture])


def production_spans():
    source = (ROOT/'src/effect.cpp').read_text(); header = (ROOT/'src/effect.h').read_text()
    return dict(types=span(header, 'inline constexpr int32_t kSparkMaxParticles', 'class TFlameEffect :'),
                params=span(header, 'struct SParticleParams\n', '_CLASSDEF(TParticle3DAnimator)'),
                clock=span(source, 'constexpr double kSparkSimTickMs', '// Retail spark params'),
                jitter=span(source, 'double SparkUnitJitter()' if 'double SparkUnitJitter()' in source else 'float SparkUnitJitter()', '}   // namespace'),
                initialize_advance=span(source, 'bool TSparkEffect::InitParticles(', 'void TSparkEffect::Submit('),
                submit=span(source,'void TSparkEffect::Submit(','// Map ownership adapter'),
                math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def decode_params(data):
    u = lambda offset: struct.unpack_from('<I',data,offset)[0]
    i = lambda offset: struct.unpack_from('<i',data,offset)[0]
    return dict(particles=i(0), pos=list(struct.unpack_from('<3f',data,4)), pspread=list(struct.unpack_from('<3f',data,16)),
                dir=list(struct.unpack_from('<3f',data,28)), spread=list(struct.unpack_from('<3f',data,40)),
                gravity=struct.unpack_from('<f',data,52)[0], trails=i(56), minstart=i(60), maxstart=i(64), minlife=i(68), maxlife=i(72),
                bounce=bool(u(76)), killobj=bool(u(80)), objflags=u(84), seektargets=bool(u(88)), seekz=bool(u(92)), numtargets=i(96))


def encode_params(params):
    """Explicit Win32 BOOL layout; the modern C++ bool layout is different."""
    data=bytearray(176)
    for key,offset in [('particles',0),('trails',56),('minstart',60),('maxstart',64),('minlife',68),('maxlife',72),('bounce',76),('killobj',80),('objflags',84),('seektargets',88),('seekz',92),('numtargets',96)]:
        struct.pack_into('<I',data,offset,int(params[key]))
    for key,offset in [('pos',4),('pspread',16),('dir',28),('spread',40)]:struct.pack_into('<3f',data,offset,*params[key])
    struct.pack_into('<f',data,52,params['gravity'])
    return bytes(data)


class SparksFixture:
    observe_random = FountainFixture.observe_random

    def __init__(self, executable, mesh=None, build=None):
        self.software = SoftwareFixture(executable,WIDTH,HEIGHT,build=build); self.vm = self.software.vm
        self.mesh=mesh;self.imagery=self.vm.allocate(0x200);self.objects=[self.vm.allocate(0x34c)for _ in range(4)]
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.packets=[]
        if mesh:self.software.set_texture(64,64,mesh['texture'],format='ARGB4444')
        self.animator = self.vm.allocate(0x400); self.owner = self.vm.allocate(0x400)
        self.vm.put_u32(self.animator,0x5a9ec0); self.vm.put_u32(self.animator+4,self.owner)
        self.vm.put_u32(self.animator+0x44,4)
        self.vm.put_u32(self.animator+8,self.imagery)
        self.vtable = self.vm.allocate(0x200); self.vm.put_u32(self.owner,self.vtable)
        self.vm.handlers['SparksFixtureCommandDone']=(1,self.command_done)
        self.vm.put_u32(self.vtable+0x158,self.vm.api_address('kernel32.dll','SparksFixtureCommandDone'))
        self.vm.handlers['SparksFixtureSetFlags']=(1,self.set_flags)
        self.vm.put_u32(self.vtable+0x40,self.vm.api_address('kernel32.dll','SparksFixtureSetFlags'))
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        for address in self.boundaries:
            self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in (0x483300,0x483312,0x48332c):
            self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.raw_random_draws=0
        self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_raw_random,begin=0x58c582,end=0x58c582)
        self.vm.uc.hook_add(UC_HOOK_CODE,self.initialized,begin=0x4e582c,end=0x4e582c)
        self.random=[]; self.random_pending=[]; self.done=False; self.initial_state=None; self.params=None
        self.software.clear(); self.software.checkpoint()

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x40eef0:
            index=self.vm.u32(sp+4)
            if index>=4:raise AssertionError('Sparks requested unknown photon')
            uc.reg_write(UC_X86_REG_EAX,self.objects[index])
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(2,1):raise AssertionError('Sparks original blend changed')
        elif address==0x40a8f0:
            obj=self.vm.u32(sp+4);index=self.objects.index(obj)
            if self.vm.u32(obj)!=4:raise AssertionError('Original Sparks must use POS1 only')
            self.packets.append(dict(index=index,position=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x10,12))),stored_scale=list(struct.unpack('<3f',self.vm.uc.mem_read(obj+0x40,12)))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def command_done(self,args):
        self.done=bool(args[0]); return 0

    def set_flags(self,args):
        self.vm.put_u32(self.owner+8,args[0]);return 0

    def observe_raw_random(self,uc,address,size,user):
        self.raw_random_draws+=1

    def initialized(self,uc,address,size,user):
        self.params=decode_params(bytes(self.vm.uc.mem_read(self.animator+0xfc,176)))
        self.initial_state=self.state()

    def reset(self):
        self.software.restore(); self.random=[]; self.random_pending=[]; self.done=False; self.initial_state=None; self.params=None;self.raw_random_draws=0
        self.vm.call(self.vm.u32(0x5a9ec0+24),this=self.animator)

    def initialize(self,params):
        if params['seektargets']or params['numtargets']:raise ValueError('Sparks seeking context is not mapped')
        address=self.vm.allocate(176);self.vm.write(address,encode_params(params));self.vm.call(self.vm.u32(0x5a9ec0+96),(address,),this=self.animator)

    def advance(self):
        self.vm.call(self.vm.u32(0x5a9ec0+44),this=self.animator)

    def state(self):
        count=self.vm.u32(self.animator+0xfc); particles=[]
        for index in range(count):
            position=list(struct.unpack('<3f',self.vm.uc.mem_read(self.vm.u32(self.animator+0x1b0)+index*12,12)))
            velocity=list(struct.unpack('<3f',self.vm.uc.mem_read(self.vm.u32(self.animator+0x1ac)+index*12,12)))
            particles.append(dict(index=index,values=position+velocity,
                life=self.vm.u32(self.vm.u32(self.animator+0x1b4)+index*4),
                start=self.vm.u32(self.vm.u32(self.animator+0x1b8)+index*4),
                variant=self.vm.u32(self.vm.u32(self.animator+0x1bc)+index*4)))
        return dict(particles=particles,done=self.done,random_count=len(self.random))

    def original_draws(self):
        self.packets=[];self.vm.call(self.vm.u32(0x5a9ec0+52),this=self.animator);draws=[]
        for packet in self.packets:
            obj=self.objects[packet['index']];self.vm.write(obj+0x10,struct.pack('<3f',*packet['position']))
            self.vm.call(0x40a420,(obj,0,0,0,0),this=self.imagery)
            matrix=list(struct.unpack('<16f',self.vm.uc.mem_read(obj+0x58,64)));positions=[];part=self.mesh['parts'][packet['index']]
            for vertex in part['vertices']:
                self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(obj+0x58,self.point,self.result))
                positions.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            draws.append(dict(positions=positions,uvs=[list(v[6:])for v in part['vertices']],indices=part['indices'],matrix=matrix,**packet))
        return draws

    def pixels(self,draws):
        self.software.clear()
        for draw in draws:
            projected=self.software.project(draw['positions'],camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Sparks viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*uv)for p,uv in zip(projected,draw['uvs'])],draw['indices'],z_enabled=True,z_write=False)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def build_port(output, params, rng_path, ticks, mesh=None):
    pieces=production_spans()
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxDebugMode:uint8_t{Normal,FullTexture};enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxLightMode:uint8_t{Unlit};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&i){draws.push_back(i);}};
struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TObjectInstance{World world;const World&Transform()const{return world;}};
struct TObjectImagery{};struct SObjectDef{};
struct TEffect{TEffect(TObjectImagery*){};TEffect(SObjectDef*,TObjectImagery*){};virtual~TEffect()=default;};
void log_error(const char*,...){}
std::ifstream rng;int random_count=0;
int random(int lo,int hi){int a,b,v;if(!(rng>>a>>b>>v)||a!=lo||b!=hi)throw std::runtime_error("Native RNG call order mismatch");++random_count;return v;}
'''
    def lit(v):
        text=format(v,'.9g'); return text+('f' if '.' in text or 'e' in text else '.0f')
    assignments=[]
    for key,value in params.items():
        if isinstance(value,list): literal='{'+','.join(lit(x)for x in value)+'}'
        elif isinstance(value,bool): literal='true'if value else'false'
        elif isinstance(value,float): literal=lit(value)
        else: literal=str(value)
        assignments.append('params.'+key+'='+literal+';')
    shape_init=''
    if mesh:
        shapes=[]
        for part in mesh['parts']:
            shapes.append('{'+','.join('{{'+','.join(lit(x)for x in v[:3])+'},{'+','.join(lit(x)for x in v[3:6])+'},'+lit(v[6])+','+lit(v[7])+'}'for v in part['vertices'])+'}')
        shape_init='S3DVertex vertices[4][4]={'+','.join(shapes)+'};for(int j=0;j<4;++j)std::memcpy(effect.variants_[j].vertices,vertices[j],sizeof(vertices[j]));effect.texture_=1;'
    trailer=r'''
int main(int argc,char**argv){rng.open(argv[1]);SParticleParams params{};PARAMETERS
 TSparkEffect effect(nullptr);SHAPES if(!effect.InitParticles(params))return2;TRenderer renderer;TObjectInstance owner;
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);
 printf("S %d %d %d\n",tick,!effect.alive_,random_count);
 for(int i=0;i<effect.num_particles_;++i){const auto&p=effect.particles_[i];
 printf("P %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %d\n",tick,i,p.pos.X,p.pos.Y,p.pos.Z,p.vel.X,p.vel.Y,p.vel.Z,p.life,p.start,p.variant);}
 renderer.draws.clear();effect.Submit(renderer,owner,EFxDebugMode::Normal);
 for(const auto&d:renderer.draws){printf("D %d",tick);for(int i=0;i<4;++i)printf(" %.9g %.9g %.9g %.9g %.9g",d.world_pos[i][0],d.world_pos[i][1],d.world_pos[i][2],d.uv[i][0],d.uv[i][1]);puts("");}
 }int a,b,c;if(rng>>a>>b>>c)throw std::runtime_error("Unconsumed RNG calls");
}
'''.replace('PARAMETERS',''.join(assignments)).replace('SHAPES',shape_init).replace('TICKS',str(ticks)).replace('return2','return 2')
    source=output/'sparks-port-state.cpp'
    source.write_text(prelude+pieces['types'].replace('  private:','  public:')+pieces['params']+pieces['clock']+pieces['jitter']+pieces['initialize_advance']+pieces['submit']+trailer)
    binary=output/'sparks-port-state'; command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError('Production Sparks compile failed; see port-compile.log')
    result=subprocess.run([str(binary),str(rng_path)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout)
    if result.returncode: raise RuntimeError(result.stderr)
    states={}
    for line in result.stdout.splitlines():
        parts=line.split();tick=int(parts[1])
        if parts[0]=='S':states[tick]=dict(done=bool(int(parts[2])),random_count=int(parts[3]),particles=[],draws=[])
        elif parts[0]=='P':states[tick]['particles'].append(dict(index=int(parts[2]),values=list(map(float,parts[3:9])),life=int(float(parts[9])),start=int(float(parts[10])),variant=int(parts[11])))
        else:
            values=list(map(float,parts[2:]));uvs=[values[i*5+3:i*5+5]for i in range(4)]
            shape=next(p for p in mesh['parts']if all(f32(a)==f32(b)for p,q in zip(uvs,[v[6:]for v in p['vertices']])for a,b in zip(p,q)))
            states[tick]['draws'].append(dict(positions=[values[i*5:i*5+3]for i in range(4)],uvs=uvs,indices=shape['indices']))
    return states,dict(command=command,source_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()))


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--profile',choices=('editor','controlled_bounce_trail'),default='editor')
    parser.add_argument('--ticks',type=int,default=52);args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    probe_sha=sha(Path(__file__).read_bytes())
    before={k:sha(v.encode())for k,v in production_spans().items()}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:asset=archive.read('Imagery/Misc/sparks.i3d')
    mesh=parse_asset(asset);fixture=SparksFixture(args.executable,mesh);fixture.reset()
    selected_params=None
    if args.profile=='controlled_bounce_trail':
        selected_params=dict(particles=20,pos=[0,0,45],pspread=[3,3,3],dir=[1,0,0],spread=[.5,.5,.5],gravity=.25,trails=2,minstart=0,maxstart=8,minlife=20,maxlife=40,bounce=True,killobj=True,objflags=2,seektargets=False,seekz=False,numtargets=0)
        fixture.initialize(selected_params)
    fixture.advance();count=10 if selected_params is None else 20
    if fixture.initial_state is None or fixture.params['particles']!=count: raise AssertionError('Original Sparks pool count did not match the selected profile')
    samples=[n for n in (1,2,4,8,12,18,24,30,38,40,52)if n<=args.ticks]
    states=[fixture.initial_state,fixture.state()];native_draws={1:fixture.original_draws()}
    for tick in range(2,args.ticks+1):
        fixture.advance();states.append(fixture.state())
        if tick in samples:native_draws[tick]=fixture.original_draws()
    rng=args.output/'native-rng.txt';rng.write_text(''.join('%d %d %d\n'%row for row in fixture.random))
    port,compiled=build_port(args.output,fixture.params,rng,args.ticks,mesh);cases=[];failures=[];checks=0;times=[];hashes={}
    for tick,native in enumerate(states):
        modern=port[tick];errors=[]
        if len(native['particles'])!=count or len(modern['particles'])!=count:raise AssertionError('Full Sparks pool must be compared')
        for field in ('done','random_count'):
            checks+=1
            if native[field]!=modern[field]:errors.append(field)
        for p,q in zip(native['particles'],modern['particles']):
            for field in ('life','start','variant'):
                checks+=1
                if p[field]!=q[field]:errors.append('particle%d.%s'%(p['index'],field))
            for index,(a,b)in enumerate(zip(p['values'],q['values'])):
                checks+=1
                if f32(a)!=f32(b):errors.append('particle%d.float%d'%(p['index'],index))
        diff=None;max_position=0;pair=None
        if tick in samples:
            start=time.perf_counter();draws=native_draws[tick]
            if len(draws)!=len(modern['draws']):errors.append('draw_count')
            max_position=max([abs(a-b)for p,q in zip(draws,modern['draws'])for x,y in zip(p['positions'],q['positions'])for a,b in zip(x,y)]or[0])
            if max_position>5e-5:errors.append('geometry')
            original,z=fixture.pixels(draws);revised,rz=fixture.pixels(modern['draws']);pair=[sha(original),sha(revised),sha(z),sha(rz)];hashes[tick]=pair
            diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',original),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',revised)))
            if diff or z!=rz:errors.append('pixels_or_depth')
            times.append((time.perf_counter()-start)*1000)
            save_rgb565_png(args.output/f'retail-{tick:03d}.png',original,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',revised,WIDTH,HEIGHT)
        if errors:failures.append(dict(tick=tick,fields=errors))
        cases.append(dict(tick=tick,original_state=native,port_state=modern,state_errors=errors,max_position_error=max_position,differing_rgb565_pixels=diff,image_hashes=pair))
    first_raw_draws=fixture.raw_random_draws;fixture.reset()
    if selected_params:fixture.initialize(selected_params)
    fixture.advance()
    if fixture.initial_state!=states[0]or fixture.state()!=states[1]:raise AssertionError('Sparks warm initialization changed')
    for tick in range(1,args.ticks+1):
        if tick>1:fixture.advance()
        if fixture.state()!=states[tick]:raise AssertionError('Sparks warm state changed')
        if tick in samples:
            original,z=fixture.pixels(fixture.original_draws());revised,rz=fixture.pixels(port[tick]['draws'])
            if[sha(original),sha(revised),sha(z),sha(rz)]!=hashes[tick]:raise AssertionError('Sparks warm pixels/depth changed')
    if fixture.raw_random_draws!=first_raw_draws or fixture.random_pending:raise AssertionError('Sparks raw RNG draws or range-call return observations changed')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production Sparks spans changed during comparison')
    if probe_sha!=sha(Path(__file__).read_bytes()):raise AssertionError('Sparks probe changed during comparison')
    report=dict(status='fail'if failures else'pass',failures=failures,case_count=len(cases),field_checks=checks,
        native_rng_calls=len(fixture.random),native_raw_crt_draws=first_raw_draws,native_params=fixture.params,profile=args.profile,retail_sha256=RETAIL_SHA,source_span_sha256=after,probe_sha256=probe_sha,
        port_component=compiled,original_initialize='0x4e53c0',original_init_particles='0x4e55b0',original_animate='0x4e5830',
        original_render='0x4e5d30',asset_sha256=sha(asset),asset_member='Imagery/Misc/sparks.i3d',sampled_pixel_ticks=samples,replays_per_case=2,median_warm_pixel_pair_ms=statistics.median(times),
        scope='Actual native ten-particle editor fallback allocator/InitParticles/ballistics/done/Render vscompiled production InitParticles/Advance/Submit and strict native RNG. Exact4photon authoredquads/ARGB4444atlas, nativePOS1matrix and originalprojection/raster. Defaultobjectflag0 supplied from independentlyexecuted NewObjectcontract; hierarchy/ownercommand/whitevertexlighting/imagerysubmission adapted. Commonraster uses originalindices; moderntriangulation/combat/map/backend separate.',dosbox_used=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in('cases','failures','port_component','source_span_sha256')},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
