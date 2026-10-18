#include "createfoodoriginal.h"
#include "../createfoodcore.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include "../revutils.h"
#include <cstring>
#include <memory>
#include <vector>
namespace createfood_original {
static uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;for(auto*b=static_cast<const uint8_t*>(p);n--;){h^=*b++;h*=1099511628211ull;}return h;}
struct State{std::unique_ptr<TObjectInstance>owner;retail_createfood::State core;retail_createfood::Lookup lookup;MeshHandle meshes[2]{};S3DMat materials[2]{};double accumulator=0;int64_t rendered=-1;~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State*Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x838cffbau):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
 if(!im||s->owner->ObjId()!=0x838cffbau||im->NumStates()!=1||im->GetAniLength(0)!=100||im->GetAniFlags(0)!=0x2001||im->NumObjects()!=2||im->NumTextures()!=2||im->NumMaterials()!=2||im->NumTags()!=0)return nullptr;
 SetIntPos(s->owner, origin);s->owner->Face(0);s->owner->OnScreen();
 for(int i=0;i<2;++i){S3DObj o={};im->GetObject(i,&o);if(std::strcmp(o.name,i?"smoke01":"smoke")||o.material!=i||o.parent[0]!=-1||im->NumObjVerts(i)!=4||im->NumObjFaces(i)!=2)return nullptr;
  S3DVertex raw[4];S3DFace faces[2];im->GetObjVerts(i,raw);im->GetObjFaces(i,faces);if(Hash(raw,sizeof(raw))!=0xf642adfd8edb9b0bull||Hash(faces,sizeof(faces))!=0x8de70ea438f81391ull)return nullptr;
  S3DTex texture={};im->GetTexture(i,&texture);const auto&f=texture.desc.pixelFormat;if(texture.numframes!=1||f.dwRGBBitCount!=16||f.dwRBitMask!=0xf800||f.dwGBitMask!=0x7e0||f.dwBBitMask!=0x1f||f.dwRGBAlphaBitMask)return nullptr;
  std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,indices)||v.size()!=4||indices.size()!=6||im->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  im->GetMaterial(i,&s->materials[i]);s->meshes[i]=Renderer->RegisterMeshAsset(0x4352464f4f440000ull+uint64_t(i),v.data(),int(v.size()),indices.data(),int(indices.size()),im->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 log_info("[native-domain] enabled=%d camera_z=0 exact Createfood",int(StartupVfxNativeDomain));log_info("[createfood-original] typed_id=838cffba quality=3 white_light0=28 ambient=32 empty_map_clock=0 kill80=1");return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(Alive(s)&&s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->SetCommandDone(false);s->core.Pulse([](int a,int b){return ::random(a,b);},s->lookup,[](int food){const auto index=retail_createfood::EmptyMapFoodIndex([](){return GameRand();},[](uint32_t seed){SeedRandom(seed);});log_info("[createfood-food-boundary] type=%d index=%u insertion=-1 empty_map=1",food,index);});if(!s->core.alive)s->owner->SetFlag(OF_KILL);}}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 root;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();int count=0;
 s->core.Render(s->lookup,[&](int object,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&root);SHelperMeshSubmit draw={};draw.mesh=s->meshes[object];draw.retail_lighting=1;draw.additive_blend=true;draw.retail_software_projection=StartupVfxNativeDomain;draw.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];const auto&m=s->materials[object].matdesc;std::memcpy(draw.diffuse,&m.diffuse,16);std::memcpy(draw.ambient,&m.ambient,16);std::memcpy(draw.specular,&m.specular,16);std::memcpy(draw.emissive,&m.emissive,16);draw.power=m.power;Renderer->SubmitHelperMesh(draw);++count;});
 log_info("[createfood-phase] tick=%d counter=%d draws=%d glow=%.9g alive=%d",s->core.ticks,s->core.owner_counter,count,s->core.glow,int(Alive(s)));
}
void Destroy(State*s){delete s;}
}
