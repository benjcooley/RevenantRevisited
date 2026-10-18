#include "game/effects/legacy/nakrnothoriginal.h"
#include "game/imagery/3dimage.h"
#include "game/effects/effect.h"
#include "engine/core/logging.h"
#include "game/imagery/meshextract.h"
#include "engine/render/renderer.h"
#include "engine/render/retailsoftwaretransform.h"
#include "game/effects/nakrnothprofile.h"
#include "game/app/testconfig.h"
#include "engine/core/revtime.h"
#include "engine/fx/authoredpartsys.h"
#include "game/effects/speedauthoredmatrix.h"
#include "game/effects/cometsoftwarebounds.h"
#include <array>
#include <cmath>
#include <cstring>
#include <memory>

namespace nakrnoth_original {
struct State {
    TObjectPtr<TObjectInstance> owner;
    struct Part {MeshHandle mesh=0;S3DMat material{};int object=0,slot=0;std::vector<std::array<hmm_vec3,3>> triangles;};
    std::vector<Part> parts;
    S3DPoint source{};
    authored_partsys::State particles;authored_partsys::TickInputs inputs;S3DVertex dust[4]{};int64_t rendered=-1;int ticks=0;
    double accumulated=0;
    ~State(){if(Renderer)for(auto& p:parts)if(p.mesh)Renderer->ReleaseMeshAssetRef(p.mesh);}

};
static bool ValidateNakrnoth(T3DImagery& image)
{
    if(image.NumStates()!=1 || image.NumObjects()!=63 || image.NumTextures()!=5 ||
       image.NumMaterials()!=6 || image.NumTags()!=2 || image.GetAniLength(0)!=775 || image.GetAniFlags(0)!=8192)return false;
    const auto hash=[](const void* data,size_t n){uint64_t h=1469598103934665603ull;
        const auto* b=static_cast<const uint8_t*>(data);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;};
    for(int i=0;i<63;++i){S3DObj o{};image.GetObject(i,&o);const auto& p=retail_nakrnoth::objects[i];
        if(std::strcmp(o.name,p.name) || o.material!=p.material || o.numverts!=p.vertices || o.numfaces!=p.faces)return false;
        for(int st=0;st<1;++st)if(o.parent[st]!=-1 || !o.anikeys[st] || o.numanikeys[st]!=p.keys || hash(o.anikeys[st],p.keys*sizeof(SAniKey32))!=p.key_hash)return false;
        for(int slot=0;slot<6;++slot)if(o.numtexfaces[slot]!=p.bins[slot])return false;
        std::vector<S3DVertex> v(size_t(p.vertices));std::vector<S3DFace> f(size_t(p.faces));image.GetObjVerts(i,v.data(),0,0);image.GetObjFaces(i,f.data());
        if(hash(v.data(),v.size()*sizeof(S3DVertex))!=p.vertex_hash || hash(f.data(),f.size()*sizeof(S3DFace))!=p.face_hash)return false;
    }
    const auto* tag=image.GetTag(0);if(!tag || tag->state!=0 || tag->frame!=17 || !tag->name || !tag->str || std::strcmp(tag->name,"blendcont") || std::strcmp(tag->str,"litadd"))return false;
    for(int i=0;i<6;++i){S3DMat m{};image.GetMaterial(i,&m);if(m.texture!=retail_nakrnoth::material_textures[i] || hash(&m.matdesc.diffuse,17*sizeof(float))!=retail_nakrnoth::materials[i])return false;}
    for(int i=0;i<5;++i){S3DTex t{};image.GetTexture(i,&t);const auto& f=t.desc.pixelFormat;
        if(t.numframes!=1 || t.desc.width!=retail_nakrnoth::sizes[i][0] || t.desc.height!=retail_nakrnoth::sizes[i][1] || f.dwRGBBitCount!=16 || f.dwRBitMask!=0xf800 || f.dwGBitMask!=0x7e0 || f.dwBBitMask!=31 || f.dwRGBAlphaBitMask)return false;}
    const auto* particles=image.GetTag(1);
    constexpr auto literal="obj=(#shine01,#shine02,#shine03,#shine04,#shine05,#shine06,#shine07,#shine08,#shine09,#shine10),particle=#magic,pps=250,initialvelocity=3:4,lifespan=25:25,localrotation=[0:(0,360,0),100:(0,540,0)],color=[0:(90,90,255),25:(18,18,185),100:(180,180,255],spread=35,azimuth=35,friction=0.5,scale=[0:5,100:0],scljitter=-0.5:0.5";
    if(!particles || particles->state || particles->frame!=41 || !particles->name || !particles->str || std::strcmp(particles->name,"partsys") || std::strcmp(particles->str,literal))return false;
    return true;
}
static void PoseMatrix(hmm_mat4& m,const char* name,const hmm_vec3& p,const hmm_vec3& r,const hmm_vec3& s)
{
 if(name[0]=='#'){BuildRetailSpeedCameraMatrix(m,p,r,s);return;}
 MtxClear(&m);MtxRotateX(&m,r.X);MtxRotateY(&m,r.Y);MtxRotateZ(&m,r.Z);MtxScale(&m,&s);MtxTranslate(&m,&p);
}
static bool CacheEmitters(State& c,int frame)
{
 auto* image=static_cast<T3DImagery*>(c.owner->GetImagery());
 for(int i=0;i<10;++i){hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(12+i,0,frame,p,r,s,true))return false;hmm_mat4 m;PoseMatrix(m,image->GetObjectName(12+i),p,r,s);auto&e=c.inputs.emitters[i];std::memcpy(e.matrix.data(),m.Elements,64);e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};e.position={p.X,p.Y,p.Z};e.scale={s.X,s.Y,s.Z};}
 c.inputs.animation_frame=frame;return true;
}
State* Spawn(const S3DPoint& source)
{
    if(!Renderer)return nullptr;
    auto c=std::make_unique<State>();c->source=source;
    auto* cls=TObjectClass::GetClass(OBJCLASS_EFFECT);const int type=cls?cls->FindObjType(0xad92bd21u):-1;
    if(type<0)return nullptr;
    SObjectDef def{};def.objclass=OBJCLASS_EFFECT;def.objtype=short(type);def.pos=source;
    c->owner.reset(cls->NewObject(&def));auto* owner=c->owner.get();
    if(!owner || owner->ObjId()!=0xad92bd21u || !dynamic_cast<TEffect*>(owner))return nullptr;
    // Controlled coordinates can lie outside loaded map sectors. Establish
    // the explicit fixture transform before creating the animator/controllers.
    SetIntPos(owner, source);owner->Face(0);owner->OnScreen();
    auto* image=dynamic_cast<T3DImagery*>(owner->GetImagery());auto* animator=dynamic_cast<T3DAnimator*>(owner->GetAnimator());
    if(!image || !animator || !ValidateNakrnoth(*image))return nullptr;
    // Original 40e8d0 renders controllers first, then texture-major objects.
    for(int slot=1;slot<=5;++slot)for(int object=1;object<63;++object){
        // Original NewObject409f61..409fda marks authored * paths hidden.
        if(image->GetObjectName(object)[0]=='*')continue;
        std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;
        if(!ExtractSubMeshTextureSlot(image,object,slot,vertices,indices))continue;
        State::Part part;part.object=object;part.slot=slot;S3DObj obj{};
        image->GetObject(object,&obj);image->GetMaterial(obj.material,&part.material);
        const uint64_t key=0x4e414b5200000000ull^(uint64_t(uint32_t(image->ImageryId()))<<16)^uint64_t(object*32+slot*4);
        for(size_t f=0;f+2<indices.size();f+=3){std::array<hmm_vec3,3> tri;for(int k=0;k<3;++k){const auto& v=vertices[indices[f+k]];tri[k]={v.pos[0],v.pos[1],v.pos[2]};}part.triangles.push_back(tri);}
        part.mesh=Renderer->RegisterMeshAsset(key,vertices.data(),int(vertices.size()),indices.data(),int(indices.size()),slot?image->GetTextureHandle(slot-1):kInvalidTexture);
        if(!part.mesh)return nullptr;
        Renderer->AddMeshAssetRef(part.mesh);c->parts.push_back(part);
    }
    image->GetObjVerts(0,c->dust);c->inputs.emitters.resize(10);c->inputs.owner_position={float(source.x),float(source.y),float(source.z)};c->inputs.ground_height=[](int,int,int){return 0;};
    if(!CacheEmitters(*c,0))return nullptr;hmm_vec3 p{},r{},scale{};if(!image->GetUninterpolatedAniKey(0,0,0,p,r,scale,true))return nullptr;
    c->inputs.has_object_zero_origin=true;c->inputs.object_zero_origin={p.X,p.Y,p.Z};authored_partsys::Definition definition;std::string diagnostic;
    if(!authored_partsys::ParseDefinition(image->GetTag(1)->str,definition,diagnostic) || !c->particles.Initialize(definition,c->inputs,0,diagnostic))return nullptr;
    log_info("[nakrnoth-original] id=ad92bd21 frames=775 objects=63 source=%d,%d,%d native=%d",source.x,source.y,source.z,int(StartupVfxNativeDomain));
    log_info("[native-domain] enabled=%d camera_z=0",int(StartupVfxNativeDomain));
    return c.release();
}
void Advance(State* c,double seconds)
{
    if(!c || !c->owner || !std::isfinite(seconds) || seconds<=0)return;
    c->accumulated+=seconds;auto* o=c->owner.get();
    while(c->accumulated+1e-10>=TTime::LegacyFrameSeconds && !(o->GetFlags()&OF_KILL)){
        c->accumulated-=TTime::LegacyFrameSeconds;o->NextFrame();
        o->Pulse();++c->ticks;if(!(o->GetFlags()&OF_KILL))c->particles.Advance(c->inputs,[](int a,int b){return random(a,b);});
    }
}
void SubmitWorld(State* c,EFxDebugMode)
{
    if(!c || !c->owner || !Renderer || (c->owner->GetFlags()&OF_KILL))return;
    if(c->rendered==TTime::FrameCount())return;c->rendered=TTime::FrameCount();
    auto* o=c->owner.get();auto* image=static_cast<T3DImagery*>(o->GetImagery());
    const auto& particles=c->particles.Particles();int draws=0;
    for(size_t slot=0;slot<particles.size();++slot){if(!particles[slot].alive)continue;const auto sample=c->particles.SampleRender(slot,[](int a,int b){return random(a,b);});if(sample.scale<=1e-5f)continue;
        hmm_mat4 m;const hmm_vec3 p={sample.position[0],sample.position[1],sample.position[2]},r={sample.rotation[0],sample.rotation[1],sample.rotation[2]},s={sample.scale,sample.scale,sample.scale};BuildRetailSpeedCameraMatrix(m,p,r,s);
        SQuadDrawItem q{};q.retail_software_projection=StartupVfxNativeDomain;q.key.texture=image->GetTextureHandle(0);q.key.blend=uint8_t(EFxBlend::AdditiveStraight);q.key.depth_mode=uint8_t(EFxDepthMode::TestNoWrite);q.light_mode=EFxLightMode::Unlit;q.retail_texture=1;q.retail_rgb565_additive=true;
        for(int i=0;i<3;++i)q.color_rgba[i]=float((int(sample.color[i])&255)>>3)/31.f;q.color_rgba[3]=sample.alpha;
        constexpr int corners[4]={0,1,3,2};for(int i=0;i<4;++i){const auto& v=c->dust[corners[i]];hmm_vec3 w;MtxTransform(&m,&v.pos,&w);q.world_pos[i][0]=w.X;q.world_pos[i][1]=w.Y;q.world_pos[i][2]=StartupVfxNativeDomain?w.Z:particles[slot].position[2]+(w.Z-sample.position[2])*WORLD3D_Z_SCALE;q.uv[i][0]=v.tu;q.uv[i][1]=v.tv;}Renderer->SubmitFxQuad(q);++draws;
    }
    CacheEmitters(*c,o->GetFrame());
    hmm_mat4 owner=o->Transform().Matrix();if(StartupVfxNativeDomain){const auto p=IntPos(o);BuildRetailSoftwareOwner(owner,p.x,p.y,p.z,uint8_t(IntFacing(o)));}
    for(const auto& part:c->parts){
        hmm_vec3 p{},r{},s{};if(!image->GetUninterpolatedAniKey(part.object,o->GetState(),o->GetFrame(),p,r,s,true) || s.X<=1e-5f || s.Y<=1e-5f || s.Z<=1e-5f)continue;
        hmm_mat4 local;PoseMatrix(local,image->GetObjectName(part.object),p,r,s);
        hmm_mat4 world;MtxMultiply(&world,&local,&owner);
        // Executed Scene mode16: two-sided, depth test, no depth write.
        SHelperMeshSubmit q{};q.mesh=part.mesh;q.additive_blend=true;q.retail_lighting=part.slot?1:4;q.retail_gold_no_depth=std::strchr(image->GetObjectName(part.object),'$')!=nullptr;
        q.retail_software_projection=StartupVfxNativeDomain;q.retail_normal_z_scale=StartupVfxNativeDomain?1.0f:1.0f/WORLD3D_Z_SCALE;
        for(int row=0;row<4;++row)for(int col=0;col<4;++col)q.world[row*4+col]=world.Elements[col][row];
        const auto& m=part.material.matdesc;std::memcpy(q.diffuse,&m.diffuse,16);std::memcpy(q.ambient,&m.ambient,16);std::memcpy(q.specular,&m.specular,16);std::memcpy(q.emissive,&m.emissive,16);q.power=m.power;q.sort_depth=world.Elements[3][2];if(StartupVfxNativeDomain && Renderer->UsesRetailSoftwareMeshLighting()){
            // Original software raster rejects edges beyond literal640/480.
            // The qualified source-domain path preserves its authored face order;
            // ordinary hardware/common draws retain the full expanding rings.
            int first=-1;for(size_t face=0;face<=part.triangles.size();++face){
                const bool allowed=face<part.triangles.size()&&RetailCometTriangleAllowed(part.triangles[face].data(),world);
                if(allowed&&first<0)first=int(face);
                if(!allowed&&first>=0){q.index_offset=first*3;q.index_count=(int(face)-first)*3;Renderer->SubmitHelperMesh(q);first=-1;++draws;}
            }
        }else{Renderer->SubmitHelperMesh(q);++draws;}
    }
    if(c->ticks%20==0 || c->ticks<4)log_info("[nakrnoth-phase] tick=%d frame=%d draws=%d seed-controlled credit=%.9g",c->ticks,o->GetFrame(),draws,c->particles.EmissionCredit());
}
bool IsAlive(const State* c)
{
    return c && c->owner && !(c->owner->GetFlags()&OF_KILL);
}
void Destroy(State* c){delete c;}
}
