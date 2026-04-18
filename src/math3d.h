// *************************************************************************
// *                         Revenant Revisited                            *
// *                math3d.h - 4x4 matrix and 3D vector helpers            *
// *************************************************************************
//
// Replaces the retired D3DMATRIX / D3DVECTOR helpers (attic/src/d3dmath.*).
// Semantics are preserved so the byte layout of produced matrices is
// identical to the original game: Translate/Scale/Rotate* right-multiply a
// local transform into dst (dst = dst * T), and MtxTransform applies the
// matrix as a row-vector transform (result = v * m).

#pragma once

#include <HandmadeMath.h>

// Sets lpMat to the identity matrix.
hmm_mat4* MtxClear(hmm_mat4* lpMat);

// lpDst = lpSrc1 * lpSrc2. lpDst may alias lpSrc1 or lpSrc2.
hmm_mat4* MtxMultiply(hmm_mat4* lpDst, const hmm_mat4* lpSrc1, const hmm_mat4* lpSrc2);

// Right-multiplies lpDst by a translation/scale/rotation. lpDst = lpDst * T.
hmm_mat4* MtxTranslate(hmm_mat4* lpDst, const hmm_vec3* lpV);
hmm_mat4* MtxMove(hmm_mat4* lpDst, const hmm_vec3* lpV);       // translate by -lpV
hmm_mat4* MtxScale(hmm_mat4* lpDst, const hmm_vec3* lpV);
hmm_mat4* MtxRotateX(hmm_mat4* lpDst, float a);
hmm_mat4* MtxRotateY(hmm_mat4* lpDst, float a);
hmm_mat4* MtxRotateZ(hmm_mat4* lpDst, float a);

// Transforms a point: lpD = lpV * lpm (row-vector convention).
void MtxTransform(const hmm_mat4* lpm, const hmm_vec3* lpV, hmm_vec3* lpD);

// Transpose-as-inverse for pure rotations (matches retired D3DMATRIXInvert).
hmm_mat4* MtxInvert(hmm_mat4* d, const hmm_mat4* a);

// Orients the rotation part of lpM so lpD is the new z-axis, lpU the new y-axis.
hmm_mat4* MtxSetRotation(hmm_mat4* lpM, const hmm_vec3* lpD, const hmm_vec3* lpU);

// Vector helpers.
hmm_vec3* VecNormalize(hmm_vec3* v);
hmm_vec3* VecCross(hmm_vec3* lpd, const hmm_vec3* lpa, const hmm_vec3* lpb);

// Catmull-Rom spline: point along the curve between p2 and p3 at parameter t in [0,1].
void Spline(hmm_vec3* p, float t,
            const hmm_vec3* p1, const hmm_vec3* p2,
            const hmm_vec3* p3, const hmm_vec3* p4);
