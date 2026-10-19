#include "game/effects/legacy/bloodpreview.h"
#include "game/effects/bloodcore.h"
#include "game/effects/bloodprofile.h"
#include "game/effects/effect.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/world/mappane.h"
#include "game/world/3dscene.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
namespace blood_preview{
int DrawLiveRandom(int a,int b){return random(a,b);}
struct State{explicit State(bool fixture):fixture_rng(fixture){}TObjectPtr<TObjectInstance>owner;blood_core::State core{2};bool fixture_rng;MeshHandle meshes[8]{};double accumulator=0;int ambient=0;int64_t rendered=-1;~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State*Spawn(const S3DPoint&origin){if(!Renderer)return nullptr;auto s=std::make_unique<State>(std::strcmp(StartupTestMode,"vfx")==0);auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0xddc4042eu):-1;if(type<0)return nullptr;SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;if(im)log_info("[blood-profile] path=%s size=%u hash=%016llx states=%d frames=%d flags=%x",im->GetResFilename(),unsigned(im->GetResSize()),(unsigned long long)blood_profile::Hash(im->GetBody(),im->GetResSize()),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0));if(!im||s->owner->ObjId()!=0xddc4042eu||!blood_profile::Matches(im->GetResFilename(),im->GetBody(),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;s->core=blood_core::State(2,{},4,s->fixture_rng?nullptr:DrawLiveRandom);s->owner->Face(0);s->owner->OnScreen();if(s->fixture_rng)MapPane.SetAmbientLight(StartupSceneAmbientSet?StartupSceneAmbient[0]:32);s->ambient=MapPane.GetAmbientLight();
 for(int i=0;i<8;++i){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;const int texture=i<4?0:1;if(!ExtractSubMeshTextureSlot(im,i,texture+1,v,indices)||v.size()!=4||indices.size()!=6||im->GetTextureHandle(texture)==kInvalidTexture)return nullptr;s->meshes[i]=Renderer->RegisterMeshAsset(0x424c4f4f44000000ull+i,v.data(),4,indices.data(),6,im->GetTextureHandle(texture));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);}
 log_info("[native-domain] enabled=%d camera_z=0 exactBlood publicspray",int(StartupVfxNativeDomain));log_info("[blood] typed_id=ddc4042e rng_scope=%s detail=4 params=40,224,3,5,5,1 map_ambient=%d actor_context=declared_public_spray map_insertion_open=1",s->fixture_rng?"fixture_init_seed2":"live_global_no_reseed",s->ambient);return s.release();}
bool Alive(State*s){return s&&s->owner&&!s->core.done&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;s->core.Advance();if(s->core.done){s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);return;}}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();s->ambient=MapPane.GetAmbientLight();const auto&p=IntPos(s->owner);hmm_mat4 root;if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();int count=0;
 s->core.Render([&](int slot,int object,int pass,const hmm_mat4&local){hmm_mat4 world;MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&root);SHelperMeshSubmit draw={};draw.mesh=s->meshes[object];draw.retail_lighting=pass?6:3;draw.retail_source_alpha=!pass;draw.additive_blend=bool(pass);draw.retail_additive_cull_none=bool(pass);draw.retail_software_projection=StartupVfxNativeDomain;
  const auto&v=s->core.slots[slot].position;int ids[3]={-1,-1,-1};Scene3D.GetClosestLights(int(float(p.x)+v.X),int(float(p.y)+v.Y),int(float(p.z)+v.Z),ids[0],ids[1],ids[2]);float brightness=0;int lights=0;for(int i=0;i<3&&ids[i]>=0;++i){float b=0;Scene3D.GetLightBrightness(ids[i],int(float(p.x)+v.X),int(float(p.y)+v.Y),int(float(p.z)+v.Z),b);brightness+=b;++lights;}const auto color=blood_core::State::PackedColor(s->ambient,brightness,lights);draw.diffuse[0]=float((color>>19)&31);draw.diffuse[1]=float((color>>11)&31);draw.diffuse[2]=float((color>>3)&31);draw.diffuse[3]=float((color>>27)&31);for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(draw);++count;
 });if(s->core.ticks<=6||s->core.ticks==16||s->core.ticks==30||s->core.ticks>=58)log_info("[blood-phase] tick=%d draws=%d rng=%u ambient=%d color=%08x",s->core.ticks,count,s->core.rng,s->ambient,blood_core::State::PackedColor(s->ambient));}
void Destroy(State*s){delete s;}
}
