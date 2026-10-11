#include "invisiblepreview.h"
#include "../invisiblecore.h"
#include "../invisibleprofile.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <cmath>
#include <memory>
#include <vector>
namespace invisible_preview{
struct State{std::unique_ptr<TObjectInstance>owner;invisible_core::State core;MeshHandle meshes[2]{};double accumulator=0;int64_t rendered=-1;~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);}};
State*Spawn(const S3DPoint&origin){if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0xad92bc16u):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;if(!im||s->owner->ObjId()!=0xad92bc16u||!invisible_profile::Matches(im))return nullptr;s->owner->Face(0);s->owner->OnScreen();
 for(int i=0;i<2;++i){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,i,i+1,v,indices)||v.size()!=size_t(i?4:12)||indices.size()!=size_t(i?6:36)||im->GetTextureHandle(i)==kInvalidTexture)return nullptr;s->meshes[i]=Renderer->RegisterMeshAsset(0x494e564953000000ull+i,v.data(),int(v.size()),indices.data(),int(indices.size()),im->GetTextureHandle(i));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);}
 log_info("[native-domain] enabled=%d camera_z=0 exact Invisible null-invoker",int(StartupVfxNativeDomain));log_info("[invisible] typed_id=ad92bc16 particles=70 rings=6 finite=500 show_after=430 null_ring_owner_translation_twice=1");return s.release();}
bool Alive(State*s){return s&&s->owner&&s->core.Alive()&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;s->core.Advance();}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();const auto&p=s->owner->Pos();hmm_mat4 root;if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();int count=0;
 s->core.Render([&](int object,const hmm_mat4&local){hmm_mat4 world=local;if(object==0)MtxMultiply(&world,const_cast<hmm_mat4*>(&local),&root); // Original rings POS3 include owner position; sparks ABSPOS bypass owner root.
  SHelperMeshSubmit draw={};draw.mesh=s->meshes[object];draw.retail_lighting=object?1:3;draw.retail_software_projection=StartupVfxNativeDomain;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];if(object){draw.additive_blend=true;draw.retail_additive_cull_none=true;Renderer->SubmitGoldFlareAfterFx(draw);}else{draw.retail_source_alpha=true;Renderer->SubmitRetailSourceAlphaAfterFx(draw);}++count;
 },float(p.x),float(p.y),float(p.z));if(s->core.life<=6||s->core.life==35||s->core.life==100||s->core.life==431||s->core.life==459||s->core.life>=499)log_info("[invisible-phase] tick=%d state=%d draws=%d rng=%u",s->core.life,s->core.state,count,s->core.rng);
}
void Destroy(State*s){delete s;}
}
