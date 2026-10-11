#include "barrierpreview.h"
#include "../barriercore.h"
#include "../barrierlegacykeys.h"
#include "../3dimage.h"
#include "../effect.h"
#include "../logging.h"
#include "../meshextract.h"
#include "../retailsoftwaretransform.h"
#include "../testconfig.h"
#include "../time.h"
#include <cmath>
#include <memory>
#include <vector>

namespace barrier_preview {
struct State {
    std::unique_ptr<TObjectInstance> owner;
    retail_barrier::State core;
    MeshHandle mesh=0;
    double accumulated=0;
    float uv=0;
    int ticks=0, renders=0;
    int64_t rendered=-1;
    ~State(){if(Renderer && mesh)Renderer->ReleaseMeshAssetRef(mesh);}
};

State* Spawn(uint32_t type_id,const S3DPoint& origin) {
    if(!Renderer || (type_id!=0x224a5adfu && type_id!=0x224b5adcu))return nullptr;
    auto s=std::make_unique<State>();
    auto* cl=TObjectClass::GetClass(OBJCLASS_EFFECT);
    const int type=cl?cl->FindObjType(type_id):-1;
    if(type<0)return nullptr;
    SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;
    s->owner.reset(cl->NewObject(&d));
    auto* im=s->owner?dynamic_cast<T3DImagery*>(s->owner->GetImagery()):nullptr;
    if(!im || s->owner->ObjId()!=type_id || !barrier_legacy::Matches(
        im->GetResFilename(),static_cast<SOld3DImageryBody*>(im->GetBody()),
        im->GetResSize(),im->NumStates(),im->GetAniLength(0),im->GetAniFlags(0)))return nullptr;
    s->owner->Face(0);s->owner->OnScreen();
    std::vector<SMeshVertex> vertices;
    std::vector<uint16_t> indices;
    if(!ExtractSubMeshTextureSlot(im,0,1,vertices,indices) ||
       vertices.size()!=24 || indices.size()!=36 || im->GetTextureHandle(0)==kInvalidTexture)return nullptr;
    std::vector<retail_barrier::Vec3> positions;
    for(const auto& v:vertices)positions.push_back({v.pos[0],v.pos[1],v.pos[2]});
    s->core.Initialize(positions.data(),positions.size());
    s->mesh=Renderer->RegisterMeshAsset(0x4241525200000000ull+type_id,
        vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),im->GetTextureHandle(0));
    if(!s->mesh)return nullptr;
    Renderer->AddMeshAssetRef(s->mesh);
    log_info("[barrier] typed_id=%08x vertices=24 faces=12 pool=30 zero_y_flares_rejected=1",type_id);
    log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT exact Barrier",int(StartupVfxNativeDomain));
    return s.release();
}

bool Alive(const State* s){return s && s->owner && !(s->owner->GetFlags()&OF_KILL);}
void Advance(State* s,double seconds) {
    if(!Alive(s) || !std::isfinite(seconds) || seconds<=0)return;
    s->accumulated+=seconds;
    while(s->accumulated+1e-10>=TTime::LegacyFrameSeconds) {
        s->accumulated-=TTime::LegacyFrameSeconds;
        s->owner->NextFrame();s->owner->Pulse();
        if(!Alive(s))return;
        s->owner->SetCommandDone(false);
        s->core.Advance([](int low,int high){return random(low,high);});
        ++s->ticks;
    }
}

void SubmitWorld(State* s) {
    if(!Alive(s) || !Renderer || s->rendered==TTime::FrameCount())return;
    s->rendered=TTime::FrameCount();++s->renders;s->uv+=.01f;
    auto* im=static_cast<T3DImagery*>(s->owner->GetImagery());
    const auto& p=s->owner->Pos();hmm_mat4 root;
    if(StartupVfxNativeDomain)BuildRetailSoftwareOwner(root,p.x,p.y,p.z,uint8_t(s->owner->GetFace()));
    else root=s->owner->Transform().Matrix();
    S3DAnimObj o={};o.objnum=o.animtrack=0;o.flags=OBJ3D_POS1;
    // Original Render4e7a10 sets wall position to zero and POS1 bypasses
    // the authored pose, producing a local identity. Zero-Y flares reject.
    if(!im->CalcObjectMatrix(&o,0,0,&root,false))return;
    hmm_mat4 world;MtxMultiply(&world,&o.matrix,&root);
    SHelperMeshSubmit q={};q.mesh=s->mesh;q.retail_lighting=3;
    q.retail_source_alpha=true;q.retail_alpha_source_quantize=StartupVfxNativeDomain;
    q.retail_software_projection=StartupVfxNativeDomain;
    q.uv_offset[0]=q.uv_offset[1]=s->uv;
    for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
    Renderer->SubmitHelperMesh(q);
    if(s->ticks<=6 || s->ticks==16 || s->ticks==30 || s->ticks==60 || s->ticks==90)
        log_info("[barrier-phase] tick=%d render=%d uv=%.9g delay=%d",s->ticks,s->renders,s->uv,s->core.delay);
}
void Destroy(State* s){delete s;}
}
