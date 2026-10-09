#!/usr/bin/env python3
"""Retail static keys/matrix/mesh versus compiled port extraction/submission."""
import argparse
import hashlib
import json
from pathlib import Path
import statistics
import struct
import subprocess
import time
import zipfile
from software_probe import SoftwareFixture,save_rgb565_png,RETAIL_SHA
from fizzle_probe import span,sha

ROOT=Path(__file__).resolve().parents[2]
WIDTH=256;HEIGHT=256;PIXELS=WIDTH*HEIGHT
FRAMES=(0,50,99)
OWNERS=((0,0,0),(16,-8,4))


def parse_asset(data,profile):
    if sha(data)!=profile['asset_sha256']:raise ValueError('Unexpected static asset SHA')
    u=lambda address:struct.unpack_from('<I',data,address)[0]
    relative=lambda address:address+u(address)
    body=20+u(16)
    if u(body)!=0xdc or u(body+4)!=3 or u(body+44)!=1 or u(body+36)!=1 or u(body+52):
        raise ValueError('Unsupported static body, object/texture count or tags')
    vertices=relative(relative(relative(body+16)));faces=relative(body+24)
    obj=relative(body+48);state=relative(obj+44);keys=relative(state+8)
    parent,count=struct.unpack_from('<2i',data,state)
    if parent!=-1 or count!=6:raise ValueError('Unsupported static hierarchy/key count')
    words=struct.unpack_from('<6I',data,keys)
    if [word&255 for word in words]!=[8,12,16,20,24,28]:raise ValueError('Static scalar TRS key layout changed')
    texture_runs=relative(obj+40);runs=struct.unpack_from('<4H',data,texture_runs)
    if runs!=(0,0,0,profile['faces']):raise ValueError('Unexpected texture face run')
    texture=relative(body+40);frame_offsets=relative(texture+108);pixels=relative(frame_offsets)
    height,width=struct.unpack_from('<2I',data,texture+8)
    pixel_format=struct.unpack_from('<8I',data,texture+72)
    if pixel_format[3]!=16:raise ValueError('Expected original16bit texture')
    format='ARGB4444' if pixel_format[7] else 'RGB565'
    vertex_bytes=data[vertices:vertices+profile['vertices']*32]
    face_bytes=data[faces:faces+profile['faces']*6]
    if min(struct.unpack('<'+'H'*(profile['faces']*3),face_bytes))<0 or max(struct.unpack('<'+'H'*(profile['faces']*3),face_bytes))>=profile['vertices']:
        raise ValueError('Original local index outside vertex array')
    return dict(body=body,vertices_offset=vertices,faces_offset=faces,keys_offset=keys,
        texture_offset=pixels,width=width,height=height,format=format,pixel_format=list(pixel_format),
        vertices=[list(v)for v in struct.iter_unpack('<8f',vertex_bytes)],
        indices=list(struct.unpack('<'+'H'*(profile['faces']*3),face_bytes)),keys=list(words),
        vertex_hex=vertex_bytes.hex(),face_hex=face_bytes.hex(),texture=data[pixels:pixels+width*height*2])


def production_spans():
    image=(ROOT/'src/3dimage.cpp').read_text();mesh=(ROOT/'src/meshextract.cpp').read_text();effects=(ROOT/'src/effect.cpp').read_text()
    return dict(key_decoder=span(image,'#define POSCHANGED 1','constexpr int32_t kLegacyTransitionBlendFrames'),
        interpolation_helpers=span(image,'static void InterpolatePoints','bool T3DImagery::GetAniKey('),
        key_getter=span(image,'bool T3DImagery::GetAniKey(','bool T3DImagery::CalcObjectMatrix('),
        make_matrix=span(image,'static void MakeMatrix(','bool T3DImagery::IsHidden('),
        object_matrix=span(image,'bool T3DImagery::CalcObjectMatrix(','bool T3DImagery::CalcObjectMatrixCopy('),
        mesh_extract=span(mesh,'bool ExtractSubMesh(','// Mirror of the static MakeMatrix'),
        static_pose=span(mesh,'void BuildStaticObjectMatrix(','void BuildAnimatedObjectMatrix('),
        static_submit=span(effects,'void TAuthoredStaticMeshEffect::SubmitWorldMeshForTest_BESPOKE(','// ----- W3-G #3'),
        math=(ROOT/'src/math3d.cpp').read_text(),math_header=(ROOT/'src/math3d.h').read_text())


def build_port(output,metadata,frames=FRAMES,texture_frames=False):
    pieces=production_spans()
    prelude=r'''
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#include "3dimagebody.h"
#undef min
#undef max
enum class EFxDebugMode:uint8_t {Normal};
struct SMeshVertex {float pos[3]{},normal[3]{},uv[2]{};};
struct StubTransform {void SetLocalPos(const hmm_vec3&){};void SetLocalRotEuler(const hmm_vec3&){};void SetLocalScl(const hmm_vec3&){};};
struct S3DAnimObj {int flags=0,objnum=0,animtrack=0;hmm_vec3 pos{},rot{},scl{1,1,1};hmm_mat4 matrix{};
 S3DAnimObj*parent=nullptr;StubTransform transform;};
struct S3DObj {int* numanikeys;void** anikeys;};
struct StubHeader {int numstates=1;struct State {int aniflags=0,frames=100;}states[1];};
bool Interpolate=false;
int stricmp(const char*,const char*){throw std::runtime_error("Unexpected cross-state tag interpolation");}
struct T3DImagery {
 std::vector<S3DVertex> vertices;std::vector<S3DFace> faces;std::vector<SAniKey32> keys;
 S3DObj objects[1];int key_count=0,frame_count=100;void* key_pointer=nullptr;int flags=0xdc;bool meshinitialized=true;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,
 retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false;
 int prevstate=-1,prevframe=0;StubHeader header;
 int NumObjects()const{return 1;}int NumStates()const{return 1;}int GetAniLength(int)const{return frame_count;}
 int NumTextures()const{return 1;}int NumObjVerts(int)const{return int(vertices.size());}int NumObjFaces(int)const{return int(faces.size());}
 void GetObjVerts(int,S3DVertex*out,int=0,int=0,ERender3DVertex=ERender3DVertex::Vertex){std::memcpy(out,vertices.data(),vertices.size()*32);}
 void GetObjFaces(int,S3DFace*out,int*starts,int*counts){if(out)std::memcpy(out,faces.data(),faces.size()*6);if(starts)starts[1]=0;if(counts)counts[1]=int(faces.size());}
 void* GetBody(){throw std::runtime_error("Unexpected body load");}void InitializeMesh(S3DImageryBody*){throw std::runtime_error("Unexpected mesh load");}
 StubHeader*GetHeader(){return &header;}char*FindTag(char*,int){return nullptr;}
 int GetObjectParent(int,int)const{return -1;}void SetPrevState(int s,int f){prevstate=s;prevframe=f;}
 bool GetUninterpolatedAniKey(int,int,int,hmm_vec3&,hmm_vec3&,hmm_vec3&);
 bool GetAniKey(int,int,int,hmm_vec3&,hmm_vec3&,hmm_vec3&);
 bool CalcObjectMatrix(S3DAnimObj*,int,int,hmm_mat4*,bool);
 bool ScrollTexOffset(int,int,int,int64_t,float*)const{throw std::runtime_error("Unexpected scroll in static fixture");}
};
#define OBJ3D_ROTMASK 0x30
#define OBJ3D_POSMASK 0x0c
#define OBJ3D_SCLMASK 0xc0
#define OBJ3D_ADDTOANI 0x200
#define OBJ3D_ANIMTRACK 0x02
#define OBJ3D_POS1 0x04
#define OBJ3D_POS2 0x08
#define OBJ3D_POS3 0x0c
#define OBJ3D_ROT1 0x10
#define OBJ3D_ROT2 0x20
#define OBJ3D_ROT3 0x30
#define OBJ3D_SCL1 0x40
#define OBJ3D_SCL2 0x80
#define OBJ3D_SCL3 0xc0
struct SMeshSubmit {int mesh=1,retail_lighting=0;float world[16]{},tint[4]{},uv_offset[2]{};};
struct TRenderer {SMeshSubmit last;void SubmitMesh(const SMeshSubmit& s){last=s;}};
TRenderer renderer;TRenderer*Renderer=&renderer;
struct TTime{static int64_t LegacyFrameCount(){return 0;}};
struct World {hmm_mat4 m;World(){MtxClear(&m);}const hmm_mat4& Matrix()const{return m;}};
struct TAuthoredStaticMeshEffect {World root;struct SStaticPart {int mesh=1,retail_lighting=0,object_index=0;float local_matrix[16]{};std::vector<int> frame_meshes;};
 std::vector<SStaticPart> parts_;T3DImagery*scroll_imagery_=nullptr;int frame_{0};int GetFrame()const{return frame_;}const World&Transform()const{return root;}
 void SubmitWorldMeshForTest_BESPOKE(EFxDebugMode);};
'''
    trailer=r'''
int main(int argc,char**argv){
 std::ifstream input(argv[1],std::ios::binary);if(!input)return 2;
 int texture_count=0; int nv,nf,nk,frames;input.read((char*)&nv,4);input.read((char*)&nf,4);input.read((char*)&nk,4);input.read((char*)&frames,4);TEXTURE_INPUT
 T3DImagery img;img.frame_count=frames;img.vertices.resize(nv);img.faces.resize(nf);img.keys.resize(nk);
 input.read((char*)img.vertices.data(),nv*32);input.read((char*)img.faces.data(),nf*6);input.read((char*)img.keys.data(),nk*4);
 img.key_count=nk;img.key_pointer=img.keys.data();img.objects[0]={&img.key_count,&img.key_pointer};
 std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(&img,0,1,vertices,indices))return 3;
 std::printf("I");for(auto i:indices)std::printf(" %u",unsigned(i));std::puts("");
 for(int frame:{FRAMES})for(int moved:{0,1}){
  TAuthoredStaticMeshEffect effect;effect.parts_.resize(1);TEXTURE_SETUP BuildStaticObjectMatrix(&img,0,0,frame,effect.parts_[0].local_matrix);
  if(moved){effect.root.m.Elements[3][0]=16;effect.root.m.Elements[3][1]=-8;effect.root.m.Elements[3][2]=4;}
  effect.SubmitWorldMeshForTest_BESPOKE(EFxDebugMode::Normal);
  std::printf("F %d %d %d\n",frame,moved,renderer.last.mesh==1?1000:renderer.last.mesh);
  std::printf("M %d %d",frame,moved);for(auto v:renderer.last.world)std::printf(" %.9g",v);std::puts("");
  for(size_t i=0;i<vertices.size();++i){const auto&v=vertices[i];const auto*m=renderer.last.world;
   float x=m[0]*v.pos[0]+m[1]*v.pos[1]+m[2]*v.pos[2]+m[3];
   float y=m[4]*v.pos[0]+m[5]*v.pos[1]+m[6]*v.pos[2]+m[7];
   float z=m[8]*v.pos[0]+m[9]*v.pos[1]+m[10]*v.pos[2]+m[11];
   std::printf("V %d %d %zu %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",frame,moved,i,x,y,z,v.normal[0],v.normal[1],v.normal[2],v.uv[0],v.uv[1]);}
 }
}
'''.replace('FRAMES',','.join(str(frame)for frame in frames)).replace('TEXTURE_INPUT','input.read((char*)&texture_count,4);'if texture_frames else'').replace('TEXTURE_SETUP','effect.frame_=frame;if(texture_count>1)for(int f=0;f<texture_count;++f)effect.parts_[0].frame_meshes.push_back(1000+f);'if texture_frames else'')
    generated=output/'static-port-components.cpp'
    generated.write_text(prelude+'\n'+pieces['make_matrix']+'\n'+pieces['key_decoder']+'\n'+
        'constexpr int32_t kLegacyTransitionBlendFrames=5;\n'+pieces['interpolation_helpers']+'\n'+pieces['key_getter']+'\n'+
        pieces['object_matrix']+'\n'+pieces['mesh_extract']+'\n'+pieces['static_pose']+'\n'+pieces['static_submit']+'\n'+trailer)
    binary=output/'static-port-components';command=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),
        '-iquote',str(ROOT/'src'),str(generated),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
    result=subprocess.run(command,capture_output=True,text=True);(output/'port-compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Static production-body compilation failed; see port-compile.log')
    traces={}
    for name,m in metadata.items():
        path=output/(name+'.mesh-input');path.write_bytes(struct.pack('<4I',len(m['vertices']),len(m['indices'])//3,len(m['keys']),m.get('frames',100))+
            (struct.pack('<I',m.get('texture_frame_count',1))if texture_frames else b'')+bytes.fromhex(m['vertex_hex'])+bytes.fromhex(m['face_hex'])+struct.pack('<'+'I'*len(m['keys']),*m['keys']))
        trace=subprocess.check_output([str(binary),str(path)],text=True);(output/(name+'-port-trace.txt')).write_text(trace)
        cases={};indices=[];selected={}
        for line in trace.splitlines():
            p=line.split()
            if p[0]=='I':indices=[int(x)for x in p[1:]]
            elif p[0]=='F':selected[(int(p[1]),int(p[2]))]=int(p[3])
            elif p[0]=='M':cases[(int(p[1]),int(p[2]))]=dict(matrix=[float(x)for x in p[3:]],vertices=[],texture_handle=selected[(int(p[1]),int(p[2]))])
            else:cases[(int(p[1]),int(p[2]))]['vertices'].append([float(x)for x in p[4:]])
        traces[name]=dict(indices=indices,cases=cases)
    return traces,dict(command=command,generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        compiled_production_bodies=['GetUninterpolatedAniKey','GetAniKey','CalcObjectMatrix','BuildStaticObjectMatrix',
                                   'ExtractSubMeshTextureSlot','TAuthoredStaticMeshEffect::SubmitWorldMeshForTest_BESPOKE','src/math3d.cpp'],
        boundaries=['decoded exact asset records supplied by verified relative-offset layout','identity/translated owner matrix',
                    'renderer captures actual generic SubmitMesh payload','cross-state interpolation disabled for one constant track'])


class StaticFixture:
    def __init__(self,executable,width=WIDTH,height=HEIGHT,instruction_limit=1000000):
        self.width=width;self.height=height;self.instruction_limit=instruction_limit
        self.software=SoftwareFixture(executable,width,height);self.vm=self.software.vm
        self.context=self.vm.allocate(0x200);vtable=self.vm.allocate(0x94);objects=self.vm.allocate(4);object=self.vm.allocate(0x48)
        self.nkeys=self.vm.allocate(4);self.key_pointer=self.vm.allocate(4);self.keys=self.vm.allocate(4096)
        self.vm.put_u32(object+0x40,self.nkeys);self.vm.put_u32(object+0x44,self.key_pointer);self.vm.put_u32(objects,object)
        self.vm.put_u32(self.context,vtable);self.vm.put_u32(self.context+0x0c,1)
        self.vm.put_u32(self.context+0x1c,0xdc);self.vm.put_u32(self.context+0x74,objects);self.vm.put_u32(0x5d7a14,0)
        states=self.vm.allocate(16);self.vm.write(states,b'\xb8\x01\x00\x00\x00\xc3')
        self.frame_count_value=self.vm.allocate(4);self.vm.put_u32(self.frame_count_value,100)
        self.frame_count_stub=self.vm.allocate(16);self.vm.write(self.frame_count_stub,b'\xa1'+struct.pack('<I',self.frame_count_value)+b'\xc2\x04\x00')
        self.vm.put_u32(vtable+0x3c,states);self.vm.put_u32(vtable+0x90,self.frame_count_stub)
        self.vm.put_u32(self.nkeys,6);self.vm.put_u32(self.key_pointer,self.keys)
        self.animobj=self.vm.allocate(0x100);self.root=self.vm.allocate(64);self.combined=self.vm.allocate(64)
        self.point=self.vm.allocate(12);self.result=self.vm.allocate(12)

    def setup(self,metadata,checkpoint=True):
        self.metadata=metadata
        if len(metadata['keys'])*4>4096:raise ValueError('Fixture key capacity exceeded')
        self.vm.put_u32(self.nkeys,len(metadata['keys']))
        self.vm.write(self.keys,struct.pack('<'+'I'*len(metadata['keys']),*metadata['keys']))
        self.vm.put_u32(self.frame_count_value,metadata.get('frames',100))
        self.software.set_texture(metadata['width'],metadata['height'],metadata['texture'],format=metadata['format'])
        if checkpoint:self.software.clear();self.software.checkpoint()

    def original(self,frame,moved):
        self.vm.put_u32(self.animobj,0)
        result=self.vm.call(0x40a420,(self.animobj,0,frame,0,0),this=self.context)
        if result!=1:raise AssertionError('Original constant pose rejected')
        matrix=struct.unpack('<16f',self.vm.uc.mem_read(self.animobj+0x58,64))
        self.vm.call(0x43a9f0,(self.root,));self.vm.write(self.root+48,struct.pack('<3f',*OWNERS[moved]))
        self.vm.call(0x43aa90,(self.combined,self.animobj+0x58,self.root));points=[]
        for record in self.metadata['vertices']:
            self.vm.write(self.point,struct.pack('<3f',*record[:3]));self.vm.call(0x43ad80,(self.combined,self.point,self.result))
            points.append(list(struct.unpack('<3f',self.vm.uc.mem_read(self.result,12))))
        return list(matrix),points

    def pixels(self,points,uvs,indices):
        self.software.clear();projected=self.software.project(points,camera=(0,0,0),zdist=1925)
        if any(not(0<=p[0]<self.width and 0<=p[1]<self.height)for p in projected):raise AssertionError('Static fixture clips geometry')
        vertices=[(*p,31,31,31,31,*uv)for p,uv in zip(projected,uvs)]
        return self.software.draw(vertices,indices,z_enabled=True,z_write=True,instruction_limit=self.instruction_limit)


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('executable',type=Path)
    parser.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--profiles',type=Path,default=Path(__file__).with_name('static_profiles.json'))
    parser.add_argument('--instruction-limit',type=int,default=1000000)
    parser.add_argument('--width',type=int,default=WIDTH);parser.add_argument('--height',type=int,default=HEIGHT)
    parser.add_argument('--repeat',type=int,default=2);args=parser.parse_args()
    if args.repeat<2:parser.error('At least two replays required')
    args.output.mkdir(parents=True,exist_ok=True);profiles=json.loads(args.profiles.read_text())['profiles']
    before={k:sha(v.encode())for k,v in production_spans().items()};metadata={}
    with zipfile.ZipFile(args.archive)as archive:
        names={n.lower():n for n in archive.namelist()}
        for p in profiles:
            member='Imagery/'+p['asset'].replace('\\','/');metadata[p['name']]=parse_asset(archive.read(names[member.lower()]),p)
    port,compiled=build_port(args.output,metadata);fixture=StaticFixture(args.executable,args.width,args.height,args.instruction_limit);pixel_count=args.width*args.height
    cases=[];failures=[];timings=[]
    for profile in profiles:
        name=profile['name'];m=metadata[name];fixture.setup(m);static_hash={}
        if port[name]['indices']!=m['indices']:raise AssertionError('Production extraction changed original indices')
        uvs=[v[6:8]for v in m['vertices']]
        for frame in FRAMES:
            for moved in (0,1):
                first=None
                for repeat in range(args.repeat):
                    start=time.perf_counter();fixture.software.restore();matrix,points=fixture.original(frame,moved)
                    native,z=fixture.pixels(points,uvs,m['indices']);packet=port[name]['cases'][(frame,moved)]
                    port_points=[v[:3]for v in packet['vertices']];port_uvs=[v[6:8]for v in packet['vertices']]
                    modern,mz=fixture.pixels(port_points,port_uvs,port[name]['indices'])
                    timings.append((time.perf_counter()-start)*1000);pair=(sha(native),sha(modern),sha(z),sha(mz))
                    if first is None:first=pair
                    elif first!=pair:raise AssertionError('Static image/depth warm replay changed')
                    if moved not in static_hash:static_hash[moved]=pair
                    elif static_hash[moved]!=pair:raise AssertionError('Constant static track produced changing frames')
                    if repeat:continue
                    position_error=max(abs(a-b)for x,y in zip(points,port_points)for a,b in zip(x,y))
                    uv_error=max(abs(a-b)for x,y in zip(uvs,port_uvs)for a,b in zip(x,y))
                    diff=sum(a!=b for a,b in zip(struct.unpack('<'+str(pixel_count)+'H',native),struct.unpack('<'+str(pixel_count)+'H',modern)))
                    visible=sum(p!=0 for p in struct.unpack('<'+str(pixel_count)+'H',native))
                    if position_error>3e-5 or uv_error>1e-7 or diff or not visible:
                        failures.append(dict(name=name,frame=frame,moved=moved,max_position_error=position_error,max_uv_error=uv_error,differing_rgb565_pixels=diff,nonzero_pixels=visible))
                    save_rgb565_png(args.output/f'{name}-retail-F{frame:02d}-P{moved}.png',native,args.width,args.height)
                    save_rgb565_png(args.output/f'{name}-port-F{frame:02d}-P{moved}.png',modern,args.width,args.height)
                    cases.append(dict(name=name,type_id=profile['id'],asset=profile['asset'],asset_sha256=profile['asset_sha256'],frame=frame,owner_position=list(OWNERS[moved]),
                        original_local_matrix=matrix,production_submit_matrix=packet['matrix'],original_vertices=points,port_vertices=port_points,
                        original_uvs=uvs,port_uvs=port_uvs,indices=m['indices'],max_position_error=position_error,max_uv_error=uv_error,
                        differing_rgb565_pixels=diff,nonzero_retail_pixels=visible,image_hashes=pair,
                        vertex_offset=hex(m['vertices_offset']),faces_offset=hex(m['faces_offset']),keys_offset=hex(m['keys_offset']),texture_offset=hex(m['texture_offset']),texture_format=m['format']))
        if static_hash[0][0]==static_hash[1][0]:raise AssertionError('Selected owner translation did not move visible pixels')
    after={k:sha(v.encode())for k,v in production_spans().items()}
    if before!=after:raise AssertionError('Production static mesh source spans changed during comparison')
    report=dict(status='pass'if not failures else 'fail',failures=failures,retail_sha256=RETAIL_SHA,
        source_span_sha256=after,probe_sha256=sha(Path(__file__).read_bytes()),profiles_sha256=sha(args.profiles.read_bytes()),port_component=compiled,
        raster_instruction_limit=args.instruction_limit,profile_count=len(profiles),case_count=len(cases),replays_per_case=args.repeat,frames=list(FRAMES),owner_positions=[list(x)for x in OWNERS],
        median_warm_pair_ms=statistics.median(timings),p95_warm_pair_ms=sorted(timings)[min(len(timings)-1,int(len(timings)*.95))],
        original_functions=['GetAniKey0x409950','uninterpolated key decoder0x409430','CalcObjectMatrix0x40a420','matrix multiply0x43aa90','point transform0x43ad80','software raster0x56d960'],
        camera=dict(position=[0,0,0],zdist=1925,viewport=[args.width,args.height],lighting='explicitwhite31pervertex',cull_mode='NONE',depth='test/write'),
        scope='Selected single-object state0/texture-frame0 asset identities, rawgeometry/localindices/UVs and originaldecodedconstantpose vscompiledproductionkeydecode/matrix/mesh-extraction/genericSubmitMesh. Frames0/50/99 muststaystatic for geometry; texture progression beyond frame0 is not asserted; selectedownertranslation mustmovevisiblepixels. Sharedoriginalsoftwareprojection/raster isolates frontendmesh behavior. Assetloader/headermetadata andrenderer/ownerinterfaces are explicit fixtures; realmapfloor/light/cullingpolicy andmodernGPU remainseparate.',
        guest_os_boots=0,dosbox_used=False,pixel_rendering_intercepted=False,full_game_integration=False,metal_backend_compared=False,cases=cases)
    (args.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items()if k!='cases'},indent=2))
    if failures:raise SystemExit(1)


if __name__=='__main__':main()
