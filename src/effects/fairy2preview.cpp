#include "fairy2preview.h"
#include "../effect.h"
#include "../authoredpartsys.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../speedauthoredmatrix.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <cctype>
#include <string>
#include <cstring>
#include <memory>
#include <vector>

namespace fairy2_preview {

static uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const unsigned char*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
static bool ExactResources(T3DImagery* img)
{
 const char* path=img->GetResFilename();if(!path)return false;std::string normalized(path);for(char&c:normalized)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));
 constexpr const char* suffix="misc/fairy2.i3d";const size_t size=std::strlen(suffix);if(normalized.size()<size || normalized.compare(normalized.size()-size,size,suffix) || (normalized.size()!=size && normalized[normalized.size()-size-1]!='/'))return false;
 
 const int counts[]={836,4,4},materials[]={0,1,2},keys[]={9,1273,404};
 const uint64_t vertices[]={0x4e00e5c162bcb007ull,0x9d435b8eb12ae9b3ull,0xdc112e4b3ef27c73ull},keyhash[]={0x728d2ab43f6737f8ull,0xd19e2e3f055b46a8ull,0x8c9e8aa728a5cc0aull};
 for(int i=0;i<3;++i){S3DObj o={};img->GetObject(i,&o);if(o.material!=materials[i] || !o.anikeys[0] || o.numanikeys[0]!=keys[i] || Hash(o.anikeys[0],keys[i]*4)!=keyhash[i] || img->NumObjVerts(i)!=counts[i] || img->NumObjFaces(i)!=(i?2:0))return false;std::vector<S3DVertex> v(static_cast<size_t>(counts[i]));img->GetObjVerts(i,v.data());if(Hash(v.data(),v.size()*sizeof(S3DVertex))!=vertices[i])return false;
 if(i){S3DFace faces[2];int starts[MAXTEXTURES+1]={},bins[MAXTEXTURES+1]={};img->GetObjFaces(i,faces,starts,bins);if(bins[i]!=2 || bins[0] || bins[3-i] || Hash(faces,sizeof(faces))!=0x8de70ea438f81391ull)return false;}}
 const uint64_t hashes[]={0x8237bf10191e1de0ull,0xd6001c890aae012dull,0x2d6e7c4f0d54f390ull};for(int i=0;i<3;++i){S3DMat m={};img->GetMaterial(i,&m);if(m.texture!=i-1 || Hash(&m.matdesc.diffuse,17*sizeof(float))!=hashes[i])return false;}return true;
}
struct State {
 std::unique_ptr<TObjectInstance> owner;
 authored_partsys::State particles;
 authored_partsys::TickInputs inputs;
 S3DVertex dust[4]{};
 MeshHandle fairy=0;
 double accumulator=0;
 int ticks=0;
 int64_t rendered=-1;
 ~State(){if(Renderer && fairy)Renderer->ReleaseMeshAssetRef(fairy);}
};
static bool Pose(State& s,int frame,hmm_mat4& matrix)
{
 auto* img=static_cast<T3DImagery*>(s.owner->GetImagery());hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(1,0,frame,p,r,scale,true))return false;
 BuildRetailSpeedCameraMatrix(matrix,p,r,scale);auto& e=s.inputs.emitters[0];
 std::memcpy(e.matrix.data(),matrix.Elements,64);e.origin={matrix.Elements[3][0],matrix.Elements[3][1],matrix.Elements[3][2]};
 e.position={p.X,p.Y,p.Z};e.scale={scale.X,scale.Y,scale.Z};s.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& origin)
{
 if(!Renderer)return nullptr;
 auto s=std::make_unique<State>();auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cl?cl->FindObjType(0xad92bd26u):-1;if(type<0)return nullptr;
 SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));
 
 if(!s->owner || s->owner->ObjId()!=0xad92bd26u)return nullptr;
 s->owner->OnScreen();auto* img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());
  
 if(!img || img->NumObjects()!=3 || img->NumStates()!=2 || img->GetAniLength(0)!=399 || img->GetAniFlags(0)!=0x2000 || img->GetAniLength(1)!=1 || img->NumTextures()!=2 || img->NumMaterials()!=3 || img->NumTags()!=2)return nullptr;
 if(!ExactResources(img)){return nullptr;}
 for(int i=0;i<3;++i)if(img->GetObjectParent(i,0)!=-1 || std::strcmp(img->GetObjectName(i),i==0?"*path":i==1?"#fairy2":"#dust2"))return nullptr;
 const auto* blend=img->GetTag(0);const auto* tag=img->GetTag(1);
 constexpr auto literal="obj=(#fairy2),particle=#dust2,pps=[0:20,100:30,120:200,260:200,300:40,300:25],initialvelocity=1:2,lifespan=15:25,localrotation=[0:(0,0,0),100:(0,360,0)],friction=[0:0],gravity=[0:0.5],color=[0:(0,0,255),10:(255,185,10),30:(255,0,0),50:(0,0,250),95:(255,255,255),100:(0,0,0)],spread=[0:20,60:20,120:360,240:360,280:20,300:20],azimuth=360,scale=[0:2,50:6,100:0.5],scljitter=0.1:1,relvel=0.2";
 if(!blend || !tag || blend->state || blend->frame!=2 || tag->state || tag->frame!=24 || !blend->name || !blend->str || !tag->name || !tag->str || std::strcmp(blend->name,"blendcont") || std::strcmp(blend->str,"litadd") || std::strcmp(tag->name,"partsys") || std::strcmp(tag->str,literal))return nullptr;
 for(int i=0;i<2;++i){S3DTex texture={};img->GetTexture(i,&texture);const auto& f=texture.desc.pixelFormat;
  if(texture.numframes!=1 || texture.desc.width!=32 || texture.desc.height!=32 || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=0x1f || f.dwRGBAlphaBitMask || img->GetTextureHandle(i)==kInvalidTexture)return nullptr;}
 std::vector<SMeshVertex> verts;std::vector<uint16_t> indices;
 if(!ExtractSubMeshTextureSlot(img,1,1,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 s->fairy=Renderer->RegisterMeshAsset(0x4641495259320001ull,verts.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->fairy)return nullptr;Renderer->AddMeshAssetRef(s->fairy);
 if(!ExtractSubMeshTextureSlot(img,2,2,verts,indices) || verts.size()!=4 || indices.size()!=6)return nullptr;
 img->GetObjVerts(2,s->dust);s->inputs.emitters.resize(1);s->inputs.owner_position={float(origin.x),float(origin.y),float(origin.z)};s->inputs.ground_height=[](int,int,int){return 0;};
 hmm_mat4 local; if(!Pose(*s,0,local))return nullptr;hmm_vec3 p{},r{},scale{};
 if(!img->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
 s->inputs.has_object_zero_origin=true;s->inputs.object_zero_origin={p.X,p.Y,p.Z};
 authored_partsys::Definition definition;std::string diagnostic;
 if(!authored_partsys::ParseDefinition(tag->str,definition,diagnostic) || !s->particles.Initialize(definition,s->inputs,0,diagnostic))return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Fairy2 authored base+dust",int(StartupVfxNativeDomain));
 log_info("[fairy2] authored cached-emitter native_domain=%d quality=0 capacity=%zu ground=0",int(StartupVfxNativeDomain),s->particles.Capacity());return s.release();
}
void Advance(State* s,double seconds)
{
 if(!s || !std::isfinite(seconds) || seconds<=0)return;s->accumulator+=seconds;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;if(!Alive(s))return;s->owner->NextFrame();s->owner->Pulse();if(!Alive(s))return;s->particles.Advance(s->inputs,[](int a,int b){return random(a,b);});++s->ticks;}
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
 hmm_mat4 local,root,world;if(!Pose(*s,s->owner->GetFrame(),local))return;const auto& origin=IntPos(s->owner);
 if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,origin.x,origin.y,origin.z,0);else root=s->owner->Transform().Matrix();MtxMultiply(&world,&local,&root);
 SHelperMeshSubmit mesh={};mesh.mesh=s->fairy;for(int row=0;row<4;++row)for(int col=0;col<4;++col)mesh.world[row*4+col]=world.Elements[col][row];mesh.additive_blend=true;mesh.retail_lighting=1;mesh.retail_positive_face_cull=true;mesh.retail_software_projection=StartupVfxNativeDomain;Renderer->SubmitGoldFlareAfterFx(mesh);
 if(s->ticks<4 || s->ticks==6 || s->ticks==16 || s->ticks==30 || s->ticks==60 || s->ticks==120 || s->ticks==200 || s->ticks==300 || s->ticks==398 || s->ticks==399 || s->ticks==400)log_info("[fairy2-phase] tick=%d frame=%d dust=%d credit=%.9g emitter=(%.9g,%.9g,%.9g)",s->ticks,s->owner->GetFrame(),draws,s->particles.EmissionCredit(),s->inputs.emitters[0].origin[0],s->inputs.emitters[0].origin[1],s->inputs.emitters[0].origin[2]);
}
void Destroy(State* s){delete s;}
}
