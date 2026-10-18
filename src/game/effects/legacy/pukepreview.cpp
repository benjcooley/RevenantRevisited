#include "game/effects/legacy/pukepreview.h"
#include "game/effects/effect.h"
#include "game/effects/pukecore.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstdlib>
#include "game/app/revutils.h"
#include <cstring>
#include <memory>
#include <vector>
namespace puke_preview {
// Diagnostic bit0 selects the proven integer vertex lattice; bit1 is the
// separately retained source-contribution negative control. Default is grid only.
static int RasterPolicy(){const char* p=std::getenv("REV_PUKE_RASTER_DIAGNOSTIC");return p && p[0]>='0' && p[0]<='3' && !p[1]?p[0]-'0':1;}
struct State {
 TObjectPtr<TObjectInstance> owner;
 retail_puke::State core;
 MeshHandle mesh[3]{};
 double accumulator=0;
 int ticks=0;
 bool diagnostic=std::getenv("REV_PUKE_RASTER_DIAGNOSTIC")!=nullptr;
 int raster_policy=RasterPolicy();
 int64_t rendered=-1;
 ~State(){if(Renderer)for(auto m:mesh)if(m)Renderer->ReleaseMeshAssetRef(m);}
};
static void ConfigureRaster(SHelperMeshSubmit& draw,const State* s){
 draw.retail_software_projection=StartupVfxNativeDomain;
 draw.retail_software_pixel_grid=StartupVfxNativeDomain && (s->raster_policy&1);
 draw.retail_alpha_source_quantize=StartupVfxNativeDomain && (s->raster_policy&2);
}
static void Trace(State* s){if(!s->diagnostic)return;log_info("[puke-state] tick=%d alive=%d life=%d scale=%.9g rng=%llu policy=%d",s->ticks,int(s->core.alive),s->core.life,s->core.scale,(unsigned long long)RandomDraws(),s->raster_policy);for(int i=0;i<25;++i){const auto&p=s->core.position[i];const auto&v=s->core.velocity[i];log_info("[puke-slot] tick=%d slot=%d start=%d p=%.9g,%.9g,%.9g v=%.9g,%.9g,%.9g",s->ticks,i,s->core.starts[i],p[0],p[1],p[2],v[0],v[1],v[2]);}}
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bc11u):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0xad92bc11u)return nullptr;s->owner->OnScreen();
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=3 || img->NumStates()!=1 || img->GetAniLength(0)!=100 || img->NumTextures()!=3 || img->NumMaterials()!=3 || img->NumTags()!=0)return nullptr;
 for(int i=0;i<3;++i){const char* name=i==0?"box01":i==1?"box02":"box03";if(std::strcmp(img->GetObjectName(i),name) || img->GetObjectParent(i,0)!=-1)return nullptr;
  S3DTex texture={};img->GetTexture(i,&texture);const auto& pf=texture.desc.pixelFormat;
  if(texture.numframes!=1 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf00 || pf.dwGBitMask!=0xf0 || pf.dwBBitMask!=0xf || pf.dwRGBAlphaBitMask!=0xf000 || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,i,i+1,verts,indices) || verts.size()!=12 || indices.size()!=36)return nullptr;
  s->mesh[i]=Renderer->RegisterMeshAsset(0x50554b4500000000ull+i,verts.data(),int(verts.size()),indices.data(),int(indices.size()),img->GetTextureHandle(i));if(!s->mesh[i])return nullptr;Renderer->AddMeshAssetRef(s->mesh[i]);
 }
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Puke null-spell",int(StartupVfxNativeDomain));log_info("[puke] exact authored meshes=3 particles=25 null_spell=1 source=(0,0,0) life=45");Trace(s.get());return s.release();
}
bool Alive(State* s){return s && s->core.alive && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds){
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;++s->ticks;s->core.Advance([](int a,int b){return random(a,b);});if(!s->core.alive)s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);Trace(s);}
}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();if(s->core.life>=30)return;
 auto* img=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 root;const auto& origin=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();int draws=0;
 // Original three object groups, not particle-index interleaving.
 for(int part=0;part<3;++part)for(int i=part;i<25;i+=3){if(s->core.starts[i]>0)continue;S3DAnimObj o={};o.objnum=part;o.flags=0x48;o.scl={s->core.scale,s->core.scale,s->core.scale};const auto& p=s->core.position[i];o.pos={p[0],p[1],p[2]};
  if(!img->CalcObjectMatrix(&o,0,s->owner->GetFrame(),&root,false))continue;hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);
  SHelperMeshSubmit draw={};draw.mesh=s->mesh[part];draw.retail_lighting=3;draw.retail_source_alpha=true;ConfigureRaster(draw,s);
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)draw.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(draw);++draws;
 }
 log_info("[puke-phase] tick=%d life=%d scale=%.9g draws=%d first=(%.9g,%.9g,%.9g)",s->ticks,s->core.life,s->core.scale,draws,s->core.position[0][0],s->core.position[0][1],s->core.position[0][2]);
}
void Destroy(State* s){delete s;}
}
