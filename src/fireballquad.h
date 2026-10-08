#pragma once
#include "math3d.h"
#include <array>

// Bounded retail head/glow geometry, not a second rendering backend.
namespace fireball_quad
{
struct Quad
{
    std::array<hmm_vec3,4> position{};
    std::array<std::array<float,2>,4> uv{};
};
Quad Build(const std::array<hmm_vec3,4>& authored,int frame,int spin_degrees,
           int owner_face,float scale,const hmm_vec3& local_position,const hmm_mat4& owner_world);
Quad BuildSpark(const std::array<hmm_vec3,4>& authored,
                const std::array<std::array<float,2>,4>& uv,float scale,
                const hmm_vec3& absolute_position);
Quad BuildSparkNativeD3D(const std::array<hmm_vec3,4>& authored,
                        const std::array<std::array<float,2>,4>& uv,float scale,
                        const hmm_vec3& absolute_position);
}
