#include "irisflarepreview.h"
#include "../irisflarecore.h"
#include "../irisflareprofile.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <cmath>
#include <memory>
#include <vector>
namespace irisflare_preview {
struct State{std::unique_ptr<TObjectInstance>owner;irisflare_core::State core;MeshHandle meshes[2]{};double accumulator=0;int64_t rendered=-1;~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State*Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x120054d1u):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
 if(!im||s->owner->ObjId()!=0x120054d1u||!irisflare_profile::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;s->owner->OnScreen();s->owner->Face(-32);
 for(int i=0;i<2;++i){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,indices)||v.size()!=size_t(i?174:12)||indices.size()!=size_t(i?576:36)||im->GetTextureHandle(i)==kInvalidTexture)return nullptr;s->meshes[i]=Renderer->RegisterMeshAsset(0x49524953464c0000ull+i,v.data(),int(v.size()),indices.data(),int(indices.size()),im->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);}
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT exact IrisFlare",int(StartupVfxNativeDomain));log_info("[irisflare] typed_id=120054d1 flares=5 cylinders=2 early_cylinder=1 reset=201 teleporter_alias=0");return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;s->owner->SetCommandDone(false);s->core.Advance();}}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 root;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();int count=0;
 s->core.Render([&](int object,uint32_t,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&root);SHelperMeshSubmit draw={};draw.mesh=s->meshes[object];draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_software_projection=StartupVfxNativeDomain;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitRetailSourceAlphaAfterFx(draw);++count;});
 if(s->core.ticks<=6||s->core.ticks==19||s->core.ticks==20||s->core.ticks==50||s->core.ticks==200)log_info("[irisflare-phase] tick=%d iterations=%d draws=%d rotation=%.9g cylinder=%.9g",s->core.ticks,s->core.iterations,count,s->core.rotation,s->core.cylinder[0]);
}
void Destroy(State*s){delete s;}
}
