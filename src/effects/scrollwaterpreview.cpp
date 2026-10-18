#include "scrollwaterpreview.h"
#include "../scrollwaterprofile.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <memory>
#include <vector>

namespace scroll_water_preview {
struct State{
 TObjectPtr<TObjectInstance> owner;TSafeRef<> identity;MeshHandle mesh=0;double accumulator=0;uint64_t ticks=0;
 ~State(){if(Renderer&&mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
State* Spawn(const S3DPoint& origin,uint32_t type_id)
{
 if(!Renderer || !scroll_water_profile::Find(type_id))return nullptr;
 auto state=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(type_id):-1;if(type<0)return nullptr;
 SObjectDef definition={};definition.objclass=OBJCLASS_EFFECT;definition.objtype=short(type);definition.pos=origin;
 state->owner.reset(cl->NewObject(&definition));auto* owner=state->owner.get();
 if(!owner || owner->ObjId()!=type_id || !dynamic_cast<TEffect*>(owner))return nullptr;
 state->identity=owner;owner->OnScreen();owner->Animate(false);
 auto* imagery=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
 if(!animator || !scroll_water_profile::Matches(owner,imagery))return nullptr;
 std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(imagery,0,1,vertices,indices) || imagery->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 const uint64_t key=0x5343524f00000000ull|type_id;
 state->mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int32_t(vertices.size()),indices.data(),int32_t(indices.size()),imagery->GetTextureHandle(0));
 if(!state->mesh)return nullptr;Renderer->AddMeshAssetRef(state->mesh);
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT vertex=MODELZ",int(StartupVfxNativeDomain));
 log_info("[scroll-water-preview] actual owner id=%08x map_index=%d vertices=%zu triangles=%zu tags=%d",type_id,owner->GetMapIndex(),vertices.size(),indices.size()/3,imagery->NumTags());
 return state.release();
}
void Advance(State* state,double seconds)
{
 if(!state || !state->owner || !std::isfinite(seconds) || seconds<=0)return;
 state->accumulator+=seconds;
 while(state->accumulator+1e-10>=TTime::LegacyFrameSeconds){state->accumulator-=TTime::LegacyFrameSeconds;auto* owner=state->identity.Get();if(!owner || owner!=state->owner.get())return;owner->NextFrame();if(owner->GetFlags()&OF_KILL){state->owner.reset();return;}owner->Pulse();++state->ticks;}
}
void SubmitWorld(State* state)
{
 if(!state || !Renderer || !state->owner)return;auto* owner=state->identity.Get();if(!owner || owner!=state->owner.get())return;
 auto* imagery=dynamic_cast<T3DImagery*>(owner->GetImagery());if(!scroll_water_profile::Matches(owner,imagery))return;
 owner->Animate(false);auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());if(!animator || !animator->IsObjectEnabled(0))return;
 SHelperMeshSubmit mesh={};mesh.mesh=state->mesh;mesh.retail_lighting=3;mesh.retail_positive_face_cull=true;mesh.retail_software_projection=StartupVfxNativeDomain;
 hmm_mat4 world,root;const auto& position=IntPos(owner);
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,position.x,position.y,position.z,uint8_t(IntFacing(owner)));else root=owner->Transform().Matrix();
 float local[16];BuildStaticObjectMatrix(imagery,0,0,owner->GetFrame(),local);
 hmm_mat4 source_local;for(int row=0;row<4;++row)for(int column=0;column<4;++column)source_local.Elements[column][row]=local[row*4+column];
 MtxMultiply(&world,&source_local,&root);
 for(int row=0;row<4;++row)for(int column=0;column<4;++column)mesh.world[row*4+column]=world.Elements[column][row];
 if(imagery->HasScrollTex())imagery->ScrollTexOffset(0,0,owner->GetFrame(),TTime::LegacyFrameCount(),mesh.uv_offset);
 if(state->ticks==1 || state->ticks==2 || state->ticks==6 || state->ticks==16 || state->ticks==30 || state->ticks==46 || state->ticks==60)
  log_info("[scroll-water-clock] tick=%llu global=%lld owner_frame=%d du=%.9g dv=%.9g",static_cast<unsigned long long>(state->ticks),static_cast<long long>(TTime::LegacyFrameCount()),owner->GetFrame(),mesh.uv_offset[0],mesh.uv_offset[1]);
 Renderer->SubmitHelperMesh(mesh);
}
void Destroy(State* state){delete state;}
}
