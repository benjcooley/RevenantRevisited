#!/usr/bin/env python3
"""Compile the actual static factory/destructor and exercise repeated ownership."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / 'src/effect.cpp').read_text()
start = source.index('TAuthoredStaticMeshEffect::~TAuthoredStaticMeshEffect()')
end = source.index('void TAuthoredStaticMeshEffect::TickAndSubmitForTest_BESPOKE', start)
actual = source[start:end]
prefix = r'''
#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <cstring>
#include <strings.h>
using MeshHandle=uint32_t; using TTextureHandle=uint32_t;
constexpr uint32_t kInvalidTexture=0;
#define stricmp strcasecmp
struct S3DPoint {int x,y,z;};
struct SMeshVertex {float data[8];};
struct S3DTex {struct {int pixelFormat=1;} desc;int numframes=0;uint32_t* framehtexs=nullptr;};
struct Image {
 uint64_t id=123; int selected=0; uint32_t frames[2][3]={{100,101,102},{110,111,0}};
 uint64_t AssetId(){return id;} int NumObjects(){return 2;} int NumTextures(){return 2;}
 int NumTags(){return 0;} struct Tag {const char* name;}; Tag* GetTag(int){return nullptr;}
 bool HasScrollTex(){return false;} bool IsHidden(int,int){return false;}
 uint32_t GetTextureHandle(int slot){return frames[slot][selected];}
 int GetTextureFrame(int){return selected;}
 void GetTexture(int slot,S3DTex* out){out->numframes=slot?2:3;out->framehtexs=frames[slot];}
} image;
struct SLoadedImagery {void* base;Image* img3d;};
SLoadedImagery SparkleLoadImagery(const char*,const char*){return {nullptr,&image};}
const char* kGlobeImageryPath="test";
void log_error(const char*,...){} void log_info(const char*,...){}
int RetailMeshTextureLighting(int){return 1;}
bool ExtractSubMeshTextureSlot(Image* im,int object,int slot,std::vector<SMeshVertex>& vertices,std::vector<uint16_t>& indices){
 vertices.resize(4);vertices[0].data[0]=float(im->id);vertices[1].data[0]=float(object);vertices[2].data[0]=float(slot);
 indices={0,1,2,0,2,3};return true;
}
void BuildStaticObjectMatrix(Image*,int,int,int,float*){}
struct RendererType {
 struct Entry {std::vector<SMeshVertex> vertices;std::vector<uint16_t> indices;uint32_t texture,refs=0;};
 std::vector<Entry> entries;std::map<uint64_t,uint32_t> keys;int fail_after=-1;
 uint32_t WhiteTextureHandle(){return 1;}
 uint32_t RegisterMeshAsset(uint64_t key,const SMeshVertex* v,int nv,const uint16_t* i,int ni,uint32_t texture){
  if(fail_after==0)return 0;if(fail_after>0)--fail_after;
  if(auto it=keys.find(key);it!=keys.end()){
   const auto& e=entries[it->second-1];assert(e.texture==texture&&e.vertices.size()==size_t(nv)&&e.indices.size()==size_t(ni));
   assert(!std::memcmp(e.vertices.data(),v,nv*sizeof(*v))&&!std::memcmp(e.indices.data(),i,ni*sizeof(*i)));return it->second;
  }
  assert(key);entries.push_back({{v,v+nv},{i,i+ni},texture,0});return keys[key]=uint32_t(entries.size());
 }
 void AddMeshAssetRef(uint32_t h){++entries.at(h-1).refs;}
 void ReleaseMeshAssetRef(uint32_t h){auto& e=entries.at(h-1);assert(e.refs);--e.refs;}
 void EmptyRefs(){for(auto&e:entries)assert(!e.refs);}
} renderer; auto* Renderer=&renderer;
struct {int n=0;int MakeIndex(){return ++n;}} MapPane;
struct TAuthoredStaticMeshEffect {
 struct SStaticPart {uint32_t mesh=0;int object_index=0,retail_lighting=0;std::vector<uint32_t> frame_meshes;float local_matrix[16]{};};
 std::vector<SStaticPart> parts_;bool animate_textures_=false;Image* scroll_imagery_=nullptr;
 explicit TAuthoredStaticMeshEffect(void*){} ~TAuthoredStaticMeshEffect();
 void ForcePos(const S3DPoint&){} void SetMapIndex(int){}
 static TAuthoredStaticMeshEffect* SpawnForTest_BESPOKE(const S3DPoint&,const char*,bool,bool);
};
'''
main = r'''
int main(){
 for(int n=0;n<100;++n){
  auto* a=TAuthoredStaticMeshEffect::SpawnForTest_BESPOKE({n,0,0},"test",false,false);
  auto* b=TAuthoredStaticMeshEffect::SpawnForTest_BESPOKE({0,n,0},"test",true,false);
  assert(a&&b);assert(renderer.entries.size()==12);delete a;delete b;renderer.EmptyRefs();
 }
 image.selected=1;
 auto* selected=TAuthoredStaticMeshEffect::SpawnForTest_BESPOKE({0,0,0},"test",false,false);
 assert(selected&&renderer.entries.size()==12);delete selected;renderer.EmptyRefs();image.selected=0;
 image.id=124;
 auto* other=TAuthoredStaticMeshEffect::SpawnForTest_BESPOKE({0,0,0},"other",true,false);
 assert(other&&renderer.entries.size()==24);delete other;renderer.EmptyRefs();
 image.id=125;renderer.fail_after=3;
 auto* failed=TAuthoredStaticMeshEffect::SpawnForTest_BESPOKE({0,0,0},"failure",true,false);
 assert(!failed);renderer.EmptyRefs();
}
'''
with tempfile.TemporaryDirectory() as temp:
    cpp = Path(temp) / 'test.cpp'; exe = Path(temp) / 'test'
    cpp.write_text(prefix + actual + main)
    subprocess.run(['c++', '-std=c++17', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print('PASS actual factory:100 repeated simultaneous static/animated owners,12 stable immutable meshes, distinct assets, exact buffers/textures, balanced refs and partial-failure cleanup')
