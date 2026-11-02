#!/usr/bin/env python3
"""Actual typed FlameB/G resource guard under ASan/UBSan and field mutations."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS,ROOT
from scroll_water_profile_contract import EXTRA
from speed_profile_contract import PRELUDE,function,sha
from flame_variants_visual_reference import PROFILES
MAIN=r'''
int main(int argc,char**argv){assert(argc==4);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});TObjectInstance o;o.id=std::strtoul(argv[2],nullptr,0);T3DImagery im;Load(im,b,argv[3]);assert(Matches(&o,&im));puts("accept literal");
auto reject=[&](auto mutate){TObjectInstance owner=o;T3DImagery v;Load(v,b,argv[3]);mutate(owner,v);assert(!Matches(&owner,&v));puts("reject field");};
assert(!Matches(nullptr,&im));assert(!Matches(&o,nullptr));puts("reject null");puts("reject null");
reject([](auto&o,auto&){o.id^=1;});reject([](auto&o,auto&){o.cls=0;});reject([](auto&o,auto&){o.state=1;});
for(const char*p:{"", "magic/flame.i3d", "magic/flameb.i3d.bak"})reject([&](auto&,auto&v){v.path=p;});reject([](auto&,auto&v){v.null_filename=true;});
for(int i=0;i<3;++i){T3DImagery v;Load(v,b,argv[3]);if(i==0)for(char&c:v.path)c=char(std::toupper((unsigned char)c));if(i==1)for(char&c:v.path)if(c=='/')c='\\';if(i==2)v.path="/assets/imagery/"+v.path;assert(Matches(&o,&v));puts("accept path");}
reject([](auto&,auto&v){++v.nstates;});reject([](auto&,auto&v){++v.frames;});reject([](auto&,auto&v){v.aflags^=1;});reject([](auto&,auto&v){++v.nobj;});reject([](auto&,auto&v){++v.nmat;});reject([](auto&,auto&v){++v.ntex;});reject([](auto&,auto&v){++v.ntags;});
reject([](auto&,auto&v){v.objects[0].name[0]='?';});reject([](auto&,auto&v){++v.objects[0].material;});reject([](auto&,auto&v){v.parent[0]=0;});reject([](auto&,auto&v){--v.keycount[0];});reject([](auto&,auto&v){v.keyptr[0]=nullptr;});reject([](auto&,auto&v){++v.objects[0].numverts;});reject([](auto&,auto&v){++v.objects[0].numfaces;});
for(int k=0;k<9;++k)reject([&](auto&,auto&v){reinterpret_cast<uint32_t*>(v.keys[0].data())[k]^=0x100;});for(int k=0;k<32;++k)reject([&](auto&,auto&v){reinterpret_cast<float*>(v.verts[0].data())[k]+=.03125f;});for(int k=0;k<6;++k)reject([&](auto&,auto&v){reinterpret_cast<uint16_t*>(v.faces[0].data())[k]^=1;});for(int k=0;k<2;++k)reject([&](auto&,auto&v){++v.facecounts[0][k];});
reject([](auto&,auto&v){++v.materials[0].texture;});for(int k=0;k<17;++k)reject([&](auto&,auto&v){reinterpret_cast<float*>(&v.materials[0].matdesc.diffuse)[k]+=.03125f;});reject([](auto&,auto&v){++v.textures[0].numframes;});reject([](auto&,auto&v){--v.textures[0].desc.width;});reject([](auto&,auto&v){--v.textures[0].desc.height;});for(int k=0;k<5;++k)reject([&](auto&,auto&v){reinterpret_cast<uint32_t*>(&v.textures[0].desc.pixelFormat)[k]^=1;});reject([](auto&,auto&v){v.valid_texture=false;});
}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/flamevariantspreview.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();render=(ROOT/'src/engine/render/render3d_types.h').read_text();records='\n'.join(function(s,t)+';'for s,t in[(body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')]);cls=CLASS.replace('#include "staticpartsysprofiles.h"','').replace('TYPED_METHOD','').replace('uint32_t ValidateRetailStaticParticleProfile();','uint32_t ValidateRetailStaticParticleProfile(){return 0;}').replace('bool null_filename=false;','bool null_filename=false,valid_texture=true;uint32_t GetTextureHandle(int){return valid_texture?1:0;}int NumObjVerts(int i){return objects[i].numverts;}int NumObjFaces(int i){return objects[i].numfaces;}void GetObject(int i,S3DObj*out){*out=objects[i];}void GetMaterial(int i,S3DMat*out){*out=materials[i];}void GetTexture(int i,S3DTex*out){*out=textures[i];}')
 cls=cls.replace('void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*6);}','void GetObjFaces(int i,S3DFace*out,int*starts=nullptr,int*counts=nullptr){if(out)memcpy(out,faces.at(i).data(),faces.at(i).size()*6);for(int t=0;t<=ntex;++t){if(starts)starts[t]=0;if(counts)counts[t]=facecounts[i][t];}}')
 driver=output/'guard.cpp';driver.write_text(PRELUDE+'\n#include <cctype>\nconstexpr uint32_t kInvalidTexture=0;\n'+records+'\n'+cls+'\n'+EXTRA+'\n'+function(source,'static uint64_t Hash(')+'\n'+function(source,'bool Matches(')+'\n'+MAIN);binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('compile failed')
 cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name.lower()+'.i3d'));assert sha(data)==digest;asset=output/(name+'.fixture');asset.write_bytes(data);q=subprocess.run([str(binary),str(asset),id,'Magic/'+name+'.I3D'],capture_output=True,text=True);(output/(name+'.trace')).write_text(q.stdout);(output/(name+'.log')).write_text(q.stderr);cases.append(dict(name=name,id=id,status='pass'if q.returncode==0 and not q.stderr else'fail',asset_sha256=digest,accept=q.stdout.count('accept '),reject=q.stdout.count('reject ')))
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,source_sha256=sha(source.encode()),driver_sha256=sha(driver.read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual strict typed FlameB/G own path/root/state0 header/key/vertex/index/material/texture guard with independent negative mutations; texels pinned by exact asset SHA. No broad alpha/geometry profile admission.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],[(c['name'],c['accept'],c['reject'])for c in cases]);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
