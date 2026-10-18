#pragma once
#include "game/effects/rockstormcore.h"
#include "engine/core/math3d.h"
#include <algorithm>
#include <cmath>
namespace rockstorm_core {
struct Draw {int object=0;hmm_vec3 scale={1,1,1},rotation={},position={};};
inline float Height(float local,int target_z){const double z=double((std::max)(local,-10.f))+target_z;return float(z/(double(1.4600000381469727f)-z*double(.0033333334140479565f)*double(.009999999776482582f))*double(1.0379999876022339f));}
template<class Submit>void ForEachDraw(const State& state,Submit submit){
 if(!state.alive)return;const auto& p=state.pool;
 for(int i=0;i<p.count;++i){const float s=p.scale[i],angle=p.angle[i];
  const hmm_vec3 position={float(double(p.radius[i])*std::cos(double(angle))+p.target_position[0]),float(double(p.radius[i])*std::sin(double(angle))+p.target_position[1]),Height(p.height[i],p.target_position[2])};
  // The custom renderer changes X/Z rotation only; the shipped constant Y key survives.
  if(p.stage<=1){const float g=s*1.7f;const float y=p.stage==0?float(double(s)*double(1.7f)/(double(s)*double(s))):g;submit(Draw{3,{g,y,g},{-30,-3.13671875f,15},{position.X,position.Y,(std::max)(-1.f,position.Z)}});}
  submit(Draw{0,{s,s,s},{angle,angle,angle},position});
 }
}
inline void Matrix(hmm_mat4& m,const Draw& d){MtxClear(&m);MtxScale(&m,&d.scale);MtxRotateX(&m,d.rotation.X);MtxRotateY(&m,d.rotation.Y);MtxRotateZ(&m,d.rotation.Z);MtxTranslate(&m,&d.position);}
}
