#include "swamppreview.h"
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

namespace swamp_preview {
struct State {
 std::unique_ptr<TObjectInstance> owner;
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
 if(!img->GetUninterpolatedAniKey(4,0,frame,p,r,scale,true))return false;
 MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&scale);MtxTranslate(&matrix,&p);auto& e=s.inputs.emitters[0];
 std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
 e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xbecffedau):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0xbecffedau)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=30 || img->NumStates()!=1 || img->GetAniLength(0)!=56 || img->GetAniFlags(0)!=0x2000 || img->NumTextures()!=3 || img->NumMaterials()!=3 || img->NumTags()!=2)return nullptr;
 constexpr const char* names[]={"rectangle01","rectangle02","rectangle03","#sparks","fgeys","rectangle04","rectangle05","rectangle06","rectangle07","rectangle08","rectangle09","bubble05","bubble01","bubble02","bubble03","bubble04","bubble06","bubble07","bubble08","bubble09","bubble10","bubble11","bubble12","bubble13","bubble14","bubble15","bubble16","bubble17","bubble18","bubble19"};
 constexpr int keyCounts[]={77,77,77,9,6,77,77,77,77,77,77,34,38,43,39,34,39,38,29,51,39,32,33,39,51,51,39,38,29,28};
 constexpr uint32_t keyHashes[]={0xb4e5a135u,0xf2d9a5aeu,0xf9d181e6u,0x42f7ff1du,0xbbb06f21u,0xb052b179u,0xee4effe1u,0xb6cd5fd2u,0x2246215fu,0xce79efd4u,0x23c3180cu,0xd7aa2e6au,0xbda5ea61u,0xdc1cd545u,0xc2fd5b1fu,0xbce84b7eu,0xcbc04ef0u,0x8f10b451u,0xd01e2c2au,0x1f2ba06u,0x920465ebu,0xd85e781du,0xcbd0b489u,0x59e2e78du,0x2f3fc1b2u,0xafed3949u,0x80477285u,0x914c7du,0x2544e073u,0xcfec33f8u};
 for(int i=0;i<30;++i){S3DObj value={};img->GetObject(i,&value);const auto* object=&value;
  if(!object || img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),names[i]) || object->numanikeys[0]!=keyCounts[i])return nullptr;
  uint32_t hash=2166136261u;const auto* bytes=reinterpret_cast<const unsigned char*>(object->anikeys[0]);
  for(int n=0;n<keyCounts[i]*4;++n)hash=(hash^bytes[n])*16777619u;
  if(hash!=keyHashes[i])return nullptr;
 }
 const auto* tag=img->GetTag(0);const auto* blend=img->GetTag(1);
 constexpr auto literal="obj=(fgeys),particle=#sparks,pps=[0:0,5:50,50:50,56:0],lifespan=10:15,initialvelocity=-5:-10,scale=[0:0.25,100:0.1],color=[0:(155,255,125),100:(0,100,0)],spread=45,azimuth=45,alpha=[0:1,60:0.8,100:0],gravity=1,emittertype=circle,emittersize=75";
 if(!tag || !blend || tag->state || tag->frame!=2 || !tag->name || !tag->str || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal) || blend->state || blend->frame!=9 || !blend->name || !blend->str || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litalpha"))return nullptr;
 for(int i=0;i<3;++i){S3DTex t={};img->GetTexture(i,&t);const auto& f=t.desc.pixelFormat;const bool argb=i!=1;
  if(t.numframes!=1 || t.desc.width!=uint32_t(i?64:128) || t.desc.height!=uint32_t(i?64:128) || f.dwRGBBitCount!=16 || f.dwRBitMask!=uint32_t(argb?0xf00:0xf800) || f.dwGBitMask!=uint32_t(argb?0xf0:0x7e0) || f.dwBBitMask!=uint32_t(argb?0xf:0x1f) || f.dwRGBAlphaBitMask!=uint32_t(argb?0xf000:0) || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 // Original texture-major traversal: nine ground cards, then eighteen bubbles.
 // #sparks is the particle prototype; bubble19 has no authored faces.
 for(int object=0;object<29;++object){if(object==3 || object==4)continue;const int slot=object<11?1:3;
  if(!ExtractSubMeshTextureSlot(img,object,slot,verts,indices) || verts.size()!=size_t(object<11?4:65) || indices.size()!=size_t(object<11?6:object==28?288:336))return nullptr;
  auto mesh=Renderer->RegisterMeshAsset(0x5357414d50000000ull|uint64_t(object),verts.data(),int(verts.size()),indices.data(),int(indices.size()),img->GetTextureHandle(slot-1));if(!mesh)return nullptr;Renderer->AddMeshAssetRef(mesh);s->parts.push_back({object,mesh});}
 if(!ExtractSubMeshTextureSlot(img,3,2,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(3,s->dust);s->inputs.emitters.resize(1);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 1;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Swamp authored layers+sparks",int(StartupVfxNativeDomain));
 log_info("[swamp] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=1",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
}
void Advance(State* s,double seconds)
{
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 // Generic effect Pulse runs its animator controllers before testing command completion.
 // Preserve that final update on tick56, but never simulate after OF_KILL.
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();++s->ticks;s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});}
}
bool Alive(State* s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void SubmitWorld(State* s)
{
 if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();
 auto* img=static_cast<T3DImagery*>(s->owner->GetImagery());const auto& particles=s->particles.Particles();int draws=0;
 for(size_t slot=0;slot<particles.size();++slot){if(!particles[slot].alive)continue;
  const auto sample=s->particles.SampleRender(slot,[](int a,int b){return random(a,b);});if(sample.scale<=9.999999747378752e-6f)continue;
  hmm_mat4 matrix;const hmm_vec3 p={sample.position[0],sample.position[1],sample.position[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},scale={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(matrix,p,r,scale);
  SQuadDrawItem item={};item.retail_software_projection=StartupVfxNativeDomain;item.key.texture=img->GetTextureHandle(1);item.key.blend=uint8_t(EFxBlend::AdditiveStraight);item.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);item.light_mode=EFxLightMode::Unlit;item.retail_texture=1;item.retail_rgb565_additive=true;
  for(int c=0;c<3;++c)item.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;item.color_rgba[3]=sample.alpha;
  constexpr int corners[4]={0,1,3,2};for(int c=0;c<4;++c){const auto& vert=s->dust[corners[c]];hmm_vec3 world;MtxTransform(&matrix,&vert.pos,&world);item.world_pos[c][0]=world.X;item.world_pos[c][1]=world.Y;item.world_pos[c][2]=StartupVfxNativeDomain?world.Z:particles[slot].position[2]+(world.Z-sample.position[2])*WORLD3D_Z_SCALE;item.uv[c][0]=vert.tu;item.uv[c][1]=vert.tv;}Renderer->SubmitFxQuad(item);++draws;
 }
 hmm_mat4 local,root,world;if(!Pose(*s,s->owner->GetFrame(),local))return;const auto& origin=s->owner->Pos();
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&local,&root);
 for(const auto& part:s->parts){hmm_vec3 p{},r{},scale{};if(!img->GetUninterpolatedAniKey(part.object,0,s->owner->GetFrame(),p,r,scale,true) || scale.X<=1e-5f || scale.Y<=1e-5f || scale.Z<=1e-5f)continue;
  MtxClear(&local);MtxRotateX(&local,r.X);MtxRotateY(&local,r.Y);MtxRotateZ(&local,r.Z);MtxScale(&local,&scale);MtxTranslate(&local,&p);MtxMultiply(&world,&local,&root);
  SHelperMeshSubmit mesh={};mesh.mesh=part.mesh;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];mesh.retail_lighting=3;mesh.retail_source_alpha=true;mesh.retail_alpha_source_quantize=StartupVfxNativeDomain;mesh.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitRetailSourceAlphaAfterFx(mesh);
 }
 if(s->ticks<4 || s->ticks==20 || s->ticks==6 || s->ticks==9 || s->ticks==16 || s->ticks==28 || s->ticks==40 || s->ticks==50 || s->ticks==55)log_info("[swamp-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
