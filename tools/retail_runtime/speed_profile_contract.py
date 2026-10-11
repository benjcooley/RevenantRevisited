#!/usr/bin/env python3
"""Strict shipped Speed admission and integer keys vs original retail decoder.

Actual production profile/decoder bodies execute against immutable decoded
asset records. Mutation cases use independent copies; no engine file is edited.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import zipfile

from speed_controller_preflight import NativeSpeed, read_asset, ROOT, SPEED_SHA
from software_probe import RETAIL_SHA


def sha(data):
    return hashlib.sha256(data).hexdigest()


def function(source, signature):
    start = source.index(signature)
    opening = source.index('{', start)
    depth, end = 1, opening+1
    while depth:
        depth += (source[end]=='{')-(source[end]=='}')
        end += 1
    return source[start:end]


PRELUDE = r'''
#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
struct hmm_vec3 {float X=0,Y=0,Z=0;};
struct SAniKey {int x=0,y=0,z=0,rx=0,ry=0,rz=0;};
#define ak_x(a) ((a).x-0x800)
#define ak_y(a) ((a).y-0x800)
#define ak_z(a) ((a).z-0x800)
#define ak_rx(a) ((a).rx-0x100)
#define ak_ry(a) ((a).ry-0x100)
#define ak_rz(a) ((a).rz-0x100)
#define POSCHANGED 1
#define ROTCHANGED 2
#define SCLCHANGED 4
struct Pixel{uint32_t dwRGBBitCount=0,dwRBitMask=0,dwGBitMask=0,dwBBitMask=0,dwRGBAlphaBitMask=0;};
struct Desc{uint32_t width=0,height=0;Pixel pixelFormat;};
struct S3DTex{Desc desc;int numframes=0;};
struct S3DObj{char name[32]={};int*parent=nullptr;int numverts=0,numfaces=0,material=0;
 int*numanikeys=nullptr;void**anikeys=nullptr;int*numtexfaces=nullptr;};
struct S3DTag{int state=0,frame=0;const char*name=nullptr,*str=nullptr;};
struct S3DImageryBody{};
'''

CLASS = r'''
struct S3DMat{S3DMaterial matdesc{};int texture=-1;};
class T3DImagery{public:
 bool meshinitialized=true,null_filename=false;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,
 retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,
 immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;
 uint32_t static_particles_profile=0;
 int version=0,flags=0,numverts=0,numfaces=0,nstates=0,frames=0,aflags=0,nobj=0,nmat=0,ntex=0,ntags=0;
 std::string path="Imagery/Magic/Speed.I3D";
 std::array<S3DObj,3>objects;std::array<S3DTex,2>textures;std::array<S3DMat,3>materials;
 std::array<S3DTag,2>tags;std::array<std::string,2>tag_names,tag_values;
 std::array<int,3>parent{},keycount{};std::array<void*,3>keyptr{};
 std::array<std::array<int,3>,3>facecounts{};
 std::array<std::vector<SAniKey32>,3>keys;
 std::array<std::vector<S3DVertex>,3>verts;std::array<std::vector<S3DFace>,3>faces;
 char*GetResFilename(){return null_filename?nullptr:path.data();}
 int NumStates(){return nstates;}int GetAniLength(int){return frames;}int GetAniFlags(int){return aflags;}
 int NumObjects(){return nobj;}int NumMaterials(){return nmat;}int NumTextures(){return ntex;}
 int NumTags(){return ntags;}S3DTag*GetTag(int i){return &tags.at(i);}
 void GetObjVerts(int i,S3DVertex*out,int=0,int=0){memcpy(out,verts.at(i).data(),verts.at(i).size()*sizeof(S3DVertex));}
 void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*sizeof(S3DFace));}
 void Select(){speed_partsys_profile=ValidateRetailSpeedPartSysProfile();}
 void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}
 bool ValidateRetailSpeedPartSysProfile();
 bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&);
};
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t v;memcpy(&v,b.data()+p,4);return v;}
uint16_t H(const std::vector<unsigned char>&b,size_t p){assert(p+2<=b.size());uint16_t v;memcpy(&v,b.data()+p,2);return v;}
size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
void Load(T3DImagery&im,const std::vector<unsigned char>&b){
 const size_t body=20+U(b,16),objs=P(b,body+48),tex=P(b,body+40),mat=P(b,body+32),tags=P(b,body+56);
 const size_t vertices=P(b,P(b,P(b,body+16))),indices=P(b,body+24);
 im.version=U(b,body+4);im.flags=U(b,body);im.numverts=U(b,body+12);im.numfaces=U(b,body+20);
 im.nobj=U(b,body+44);im.nmat=U(b,body+28);im.ntex=U(b,body+36);im.ntags=U(b,body+52);
 im.nstates=U(b,24);im.frames=H(b,70);im.aflags=H(b,68);
 for(int i=0;i<3;++i){const size_t o=objs+i*48,st=P(b,o+44),raw=P(b,st+8),fg=P(b,o+40);
  auto&obj=im.objects[i];memcpy(obj.name,b.data()+o,32);im.parent[i]=int32_t(U(b,st));im.keycount[i]=U(b,st+4);
  im.keys[i].resize(im.keycount[i]);memcpy(im.keys[i].data(),b.data()+raw,4*im.keycount[i]);im.keyptr[i]=im.keys[i].data();
  obj.parent=&im.parent[i];obj.numanikeys=&im.keycount[i];obj.anikeys=&im.keyptr[i];
  obj.numverts=H(b,o+36);obj.material=H(b,o+32);obj.numfaces=0;
  for(int t=0;t<3;++t){im.facecounts[i][t]=H(b,fg+t*4+2);obj.numfaces+=im.facecounts[i][t];
   for(int f=0;f<im.facecounts[i][t];++f){S3DFace face;memcpy(&face,b.data()+indices+6*(H(b,fg+t*4)+f),6);im.faces[i].push_back(face);}}
  obj.numtexfaces=im.facecounts[i].data();im.verts[i].resize(obj.numverts);
  if(obj.numverts)memcpy(im.verts[i].data(),b.data()+vertices+32*H(b,o+34),32*obj.numverts);
 }
 for(int i=0;i<2;++i){const size_t p=tex+i*120;auto&t=im.textures[i];t.numframes=U(b,p+116);
  t.desc.width=U(b,p+12);t.desc.height=U(b,p+8);t.desc.pixelFormat={U(b,p+84),U(b,p+88),U(b,p+92),U(b,p+96),U(b,p+100)};}
 for(int i=0;i<3;++i){memcpy(&im.materials[i].matdesc,b.data()+mat+i*80,80);
  im.materials[i].texture=int32_t(U(b,mat+i*80+72));}
 for(int i=0;i<2;++i){const size_t p=tags+i*16;im.tags[i].state=int32_t(U(b,p));im.tags[i].frame=int32_t(U(b,p+4));
  im.tag_names[i]=reinterpret_cast<const char*>(b.data()+P(b,p+8));im.tag_values[i]=reinterpret_cast<const char*>(b.data()+P(b,p+12));
  im.tags[i].name=im.tag_names[i].c_str();im.tags[i].str=im.tag_values[i].c_str();}
}
'''

MAIN = r'''
int main(int argc,char**argv){assert(argc==2);static_assert(sizeof(SAniKey32)==4);static_assert(sizeof(S3DVertex)==32);
 static_assert(sizeof(S3DFace)==6);static_assert(sizeof(S3DMaterial)==80);
 std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});
 T3DImagery im;Load(im,b);im.Select();assert(im.speed_partsys_profile);puts("C accept literal");
 auto reject=[&](const char*category,int index,auto mutate){T3DImagery bad;Load(bad,b);mutate(bad);
  bad.Select();assert(!bad.speed_partsys_profile);printf("C reject %s %d\n",category,index);};
 int n=0;
 reject("path",n++,[](auto&v){v.null_filename=true;});reject("path",n++,[](auto&v){v.path="";});
 for(const char*p:{"notmagic/speed.i3d","magic/speed.i3d.bak","magic/quicksilver.i3d","magic/unverified.i3d"})reject("path",n++,[&](auto&v){v.path=p;});
 for(const char*p:{"magic\\Speed.i3d","MAGIC/SPEED.I3D","/assets/Imagery/Magic/Speed.i3d"}){
  T3DImagery good;Load(good,b);good.path=p;good.Select();assert(good.speed_partsys_profile);puts("C accept path");}
 reject("header",0,[](auto&v){v.version=2;});reject("header",1,[](auto&v){v.flags^=1;});
 reject("header",2,[](auto&v){v.nstates=2;});reject("header",3,[](auto&v){v.frames=29;});
 reject("header",4,[](auto&v){v.aflags^=1;});reject("header",5,[](auto&v){v.nobj=2;});
 reject("header",6,[](auto&v){v.nmat=2;});reject("header",7,[](auto&v){v.ntex=1;});
 reject("header",8,[](auto&v){v.ntags=1;});reject("header",9,[](auto&v){--v.numverts;});reject("header",10,[](auto&v){--v.numfaces;});
 for(int i=0;i<3;++i){
  for(int j=0;j<int(im.keys[i].size());++j)reject("key_word",i*100+j,[&](auto&v){reinterpret_cast<uint32_t*>(v.keys[i].data())[j]^=0x100;});
  reject("object",i*10,[&](auto&v){--v.keycount[i];});reject("object",i*10+1,[&](auto&v){v.keyptr[i]=nullptr;});
  reject("object",i*10+2,[&](auto&v){v.parent[i]=0;});reject("object",i*10+3,[&](auto&v){v.objects[i].name[0]='?';});
  reject("object",i*10+4,[&](auto&v){v.objects[i].material=99;});reject("object",i*10+5,[&](auto&v){++v.objects[i].numverts;});
  reject("object",i*10+6,[&](auto&v){++v.objects[i].numfaces;});
  for(int j=0;j<3;++j)reject("texture_face_bin",i*3+j,[&](auto&v){++v.facecounts[i][j];});
  for(int j=0;j<17;++j)reject("material_float",i*17+j,[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(&v.materials[i].matdesc.diffuse)+j*4;float x;memcpy(&x,p,4);x+=.03125f;memcpy(p,&x,4);});
  reject("material_texture",i,[&](auto&v){++v.materials[i].texture;});
 }
 for(int i=0;i<2;++i){
  reject("tag",i*10,[&](auto&v){v.tags[i].state=1;});reject("tag",i*10+1,[&](auto&v){--v.tags[i].frame;});
  reject("tag",i*10+2,[&](auto&v){v.tags[i].name=nullptr;});reject("tag",i*10+3,[&](auto&v){v.tags[i].str=nullptr;});
  reject("tag",i*10+4,[&](auto&v){v.tags[i].name="unknown";});reject("tag",i*10+5,[&](auto&v){v.tags[i].str="litadd";});
  reject("texture",i*10,[&](auto&v){v.textures[i].numframes=2;});reject("texture",i*10+1,[&](auto&v){--v.textures[i].desc.width;});
  reject("texture",i*10+2,[&](auto&v){--v.textures[i].desc.height;});
  for(int j=0;j<5;++j)reject("pixel_format",i*5+j,[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(&v.textures[i].desc.pixelFormat)+j*4;uint32_t x;memcpy(&x,p,4);x^=1;memcpy(p,&x,4);});
 }
 for(int i=1;i<3;++i){
  for(int j=0;j<32;++j)reject("quad_float",i*32+j,[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.verts[i].data())+j*4;float x;memcpy(&x,p,4);x+=.03125f;memcpy(p,&x,4);});
  for(int j=0;j<6;++j)reject("face_index",i*6+j,[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.faces[i].data())+j*2;uint16_t x;memcpy(&x,p,2);x^=1;memcpy(p,&x,2);});
 }
 // The exact profile keeps original inclusive integer traversal. Unknown
 // imagery must preserve the established half-open production policy.
 SAniKey32 test[6]={};for(int i=0;i<3;++i){test[i].code=8+i;test[i].value=64;test[3+i].code=8+i;test[3+i].value=128;}
 T3DImagery synthetic;synthetic.flags=I3D_ANIKEY32;synthetic.nstates=1;synthetic.frames=3;int count=6;void*ptr=test;
 synthetic.objects[0].numanikeys=&count;synthetic.objects[0].anikeys=&ptr;hmm_vec3 p,r,s;
 synthetic.speed_partsys_profile=false;assert(synthetic.GetUninterpolatedAniKey(0,0,1,p,r,s)&&s.X==.5f);puts("B default_half_open");
 synthetic.speed_partsys_profile=true;assert(synthetic.GetUninterpolatedAniKey(0,0,1,p,r,s)&&s.X==.25f);puts("B admitted_inclusive");
 for(int i=0;i<3;++i)for(int frame=0;frame<30;++frame){assert(im.GetUninterpolatedAniKey(i,0,frame,p,r,s));
  printf("P %d %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",i,frame,p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z);}
}
'''


def run(executable, archive, output):
    output.mkdir(parents=True, exist_ok=True)
    source_paths = ('src/3dimage.cpp','src/3dimage.h','src/3dimagebody.h','src/render3d_types.h')
    pinned = {p:sha((ROOT/p).read_bytes()) for p in source_paths}
    source = (ROOT/'src/3dimage.cpp').read_text()
    body = (ROOT/'src/3dimagebody.h').read_text()
    render = (ROOT/'src/render3d_types.h').read_text()
    signatures = ('static inline int32_t SkipAniKey32(', 'static inline void GetAniKey32(',
                  'bool T3DImagery::ValidateRetailSpeedPartSysProfile()',
                  'bool T3DImagery::GetUninterpolatedAniKey(')
    methods = {s:function(source,s) for s in signatures}
    prefixes = ('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32')
    constants = '\n'.join(line for line in body.splitlines() if line.startswith('#define ') and
                          any(line.startswith('#define '+p) for p in prefixes))
    records = '\n'.join(function(text,s)+';' for text,s in (
        (body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),
        (render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
    with zipfile.ZipFile(archive) as z:
        members = {n.lower():n for n in z.namelist()}
        class_bytes = z.read(members['class.def'])
        section = re.search(r'CLASS\s+"EFFECT"(.*?)(?=\bCLASS\s+"|\Z)',class_bytes.decode('latin1'),re.S)[1]
        binding = re.findall(r'^\s*"speed"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)',section,re.M)
        if binding != [('magic\\Speed.i3d','0xad92bd36')]:
            raise AssertionError('Exact CLASS EFFECT Speed binding changed')
        member = members['imagery/magic/speed.i3d']
        asset_bytes = z.read(member)
    if sha(asset_bytes) != SPEED_SHA or sha(executable.read_bytes()) != RETAIL_SHA:
        raise ValueError('Exact Speed asset or original executable changed')
    asset = read_asset(asset_bytes)
    asset.update(name='speed',class_definition=binding,archive_member=member)
    (output/'asset-triage.json').write_text(json.dumps([asset],indent=2)+'\n')
    asset_input = output/'speed.asset-input'
    asset_input.write_bytes(asset_bytes)
    generated = output/'production-profile-decoder.cpp'
    generated.write_text(PRELUDE+'\n'+constants+'\n'+records+'\n'+CLASS+'\n'+'\n'.join(methods.values())+'\n'+MAIN)
    binary = output/'production-profile-decoder'
    command = ['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',
               '-fno-omit-frame-pointer',str(generated),'-o',str(binary)]
    compiled = subprocess.run(command,capture_output=True,text=True)
    (output/'compile.log').write_text(compiled.stdout+compiled.stderr)
    if compiled.returncode:
        raise RuntimeError('Actual profile/decoder compile failed; see compile.log')
    result = subprocess.run([str(binary),str(asset_input)],capture_output=True,text=True)
    (output/'production-trace.txt').write_text(result.stdout)
    (output/'production-run.log').write_text(result.stderr)
    if result.returncode or result.stderr:
        raise RuntimeError('Profile rejection/decoder contract or sanitizer failed; see production-run.log')
    cases, policies, port = [], [], {}
    for line in result.stdout.splitlines():
        words = line.split()
        if words[0]=='C':cases.append(dict(outcome=words[1],category=words[2],index=int(words[3]) if len(words)>3 else None))
        elif words[0]=='B':policies.append(words[1])
        elif words[0]=='P':port[(int(words[1]),int(words[2]))]=[float(v) for v in words[3:]]
        else:raise AssertionError('Unexpected production trace')
    if len(port)!=90 or policies!=['default_half_open','admitted_inclusive']:
        raise AssertionError('Incomplete production decoder/boundary trace')
    native = NativeSpeed(executable,asset)
    vm = native.vm
    vectors = vm.allocate(36)
    errors, poses, maximum = [], [], 0
    for obj in range(3):
        for frame in range(30):
            # Immutable state/frame limits and exact keys are supplied by the
            # generalized resource fixture. Original walker and conversion run.
            if vm.call(0x409430,(obj,0,frame,vectors,vectors+12,vectors+24),this=native.imagery)!=1:
                raise AssertionError('Original exact Speed decoder rejected a valid pose')
            original = list(struct.unpack('<9f',vm.uc.mem_read(vectors,36)))
            current = port[(obj,frame)]
            for channel,(a,b) in enumerate(zip(original,current)):
                delta = abs(a-b)
                maximum = max(maximum,delta)
                if delta>3e-6:errors.append(dict(object=obj,frame=frame,channel=channel,retail=a,production=b,error=delta))
            poses.append(dict(object=obj,frame=frame,retail=original,production=current))
    if pinned != {p:sha((ROOT/p).read_bytes()) for p in source_paths}:
        raise AssertionError('Production source changed during contract comparison')
    categories = {}
    for c in cases:
        key=c['outcome']+':'+c['category']
        categories[key]=categories.get(key,0)+1
    report = dict(status='fail' if errors else 'pass',error_count=len(errors),errors=errors,
        type_id='0xad92bd36',asset=member,asset_sha256=SPEED_SHA,retail_sha256=RETAIL_SHA,
        class_def_sha256=sha(class_bytes),source_sha256=pinned,
        source_span_sha256={k:sha(v.encode()) for k,v in methods.items()},
        constants_sha256=sha(constants.encode()),record_definitions_sha256=sha(records.encode()),
        generated_source_sha256=sha(generated.read_bytes()),binary_sha256=sha(binary.read_bytes()),
        probe_sha256=sha(Path(__file__).read_bytes()),command=command,sanitizers='ASan/UBSan',
        profile_positive_cases=sum(c['outcome']=='accept' for c in cases),
        profile_rejection_cases=sum(c['outcome']=='reject' for c in cases),
        mutation_categories=categories,profile_cases=cases,boundary_policy_cases=policies,
        pose_samples=90,channel_comparisons=810,absolute_tolerance=3e-6,
        maximum_absolute_error=maximum,poses=poses,original_decoder='0x409430',
        original_key_conversion='0x4105d0',native_resource_boundaries=native.constructor,
        no_source_changes=True,new_rendered_ab_cases=0,accepted=False,full_accepted_rows=0,
        scope='Actual strict Speed profile and integer decoder source bodies/constants/packed records compile and execute. Literal shipped keys/tags/materials/vertices/faces/descriptors supplied by decoded readonly resource interface; independent mutable copies reject changes. Original decoder/conversion vs all3x30x9TRS channels. Full InitializeMesh/GetAniKey interpolation/owner matrices/controller geometry/lighting/raster/Metal/caller acceptance separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable',type=Path)
    p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi')
    p.add_argument('--output',type=Path,required=True)
    args=p.parse_args()
    result=run(args.executable,args.archive,args.output)
    print(json.dumps({k:result[k] for k in ('status','error_count','profile_positive_cases','profile_rejection_cases','mutation_categories','pose_samples','channel_comparisons','maximum_absolute_error')},indent=2))
    if result['error_count']:raise SystemExit(1)


if __name__=='__main__':
    main()
