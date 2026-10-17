#pragma once
#include "speedauthoredmatrix.h"

// Original default CalcObjectMatrix: ordinary R*S*T, '#' rebuilds S*R then
// camera orientation. The ordinary MPAppear objects retain their authored transforms.
inline void BuildRetailMPAppearMatrix(hmm_mat4& matrix,int object,
    const hmm_vec3& position,const hmm_vec3& rotation,const hmm_vec3& scale)
{
    if(object==1){BuildRetailSpeedCameraMatrix(matrix,position,rotation,scale);return;}
    MtxClear(&matrix);
    MtxRotateX(&matrix,rotation.X);MtxRotateY(&matrix,rotation.Y);MtxRotateZ(&matrix,rotation.Z);
    MtxScale(&matrix,&scale);MtxTranslate(&matrix,&position);
}
