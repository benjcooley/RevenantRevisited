#pragma once
#include "engine/core/math3d.h"
namespace retail_teleporter {
// Original null-spell owner4e6a80: duration50, actual kill when prior life>=50.
// This is the standalone portal branch; spell destination/actor actions differ.
struct State {
 int ticks=0,life=0,phase=1,iterations=0;
 float rotation=0;
 bool alive=true;
 void Pulse(){
  if(!alive)return;
  ++ticks;rotation+=.20000000298023224f;if(iterations<24)++iterations;
  if(phase==1){phase=2;life=0;}else if(phase==2&&life==25)phase=4;
  if(life>=50)alive=false;
  ++life;
 }
 template<class Submit>void Render(Submit&&submit)const{
  if(!alive)return;
  const int t=ticks%50;const int folded=t<25?t:50-t;
  const double ratio=double(folded)*double(.03333333507180214f);
  const float base_z=float(10.0-ratio*5.0),base_xy=float(ratio*2.5);
  for(int i=0;i<5;++i){
   const float z=float(double(base_z)-double(i));if(z<=.01)continue;
   const float a=float(double(rotation)+double(i)*.5);
   const float xy=float(double(base_xy)+double(i)*.5);if(xy<=1e-5f)continue;
   hmm_mat4 m;MtxClear(&m);MtxRotateZ(&m,a);const hmm_vec3 scale={xy,xy,z};MtxScale(&m,&scale);submit(i,m);
  }
 }
};
}
