#include "game/effects/legacy/prelitrgbcontrol.h"
#include "engine/render/renderer.h"
#include <memory>
#include <vector>
namespace prelit_rgb_control {
struct Draw {MeshHandle mesh;float farthest;bool prelit;};
struct State {std::vector<Draw> draws;~State(){if(Renderer)for(auto d:draws)Renderer->ReleaseMeshAssetRef(d.mesh);}};
State* Spawn(){if(!Renderer)return nullptr;auto s=std::make_unique<State>();constexpr uint16_t indices[]={0,1,2,0,2,3};
 auto add=[&](float cx,float cy,float z,int r,int g,int b,bool prelit,int alpha=31){SMeshVertex verts[4]={};constexpr int xs[]={-24,24,24,-24},ys[]={-24,-24,24,24};float farthest=-1e30f;for(int i=0;i<4;++i){const float diff=(cx+xs[i]-320)/1.0101525783538818f,sum=(cy+ys[i]-240+1.237179160118103f*z)/.5050762891769409f;auto&v=verts[i];v.pos[0]=(sum+diff)*.5f;v.pos[1]=(sum-diff)*.5f;v.pos[2]=z;v.normal[0]=float(r);v.normal[1]=float(g);v.normal[2]=float(b);v.uv[0]=float(alpha);const float d=-.8748177289962769f*sum-.7142857909202576f*z;if(d>farthest)farthest=d;}
 const auto m=Renderer->RegisterMeshAsset(0x5052454c49540000ull+s->draws.size(),verts,4,indices,6,Renderer->WhiteTextureHandle());if(!m)return;Renderer->AddMeshAssetRef(m);s->draws.push_back({m,farthest,prelit});};
 for(int row=0;row<2;++row){constexpr int colors[][3]={{8,2,3},{31,31,31},{0,31,0},{31,0,0}};for(int c=0;c<4;++c)add(140+120*c,90+90*row,0,colors[c][0],colors[c][1],colors[c][2],row==0);}
 constexpr int alphas[]={0,7,15,30};for(int c=0;c<4;++c)add(140+120*c,270,0,8,2,3,true,alphas[c]);
 // Positive Z is nearer at fixed projected position; equal depth overwrites.
 add(180,390,20,31,0,0,true);add(180,390,0,0,31,0,true);
 add(320,390,0,31,0,0,true);add(320,390,20,0,31,0,true);
 add(460,390,0,31,0,0,true);add(460,390,0,0,31,0,true);
 return s.release();}
void SubmitWorld(State*s){if(!s||!Renderer)return;for(auto d:s->draws){SHelperMeshSubmit q={};q.mesh=d.mesh;q.world[0]=q.world[5]=q.world[10]=q.world[15]=1;q.retail_lighting=4;q.retail_prelit_rgba5=d.prelit;q.retail_source_alpha=true;q.retail_software_projection=true;q.retail_flat_depth=true;q.retail_flat_depth_offset=d.farthest;Renderer->SubmitHelperMesh(q);}}
void Destroy(State*s){delete s;}
}
