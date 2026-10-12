#pragma once
#include "dragonfirecore.h"
#include "math3d.h"
namespace retail_dragonfire {
inline hmm_mat4 LocalMatrix(const Particle& p,float facing=0){
 hmm_mat4 m;MtxClear(&m);MtxRotateX(&m,float(double(p.rotation.x)*0.017453292519943295));MtxRotateY(&m,float(double(p.rotation.y)*0.017453292519943295));MtxRotateZ(&m,float(double(p.rotation.z)*0.017453292519943295));
 MtxRotateX(&m,-1.5707963705062866f);MtxRotateZ(&m,-0.7853981852531433f);MtxRotateZ(&m,facing);
 const hmm_vec3 scale={p.scale.x,p.scale.y,p.scale.z},position={p.pos.x,p.pos.y,p.pos.z};MtxScale(&m,&scale);MtxTranslate(&m,&position);return m;
}
}
