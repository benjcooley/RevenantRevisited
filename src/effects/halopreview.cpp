#include "halopreview.h"
#include "../halocore.h"
#include "../haloprofile.h"
#include "../effect.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include "../logging.h"
#include <cmath>
#include <cstdlib>
#include "../revutils.h"
#include <memory>
#include <vector>
// Restore the shipped exact owner factory; rendering remains explicitly controlled.
DEFINE_BUILDER("Halo", THaloEffect)
REGISTER_BUILDER(THaloEffect)
namespace halo_preview {
// Both source raster inputs are independently proven for this exact asset.
// Keep the four-policy diagnostic explicit; ordinary defaults select both.
static int RasterPolicy(){const char* p=std::getenv("REV_HALO_RASTER_DIAGNOSTIC");return p && p[0]>='0' && p[0]<='3' && !p[1]?p[0]-'0':3;}
struct State {
    State(int total,float step):core(total,step){}
    std::unique_ptr<TObjectInstance> owner;
    halo_core::State core;
    MeshHandle mesh{};
    double accumulator=0;
    int64_t rendered=-1;
    bool diagnostic=std::getenv("REV_HALO_RASTER_DIAGNOSTIC")!=nullptr;
    int raster_policy=RasterPolicy();
    ~State(){if(Renderer && mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};
static void ConfigureRaster(SHelperMeshSubmit& draw,const State* s){
 draw.retail_software_projection=StartupVfxNativeDomain;
 draw.retail_software_pixel_grid=StartupVfxNativeDomain && (s->raster_policy&1);
 draw.retail_alpha_source_quantize=StartupVfxNativeDomain && (s->raster_policy&2);
}
State* Spawn(const S3DPoint& origin,int totalframes,float step) {
    if(!Renderer || totalframes<=0 || !std::isfinite(step) || step<=0)return nullptr;
    auto s=std::make_unique<State>(totalframes,step);
    auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int type=cl?cl->FindObjType(0xdedbdbedu):-1;if(type<0){log_warn("[halo] missing exact type dedbdbed");return nullptr;}
    SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;
    s->owner.reset(cl->NewObject(&d));
    auto* image=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
    if(image)log_info("[halo-profile] id=%08x path=%s size=%u hash=%016llx states=%d frames=%d flags=%x",s->owner->ObjId(),image->GetResFilename(),unsigned(image->GetResSize()),(unsigned long long)halo_profile::Hash(image->GetBody(),image->GetResSize()),image->NumStates(),image->GetAniLength(0),image->GetAniFlags(0));
    if(!image || s->owner->ObjId()!=0xdedbdbedu || !halo_profile::Matches(image->GetResFilename(),image->GetBody(),image->GetResSize(),image->NumStates(),image->GetAniLength(0),image->GetAniFlags(0)))return nullptr;
    // Public API values are declared comparison inputs, not discovered caller constants.
    auto* owner=dynamic_cast<THaloEffect*>(s->owner.get());if(!owner){log_warn("[halo] exact owner class unsupported");return nullptr;}
    owner->InitParams(totalframes,step);owner->Face(0);owner->OnScreen();
    std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
    if(!ExtractSubMeshTextureSlot(image,0,1,vertices,indices) || vertices.size()!=12 || indices.size()!=36 || image->GetTextureHandle(0)==kInvalidTexture)return nullptr;
    s->mesh=Renderer->RegisterMeshAsset(0x48414c4f00000001ull,vertices.data(),12,indices.data(),36,image->GetTextureHandle(0));
    if(!s->mesh)return nullptr;Renderer->AddMeshAssetRef(s->mesh);
    log_info("[native-domain] enabled=%d camera_z=0 exactHalo publicParams",int(StartupVfxNativeDomain));
    log_info("[halo] typed_id=dedbdbed totalframes=%d step=%.9g declared_input=1 natural_caller_open=1",totalframes,double(step));
    return s.release();
}
bool Alive(State* s){return s && s->owner && !s->core.done && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds) {
    if(!Alive(s) || !std::isfinite(seconds) || seconds<=0)return;
    s->accumulator+=seconds;
    while(s->accumulator+1e-10>=TTime::LegacyFrameSeconds) {
        s->accumulator-=TTime::LegacyFrameSeconds;s->owner->NextFrame();s->owner->Pulse();
        if(!Alive(s))return;s->core.Advance();
        if(s->diagnostic)log_info("[halo-state] frame=%d scale=%.9g done=%d rng=%llu policy=%d",s->core.frame,s->core.scale,int(s->core.done),(unsigned long long)RandomDraws(),s->raster_policy);
        if(s->core.done){s->owner->SetFlags(s->owner->GetFlags()|OF_KILL);return;}
    }
}
void SubmitWorld(State* s) {
    if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;
    s->rendered=TTime::FrameCount();hmm_mat4 local;
    if(!s->core.Matrix(local))return;
    const auto& p=IntPos(s->owner);hmm_mat4 root,world;
    if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);else root=s->owner->Transform().Matrix();
    MtxMultiply(&world,&local,&root);
    SHelperMeshSubmit draw={};draw.mesh=s->mesh;draw.retail_lighting=3;
    draw.retail_source_alpha=true;draw.additive_blend=false;
    ConfigureRaster(draw,s);
    for(int r=0;r<4;++r)for(int c=0;c<4;++c)draw.world[r*4+c]=world.Elements[c][r];
    Renderer->SubmitHelperMesh(draw);
}
void Destroy(State* s){delete s;}
}
