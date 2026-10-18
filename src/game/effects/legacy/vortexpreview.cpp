#include "game/effects/legacy/vortexpreview.h"
#include "game/effects/vortexlegacykeys.h"
#include "game/effects/effect.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "engine/render/retailmeshlighting.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/core/logging.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
namespace vortex_preview {
struct State {
 struct Part{int object;MeshHandle mesh;int lighting;std::vector<hmm_vec3> positions;std::vector<uint16_t> indices;S3DMaterial material{};};
 TObjectPtr<TObjectInstance> owner;std::vector<Part>parts;double accumulated=0;
 ~State(){if(Renderer)for(auto p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cls=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cls?cls->FindObjType(0x452dade0u):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cls->NewObject(&d));auto*im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
 if(!im || s->owner->ObjId()!=0x452dade0u || !vortex_legacy::Matches(im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;s->owner->OnScreen();
 for(int object=0;object<3;++object)for(int slot=0;slot<2;++slot){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,object,slot,v,indices)||indices.empty())continue;TTextureHandle texture=Renderer->WhiteTextureHandle();int lighting=4;if(slot){S3DTex t={};im->GetTexture(0,&t);texture=t.htexture;lighting=RetailMeshTextureLighting(t.desc.pixelFormat);}if(texture==kInvalidTexture)return nullptr;auto mesh=Renderer->RegisterMeshAsset(0x564f525445580000ull+object*2+slot,v.data(),int32_t(v.size()),indices.data(),int32_t(indices.size()),texture);if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);State::Part part{object,mesh,lighting,{},{}};for(const auto&vertex:v)part.positions.push_back({vertex.pos[0],vertex.pos[1],vertex.pos[2]});part.indices=std::move(indices);S3DObj object_data={};S3DMat material_data={};im->GetObject(object,&object_data);im->GetMaterial(object_data.material,&material_data);part.material=material_data.matdesc;s->parts.push_back(std::move(part));}
 if(s->parts.size()!=3)return nullptr;log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT vertex=MODELZ",int(StartupVfxNativeDomain));log_info("[vortex-default] typed_id=452dade0 meshes=3 vertices=205 triangles=304 state0=static own_legacy_keys=24");return s.release();
}
bool Alive(State*s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulated+=seconds;while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds){s->accumulated-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;}}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer)return;auto*im=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 root;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(IntFacing(s->owner)));else root=s->owner->Transform().Matrix();
 for(const auto&part:s->parts){S3DAnimObj o={};o.objnum=o.animtrack=part.object;if(!im->CalcObjectMatrix(&o,0,0,&root,false))continue;hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;mesh.retail_lighting=part.lighting;mesh.retail_source_alpha=part.lighting==4;mesh.retail_software_projection=StartupVfxNativeDomain;mesh.retail_positive_face_cull=true;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];if(part.lighting!=4){Renderer->SubmitHelperMesh(mesh);continue;}
  // The modern fallback consumes material uniforms; the software RGB5 path
  // deliberately ignores them. Preserve the own resource rather than zero alpha.
  const auto copy=[](float* out,const SRenderColor& c){out[0]=c.r;out[1]=c.g;out[2]=c.b;out[3]=c.a;};
  copy(mesh.diffuse,part.material.diffuse);copy(mesh.ambient,part.material.ambient);
  copy(mesh.specular,part.material.specular);copy(mesh.emissive,part.material.emissive);
  mesh.power=part.material.power;
  // Original untextured kernel: positive screen-down cull and maximum vertex
  // depth per triangle. The existing mode4 pipeline supplies the depth write.
  std::vector<hmm_vec3> points(part.positions.size());for(size_t i=0;i<points.size();++i)MtxTransform(&world,&part.positions[i],&points[i]);
  for(size_t i=0;i<part.indices.size();i+=3){float x[3],y[3],depth[3];for(int k=0;k<3;++k){const auto&v=points[part.indices[i+k]];const float sum=v.X+v.Y;x[k]=v.X-v.Y;y[k]=StartupVfxNativeDomain?.5050762891769409f*sum-1.237179160118103f*v.Z:.5f*sum-.867f*v.Z;depth[k]=StartupVfxNativeDomain?-.8748177289962769f*sum-.7142857909202576f*v.Z:-.8660254037844386f*sum-.5f*v.Z;}
   if((x[1]-x[0])*(y[2]-y[0])-(y[1]-y[0])*(x[2]-x[0])<=0)continue;mesh.index_offset=int(i);mesh.index_count=3;mesh.retail_flat_depth=true;mesh.retail_flat_depth_offset=(std::max)({depth[0],depth[1],depth[2]});Renderer->SubmitHelperMesh(mesh);}
 }
}
void Destroy(State*s){delete s;}
}
