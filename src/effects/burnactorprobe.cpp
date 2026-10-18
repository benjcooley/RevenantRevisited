#include "burnactorprobe.h"
#include "../character.h"
#include "../3dimage.h"
#include "../logging.h"
#include "../time.h"
#include <memory>
namespace burn_actor_probe {
struct State {TObjectPtr<TCharacter> actor;TObjectPtr<TObjectInstance> effect;};
State* Spawn(const S3DPoint& origin) {
 auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_CHARACTER);const int type=cl?cl->FindObjType("Acolyte"):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_CHARACTER;d.objtype=short(type);d.pos=origin;s->actor.reset(dynamic_cast<TCharacter*>(cl->NewObject(&d)));
 if(!s->actor)return nullptr;log_info("[burn-actor-before-create] state=%d frame=%d face=%d",static_cast<TObjectInstance*>(s->actor.get())->GetState(),s->actor->GetFrame(),IntFacing(s->actor));if(!s->actor->CreateAnimator())return nullptr;
 log_info("[burn-actor-time] render_frame=%lld fraction=%.9g",(long long)TTime::FrameCount(),TTime::LegacyFrameFraction());
 auto*anim=dynamic_cast<T3DAnimator*>(s->actor->GetAnimator());auto*im=dynamic_cast<T3DImagery*>(s->actor->GetImagery());if(!anim || !im)return nullptr;
 log_info("[burn-actor-context] typed=%08x state=%d frame=%d face=%d objects=%d health=%d radius=%d path=%s ani=%s",s->actor->ObjId(),static_cast<TObjectInstance*>(s->actor.get())->GetState(),s->actor->GetFrame(),IntFacing(s->actor),im->NumObjects(),s->actor->Health(),s->actor->Radius(),im->GetResFilename(),im->GetAniName(static_cast<TObjectInstance*>(s->actor.get())->GetState()));
 for(int i=0;i<im->NumObjects();++i){hmm_vec3 pos={},rot={},scale={};bool key=im->GetUninterpolatedAniKey(i,14,0,pos,rot,scale);log_info("[burn-actor-key] object=%d ok=%d p=%.9g,%.9g,%.9g r=%.9g,%.9g,%.9g s=%.9g,%.9g,%.9g",i,int(key),pos.X,pos.Y,pos.Z,rot.X,rot.Y,rot.Z,scale.X,scale.Y,scale.Z);log_info("[burn-actor-parent] object=%d state0=%d state14=%d",i,im->GetObjectParent(i,0),im->GetObjectParent(i,14));hmm_mat4 m;const bool ok=anim->GetObjectMatrix(i,&m);const auto local=anim->GetObject(i)->transform.LocalMatrix();log_info("[burn-actor-local] object=%d matrix=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",i,local.Elements[0][0],local.Elements[0][1],local.Elements[0][2],local.Elements[0][3],local.Elements[1][0],local.Elements[1][1],local.Elements[1][2],local.Elements[1][3],local.Elements[2][0],local.Elements[2][1],local.Elements[2][2],local.Elements[2][3],local.Elements[3][0],local.Elements[3][1],local.Elements[3][2],local.Elements[3][3]);log_info("[burn-actor-pose] object=%d ok=%d matrix=%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",i,int(ok),m.Elements[0][0],m.Elements[0][1],m.Elements[0][2],m.Elements[0][3],m.Elements[1][0],m.Elements[1][1],m.Elements[1][2],m.Elements[1][3],m.Elements[2][0],m.Elements[2][1],m.Elements[2][2],m.Elements[2][3],m.Elements[3][0],m.Elements[3][1],m.Elements[3][2],m.Elements[3][3]);}
 auto* effect_class=TObjectClass::GetClass(OBJCLASS_EFFECT);const int effect_type=effect_class?effect_class->FindObjType(0x55471bffu):-1;
 if(effect_type>=0){SObjectDef def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(effect_type);def.pos=origin;s->effect.reset(effect_class->NewObject(&def));auto* image=s->effect?dynamic_cast<T3DImagery*>(s->effect->GetImagery()):nullptr;if(image){uint64_t hash=1469598103934665603ull;for(size_t j=0;j<image->GetResSize();++j){hash^=reinterpret_cast<const unsigned char*>(image->GetBody())[j];hash*=1099511628211ull;}log_info("[burn-resource] size=%u hash=%016llx objects=%d verts=%d faces=%d materials=%d textures=%d states=%d frames=%d flags=%x",unsigned(image->GetResSize()),(unsigned long long)hash,image->NumObjects(),image->NumVerts(),image->NumFaces(),image->NumMaterials(),image->NumTextures(),image->NumStates(),image->GetAniLength(0),image->GetAniFlags(0));for(int j=0;j<image->NumTextures();++j){S3DTex t={};image->GetTexture(j,&t);const auto&f=t.desc.pixelFormat;log_info("[burn-texture] slot=%d frames=%d dimensions=%ux%u bits=%u masks=%08x,%08x,%08x,%08x",j,t.numframes,t.desc.width,t.desc.height,f.dwRGBBitCount,f.dwRBitMask,f.dwGBitMask,f.dwBBitMask,f.dwRGBAlphaBitMask);}for(int j=0;j<image->NumObjects();++j)log_info("[burn-object] index=%d name=%s verts=%d faces=%d",j,image->GetObjectName(j),image->NumObjVerts(j),image->NumObjFaces(j));}}
 return s.release();
}
void Destroy(State*s){delete s;}
}
