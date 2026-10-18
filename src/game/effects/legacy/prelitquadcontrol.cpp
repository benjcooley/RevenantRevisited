#include "game/effects/legacy/prelitquadcontrol.h"
#include "engine/render/renderer.h"
#include <vector>
namespace prelit_quad_control {
struct State {std::vector<SQuadDrawItem> triangles;};
State* Spawn(){if(!Renderer)return nullptr;auto* s=new State;
 auto add=[&](int x,int y,const float rgb[3][3],const float alpha[3],const float depth[3],bool policy=true){SQuadDrawItem q{};q.corner_count=3;q.per_corner_color=true;q.key.texture=Renderer->WhiteTextureHandle();q.key.blend=uint8_t(EFxBlend::Alpha);q.key.depth_mode=uint8_t(EFxDepthMode::TestWrite);q.retail_software_projection=true;q.retail_prelit_rgba5=policy;constexpr int dx[]={0,96,0},dy[]={0,0,96};for(int i=0;i<3;++i){
 // Literal fractional projected coordinates test original truncation; no fit.
 double screenx=x+dx[i]+.25-320,screeny=y+dy[i]+.25-240;
 double z=(-(double(depth[i])-2750)-(.8748177289962769/.5050762891769409)*screeny)/(.7142857909202576+.8748177289962769*1.237179160118103/.5050762891769409);
 double sum=(screeny+1.237179160118103*z)/.5050762891769409,diff=screenx/1.0101525783538818;
 q.world_pos[i][0]=float((sum+diff)*.5);q.world_pos[i][1]=float((sum-diff)*.5);q.world_pos[i][2]=float(z);
 for(int j=0;j<3;++j)q.corner_color_rgba[i][j]=policy?rgb[i][j]:rgb[i][j]*(j==1?2.f/63.f:1.f/31.f);q.corner_color_rgba[i][3]=policy?alpha[i]:(alpha[i]+1.f)/32.f;
 }s->triangles.push_back(q);};
 const float gray[3][3]={{22,22,22},{22,22,22},{22,22,22}},gradient[3][3]={{0,22,31},{31,0,0},{0,22,31}},red[3][3]={{31,0,0},{31,0,0},{31,0,0}},green[3][3]={{0,31,0},{0,31,0},{0,31,0}};
 const float variable[]={0,31,0},low[]={3,3,3},opaque[]={31,31,31},flat[]={2750,2750,2750};
 add(40,40,gray,variable,flat);add(220,40,gradient,low,flat);add(400,40,gradient,variable,flat);
 const float background[]={2770,2770,2770},behind[]={2700.25f,2800.75f,2700.5f},near[]={2700.25f,2769.75f,2700.5f},fractional[]={2769.1f,2769.8f,2769.5f};
 add(40,200,red,opaque,background);add(40,200,green,opaque,behind);add(220,200,red,opaque,background);add(220,200,green,opaque,near);add(400,200,red,opaque,background);add(400,200,green,opaque,fractional);
 add(40,350,gray,low,flat);add(220,350,gray,low,flat,false);return s;
}
void SubmitWorld(State* s){if(s&&Renderer)for(const auto& q:s->triangles)Renderer->SubmitFxQuad(q);}
void Destroy(State* s){delete s;}
}
