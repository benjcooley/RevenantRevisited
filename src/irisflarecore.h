#pragma once
#include "math3d.h"
#include <array>
namespace irisflare_core {
struct State {
 float rotation=0;std::array<hmm_vec3,5> positions{};int iterations=0,ticks=0;float cylinder[2]={1,1};float growth[2]={.3f,.1f};float flare_angle=0;
 State(){Reset();}
 void Reset(){iterations=ticks=0;for(auto&p:positions){p.Y=20;p.Z=45;}positions[1].X=-5;positions[2].X=5;positions[3].Z+=20;positions[4].Z-=20;cylinder[0]=cylinder[1]=1;growth[0]=.3f;growth[1]=.1f;}
 void Advance(){rotation+=.1f;if(iterations<50){if(iterations>=25){positions[1].X+=.5f;positions[2].X-=.5f;}positions[3].Z-=1;positions[4].Z+=1;++iterations;}if(++ticks>200)Reset();}
 template<class Submit>void Render(Submit&&submit){
  // Original first three draws use explicit matrices. The two horizontal
  // flare draws instead invoke CalcObjectMatrix with SCL1|ROT2 and the
  // angle retained from the preceding Render, with no position override.
  for(int i=0;i<3;++i){const float xy=growth[i?1:0]*float(iterations);const hmm_vec3 scale={xy,xy,i?8+growth[1]*float(iterations):15};hmm_mat4 m;MtxClear(&m);MtxScale(&m,&scale);MtxRotateX(&m,-1.5707963705062866f);MtxTranslate(&m,&positions[i]);submit(0,0x100u,m);}
  const float xy=growth[1]*float(iterations);if(xy>1e-5f){const hmm_vec3 scale={xy,xy,8+xy};for(int i=0;i<2;++i){hmm_mat4 m;MtxClear(&m);MtxScale(&m,&scale);MtxRotateZ(&m,flare_angle);submit(0,0x60u,m);}}
  flare_angle=float(double(ticks)*7.2*0.017453292522222223);
  if(iterations<50)cylinder[0]=cylinder[1]=float(double(iterations)*.1);else if(iterations>150){cylinder[0]-=.1f;cylinder[1]-=.1f;}
  for(int i=0;i<2;++i)if(cylinder[i]>1e-5f){hmm_mat4 m;MtxClear(&m);MtxRotateZ(&m,i?-rotation:rotation);const hmm_vec3 scale={cylinder[i],cylinder[i],cylinder[i]};MtxScale(&m,&scale);submit(1,0x90u,m);}
  if(ticks<20&&ticks>0){hmm_mat4 m;MtxClear(&m);MtxRotateZ(&m,-rotation);const hmm_vec3 scale={float(ticks)*3,float(ticks)*3,float((20-ticks)>10?10:20-ticks)};MtxScale(&m,&scale);submit(1,0x90u,m);}
 }
};
}
