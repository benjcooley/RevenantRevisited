#pragma once
#include "game/effects/speedauthoredmatrix.h"

// Original default CalcObjectMatrix: ordinary R*S*T, '#' rebuilds S*R then
// camera orientation. The ordinary YAttack objects retain their authored transforms.
inline void BuildRetailYAttackMatrix(hmm_mat4& matrix,int object,
    const hmm_vec3& position,const hmm_vec3& rotation,const hmm_vec3& scale)
{
    if(object>=0 && object<=6){BuildRetailSpeedCameraMatrix(matrix,position,rotation,scale);return;}
    MtxClear(&matrix);
    MtxRotateX(&matrix,rotation.X);MtxRotateY(&matrix,rotation.Y);MtxRotateZ(&matrix,rotation.Z);
    MtxScale(&matrix,&scale);MtxTranslate(&matrix,&position);
}
