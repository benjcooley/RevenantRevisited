#pragma once
#include "game/effects/manadraincore.h"
#include "engine/core/math3d.h"
namespace manadrain_core {
// Original4fc440 matrices. Absolute positions already use integer native FIX_Z.
// The emitter's forty-unit offset is present in the executable, not the old
// disabled source snapshot's fifteen-unit placement.
template<class Emit>void ForEachDraw(const State& s,int facing,float glow_scale,Emit emit){
 if(!s.alive)return;
 auto sprite=[](float scale,Point p){hmm_mat4 m;MtxClear(&m);hmm_vec3 v={scale,scale,scale};MtxScale(&m,&v);MtxRotateZ(&m,.7421079277992249f);MtxRotateY(&m,1.1131618022918701f);v={float(p.x),float(p.y),float(p.z)};MtxTranslate(&m,&v);return m;};
 if(s.stage>=1)for(int i=0;i<15;++i)emit(2,sprite(s.stream_scale[i],s.position[i]),false);
 const float angle=float(double(facing+127)*double(.024736929684877396f));
 const double offset_angle=double(angle)+double(1.5707900524139404f);
 const hmm_vec3 p={float(double(s.caster.x)+std::cos(offset_angle)*40.0),float(double(s.caster.y)+std::sin(offset_angle)*40.0),float(s.caster.z+30)};
 hmm_mat4 cone;MtxClear(&cone);hmm_vec3 scale={s.scale*2.f,s.scale*2.f,s.scale*2.f};MtxScale(&cone,&scale);MtxRotateZ(&cone,s.rotation);MtxRotateX(&cone,1.5831634998321533f);MtxRotateZ(&cone,angle);MtxTranslate(&cone,&p);emit(3,cone,true);
 hmm_mat4 glow;MtxClear(&glow);scale={glow_scale,glow_scale,glow_scale};MtxScale(&glow,&scale);MtxRotateZ(&glow,.7421079277992249f);MtxRotateY(&glow,1.1131618022918701f);MtxTranslate(&glow,&p);emit(2,glow,false);
}
}
