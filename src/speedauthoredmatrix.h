#pragma once
#include "goldauthoredmatrix.h"

// Exact Speed '#speedflare' emitter/base branch of retail CalcObjectMatrix.
// The source '#' path rebuilds SRT before its fixed camera orientation,
// preserving the authored translation. Nonuniform scale makes order matter.
inline void BuildRetailSpeedCameraMatrix(hmm_mat4& matrix, const hmm_vec3& position,
                                        const hmm_vec3& rotation, const hmm_vec3& scale)
{
    MtxClear(&matrix);
    MtxScale(&matrix, &scale);
    MtxRotateX(&matrix, rotation.X);
    MtxRotateY(&matrix, rotation.Y);
    MtxRotateZ(&matrix, rotation.Z);
    ApplyRetailGoldCameraOrientation(matrix);
    MtxTranslate(&matrix, &position);
}
