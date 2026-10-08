#!/usr/bin/env python3
"""Bounded moving Fireball head/glow render A/B; full animator remains separate.

Motion comes from original Pulse/Move in the same Runtime. Original render
matrices/UVs and software pixels execute from the EXE. Selected head/glow pose
inputs isolate those kernels rather than claiming an entire Fireball lifecycle.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import statistics
import struct
import subprocess
import time
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_EIP,UC_X86_REG_ESP
from projectile_probe import MissileProbe
from software_probe import SoftwareFixture,save_rgb565_png

ROOT=Path(__file__).resolve().parents[2]
ASSET_SHA='3c78b8e2dbb1251e9267b65caa92b038a026c8d1fa2919cc75e1d8ec7cd7a6a1'
VERTEX_OFFSET=0x270;INDEX_OFFSET=0xf70;TEXTURE_OFFSET=0x1514
INDICES=(2,0,3,1,3,0)


def sha(data):return hashlib.sha256(data).hexdigest()


def function(text,signature):
    start=text.index(signature);opening=text.index('{',start);depth=1;end=opening+1
    while depth:
        if text[end]=='{':depth+=1
        elif text[end]=='}':depth-=1
        end+=1
    return text[start:end]


def port_driver(output,asset):
    """Actual SubmitBillboards body with a capturing renderer/owner fixture."""
    effect=(ROOT/'src/effect.cpp').read_text();header=(ROOT/'src/effect.h').read_text()
    body=function(effect,'void TFireBallEffect::SubmitBillboards(')
    uv=function(effect,'void TFireBallEffect::FillAtlasUv(')
    helper=function(effect,'void TFireBallEffect::SubmitAuthoredCard(') if 'void TFireBallEffect::SubmitAuthoredCard(' in effect else ''
    trail_helper=function(effect,'void TFireBallEffect::SubmitAuthoredTrail(') if 'void TFireBallEffect::SubmitAuthoredTrail(' in effect else ''
    spark_helper=function(effect,'void TFireBallEffect::SubmitAuthoredSpark(') if 'void TFireBallEffect::SubmitAuthoredSpark(' in effect else ''
    constants='\n'.join(line for line in header.splitlines() if line.startswith('inline constexpr') and 'kFireBall' in line)
    data=header[header.index('struct SFireBallData'):header.index('_CLASSDEF(TFireBallEffect)',header.index('struct SFireBallData'))]
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <cstring>
#include "math3d.h"
#include "render3d_types.h"
#if __has_include("fireballquad.h")
#include "fireballquad.h"
#endif
struct S3DPoint {int32_t x{},y{},z{};};
enum class EFxBlend:uint8_t{Alpha,Additive};
enum class EFxDepthMode:uint8_t{TestNoWrite};
enum class EFxLightMode:uint8_t{Unlit};
enum class EFxPipeline:uint16_t{Billboard=1,Particle=2};
enum class EFxDebugMode:uint8_t{Normal,SolidColor,FullTexture,CurrentFrame};
enum class EFxBillboardOrientation:uint8_t{ScreenAligned,WorldXY};
struct Key {TTextureHandle texture{};uint16_t pipeline_id{};uint8_t blend{},depth_mode{};};
struct SParticleDrawItem {float world_pos[3]{},size_wu[2]{},uv_rect[4]{},color_rgba[4]{};
    float rotation_rad{};Key key;EFxLightMode light_mode{};EFxBillboardOrientation orientation{};EFxDebugMode debug_mode{};};
struct SQuadDrawItem {float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{1,1,1,1};
    Key key;uint8_t corner_count{4},retail_texture{};EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer {std::vector<SParticleDrawItem> particles;std::vector<SQuadDrawItem> quads;
    void SubmitFxParticle(const SParticleDrawItem&p){particles.push_back(p);}
    void SubmitFxQuad(const SQuadDrawItem&q){quads.push_back(q);}};
TRenderer* Renderer=nullptr;
struct World{hmm_mat4 matrix;const hmm_mat4& Matrix()const{return matrix;}};
'''
    class_body=r'''
struct TFireBallEffect {
    S3DPoint position{};World world;
    int state_{1},aim_angle_{64},frame_count_{16},glow_frame_{3},face{};
    SFireBallData fireball_;SFireBallData trail_[kFireBallTrailSize],burst_[kFireBallMaxBurst];
    SFireBallSpark sparks_[kFireBallMaxSpark];
    struct Asset {TTextureHandle box01_tex{1},box02_tex{2};S3DVertex box01_vertices[4]{},box02_vertices[4]{};bool has_box01_quad{true},has_box02_quad{true};float atlas_cell_w{.25f},atlas_cell_h{.25f};} asset_;
    const S3DPoint& Pos()const{return position;}int GetFace()const{return face;}
    const World& Transform()const{return world;}
    void FillAtlasUv(int32_t,float*)const;
    void SubmitBillboards(EFxDebugMode)const;
    void SubmitAuthoredCard(const SFireBallData&,int32_t,bool,EFxDebugMode)const;
    void SubmitAuthoredTrail(EFxDebugMode)const;
    void SubmitAuthoredSpark(const SFireBallSpark&,EFxDebugMode)const;
    bool EnsureAuthoredSpark()const{return asset_.has_box02_quad;}
    bool EnsureAuthoredQuad()const{return asset_.has_box01_quad;}
};
'''
    trailer=r'''
int main(int argc,char**argv){
    if(argc!=11&&argc!=12&&argc!=13)return 2;
    TRenderer renderer;Renderer=&renderer;TFireBallEffect effect;
    FILE*f=std::fopen(argv[1],"rb");if(!f)return 3;
    if(std::fread(effect.asset_.box01_vertices,1,128,f)!=128)return 4;
    if(argc==13&&std::fread(effect.asset_.box02_vertices,1,128,f)!=128)return 7;std::fclose(f);
    effect.position={std::atoi(argv[2]),std::atoi(argv[3]),std::atoi(argv[4])};
    effect.face=std::atoi(argv[5]);effect.aim_angle_=std::atoi(argv[6]);
    effect.fireball_.frame=std::atof(argv[7]);effect.fireball_.rotation=std::atof(argv[8]);
    effect.fireball_.scale=std::atof(argv[9]);effect.fireball_.glow=std::atof(argv[10]);
    if(argc>=12){FILE*t=std::fopen(argv[11],"rb");if(!t)return 5;
        for(auto&slot:effect.trail_){float record[7];if(std::fread(record,1,28,t)!=28)return 6;
            int rotation;std::memcpy(&rotation,&record[4],4);
            slot.pos={record[0],record[1],record[2]};slot.scale=record[3];slot.rotation=float(rotation);
            slot.frame=record[5];slot.glow=record[6];slot.used=slot.scale>0;}
        std::fclose(t);}
    if(argc==13){FILE*t=std::fopen(argv[12],"rb");if(!t)return 8;
        for(auto&slot:effect.sparks_){float record[8];uint32_t used,flicker;
            if(std::fread(record,1,32,t)!=32||std::fread(&used,1,4,t)!=4||std::fread(&flicker,1,4,t)!=4)return 9;
            slot.pos={record[0],record[1],record[2]};slot.vel={record[3],record[4],record[5]};slot.scale=record[6];
            std::memcpy(&slot.life,&record[7],4);slot.used=used!=0;slot.flicker_status=flicker!=0;}
        std::fclose(t);}
    MtxClear(&effect.world.matrix);const hmm_vec3 scale{1,1,1.5f};MtxScale(&effect.world.matrix,&scale);
    const hmm_vec3 position{float(effect.position.x),float(effect.position.y),float(effect.position.z)};
    MtxTranslate(&effect.world.matrix,&position);
    effect.SubmitBillboards(EFxDebugMode::Normal);
    std::printf("{\"particles\":[");
    for(size_t n=0;n<renderer.particles.size();++n){const auto&p=renderer.particles[n];
        if(n)std::printf(",");std::printf("{\"position\":[%.9g,%.9g,%.9g],\"size\":[%.9g,%.9g],\"uv\":[%.9g,%.9g,%.9g,%.9g],\"rotation\":%.9g,\"orientation\":%d,\"texture\":%u}",
        p.world_pos[0],p.world_pos[1],p.world_pos[2],p.size_wu[0],p.size_wu[1],p.uv_rect[0],p.uv_rect[1],p.uv_rect[2],p.uv_rect[3],p.rotation_rad,int(p.orientation),p.key.texture);}
    std::printf("],\"quads\":[");
    for(size_t n=0;n<renderer.quads.size();++n){const auto&q=renderer.quads[n];if(n)std::printf(",");
        std::printf("{\"positions\":[");for(int i=0;i<4;++i){if(i)std::printf(",");std::printf("[%.9g,%.9g,%.9g]",q.world_pos[i][0],q.world_pos[i][1],q.world_pos[i][2]);}
        std::printf("],\"uvs\":[");for(int i=0;i<4;++i){if(i)std::printf(",");std::printf("[%.9g,%.9g]",q.uv[i][0],q.uv[i][1]);}
        std::printf("],\"count\":%d,\"texture\":%u}",q.corner_count,q.key.texture);}
    std::puts("]}");
}
'''
    source=output/'port-submit.cpp';source.write_text(prelude+'\n'+constants+'\n'+data+'\n'+class_body+'\n'+uv+'\n'+helper+'\n'+trail_helper+'\n'+spark_helper+'\n'+body+'\n'+trailer)
    vertices=output/'authored-box01.bin';vertices.write_bytes(asset[VERTEX_OFFSET:VERTEX_OFFSET+128])
    binary=output/'port-submit'
    command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp')]
    if helper:command.append(str(ROOT/'src/fireballquad.cpp'))
    command+=['-o',str(binary)]
    result=subprocess.run(command,text=True,capture_output=True);(output/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Production Submit compile failed; see compile.log')
    return binary,vertices,dict(command=command,source_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()))


class HeadFixture:
    def __init__(self,executable,asset,width=320,height=180,camera=(0,0,128),vertex_offset=VERTEX_OFFSET,texture_offset=TEXTURE_OFFSET):
        self.surface=SoftwareFixture(executable,width,height);self.vm=self.surface.vm;self.camera=tuple(camera)
        self.missile=MissileProbe(executable,vm=self.vm)
        self.head_address=0x511dd0;self.glow_address=0x511fa0
        self.animator=self.vm.allocate(0x600);self.obj=self.vm.allocate(0x200)
        self.lverts=self.vm.allocate(128);self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)
        self.owner_matrix=self.vm.allocate(64);self.combined_matrix=self.vm.allocate(64)
        self.vm.put_u32(self.animator+4,self.missile.obj);self.vm.put_u32(self.animator+8,self.vm.allocate(0x200))
        self.vm.put_u32(self.obj+0xa0,4);self.vm.put_u32(self.obj+0xa4,self.lverts)
        self.vm.put_u32(self.animator+0x244,3)
        self.points=[struct.unpack_from('<3f',asset,vertex_offset+32*i) for i in range(4)]
        for i,p in enumerate(self.points):self.vm.write(self.lverts+i*32,struct.pack('<3f3I2f',*p,0,0xffffffff,0,0,0))
        self.surface.set_texture(256,256,asset[texture_offset:texture_offset+131072],format='ARGB4444')
        for address in (0x40eef0,0x40c960,0x40a8f0,0x40c9c0):
            self.vm.uc.hook_add(UC_HOOK_CODE,self.boundary,begin=address,end=address)
        self.surface.clear();self.surface.checkpoint()

    def boundary(self,uc,address,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);cleanup={0x40eef0:4,0x40c960:0,0x40a8f0:24,0x40c9c0:4}[address]
        if address==0x40eef0:
            if self.vm.u32(sp+4)!=0:raise AssertionError('Head kernel requested different object')
            uc.reg_write(UC_X86_REG_EAX,self.obj)
        uc.reg_write(UC_X86_REG_EIP,self.vm.u32(sp));uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)

    def packet(self,glow,frame,rotation,scale,glow_factor,face):
        vm=self.vm
        vm.write(self.missile.obj+0x36,bytes([face]))
        vm.write(self.animator+0x4b8,struct.pack('<fif',scale,rotation,float(frame)))
        vm.write(self.animator+0x4c4,struct.pack('<f',glow_factor))
        vm.call(self.glow_address if glow else self.head_address,this=self.animator)
        position=self.missile.inspect()['position']
        world=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.5,0.,*map(float,position),1.]
        vm.write(self.owner_matrix,struct.pack('<16f',*world))
        # Original RenderObject composes local*owner before transforming verts.
        vm.call(0x43aa90,(self.combined_matrix,self.obj+0x58,self.owner_matrix))
        points=[]
        for p in self.points:
            vm.write(self.point,struct.pack('<3f',*p));vm.call(0x43ad80,(self.combined_matrix,self.point,self.result))
            points.append(struct.unpack('<3f',vm.uc.mem_read(self.result,12)))
        # Explicit upstream owner matrix, model-Z stretch1.5 and translation.
        # Owner rotation is identity in this bounded fixture; face input tests
        # the render kernel independently of that chosen matrix.
        uvs=[struct.unpack('<2f',vm.uc.mem_read(self.lverts+i*32+24,8)) for i in range(4)]
        return dict(positions=points,uvs=uvs,count=4)

    def pixels(self,cards,instruction_limit=1000000):
        self.surface.clear()
        for card in cards:
            points=self.surface.project(card['positions'],camera=self.camera,zdist=1925)
            vertices=[(*p,31,31,31,31,*uv) for p,uv in zip(points,card['uvs'])]
            indices=INDICES if card['count']==4 else (0,1,2)
            self.surface.draw(vertices,indices,True,False,instruction_limit=instruction_limit)
        return self.vm.surface_bytes('screen')


def particle_card(p):
    # Diagnostic expansion of the current production WorldXY shader contract.
    # The actual Submit body supplies these descriptors; this is not GPU parity.
    if p['orientation']!=1:raise ValueError('Only current WorldXY particle diagnostic supported')
    c,s=math.cos(p['rotation']),math.sin(p['rotation']);points=[];uvs=[]
    for x,y in ((-.5,-.5),(.5,-.5),(-.5,.5),(.5,.5)):
        points.append((p['position'][0]+(c*x-s*y)*p['size'][0],p['position'][1]+(s*x+c*y)*p['size'][1],p['position'][2]))
        uvs.append((p['uv'][0]+(x+.5)*p['uv'][2],p['uv'][1]+(y+.5)*p['uv'][3]))
    return dict(positions=points,uvs=uvs,count=4)


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--output',type=Path,required=True);parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    parser.add_argument('--producer',type=Path,help='Retained compiled producer for a before regression')
    parser.add_argument('--producer-manifest',type=Path,help='Manifest pinning the retained producer SHA')
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(args.archive)as archive:asset=archive.read('Imagery/Magic/newfireball.i3d')
    if sha(asset)!=ASSET_SHA or struct.unpack_from('<6H',asset,INDEX_OFFSET)!=INDICES:raise ValueError('Wrong authored asset')
    if struct.unpack_from('<I',asset,TEXTURE_OFFSET-4)[0]!=4:raise ValueError('Expected uncompressed atlas')
    paths=[ROOT/p for p in ('src/effect.cpp','src/effect.h','src/math3d.cpp','src/math3d.h')]
    paths+=[p for p in (ROOT/'src/fireballquad.h',ROOT/'src/fireballquad.cpp')if p.exists()]
    before={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in paths}
    if args.producer:
        if not args.producer_manifest:parser.error('--producer requires its retained --producer-manifest')
        producer_report=json.loads(args.producer_manifest.read_text());compiled=dict(producer_report['compiled_port'])
        binary=args.producer.resolve()
        if sha(binary.read_bytes())!=compiled['binary_sha256']:raise ValueError('Retained producer changed')
        vertices=args.output/'authored-box01.bin';vertices.write_bytes(asset[VERTEX_OFFSET:VERTEX_OFFSET+128])
        compiled['retained_manifest']=str(args.producer_manifest.resolve())
    else:binary,vertices,compiled=port_driver(args.output,asset)
    started=time.perf_counter();fixture=HeadFixture(args.executable,asset);setup_ms=(time.perf_counter()-started)*1000
    cases=[];timings=[];original_hashes=[];pixel_errors=0;geometry_errors=0;field_checks=0
    for face in (0,32,64,255):
        # All motion is computed by original missile Pulse in this same VM.
        fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0)
        rows=fixture.missile.step(1);poses=[]
        for tick in range(12):
            if tick:rows=fixture.missile.step(1)
            motion=rows[-1];frame=tick%16;rotation=tick*2
            pose=dict(position=motion['position'],frame=frame,rotation=rotation,scale=.3,glow=1.5,face=face)
            started=time.perf_counter()
            retail=[fixture.packet(True,frame,rotation,.3,1.5,face),fixture.packet(False,frame,rotation,.3,1.5,face)]
            data=json.loads(subprocess.check_output([str(binary),str(vertices),*[str(p) for p in pose['position']],
                str(face),str(motion['angle']),str(frame),str(rotation),'.3','1.5'],text=True))
            port=[particle_card(p) for p in data['particles']]+data['quads']
            if len(port)!=2:raise AssertionError(f'Expected head+glow only; got{len(port)} cards')
            original=fixture.pixels(retail);candidate=fixture.pixels(port)
            timings.append((time.perf_counter()-started)*1000)
            pixel_difference=sum(a!=b for a,b in zip(struct.unpack('<57600H',original),struct.unpack('<57600H',candidate)))
            pixel_errors+=pixel_difference
            differences=[]
            for r,p in zip(retail,port):
                for kind in ('positions','uvs'):
                    for a,b in zip(r[kind],p[kind]):
                        for x,y in zip(a,b):
                            field_checks+=1;differences.append(abs(x-y))
                            if abs(x-y)>(3e-5 if kind=='positions' else 1e-7):geometry_errors+=1
            digest=sha(original);original_hashes.append(digest)
            if tick in (0,6,11):
                save_rgb565_png(args.output/f'retail-face{face:03d}-tick{tick:02d}.png',original,320,180)
                save_rgb565_png(args.output/f'port-face{face:03d}-tick{tick:02d}.png',candidate,320,180)
            poses.append(dict(**pose,original_motion=motion,original_cards=retail,port_cards=port,
                differing_rgb565_pixels=pixel_difference,max_field_error=max(differences),
                retail_rgb565_sha256=digest,port_rgb565_sha256=sha(candidate)))
        if len({tuple(p['position']) for p in poses})!=12:raise AssertionError('Moving fixture did not move every tick')
        cases.append(dict(face=face,poses=poses))
    after={str(p.relative_to(ROOT)):sha(p.read_bytes())for p in paths}
    if before!=after:raise AssertionError('Production source changed during render comparison')
    replay_checks=0
    for case in cases:
        fixture.surface.restore();fixture.missile.configure([0,0,128],[1000,0,128],target_hp=0)
        for pose in case['poses']:
            motion=fixture.missile.step(1)[-1]
            if motion!=pose['original_motion']:raise AssertionError('Moving pose failed warm reset')
            cards=[fixture.packet(True,pose['frame'],pose['rotation'],pose['scale'],pose['glow'],pose['face']),
                   fixture.packet(False,pose['frame'],pose['rotation'],pose['scale'],pose['glow'],pose['face'])]
            if sha(fixture.pixels(cards))!=pose['retail_rgb565_sha256']:
                raise AssertionError('Original moving pixels failed warm replay')
            replay_checks+=1
    report=dict(status='pass' if not geometry_errors and not pixel_errors else 'differences_found',
        asset_sha256=ASSET_SHA,asset_member='Imagery/Magic/newfireball.i3d',vertex_offset=hex(VERTEX_OFFSET),index_offset=hex(INDEX_OFFSET),texture_offset=hex(TEXTURE_OFFSET),
        original_functions=['0x510220','0x470920','0x511dd0','0x511fa0','0x43aa90','0x43ad80','0x56d960'],
        source_sha256=after,compiled_port=compiled,setup_ms=setup_ms,median_warm_pair_ms=statistics.median(timings),
        case_count=len(cases),moving_ticks_per_case=12,render_pairs=len(timings),field_checks=field_checks,
        geometry_error_count=geometry_errors,differing_rgb565_pixels=pixel_errors,distinct_original_frames=len(set(original_hashes)),
        exact_original_warm_replays=replay_checks,
        cases=cases,host_computed_trajectory=False,original_moving_missile=True,pixel_raster_intercepted=False,
        actual_metal_compared=False,full_fireball_animation_damage_context=False,
        scope='Original moving missile source/destination in same Runtime, then original head/glow matrix/UV kernels and textured SW pixels. '
              'Pose frame/spin/scale/glow are explicitly selected render inputs, not a full original animator lifecycle. '
              'Exact compiled production SubmitBillboards captures either old WorldXY particle descriptors (diagnostic shader expansion) '
              'or authored quad geometry. Both use original raster; Metal/backend parity remains separate.')
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k not in ('cases',)},indent=2))
    if geometry_errors or pixel_errors:raise SystemExit(1)


if __name__=='__main__':main()
