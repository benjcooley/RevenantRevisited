#pragma once
#include "speedauthoredmatrix.h"
// All four original ceyes objects carry the camera-facing '#' flag.
inline void BuildRetailCEyesMatrix(hmm_mat4& matrix,int,
    const hmm_vec3& position,const hmm_vec3& rotation,const hmm_vec3& scale)
{
    BuildRetailSpeedCameraMatrix(matrix,position,rotation,scale);
}
