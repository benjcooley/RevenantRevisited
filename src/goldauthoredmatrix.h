#pragma once
#include "math3d.h"

// Literal retail CalcObjectMatrix 40a71f..40a852, software '#' object branch.
// Keep the original authored mesh; these are exporter camera-orientation
// matrices, not a generated billboard or an empirical view/size adjustment.
inline void ApplyRetailGoldCameraOrientation(hmm_mat4& matrix)
{
    hmm_mat4 fixed;
    MtxClear(&fixed);
    fixed.Elements[0][0] = fixed.Elements[2][2] = 0.8191519975662231f; // 3f51b3f2
    fixed.Elements[0][2] = -0.5735759735107422f; // bf12d5e0
    fixed.Elements[2][0] = 0.5735759735107422f; // 3f12d5e0
    MtxMultiply(&matrix, &matrix, &fixed);
    MtxClear(&fixed);
    fixed.Elements[0][0] = fixed.Elements[1][1] = -0.7071059942245483f; // bf3504e6
    fixed.Elements[0][1] = 0.7071059942245483f;
    fixed.Elements[1][0] = -0.7071059942245483f;
    MtxMultiply(&matrix, &matrix, &fixed);
}
