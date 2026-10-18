#include "quickpreview.h"
#include "../effect.h"
#include "../authoredpartsys.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../speedauthoredmatrix.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace quick_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};
 struct Part {int object;MeshHandle mesh;};
 std::vector<Part> parts;
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer)for(auto&p:parts)Renderer->ReleaseMeshAssetRef(p.mesh);}
};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(6,0,frame,p,r,scale,true))return false;
 MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&scale);MtxTranslate(&matrix,&p);auto& e=s.inputs.emitters[0];
 std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
 e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xbabffaceu):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0xbabffaceu)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=8 || img->NumStates()!=1 || img->GetAniLength(0)!=56 || img->GetAniFlags(0)!=0x2000 || img->NumTextures()!=3 || img->NumMaterials()!=3 || img->NumTags()!=3)return nullptr;
 constexpr const char* names[]={"rectangle02","rectangle03","rectangle04","rectangle05","rectangle06","#sparks","fgeys","rectangle01"};
 for(int i=0;i<8;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),names[i]))return nullptr;
 const auto* tag=img->GetTag(1);
 constexpr auto literal="obj=(fgeys),particle=#sparks,pps=[0:0,5:100,50:100,56:0],lifespan=3:7,initialvelocity=-10:-20,scale=[0:0.25,100:0.1],color=[0:(155,155,125),100:(0,0,0)],spread=45,azimuth=45,alpha=[0:1,60:0.8,100:0],gravity=1,emittertype=circle,emittersize=75";
 if(!tag || tag->state || tag->frame!=2 || !tag->name || !tag->str || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 const auto* sound=img->GetTag(0);const auto* blend=img->GetTag(2);
 if(sound->state || sound->frame!=1 || std::strcmp(sound->name,"play") || std::strcmp(sound->str,"quicksand") || blend->state || blend->frame!=9 || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litalpha"))return nullptr;
 for(int i=0;i<3;++i){S3DTex t={};img->GetTexture(i,&t);const auto& f=t.desc.pixelFormat;const bool argb=i<2;
  if(t.numframes!=1 || t.desc.width!=uint32_t(argb?128:64) || t.desc.height!=uint32_t(argb?128:64) || f.dwRGBBitCount!=16 || f.dwRBitMask!=uint32_t(argb?0xf00:0xf800) || f.dwGBitMask!=uint32_t(argb?0xf0:0x7e0) || f.dwBBitMask!=uint32_t(argb?0xf:0x1f) || f.dwRGBAlphaBitMask!=uint32_t(argb?0xf000:0) || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 // The native material traversal emits these six layers after PartSys.
 for(int object:{0,1,7,2,3,4}){const int slot=object==0 || object==1 || object==7?1:2;
  if(!ExtractSubMeshTextureSlot(img,object,slot,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
  auto mesh=Renderer->RegisterMeshAsset(0x515549434b000000ull|uint64_t(object),verts.data(),4,indices.data(),6,img->GetTextureHandle(slot-1));if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);s->parts.push_back({object,mesh});}
 if(!ExtractSubMeshTextureSlot(img,5,3,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(5,s->dust);s->inputs.emitters.resize(1);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Quick authored layers+sparks",int(StartupVfxNativeDomain));
 log_info("[quick] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=0",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
}
void Advance(State* s,double seconds)
{
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();++s->ticks;if(!Alive(s))return;s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});}
}
bool Alive(State* s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void SubmitWorld(State* s)
{
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 auto* img=static_cast<T3DImagery*>(s->owner->GetImagery());const auto& particles=s->particles.Particles();int draws=0;
 for(size_t slot=0;slot<particles.size();++slot){if(!particles[slot].alive)continue;
  const auto sample=s->particles.SampleRender(slot,[](int a,int b){return random(a,b);});if(sample.scale<=9.999999747378752e-6f)continue;
  hmm_mat4 matrix;const hmm_vec3 p={sample.position[0],sample.position[1],sample.position[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},scale={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(matrix,p,r,scale);
  SQuadDrawItem item={};item.retail_software_projection=StartupVfxNativeDomain;item.key.texture=img->GetTextureHandle(2);item.key.blend=uint8_t(EFxBlend::AdditiveStraight);item.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);item.light_mode=EFxLightMode::Unlit;item.retail_texture=1;item.retail_rgb565_additive=true;
  for(int c=0;c<3;++c)item.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;item.color_rgba[3]=sample.alpha;
  constexpr int corners[4]={0,1,3,2};for(int c=0;c<4;++c){const auto& vert=s->dust[corners[c]];hmm_vec3 world;MtxTransform(&matrix,&vert.pos,&world);item.world_pos[c][0]=world.X;item.world_pos[c][1]=world.Y;item.world_pos[c][2]=StartupVfxNativeDomain?world.Z:particles[slot].position[2]+(world.Z-sample.position[2])*WORLD3D_Z_SCALE;item.uv[c][0]=vert.tu;item.uv[c][1]=vert.tv;}Renderer->SubmitFxQuad(item);++draws;
 }
 hmm_mat4 local,root,world;if(!Pose(*s,s->owner->GetFrame(),local))return;const auto& origin=IntPos(s->owner);
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&local,&root);
 for(const auto& part:s->parts){hmm_vec3 p{},r{},scale{};if(!img->GetUninterpolatedAniKey(part.object,0,s->owner->GetFrame(),p,r,scale,true) || scale.X<=0 || scale.Y<=0 || scale.Z<=0)continue;
  MtxClear(&local);MtxRotateX(&local,r.X);MtxRotateY(&local,r.Y);MtxRotateZ(&local,r.Z);MtxScale(&local,&scale);MtxTranslate(&local,&p);MtxMultiply(&world,&local,&root);
  SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];mesh.retail_lighting=3;mesh.retail_source_alpha=true;mesh.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitRetailSourceAlphaAfterFx(mesh);
 }
 if(s->ticks<4 || s->ticks==20 || s->ticks==6 || s->ticks==9 || s->ticks==16 || s->ticks==28 || s->ticks==40 || s->ticks==50 || s->ticks==55)log_info("[quick-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
