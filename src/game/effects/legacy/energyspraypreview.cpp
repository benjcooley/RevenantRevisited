#include "game/effects/legacy/energyspraypreview.h"
#include "game/effects/effect.h"
#include "game/effects/energyspraymatrix.h"
#include "game/effects/cometsoftwarebounds.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
namespace energyspray_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 retail_energyspray::State core;
 MeshHandle mesh[2]{}, triangle_mesh=0;
 std::vector<SMeshVertex> vertices[2];std::vector<uint16_t> faces[2];
 std::vector<SMeshVertex> triangle;
 std::vector<uint16_t> indices;
 double accumulator=0;int ticks=0;int64_t rendered=-1;
 ~State(){if(Renderer){for(auto m:mesh)if(m)Renderer->ReleaseMeshAssetRef(m);if(triangle_mesh)Renderer->ReleaseMeshAssetRef(triangle_mesh);}}
};
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bc1cu):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner || s->owner->ObjId()!=0xad92bc1cu)return nullptr;s->owner->OnScreen();
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=3 || img->NumStates()!=1 || img->GetAniLength(0)!=100 || img->NumTextures()!=2 || img->NumMaterials()!=3 || img->NumTags()!=0)return nullptr;
 for(int i=0;i<3;++i){const char* name=i==0?"rectangle0":i==1?"rectangle1":"ngon01";if(std::strcmp(img->GetObjectName(i),name) || img->GetObjectParent(i,0)!=-1)return nullptr;
  std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,i,i<2?i+1:0,verts,indices) || verts.size()!=size_t(i<2?4:3) || indices.size()!=size_t(i<2?6:3))return nullptr;
  if(i==2){
   s->triangle=verts;s->indices=indices;
   for(auto& v:verts){for(float& n:v.normal)n=0;v.uv[0]=v.uv[1]=0;}
   verts[0].uv[1]=1; // Original vertex0 carries color; other two stay zero.
   s->triangle_mesh=Renderer->RegisterMeshAsset(0x454e545257454947ull,verts.data(),3,indices.data(),3,Renderer->WhiteTextureHandle());
   if(!s->triangle_mesh)return nullptr;Renderer->AddMeshAssetRef(s->triangle_mesh);continue;
  }
  S3DTex texture={};img->GetTexture(i,&texture);const auto& pf=texture.desc.pixelFormat;if(texture.numframes!=1 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf00 || pf.dwGBitMask!=0xf0 || pf.dwBBitMask!=0xf || pf.dwRGBAlphaBitMask!=0xf000 || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;
  s->vertices[i]=verts;s->faces[i]=indices;
  s->mesh[i]=Renderer->RegisterMeshAsset(0x454e455247590000ull+i,verts.data(),int(verts.size()),indices.data(),int(indices.size()),img->GetTextureHandle(i));if(!s->mesh[i])return nullptr;Renderer->AddMeshAssetRef(s->mesh[i]);
 }
 const auto resources=Renderer->GetAssetStats();
 log_info("[energyspray-resources] fixed_meshes=3 cache_meshes=%u vertex_bytes=%llu index_bytes=%llu",resources.mesh_count,static_cast<unsigned long long>(resources.mesh_vertex_bytes),static_cast<unsigned long long>(resources.mesh_index_bytes));
 s->core.Initialize([](int a,int b){return random(a,b);});
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT EnergySpray null-spell",int(StartupVfxNativeDomain));log_info("[energyspray] meshes=3 radial=25 triangles=20 init_rng=160 render_rng=1 phases=60/50/25 null_spell=1 fixture_translation_only");return s.release();
}
bool Alive(State* s){return s && s->core.alive && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds){if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;++s->ticks;s->core.Pulse();if(!s->core.alive){s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);return;}s->core.Animate([](int a,int b){return random(a,b);});}}
void SubmitWorld(State* s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 root;const auto& p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();
 auto submit=[&](MeshHandle mesh,const hmm_mat4& local,bool prelit,int part=0,uint32_t color=0){hmm_mat4 world;MtxMultiply(&world,&local,&root);SHelperMeshSubmit draw={};draw.mesh=mesh;draw.retail_lighting=prelit?4:3;draw.retail_source_alpha=true;draw.retail_prelit_rgba5=prelit;draw.retail_software_projection=StartupVfxNativeDomain;draw.retail_alpha_source_quantize=StartupVfxNativeDomain && !prelit;
  if(prelit){draw.retail_prelit_color_uniform=true;draw.retail_prelit_color=((color>>27)<<15)|(((color>>19)&31)<<10)|(((color>>11)&31)<<5)|((color>>3)&31);}
  if(prelit && StartupVfxNativeDomain){draw.retail_flat_depth=true;float farthest=-1e30f;for(const auto& v:s->triangle){hmm_vec3 xyz{v.pos[0],v.pos[1],v.pos[2]},w;MtxTransform(&world,&xyz,&w);const float depth=-.8748177289962769f*(w.X+w.Y)-.7142857909202576f*w.Z;if(depth>farthest)farthest=depth;}draw.retail_flat_depth_offset=farthest;}
  for(int r=0;r<4;++r)for(int c=0;c<4;++c)draw.world[r*4+c]=world.Elements[c][r];
  // Untextured56d9fd..56da7f uses the same literal primitive-span limits.
  if(prelit && StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){hmm_vec3 points[3];for(int j=0;j<3;++j){const auto&v=s->triangle[s->indices[j]];points[j]={v.pos[0],v.pos[1],v.pos[2]};}if(!RetailCometTriangleAllowed(points,world))return;}
  // Actual source textured raster rejects oversized edges from vertex0.
  // Only the declared native software fixture selects this admission policy.
  if(!prelit && StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
   draw.index_count=3;for(size_t face=0;face<s->faces[part].size();face+=3){hmm_vec3 points[3];for(int j=0;j<3;++j){const auto&v=s->vertices[part][s->faces[part][face+j]];points[j]={v.pos[0],v.pos[1],v.pos[2]};}if(!RetailCometTriangleAllowed(points,world))continue;draw.index_offset=int(face);Renderer->SubmitHelperMesh(draw);}
  }else Renderer->SubmitHelperMesh(draw);
 };
 if(s->core.phase<3)for(int i=0;i<25;++i)submit(s->mesh[1],retail_energyspray::RadialMatrix(s->core,i),false,1);
 const float scale=s->core.RenderScale([](int a,int b){return random(a,b);});submit(s->mesh[0],retail_energyspray::FlareMatrix(scale),false);
 if(s->core.phase<3)for(int i=0;i<20;++i)submit(s->triangle_mesh,retail_energyspray::TriangleMatrix(s->core,i),true,0,retail_energyspray::PackedColor(s->core,i));
 log_info("[energyspray-phase] tick=%d phase=%d countdown=%d core=%.9g rendered_scale=%.9g draws=%d rng=%llu",s->ticks,s->core.phase,s->core.countdown,s->core.core_scale,scale,s->core.phase<3?46:1,static_cast<unsigned long long>(RandomDraws()));
}
void Destroy(State* s){delete s;}
}
