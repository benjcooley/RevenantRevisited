#include "jtelepreview.h"
#include "../jteleprofile.h"
#include "../cometsoftwarebounds.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../authoredpartsys.h"
#include "../speedauthoredmatrix.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
namespace jtele_preview {
struct State {
 struct Part{int object;MeshHandle mesh;std::vector<std::array<hmm_vec3,3>> triangles;};std::unique_ptr<TObjectInstance>owner;std::vector<Part>parts;authored_partsys::State particles;authored_partsys::TickInputs inputs;S3DVertex trail[4]{};double accumulator=0;int ticks=0;int64_t rendered=-1;
 ~State(){if(Renderer)for(auto p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool Pose(T3DImagery*im,int object,int frame,hmm_mat4&matrix,hmm_vec3&p,hmm_vec3&r,hmm_vec3&s){
 if(!im->GetUninterpolatedAniKey(object,0,frame,p,r,s,true))return false;if(s.X<=1e-5f||s.Y<=1e-5f||s.Z<=1e-5f)return false;
 if(object==0||(object>=2&&object<=4))BuildRetailSpeedCameraMatrix(matrix,p,r,s);else{MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&s);MtxTranslate(&matrix,&p);}return true;
}
State* Spawn(const S3DPoint&origin,uint32_t id){
 if(!Renderer||!jtele_profile::Find(id))return nullptr;auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(id):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner||s->owner->ObjId()!=id)return nullptr;s->owner->Face(0);s->owner->OnScreen();auto*im=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!jtele_profile::Matches(id,im))return nullptr;
 // Original default Render draws controllers first, then texture-major roots.
 for(int slot=1;slot<=2;++slot)for(int object=2;object<8;++object){std::vector<SMeshVertex>v;std::vector<uint16_t>indices;if(!ExtractSubMeshTextureSlot(im,object,slot,v,indices)||indices.empty())continue;auto mesh=Renderer->RegisterMeshAsset(0x4a54454c45000000ull^(uint64_t(id)<<8)^(object<<2)^slot,v.data(),int(v.size()),indices.data(),int(indices.size()),im->GetTextureHandle(slot-1));if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);State::Part part{object,mesh,{}};for(size_t face=0;face<indices.size();face+=3){std::array<hmm_vec3,3>triangle{};for(int k=0;k<3;++k){const auto&vertex=v[indices[face+k]];triangle[k]={vertex.pos[0],vertex.pos[1],vertex.pos[2]};}part.triangles.push_back(triangle);}s->parts.push_back(std::move(part));}
 if(s->parts.size()!=6)return nullptr;im->GetObjVerts(0,s->trail);s->inputs.emitters.resize(3);for(auto&e:s->inputs.emitters){e.scale={0,0,0};e.matrix.fill(0);}s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={0,0,0};s->inputs.ground_height=[](int,int,int){return 0;};authored_partsys::Definition definition;std::string diagnostic;if(!authored_partsys::ParseDefinition(jtele_profile::particle_tag,definition,diagnostic)||!s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Jtele literaldefault",int(StartupVfxNativeDomain));log_info("[jtele] typed_id=%08x meshes=6 emitters=3 frames=50 real_finite=1 capacity=%zu tagframe15_notactivation=1",id,s->particles.Capacity());return s.release();
}
bool Alive(State*s){return s&&s->owner&&!(s->owner->GetFlags()&OF_KILL);}
void Advance(State*s,double seconds){
 if(!Alive(s)||!std::isfinite(seconds)||seconds<=0)return;s->accumulator+=seconds;while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;const int cached=s->owner->GetFrame();s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;auto*im=static_cast<T3DImagery*>(s->owner->GetImagery());s->inputs.animation_frame=cached;for(int i=0;i<3;++i){hmm_mat4 m;hmm_vec3 p{},r{},scale{};const bool valid=Pose(im,i+2,cached,m,p,r,scale);auto&e=s->inputs.emitters[i];e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};if(valid){std::memcpy(e.matrix.data(),m.Elements,64);e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};}}s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});++s->ticks;}
}
void SubmitWorld(State*s){
 if(!Alive(s)||!Renderer||s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();auto*im=static_cast<T3DImagery*>(s->owner->GetImagery());const auto&particles=s->particles.Particles();int draws=0;
 for(size_t slot=0;slot<particles.size();++slot){if(!particles[slot].alive)continue;const auto sample=s->particles.SampleRender(slot,[](int a,int b){return random(a,b);});if(sample.scale<=9.999999747378752e-6f)continue;hmm_mat4 m;const hmm_vec3 p={sample.position[0],sample.position[1],sample.position[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},scale={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(m,p,r,scale);SQuadDrawItem q={};q.retail_software_projection=StartupVfxNativeDomain;q.key.texture=im->GetTextureHandle(0);q.key.blend=uint8_t(EFxBlend::AdditiveStraight);q.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);q.light_mode=EFxLightMode::Unlit;q.retail_texture=1;q.retail_rgb565_additive=true;for(int c=0;c<3;++c)q.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;q.color_rgba[3]=sample.alpha;constexpr int corners[]={0,1,3,2};for(int c=0;c<4;++c){const auto&v=s->trail[corners[c]];hmm_vec3 world;MtxTransform(&m,&v.pos,&world);q.world_pos[c][0]=world.X;q.world_pos[c][1]=world.Y;q.world_pos[c][2]=StartupVfxNativeDomain?world.Z:particles[slot].position[2]+(world.Z-sample.position[2])*WORLD3D_Z_SCALE;q.uv[c][0]=v.tu;q.uv[c][1]=v.tv;}Renderer->SubmitFxQuad(q);++draws;}
 hmm_mat4 root;const auto&origin=s->owner->Pos();if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();for(const auto&part:s->parts){hmm_mat4 local,world;hmm_vec3 p{},r{},scale{};if(!Pose(im,part.object,s->owner->GetFrame(),local,p,r,scale))continue;MtxMultiply(&world,&local,&root);SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;mesh.retail_lighting=1;mesh.additive_blend=true;mesh.retail_positive_face_cull=true;mesh.retail_software_projection=StartupVfxNativeDomain;if(part.object==5)mesh.uv_offset[1]=float(s->ticks)*.1f;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];if(StartupVfxNativeDomain&&Renderer->UsesRetailSoftwareMeshLighting()){int first=-1;for(size_t face=0;face<=part.triangles.size();++face){const bool allowed=face<part.triangles.size()&&RetailCometTriangleAllowed(part.triangles[face].data(),world);if(allowed&&first<0)first=int(face);if(!allowed&&first>=0){mesh.index_offset=first*3;mesh.index_count=(int(face)-first)*3;Renderer->SubmitGoldFlareAfterFx(mesh);first=-1;}}}else Renderer->SubmitGoldFlareAfterFx(mesh);}
 if(s->ticks<=3||s->ticks==6||s->ticks==16||s->ticks==25||s->ticks==40||s->ticks==48||s->ticks==49)log_info("[jtele-phase] tick=%d frame=%d trails=%d credit=%.9g",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit());
}
void Destroy(State*s){delete s;}
}
