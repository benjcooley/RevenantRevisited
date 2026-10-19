#pragma once
#include "game/effects/arrowcore.h"
#include "engine/core/math3d.h"
namespace arrow_core {
inline void Matrix(hmm_mat4& m,const Draw& d){MtxClear(&m);hmm_vec3 scale={d.scale,d.scale,d.scale};MtxScale(&m,&scale);MtxRotateY(&m,64.f);MtxRotateX(&m,-64.f);hmm_vec3 p={d.x,d.y,d.z};MtxTranslate(&m,&p);}
}
