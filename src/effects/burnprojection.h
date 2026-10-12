#pragma once
#include "../math3d.h"
#include "../revdefs.h"

namespace burn_projection {
// Burn's ABSPOS Render matrix contains the original nonlinear MODELZ center.
// The common renderer instead consumes raw map translation and stretched
// mesh-local Z, like T3DAnimator::SubmitPartSys. Bridge once at submission;
// retain the literal original matrix for native software comparisons.
inline hmm_mat4 World(const hmm_mat4& native, const hmm_vec3& map_center,
                     bool native_domain)
{
    hmm_mat4 world = native;
    if (!native_domain) {
        for (int column = 0; column < 3; ++column)
            world.Elements[column][2] *= WORLD3D_Z_SCALE;
        world.Elements[3][2] = map_center.Z;
    }
    return world;
}
}
