#!/usr/bin/env python3
"""Actual strict admission bodies for Regeneration, SwiftStrike and Fspray."""
import argparse,json,re,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import PRELUDE,function,sha,ROOT

PROFILES=(
 dict(name='Regeneration',id='0x10ac03de',asset='magic/Regen.i3d',sha256='42124796a4e5573f58e34d5afd96e735e132c87bd30d2047b5d74aeae21dff8f'),
 dict(name='SwiftStrike',id='0xe0a3bc43',asset='magic/SwiftStrike.i3d',sha256='45896fb4ff8d19253a242849b496b52023ca4a91fe5b2ec3a540dfde1b6b384c'),
 dict(name='fspray',id='0x0c052638',asset='misc/Fspray.i3d',sha256='1d883eeba4cb9ee857849fd9635084e84ae5ebf5f328c857fd4038a786fada4e'),
)

Y_PROFILES=(
 dict(name='YEnergy',id='0xaeaeeb30',asset='magic/yenergy.i3d',sha256='89f1f9e88c8d64ba1278f54d3bc7a10275204feb7463c4ee3b9145dff1cd9be2',prototype=0,width=1024,samples=(5,9,15,20,25),numeric_tolerance=0.0003),
 dict(name='YEnergyLose',id='0xaeaeeb33',asset='magic/yenergylose.i3d',sha256='b499a9c11058ccf9d1cd886fc2dbfc18cd62f528344f4a527889d1ebaf20dfaa',prototype=0,width=1024,samples=(5,9,15,20,25),numeric_tolerance=0.0003,clamp=True),
 dict(name='YAbsorb',id='0xaeaeeb29',asset='magic/yabsorb.i3d',sha256='402b13540b8ca345cd9683bae5d5a25b691f7fbb2894879329ff8c25a2ead587',prototype=0,width=1024,samples=(5,15,29,45,59),numeric_tolerance=0.0003,clamp=True),
 dict(name='nullifier',id='0xad92bd38',asset='magic/Nullifier.i3d',sha256='7d834a927570771d479d5955902f6ff7fff85fc98442098a0d6eb96d8b74286c',prototype=1,width=1024,samples=(5,15,29,60,89)),
)

CAMERA_PROFILES=(
 dict(name='Ogrestrength',id='0x42e0fcd0',asset='magic/Ogre.i3d',sha256='df40ba633e9db0f0cfbcf8fa819d5a7e8105317707890e0293c5786532755072',prototype=0,width=1024,samples=(5,15,29,45,59),numeric_tolerance=0.0003),
 dict(name='trollblood',id='0xad92bd39',asset='magic/Trollblood.i3d',sha256='8b6bc7b9a2c67fea510fa03bb8107a1661ed9a30194b4cd6f8d2e7fcc1fda95a',prototype=0,width=1024,samples=(5,15,29,45,59),numeric_tolerance=0.0003),
)

CLASS=r'''
#include "staticpartsysprofiles.h"
struct S3DMat{S3DMaterial matdesc{};int texture=-1;};
class T3DImagery{public:
 bool null_filename=false;uint32_t static_particles_profile=0;
 int version=0,flags=0,numverts=0,numfaces=0,nstates=0,frames=0,aflags=0,nobj=0,nmat=0,ntex=0,ntags=0;
 std::string path;std::array<S3DObj,40>objects;std::array<S3DTex,1>textures;std::array<S3DMat,3>materials;
 std::array<S3DTag,2>tags;std::array<std::string,2>tag_names,tag_values;
 std::array<int,40>parent{},keycount{};std::array<void*,40>keyptr{};std::array<std::array<int,2>,40>facecounts{};
 std::array<std::vector<SAniKey32>,40>keys;std::array<std::vector<S3DVertex>,40>verts;std::array<std::vector<S3DFace>,40>faces;
 char*GetResFilename(){return null_filename?nullptr:path.data();}int NumStates(){return nstates;}
 int GetAniLength(int){return frames;}int GetAniFlags(int){return aflags;}int NumObjects(){return nobj;}
 int NumMaterials(){return nmat;}int NumTextures(){return ntex;}int NumTags(){return ntags;}S3DTag*GetTag(int i){return &tags.at(i);}
 void GetObjVerts(int i,S3DVertex*out,int=0,int=0){memcpy(out,verts.at(i).data(),verts.at(i).size()*32);}
 void GetObjFaces(int i,S3DFace*out){memcpy(out,faces.at(i).data(),faces.at(i).size()*6);}
 void Select(){static_particles_profile=ValidateRetailStaticParticleProfile();}
 uint32_t ValidateRetailStaticParticleProfile();
 TYPED_METHOD
};
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t v;memcpy(&v,b.data()+p,4);return v;}
uint16_t H(const std::vector<unsigned char>&b,size_t p){assert(p+2<=b.size());uint16_t v;memcpy(&v,b.data()+p,2);return v;}
size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
void Load(T3DImagery&im,const std::vector<unsigned char>&b,const char*path){im.path=path;
 const size_t body=20+U(b,16),objs=P(b,body+48),tex=P(b,body+40),mat=P(b,body+32),tag=P(b,body+56),vtx=P(b,P(b,P(b,body+16))),idx=P(b,body+24);
 im.version=U(b,body+4);im.flags=U(b,body);im.numverts=U(b,body+12);im.numfaces=U(b,body+20);
 im.nobj=U(b,body+44);im.nmat=U(b,body+28);im.ntex=U(b,body+36);im.ntags=U(b,body+52);im.nstates=U(b,24);im.frames=H(b,70);im.aflags=H(b,68);
 for(int i=0;i<im.nobj;++i){const size_t o=objs+i*48,st=P(b,o+44),raw=P(b,st+8),fg=P(b,o+40);auto&obj=im.objects[i];
 memcpy(obj.name,b.data()+o,32);im.parent[i]=int32_t(U(b,st));im.keycount[i]=U(b,st+4);im.keys[i].resize(im.keycount[i]);memcpy(im.keys[i].data(),b.data()+raw,4*im.keycount[i]);im.keyptr[i]=im.keys[i].data();
 obj.parent=&im.parent[i];obj.numanikeys=&im.keycount[i];obj.anikeys=&im.keyptr[i];obj.material=H(b,o+32);obj.numverts=H(b,o+36);obj.numfaces=0;
 for(int t=0;t<2;++t){im.facecounts[i][t]=H(b,fg+t*4+2);obj.numfaces+=im.facecounts[i][t];for(int f=0;f<im.facecounts[i][t];++f){S3DFace face;memcpy(&face,b.data()+idx+6*(H(b,fg+t*4)+f),6);im.faces[i].push_back(face);}}
 obj.numtexfaces=im.facecounts[i].data();im.verts[i].resize(obj.numverts);if(obj.numverts)memcpy(im.verts[i].data(),b.data()+vtx+32*H(b,o+34),32*obj.numverts);}
 auto&t=im.textures[0];t.numframes=U(b,tex+116);t.desc.width=U(b,tex+12);t.desc.height=U(b,tex+8);t.desc.pixelFormat={U(b,tex+84),U(b,tex+88),U(b,tex+92),U(b,tex+96),U(b,tex+100)};
 for(int i=0;i<im.nmat;++i){memcpy(&im.materials[i].matdesc,b.data()+mat+i*80,80);im.materials[i].texture=int32_t(U(b,mat+i*80+72));}
 for(int i=0;i<im.ntags;++i){size_t p=tag+i*16;auto&t0=im.tags[i];t0.state=int32_t(U(b,p));t0.frame=int32_t(U(b,p+4));im.tag_names[i]=reinterpret_cast<const char*>(b.data()+P(b,p+8));im.tag_values[i]=reinterpret_cast<const char*>(b.data()+P(b,p+12));t0.name=im.tag_names[i].c_str();t0.str=im.tag_values[i].c_str();}
}
'''
MAIN=r'''
int main(int argc,char**argv){assert(argc==4);static_assert(sizeof(S3DVertex)==32);static_assert(sizeof(S3DMaterial)==80);
 std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});uint32_t id=std::strtoul(argv[2],nullptr,0);T3DImagery im;Load(im,b,argv[3]);im.Select();assert(im.HasRetailStaticParticleProfile(id));puts("C accept literal");
 int proto=-1;for(const auto&p:retail_static_particles::profiles)if(p.id==id)proto=p.prototype;assert(proto>=0);
 auto reject=[&](const char*cat,auto mutate){T3DImagery bad;Load(bad,b,argv[3]);mutate(bad);bad.Select();assert(bad.static_particles_profile==0);printf("C reject %s\n",cat);};
 for(uint32_t other:{0u,0xdeadbeefu,0x10ac03deu,0xe0a3bc43u,0x0c052638u})if(other!=id){assert(!im.HasRetailStaticParticleProfile(other));puts("C reject typed_id");}
 for(const char*p:{"","magic/unverified.i3d","notmagic/regen.i3d","magic/regen.i3d.bak"})reject("path",[&](auto&v){v.path=p;});reject("path",[](auto&v){v.null_filename=true;});
 for(int variant=0;variant<3;++variant){T3DImagery good;Load(good,b,argv[3]);if(variant==0)for(char&c:good.path)c=char(std::toupper(static_cast<unsigned char>(c)));if(variant==1)for(char&c:good.path)if(c=='/')c='\\';if(variant==2)good.path="/assets/Imagery/"+good.path;good.Select();assert(good.static_particles_profile==id);puts("C accept path");}
 reject("header",[](auto&v){--v.version;});reject("header",[](auto&v){v.flags^=1;});reject("header",[](auto&v){++v.nstates;});reject("header",[](auto&v){--v.frames;});reject("header",[](auto&v){v.aflags^=1;});reject("header",[](auto&v){--v.nobj;});reject("header",[](auto&v){--v.nmat;});reject("header",[](auto&v){++v.ntex;});reject("header",[](auto&v){++v.ntags;});reject("header",[](auto&v){++v.numverts;});reject("header",[](auto&v){++v.numfaces;});
 for(int i=0;i<im.nobj;++i){for(int k=0;k<int(im.keys[i].size());++k)reject("key_word",[&](auto&v){reinterpret_cast<uint32_t*>(v.keys[i].data())[k]^=0x100;});
 reject("object",[&](auto&v){v.objects[i].name[0]='?';});reject("object",[&](auto&v){v.parent[i]=0;});reject("object",[&](auto&v){++v.objects[i].material;});reject("object",[&](auto&v){++v.keycount[i];});reject("object",[&](auto&v){v.keyptr[i]=nullptr;});reject("object",[&](auto&v){++v.objects[i].numverts;});reject("object",[&](auto&v){++v.objects[i].numfaces;});for(int bin=0;bin<2;++bin)reject("face_bin",[&](auto&v){++v.facecounts[i][bin];});}
 for(int i=0;i<im.nmat;++i){for(int j=0;j<17;++j)reject("material_float",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(&v.materials[i].matdesc.diffuse)+j*4;float x;memcpy(&x,p,4);x+=.03125f;memcpy(p,&x,4);});reject("material_texture",[&](auto&v){++v.materials[i].texture;});}
 for(int i=0;i<im.ntags;++i){reject("tag",[&](auto&v){v.tags[i].state=1;});reject("tag",[&](auto&v){--v.tags[i].frame;});reject("tag",[&](auto&v){v.tags[i].name=nullptr;});reject("tag",[&](auto&v){v.tags[i].str=nullptr;});reject("tag",[&](auto&v){v.tags[i].name="unknown";});reject("tag",[&](auto&v){v.tags[i].str="obj=(unknown),particle=#particle";});}
 reject("texture",[](auto&v){++v.textures[0].numframes;});reject("texture",[](auto&v){--v.textures[0].desc.width;});reject("texture",[](auto&v){--v.textures[0].desc.height;});for(int j=0;j<5;++j)reject("texture",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(&v.textures[0].desc.pixelFormat)+j*4;uint32_t x;memcpy(&x,p,4);x^=1;memcpy(p,&x,4);});
 for(int j=0;j<32;++j)reject("vertex_float",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.verts[proto].data())+j*4;float x;memcpy(&x,p,4);x+=.03125f;memcpy(p,&x,4);});for(int j=0;j<6;++j)reject("face_index",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.faces[proto].data())+j*2;uint16_t x;memcpy(&x,p,2);x^=1;memcpy(p,&x,2);});
 for(const auto&profile:retail_static_particles::profiles)if(profile.id==id)for(int object=0;object<im.nobj;++object)if(profile.object[object].mesh_vertices){
 for(int j=0;j<im.objects[object].numverts*8;++j)reject("base_vertex_float",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.verts[object].data())+j*4;float x;memcpy(&x,p,4);x+=.03125f;memcpy(p,&x,4);});
 for(int j=0;j<im.objects[object].numfaces*3;++j)reject("base_face_index",[&](auto&v){auto*p=reinterpret_cast<unsigned char*>(v.faces[object].data())+j*2;uint16_t x;memcpy(&x,p,2);x^=1;memcpy(p,&x,2);});}

}
'''

def run(archive,output,profiles=PROFILES):
 output.mkdir(parents=True,exist_ok=True);source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text();method=function(source,'uint32_t T3DImagery::ValidateRetailStaticParticleProfile()');typed=function((ROOT/'src/3dimage.h').read_text(),'bool HasRetailStaticParticleProfile(')
 records='\n'.join(function(text,s)+';'for text,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cpp=PRELUDE+'\n'+records+'\n'+CLASS.replace('TYPED_METHOD',typed)+'\n'+method+'\n'+MAIN;driver=output/'profile.cpp';driver.write_text(cpp);binary=output/'profile';cmd=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)];p=subprocess.run(cmd,text=True,capture_output=True);(output/'compile.log').write_text(p.stdout+p.stderr)
 if p.returncode:raise RuntimeError('Profile compilation failed')
 cases=[]
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()};definition=z.read(ns['class.def']).decode('latin1');section=re.search(r'CLASS\s+"EFFECT"(.*?)(?=\bCLASS\s+"|\Z)',definition,re.S)[1]
  for profile in profiles:
   binding=re.findall(r'^\s*"'+re.escape(profile['name'])+r'"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)',section,re.M);assert len(binding)==1 and binding[0][0].replace('\\','/').lower()==profile['asset'].lower() and int(binding[0][1],16)==int(profile['id'],16)
   data=z.read(ns['imagery/'+profile['asset'].lower()]);assert sha(data)==profile['sha256'];path=output/(profile['name']+'.asset-input');path.write_bytes(data);p=subprocess.run([str(binary),str(path),profile['id'],profile['asset']],text=True,capture_output=True);(output/(profile['name']+'.log')).write_text(p.stderr);(output/(profile['name']+'.trace')).write_text(p.stdout)
   if p.returncode or p.stderr:raise RuntimeError(profile['name']+' actual profile mutation failed')
   categories={}
   for line in p.stdout.splitlines():categories[line]=categories.get(line,0)+1
   cases.append(dict(name=profile['name'],status='pass',positive_cases=sum(v for k,v in categories.items()if k.startswith('C accept')),rejection_cases=sum(v for k,v in categories.items()if k.startswith('C reject')),categories=categories,asset_sha256=profile['sha256'],class_binding=binding))
 report=dict(status='pass',cases=cases,compiled_profile_body_sha256=sha(method.encode()),compiled_typed_gate_sha256=sha(typed.encode()),header_sha256=sha((ROOT/'src/staticpartsysprofiles.h').read_bytes()),driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual exact profile validator and typed gate compiled against literal decoded record API. Every keyword/material float/vertex channel/index plus header/tag/path/type/texture mutation rejects; raw texels pinned by fixture asset SHA, not hashed by engine admission.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--y-family',action='store_true');p.add_argument('--camera-particles',action='store_true');a=p.parse_args();print(json.dumps(run(a.archive,a.output,CAMERA_PROFILES if a.camera_particles else Y_PROFILES if a.y_family else PROFILES),indent=2))
