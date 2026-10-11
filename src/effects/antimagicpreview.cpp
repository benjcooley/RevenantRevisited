#include "antimagicpreview.h"
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

namespace antimagic_preview {
struct State {
 std::unique_ptr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};
 MeshHandle spheres[3]{};
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer)for(auto mesh:spheres)if(mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(5,0,frame,p,r,scale,true))return false;
 BuildRetailSpeedCameraMatrix(matrix,p,r,scale);auto& e=s.inputs.emitters[0];
 std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
 e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xd3ae043au):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0xd3ae043au)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=6 || img->NumStates()!=1 || img->GetAniLength(0)!=300 || img->GetAniFlags(0)!=0x2000 || img->NumTextures()!=2 || img->NumMaterials()!=2 || img->NumTags()!=2)return nullptr;
 const char* names[]={"#rect","sphere03","sphere04","sphere05","*helix01","#cfire1"};for(int i=0;i<6;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),names[i]))return nullptr;
 const auto* blend=img->GetTag(0);const auto* tag=img->GetTag(1);
 constexpr auto literal="obj=(#cfire1),particle=#rect,pps=[0:0,19:0,20:50,270:50,271:0],lifespan=15:25,initialvelocity=1:2,scale=[0:1,100:0],localrotation=[0:(0,0,0),100:(0,90,0)],color=[0:(255,255,225),5:(255,225,200),25:(255,185,1),100:(10,1,1)],spread=60,azimuth=60,alpha=[0:1,100:0],gravity=0.25,scljitter=-0.25:0.25,bounce=-50:-50";
 if(!blend || !tag || blend->state || blend->frame!=1 || tag->state || tag->frame!=40 || !blend->name || !blend->str || !tag->name || !tag->str || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litadd") || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 for(int i=0;i<2;++i){S3DTex texture={};img->GetTexture(i,&texture);const auto& f=texture.desc.pixelFormat;
  if(texture.numframes!=1 || texture.desc.width!=uint32_t(i?128:64) || texture.desc.height!=uint32_t(i?128:64) || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 for(int i=0;i<3;++i){if(!ExtractSubMeshTextureSlot(img,i+1,2,verts,indices) || verts.size()!=size_t(i==2?27:39) || indices.size()!=size_t(i==2?108:144))return nullptr;
  s->spheres[i]=Renderer->RegisterMeshAsset(0x414e54494d470000ull+uint64_t(i),verts.data(),int(verts.size()),indices.data(),int(indices.size()),img->GetTextureHandle(1));if(!s->spheres[i])return nullptr;Renderer->AddMeshAssetRef(s->spheres[i]);}
 if(!ExtractSubMeshTextureSlot(img,0,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(0,s->dust);s->inputs.emitters.resize(1);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Antimagic authored spheres+sparks",int(StartupVfxNativeDomain));
 log_info("[antimagic] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=0",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
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
 hmm_mat4 local,root,world;if(!Pose(*s,s->owner->GetFrame(),local))return;const auto& origin=s->owner->Pos();
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&local,&root);
 for(int i=0;i<3;++i){hmm_vec3 p{},r{},scale{};if(!img->GetUninterpolatedAniKey(i+1,0,s->owner->GetFrame(),p,r,scale,true) || scale.X<=0 || scale.Y<=0 || scale.Z<=0)continue;
  MtxClear(&local);MtxRotateX(&local,r.X);MtxRotateY(&local,r.Y);MtxRotateZ(&local,r.Z);MtxScale(&local,&scale);MtxTranslate(&local,&p);MtxMultiply(&world,&local,&root);
  // Whole original mode16 sets D3DCULL_NONE (state22=1); these authored
  // sphere faces wind opposite the prior single-sided quad fixtures.
  SHelperMeshSubmit mesh={};mesh.mesh=s->spheres[i];for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];mesh.additive_blend=true;mesh.retail_lighting=1;mesh.retail_positive_face_cull=false;mesh.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitGoldFlareAfterFx(mesh);}

 if(s->ticks<4 || s->ticks==10 || s->ticks==19 || s->ticks==20 || s->ticks==21 || s->ticks==40 || s->ticks==80 || s->ticks==140 || s->ticks==200 || s->ticks==270 || s->ticks==280 || s->ticks==295 || s->ticks==299)log_info("[antimagic-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
