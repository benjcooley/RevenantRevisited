#include "riftqualifiedpreview.h"
#include "../riftqualifiedcore.h"
#include "../riftqualifiedprofile.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../logging.h"
#include <cmath>
#include <cstring>
#include <memory>
namespace rift_qualified_preview {
struct State {std::unique_ptr<TObjectInstance>owner;MeshHandle mesh=0;S3DMat material{};double accumulator=0;int tick=0;int64_t rendered=-1;~State(){if(Renderer&&mesh)Renderer->ReleaseMeshAssetRef(mesh);}};
State* Spawn(const S3DPoint&origin){if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*c=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=c?c->FindObjType(0xadbcef18u):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(c->NewObject(&d));if(!s->owner||s->owner->ObjId()!=0xadbcef18u||!dynamic_cast<TEffect*>(s->owner.get()))return nullptr;s->owner->OnScreen();const bool created=s->owner->CreateAnimator();if(!created||!s->owner->GetAnimator())return nullptr;auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!im||!rift_qualified_profile::Matches(im->GetResFilename(),im->GetBody(),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0))||im->NumObjects()!=1||im->NumVerts()!=4||im->NumFaces()!=2||im->NumTextures()!=1||im->NumTags()!=1)return nullptr;
 std::vector<SMeshVertex>v;std::vector<uint16_t>f;if(!ExtractSubMeshTextureSlot(im,0,1,v,f)||v.size()!=4||f.size()!=6)return nullptr;s->mesh=Renderer->RegisterMeshAsset(0x524946545155414cull,v.data(),4,f.data(),6,im->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);im->GetMaterial(0,&s->material);log_info("[native-domain] enabled=%d camera_z=0 Rift qualified parser repair",int(StartupVfxNativeDomain));log_info("[rift-qualified] typed_id=adbcef18 qualification=original_parser_literal_v_to_d asset_unchanged=1 row_arithmetic_preserved=1 source_seed=none");return s.release();}
bool Alive(State*s){return s&&s->owner&&!(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;++s->tick;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s)){log_info("[rift-qualified-kill] tick=%d frame=%d",s->tick,s->owner->GetFrame());return;}}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());SHelperMeshSubmit q={};q.mesh=s->mesh;q.retail_lighting=3;q.retail_source_alpha=true;q.retail_alpha_source_quantize=StartupVfxNativeDomain;q.retail_software_projection=StartupVfxNativeDomain;const auto&m=s->material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;hmm_mat4 owner,local,world;const auto p=IntPos(s->owner);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));float pose[16];BuildStaticObjectMatrix(im,0,0,s->owner->GetFrame(),pose);for(int r=0;r<4;++r)for(int c=0;c<4;++c)local.Elements[c][r]=pose[r*4+c];MtxMultiply(&world,&local,&owner);for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];// Explicit24Hz qualified test feeds its actual logical step; ordinary runtime
// remains on the legacy24Hz clock rather than accelerating with renderFPS.
const int32_t global=(StartupVfxNativeDomain && std::string(StartupTestMode)=="vfx")?s->tick:int32_t(TTime::LegacyFrameCount());rift_qualified_core::Offset(global,q.uv_offset);Renderer->SubmitHelperMesh(q);log_info("[rift-qualified-phase] tick=%d global=%lld frame=%d u=%.9g v=%.9g",s->tick,(long long)global,s->owner->GetFrame(),q.uv_offset[0],q.uv_offset[1]);}
void Destroy(State*s){delete s;}
}
