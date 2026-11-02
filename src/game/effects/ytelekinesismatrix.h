#pragma once
#include "game/effects/ytelekinesiscore.h"
#include "engine/core/math3d.h"
namespace ytelekinesis_core {
inline void Matrix(hmm_mat4& m,const Rock& r){
 const hmm_vec3 scale={r.scale.x,r.scale.y,r.scale.z},position={r.position.x,r.position.y,r.position.z};
 MtxClear(&m);MtxScale(&m,&scale);MtxRotateX(&m,r.rotation.x);MtxRotateY(&m,r.rotation.y);MtxRotateZ(&m,r.rotation.z);MtxTranslate(&m,&position);
}
}
