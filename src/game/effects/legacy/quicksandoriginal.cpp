#include "game/effects/legacy/quicksandoriginal.h"
#include "game/effects/effect.h"
#include "game/imagery/3dimage.h"
#include "game/imagery/meshextract.h"
#include "game/effects/quicksandcore.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <cmath>
#include <cstring>
#include <memory>
namespace quicksand_original {
struct State {
 TObjectPtr<TObjectInstance> owner;
 retail_quicksand::State core;
 MeshHandle meshes[4]{};
 S3DPoint source{};
 double accumulated=0;
 ~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}
};
State* Spawn(const S3DPoint& source){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xbabefaceu):-1;if(type<0)return nullptr;
 SObjectState d{};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=source;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0xbabefaceu)return nullptr;SetIntPos(s->owner, source);s->source=source;s->owner->OnScreen();
 auto* im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!im || im->NumStates()!=1 || im->GetAniLength(0)!=3 || im->GetAniFlags(0)!=0x2001 || im->NumObjects()!=4 || im->NumMaterials()!=4 || im->NumTextures()!=4 || im->NumTags()!=0)return nullptr;
 const char* names[]={"box01","cylinder01","cone01","box02"};const int vertices[]={4,26,34,4},faces[]={2,24,32,2},sizes[]={256,64,64,32};
 for(int i=0;i<4;++i){
  if(std::strcmp(im->GetObjectName(i),names[i]) || im->GetObjectParent(i,0)!=-1)return nullptr;
  S3DTex tex{};im->GetTexture(i,&tex);const auto& f=tex.desc.pixelFormat;
  if(tex.numframes!=1 || tex.desc.width!=uint32_t(sizes[i]) || tex.desc.height!=uint32_t(sizes[i]) || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf00 || f.dwGBitMask!=0xf0 || f.dwBBitMask!=0xf || f.dwRGBAlphaBitMask!=0xf000 || im->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> v;std::vector<uint16_t> ix;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,ix) || int(v.size())!=vertices[i] || int(ix.size())!=faces[i]*3)return nullptr;
  s->meshes[i]=Renderer->RegisterMeshAsset(0x5153414e44000000ull|uint64_t(i),v.data(),int(v.size()),ix.data(),int(ix.size()),im->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 }
 log_info("[quicksand-original] id=babeface configured_target_offset=96,0,0 source=%d,%d,%d kill_after55=1 actor_damage_port=0",source.x,source.y,source.z);
 log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return s.release();
}
void Advance(State* s,double dt){if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulated+=dt;while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds && s->core.alive){s->accumulated-=TTime::LegacyFrameSeconds;s->core.Pulse();s->owner->SetFrame(s->core.frame);if(!s->core.alive)s->owner->SetFlag(OF_KILL);}}
bool IsAlive(const State* s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void SubmitWorld(State* s){
 if(!IsAlive(s) || !Renderer)return;auto* im=static_cast<T3DImagery*>(s->owner->GetImagery());
 // Actual owner stores integer FIX_Z_VALUE(target.z); Render applies this
 // raw absolute translation, without a second nonlinear owner-Z conversion.
 hmm_vec3 target={float(s->source.x+96),float(s->source.y),float(int(RetailSoftwareOwnerZ(s->source.z)))};
 for(int i=0;i<4;++i){hmm_vec3 p{},r{},scale{};if(!im->GetUninterpolatedAniKey(i,0,s->core.frame,p,r,scale,true))continue;hmm_mat4 world;retail_quicksand::Matrix(world,p,r,scale);MtxTranslate(&world,&target);
  SHelperMeshSubmit q{};q.mesh=s->meshes[i];q.retail_lighting=3;q.retail_source_alpha=true;q.retail_software_projection=StartupVfxNativeDomain;
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(q);
 }
 log_info("[quicksand-phase] tick=%d frame=%d target=%g,%g,%g alive=%d",s->core.ticks,s->core.frame,target.X,target.Y,target.Z,int(IsAlive(s)));
}
void Destroy(State* s){delete s;}
}
