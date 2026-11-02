#include "game/effects/legacy/genericstaticpreview.h"
#include "game/effects/genericstaticprofile.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "engine/render/retailmeshlighting.h"
#include "engine/render/renderer.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <memory>
#include <vector>
namespace generic_static_preview {
struct State {
 struct Part{int object;MeshHandle mesh;int lighting;};
 TObjectPtr<TObjectInstance> owner;TSafeRef<> identity;std::vector<Part>parts;double accumulated=0;
 ~State(){if(Renderer)for(const auto&p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
State* Spawn(const S3DPoint& origin,uint32_t id){
 if(!Renderer||!generic_static_profile::Find(id))return nullptr;auto state=std::make_unique<State>();auto*cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(id):-1;if(type<0)return nullptr;
 SObjectState def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=origin;state->owner.reset(cls->NewObject(&def));auto*owner=state->owner.get();auto*im=owner?dynamic_cast<T3DImagery*>(owner->GetImagery()):nullptr;
 if(!generic_static_profile::Matches(owner,im)||!dynamic_cast<TEffect*>(owner)||owner->GetMapIndex()<=0)return nullptr;state->identity=owner;owner->OnScreen();owner->Animate(false);if(!dynamic_cast<T3DAnimator*>(owner->GetAnimator()))return nullptr;
 for(int object=0;object<im->NumObjects();++object)for(int slot=1;slot<=im->NumTextures();++slot){
  std::vector<SMeshVertex>vertices;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,object,slot,vertices,indices)||indices.empty())continue;S3DTex texture={};im->GetTexture(slot-1,&texture);if(texture.htexture==kInvalidTexture)return nullptr;
  const uint64_t key=0x4753540000000000ull^(uint64_t(id)<<8)^(uint64_t(object)<<4)^uint64_t(slot);auto mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),indices.data(),int32_t(indices.size()),texture.htexture);if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);state->parts.push_back({object,mesh,RetailMeshTextureLighting(texture.desc.pixelFormat)});
 }
 if(state->parts.size()!=size_t(generic_static_profile::Find(id)->objects))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT vertex=MODELZ",int(StartupVfxNativeDomain));
 log_info("[generic-static-preview] actual owner id=%08x state=0 map_index=%d meshes=%zu exact_profile=1 native_helper_depth_write=0",id,owner->GetMapIndex(),state->parts.size());return state.release();
}
void Advance(State*state,double seconds){
 if(!state||!state->owner||!std::isfinite(seconds)||seconds<=0)return;state->accumulated+=seconds;
 while(state->accumulated+1e-10>=TTime::LegacyFrameSeconds){state->accumulated-=TTime::LegacyFrameSeconds;auto*owner=state->identity.Get();if(!owner||owner!=state->owner.get()||!generic_static_profile::Matches(owner,dynamic_cast<T3DImagery*>(owner->GetImagery())))return;owner->NextFrame();owner->Pulse();
  if(owner->ObjId()==0xacabfadau && (owner->GetFrame()==99 || owner->GetFrame()==0))
   log_info("[icedsparks-owner] frame=%d flags=%08x",owner->GetFrame(),owner->GetFlags());
 }
}
void SubmitWorld(State*state,EFxDebugMode mode){
 if(!state||!Renderer||!state->owner||mode!=EFxDebugMode::Normal)return;auto*owner=state->identity.Get();auto*im=owner?dynamic_cast<T3DImagery*>(owner->GetImagery()):nullptr;if(!generic_static_profile::Matches(owner,im))return;owner->Animate(false);auto*animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());if(!animator)return;
 for(const auto&part:state->parts){if(!animator->IsObjectEnabled(part.object))continue;
  if(StartupVfxNativeDomain){hmm_mat4 root,world;const auto&p=IntPos(owner);BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(owner)));float local[16];BuildStaticObjectMatrix(im,part.object,0,owner->GetFrame(),local);hmm_mat4 matrix;for(int row=0;row<4;++row)for(int col=0;col<4;++col)matrix.Elements[col][row]=local[row*4+col];MtxMultiply(&world,&matrix,&root);
   SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;mesh.retail_lighting=part.lighting;mesh.retail_software_projection=true;mesh.retail_positive_face_cull=true;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(mesh);
  }else{auto*bone=animator->GetObject(part.object);if(!bone)continue;SMeshSubmit mesh={};mesh.mesh=part.mesh;mesh.retail_lighting=part.lighting;mesh.retail_positive_face_cull=true;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=bone->transform.Matrix().Elements[col][row];mesh.tint[0]=mesh.tint[1]=mesh.tint[2]=mesh.tint[3]=1;Renderer->SubmitMesh(mesh);}
 }
}
void Destroy(State*state){delete state;}
}
