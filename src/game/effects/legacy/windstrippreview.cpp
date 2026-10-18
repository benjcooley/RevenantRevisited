#include "game/effects/legacy/windstrippreview.h"
#include "game/effects/effect.h"
#include "game/effects/windstripmatrix.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
namespace windstrip_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;retail_windstrip::State core;
 std::vector<SMeshVertex> drop_vertices;std::vector<uint16_t> drop_faces;TTextureHandle strip_texture=kInvalidTexture;MeshHandle halo=0;
 double accumulator=0;int ticks=0;int64_t rendered=-1;
 ~State(){if(Renderer && halo)Renderer->ReleaseMeshAssetRef(halo);}
};
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xded3dbedu):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0xded3dbedu)return nullptr;s->owner->OnScreen();
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=2 || img->NumStates()!=3 || img->NumTextures()!=2 || img->NumMaterials()!=2 || img->NumTags()!=0)return nullptr;
 for(int i=0;i<2;++i){if(std::strcmp(img->GetObjectName(i),i?"box02":"box01") || img->GetObjectParent(i,0)!=-1)return nullptr;std::vector<SMeshVertex> vertices;std::vector<uint16_t> faces;if(!ExtractSubMeshTextureSlot(img,i,i+1,vertices,faces) || vertices.size()!=12 || faces.size()!=36)return nullptr;
  S3DTex texture={};img->GetTexture(i,&texture);const auto& p=texture.desc.pixelFormat;if(texture.numframes!=1 || p.dwRGBBitCount!=16 || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  if(i==0){if(p.dwRBitMask!=0xf00 || p.dwGBitMask!=0xf0 || p.dwBBitMask!=0xf || p.dwRGBAlphaBitMask!=0xf000)return nullptr;s->drop_vertices=vertices;s->drop_faces=faces;s->strip_texture=img->GetTextureHandle(i);}else{if(p.dwRBitMask!=0xf800 || p.dwGBitMask!=0x7e0 || p.dwBBitMask!=0x1f || p.dwRGBAlphaBitMask!=0)return nullptr;s->halo=Renderer->RegisterMeshAsset(0x57494e4448414c4full,vertices.data(),int(vertices.size()),faces.data(),int(faces.size()),img->GetTextureHandle(i));if(!s->halo)return nullptr;Renderer->AddMeshAssetRef(s->halo);}}
 for(int i=0;i<3;++i)if(img->GetAniLength(i)!=1 || img->GetAniFlags(i)!=8192)return nullptr;
 s->core.Initialize([](int a,int b){return random(a,b);});log_info("[windstrip] original20 strips,400 drops,2 authored textures,null_spell,source_count252");log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT WindStrip",int(StartupVfxNativeDomain));return s.release();
}
bool Alive(State* s){return s && s->core.alive && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds){if(!s || !std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;
 // Screen Pulse precedes NextFrame, then the actual custom animator clears
 // CommandDone. Reversing these kills this one-frame non-looping owner.
 s->owner->Pulse();if(!Alive(s))return;s->owner->NextFrame();s->owner->SetCommandDone(false);s->core.Animate();++s->ticks;if(!s->core.alive)s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);
}}
void SubmitWorld(State* s){if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();if(!s->core.frame)return;
 hmm_mat4 root;const auto p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();int drops=0;
 auto triangle=[&](const hmm_vec3* points,const float uv[3][2]){SQuadDrawItem q{};q.corner_count=3;q.key.texture=s->strip_texture;q.key.blend=uint8_t(EFxBlend::Alpha);q.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);q.retail_argb4444=true;q.retail_texture=1;q.retail_software_projection=StartupVfxNativeDomain;for(int j=0;j<3;++j){std::memcpy(q.world_pos[j],points[j].Elements,12);std::memcpy(q.uv[j],uv[j],8);}Renderer->SubmitFxQuad(q);};
 const auto faces=retail_windstrip::StripFaces();for(const auto& strip:s->core.strips){hmm_vec3 vertices[20];for(int i=0;i<20;++i){const auto v=retail_windstrip::StripVertex(strip,i);hmm_vec3 point{v.x,v.y,v.z};MtxTransform(&root,&point,&vertices[i]);}for(int f=0;f<18;++f){hmm_vec3 points[3];float uv[3][2];for(int j=0;j<3;++j){int i=faces[f*3+j];points[j]=vertices[i];uv[j][0]=float(double(i/2)*double(.10000000149011612f));uv[j][1]=float(i&1);}triangle(points,uv);}}
 s->core.RenderDrops([](int a,int b){return random(a,b);},[&](int,const retail_windstrip::Drop& d){++drops;const auto local=retail_windstrip::BillboardMatrix(d.scale,d.position);hmm_mat4 world;MtxMultiply(&world,&local,&root);for(size_t f=0;f<s->drop_faces.size();f+=3){hmm_vec3 points[3];float uv[3][2];for(int j=0;j<3;++j){const auto& v=s->drop_vertices[s->drop_faces[f+j]];hmm_vec3 xyz{v.pos[0],v.pos[1],v.pos[2]};MtxTransform(&world,&xyz,&points[j]);std::memcpy(uv[j],v.uv,8);}triangle(points,uv);}});
 if(s->core.halo>0){const auto local=retail_windstrip::BillboardMatrix(s->core.halo,{0,0,s->core.halo});hmm_mat4 world;MtxMultiply(&world,&local,&root);SHelperMeshSubmit q{};q.mesh=s->halo;q.retail_lighting=1;q.additive_blend=true;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;for(int r=0;r<4;++r)for(int c=0;c<4;++c)q.world[r*4+c]=world.Elements[c][r];Renderer->SubmitGoldFlareAfterFx(q);}
 log_info("[windstrip-phase] tick=%d frame=%d halo=%.9g drops=%d rng=%llu",s->ticks,s->core.frame,s->core.halo,drops,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* s){delete s;}
}
