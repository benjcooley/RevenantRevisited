#pragma once
#include "game/effects/speedauthoredmatrix.h"
// All three visible prototypes retain their literal '#' camera transform.
// Unlike JhagaAttack's '#$head', this asset's '#head' retains depth testing.
inline void BuildRetailHeadFireBallMatrix(hmm_mat4& matrix,int object,
    const hmm_vec3& position,const hmm_vec3& rotation,const hmm_vec3& scale)
{
    if(object==0 || object==3 || object==4){BuildRetailSpeedCameraMatrix(matrix,position,rotation,scale);return;}
    MtxClear(&matrix);MtxRotateX(&matrix,rotation.X);MtxRotateY(&matrix,rotation.Y);MtxRotateZ(&matrix,rotation.Z);
    MtxScale(&matrix,&scale);MtxTranslate(&matrix,&position);
}
