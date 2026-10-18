#include "game/effects/legacy/argbsourcecontrol.h"
#include "engine/render/renderer.h"
#include "engine/core/logging.h"
#include <memory>
namespace argb_source_control {
// Diagnostic-only literal swatches. Never mapped to a retail effect identity.
struct State {TTextureHandle textures[7]{};MeshHandle meshes[14]{};~State(){if(Renderer){for(auto m:meshes)if(m)Renderer->ReleaseMeshAssetRef(m);for(auto t:textures)if(t)Renderer->ReleaseTextureAssetRef(t);}}};
State* Spawn(bool energy){
 if(!Renderer)return nullptr;auto s=std::make_unique<State>();constexpr uint16_t shriek_words[]={0x0204,0x0305,0x0205,0x0304,0x0306,0x0fff,0xffff},energy_words[]={0x0323,0x0322,0x1433,0x0755,0x0856,0x0fff,0xffff};const auto*words=energy?energy_words:shriek_words;constexpr uint16_t indices[]={0,1,2,0,2,3};
 for(int col=0;col<7;++col){const auto word=words[col];uint8_t rgba[16];for(int i=0;i<4;++i){rgba[4*i]=((word>>8)&15)*17;rgba[4*i+1]=((word>>4)&15)*17;rgba[4*i+2]=(word&15)*17;rgba[4*i+3]=(word>>12)*17;}
  s->textures[col]=Renderer->RegisterTextureAsset(0x4152474243540000ull+(energy?0x100:0)+col,rgba,sizeof rgba,2,2,ERendererTextureFormat::RGBA8,sizeof rgba,ERendererTextureFilter::Nearest,true);if(s->textures[col]==kInvalidTexture)return nullptr;Renderer->AddTextureAssetRef(s->textures[col]);
  for(int row=0;row<2;++row){const float cx=92+72*col-320,cy=180+120*row-240;SMeshVertex vertices[4]={};constexpr int x[]={-24,24,24,-24},y[]={-24,-24,24,24};for(int i=0;i<4;++i){const float sx=cx+x[i],sy=cy+y[i];vertices[i].pos[0]=sx*.5f+sy;vertices[i].pos[1]=sy-sx*.5f;vertices[i].normal[2]=1;vertices[i].uv[0]=.5f;vertices[i].uv[1]=.5f;}
   auto& m=s->meshes[row*7+col];m=Renderer->RegisterMeshAsset(0x41524742434d0000ull+(energy?0x100:0)+row*7+col,vertices,4,indices,6,s->textures[col]);if(!m)return nullptr;Renderer->AddMeshAssetRef(m);
  }
 }
 log_info("[argb-source-control] top=default bottom=source-quantized energy=%d centers=(92+72*i,180/300) black-background",int(energy));return s.release();
}
void SubmitWorld(State* s){if(!s || !Renderer)return;for(int row=0;row<2;++row)for(int col=0;col<7;++col){SHelperMeshSubmit draw={};draw.mesh=s->meshes[row*7+col];draw.world[0]=draw.world[5]=draw.world[10]=draw.world[15]=1;draw.retail_lighting=3;draw.retail_source_alpha=true;draw.retail_alpha_source_quantize=row!=0;Renderer->SubmitHelperMesh(draw);}}
void Destroy(State* s){delete s;}
}
