#include "paralizepreview.h"
#include "../effect.h"
#include "../paralizecore.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../logging.h"
#include "../testconfig.h"
#include "../time.h"
#include <memory>
#include <cstring>
#include <vector>
namespace paralize_preview {
struct State {
 std::unique_ptr<TObjectInstance> owner;
 paralize_core::State core;
 paralize_core::Vec origin;
 MeshHandle mesh=0;
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(mesh && Renderer)Renderer->ReleaseMeshAssetRef(mesh);}
};
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bc14u):-1;if(type<0)return nullptr;
 SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=origin;s->owner.reset(cl->NewObject(&def));
 if(!s->owner || s->owner->ObjId()!=0xad92bc14u)return nullptr;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumStates()!=1 || img->GetAniLength(0)!=10 || img->GetAniFlags(0)!=0x2001 || img->NumObjects()!=6 || img->NumTextures()!=6 || img->NumMaterials()!=6 || img->NumTags()!=0)return nullptr;
 const char* names[]={"lightface0","sphere01","lightface1","sphere02","sphere03","lightface2"};for(int i=0;i<6;++i)if(std::strcmp(img->GetObjectName(i),names[i]) || img->GetObjectParent(i,0)!=-1)return nullptr;
 S3DTex tex={};img->GetTexture(0,&tex);const auto& pf=tex.desc.pixelFormat;
 if(tex.numframes!=1 || tex.desc.width!=128 || tex.desc.height!=128 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x7e0 || pf.dwBBitMask!=0x1f || pf.dwRGBAlphaBitMask || img->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,0,1,vertices,indices) || vertices.size()!=4 || indices.size()!=6)return nullptr;
 s->mesh=Renderer->RegisterMeshAsset(0x504152414c495a31ull,vertices.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);
 hmm_mat4 owner;BuildRetailSoftwareOwner(owner,origin.x,origin.y,origin.z,0);s->origin={owner.Elements[3][0],owner.Elements[3][1],owner.Elements[3][2]};
 log_info("[native-domain] enabled=%d camera_z=0 owner=absolute_SCL1_POS2 Paralize1",int(StartupVfxNativeDomain));
 log_info("[paralize] typed_id=ad92bc14 spell=Physical_Paralysis invoker=null targets=0 native_domain=%d",int(StartupVfxNativeDomain));return s.release();
}
bool Alive(State* s){return s && s->core.alive;}
void Advance(State* s,double dt)
{
 if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->core.Tick([](int a,int b){return random(a,b);},s->origin);++s->ticks;
  const auto& p=s->core.position[0];log_info("[paralize-phase] tick=%d variant=%d phase=%d count=%d scale=%.9g knot=%.9g first=(%.9g,%.9g,%.9g) alive=%d",s->ticks,s->core.variant,s->core.phase,s->core.count,s->core.scale,s->core.knot,p.x,p.y,p.z,int(s->core.alive));}
}
void SubmitWorld(State* s)
{
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 // Original scale gate rejects <=1e-5 before RenderObject. No actor means the
 // three sphere passes have zero targets; they must not be invented here.
 if(s->core.scale<=1e-5f)return;
 for(const auto& p:s->core.position){SHelperMeshSubmit item={};item.mesh=s->mesh;
  item.world[15]=1;item.world[0]=item.world[5]=item.world[10]=s->core.scale;item.world[3]=p.x;item.world[7]=p.y;item.world[11]=p.z;
  item.additive_blend=true;item.retail_lighting=1;item.retail_positive_face_cull=false;item.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitGoldFlareAfterFx(item);}
}
void Destroy(State* s){delete s;}
}
