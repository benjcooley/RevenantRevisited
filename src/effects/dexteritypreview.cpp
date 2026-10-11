#include "dexteritypreview.h"
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

namespace dexterity_preview {
struct State {
 std::unique_ptr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};
 MeshHandle stars[4]{};
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer)for(auto mesh:stars)if(mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());
 constexpr int emitters[4]={5,1,4,3};
 for(int i=0;i<4;++i){hmm_vec3 p{},r{},scale{};
  if(!img->GetUninterpolatedAniKey(emitters[i],0,frame,p,r,scale,true))return false;
  BuildRetailSpeedCameraMatrix(matrix,p,r,scale);auto& e=s.inputs.emitters[i];
  std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
  e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};
 }s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0x8cd3ea0fu):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=0x8cd3ea0fu)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=6 || img->NumStates()!=2 || img->GetAniLength(1)!=1 || img->GetAniFlags(1)!=0x2000 || img->GetAniLength(0)!=60 || img->GetAniFlags(0)!=0x2001 || img->NumTextures()!=2 || img->NumMaterials()!=3 || img->NumTags()!=1)return nullptr;
 constexpr const char* names[]={"#particle","#dexter02","*circle01","#dexter04","#dexter03","#dexter01"};
 for(int i=0;i<6;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),names[i]))return nullptr;
 const auto* tag=img->GetTag(0);
 constexpr auto literal="obj=(#dexter01,#dexter02,#dexter03,#dexter04),particle=#particle,pps=100,lifespan=8:10,initialvelocity=0:1,scale=[0:1,100:0],rlocalrotation=[0:(0,0,0),100:(0,15,0)],color=[0:(150,200,255),25:(1,139,255),100:(1,1,10)],spread=45,gravity=0.1,friction=0.1,scljitter=-0.5:0.5";
 if(!tag || tag->state || tag->frame!=10 || !tag->name || !tag->str || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 for(int i=0;i<2;++i){S3DTex texture={};img->GetTexture(i,&texture);const auto& f=texture.desc.pixelFormat;
  if(texture.numframes!=1 || texture.desc.width!=32 || texture.desc.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 constexpr int stars[4]={1,3,4,5};
 for(int i=0;i<4;++i){
  if(!ExtractSubMeshTextureSlot(img,stars[i],2,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
  s->stars[i]=Renderer->RegisterMeshAsset(0x4445585445000000ull+stars[i],verts.data(),4,indices.data(),6,img->GetTextureHandle(1));
  if(!s->stars[i])return nullptr;Renderer->AddMeshAssetRef(s->stars[i]);
 }
 if(!ExtractSubMeshTextureSlot(img,0,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(0,s->dust);s->inputs.emitters.resize(4);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT dexterity authored four stars+particles",int(StartupVfxNativeDomain));
 log_info("[dexterity] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=0",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
}
void Advance(State* s,double seconds)
{
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});++s->ticks;}
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
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();
 constexpr int stars[4]={1,3,4,5};
 for(int i=0;i<4;++i){hmm_vec3 p{},r{},scale{};if(!img->GetUninterpolatedAniKey(stars[i],0,s->owner->GetFrame(),p,r,scale,true))return;
  if(scale.X<=0 || scale.Y<=0 || scale.Z<=0)continue;
  BuildRetailSpeedCameraMatrix(local,p,r,scale);MtxMultiply(&world,&local,&root);
  SHelperMeshSubmit mesh={};mesh.mesh=s->stars[i];for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];
  mesh.additive_blend=draws>0;mesh.retail_lighting=1;mesh.retail_positive_face_cull=draws==0;mesh.retail_software_projection=StartupVfxNativeDomain;
  if(draws)Renderer->SubmitGoldFlareAfterFx(mesh);else Renderer->SubmitHelperMesh(mesh);
 }
 if(s->ticks<4 || s->ticks==6 || s->ticks==16 || s->ticks==28 || s->ticks==40 || s->ticks==90 || s->ticks==122 || s->ticks==60 || s->ticks==59 || s->ticks==61 || s->ticks==119 || s->ticks==120)log_info("[dexterity-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
