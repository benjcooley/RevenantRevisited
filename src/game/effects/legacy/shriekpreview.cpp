#include "game/effects/legacy/shriekpreview.h"
#include "game/effects/effect.h"
#include "game/effects/shriekcore.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
namespace shriek_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 retail_shriek::State core;
 MeshHandle mesh[4]{};
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer)for(auto m:mesh)if(m)Renderer->ReleaseMeshAssetRef(m);}
};
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bc12u):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0xad92bc12u)return nullptr;s->owner->OnScreen();
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=4 || img->NumStates()!=1 || img->GetAniLength(0)!=100 || img->NumTextures()!=4 || img->NumMaterials()!=4 || img->NumTags()!=0)return nullptr;
 for(int i=0;i<4;++i){const char* name=i==0?"box01":i==1?"box02":i==2?"box03":"box04";if(std::strcmp(img->GetObjectName(i),name) || img->GetObjectParent(i,0)!=-1)return nullptr;
  S3DTex texture={};img->GetTexture(i,&texture);const auto& pf=texture.desc.pixelFormat;
  if(texture.numframes!=1 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf00 || pf.dwGBitMask!=0xf0 || pf.dwBBitMask!=0xf || pf.dwRGBAlphaBitMask!=0xf000 || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,i,i+1,verts,indices) || verts.size()!=12 || indices.size()!=36)return nullptr;
  s->mesh[i]=Renderer->RegisterMeshAsset(0x5348524b00000000ull+i,verts.data(),int(verts.size()),indices.data(),int(indices.size()),img->GetTextureHandle(i));if(!s->mesh[i])return nullptr;Renderer->AddMeshAssetRef(s->mesh[i]);
 }
 s->core.Initialize([](int a,int b){return random(a,b);});
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Shriek null-spell",int(StartupVfxNativeDomain));log_info("[shriek] exact authored meshes=4 rings=8 null_spell=1 counter=0 cutoff=45 source_term_quantize=native_domain_only");return s.release();
}
bool Alive(State* s){return s && s->core.alive && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds){
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;++s->ticks;s->core.Advance();if(!s->core.alive)s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);}
}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();if(s->core.counter<=15)return;
 auto* img=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 root;const auto& origin=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();int draws=0;
 // Original ring-index order, cycling through the four authored meshes.
 for(int i=0;i<8;++i){const int part=i%4;S3DAnimObj o={};o.objnum=part;o.flags=0x48;o.scl={s->core.scale[i],s->core.scale[i],s->core.scale[i]};o.pos={-s->core.scale[i],-s->core.scale[i]-s->core.position[i],0};
  if(!img->CalcObjectMatrix(&o,0,s->owner->GetFrame(),&root,false))continue;hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);
  SHelperMeshSubmit draw={};draw.mesh=s->mesh[part];draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_alpha_source_quantize=StartupVfxNativeDomain;draw.retail_software_projection=StartupVfxNativeDomain;
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(draw);++draws;
 }
 log_info("[shriek-phase] tick=%d counter=%d draws=%d first_scale=%.9g first_position=%.9g",s->ticks,s->core.counter,draws,s->core.scale[0],s->core.position[0]);
}
void Destroy(State* s){delete s;}
}
