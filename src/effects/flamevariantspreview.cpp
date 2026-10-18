#include "flamevariantspreview.h"
#include "../effect.h"
#include "../testconfig.h"
#include "../logging.h"
#include <cctype>
#include <cstring>
#include <memory>
#include <string>
namespace flame_variants_preview {
struct State {TObjectPtr<TObjectInstance> owner;};
static uint64_t Hash(const void* p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const unsigned char*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
bool Matches(const TObjectInstance* owner,T3DImagery* img)
{
 if(!owner || !img || owner->ObjClass()!=OBJCLASS_EFFECT || owner->GetState()!=0)return false;
 const uint32_t id=owner->ObjId();if(id!=0x50ba373cu && id!=0x50ba373du)return false;
 if(img->NumObjects()!=1 || img->NumStates()!=1 || img->GetAniLength(0)!=1 || img->GetAniFlags(0)!=0x2001 || img->NumMaterials()!=1 || img->NumTextures()!=1 || img->NumTags()!=0)return false;
 if(!img->GetResFilename())return false;std::string path(img->GetResFilename());for(char&c:path)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));const char*suffix=id==0x50ba373cu?"magic/flameb.i3d":"magic/flameg.i3d";const size_t n=std::strlen(suffix);if(path.size()<n || path.compare(path.size()-n,n,suffix) || (path.size()!=n && path[path.size()-n-1]!='/'))return false;
 S3DObj o={};img->GetObject(0,&o);if(std::strcmp(o.name,id==0x50ba373cu?"box02":"box03") || o.material || o.parent[0]!=-1 || o.numanikeys[0]!=9 || !o.anikeys[0] || Hash(o.anikeys[0],36)!=(id==0x50ba373cu?0xedd9e4bc8d72b2e7ull:0xa159b3a8406b5f3cull) || img->NumObjVerts(0)!=4 || img->NumObjFaces(0)!=2)return false;
 S3DVertex v[4];S3DFace faces[2];int starts[MAXTEXTURES+1]={},bins[MAXTEXTURES+1]={};img->GetObjVerts(0,v);img->GetObjFaces(0,faces,starts,bins);if(Hash(v,sizeof(v))!=0x72d33224e307174full || Hash(faces,sizeof(faces))!=0xf44208a16a617ed8ull || bins[0] || bins[1]!=2)return false;
 S3DMat m={};img->GetMaterial(0,&m);if(m.texture || Hash(&m.matdesc.diffuse,68)!=0x2d6e7c4f0d54f390ull)return false;
 S3DTex t={};img->GetTexture(0,&t);const auto&f=t.desc.pixelFormat;return t.numframes==1 && t.desc.width==128 && t.desc.height==128 && f.dwRGBBitCount==16 && f.dwRBitMask==0xf00 && f.dwGBitMask==0xf0 && f.dwBBitMask==0xf && f.dwRGBAlphaBitMask==0xf000 && img->GetTextureHandle(0)!=kInvalidTexture;
}
State* Spawn(uint32_t id,const S3DPoint& origin)
{
 if(!Renderer || (id!=0x50ba373cu && id!=0x50ba373du))return nullptr;
 auto s=std::make_unique<State>();auto*cl=TObjectClass::GetClass(OBJCLASS_EFFECT);int type=cl?cl->FindObjType(id):-1;if(type<0)return nullptr;SObjectDef d={};d.objclass=OBJCLASS_EFFECT;d.objtype=short(type);d.pos=origin;s->owner.reset(cl->NewObject(&d));if(!s->owner)return nullptr;s->owner->OnScreen();auto*img=dynamic_cast<T3DImagery*>(s->owner->GetImagery());if(!Matches(s->owner.get(),img))return nullptr;
 TFlameEffect::AttachVisualComponent(s->owner.get(),img);s->owner->ActivateComponents();if(!s->owner->GetComponent<TFlipbookBillboardComponent>())return nullptr;
 log_info("[native-domain] enabled=%d camera_z=0 owner=RzRxRyT Flame variant typed=%08x",int(StartupVfxNativeDomain),id);log_info("[flame-variant] typed_id=%08x authored18tick8cell atlas",id);return s.release();
}
void Submit(State* s,EFxDebugMode mode){if(!s || !Renderer)return;if(auto*c=s->owner->GetComponent<TFlipbookBillboardComponent>()){c->SetDebugMode(mode);c->Submit(*Renderer,*s->owner);}}
void Destroy(State* s){delete s;}
}
