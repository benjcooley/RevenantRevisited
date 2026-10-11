#!/usr/bin/env python3
"""FireFlash null-spell moving pool and native packets versus compiled production."""
import argparse,json,struct,subprocess,time,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fountain_probe import FountainFixture
from fizzle_probe import span,sha,f32
ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='7c7471fced7e8175a56327ad2bfa370579d2cc08400ce735e4fe63104244d526'
WIDTH=HEIGHT=1024


def native_mode_mapping(vm):
    """Observe original Scene mode code, not numeric-name assumptions."""
    calls=[]
    vm.put_u32(0x5d7a28,1);vm.put_u32(0x5e8790,0);vm.put_u32(0x66818c,0)
    def setter(uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);calls.append([vm.u32(sp+4),vm.u32(sp+8)])
        uc.reg_write(UC_X86_REG_EIP,vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+12);uc.reg_write(UC_X86_REG_EAX,1)
    hook=vm.uc.hook_add(UC_HOOK_CODE,setter,begin=0x417060,end=0x417060)
    try:vm.call(0x417d60,(8,1),this=0x65a57c)
    finally:vm.uc.hook_del(hook)
    states=dict(calls)
    if states.get(19)!=2 or states.get(20)!=2:raise AssertionError('Mode8 no longer maps to ONE/ONE')
    return dict(original_function='0x417d60',observed_setter='0x417060',mode=8,states=calls,
        source_blend='ONE',destination_blend='ONE',source_additive_metadata_correct=True)


def parse_asset(data):
    if sha(data)!=ASSET_SHA:raise ValueError('Wrong literal FireFlash asset')
    u=lambda a:struct.unpack_from('<I',data,a)[0];r=lambda a:a+u(a)
    if(u(108),u(368),u(376),u(1020),u(1984))!=(8,4,2,2,2):raise ValueError('Full old-layout topology changed')
    vertices=r(r(112));faces=r(372);parts=[]
    for obj in range(2):
        desc=1024+obj*108;bits=r(r(1888+obj*4));records=list(struct.iter_unpack('<8f',data[vertices+obj*128:vertices+(obj+1)*128]))
        indices=list(struct.unpack_from('<6H',data,faces+obj*12))
        if struct.unpack_from('<2I',data,desc+8)!=(64,64)or struct.unpack_from('<4I',data,desc+88)!=(0xf800,0x7e0,0x1f,0):raise ValueError('RGB565 texture changed')
        if max(indices)>=4:raise ValueError('Local indices changed')
        material=list(struct.unpack_from('<17f',data,380+obj*80+4))
        parts.append(dict(vertices=[list(v)for v in records],indices=indices,texture=data[bits:bits+8192],material=material,
            vertex_offset=vertices+obj*128,face_offset=faces+obj*12,texture_offset=bits))
    return parts


class FireFlashFixture:
    observe_random=FountainFixture.observe_random
    def __init__(self,exe,asset):
        self.parts=parse_asset(asset);self.software=SoftwareFixture(exe,WIDTH,HEIGHT);self.vm=self.software.vm;v=self.vm
        self.blend_mapping=native_mode_mapping(v)
        v.call(0x41e2de,stop_address=0x41e535,instruction_limit=20000000)
        self.lookup=bytes(v.uc.mem_read(0x634d44,1024))
        self.animator=v.allocate(0x3500);self.owner=v.allocate(0x400);self.imagery=v.allocate(0x200);vt=v.allocate(0x220)
        self.objects=[v.allocate(0x34c)for _ in range(2)];self.matrix=v.allocate(64);self.point=v.allocate(12);self.result=v.allocate(12)
        v.put_u32(self.animator,0x5a9194);v.put_u32(self.animator+4,self.owner);v.put_u32(self.animator+8,self.imagery);v.put_u32(self.owner,vt)
        v.put_u32(vt+0x1fc,0x4e1770) # Actual null-spell effect Initialize, including its guard.
        done=v.allocate(16);v.write_code(done,b'\x31\xc0\xc2\x04\x00');v.put_u32(vt+0x158,done)
        self.boundaries={0x40dd60:0,0x40e2e0:0,0x49c430:4,0x40eef0:4,0x417d60:8,0x40c960:0,0x40a8f0:24,0x40c9c0:4,0x417b00:0}
        for address in self.boundaries:v.uc.hook_add(UC_HOOK_CODE,self.external,begin=address,end=address)
        for address in(0x483300,0x483312,0x48332c):v.uc.hook_add(UC_HOOK_CODE,self.observe_random,begin=address,end=address)
        self.random=[];self.random_pending=[];self.packets=[];self.mode=[];self.software.checkpoint()
    def external(self,uc,address,size,user):
        v=self.vm;sp=uc.reg_read(UC_X86_REG_ESP)
        if address==0x49c430:uc.reg_write(UC_X86_REG_EAX,0xffffffff) # Declared silent audio boundary.
        elif address==0x40eef0:
            index=v.u32(sp+4)
            if index not in(0,1):raise AssertionError('Wrong authored object')
            uc.reg_write(UC_X86_REG_EAX,self.objects[index])
        elif address==0x417d60:self.mode.append([v.u32(sp+4),v.u32(sp+8)])
        elif address==0x40a8f0:
            obj=v.u32(sp+4)
            if v.u32(obj)!=0x100:raise AssertionError('Null-target native MATRIX flag changed')
            self.packets.append(dict(object=self.objects.index(obj),matrix=list(struct.unpack('<16f',v.uc.mem_read(obj+0x58,64)))))
        uc.reg_write(UC_X86_REG_EIP,v.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+self.boundaries[address])
    def reset(self):
        self.software.restore();self.random=[];self.random_pending=[];self.packets=[];self.mode=[];self.vm.call(0x4e18c0,this=self.animator)
    def step(self):self.vm.call(0x4e1ad0,this=self.animator)
    def render(self):
        self.packets=[];self.mode=[];self.vm.call(0x4e1fa0,this=self.animator)
        if self.mode!=[[8,1]]:raise AssertionError('Native FireFlash render mode changed')
        return self.packets.copy()
    def state(self):
        v=self.vm;a=self.animator;particles=[]
        for index in range(150):
            row=struct.unpack('<15fi2f4i',v.uc.mem_read(a+0x12c+index*88,88))
            particles.append(dict(index=index,values=list(row[:15])+list(row[16:18]),integers=[row[15],*row[18:]]))
        return dict(frame=v.u32(a+0x118),random_count=len(self.random),values=list(struct.unpack('<3f',v.uc.mem_read(a+0x10c,12)))+
            list(struct.unpack('<3f',v.uc.mem_read(a+0x11c,12))),particles=particles)
    def points(self,draw):
        v=self.vm;v.write(self.matrix,struct.pack('<16f',*draw['matrix']));points=[]
        for vertex in self.parts[draw['object']]['vertices']:
            v.write(self.point,struct.pack('<3f',*vertex[:3]));v.call(0x43ad80,(self.matrix,self.point,self.result))
            points.append(list(struct.unpack('<3f',v.uc.mem_read(self.result,12))))
        return points
    def pixels(self,draws,common=False):
        self.software.clear()
        for draw in draws:
            part=self.parts[draw['object']];self.software.set_texture(64,64,part['texture'],format='RGB565')
            points=self.points(draw)
            # Explicit inverse of the source REV_FIX_Z common-world bridge;
            # not a fitted size/camera adjustment. Residuals must remain failures.
            if common:points=[[x,y,z/1.46]for x,y,z in points]
            projected=self.software.project(points,camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<WIDTH and 0<=p[1]<HEIGHT)for p in projected):raise AssertionError('Fixed viewport clips authored geometry')
            self.software.draw([(*p,31,31,31,31,*v[6:])for p,v in zip(projected,part['vertices'])],part['indices'],z_enabled=True,z_write=False,instruction_limit=5000000)
        return self.vm.surface_bytes('screen'),self.vm.surface_bytes('depth')


def function(source,signature):
    start=source.index(signature);opening=source.index('{',start);depth=1;end=opening+1
    while depth:
        if source[end]=='{':depth+=1
        elif source[end]=='}':depth-=1
        end+=1
    return source[start:end]


def build_port(output,parts,rng,lookup,ticks):
    source=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text();obj=(ROOT/'src/object.cpp').read_text()
    methods={name:function(source,'void TFireFlashEffect_Bespoke::'+name+'(')for name in('SimulateTick','Advance','Submit')}
    methods['BindMeshes']=function(source,'bool TFireFlashEffect_Bespoke::BindMeshes(')
    initialize=span(source,'    // Value-clear the full pool before seeding','    if (attach_runtime_component) {')
    particle=span(header,'    // Literal FFLASH_PARTICLE layout/state','    void SimulateTick();')
    vector=function(obj,'void ConvertToVector(')
    prelude=r'''
#include <cstdio>
#include <cstring>
#include <cmath>
#include <fstream>
#include <vector>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
#define REV_FIX_Z_VALUE(z) ((float)(z)*1.46f)
#define WORLD3D_Z_SCALE 1.5f
enum class EFxDebugMode{Normal};using MeshHandle=unsigned;
struct SMeshVertex{float pos[3]{},normal[3]{},uv[2]{};};
struct Material{float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0;};struct S3DMat{Material matdesc;};
struct SHelperMeshSubmit{unsigned mesh=1;bool additive_blend=false;int retail_lighting=0;float retail_normal_z_scale=1;float world[16]{},diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0,sort_depth=0;};
struct TRenderer{std::vector<SHelperMeshSubmit>draws;void SubmitHelperMesh(const SHelperMeshSubmit&s){draws.push_back(s);}unsigned RegisterMeshAsset(uint64_t,const SMeshVertex*,int,const uint16_t*,int,TTextureHandle){return1;}void AddMeshAssetRef(unsigned){}};TRenderer renderer;TRenderer*Renderer=&renderer;
struct TObjectInstance{S3DPoint Pos()const{return{};}};struct Imagery{int ImageryId()const{return1;}}imagery;
struct Target{TObjectInstance*Get(){return nullptr;}void Clear(){}};struct Spell{void Kill(){}};
struct World{hmm_mat4 m;World(){MtxClear(&m);m.Elements[2][2]=WORLD3D_Z_SCALE;}const hmm_mat4&Matrix()const{return m;}};
std::ifstream inputs;int rng_count=0;int random(int lo,int hi){int a,b,v;if(!(inputs>>a>>b>>v)||a!=lo||b!=hi)throw std::runtime_error("Native/production RNG order drift");++rng_count;return v;}
short DistX[256],DistY[256];void ConvertToVector(int32_t,int32_t,S3DPoint&,int32_t=0);
struct TFireFlashEffect_Bespoke{PARTICLE
 Particle particles_[150]{};mutable unsigned meshes_[2]{1,2};TTextureHandle textures_[2]{1,2};std::vector<SMeshVertex>vertices_[2];std::vector<uint16_t>indices_[2];S3DMat materials_[2]{};
 Target target_;Spell*spell=nullptr;World world;hmm_vec3 gsphere_{0,0,30};float facing_=0,mainscale_=0,gsize_=1,rsize_=0;int frameon_=0;double sim_accum_ms_=0;bool initialized_=false,alive_=false,runtime_owned_=false;
 Imagery*GetImagery()const{return&imagery;}const World&Transform()const{return world;}void SetCommandDone(bool){};void KillThisEffect(){};
 void Initialize(bool);void SimulateTick();void Advance(double);void Submit(EFxDebugMode);bool BindMeshes()const;};
'''.replace('PARTICLE',particle).replace('return1','return 1')
    trailer=r'''
int main(int argc,char**argv){inputs.open(argv[1]);std::ifstream tables(argv[2],std::ios::binary),assets(argv[3],std::ios::binary);tables.read((char*)DistX,512);tables.read((char*)DistY,512);
 TFireFlashEffect_Bespoke effect;for(int j=0;j<2;++j){effect.vertices_[j].resize(4);effect.indices_[j].resize(6);assets.read((char*)effect.vertices_[j].data(),128);assets.read((char*)effect.indices_[j].data(),12);assets.read((char*)&effect.materials_[j].matdesc,68);}
 effect.Initialize(false);
 for(int tick=0;tick<=TICKS;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();effect.Submit(EFxDebugMode::Normal);
 printf("S %d %d %d %.9g %.9g %.9g %.9g %.9g %.9g\n",tick,effect.frameon_,rng_count,effect.gsphere_.X,effect.gsphere_.Y,effect.gsphere_.Z,effect.mainscale_,effect.gsize_,effect.rsize_);
 for(int i=0;i<150;++i){const auto&p=effect.particles_[i];printf("P %d %d",tick,i);for(const auto*v:{&p.pos,&p.pivot,&p.vel,&p.angle,&p.angvel})printf(" %.9g %.9g %.9g",v->X,v->Y,v->Z);printf(" %.9g %.9g %d %d %d %d %d\n",p.scale,p.dist,p.state,p.life,p.startfade,p.stopfade,p.color);}
 for(const auto&d:renderer.draws){printf("D %d %u %d",tick,d.mesh-1,int(d.additive_blend));for(int row=0;row<4;++row)for(int col=0;col<4;++col)printf(" %.9g",d.world[col*4+row]);printf(" %.9g\n",d.retail_normal_z_scale);}}
}
'''.replace('TICKS',str(ticks))
    cpp=output/'fireflash-port.cpp';cpp.write_text(prelude+vector+'\nvoid TFireFlashEffect_Bespoke::Initialize(bool attach_runtime_component){\n'+initialize+'}\n'+'\n'.join(methods.values())+trailer)
    binary=output/'fireflash-port';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(cpp),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    compile_result=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(compile_result.stdout+compile_result.stderr)
    if compile_result.returncode:raise RuntimeError('Production FireFlash compile failed')
    assets=output/'authored-parts.bin';assets.write_bytes(b''.join(struct.pack('<32f',*(n for v in p['vertices']for n in v))+struct.pack('<6H',*p['indices'])+struct.pack('<17f',*p['material'])for p in parts))
    tables=output/'native-facing-tables.bin';tables.write_bytes(lookup)
    result=subprocess.run([str(binary),str(rng),str(tables),str(assets)],capture_output=True,text=True);(output/'port-trace.txt').write_text(result.stdout);(output/'port-run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('Production FireFlash trace failed')
    frames={}
    for line in result.stdout.splitlines():
        p=line.split();tick=int(p[1])
        if p[0]=='S':frames[tick]=dict(frame=int(p[2]),random_count=int(p[3]),values=list(map(float,p[4:])),particles=[],draws=[])
        elif p[0]=='P':frames[tick]['particles'].append(dict(index=int(p[2]),values=list(map(float,p[3:20])),integers=list(map(int,p[20:]))))
        else:frames[tick]['draws'].append(dict(object=int(p[2]),additive=bool(int(p[3])),matrix=list(map(float,p[4:20])),normal_z_scale=float(p[20])))
    return frames,dict(command=command,binary_sha256=sha(binary.read_bytes()),driver_sha256=sha(cpp.read_bytes()),method_sha256={k:sha(v.encode())for k,v in methods.items()},initialize_sha256=sha(initialize.encode()),owner_common_z_scale=1.5)


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--state-only',action='store_true');args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=False)
    with zipfile.ZipFile(args.archive)as archive:asset=archive.read('Imagery/Magic/fireflash.i3d')
    fixture=FireFlashFixture(args.executable,asset);states=[];draws={};fixture.reset()
    for tick in range(101):
        if tick:fixture.step()
        packets=fixture.render();states.append(fixture.state())
        if tick in(0,1,12,26,30,35,45,60,71,100):draws[tick]=packets
    inputs=args.output/'native-rng.txt';inputs.write_text(''.join('%d %d %d\n'%row for row in fixture.random))
    port,compiled=build_port(args.output,fixture.parts,inputs,fixture.lookup,100);cases=[];errors=[]
    for tick,native in enumerate(states):
        modern=port[tick];failures=[];float_errors=0;max_error=0
        if len(native['particles'])!=150 or len(modern['particles'])!=150:raise AssertionError('Complete150-record pools required')
        if native['frame']!=modern['frame']or native['random_count']!=modern['random_count']:failures.append('frame/RNG')
        for p,q in zip(native['particles'],modern['particles']):
            if p['integers']!=q['integers']:failures.append('integer fields')
            for a,b in zip(p['values'],q['values']):
                if f32(a)!=f32(b):float_errors+=1;max_error=max(max_error,abs(a-b))
        if float_errors:failures.append('float32 fields')
        pixel_pair=None;diff=None
        if tick in draws:
            if len(draws[tick])!=len(modern['draws']):failures.append('submission count')
            if any(not d['additive']for d in modern['draws']):failures.append('native ONE/ONE versus nonadditive port metadata')
            if not args.state_only:
                native_color,z=fixture.pixels(draws[tick]);revised,rz=fixture.pixels(modern['draws'],common=True)
                diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(WIDTH*HEIGHT)+'H',native_color),struct.unpack('<'+str(WIDTH*HEIGHT)+'H',revised)))
                pixel_pair=[sha(native_color),sha(revised),sha(z),sha(rz)]
                if diff or z!=rz:failures.append('software pixels/depth in declared coordinate bridge')
                save_rgb565_png(args.output/f'retail-{tick:03d}.png',native_color,WIDTH,HEIGHT);save_rgb565_png(args.output/f'port-{tick:03d}.png',revised,WIDTH,HEIGHT)
        if failures:errors.append(dict(tick=tick,fields=sorted(set(failures))))
        cases.append(dict(tick=tick,float32_differences=float_errors,max_float_difference=max_error,original_state=native,port_state=modern,
            original_draw_count=len(draws[tick])if tick in draws else None,differing_rgb565_pixels=diff,image_hashes=pixel_pair,errors=sorted(set(failures))))
    report=dict(status='differences_found'if errors else'pass',errors=errors,type_id='0x37780ae2',retail_sha256=RETAIL_SHA,asset_sha256=ASSET_SHA,
        native_particle_count=150,ticks=100,native_birth_rng=states[0]['random_count'],native_final_rng=states[-1]['random_count'],native_mode=8,
        native_blend_mapping=fixture.blend_mapping,
        original_initialize='0x4e18c0',original_animate='0x4e1ad0',original_render='0x4e1fa0',compiled_port=compiled,probe_sha256=sha(Path(__file__).read_bytes()),
        scope='Nullspell stationary owner with actually moving native particle pool; literal150records and both authored meshes/textures. Native Init/Animate/Render and matrix packets; compiled current SimulateTick/Advance/Submit. '+
            ('Software pixels not executed in this state/packet diagnostic. 'if args.state_only else'Original software projection/raster pair executed. ')+
            'Base animator/audio/mesh submission interfaces adapted; identity native D3D owner vs explicit port common-world1.5Z and inverseREV_FIX1.46 for software bridge. Normal lighting/device blend/culling/common-Z residuals and natural spell/target/owner reaper remain separate; discrepancies never fitted.',
        software_pixel_comparison_executed=not args.state_only,dosbox_used=False,full_acceptance=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items()if k not in('cases','compiled_port','errors')},indent=2))


if __name__=='__main__':main()
