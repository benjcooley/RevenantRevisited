#include "restorelifepreview.h"
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

namespace restorelife_preview {
struct State {
 std::unique_ptr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};

 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;

};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());
 for(int i=0;i<8;++i){hmm_vec3 p{},r{},scale{};
  if(!img->GetUninterpolatedAniKey(8+i,0,frame,p,r,scale,true))return false;
  MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&scale);MtxTranslate(&matrix,&p);auto& e=s.inputs.emitters[i];
  std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};
 }
 s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xb3ae0fedu):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0xb3ae0fedu)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=17 || img->NumStates()!=1 || img->GetAniLength(0)!=60 || img->GetAniFlags(0)!=0x2000 || img->NumTextures()!=1 || img->NumMaterials()!=2 || img->NumTags()!=1)return nullptr;
 constexpr const char* names[]={"*path01","*path02","*path03","*path04","*path05","*path06","*path07","*path08","glint01","glint02","glint03","glint04","glint05","glint06","glint07","glint08","#twinkle"};
 for(int i=0;i<17;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),names[i]))return nullptr;
 const auto* tag=img->GetTag(0);
 constexpr auto literal="obj=(glint01,glint02,glint03,glint04,glint05,glint06,glint07,glint08),particle=#twinkle,pps=400,lifespan=25:55,initialvelocity=3:4,scale=[0:4,5:2,100:0.5],localrotation=[0:(0,360,0),100:(0,540,0)],color=[0:(90,90,255),25:(18,18,185),100:(180,180,255)],spread=35,azimuth=35,gravity=0.04,friction=0.1,scljitter=-0.5:0.5";
 if(!tag || tag->state || tag->frame!=6 || !tag->name || !tag->str || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 S3DTex texture={};img->GetTexture(0,&texture);const auto& f=texture.desc.pixelFormat;
 if(texture.numframes!=1 || texture.desc.width!=32 || texture.desc.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask || img->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,16,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(16,s->dust);s->inputs.emitters.resize(8);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Restorelife authored eight-emitters+twinkle",int(StartupVfxNativeDomain));
 log_info("[restorelife] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=0",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
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
  SQuadDrawItem item={};item.retail_software_projection=StartupVfxNativeDomain;item.key.texture=img->GetTextureHandle(0);item.key.blend=uint8_t(EFxBlend::AdditiveStraight);item.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);item.light_mode=EFxLightMode::Unlit;item.retail_texture=1;item.retail_rgb565_additive=true;
  for(int c=0;c<3;++c)item.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;item.color_rgba[3]=sample.alpha;
  constexpr int corners[4]={0,1,3,2};for(int c=0;c<4;++c){const auto& vert=s->dust[corners[c]];hmm_vec3 world;MtxTransform(&matrix,&vert.pos,&world);item.world_pos[c][0]=world.X;item.world_pos[c][1]=world.Y;item.world_pos[c][2]=StartupVfxNativeDomain?world.Z:particles[slot].position[2]+(world.Z-sample.position[2])*WORLD3D_Z_SCALE;item.uv[c][0]=vert.tu;item.uv[c][1]=vert.tv;}Renderer->SubmitFxQuad(item);++draws;
 }
 hmm_mat4 local;if(!Pose(*s,s->owner->GetFrame(),local))return;
 if(s->ticks<4 || s->ticks==6 || s->ticks==9 || s->ticks==16 || s->ticks==28 || s->ticks==40 || s->ticks==50 || s->ticks==59)log_info("[restorelife-phase] tick=%d frame=%d draws=%d credit=%.9g",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit());
}
void Destroy(State* s){delete s;}
}
