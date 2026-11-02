#include "game/effects/legacy/essencedrainpreview.h"
#include "game/effects/essencedrainprofile.h"
#include "game/effects/effect.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <cmath>
#include <memory>
#include <vector>
namespace essencedrain_preview{
struct State{struct Part{int object;MeshHandle mesh;};TObjectPtr<TObjectInstance>owner;std::vector<Part>parts;double accumulator=0;int renders=0;int64_t rendered=-1;~State(){if(Renderer)for(auto&p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}};
State*Spawn(const S3DPoint&origin){if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0xdefeca7eu):-1;if(type<0)return nullptr;SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;if(!im||s->owner->ObjId()!=0xdefeca7eu||!essencedrain_profile::Matches(im))return nullptr;s->owner->Face(0);s->owner->OnScreen();
 for(int slot=1;slot<=4;++slot)for(int object=0;object<29;++object){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,object,slot,v,indices)||indices.empty())continue;auto tex=im->GetTextureHandle(slot-1);if(tex==kInvalidTexture)return nullptr;auto mesh=Renderer->RegisterMeshAsset(0x455353454e434500ull+object,v.data(),int(v.size()),indices.data(),int(indices.size()),tex);if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);s->parts.push_back({object,mesh});}if(s->parts.size()!=29)return nullptr;log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT exact EssenceDrain",int(StartupVfxNativeDomain));log_info("[essencedrain] typed_id=defeca7e meshes=29 keyed=3 zero_key_visible=26 real_finite=200 blend_tag40_notactivation=1");return s.release();}
bool Alive(State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();++s->renders;auto*im=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 root;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();int draws=0;
 for(const auto&part:s->parts){hmm_mat4 local;MtxClear(&local);const auto&profile=essencedrain_profile::objects[part.object];if(profile.keys){hmm_vec3 p{},r{},scale{};if(!im->GetUninterpolatedAniKey(part.object,0,s->owner->GetFrame(),p,r,scale,true)||scale.X<=1e-5f||scale.Y<=1e-5f||scale.Z<=1e-5f)continue;MtxRotateX(&local,r.X);MtxRotateY(&local,r.Y);MtxRotateZ(&local,r.Z);MtxScale(&local,&scale);MtxTranslate(&local,&p);} // Original zero-key roots retain identity, independently of cached zero scale.
  hmm_mat4 world;MtxMultiply(&world,&local,&root);SHelperMeshSubmit draw={};draw.mesh=part.mesh;draw.retail_lighting=1;draw.additive_blend=true;draw.retail_additive_cull_none=true;draw.retail_software_projection=StartupVfxNativeDomain;if(profile.keys)draw.uv_offset[0]=float(s->renders)*-.01f;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitGoldFlareAfterFx(draw);++draws;
 }if(s->renders<=6||s->renders==40||s->renders==100||s->renders>=198)log_info("[essencedrain-phase] render=%d frame=%d draws=%d",s->renders,s->owner->GetFrame(),draws);
}
void Destroy(State*s){delete s;}
}
