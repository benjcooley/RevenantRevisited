#pragma once
#include "math3d.h"
namespace retail_quicksand {
// Original animator4edb40 increments once via owner4ed3d0; owner kills when>55.
struct State {int ticks=0,frame=0;bool alive=true;void Pulse(){if(!alive)return;frame=(frame+1)%3;++ticks;if(ticks>55)alive=false;}};
inline void Matrix(hmm_mat4& m,const hmm_vec3& p,const hmm_vec3& r,const hmm_vec3& s){
 MtxClear(&m);MtxRotateX(&m,r.X);MtxRotateY(&m,r.Y);MtxRotateZ(&m,r.Z);MtxScale(&m,&s);MtxTranslate(&m,&p);
}
}
