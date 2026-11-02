#pragma once
#include "game/effects/speedauthoredmatrix.h"
inline void BuildRetailFunnelMatrix(hmm_mat4& matrix,int object,const hmm_vec3& p,const hmm_vec3& r,const hmm_vec3& s)
{
    if(object==4 || object==6){BuildRetailSpeedCameraMatrix(matrix,p,r,s);return;}
    MtxClear(&matrix);MtxRotateX(&matrix,r.X);MtxRotateY(&matrix,r.Y);MtxRotateZ(&matrix,r.Z);MtxScale(&matrix,&s);MtxTranslate(&matrix,&p);
}
