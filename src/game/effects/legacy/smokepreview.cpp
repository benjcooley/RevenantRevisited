#include "game/effects/legacy/smokepreview.h"
#include "game/effects/effect.h"
#include "game/effects/smokecore.h"
#include "game/effects/smokelegacykeys.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <memory>
#include <vector>
namespace smoke_preview {
struct State {TObjectPtr<TObjectInstance>owner;retail_smoke::State core;MeshHandle mesh=0;double accumulator=0;int64_t rendered=-1;~State(){if(Renderer&&mesh)Renderer->ReleaseMeshAssetRef(mesh);}};
State* Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x224a5cccu):-1;if(type<0)return nullptr;SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
 if(!im||s->owner->ObjId()!=0x224a5cccu||im->NumStates()!=3||!smoke_legacy::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniLength(1),im->GetAniLength(2),im->GetAniFlags(0)))return nullptr;s->owner->OnScreen();
 std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,0,1,v,indices)||v.size()!=4||indices.size()!=6||im->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 s->mesh=Renderer->RegisterMeshAsset(0x534d4f4b45000000ull,v.data(),4,indices.data(),6,im->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);
 s->core.Initialize([](){return GameRand();},StartupPartSysQuality);if(!s->core.alive)s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Smoke",int(StartupVfxNativeDomain));log_info("[smoke] typed_id=224a5ccc quality=%d alive=%d puffs=50 own_legacy_keys=10",StartupPartSysQuality,int(s->core.alive));return s.release();
}
bool Alive(State*s){return s&&s->core.alive&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->Pulse();if(!Alive(s))return;s->owner->NextFrame();s->owner->SetCommandDone(false);s->core.Advance([](){return GameRand();});}}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();auto*im=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 root;const auto&origin=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();
 for(const auto&p:s->core.puffs){S3DAnimObj o={};o.flags=0x48;o.scl={p.size,p.size,p.size};o.pos={p.x,p.y,p.z};if(!im->CalcObjectMatrix(&o,0,0,&root,false))continue;hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);SHelperMeshSubmit draw={};draw.mesh=s->mesh;draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_alpha_source_quantize=StartupVfxNativeDomain;draw.retail_software_projection=StartupVfxNativeDomain;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(draw);}
 const auto&p=s->core.puffs[0];log_info("[smoke-phase] tick=%d center=(%.9g,%.9g) first=(%.9g,%.9g,%.9g) size=%.9g life=%d",s->core.ticks,s->core.centerx,s->core.centery,p.x,p.y,p.z,p.size,p.life);
}
void Destroy(State*s){delete s;}
}
