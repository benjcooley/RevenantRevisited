#include "game/effects/legacy/ribbonpreview.h"
#include "game/effects/legacy/ribbonstate.h"
#include "game/effects/effect.h"
#include "game/effects/ribbonlegacykeys.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <cmath>
#include <memory>
#include <vector>
namespace ribbon_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;ribbon_state::State simulation;MeshHandle spark[5]{},tube=0;double accumulator=0;int64_t rendered=-1;
 ~State(){if(Renderer){for(auto m:spark)if(m)Renderer->ReleaseMeshAssetRef(m);if(tube)Renderer->ReleaseMeshAssetRef(tube);}}
};
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(0x562c7439u):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0x562c7439u)return nullptr;s->owner->OnScreen();auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!im || !ribbon_legacy::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;
 if(im->NumObjects()!=2 || im->NumMaterials()!=2 || im->NumTextures()!=2 || im->NumTags())return nullptr;
 S3DTex t={};im->GetTexture(0,&t);if(t.numframes!=5 || !t.framehtexs)return nullptr;std::vector<SMeshVertex> v;std::vector<uint16_t>indices;
 if(!ExtractSubMeshTextureSlot(im,0,1,v,indices) || v.size()!=12 || indices.size()!=36)return nullptr;
 for(int f=0;f<5;++f){if(t.framehtexs[f]==kInvalidTexture)return nullptr;s->spark[f]=Renderer->RegisterMeshAsset(0x524942424f4e0000ull+uint64_t(f),v.data(),12,indices.data(),36,t.framehtexs[f]);if(!s->spark[f])return nullptr;Renderer->AddMeshAssetRef(s->spark[f]);}
 if(!ExtractSubMeshTextureSlot(im,1,2,v,indices) || v.size()!=174 || indices.size()!=576 || im->GetTextureHandle(1)==kInvalidTexture)return nullptr;s->tube=Renderer->RegisterMeshAsset(0x524942424f4e0010ull,v.data(),174,indices.data(),576,im->GetTextureHandle(1));if(!s->tube)return nullptr;Renderer->AddMeshAssetRef(s->tube);s->simulation.Initialize([](int a,int b){return random(a,b);});
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT exact baseRibbon",int(StartupVfxNativeDomain));log_info("[ribbon] typed_id=562c7439 sparks=30 tubes=3 texture_frames=5 grow=20 nullspell_target=0");return s.release();
}
bool Alive(State*s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s) || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;s->simulation.Advance([](int a,int b){return random(a,b);});}}
static void Submit(State&s,MeshHandle mesh,int object,uint32_t flags,const hmm_vec3&p,const hmm_vec3&r,const hmm_vec3&scale)
{
 auto*im=static_cast<T3DImagery*>(s.owner->GetImagery());S3DAnimObj o={};o.objnum=o.animtrack=object;o.flags=int32_t(flags);o.pos=p;o.rot=r;o.scl=scale;hmm_mat4 root;
 const auto&pos=IntPos(s.owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,pos.x,pos.y,pos.z,uint8_t(IntFacing(s.owner)));else root=s.owner->Transform().Matrix();if(!im->CalcObjectMatrix(&o,0,0,&root,false))return;
 hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);
 SHelperMeshSubmit draw={};draw.mesh=mesh;for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitRetailSourceAlphaAfterFx(draw);
}
void SubmitWorld(State*s)
{
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();int count=0;
 for(const auto&p:s->simulation.sparks)if(p.frame>0 && p.scale>0){Submit(*s,s->spark[p.frame%5],0,OBJ3D_ROT1|OBJ3D_SCL2|OBJ3D_POS3,{p.p[0],p.p[1],p.p[2]},{-float(M_PI)/2,0,-float(M_PI)/4},{p.scale,p.scale,p.scale});++count;}
 float floor=s->simulation.FloorScale([](int a,int b){return random(a,b);});Submit(*s,s->spark[s->simulation.ticks%5],0,OBJ3D_SCL1,{0,0,0},{0,0,0},{floor,floor,floor});
 for(float r:s->simulation.rotations)Submit(*s,s->tube,1,OBJ3D_ROT1|OBJ3D_SCL2|OBJ3D_POS3,{0,0,0},{0,0,r},{s->simulation.scale,s->simulation.scale,s->simulation.scale*2});
 if(s->simulation.ticks<=3 || s->simulation.ticks==6 || s->simulation.ticks==16 || s->simulation.ticks==20 || s->simulation.ticks==30 || s->simulation.ticks==60 || s->simulation.ticks==90)log_info("[ribbon-phase] tick=%d scale=%.9g sparks=%d floor=%.9g",s->simulation.ticks,s->simulation.scale,count,floor);
}
void Destroy(State*s){delete s;}
}
