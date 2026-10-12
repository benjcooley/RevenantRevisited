#!/usr/bin/env python3
"""Compile actual exact multi-texture generic root admission with sanitizers."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS,ROOT
from scroll_water_profile_contract import EXTRA
from speed_profile_contract import PRELUDE,function,sha
MAIN=r'''
int main(int argc,char**argv){assert(argc==4);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});TObjectInstance owner;owner.id=std::strtoul(argv[2],nullptr,0);T3DImagery im;Load(im,b,argv[3]);assert(generic_static_profile::Matches(&owner,&im));puts("accept literal");
 auto reject=[&](const char*cat,auto mutate){TObjectInstance o=owner;T3DImagery v;Load(v,b,argv[3]);mutate(o,v);assert(!generic_static_profile::Matches(&o,&v));printf("reject %s\n",cat);};
 assert(!generic_static_profile::Matches(nullptr,&im));assert(!generic_static_profile::Matches(&owner,nullptr));puts("reject null_owner");puts("reject null_image");
 reject("owner",[](auto&o,auto&){o.cls=0;});reject("owner",[](auto&o,auto&){o.state=1;});reject("owner",[](auto&o,auto&){o.id=0;});for(uint32_t id:{0xadbcef13u,0xad92bd23u})if(id!=owner.id)reject("typed_id",[&](auto&o,auto&){o.id=id;});
 for(const char*path:{"","misc/unverified.i3d","misc/charutility.i3d.bak"})reject("path",[&](auto&,auto&v){v.path=path;});reject("path",[](auto&,auto&v){v.null_filename=true;});
 for(int i=0;i<3;++i){T3DImagery good;Load(good,b,argv[3]);if(i==0)for(char&c:good.path)c=char(std::toupper((unsigned char)c));if(i==1)for(char&c:good.path)if(c=='/')c='\\';if(i==2)good.path="/assets/imagery/"+good.path;assert(generic_static_profile::Matches(&owner,&good));puts("accept path");}
 reject("header",[](auto&,auto&v){++v.nstates;});reject("header",[](auto&,auto&v){--v.frames;});reject("header",[](auto&,auto&v){v.aflags^=1;});reject("header",[](auto&,auto&v){--v.nobj;});reject("header",[](auto&,auto&v){--v.nmat;});reject("header",[](auto&,auto&v){++v.ntex;});reject("header",[](auto&,auto&v){++v.ntags;});reject("header",[](auto&,auto&v){v.hidden=true;});
 for(int i=0;i<im.nobj;++i){reject("object",[&](auto&,auto&v){v.objects[i].name[0]='?';});reject("object",[&](auto&,auto&v){++v.objects[i].material;});reject("object",[&](auto&,auto&v){v.parent[i]=0;});reject("object",[&](auto&,auto&v){--v.keycount[i];});reject("object",[&](auto&,auto&v){v.keyptr[i]=nullptr;});reject("object",[&](auto&,auto&v){++v.objects[i].numverts;});reject("object",[&](auto&,auto&v){++v.objects[i].numfaces;});
 for(int k=0;k<im.keycount[i];++k)reject("keyword",[&](auto&,auto&v){reinterpret_cast<uint32_t*>(v.keys[i].data())[k]^=0x100;});for(int t=0;t<=im.ntex;++t)reject("face_bin",[&](auto&,auto&v){++v.facecounts[i][t];});
 for(int k=0;k<im.objects[i].numverts*8;++k)reject("vertex",[&](auto&,auto&v){reinterpret_cast<float*>(v.verts[i].data())[k]+=.03125f;});for(int k=0;k<im.objects[i].numfaces*3;++k)reject("index",[&](auto&,auto&v){reinterpret_cast<uint16_t*>(v.faces[i].data())[k]^=1;});}
 for(int i=0;i<im.nmat;++i){reject("material_texture",[&](auto&,auto&v){++v.materials[i].texture;});for(int k=0;k<17;++k)reject("material_float",[&](auto&,auto&v){reinterpret_cast<float*>(&v.materials[i].matdesc.diffuse)[k]+=.03125f;});}
 for(int i=0;i<im.ntex;++i){reject("texture",[&](auto&,auto&v){++v.textures[i].numframes;});reject("texture",[&](auto&,auto&v){--v.textures[i].desc.width;});reject("texture",[&](auto&,auto&v){--v.textures[i].desc.height;});for(int k=0;k<5;++k)reject("texture",[&](auto&,auto&v){reinterpret_cast<uint32_t*>(&v.textures[i].desc.pixelFormat)[k]^=1;});}
}
'''
def run(archive,output):
 output.mkdir(parents=True,exist_ok=False);header=(ROOT/'src/genericstaticprofile.h').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text();guard='\n'.join(l for l in header.splitlines()if not l.startswith('#include')and l!='#pragma once');records='\n'.join(function(s,t)+';'for s,t in[(body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')])
 cls=CLASS.replace('#include "staticpartsysprofiles.h"','').replace('TYPED_METHOD','').replace('bool null_filename=false;','bool null_filename=false,hidden=false;int NumObjVerts(int i){return objects[i].numverts;}int NumObjFaces(int i){return objects[i].numfaces;}bool IsHidden(int,int){return hidden;}void GetObject(int i,S3DObj*out){*out=objects[i];}void GetTexture(int i,S3DTex*out){*out=textures[i];}void GetMaterial(int i,S3DMat*out){*out=materials[i];}').replace('std::array<S3DTex,1>','std::array<S3DTex,5>').replace('std::array<S3DMat,3>','std::array<S3DMat,5>').replace('std::array<int,2>,40>facecounts','std::array<int,6>,40>facecounts').replace('for(int t=0;t<2;++t)','for(int t=0;t<=im.ntex;++t)').replace('uint32_t ValidateRetailStaticParticleProfile();','uint32_t ValidateRetailStaticParticleProfile(){return 0;}')
 start=cls.index(' auto&t=im.textures[0];');end=cls.index('\n for(int i=0;i<im.nmat;',start);cls=cls[:start]+''' for(int i=0;i<im.ntex;++i){const size_t p=tex+120*i;auto&t=im.textures[i];t.numframes=U(b,p+116);t.desc.width=U(b,p+12);t.desc.height=U(b,p+8);t.desc.pixelFormat={U(b,p+84),U(b,p+88),U(b,p+92),U(b,p+96),U(b,p+100)};}'''+cls[end:]
 cls=cls.replace('void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*6);}','void GetObjFaces(int i,S3DFace*out,int*starts=nullptr,int*counts=nullptr){if(out)memcpy(out,faces.at(i).data(),faces.at(i).size()*6);for(int t=0;t<=ntex;++t){if(starts)starts[t]=0;if(counts)counts[t]=facecounts[i][t];}}')
 driver=output/'guard.cpp';driver.write_text(PRELUDE+'\n'+records+'\n'+cls+'\n'+EXTRA+'\n'+guard+'\n'+MAIN);binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Generic source admission compilation failed')
 cases=[]
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()}
  for p in json.loads((ROOT/'tools/retail_runtime/generic_static_profiles.json').read_text())['profiles']:
   data=z.read(ns['imagery/'+p['asset']]);assert sha(data)==p['asset_sha256'];path=output/(p['name']+'.asset-input');path.write_bytes(data);q=subprocess.run([str(binary),str(path),p['id'],p['asset']],capture_output=True,text=True);(output/(p['name']+'.log')).write_text(q.stderr);(output/(p['name']+'.trace')).write_text(q.stdout)
   if q.returncode or q.stderr:raise RuntimeError(p['name']+' source admission mutation failed')
   categories={}
   for l in q.stdout.splitlines():categories[l]=categories.get(l,0)+1
   cases.append(dict(name=p['name'],status='pass',asset_sha256=p['asset_sha256'],accept=sum(v for k,v in categories.items()if k.startswith('accept')),reject=sum(v for k,v in categories.items()if k.startswith('reject')),categories=categories))
 report=dict(status='pass',cases=cases,guard_sha256=sha(header.encode()),driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual strict owner/type/path/state0/header/root/key/vertex/face/texture/material guard against readonly exact multi-texture resources. Every mesh/material/key/facebin/descriptor field independently mutated. Whole assetSHA pins rawtexels; Door otherstates/naturalcontext/device separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.archive,a.output);print(r['status'],[(c['name'],c['accept'],c['reject'])for c in r['cases']])
