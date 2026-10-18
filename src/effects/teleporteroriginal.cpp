#include "teleporteroriginal.h"
#include "../teleportercore.h"
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
namespace teleporter_original {
struct State{std::unique_ptr<TObjectInstance>owner;retail_teleporter::State core;MeshHandle meshes[1]{};double accumulator=0;int64_t rendered=-1;~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State*Spawn(const S3DPoint&origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x369a5d2eu):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
 if(!im||s->owner->ObjId()!=0x369a5d2eu||!irisflare_profile::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;s->owner->OnScreen();s->owner->Face(-32);
 std::vector<SMeshVertex>v;std::vector<uint16_t>indices;
 if(!ExtractSubMeshTextureSlot(im,1,2,v,indices)||v.size()!=174||indices.size()!=576||im->GetTextureHandle(1)==kInvalidTexture)return nullptr;
 s->meshes[0]=Renderer->RegisterMeshAsset(0x54454c45504f5254ull,v.data(),int(v.size()),indices.data(),int(indices.size()),im->GetTextureHandle(1));
 if(!s->meshes[0])return nullptr;Renderer->AddMeshAssetRef(s->meshes[0]);
 log_info("[native-domain] enabled=%d camera_z=0 exact Teleporter null-spell",int(StartupVfxNativeDomain));log_info("[teleporter-original] typed_id=369a5d2e null_spell=1 cylinders=5 kill51=1 actor_teleport_port=0");return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(Alive(s)&&s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->SetCommandDone(false);s->core.Pulse();if(!s->core.alive)s->owner->SetFlag(OF_KILL);}}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 root;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();int count=0;
 s->core.Render([&](int,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&root);SHelperMeshSubmit draw={};draw.mesh=s->meshes[0];draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_software_projection=StartupVfxNativeDomain;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitRetailSourceAlphaAfterFx(draw);++count;});
 log_info("[teleporter-phase] tick=%d life=%d phase=%d draws=%d rotation=%.9g alive=%d",s->core.ticks,s->core.life,s->core.phase,count,s->core.rotation,int(Alive(s)));
}
void Destroy(State*s){delete s;}
}
