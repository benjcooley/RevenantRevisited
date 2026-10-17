"""Compile actual water format admission and default/native quad submission."""
import subprocess,tempfile,unittest
from pathlib import Path
from test_vfxreviewspawn import function
ROOT=Path(__file__).resolve().parents[2]
PRELUDE=r'''
#include <cassert>
#include <vector>
#include "particlefx.h"
#include "math3d.h"
#include "retailsoftwaretransform.h"
#undef min
#undef max
struct Format{unsigned dwRGBBitCount=16,dwRBitMask=0xf00,dwGBitMask=0xf0,dwBBitMask=0xf,dwRGBAlphaBitMask=0xf000;};
struct S3DTex{struct{Format pixelFormat;}desc;TTextureHandle htexture=1;};
struct T3DImagery{S3DTex tex;int NumObjects(){return 1;}int NumObjVerts(int){return 4;}int NumTextures(){return 1;}
 void GetObjVerts(int,S3DVertex*v){for(int i=0;i<4;++i){v[i]={};v[i].pos=hmm_vec3{float(i&1),float(i>>1),0};}}
 void GetTexture(int,S3DTex*out){*out=tex;}};
struct SMeshVertex{};
bool ExtractSubMeshTextureSlot(T3DImagery*,int,int,std::vector<SMeshVertex>&,std::vector<uint16_t>&i){i={0,1,3,3,2,0};return true;}
enum class EFxDebugMode{Normal,FullTexture};enum class EFxBlend{Alpha};enum class EFxDepthMode{TestNoWrite};enum class EFxLightMode{Unlit};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;
 bool retail_software_projection=false,retail_argb4444=false;EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&q){draws.push_back(q);}} renderer;TRenderer*Renderer=&renderer;
struct World{hmm_mat4 matrix;World(){MtxClear(&matrix);}const hmm_mat4&Matrix()const{return matrix;}};
struct TObjectInstance{World world;S3DPoint pos{};int GetFace(){return 0;}const S3DPoint&Pos(){return pos;}const World&Transform(){return world;}};
'''
MAIN=r'''
int main(){
 T3DImagery image;std::vector<S3DVertex>vertices;TTextureHandle texture=0;bool admitted=false;
 assert(LoadWaterQuad(&image,0,vertices,texture,&admitted)&&admitted);
 Format original=image.tex.desc.pixelFormat;
 for(int channel=0;channel<5;++channel){
  image.tex.desc.pixelFormat=original;auto&f=image.tex.desc.pixelFormat;
  unsigned*fields[]={&f.dwRGBBitCount,&f.dwRBitMask,&f.dwGBitMask,&f.dwBBitMask,&f.dwRGBAlphaBitMask};*fields[channel]^=1;
  admitted=true;assert(LoadWaterQuad(&image,0,vertices,texture,&admitted)&&!admitted);
 }
 image.tex.desc.pixelFormat={16,0x7c00,0x3e0,0x1f,0x8000};
 admitted=true;assert(LoadWaterQuad(&image,0,vertices,texture,&admitted)&&!admitted);
 TObjectInstance owner;const hmm_vec3 zero{},one{1,1,1};
 SubmitWaterQuad(owner,vertices,texture,zero,one,EFxDebugMode::Normal);
 auto normal=renderer.draws.back();assert(!normal.retail_software_projection&&!normal.retail_argb4444);
 SubmitWaterQuad(owner,vertices,texture,zero,one,EFxDebugMode::Normal,-1,false,true);
 auto strict_common=renderer.draws.back();assert(!strict_common.retail_software_projection&&!strict_common.retail_argb4444);
 for(int i=0;i<4;++i)for(int j=0;j<3;++j)assert(normal.world_pos[i][j]==strict_common.world_pos[i][j]);
 SubmitWaterQuad(owner,vertices,texture,zero,one,EFxDebugMode::Normal,-1,true,true);
 assert(renderer.draws.back().retail_software_projection&&renderer.draws.back().retail_argb4444);
 SubmitWaterQuad(owner,vertices,texture,zero,one,EFxDebugMode::Normal,-1,true,false);
 assert(renderer.draws.back().retail_software_projection&&!renderer.draws.back().retail_argb4444);
}
'''
class WaterQuadPolicy(unittest.TestCase):
 def test_actual_format_and_default_opt_in(self):
  source=(ROOT/'src/effect.cpp').read_text()
  body=function(source,'bool LoadWaterQuad(')+'\n'+function(source,'void SubmitWaterQuad(')
  with tempfile.TemporaryDirectory()as temp:
   p=Path(temp);(p/'test.cpp').write_text(PRELUDE+body+MAIN)
   command=['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(p/'test.cpp'),str(ROOT/'src/math3d.cpp'),'-o',str(p/'test')]
   r=subprocess.run(command,capture_output=True,text=True);self.assertEqual(r.returncode,0,r.stdout+r.stderr)
   r=subprocess.run([str(p/'test')],capture_output=True,text=True);self.assertEqual(r.returncode,0,r.stdout+r.stderr)
if __name__=='__main__':unittest.main()
