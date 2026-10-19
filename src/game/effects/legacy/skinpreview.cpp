#include "game/effects/legacy/skinpreview.h"
#include "game/effects/effect.h"
#include "engine/fx/authoredpartsys.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "game/effects/speedauthoredmatrix.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace skin_preview {
struct State {
 TObjectPtr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};
 MeshHandle flare=0;
 double accumulator=0;
 bool iron=false;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer && flare)Renderer->ReleaseMeshAssetRef(flare);}
};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(1,0,frame,p,r,scale,true))return false;
 MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&scale);MtxTranslate(&matrix,&p);auto& e=s.inputs.emitters[0];
 std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
 e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin,bool iron)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();s->iron=iron;const uint32_t id=iron?0xad92bd34u:0xad92bd33u;auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(id):-1;if(type<0)return nullptr;
 SObjectState d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 if(!s->owner || s->owner->ObjId()!=id)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
 if(!img || img->NumObjects()!=3 || img->NumStates()!=1 || img->GetAniLength(0)!=30 || img->GetAniFlags(0)!=0x2001 || img->NumTextures()!=1 || img->NumMaterials()!=2 || img->NumTags()!=2)return nullptr;
 for(int i=0;i<3;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),i==0?"#particle":i==1?"stone":"#flare"))return nullptr;
 const auto* blend=img->GetTag(0);const auto* tag=img->GetTag(1);
 constexpr auto iron_tag="obj=(stone),particle=#particle,pps=25,initialvelocity=4:5,localrotation=[0:(0,0,0),100(0,0,0)],lifespan=10:10,color=[0:(255,255,255),10:(125,1,255),100:(0,0,0)],spread=360,azimuth=360,scale=[0:1,50:1.5,100:0],numtrails=3:3";
 constexpr auto stone_tag="obj=(stone),particle=#particle,pps=25,initialvelocity=4:5,localrotation=[0:(0,0,0),100(0,0,0)],lifespan=10:10,color=[0:(255,255,255),10:(255,200,1),75:(255,100,1),100:(0,0,0)],spread=360,azimuth=360,scale=[0:1,50:1.5,100:0],numtrails=3:3";
 const char* literal=iron?iron_tag:stone_tag;
 if(!blend || !tag || blend->state || blend->frame!=1 || tag->state || tag->frame!=5 || !blend->name || !blend->str || !tag->name || !tag->str || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litadd") || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 for(int i=0;i<1;++i){S3DTex texture={};img->GetTexture(i,&texture);const auto& f=texture.desc.pixelFormat;
  if(texture.numframes!=1 || texture.desc.width!=64 || texture.desc.height!=64 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,2,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 s->flare=Renderer->RegisterMeshAsset((iron?0x49534b494e000001ull:0x53544f4e45000001ull),verts.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->flare)return nullptr;Renderer->AddMeshAssetRef(s->flare);
 if(!ExtractSubMeshTextureSlot(img,0,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(0,s->dust);s->inputs.emitters.resize(1);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT skin authored flare+original trail history",int(StartupVfxNativeDomain));
 log_info("[skin] authored cached-emitter name=%s native_domain=%d quality=0 capacity=%zu ground=0",iron?"ironskin":"stoneskin",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
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
  const auto sample=s->particles.SampleRender(slot,[](int a,int b){return random(a,b);});auto history=s->particles.SampleTrailPositions(slot);if(sample.scale<=9.999999747378752e-6f)continue;
  // Original main draw precedes shifted history and duplicate current.
  std::vector<authored_partsys::Vec3> positions{sample.position};positions.insert(positions.end(),history.begin(),history.end());
  for(const auto& center:positions){
   hmm_mat4 matrix;const hmm_vec3 p={center[0],center[1],center[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},scale={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(matrix,p,r,scale);
   SQuadDrawItem item={};item.retail_software_projection=StartupVfxNativeDomain;item.key.texture=img->GetTextureHandle(0);item.key.blend=uint8_t(EFxBlend::AdditiveStraight);item.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);item.light_mode=EFxLightMode::Unlit;item.retail_texture=1;item.retail_rgb565_additive=true;
   for(int c=0;c<3;++c)item.color_rgba[c]=float((int(sample.color[c])&255)>>3)/31.f;item.color_rgba[3]=sample.alpha;
   // History stores original corrected Z. Invert that documented formula
   // only for the ordinary-world bridge; native-domain positions stay exact.
   const float world_z=center[2]*1.460000038146973f/(1.037999987602234f+center[2]*.0033333334140479565f*.009999999776482582f);
   constexpr int corners[4]={0,1,3,2};for(int c=0;c<4;++c){const auto& vert=s->dust[corners[c]];hmm_vec3 world;MtxTransform(&matrix,&vert.pos,&world);item.world_pos[c][0]=world.X;item.world_pos[c][1]=world.Y;item.world_pos[c][2]=StartupVfxNativeDomain?world.Z:world_z+(world.Z-center[2])*WORLD3D_Z_SCALE;item.uv[c][0]=vert.tu;item.uv[c][1]=vert.tv;}Renderer->SubmitFxQuad(item);++draws;
  }
 }
 hmm_mat4 local,root,world;if(!Pose(*s,s->owner->GetFrame(),local))return;const auto& origin=IntPos(s->owner);
 hmm_vec3 p{},r{},scale{};if(!img->GetUninterpolatedAniKey(2,0,s->owner->GetFrame(),p,r,scale,true))return;BuildRetailSpeedCameraMatrix(local,p,r,scale);
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&local,&root);
 SHelperMeshSubmit mesh={};mesh.mesh=s->flare;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];mesh.additive_blend=true;mesh.retail_lighting=1;mesh.retail_positive_face_cull=false;mesh.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitGoldFlareAfterFx(mesh);
 if(s->ticks<4 || s->ticks==5 || s->ticks==10 || s->ticks==20 || s->ticks==29 || s->ticks==30 || s->ticks==31 || s->ticks==45 || s->ticks==59 || s->ticks==60 || s->ticks==61 || s->ticks==90 || s->ticks==91)log_info("[skin-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
