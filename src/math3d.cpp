// *************************************************************************
// *                         Revenant Revisited                            *
// *                math3d.cpp - 4x4 matrix and 3D vector helpers          *
// *************************************************************************
//
// Ported from the retired D3DMATRIX/D3DVECTOR helpers in attic/src/d3dmath.*.
// Matrix memory layout is preserved so downstream consumers (renderer,
// animation data, etc.) see the same bytes they did under the original
// DirectX 5 code path.

#include "math3d.h"

#include <cmath>
#include <cstring>

namespace
{
    constexpr hmm_mat4 kIdentity = {{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
}

hmm_mat4* MtxClear(hmm_mat4* lpMat)
{
    *lpMat = kIdentity;
    return lpMat;
}

hmm_mat4* MtxMultiply(hmm_mat4* lpDst, const hmm_mat4* lpSrc1, const hmm_mat4* lpSrc2)
{
    hmm_mat4 d;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            float s = 0.0f;
            for (int i = 0; i < 4; ++i)
                s += lpSrc1->Elements[r][i] * lpSrc2->Elements[i][c];
            d.Elements[r][c] = s;
        }
    }
    *lpDst = d;
    return lpDst;
}

hmm_mat4* MtxTranslate(hmm_mat4* lpDst, const hmm_vec3* lpV)
{
    hmm_mat4 m = kIdentity;
    m.Elements[3][0] = lpV->X;
    m.Elements[3][1] = lpV->Y;
    m.Elements[3][2] = lpV->Z;
    return MtxMultiply(lpDst, lpDst, &m);
}

hmm_mat4* MtxMove(hmm_mat4* lpDst, const hmm_vec3* lpV)
{
    hmm_mat4 m = kIdentity;
    m.Elements[3][0] = -lpV->X;
    m.Elements[3][1] = -lpV->Y;
    m.Elements[3][2] = -lpV->Z;
    return MtxMultiply(lpDst, lpDst, &m);
}

hmm_mat4* MtxScale(hmm_mat4* lpDst, const hmm_vec3* lpV)
{
    hmm_mat4 m = kIdentity;
    m.Elements[0][0] = lpV->X;
    m.Elements[1][1] = lpV->Y;
    m.Elements[2][2] = lpV->Z;
    return MtxMultiply(lpDst, lpDst, &m);
}

hmm_mat4* MtxRotateX(hmm_mat4* lpDst, float a)
{
    const float c = std::cos(a);
    const float s = std::sin(a);
    hmm_mat4 m = kIdentity;
    m.Elements[1][1] = c;
    m.Elements[1][2] = s;
    m.Elements[2][1] = -s;
    m.Elements[2][2] = c;
    return MtxMultiply(lpDst, lpDst, &m);
}

hmm_mat4* MtxRotateY(hmm_mat4* lpDst, float a)
{
    const float c = std::cos(a);
    const float s = std::sin(a);
    hmm_mat4 m = kIdentity;
    m.Elements[0][0] = c;
    m.Elements[0][2] = -s;
    m.Elements[2][0] = s;
    m.Elements[2][2] = c;
    return MtxMultiply(lpDst, lpDst, &m);
}

hmm_mat4* MtxRotateZ(hmm_mat4* lpDst, float a)
{
    const float c = std::cos(a);
    const float s = std::sin(a);
    hmm_mat4 m = kIdentity;
    m.Elements[0][0] = c;
    m.Elements[0][1] = s;
    m.Elements[1][0] = -s;
    m.Elements[1][1] = c;
    return MtxMultiply(lpDst, lpDst, &m);
}

void MtxTransform(const hmm_mat4* lpm, const hmm_vec3* lpV, hmm_vec3* lpD)
{
    const float x = lpV->X;
    const float y = lpV->Y;
    const float z = lpV->Z;
    lpD->X = lpm->Elements[0][0] * x + lpm->Elements[1][0] * y + lpm->Elements[2][0] * z + lpm->Elements[3][0];
    lpD->Y = lpm->Elements[0][1] * x + lpm->Elements[1][1] * y + lpm->Elements[2][1] * z + lpm->Elements[3][1];
    lpD->Z = lpm->Elements[0][2] * x + lpm->Elements[1][2] * y + lpm->Elements[2][2] * z + lpm->Elements[3][2];
}

hmm_mat4* MtxInvert(hmm_mat4* d, const hmm_mat4* a)
{
    // Matches the retired helper: transposes the 3x3 rotation and passes the
    // rest through (only valid for pure rotation matrices).
    d->Elements[0][0] = a->Elements[0][0];
    d->Elements[0][1] = a->Elements[1][0];
    d->Elements[0][2] = a->Elements[2][0];
    d->Elements[0][3] = a->Elements[0][3];

    d->Elements[1][0] = a->Elements[0][1];
    d->Elements[1][1] = a->Elements[1][1];
    d->Elements[1][2] = a->Elements[2][1];
    d->Elements[1][3] = a->Elements[1][3];

    d->Elements[2][0] = a->Elements[0][2];
    d->Elements[2][1] = a->Elements[1][2];
    d->Elements[2][2] = a->Elements[2][2];
    d->Elements[2][3] = a->Elements[2][3];

    d->Elements[3][0] = a->Elements[0][3];
    d->Elements[3][1] = a->Elements[1][3];
    d->Elements[3][2] = a->Elements[2][3];
    d->Elements[3][3] = a->Elements[3][3];
    return d;
}

hmm_mat4* MtxSetRotation(hmm_mat4* lpM, const hmm_vec3* lpD, const hmm_vec3* lpU)
{
    hmm_vec3 d = *lpD;
    VecNormalize(&d);

    hmm_vec3 u = *lpU;
    const float t = u.X * d.X + u.Y * d.Y + u.Z * d.Z;
    u.X -= d.X * t;
    u.Y -= d.Y * t;
    u.Z -= d.Z * t;
    VecNormalize(&u);

    hmm_vec3 r;
    VecCross(&r, &u, &d);

    lpM->Elements[0][0] = r.X;
    lpM->Elements[0][1] = r.Y;
    lpM->Elements[0][2] = r.Z;
    lpM->Elements[1][0] = u.X;
    lpM->Elements[1][1] = u.Y;
    lpM->Elements[1][2] = u.Z;
    lpM->Elements[2][0] = d.X;
    lpM->Elements[2][1] = d.Y;
    lpM->Elements[2][2] = d.Z;
    return lpM;
}

hmm_vec3* VecNormalize(hmm_vec3* v)
{
    const float x = v->X;
    const float y = v->Y;
    const float z = v->Z;
    if (x == 0.0f && y == 0.0f && z == 0.0f)
        return v;
    const float inv = 1.0f / std::sqrt(x * x + y * y + z * z);
    v->X = x * inv;
    v->Y = y * inv;
    v->Z = z * inv;
    return v;
}

hmm_vec3* VecCross(hmm_vec3* lpd, const hmm_vec3* lpa, const hmm_vec3* lpb)
{
    lpd->X = lpa->Y * lpb->Z - lpa->Z * lpb->Y;
    lpd->Y = lpa->Z * lpb->X - lpa->X * lpb->Z;
    lpd->Z = lpa->X * lpb->Y - lpa->Y * lpb->X;
    return lpd;
}

void Spline(hmm_vec3* p, float t,
            const hmm_vec3* p1, const hmm_vec3* p2,
            const hmm_vec3* p3, const hmm_vec3* p4)
{
    const float t2 = t * t;
    const float t3 = t2 * t;

    const float m1 = 0.5f * (-t3 + 2.0f * t2 - t);
    const float m2 = 0.5f * (3.0f * t3 - 5.0f * t2 + 2.0f);
    const float m3 = 0.5f * (-3.0f * t3 + 4.0f * t2 + t);
    const float m4 = 0.5f * (t3 - t2);

    p->X = p1->X * m1 + p2->X * m2 + p3->X * m3 + p4->X * m4;
    p->Y = p1->Y * m1 + p2->Y * m2 + p3->Y * m3 + p4->Y * m4;
    p->Z = p1->Z * m1 + p2->Z * m2 + p3->Z * m3 + p4->Z * m4;
}
