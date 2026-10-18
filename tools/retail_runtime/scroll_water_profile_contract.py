#!/usr/bin/env python3
"""Actual scroll-water exact resource guard compiled under ASan/UBSan."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS,ROOT
from speed_profile_contract import PRELUDE,function,sha

EXTRA=r'''
struct Owner{uint32_t id=0;int cls=25,state=0;int ObjClass()const{return cls;}uint32_t ObjId()const{return id;}int GetState()const{return state;}};
struct TObjectInstance:Owner{};
constexpr int OBJCLASS_EFFECT=25,MAXTEXTURES=8;
'''
MAIN=r'''
int main(int argc,char**argv){assert(argc==4);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});TObjectInstance owner;owner.id=std::strtoul(argv[2],nullptr,0);T3DImagery im;Load(im,b,argv[3]);assert(scroll_water_profile::Matches(&owner,&im));puts("accept literal");
 auto reject=[&](const char*name,auto mutate){TObjectInstance o=owner;T3DImagery v;Load(v,b,argv[3]);mutate(o,v);assert(!scroll_water_profile::Matches(&o,&v));printf("reject %s\n",name);};
 assert(!scroll_water_profile::Matches(nullptr,&im));assert(!scroll_water_profile::Matches(&owner,nullptr));puts("reject null_owner");puts("reject null_image");
 reject("owner",[](auto&o,auto&){o.cls=1;});reject("owner",[](auto&o,auto&){o.state=1;});reject("owner",[](auto&o,auto&){o.id=0;});
 for(uint32_t id:{0xadbcef15u,0xadbcef16u,0xadbcef17u})if(id!=owner.id)reject("cross_identity",[&](auto&o,auto&){o.id=id;});
 for(const char*path:{"","misc/unverified.i3d","misc/flowwater.i3d.bak"})reject("path",[&](auto&,auto&v){v.path=path;});reject("path",[](auto&,auto&v){v.null_filename=true;});
 for(int i=0;i<3;++i){T3DImagery good;Load(good,b,argv[3]);if(i==0)for(char&c:good.path)c=std::toupper((unsigned char)c);if(i==1)for(char&c:good.path)if(c=='/')c='\\';if(i==2)good.path="/assets/imagery/"+good.path;assert(scroll_water_profile::Matches(&owner,&good));puts("accept path");}
 reject("header",[](auto&,auto&v){++v.nstates;});reject("header",[](auto&,auto&v){--v.frames;});reject("header",[](auto&,auto&v){v.aflags^=1;});reject("header",[](auto&,auto&v){++v.nobj;});reject("header",[](auto&,auto&v){++v.nmat;});reject("header",[](auto&,auto&v){++v.ntex;});reject("header",[](auto&,auto&v){++v.ntags;});reject("header",[](auto&,auto&v){v.hidden=true;});reject("header",[](auto&,auto&v){v.scroll=!v.scroll;});
 reject("object",[](auto&,auto&v){v.objects[0].name[0]='?';});reject("object",[](auto&,auto&v){v.objects[0].material=1;});reject("object",[](auto&,auto&v){v.parent[0]=0;});reject("object",[](auto&,auto&v){--v.keycount[0];});reject("object",[](auto&,auto&v){v.keyptr[0]=nullptr;});reject("object",[](auto&,auto&v){++v.objects[0].numverts;});reject("object",[](auto&,auto&v){++v.objects[0].numfaces;});
 for(int i=0;i<im.keycount[0];++i)reject("keyword",[&](auto&,auto&v){reinterpret_cast<uint32_t*>(v.keys[0].data())[i]^=0x100;});
 if(im.ntags){ reject("tag",[](auto&,auto&v){v.tags[0].name=nullptr;});reject("tag",[](auto&,auto&v){v.tags[0].str=nullptr;});reject("tag",[](auto&,auto&v){v.tags[0].state=1;});reject("tag",[](auto&,auto&v){v.tags[0].frame=1;});reject("tag",[](auto&,auto&v){v.tags[0].name="unknown";});reject("tag",[](auto&,auto&v){v.tags[0].str="obj=water,dv=0";});}

 for(int i=0;i<17;++i)reject("material",[&](auto&,auto&v){auto*p=reinterpret_cast<float*>(&v.materials[0].matdesc.diffuse);p[i]+=.03125f;});reject("material",[](auto&,auto&v){v.materials[0].texture=1;});
 reject("texture",[](auto&,auto&v){v.textures[0].numframes=2;});reject("texture",[](auto&,auto&v){v.textures[0].desc.width=64;});reject("texture",[](auto&,auto&v){v.textures[0].desc.height=64;});for(int i=0;i<5;++i)reject("texture",[&](auto&,auto&v){reinterpret_cast<uint32_t*>(&v.textures[0].desc.pixelFormat)[i]^=1;});
 for(int i=0;i<im.objects[0].numverts*8;++i)reject("vertex",[&](auto&,auto&v){reinterpret_cast<float*>(v.verts[0].data())[i]+=.03125f;});for(int i=0;i<im.objects[0].numfaces*3;++i)reject("index",[&](auto&,auto&v){reinterpret_cast<uint16_t*>(v.faces[0].data())[i]^=1;});
 for(int i=0;i<2;++i)reject("face_bin",[&](auto&,auto&v){++v.facecounts[0][i];});
}
'''

def run(archive,output,profiles=None):
 output.mkdir(parents=True,exist_ok=True);body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();render=(ROOT/'src/engine/render/render3d_types.h').read_text();header=(ROOT/'src/game/effects/scrollwaterprofile.h').read_text();guard='\n'.join(l for l in header.splitlines()if not l.startswith('#include')and l!='#pragma once');records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cls=CLASS.replace('#include "staticpartsysprofiles.h"','').replace('TYPED_METHOD','').replace('bool null_filename=false;', 'bool null_filename=false,hidden=false,scroll=true;int NumObjVerts(int i){return objects[i].numverts;}int NumObjFaces(int i){return objects[i].numfaces;}bool IsHidden(int,int){return hidden;}bool HasScrollTex(){return scroll;}void GetObject(int i,S3DObj*out){*out=objects[i];}void GetTexture(int i,S3DTex*out){*out=textures[i];}void GetMaterial(int i,S3DMat*out){*out=materials[i];}')
 cls=cls.replace('im.ntags=U(b,body+52);','im.ntags=U(b,body+52);im.scroll=im.ntags>0;')
 cls=cls.replace('uint32_t ValidateRetailStaticParticleProfile();','uint32_t ValidateRetailStaticParticleProfile(){return 0;}').replace('void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*6);}','void GetObjFaces(int i,S3DFace*out,int*starts=nullptr,int*counts=nullptr){if(out)memcpy(out,faces.at(i).data(),faces.at(i).size()*6);if(starts)starts[0]=starts[1]=0;if(counts){counts[0]=facecounts[i][0];counts[1]=facecounts[i][1];}}')
 driver=output/'guard.cpp';driver.write_text(PRELUDE+'\n'+records+'\n'+cls+'\n'+EXTRA+'\n'+guard+'\n'+MAIN);binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Actual guard compilation failed')
 profiles=json.loads((ROOT/'tools/retail_runtime/scrolltex_water_profiles.json').read_text())['profiles'][:3]if profiles is None else profiles;cases=[]
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()}
  for p in profiles:
   data=z.read(ns['imagery/'+p['asset'].lower()]);assert sha(data)==p['asset_sha256'];path=output/(p['name']+'.asset-input');path.write_bytes(data);q=subprocess.run([str(binary),str(path),p['id'],p['asset']],capture_output=True,text=True);(output/(p['name']+'.log')).write_text(q.stderr);(output/(p['name']+'.trace')).write_text(q.stdout)
   if q.returncode or q.stderr:raise RuntimeError(p['name']+' guard mutation failed')
   categories={}
   for l in q.stdout.splitlines():categories[l]=categories.get(l,0)+1
   cases.append(dict(name=p['name'],status='pass',asset_sha256=p['asset_sha256'],accept=sum(v for k,v in categories.items()if k.startswith('accept')),reject=sum(v for k,v in categories.items()if k.startswith('reject')),categories=categories))
 report=dict(status='pass',cases=cases,guard_header_sha256=sha(header.encode()),driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual typed scroll-water source guard against immutable exact resource records, with independent mutations of every vertex channel/index/key word/material channel/tag/header/type/path/texture contract. Raw texels fixture-pinned by whole asset SHA; scene/device/caller behavior separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--other-water',action='store_true');a=p.parse_args();profiles=json.loads((ROOT/'tools/retail_runtime/other_water_profiles.json').read_text())['profiles']if a.other_water else None;print(json.dumps(run(a.archive,a.output,profiles),indent=2))
