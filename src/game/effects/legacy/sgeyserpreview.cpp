#include "game/effects/legacy/sgeyserpreview.h"
#include "game/effects/effect.h"
#include "engine/fx/authoredpartsys.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "game/effects/sgeyserpose.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <cstdlib>
namespace sgeyser_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex steam[4]{};MeshHandle blast=0;S3DMat material{};
 double accumulated=0;int ticks=0;int64_t rendered=-1;uint64_t initial_draws=0;bool trace=std::getenv("REV_SGEYSER_TRACE")!=nullptr;
 ~State(){log_info("[sgeyser-cleanup] tick=%d alive=%d",ticks,int(owner && !(owner->Flags()&OF_KILL)));if(Renderer && blast)Renderer->ReleaseMeshAssetRef(blast);}
};
static uint32_t Hash(const void* p,size_t n){uint32_t h=2166136261u;auto*b=static_cast<const uint8_t*>(p);while(n--)h=(h^*b++)*16777619u;return h;}
static bool Pose(State& s,int object,int frame,hmm_mat4& m){auto*img=static_cast<T3DImagery*>(s.owner->GetImagery());hmm_vec3 p{},r{},scale{};
 if(!retail_sgeyser::Key([&](int o,int f,hmm_vec3&pp,hmm_vec3&rr,hmm_vec3&ss){return img->GetUninterpolatedAniKey(o,0,f,pp,rr,ss,true);},object,frame,p,r,scale))return false;
 retail_sgeyser::Matrix(m,object,p,r,scale);
 if(object==1){auto&e=s.inputs.emitters[0];e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};std::memcpy(e.matrix.data(),m.Elements,64);s.inputs.animation_frame=frame;}
 return scale.X>1e-5f && scale.Y>1e-5f && scale.Z>1e-5f;
}
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();auto*cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad92bd30u):-1;if(type<0)return nullptr;
 SObjectState def={};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=origin;s->owner.reset(cls->NewObject(&def));if(!s->owner || s->owner->ObjId()!=0xad92bd30u)return nullptr;s->owner->OnScreen();
 auto*img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!img || img->NumObjects()!=3 || img->NumStates()!=1 || img->GetAniLength(0)!=45 || img->GetAniFlags(0)!=0x2000 || img->NumTags()!=2 || img->NumTextures()!=1 || img->NumMaterials()!=1)return nullptr;
 constexpr const char* names[]={"#steam","steam","#blast"};constexpr int counts[]={6,6,44};constexpr uint32_t hashes[]={0xbbb06f21u,0x76c1714u,0x5c7b93b5u};
 for(int i=0;i<3;++i){S3DObj o={};img->GetObject(i,&o);if(std::strcmp(img->GetObjectName(i),names[i]) || o.parent[0]!=-1 || o.numanikeys[0]!=counts[i] || Hash(o.anikeys[0],counts[i]*4)!=hashes[i])return nullptr;}
 constexpr auto literal="obj=(steam),particle=#steam,pps=[0:0,5:250,25:150,30:0],lifespan=7:10,initialvelocity=20:35,scale=[0:0.5,100:2],rlocalrotation=[0:(0,45,0),100:(0,360,0)],color=[0:(100,100,100),100:(0,0,0)],spread=1,friction=[0:0.1],gravity=[0:0.3]";
 for(int i=0;i<2;++i){const auto*t=img->GetTag(i);if(!t || t->state || t->frame!=(i?30:10) || !t->name || !t->str || std::strcmp(t->name,"partsys") || std::strcmp(t->str,i?"filename vapor":literal))return nullptr;}
 S3DTex tex={};img->GetTexture(0,&tex);const auto&pf=tex.desc.pixelFormat;if(tex.numframes!=1 || tex.desc.width!=64 || tex.desc.height!=64 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf800 || pf.dwGBitMask!=0x7e0 || pf.dwBBitMask!=31 || pf.dwRGBAlphaBitMask || img->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;if(!ExtractSubMeshTextureSlot(img,2,1,vertices,indices) || vertices.size()!=4 || indices.size()!=6)return nullptr;
 s->blast=Renderer->RegisterMeshAsset(0x5347455953455202ull,vertices.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->blast)return nullptr;Renderer->AddMeshAssetRef(s->blast);img->GetMaterial(0,&s->material);img->GetObjVerts(0,s->steam);
 s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={0,0,0};s->inputs.emitters.resize(1);
 // Actual loaded Init retains an untouched zero emitter cache. First Pulse has
 // zero PPS; only original Animate creates the authored emitter matrix.
 s->inputs.emitters[0].matrix.fill(0);s->inputs.emitters[0].scale={0,0,0};s->inputs.ground_height=[state=s.get()](int x,int y,int z){if(state->trace)log_info("[sgeyser-ground] tick=%d point=%d,%d,%d",state->ticks,x,y,z);return 0;};
 authored_partsys::Definition d;std::string why;if(!authored_partsys::ParseDefinition(literal,d,why) || !s->particles.Initialize(d,s->inputs,0,why) || s->particles.Capacity()!=104)return nullptr;
 s->initial_draws=RandomDraws();log_info("[sgeyser] id=ad92bd30 source=%d,%d,%d controllers=1 capacity=104 optional_vapor=missing ground=0 seeded=0",origin.x,origin.y,origin.z);log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return s.release();
}
bool Alive(State*s){return s && s->owner && !(s->owner->Flags()&OF_KILL);}
void Advance(State*s,double dt){if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulated+=dt;while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds){s->accumulated-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();++s->ticks;s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});if(!Alive(s))log_info("[sgeyser-kill] tick=%d frame=%d rng=%llu",s->ticks,s->owner->GetFrame(),static_cast<unsigned long long>(RandomDraws()-s->initial_draws));}}
void SubmitWorld(State*s){
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();auto*img=static_cast<T3DImagery*>(s->owner->GetImagery());hmm_mat4 m;Pose(*s,1,s->owner->GetFrame(),m);int draws=0;
 for(size_t slot=0;slot<s->particles.Capacity();++slot){if(!s->particles.Particles()[slot].alive)continue;const auto sample=s->particles.SampleRender(slot,[](int a,int b){return random(a,b);});if(s->trace){const auto&v=s->particles.Particles()[slot];log_info("[sgeyser-particle] tick=%d slot=%zu pos=%.9g,%.9g,%.9g vel=%.9g,%.9g,%.9g",s->ticks,slot,v.position[0],v.position[1],v.position[2],v.velocity[0],v.velocity[1],v.velocity[2]);}if(sample.scale<=1e-5f)continue;
  const hmm_vec3 p={sample.position[0],sample.position[1],sample.position[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},scale={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(m,p,r,scale);
  SQuadDrawItem q={};q.retail_software_projection=StartupVfxNativeDomain;q.key.texture=img->GetTextureHandle(0);q.key.blend=uint8_t(EFxBlend::AdditiveStraight);q.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);q.light_mode=EFxLightMode::Unlit;q.retail_texture=1;q.retail_rgb565_additive=true;
  for(int c=0;c<3;++c)q.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;q.color_rgba[3]=sample.alpha;
  constexpr int corners[]={0,1,3,2};for(int c=0;c<4;++c){const auto&vert=s->steam[corners[c]];hmm_vec3 w;MtxTransform(&m,&vert.pos,&w);q.world_pos[c][0]=w.X;q.world_pos[c][1]=w.Y;q.world_pos[c][2]=StartupVfxNativeDomain?w.Z:p.Z+(w.Z-p.Z)*WORLD3D_Z_SCALE;q.uv[c][0]=vert.tu;q.uv[c][1]=vert.tv;}Renderer->SubmitFxQuad(q);++draws;
 }
 if(Pose(*s,2,s->owner->GetFrame(),m)){hmm_mat4 root,world;const auto&p=IntPos(s->owner);if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&m,&root);
  SHelperMeshSubmit q={};q.mesh=s->blast;q.retail_lighting=1;q.additive_blend=true;q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.f:1.f/WORLD3D_Z_SCALE;const auto&mat=s->material.matdesc;std::memcpy(q.diffuse,&mat.diffuse,16);std::memcpy(q.ambient,&mat.ambient,16);std::memcpy(q.specular,&mat.specular,16);std::memcpy(q.emissive,&mat.emissive,16);q.power=mat.power;for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(q);
 }
 log_info("[sgeyser-phase] tick=%d frame=%d particles=%d rng=%llu credit=%.9g",s->ticks,s->owner->GetFrame(),draws,static_cast<unsigned long long>(RandomDraws()-s->initial_draws),s->particles.EmissionCredit());
}
void Destroy(State*s){delete s;}
}
