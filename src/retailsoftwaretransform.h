#pragma once
#include "math3d.h"
#include <cstdint>

// Pinned retail 40e4d6..40e4f6. Keep extended intermediate precision until the
// animator's float store; this is owner translation, not a model vertex scale.
inline float RetailSoftwareOwnerZ(int32_t z)
{
    const double raw = z;
    return float(raw / (double(1.4600000381469727f) -
        raw * double(0.0033333334140479565f) * double(0.009999999776482582f)) *
        double(1.0379999876022339f));
}

// Actual 40e470 owner pose followed by 40e740 matrix order Rz * Rx * Ry * T.
// Ordinary native owners do not carry the port's common-world 1.5 Z stretch.
inline void BuildRetailSoftwareOwner(hmm_mat4& matrix, int32_t x, int32_t y,
        int32_t z, uint8_t facing, float rotation_x=0, float rotation_y=0)
{
    MtxClear(&matrix);
    MtxRotateZ(&matrix, float(double(facing) * double(0.02454369328916073f)));
    MtxRotateX(&matrix, rotation_x);
    MtxRotateY(&matrix, rotation_y);
    const hmm_vec3 position={float(x),float(y),RetailSoftwareOwnerZ(z)};
    MtxTranslate(&matrix,&position);
}

// Actual 56cdc0: inverse integer camera translation, Rz(pi/4), Rx(-2pi/3),
// then +zdist. 411640 and software SetTransform56d5f0 make all three projection
// diagonal terms 0x3fb6db6e. These constants are opcode inputs, not fitted slopes.
inline void BuildRetailSoftwareViewProjection(hmm_mat4& matrix,
        int32_t camera_x, int32_t camera_y, int32_t camera_z, int32_t zdist=1925)
{
    MtxClear(&matrix);
    const hmm_vec3 camera={float(camera_x),float(camera_y),float(camera_z)};
    MtxMove(&matrix,&camera);
    MtxRotateZ(&matrix,0.7853981852531433f); // 0x3f490fdb
    MtxRotateX(&matrix,-2.094395160675049f); // 0xc0060a92
    const hmm_vec3 forward={0,0,float(zdist)};
    MtxTranslate(&matrix,&forward);
    const hmm_vec3 projection={1.4285714626312256f,1.4285714626312256f,1.4285714626312256f};
    MtxScale(&matrix,&projection);
}
