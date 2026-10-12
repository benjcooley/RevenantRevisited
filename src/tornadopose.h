#pragma once
#include "tornadocore.h"
#include "retailsoftwaretransform.h"
namespace tornado_core {
// Original full4ef3c0 draw order. Object0 RGB565 additive flames, then
// source-alpha object2 swirls and object3/1 ice; native owner supplied explicitly.
template<class Emit>void ForEachDraw(const State& s,const hmm_mat4& owner,uint8_t facing,Emit emit){
 if(!s.alive)return;
 auto pose=[&](Vec scale,Vec angle,Vec pos){hmm_mat4 m;MtxClear(&m);hmm_vec3 v={scale.x,scale.y,scale.z};MtxScale(&m,&v);MtxRotateX(&m,angle.x);MtxRotateY(&m,angle.y);MtxRotateZ(&m,angle.z);v={pos.x,pos.y,pos.z};MtxTranslate(&m,&v);MtxMultiply(&m,&m,&owner);return m;};
 auto sprite=[&](float scale,Vec pos){hmm_mat4 m;MtxClear(&m);hmm_vec3 v={scale,scale,scale};MtxScale(&m,&v);MtxRotateX(&m,-1.5707963705062866f);MtxRotateX(&m,-.5235987901687622f);MtxRotateZ(&m,-.7853981852531433f);MtxRotateZ(&m,float((-int(facing)*360/256)*State::radians));v={pos.x,pos.y,pos.z};MtxTranslate(&m,&v);MtxMultiply(&m,&m,&owner);return m;};
 for(const auto& p:s.flames)if(p.scale>0)emit(0,sprite(p.scale,p.pos));
 for(const auto& p:s.swirls)emit(2,sprite(p.scale,p.pos));
 for(int i=0;i<30;++i){const auto& p=s.ice[i];if(p.time || p.stage)continue;emit(i%6==0?3:1,pose(p.scale,p.angle,p.pos));}
}
}
