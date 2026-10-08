#!/usr/bin/env python3
"""Original null-spell Cure state versus the production cure_retail module."""
import argparse
import json
import struct
import subprocess
import statistics
import time
import zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,RETAIL_SHA,save_rgb565_png
from fountain_probe import FountainFixture
from fizzle_probe import sha,span

ROOT=Path(__file__).resolve().parents[2]
PARTICLE=struct.Struct('<12fi4f4i')
STATE_BYTES=24+85*84
WIDTH,HEIGHT=512,512
ASSET_SHA='e9f42e51042c832dcf2acc5964017663d4089bec109666c9ebd8711cf9560216'


def parse_asset(asset):
    if sha(asset)!=ASSET_SHA:raise ValueError('Unexpected Cure imagery')
    u=lambda o:struct.unpack_from('<I',asset,o)[0];r=lambda o:o+u(o);b=20+u(16)
    if(u(b+4),u(b+12),u(b+20),u(b+36),u(b+44))!=(1,12,6,3,3):raise ValueError('Cure geometry identity changed')
    verts=r(r(r(b+16)));faces=r(b+24);texture=r(b+40);bits=r(r(texture+108))
    if struct.unpack_from('<8I',asset,texture+72)!=(32,64,0,16,0xf800,0x7e0,0x1f,0):raise ValueError('Cure object0 is not RGB565')
    return dict(vertices=list(struct.iter_unpack('<8f',asset[verts:verts+128])),indices=list(struct.unpack_from('<6H',asset,faces)),texture=asset[bits:bits+8192],offsets=[verts,faces,bits])


class CureFixture:
    observe_random=FountainFixture.observe_random

    def __init__(self,executable,mesh=None,build=None):
        self.software=SoftwareFixture(executable,WIDTH,HEIGHT,build=build);self.vm=self.software.vm;self.mesh=mesh
        self.obj=self.vm.allocate(0x200);self.imagery=self.vm.allocate(0x200);self.point=self.vm.allocate(12);self.result=self.vm.allocate(12);self.matrix=self.vm.allocate(64);self.packets=[]
        if mesh:self.software.set_texture(64,64,mesh['texture'],format='RGB565')
        self.animator=self.vm.allocate(0x2000);self.owner=self.vm.allocate(0x400);self.vtable=self.vm.allocate(0x200)
        self.vm.put_u32(self.animator,0x5a8f20);self.vm.put_u32(self.animator+4,self.owner);self.vm.put_u32(self.owner,self.vtable)
        self.vm.put_u32(self.animator+8,self.imagery)
        self.vm.handlers['CureFixtureSetFlags']=(1,self.set_flags)
        self.vm.put_u32(self.vtable+0x40,self.vm.api_address('kernel32.dll','CureFixtureSetFlags'))
        self.vm.handlers['CureFixtureCommandDone']=(1,lambda args:0)
        self.vm.put_u32(self.vtable+0x158,self.vm.api_address('kernel32.dll','CureFixtureCommandDone'))
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x49c430:4,0x40eef0:4,0x4178e0:0,0x417d60:8,0x417b00:0,0x40c960:0,0x40a8f0:24,0x40c9c0:4}
        for address in self.boundaries:self.vm.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in(0x483300,0x483312,0x48332c):self.vm.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.random_pending=[];self.software.clear();self.software.checkpoint()

    def set_flags(self,args):
        self.vm.put_u32(self.owner+8,args[0]);return 0

    def external(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff)
        elif address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Cure requested visible guide ball/color object')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        elif address==0x417d60:
            if(self.vm.u32(sp+4),self.vm.u32(sp+8))!=(8,1):raise AssertionError('Cure additive blend request changed')
        elif address==0x40a8f0:
            if self.vm.u32(sp+4)!=self.obj or self.vm.u32(self.obj)!=0x100:raise AssertionError('Cure matrix payload flags changed')
            self.packets.append(list(struct.unpack('<16f',self.vm.uc.mem_read(self.obj+0x58,64))))
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])

    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[]
        self.vm.call(self.vm.u32(0x5a8f20+24),this=self.animator)

    def advance(self):
        # Normal owner reaping ends updates. Preserve the final record for
        # explicit post-death observations rather than updating a killed owner.
        if not self.vm.u32(self.owner+8)&0x1000:self.vm.call(self.vm.u32(0x5a8f20+44),this=self.animator,instruction_limit=10000000)

    def raw_state(self):
        return struct.pack('<4IfI',self.vm.u32(self.animator+0xfc),self.vm.u32(self.animator+0x1ce4),self.vm.u32(self.animator+0x1ce8),self.vm.u32(self.animator+0x1cf0),
            struct.unpack('<f',self.vm.uc.mem_read(self.animator+0x1cec,4))[0],int(not self.vm.u32(self.owner+8)&0x1000))+bytes(self.vm.uc.mem_read(self.animator+0x100,85*84))

    def state(self):
        raw=self.raw_state();header=struct.unpack('<4IfI',raw[:24]);records=[]
        for index in range(85):
            values=PARTICLE.unpack_from(raw,24+index*84)
            records.append(dict(index=index,values=list(values[:12])+list(values[13:17]),state=values[12],starttime=values[17],whichball=values[18],color=values[19],linedup=values[20]))
        return dict(frame=header[0],numactiveballs=header[1],ballslinedup=header[2],killing=header[3],facing=header[4],alive=bool(header[5]),swirls=records[:80],balls=records[80:],rng_calls=len(self.random),rng_nonconstant_calls=sum(a!=b for a,b,_ in self.random))

    def original_matrices(self):
        self.packets=[];self.vm.call(self.vm.u32(0x5a8f20+52),this=self.animator);return list(self.packets)

    def pixels(self,matrices):
        self.software.clear();points=[]
        for matrix in matrices:
            self.vm.write(self.matrix,struct.pack('<16f',*matrix));positions=[]
            for vertex in self.mesh['vertices']:
                self.vm.write(self.point,struct.pack('<3f',*vertex[:3]));self.vm.call(0x43ad80,(self.matrix,self.point,self.result))
                positions.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
            projected=self.software.project(positions,camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Cure viewport clips geometry')
            self.software.draw([(*p,31,31,31,31,*v[6:])for p,v in zip(projected,self.mesh['vertices'])],self.mesh['indices'],z_enabled=True,z_write=False)
            points.append(positions)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth'),points


def build_submission(output,port_states,ticks):
    body=span((ROOT/'src/effects/cure.cpp').read_text(),'void TCureEffect_Bespoke::Submit(','void TCureEffect_Bespoke::TickAndSubmitForTest_BESPOKE(')
    prelude=r'''
#define CURE_STATE_ONLY
#include "effects/cure.h"
#include "math3d.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <vector>
enum class EFxDebugMode{Normal};
struct SHelperMeshSubmit{unsigned mesh=1;bool additive_blend=false,shadow_plane=false;int retail_lighting=0;float world[16]{},diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0,sort_depth=0;};
struct TRenderer{std::vector<SHelperMeshSubmit>draws;void SubmitHelperMesh(const SHelperMeshSubmit&v){draws.push_back(v);}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct World{hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4&Matrix()const{return m;}};
struct TCureEffect_Bespoke{struct Material{float diffuse[4]{1,1,1,1},ambient[4]{1,1,1,1},specular[4]{},emissive[4]{1,1,1,1},power=0;};
 struct Impl{cure_retail::State state;unsigned mesh=1;struct{Material matdesc;}material;bool BindMesh(){return true;}};
 Impl data;Impl*impl_=&data;World world;bool IsAlive()const{return data.state.alive;}const World&Transform()const{return world;}void Submit(EFxDebugMode)const;};
'''
    trailer=r'''
int main(int argc,char**argv){std::ifstream input(argv[1],std::ios::binary);TCureEffect_Bespoke effect;
 for(int tick=0;tick<=TICKS;++tick){int header[4],alive;input.read((char*)header,16);input.read((char*)&effect.data.state.facing,4);input.read((char*)&alive,4);
 effect.data.state.alive=alive;input.read((char*)effect.data.state.sm.data(),80*84);input.read((char*)effect.data.state.balls.data(),5*84);
 renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
 printf("S %d %zu\n",tick,renderer.draws.size());
 for(const auto&d:renderer.draws){printf("M %d",tick);for(int row=0;row<4;++row)for(int col=0;col<4;++col)printf(" %.9g",d.world[col*4+row]);puts("");}
 }}
'''.replace('TICKS',str(ticks))
    source=output/'cure-submit-driver.cpp';source.write_text(prelude+body+trailer);binary=output/'cure-submit-driver'
    command=['clang++','-std=c++17','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'submit-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Actual Cure Submit compile failed')
    trace=subprocess.check_output([str(binary),str(port_states)],text=True);(output/'submit-trace.txt').write_text(trace);matrices={}
    for line in trace.splitlines():
        fields=line.split();tick=int(fields[1])
        if fields[0]=='S':matrices[tick]=[]
        else:matrices[tick].append(list(map(float,fields[2:])))
    return matrices,dict(method_sha256=sha(body.encode()),driver_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),command=command)


def build_port(output,rng_path,ticks):
    source=output/'cure-state-driver.cpp'
    source.write_text(r'''
#include "effects/cure.h"
#include <fstream>
#include <stdexcept>
int main(int argc,char**argv){std::ifstream inputs(argv[1]);std::ofstream out(argv[2],std::ios::binary);
 cure_retail::State state;auto random=[&](int lo,int hi){int a,b,v;if(!(inputs>>a>>b>>v)||a!=lo||b!=hi)throw std::runtime_error("Original Cure RNG order mismatch");return v;};
 cure_retail::Initialize(state,random);
 for(int tick=0;tick<=TICKS;++tick){if(tick)cure_retail::AdvanceOneTick(state,random);
  int header[4]={state.framenum,state.numactiveballs,state.ballslinedup,state.killing};int alive=state.alive;
  out.write((char*)header,sizeof(header));out.write((char*)&state.facing,4);out.write((char*)&alive,4);
  out.write((char*)state.sm.data(),80*84);out.write((char*)state.balls.data(),5*84);
 }int a,b,v;if(inputs>>a>>b>>v)throw std::runtime_error("Unconsumed Cure RNG");
}
'''.replace('TICKS',str(ticks)))
    binary=output/'cure-state-driver';command=['clang++','-std=c++17','-DCURE_STATE_ONLY','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/effects/cure.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Cure production compile failed')
    result=subprocess.run([str(binary),str(rng_path),str(output/'port-states.bin')],capture_output=True,text=True);(output/'port-run.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Cure production RNG replay failed; see port-run.log')
    data=(output/'port-states.bin').read_bytes()
    if len(data)!=(ticks+1)*STATE_BYTES:raise AssertionError('Cure state record cardinality changed')
    return[data[i*STATE_BYTES:(i+1)*STATE_BYTES]for i in range(ticks+1)],dict(command=command,driver_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()))


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--ticks',type=int,default=150);args=p.parse_args()
    args.output.mkdir(parents=True,exist_ok=True);paths=[ROOT/'src/effects/cure.cpp',ROOT/'src/effects/cure.h'];before={str(p.relative_to(ROOT)):sha(p.read_bytes())for p in paths}
    with zipfile.ZipFile(ROOT/'data/imagery.rvi')as archive:asset=archive.read('Imagery/Magic/cure.i3d')
    mesh=parse_asset(asset);fixture=CureFixture(args.executable,mesh);fixture.reset();states=[fixture.raw_state()];samples=[t for t in(0,1,8,16,32,48,52,64,80,104,120,131,132,150)if t<=args.ticks];native_matrices={0:fixture.original_matrices()}
    for tick in range(1,args.ticks+1):
        fixture.advance();states.append(fixture.raw_state())
        if tick in samples:native_matrices[tick]=fixture.original_matrices()
    (args.output/'original-states.bin').write_bytes(b''.join(states));calls=[r for r in fixture.random if r[0]!=r[1]]
    rng_path=args.output/'native-rng.txt';rng_path.write_text(''.join('%d %d %d\n'%r for r in calls));modern,compiled=build_port(args.output,rng_path,args.ticks)
    port_matrices,submit_compiled=build_submission(args.output,args.output/'port-states.bin',args.ticks)
    cases=[];failures=[];fields=0;max_float=0;times=[];hashes={}
    for tick,(original,port)in enumerate(zip(states,modern)):
        errors=[];mismatches=0
        if original[:24]!=port[:24]:errors.append('frame/counters/facing/alive')
        for index in range(85):
            a=PARTICLE.unpack_from(original,24+index*84);b=PARTICLE.unpack_from(port,24+index*84)
            for field,(x,y)in enumerate(zip(a,b)):
                fields+=1
                if original[24+index*84+field*4:28+index*84+field*4]!=port[24+index*84+field*4:28+index*84+field*4]:
                    mismatches+=1
                    if field in tuple(range(12))+(13,14,15,16):max_float=max(max_float,abs(x-y))
        if mismatches:errors.append('particle_fields')
        diff=None;max_corner=0;pair=None
        if tick in samples:
            start=time.perf_counter();matrices=native_matrices[tick]
            if len(matrices)!=len(port_matrices[tick]):errors.append('submission_count')
            native,z,npoints=fixture.pixels(matrices);revised,rz,ppoints=fixture.pixels(port_matrices[tick]);pair=[sha(native),sha(revised),sha(z),sha(rz)];hashes[tick]=pair
            max_corner=max([abs(a-b)for p,q in zip(npoints,ppoints)for x,y in zip(p,q)for a,b in zip(x,y)]or[0])
            if max_corner>5e-5:errors.append('geometry')
            diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',native),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',revised)))
            if diff or z!=rz:errors.append('pixels_or_depth')
            times.append((time.perf_counter()-start)*1000);save_rgb565_png(args.output/f'retail-{tick:03d}.png',native,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',revised,WIDTH,HEIGHT)
        if errors:failures.append(dict(tick=tick,errors=errors,differing_particle_fields=mismatches))
        cases.append(dict(tick=tick,original_header=list(struct.unpack('<4IfI',original[:24])),port_header=list(struct.unpack('<4IfI',port[:24])),differing_particle_fields=mismatches,original_sha256=sha(original),port_sha256=sha(port),differing_rgb565_pixels=diff,max_corner_error=max_corner,image_hashes=pair))
    fixture.reset()
    for tick in range(args.ticks+1):
        if tick:fixture.advance()
        if fixture.raw_state()!=states[tick]:raise AssertionError('Cure native warm state replay changed')
        if tick in samples:
            native,z,_=fixture.pixels(fixture.original_matrices());revised,rz,_=fixture.pixels(port_matrices[tick])
            if[sha(native),sha(revised),sha(z),sha(rz)]!=hashes[tick]:raise AssertionError('Cure native warm pixels/depth changed')
    after={str(p.relative_to(ROOT)):sha(p.read_bytes())for p in paths}
    if before!=after:raise AssertionError('Cure production source changed')
    report=dict(status='fail'if failures else'pass',failures=failures,case_count=len(cases),particle_records=85,field_checks=fields,max_float_difference=max_float,
        native_range_invocations=len(fixture.random),nonconstant_native_rng_calls=len(calls),retail_sha256=RETAIL_SHA,source_sha256=after,port_component=compiled,
        original_initialize='0x4e0c50',original_animate='0x4e0d90',scope='Original null-spell 80swirl/5ball Init/Animate and observed ownerkill vs actual cure_retail pureproductionstate. Basehierarchy/audio lookup/ownerflagendpoint adapted; rendering/map/spell/backend separate.',dosbox_used=False,cases=cases)
    report.update(original_render='0x4e1580',submit_component=submit_compiled,sampled_pixel_ticks=samples,replays_per_case=2,median_warm_pixel_pair_ms=statistics.median(times),asset_sha256=sha(asset),asset_member='Imagery/Magic/cure.i3d',viewport=[WIDTH,HEIGHT],render_scope='Exactobject0/quad/RGB565atlas and actual compiled Submit matrix/mesh payload through originalprojection/raster. Mesh/materialprovider andidentityowner adapted; whitevertexlight/cullNONE/commonraster overwrite policy explicit, actualadditive/materiallighting/sort/device/Metal separate.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in('cases','failures','source_sha256','port_component')},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
