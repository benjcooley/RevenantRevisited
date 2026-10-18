#include "blastpreview.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../speedauthoredmatrix.h"
#include "../cometsoftwarebounds.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <array>
#include <cstring>
#include <memory>
#include <vector>

namespace blast_preview {
struct Part { int object=0, slot=0; MeshHandle mesh=0; std::vector<std::array<hmm_vec3,3>> triangles; };
struct State {
 TObjectPtr<TObjectInstance> owner;
 std::vector<Part> parts;
 double accumulator=0;
 int ticks=0;
 ~State(){if(Renderer)for(const auto& p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0x0ea34fa3u):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0x0ea34fa3u)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=56 || img->NumStates()!=1 || img->GetAniLength(0)!=120 || img->GetAniFlags(0)!=0x2000 || img->NumTextures()!=3 || img->NumMaterials()!=3 || img->NumTags()!=2)return nullptr;
 const auto* scroll=img->GetTag(0);const auto* blend=img->GetTag(1);
 if(!scroll || !blend || scroll->state || scroll->frame || blend->state || blend->frame!=20 ||
    !scroll->name || !scroll->str || !blend->name || !blend->str || std::strcmp(scroll->name,"scrolltex") ||
    std::strcmp(scroll->str,"obj=(sparks42),du=-0.01,dv=0") || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litaddz"))return nullptr;
 for(int i=0;i<3;++i){S3DTex texture={};img->GetTexture(i,&texture);const auto& f=texture.desc.pixelFormat;
  if(texture.numframes!=1 || texture.desc.width!=uint32_t(32<<i) || texture.desc.height!=uint32_t(32<<i) || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask)return nullptr;}
 for(int slot=1;slot<=3;++slot)for(int object=0;object<56;++object){
  if(img->GetObjectParent(object,0)!=-1)return nullptr;
  std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
  if(!ExtractSubMeshTextureSlot(img,object,slot,vertices,indices))continue;
  const auto texture=img->GetTextureHandle(slot-1);if(texture==kInvalidTexture)return nullptr;
  const auto mesh=Renderer->RegisterMeshAsset(0x424c415354000000ull|uint64_t(slot<<16)|uint64_t(object),vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),texture);
  if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);
  Part part{object,slot,mesh,{}};
  for(size_t face=0;face<indices.size();face+=3){std::array<hmm_vec3,3> triangle;
   for(int c=0;c<3;++c){const auto& v=vertices[indices[face+c]];triangle[c]={v.pos[0],v.pos[1],v.pos[2]};}
   part.triangles.push_back(triangle);
  }
  s->parts.push_back(std::move(part));
 }
 if(s->parts.size()!=56)return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Blast authored=56",int(StartupVfxNativeDomain));
 return s.release();
}
void Advance(State* s,double seconds)
{
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!s->owner || (s->owner->GetFlags()&OF_KILL))return;s->owner->NextFrame();s->owner->Pulse();++s->ticks;}
}
bool Alive(State* s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void SubmitWorld(State* s)
{
 if(!Alive(s) || !Renderer)return;
 auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img)return;
 img->SetPrevState(-1,0);hmm_mat4 root;const auto& origin=IntPos(s->owner);
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();
 int draws=0;
 for(const auto& part:s->parts){
  hmm_vec3 pos{},rot{},scale{};if(!img->GetUninterpolatedAniKey(part.object,0,s->owner->GetFrame(),pos,rot,scale,true) || scale.X<=0 || scale.Y<=0 || scale.Z<=0)continue;
  hmm_mat4 local,world;
  if(img->GetObjectName(part.object)[0]=='#')BuildRetailSpeedCameraMatrix(local,pos,rot,scale);
  else {MtxClear(&local);MtxRotateX(&local,rot.X);MtxRotateY(&local,rot.Y);MtxRotateZ(&local,rot.Z);MtxScale(&local,&scale);MtxTranslate(&local,&pos);}
  MtxMultiply(&world,&local,&root);SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;
  if(s->ticks==16 || s->ticks==60)log_info("[blast-matrix] tick=%d object=%d m=(%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g)",s->ticks,part.object,local.Elements[0][0],local.Elements[0][1],local.Elements[0][2],local.Elements[0][3],local.Elements[1][0],local.Elements[1][1],local.Elements[1][2],local.Elements[1][3],local.Elements[2][0],local.Elements[2][1],local.Elements[2][2],local.Elements[2][3],local.Elements[3][0],local.Elements[3][1],local.Elements[3][2],local.Elements[3][3]);
  for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];
  mesh.additive_blend=true;mesh.retail_lighting=1;mesh.retail_positive_face_cull=true;mesh.retail_gold_no_depth=true;
  mesh.retail_software_projection=StartupVfxNativeDomain;
  if(part.object==55)mesh.uv_offset[0]=float(TTime::LegacyFrameCount())*-.01f;
  // The original RGB565 raster rejects oversized edges from vertex0 before
  // drawing. Keep complete ordinary geometry and source face order.
  if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
   mesh.index_count=3;
   for(size_t face=0;face<part.triangles.size();++face){
    if(!RetailCometTriangleAllowed(part.triangles[face].data(),world))continue;
    mesh.index_offset=int(face*3);Renderer->SubmitHelperMesh(mesh);
   }
  }else Renderer->SubmitHelperMesh(mesh);
  ++draws;
 }
 if(s->ticks==2 || s->ticks==16 || s->ticks==60 || s->ticks==119)log_info("[blast-phase] tick=%d global=%lld draws=%d",s->ticks,static_cast<long long>(TTime::LegacyFrameCount()),draws);
}
void Destroy(State* s){delete s;}
}
