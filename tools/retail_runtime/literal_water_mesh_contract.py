#!/usr/bin/env python3
"""Compile actual literal Water RGB565 geometry admission, mutate every input."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS,ROOT
from speed_profile_contract import PRELUDE,function,sha
MAIN=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b,"misc/water.i3d");assert(literal_water_mesh::Matches(&im));puts("accept literal");assert(!literal_water_mesh::Matches(nullptr));puts("reject null_image");
auto reject=[&](const char*category,auto mutate){T3DImagery v;Load(v,b,"misc/water.i3d");mutate(v);assert(!literal_water_mesh::Matches(&v));printf("reject %s\n",category);};
for(const char*p:{"Misc\\Water.I3D","/assets/imagery/misc/water.i3d"}){T3DImagery v;Load(v,b,p);assert(literal_water_mesh::Matches(&v));puts("accept path");}
for(const char*p:{"misc/wfall.i3d","notmisc/water.i3d","misc/water.i3d.bak",""})reject("path",[&](auto&v){v.path=p;});reject("path",[](auto&v){v.null_filename=true;});
reject("header",[](auto&v){++v.nobj;});reject("header",[](auto&v){++v.nmat;});reject("header",[](auto&v){++v.ntex;});reject("header",[](auto&v){++v.nstates;});reject("header",[](auto&v){++v.frames;});reject("header",[](auto&v){++v.ntags;});reject("object",[](auto&v){v.hidden=true;});reject("object",[](auto&v){v.objects[0].name[0]='?';});reject("object",[](auto&v){++v.objects[0].material;});reject("object",[](auto&v){v.parent[0]=0;});reject("object",[](auto&v){++v.objects[0].numverts;});reject("object",[](auto&v){++v.objects[0].numfaces;});for(int i=0;i<2;++i)reject("face_bin",[&](auto&v){++v.facecounts[0][i];});
for(int i=0;i<32;++i)reject("vertex",[&](auto&v){reinterpret_cast<float*>(v.verts[0].data())[i]+=.03125f;});for(int i=0;i<6;++i)reject("index",[&](auto&v){reinterpret_cast<uint16_t*>(v.faces[0].data())[i]^=1;});for(int i=0;i<17;++i)reject("material",[&](auto&v){reinterpret_cast<float*>(&v.materials[0].matdesc.diffuse)[i]+=.03125f;});reject("material",[](auto&v){++v.materials[0].texture;});
reject("texture",[](auto&v){++v.textures[0].numframes;});reject("texture",[](auto&v){--v.textures[0].desc.width;});reject("texture",[](auto&v){--v.textures[0].desc.height;});for(int i=0;i<5;++i)reject("texture",[&](auto&v){reinterpret_cast<uint32_t*>(&v.textures[0].desc.pixelFormat)[i]^=1;});
}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);header=(ROOT/'src/literalwatermesh.h').read_text();guard=header[:header.index('inline MeshHandle Cache')];guard='\n'.join(l for l in guard.splitlines()if not l.startswith('#include')and l!='#pragma once')+'\n}\n'
 body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text();records='\n'.join(function(s,t)+';'for s,t in[(body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')])
 cls=CLASS.replace('#include "staticpartsysprofiles.h"','').replace('TYPED_METHOD','').replace('uint32_t ValidateRetailStaticParticleProfile();','uint32_t ValidateRetailStaticParticleProfile(){return 0;}').replace('bool null_filename=false;','bool null_filename=false,hidden=false;int NumObjVerts(int i){return objects[i].numverts;}int NumObjFaces(int i){return objects[i].numfaces;}bool IsHidden(int,int){return hidden;}void GetObject(int i,S3DObj*out){*out=objects[i];}void GetTexture(int i,S3DTex*out){*out=textures[i];}void GetMaterial(int i,S3DMat*out){*out=materials[i];}')
 # The old sixteen-frame eight-byte keys are irrelevant to these custom
 # OBJ3D_MATRIX Render paths; keep only the exact parent field in this guard
 # contract. Full native fixture executes the actual legacy record provider.
 start=cls.index('im.keycount[i]=U(b,st+4);');end=cls.index('\n obj.parent=',start);cls=cls[:start]+'im.keycount[i]=0;im.keyptr[i]=nullptr;'+cls[end:]
 cls=cls.replace('void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*6);}','void GetObjFaces(int i,S3DFace*out,int*starts=nullptr,int*counts=nullptr){if(out)memcpy(out,faces.at(i).data(),faces.at(i).size()*6);for(int t=0;t<2;++t){if(starts)starts[t]=0;if(counts)counts[t]=facecounts[i][t];}}')
 driver=output/'guard.cpp';driver.write_text(PRELUDE+'\nconstexpr int MAXTEXTURES=8;\n'+records+'\n'+cls+'\n'+guard+'\n'+MAIN);binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Literal guard compilation failed')
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/water.i3d')
 assert sha(data)=='ece24f00eab927662fea2027dde203afec2525471eb6cde7b2a06f69c9b9aa75';asset=output/'water.asset-input';asset.write_bytes(data);q=subprocess.run([str(binary),str(asset)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr)
 if q.returncode or q.stderr:raise RuntimeError('Literal guard mutation failed')
 report=dict(status='pass',accepted=False,positive=q.stdout.count('accept '),negative=q.stdout.count('reject '),asset_sha256=sha(data),header_sha256=sha(header.encode()),binary_sha256=sha(binary.read_bytes()),driver_sha256=sha(driver.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),sanitizers='ASan/UBSan',scope='Actual literal water path/object/parent/geometry/facebins/all32vertexchannels/all6indices/all17materialfloats/texture descriptor guard. Old16frame pose track is unused by custom matrix Render; every state/RNG/whole matrix/light/raster independently executed in native scene. Raw texture pinned by whole asset SHA, not engine hash.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(json.dumps(run(a.output),indent=2))
