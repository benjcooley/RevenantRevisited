#pragma once

#include "partsysdefinition.h"
#include <array>
#include <cstddef>
#include <functional>
#include <vector>

// Retail partsys controller (vtable 0x5a3544). Simulation and the original
// render-time random rotation sampling deliberately have separate entry points.
namespace authored_partsys
{
using Vec3 = std::array<float, 3>;
using Matrix = std::array<float, 16>; // original D3D row-vector layout
using RandomRange = std::function<int(int, int)>;
using GroundHeight = std::function<int(int, int, int)>;

struct EmitterPose
{
    Vec3 origin{};      // GetObjectPos/GetObjectOrigin, without owner translation
    Vec3 position{};    // S3DAnimObj.pos, used by relative velocity inheritance
    Vec3 scale{{1, 1, 1}};
    Matrix matrix{{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}};
};

struct TickInputs
{
    Vec3 owner_position{};
    unsigned char owner_face = 0;
    int animation_frame = 0;
    std::vector<EmitterPose> emitters; // same order as Definition.objects
    bool has_object_zero_origin = false;
    Vec3 object_zero_origin{}; // Initialize obtains object 0 before parsing obj
    // Retail optionally inherits its spell invoker's horizontal movement.
    bool has_invoker = false;
    Vec3 invoker_position{};
    GroundHeight ground_height;
};

struct Particle
{
    bool alive = false;
    int age = 0;
    int lifespan = 0;
    Vec3 position{}, velocity{};
    Vec3 initial_local_rotation{}, initial_global_rotation{}; // degrees
    Vec3 local_rotation{}, random_local_rotation{}; // degrees / range widths
    Vec3 global_rotation{}, random_global_rotation{};
    Vec3 color{}; // original 0..255 lit vertex channels
    float alpha = 1, scale = 1;
    float friction = 0, gravity = 0;
    float bounce_min = 100, bounce_max = 100;
    float scale_delta = 0;
};

struct RenderSample
{
    Vec3 position{}; // retail z perspective correction already applied
    Vec3 rotation{}; // radians, sampled local rotation + initial local rotation
    Vec3 color{};
    float alpha = 1, scale = 1;
    int blendmode = 16;
};

class State
{
public:
    bool Initialize(const Definition& definition, const TickInputs& inputs,
                    int quality, std::string& diagnostic);
    // Exactly one call per original 24 Hz Pulse; never call from Submit.
    void Advance(const TickInputs& inputs, const RandomRange& random);
    // Preserve three retail helper evaluations per rendered particle. Equal
    // endpoints return a constant without consuming the supplied RNG stream.
    RenderSample SampleRender(std::size_t slot, const RandomRange& random) const;
    const std::vector<Particle>& Particles() const { return particles_; }
    std::size_t Capacity() const { return particles_.size(); }
    float EmissionCredit() const { return emission_credit_; }
    std::size_t NextEmitter() const { return next_emitter_; }
private:
    Definition definition_{};
    std::vector<Particle> particles_;
    float emission_credit_ = 0;
    float initialized_rate_ = 0;
    std::size_t next_emitter_ = 0;
    Vec3 previous_emitter_position_{}, previous_invoker_position_{};
    void Update(Particle&, const TickInputs&, const RandomRange&);
    void Spawn(Particle&, const TickInputs&, const RandomRange&);
};
}
