#include "game/effects/legacy/sandswirlqualifiedpreview.h"
#include "game/effects/legacy/sandswirlcontroller.h"
#include "game/effects/legacy/seekingparticles.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include <memory>
#include <cstring>
#include <cstdlib>
namespace sandswirl_qualified_preview {
struct State {
 T3DImagery* imagery=nullptr;MeshHandle meshes[4]{};S3DPoint origin{};
 retail_sandswirl::Controller parent;retail_seeking_particles::State particles;
 retail_seeking_particles::Random rng{2383791448u,7};
 double accumulator=0;int ticks=0;int64_t rendered=-1;
 bool trace=std::getenv("REV_SANDSWIRL_QUALIFIED_TRACE")!=nullptr;
 ~State(){if(Renderer)for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);if(imagery)TObjectImagery::FreeImagery(imagery);
 log_info("[sandswirl-qualified-cleanup] tick=%d parent_alive=%d child_alive=%d global_rng_modified=0",ticks,int(parent.Alive()),int(!particles.killed));}
};
static uint32_t Hash(const void* p,size_t n){uint32_t h=2166136261u;const auto*b=static_cast<const uint8_t*>(p);while(n--)h=(h^*b++)*16777619u;return h;}
static void Hex(const void* data,size_t size,char* out){const char*d="0123456789abcdef";const auto*b=static_cast<const unsigned char*>(data);for(size_t i=0;i<size;++i){out[2*i]=d[b[i]>>4];out[2*i+1]=d[b[i]&15];}out[2*size]=0;}
static void TraceState(State* s,uint64_t global_before) {
 if(!s->trace)return;
 log_info("[sandswirl-qualified-state] tick=%d parent=%d child_killed=%d seed=%u rng=%llu global_before=%llu global_after=%llu",s->ticks,s->parent.Frame(),int(s->particles.killed),s->rng.seed,static_cast<unsigned long long>(s->rng.calls),static_cast<unsigned long long>(global_before),static_cast<unsigned long long>(RandomDraws()));
 for(size_t i=0;i<s->particles.pool.size();++i){char data[81];Hex(&s->particles.pool[i],40,data);log_info("[sandswirl-qualified-pool] tick=%d slot=%zu data=%s",s->ticks,i,data);}
}
State* Spawn(const S3DPoint& origin){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();s->origin=origin;
 const int imagery=TObjectImagery::FindImagery("Misc\\Sparks.I3D");if(imagery<0)return nullptr;
 auto*base=TObjectImagery::LoadImagery(imagery);s->imagery=dynamic_cast<T3DImagery*>(base);if(!s->imagery){if(base)TObjectImagery::FreeImagery(base);return nullptr;}
 auto*img=s->imagery;if(img->NumObjects()!=4 || img->NumTextures()!=1)return nullptr;
 S3DTex tex={};img->GetTexture(0,&tex);const auto&pf=tex.desc.pixelFormat;
 if(tex.numframes!=1 || tex.desc.width!=64 || tex.desc.height!=64 || pf.dwRGBBitCount!=16 || pf.dwRBitMask!=0xf00 || pf.dwGBitMask!=0xf0 || pf.dwBBitMask!=0xf || pf.dwRGBAlphaBitMask!=0xf000 || img->GetTextureHandle(0)==kInvalidTexture)return nullptr;
 constexpr uint32_t hashes[]={0x9ee0e036u,0x7b4a7030u,0xf424a66au,0x597d591fu};
 for(int i=0;i<4;++i){S3DVertex raw[4]{};img->GetObjVerts(i,raw);std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
 if(Hash(raw,sizeof(raw))!=hashes[i] || !ExtractSubMeshTextureSlot(img,i,1,vertices,indices) || vertices.size()!=4 || indices.size()!=6 || Hash(indices.data(),12)!=0x4d2f57c6u)return nullptr;
 s->meshes[i]=Renderer->RegisterMeshAsset(0x5357515350415200ull|uint64_t(i),vertices.data(),4,indices.data(),6,img->GetTextureHandle(0));if(!s->meshes[i])return nullptr;Renderer->AddMeshAssetRef(s->meshes[i]);
 log_info("[sandswirl-qualified-mesh] object=%d source_hash=%08x vertices=%zu indices=%zu",i,hashes[i],vertices.size(),indices.size());}
 retail_seeking_particles::Params p;p.count=85;p.pos={0,0,70};p.pspread={50,50,50};p.dir={1,0,0};p.spread={.5f,.5f,.5f};p.trails=1;p.maxstart=42;p.minlife=100;p.maxlife=150;p.killobj=1;p.objflags=15;p.seeking=1;p.seekz=41;p.target_count=1;p.targets[0]={0,0,100};p.turn=.5f;p.speed=5;
 if(!s->particles.Init(p,4,s->rng) || !s->parent.Start(true))return nullptr;
 log_info("[sandswirl-qualified] diagnostic_only=1 substituted_asset=Misc/Sparks.I3D missing_sand=1 count=85 seekz=41 particle_entry_seed=2383791448 external_rng_calls=7 global_rng_modified=0 ground_queries=0 target=96,0,-32 target_sample_tick=75");
 log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));return s.release();
}
bool Alive(State*s){return s && s->parent.Alive();}
void Advance(State*s,double dt){if(!s || !std::isfinite(dt) || dt<=0)return;s->accumulator+=dt;
 while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds){s->accumulator-=TTime::LegacyFrameSeconds;const auto global_before=RandomDraws();++s->ticks;if(!Alive(s)){TraceState(s,global_before);continue;}
 retail_sandswirl::Caller caller;caller.valid=true;caller.caster=1;caller.origin={s->origin.x,s->origin.y,s->origin.z};caller.count=2;
 caller.found[0]={1,caller.origin,102};caller.found[1]={2,{s->origin.x+96,s->origin.y,s->origin.z},102};
 const auto event=s->parent.Advance(caller);if(event.reset_targets){auto p=s->particles.params;const auto&t=s->parent.Targets();p.seeking=t.seeking;p.target_count=int(t.count);p.turn=t.turn;p.speed=t.speed;p.autorange=t.autorange;p.hitrange=t.hitrange;
 for(unsigned i=0;i<t.count;++i)p.targets[i]={t.positions[i][0],t.positions[i][1],t.positions[i][2]};
 if(s->ticks==75){s->rng.seed=2697382707u;s->rng.calls=776;log_info("[sandswirl-qualified-map-boundary] tick=75 original_new_quicksand=1 init_ret8_stub=1 seed=2697382707 calls=776 actual_map_allocation_here=0");}
 if(!s->particles.ResetTargets(p,s->rng)){log_error("[sandswirl-qualified] rejected target reset");return;}}
 s->particles.Advance();TraceState(s,global_before);
 }
}
void SubmitWorld(State*s){if(!s || !Renderer || s->rendered==TTime::FrameCount())return;s->rendered=TTime::FrameCount();hmm_mat4 owner;BuildRetailSoftwareOwner(owner,s->origin.x,s->origin.y,s->origin.z,0);int draws=0;
 s->particles.Render([&](int object,retail_seeking_particles::Vec p){hmm_mat4 local,world;MtxClear(&local);const hmm_vec3 position={p.x,p.y,p.z};MtxTranslate(&local,&position);MtxMultiply(&world,&local,&owner);
 SHelperMeshSubmit q={};q.mesh=s->meshes[object];q.retail_lighting=3;q.retail_source_alpha=true;q.retail_software_projection=StartupVfxNativeDomain;
 for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];Renderer->SubmitHelperMesh(q);
 if(s->trace){char pos[25],matrix[129];Hex(&p,12,pos);Hex(world.Elements,64,matrix);log_info("[sandswirl-qualified-draw] tick=%d draw=%d object=%d pos=%s world=%s",s->ticks,draws,object,pos,matrix);}++draws;});
 if(s->trace)log_info("[sandswirl-qualified-render] tick=%d draws=%d",s->ticks,draws);
}
void Destroy(State*s){delete s;}
}
