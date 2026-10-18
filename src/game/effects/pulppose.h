#pragma once
#include "game/effects/pulpcore.h"
#include "engine/core/math3d.h"
namespace pulp_core {
// Original OBJ3D_ROT1|SCL2|POS3|ABSPOS: raw absolute float position, no owner FIX_Z.
inline hmm_mat4 Matrix(Vec position,Vec rotation,Vec scale){
 hmm_mat4 m;MtxClear(&m);MtxRotateX(&m,rotation.x);MtxRotateY(&m,rotation.y);MtxRotateZ(&m,rotation.z);
 hmm_vec3 s={scale.x,scale.y,scale.z},p={position.x,position.y,position.z};MtxScale(&m,&s);MtxTranslate(&m,&p);return m;
}
template<class Emit>void ForEachDraw(const State& s,Emit emit){
 if(!s.valid)return;
 for(const auto& p:s.bodies)emit(p.object,Matrix(p.position,p.rotation,p.scale));
 for(const auto& p:s.blood)if(p.used && p.scale.x!=0 && p.scale.y!=0 && p.scale.z!=0)
  emit(1,Matrix(p.position,{-.5235987901687622f,0,-.7853981852531433f},p.scale));
}
}
