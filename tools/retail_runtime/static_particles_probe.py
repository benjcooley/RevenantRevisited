#!/usr/bin/env python3
"""Three exact static-emitter particle profiles versus native software geometry.

The modern raw-world Z packet is compared in its declared original MODELZ
domain. Shared original projection/raster can isolate frontend differences;
this never claims the real map/Metal projector or normal material lighting.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import zipfile

from speed_controller_preflight import NativeSpeed, ROOT, read_asset
from static_particles_profile_contract import PROFILES,Y_PROFILES
from unicorn.x86_const import UC_X86_REG_ECX

from speed_state_compare import compare as compare_state
from software_probe import save_rgb565_png
from unicorn.x86_const import (UC_X86_REG_ESI,UC_X86_REG_EBX,UC_X86_REG_ESP,
                               UC_X86_REG_FPCW,UC_X86_REG_EIP)


def fn(source,start):
    a=source.index(start);b=source.index('{',a);n=1;c=b+1
    while n:n+=(source[c]=='{')-(source[c]=='}');c+=1
    return source[a:c]


PRELUDE = r'''
#include "authoredpartsys.h"
#include "math3d.h"
#include "goldauthoredmatrix.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>
using TTextureHandle=uint32_t;constexpr TTextureHandle kInvalidTexture=0;
enum class EFxBlend:uint8_t{Alpha=0,Additive=1,AdditiveStraight=2,PremulAlpha=3};
enum class EFxDepthMode:uint8_t{TestNoWrite=0,None=1,TestWrite=2};
enum class EFxDebugMode:uint8_t{Normal=0};enum class EFxLightMode:uint8_t{Unlit=0};
enum class EFxPipeline:uint16_t{Billboard=1,Particle=2,Strip=3};
struct SFxBatchKey{TTextureHandle texture=0;uint16_t pipeline_id=1;uint8_t blend=0,depth_mode=0;};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{};SFxBatchKey key;
 bool retail_software_projection=false;
 uint8_t corner_count=4,retail_texture=0;EFxDebugMode debug_mode=EFxDebugMode::Normal;EFxLightMode light_mode=EFxLightMode::Unlit;};
struct S3DVertex{hmm_vec3 pos,normal;float tu,tv;};
uint32_t type_id=0;
struct Owner{uint32_t ObjId()const{return type_id;}int GetState()const{return 0;}};
struct Imagery{uint32_t static_particles_profile=type_id;
 STATIC_PROFILE_METHOD
 bool HasRetailMightPartSysProfile()const{return false;}
 bool HasRetailImmortalmightPartSysProfile()const{return false;}
 bool HasRetailFmasteryPartSysProfile()const{return false;}
 bool speed_partsys_profile=false,quicksilver_partsys_profile=false;
 bool HasRetailSpeedPartSysProfile()const{return false;}
 SPEED_FAMILY_METHOD
 bool HasCombatFlashStart1PartSysProfile()const{return false;}
 TTextureHandle GetTextureHandle(int index){if(index!=0)std::abort();return 1;}};
struct TRenderer{std::vector<SQuadDrawItem>quads;void SubmitFxQuad(const SQuadDrawItem&q){quads.push_back(q);}};
struct TTime{static int frame;static int FrameCount(){return frame;}};int TTime::frame=0;
unsigned random_state=1;int random(int a,int b){random_state=random_state*214013u+2531011u;return a+((random_state>>16)&32767u)%(b-a+1);}
struct T3DAnimator{struct SPartSysControllers{
 struct Track{int state=0,prototype=0,texture_slot=1;std::array<S3DVertex,4>vertices;};
 struct Controller{Track track;authored_partsys::State simulation;int64_t last_render_frame=-1;};
 std::vector<Controller>controllers;bool unsupported=false;uint64_t renders=0,quads=0;};
 std::unique_ptr<SPartSysControllers>partsys_controllers;Owner*inst;Imagery im;
 Imagery*Get3DImagery(){return &im;}
 int32_t SubmitPartSys(TRenderer&,int32_t,int32_t,const hmm_vec3&,bool=false);};
constexpr float WORLD3D_Z_SCALE=1.5f;
'''
MAIN = r'''
int main(int argc,char**argv){if(argc!=4)return 2;type_id=std::strtoul(argv[1],nullptr,0);
 authored_partsys::Definition d;std::string diag;if(!authored_partsys::ParseDefinition(argv[2],d,diag))return 3;
 std::ifstream f(argv[3],std::ios::binary);int ticks,emitters;f.read((char*)&ticks,4);f.read((char*)&emitters,4);
 if(ticks!=90||emitters!=int(d.objects.size()))return 8;
 Owner owner;T3DAnimator a;a.inst=&owner;a.partsys_controllers=std::make_unique<T3DAnimator::SPartSysControllers>();
 a.partsys_controllers->controllers.emplace_back();auto&c=a.partsys_controllers->controllers[0];
 authored_partsys::TickInputs in;in.owner_position={0,0,0};in.has_object_zero_origin=true;
 f.read((char*)&c.track.prototype,4);f.read((char*)in.object_zero_origin.data(),12);f.read((char*)c.track.vertices.data(),128);
 in.emitters.resize(emitters);in.ground_height=[](int,int,int){return 0;};TRenderer renderer;
 for(int tick=0;tick<ticks;++tick){f.read((char*)&in.animation_frame,4);
 for(auto&e:in.emitters){f.read((char*)e.position.data(),12);f.read((char*)e.scale.data(),12);f.read((char*)e.matrix.data(),64);for(int i=0;i<3;++i)e.origin[i]=e.matrix[12+i];}
 if(tick==0&&!c.simulation.Initialize(d,in,0,diag))return 4;
 if(tick==60)in.owner_position={16,-8,4};TTime::frame=tick;renderer.quads.clear();
 c.simulation.Advance(in,[](int a,int b){return random(a,b);});
 int n=a.SubmitPartSys(renderer,c.track.prototype,1,{1,1,1});if(n!=int(renderer.quads.size()))return 5;
 size_t q=0;for(size_t slot=0;slot<c.simulation.Capacity();++slot){const auto&p=c.simulation.Particles()[slot];if(!p.alive||p.scale<=9.999999747378752e-6f)continue;
 const auto&item=renderer.quads[q++];printf("%d %zu %.9g %d %d %d",tick,slot,p.position[2],int(item.key.blend),int(item.key.depth_mode),item.retail_texture);
 for(float v:item.color_rgba)printf(" %.9g",v);
 for(int corner=0;corner<4;++corner){for(float v:item.world_pos[corner])printf(" %.9g",v);for(float v:item.uv[corner])printf(" %.9g",v);}puts("");}
 if(a.SubmitPartSys(renderer,c.track.prototype,1,{1,1,1})!=0)return 6;
 }
 return f?0:7;
}
'''



def sha(data):return hashlib.sha256(data).hexdigest()


def registry(f):
    v=f.vm;v.put_u32(0x5e872c,0);v.call(0x406160)
    for row in f.dispatch['registrations']:v.call(int(row['call'],16)-10)
    name=f.put_string(f.asset['name']);builder=v.call(0x40dca0,(name,))
    if v.u32(0x5e872c)!=93 or builder!=0x5e8508 or v.u32(v.u32(builder))!=0x40dc00:
        raise AssertionError('Complete native registry selects a different factory')
    return dict(default_constructor='0x406160',complete_named_constructors=92,registry_count=93,
        actual_builder=hex(builder),actual_factory=hex(v.u32(v.u32(builder))),actual_animator_vtable=hex(v.u32(f.animator)))


def punctuation(f):
    v=f.vm;sp=v.STACK+v.STACK_SIZE-0x1000
    for index,o in enumerate(f.asset['objects']):
        name=f.put_string(o['name']);obj=f.animobjs[index];v.put_u32(sp+0x1c,name)
        v.uc.reg_write(UC_X86_REG_ESP,sp);v.uc.reg_write(UC_X86_REG_ECX,name);v.uc.reg_write(UC_X86_REG_EBX,obj)
        v.uc.emu_start(0x409f61,0x409fe0,count=10000)
        if v.uc.reg_read(UC_X86_REG_EIP)!=0x409fe0:raise AssertionError('Original punctuation did not finish')
        if bool(v.u32(obj)&0x20000000)!=o['name'].startswith('#') or bool(v.u32(obj)&1)!=o['name'].startswith('*'):raise AssertionError('Exact native camera-facing flags differ')


def run(executable,archive,output,profile,independent_poses=None):
    output.mkdir(parents=True,exist_ok=True)
    source_paths=('src/3dimage.cpp','src/3dimage.h','src/staticpartsysprofiles.h','src/authoredpartsys.cpp','src/authoredpartsys.h','src/partsysdefinition.cpp','src/partsysdefinition.h','src/speedauthoredmatrix.h','src/goldauthoredmatrix.h','src/math3d.cpp','src/math3d.h')
    pinned={p:sha((ROOT/p).read_bytes())for p in source_paths}
    with zipfile.ZipFile(archive)as z:
        member=next(n for n in z.namelist()if n.lower()==('imagery/'+profile['asset']).lower());data=z.read(member)
    asset=read_asset(data);asset['name']=profile['name']
    if asset['sha256']!=profile['sha256']:raise ValueError('Exact shipped asset changed')
    size=profile.get('width',512);prototype=profile.get('prototype',0);numeric_tolerance=profile.get('numeric_tolerance',6e-5)
    f=NativeSpeed(executable,asset,size,size);dispatch=registry(f);punctuation(f);initialization=json.loads(json.dumps(f.initialize(0)))
    native=f.state_trace(90,clamp_frames=profile.get('clamp',False))
    first=sha(json.dumps(dict(rows=native['rows'],poses=native['poses']),separators=(',',':')).encode())
    fresh=NativeSpeed(executable,asset,size,size);registry(fresh);punctuation(fresh);fresh.initialize(0)
    replay=fresh.state_trace(90,clamp_frames=profile.get('clamp',False))
    second=sha(json.dumps(dict(rows=replay['rows'],poses=replay['poses']),separators=(',',':')).encode())
    if first!=second:raise AssertionError('Fresh native full-pool/pose replay changed')
    del fresh,replay
    (output/'asset-triage.json').write_text(json.dumps([asset],indent=2)+'\n')
    (output/'native-init.json').write_text(json.dumps(initialization,indent=2)+'\n')
    (output/'retail-state-trace.json').write_text(json.dumps(native,indent=2)+'\n')
    state_compare=compare_state(output,output/'state-comparison')
    if state_compare['status']!='pass':raise AssertionError('Full independent production state/RNG differs')
    production=(ROOT/'src/3dimage.cpp').read_text();body=fn(production,'int32_t T3DAnimator::SubmitPartSys(')
    family=fn((ROOT/'src/3dimage.h').read_text(),'bool HasRetailSpeedFamilyPartSysProfile(')
    static_profile=fn((ROOT/'src/3dimage.h').read_text(),'bool HasRetailStaticParticleProfile(')
    cpp=PRELUDE.replace('SPEED_FAMILY_METHOD',family).replace('STATIC_PROFILE_METHOD',static_profile)+'\n'+body+'\n'+MAIN
    source=output/'packet_driver.cpp';source.write_text(cpp);binary=output/'packet_driver'
    command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',
        '-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(source),
        str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    q=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
    if q.returncode:raise RuntimeError('Actual SubmitPartSys compile failed; see compile.log')
    pose_fixture_sha=None;source_poses=None
    origin=native['object_zero_initial_origin']
    if independent_poses is not None:
        pose_path=independent_poses/(profile['name']+'-source-poses.json');pose_data=json.loads(pose_path.read_text())
        if pose_data['asset_sha256']!=profile['sha256']or pose_data['type_id']!=profile['id']:raise AssertionError('Independent pose identity differs')
        if any(sha((ROOT/p).read_bytes())!=h for p,h in pose_data['source_sha256'].items()):raise AssertionError('Independent poses compiled from different source')
        source_poses={(r['object'],r['frame']):r for r in pose_data['poses']};pose_fixture_sha=sha(pose_path.read_bytes());origin=source_poses[(0,0)]['trs'][:3]
        emitter_indices=[next(o['index']for o in asset['objects']if o['name']==n)for n in state_compare['definition']['objects']] if 'definition'in state_compare else None
        # Exact original initialized controller pointer list grounds emitter order.
        v=f.vm;controller=f.controller;ptr=v.u32(controller+0x28)
        emitter_indices=[f.animobjs.index(v.u32(ptr+i*4))for i in range(v.u32(controller+0x18))]
    inputs=struct.pack('<3i3f',90,len(native['poses'][0]['emitters']),prototype,*origin)+bytes.fromhex(asset['objects'][prototype]['vertex_hex'])
    for p in native['poses']:
        inputs+=struct.pack('<i',p['frame'])
        for index,e in enumerate(p['emitters']):
            if source_poses is not None:
                actual=source_poses[(emitter_indices[index],p['frame'])];trs=actual['trs'];position=trs[:3];scale=trs[6:];matrix=actual['matrix']
            else:position=e['position'];scale=e['scale'];matrix=e['matrix']
            inputs+=struct.pack('<22f',*position,*scale,*matrix)
    (output/'inputs.bin').write_bytes(inputs)
    tag=next(t['parameters']for t in asset['tags']if t['name']=='partsys')
    q=subprocess.run([str(binary),profile['id'],tag,str(output/'inputs.bin')],capture_output=True,text=True)
    (output/'run.log').write_text(q.stderr);(output/'production-packets.csv').write_text(q.stdout)
    if q.returncode or q.stderr:raise RuntimeError('Actual SubmitPartSys execution failed')
    packets={(int(p[0]),int(p[1])):p for line in q.stdout.splitlines()if(p:=[float(x)for x in line.split()])}
    v=f.vm;obj=f.animobjs[prototype];rawvertices=list(struct.iter_unpack('<8f',bytes.fromhex(asset['objects'][prototype]['vertex_hex'])))
    point=v.allocate(12);result=v.allocate(12);geometry_errors=[];max_corner=0;max_uv=0;draws={i:[]for i in range(90)}
    zero_scale_rejections=[];all_packets=0
    centre_errors=[];max_raw_centre_error=0;max_model_centre_error=0;centre_checks=0
    centre_particle=v.u32(f.controller+0x2c)
    for row in native['rows']:
        tick,slot=int(row[0]),int(row[1])
        if not row[3]:continue
        sample=row[20:];packet=packets.pop((tick,slot),None)
        all_packets+=1
        if packet is not None:
            # Independently ground the common-domain centre. Use compiled
            # raw Z as an input to the unmodified native FIX fragment, and
            # compare its output with actual native RenderSample saved above.
            # This cannot silently substitute the correct native translation
            # for a wrong compiled raw particle centre.
            raw_error=abs(packet[2]-row[7])
            v.write(centre_particle+0xc0,struct.pack('<f',packet[2]))
            v.uc.reg_write(UC_X86_REG_ESI,centre_particle)
            v.uc.reg_write(UC_X86_REG_EBX,obj)
            v.uc.reg_write(UC_X86_REG_FPCW,0x37f)
            # Use the instruction stop hook: Unicorn's previously translated
            # full RenderSample block can otherwise pass an emu_start end PC.
            v.call(0x4027e8,stop_address=0x40281f,instruction_limit=10000)
            if v.uc.reg_read(UC_X86_REG_EIP)!=0x40281f:
                raise AssertionError('Original FIX centre fragment did not finish')
            model=struct.unpack('<f',v.uc.mem_read(obj+0x18,4))[0]
            model_error=abs(model-sample[2]);centre_checks+=2
            max_raw_centre_error=max(max_raw_centre_error,raw_error)
            max_model_centre_error=max(max_model_centre_error,model_error)
            if raw_error>max(2e-5,numeric_tolerance) or model_error>max(3e-6,numeric_tolerance):
                centre_errors.append(dict(tick=tick,slot=slot,compiled_raw_z=packet[2],
                    original_raw_z=row[7],native_FIX_of_compiled_raw_z=model,
                    original_sample_MODELZ=sample[2],raw_error=raw_error,model_error=model_error))
        v.write(obj+0x10,struct.pack('<3f',*sample[:3]));v.write(obj+0x28,struct.pack('<3f',*sample[3:6]));v.write(obj+0x40,struct.pack('<3f',*[sample[10]]*3))
        admitted=v.call(0x40a420,(obj,0,0,0,0),this=f.imagery)
        if admitted!=1:
            # Both frontends now reject zero-scale geometry after render
            # sampling. Never transform the native failure's identity matrix.
            if sample[10]>9.999999747378752e-6 or packet is not None:
                raise AssertionError('Native/source zero-scale draw admission differs')
            zero_scale_rejections.append(dict(tick=tick,slot=slot,native_matrix_return=admitted,
                source_draw_count=0))
            continue
        if packet is None:raise AssertionError('Production dropped actual native geometry')
        original=[]
        for vertex in rawvertices:
            v.write(point,struct.pack('<3f',*vertex[:3]));v.call(0x43ad80,(obj+0x58,point,result))
            original.append(list(struct.unpack('<3f',v.uc.mem_read(result,12))))
        modern=[packet[10+i*5:13+i*5]for i in range(4)]
        uvs=[packet[13+i*5:15+i*5]for i in range(4)]
        # Source packet centre is raw-world Z. Its mesh-local Z is stretched
        # 1.5 exactly once. Compare in the original sampled MODELZ domain.
        modern=[[p[0],p[1],sample[2]+(p[2]-packet[2])/1.5]for p in modern]
        order=(0,1,3,2)
        error=max(abs(a-b)for index,i in enumerate(order)for a,b in zip(original[i],modern[index]))
        uv_error=max(abs(a-b)for index,i in enumerate(order)for a,b in zip(rawvertices[i][6:],uvs[index]))
        max_corner=max(max_corner,error);max_uv=max(max_uv,uv_error)
        expected_color=[(int(c)&255)>>3 for c in sample[6:9]]
        if packet[3:6]!=[2,0,1] or any(abs(packet[6+i]-expected_color[i]/31)>1e-7 for i in range(3)):
            raise AssertionError('Production own blend/depth/texture/RGB5 policy differs')
        if error>numeric_tolerance or uv_error>1e-7:geometry_errors.append(dict(tick=tick,slot=slot,corner_error=error,uv_error=uv_error))
        draws[tick].append(dict(native=original,port=modern,native_uv=[list(x[6:])for x in rawvertices],
            port_uv=uvs,color=expected_color,alpha=int(sample[9]*255)>>3))
    if packets:raise AssertionError('Production submitted extra particles')
    (output/'geometry.json').write_text(json.dumps(dict(status='pass'if not geometry_errors and not centre_errors else'fail',
        sampled_live_packets=all_packets,actual_native_draw_packets=sum(len(x)for x in draws.values()),
        native_zero_scale_rejections=zero_scale_rejections,max_corner_error=max_corner,max_uv_error=max_uv,
        independent_centre_checks=centre_checks,max_raw_centre_error=max_raw_centre_error,
        max_native_FIX_centre_error=max_model_centre_error,centre_errors=centre_errors,
        geometry_errors=geometry_errors,accepted=False),indent=2)+'\n')
    # No billboard or scaled stand-in. Decode the actual prototype texture.
    u=lambda p:struct.unpack_from('<I',data,p)[0];r=lambda p:p+u(p);bo=20+u(16);tex=r(bo+40)
    height,width=struct.unpack_from('<2I',data,tex+8);pixel=r(r(tex+108));texture=data[pixel:pixel+width*height*2]
    f.software.set_texture(width,height,texture,format='RGB565')
    # Canonical ONE/ONE RGB565 uses original carry/add tables. This bounded
    # software fixture's default setup only built its normal modulation table.
    # Execute the original table builder and publish its returned pointers.
    v.call(0x56c730,(65536,16),this=0x675e90,instruction_limit=5000000)
    carry,add=v.u32(0x675e90),v.u32(0x675e94)
    if not carry or not add:raise AssertionError('Original additive lookup table initialization failed')
    v.put_u32(0x670674,carry);v.put_u32(0x67067c,add)
    pairs=[];pixel_errors=[]
    def raster(tick,label):
        f.software.clear();v.call(0x417d60,(16,1),this=0x65a57c)
        for draw in draws[tick]:
            positions=draw[label];uv=draw[label+'_uv'];projected=f.software.project(positions,camera=(0,0,0),zdist=1925)
            if any(not(0<=p[0]<size and 0<=p[1]<size)for p in projected):raise AssertionError('Literal geometry exceeds fixed viewport; defer rather than fit')
            f.software.draw([(*p,*draw['color'],draw['alpha'],*t)for p,t in zip(projected,uv)],
                [2,3,0,1,2,0]if label=='native'else[2,0,3,1,3,0],z_enabled=True,z_write=False,instruction_limit=5000000)
        return v.surface_bytes('screen'),v.surface_bytes('depth')
    for tick in profile.get('samples',(5,15,29,60,89)):
        prior=None
        for repeat in range(2):
            images=[];depths=[]
            for label in('native','port'):
                image,depth=raster(tick,label);images.append(image);depths.append(depth)
                save_rgb565_png(output/f'{label}-tick{tick:03d}.png',image,size,size)
            hashes=[sha(p)for p in images+depths]
            if prior is not None and hashes!=prior:raise AssertionError('Full RGB565/depth warm replay differs')
            prior=hashes
        differences=sum(a!=b for a,b in zip(struct.unpack('<'+str(size*size)+'H',images[0]),struct.unpack('<'+str(size*size)+'H',images[1])))
        visible=sum(p!=0 for p in struct.unpack('<'+str(size*size)+'H',images[0]))
        if differences or depths[0]!=depths[1] or not visible:pixel_errors.append(dict(tick=tick,differing_pixels=differences,visible=visible))
        pairs.append(dict(tick=tick,differing_rgb565_pixels=differences,visible_native_pixels=visible,depth_equal=depths[0]==depths[1],image_sha256=[sha(p)for p in images],depth_sha256=[sha(p)for p in depths],warm_replays=2))
    if pinned!={p:sha((ROOT/p).read_bytes())for p in source_paths}:raise AssertionError('Production source changed during comparison')
    report=dict(native_dispatch=dispatch,fresh_native_state_replays=2,native_row_pose_sha256=[first,second],source_sha256=pinned,full_state_comparisons=state_compare['comparisons'],full_state_errors=state_compare['error_count'],raster_replays_per_case=2,name=profile['name'],type_id=profile['id'],status='pass'if not geometry_errors and not pixel_errors and not centre_errors else'fail',geometry_errors=geometry_errors,
        pixel_errors=pixel_errors,sampled_live_packets=all_packets,actual_native_draw_packets=sum(len(x)for x in draws.values()),
        native_zero_scale_rejections=zero_scale_rejections,max_corner_error=max_corner,max_uv_error=max_uv,
        independent_centre_checks=centre_checks,max_raw_centre_error=max_raw_centre_error,
        max_native_FIX_centre_error=max_model_centre_error,centre_errors=centre_errors,
        centre_oracle='Compiled rawZ vsactualnative particle rawZ; original4027e8..40281f x87FIX/store '
            'executes withcompiledrawZ input and is comparedtoseparatelyretained nativeRenderSample MODELZ. No PythonFIX formula.',
        independent_source_pose_fixture_sha256=pose_fixture_sha,pairs=pairs,viewport=[size,size],geometry_absolute_tolerance=numeric_tolerance,compiled_SubmitPartSys_sha256=sha(body.encode()),asset_sha256=asset['sha256'],
        generated_source_sha256=sha(cpp.encode()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),
        accepted=False,full_game_integration=False,metal_backend_compared=False,
        scope='Actual compiled parser/State/SubmitPartSys against native whole parser/Initialize/Pulse/liveSample/#CalcObjectMatrix/x87transform; '
            'declared emitter poses supplied by native fixture or separately compiled source pose artifact. Full quad positions/UV/ownRGB5/texture16/depth checked; '
            'raw-world portZ compared via declared MODELZ bridge using actual native sample centre. Shared original software '
            'projector/raster used at declared fixed viewport white/color fixture; no actual map/Metal projector, base material lighting/caster/full acceptance.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path)
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True)
    p.add_argument('--independent-poses',type=Path);p.add_argument('--y-family',action='store_true');p.add_argument('--profiles',nargs='+');a=p.parse_args();selected=Y_PROFILES if a.y_family else PROFILES
    if a.profiles is None:a.profiles=[p['name']for p in selected]
    cases=[]
    for profile in selected:
        if profile['name']not in a.profiles:continue
        result=run(a.executable,a.archive,a.output/profile['name'],profile,a.independent_poses);cases.append(result)
        print(json.dumps({k:result[k]for k in('name','status','full_state_comparisons','sampled_live_packets','actual_native_draw_packets','max_corner_error','max_uv_error','pixel_errors')},indent=2),flush=True)
    (a.output/'manifest.json').write_text(json.dumps(dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=[dict(name=c['name'],status=c['status'],manifest=str(a.output/c['name']/'manifest.json'))for c in cases],rendered_types=len(cases),accepted=False),indent=2)+'\n')
    if any(c['status']!='pass'for c in cases):raise SystemExit(1)
