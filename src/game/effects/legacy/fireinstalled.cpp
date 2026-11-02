#include "game/effects/legacy/fireinstalled.h"
#include "game/effects/effect.h"
#include "game/effects/firelegacykeys.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/logging.h"
#include <memory>
#include <cmath>
#include <cstring>
namespace fire_installed {
struct State {TObjectPtr<TObjectInstance>owner;MeshHandle mesh=0;S3DMat material{};double accumulated=0;int tick=0;int64_t rendered=-1;~State(){if(Renderer&&mesh)Renderer->ReleaseMeshAssetRef(mesh);}};
State* Spawn(const S3DPoint& origin,uint8_t facing){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x119f01e3u):-1;if(type<0)return nullptr;
 SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;d.rotatez=facing;s->owner.reset(cl->NewObject(&d));auto*o=s->owner.get();auto*im=o?dynamic_cast<T3DImagery*>(o->GetImagery()):nullptr;
 if(!o||o->ObjId()!=0x119f01e3u||!dynamic_cast<TEffect*>(o)||!im||!fire_legacy::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(static_cast<void*>(im->GetBody())),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;
 o->OnScreen();o->Animate(false);if(!dynamic_cast<T3DAnimator*>(o->GetAnimator()))return nullptr;
 std::vector<SMeshVertex>v;std::vector<uint16_t>f;if(!ExtractSubMeshTextureSlot(im,0,1,v,f)||v.size()!=4||f.size()!=6)return nullptr;
 S3DTex tex={};im->GetTexture(0,&tex);if(tex.numframes!=33||tex.desc.width!=64||tex.desc.height!=64)return nullptr;
 s->mesh=Renderer->RegisterMeshAsset(0x46495245494e5354ull,v.data(),4,f.data(),6,im->GetTextureHandle(0));if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);im->GetMaterial(0,&s->material);
 log_info("[native-domain] enabled=%d camera_z=0 owner=native_matrix",int(StartupVfxNativeDomain));log_info("[fire-installed] id=119f01e3 facing=%d states=1 frames=1 textureframes=33 texture_selector=0 persistent=1",int(facing));return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double dt){if(!Alive(s)||!std::isfinite(dt)||dt<=0)return;s->accumulated+=dt;while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds){s->accumulated-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();++s->tick;}}
void SubmitWorld(State*s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();auto*o=s->owner.get();o->Animate(false);auto*im=static_cast<T3DImagery*>(o->GetImagery());
 float values[16];BuildStaticObjectMatrix(im,0,0,o->GetFrame(),values);hmm_mat4 local,owner,world;for(int r=0;r<4;++r)for(int c=0;c<4;++c)local.Elements[c][r]=values[r*4+c];
 const auto&p=IntPos(o);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(o)));else owner=o->Transform().Matrix();MtxMultiply(&world,&local,&owner);
 SHelperMeshSubmit q={};q.mesh=s->mesh;q.retail_lighting=3;q.retail_source_alpha=true;q.retail_alpha_source_quantize=StartupVfxNativeDomain;q.retail_software_projection=StartupVfxNativeDomain;q.retail_software_pixel_grid=StartupVfxNativeDomain;const auto&m=s->material.matdesc;
 std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];Renderer->SubmitHelperMesh(q);
 log_info("[fire-installed-phase] tick=%d frame=%d face=%d rng=%llu matrix=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",s->tick,o->GetFrame(),IntFacing(o),(unsigned long long)RandomDraws(),world.Elements[0][0],world.Elements[0][1],world.Elements[0][2],world.Elements[0][3],world.Elements[1][0],world.Elements[1][1],world.Elements[1][2],world.Elements[1][3],world.Elements[2][0],world.Elements[2][1],world.Elements[2][2],world.Elements[2][3],world.Elements[3][0],world.Elements[3][1],world.Elements[3][2],world.Elements[3][3]);
}
void Destroy(State*s){delete s;}
}
