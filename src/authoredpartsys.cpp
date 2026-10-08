#include "authoredpartsys.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace authored_partsys
{
namespace
{
// Literal floats from retail .rdata, rather than modern pi/degree constants.
constexpr float kTurn = 6.280000209808350f; // 0x5a3518
constexpr float kDegree = 0.002777777845040f; // 0x5a3514
constexpr float kTick = 0.041666667908430f; // 0x5a3564
constexpr float kQuarter = 1.570000052452087f; // 0x5a3574
int Integer(long double value) { return static_cast<int>(value); } // retail _ftol truncates
int Draw(const RandomRange& random, int a, int b)
{
    // 0x483300 returns equal endpoints without calling CRT rand().
    if (a == b) return a;
    if (a > b) std::swap(a, b);
    return random(a, b);
}
float Angle(long double degrees)
{
    return static_cast<float>(static_cast<long double>(degrees) * kTurn * kDegree);
}
Vec3 Transform(const Matrix& matrix, const Vec3& value)
{
    Vec3 result{};
    for (int i = 0; i < 3; ++i)
        result[i] = static_cast<float>(static_cast<long double>(value[0]) * matrix[i]
            + static_cast<long double>(value[1]) * matrix[4+i]
            + static_cast<long double>(value[2]) * matrix[8+i] + matrix[12+i]);
    return result;
}
Vec3 RotateX(Vec3 v, float angle)
{
    const long double c = std::cos(static_cast<long double>(angle));
    const long double s = std::sin(static_cast<long double>(angle));
    return {v[0], static_cast<float>(v[1]*c-v[2]*s), static_cast<float>(v[1]*s+v[2]*c)};
}
Vec3 RotateY(Vec3 v, float angle)
{
    const long double c = std::cos(static_cast<long double>(angle));
    const long double s = std::sin(static_cast<long double>(angle));
    return {static_cast<float>(v[0]*c+v[2]*s), v[1], static_cast<float>(-v[0]*s+v[2]*c)};
}
Vec3 RotateZ(Vec3 v, float angle)
{
    const long double c = std::cos(static_cast<long double>(angle));
    const long double s = std::sin(static_cast<long double>(angle));
    return {static_cast<float>(v[0]*c-v[1]*s), static_cast<float>(v[0]*s+v[1]*c), v[2]};
}
float Maximum(const Expression& expression, int component)
{
    if (!expression.IsCurve()) return expression.constant[component];
    float value = 0; // literal Initialize starts its key maximum at zero
    for (const auto& key : expression.keys) value = std::max(value, key.value[component]);
    return value;
}
float Initial(const Expression& expression, int component = 0)
{
    // Curves are stored in the track list; ParseItem leaves template defaults.
    return expression.constant[component];
}
void Apply(const Expression& expression, int percent, float& field)
{
    if (expression.IsCurve()) field = expression.Evaluate(percent)[0];
}
void Apply(const Expression& expression, int percent, Vec3& field)
{
    if (expression.IsCurve()) field = expression.Evaluate(percent);
}
float TowardZero(float value, float amount)
{
    if (value > 0) return std::max(0.0f, value-amount);
    if (value < 0) return std::min(0.0f, value+amount);
    return value;
}
}

bool State::Initialize(const Definition& definition, const TickInputs& inputs,
                       int quality, std::string& diagnostic)
{
    if (definition.objects.empty() || definition.particle.empty()
        || inputs.emitters.size() != definition.objects.size()) {
        diagnostic = "partsys requires resolved prototype and ordered emitter poses";
        return false;
    }
    if (definition.relvel.IsCurve() || definition.charvel.IsCurve()) {
        diagnostic = "partsys relvel/charvel curves are outside the recovered constant subset";
        return false;
    }
    if (definition.relvel.constant[0] != 0 && !inputs.has_object_zero_origin) {
        diagnostic = "partsys relvel requires the original object-zero initialization origin";
        return false;
    }
    float rate = Maximum(definition.pps, 0);
    if (quality == 2) rate *= .5f;
    else if (quality == 1) rate *= .25f;
    const long double capacity = static_cast<long double>(Maximum(definition.lifespan, 1))*rate*kTick;
    if (!std::isfinite(capacity) || capacity < 0 || capacity > 1000000) {
        diagnostic = "partsys particle capacity outside supported allocation bounds";
        return false;
    }
    definition_ = definition;
    particles_.assign(static_cast<std::size_t>(static_cast<int>(capacity)), Particle{});
    emission_credit_ = 0;
    initialized_rate_ = rate;
    next_emitter_ = 0;
    // Initialize gets object zero's origin before parsing obj; bridge supplies
    // the equivalent initial origin when relvel is used.
    previous_emitter_position_ = inputs.has_object_zero_origin
        ? inputs.object_zero_origin : inputs.emitters.front().origin;
    for (int i = 0; i < 3; ++i) previous_emitter_position_[i] += inputs.owner_position[i];
    previous_invoker_position_ = inputs.invoker_position;
    diagnostic.clear();
    return true;
}

void State::Spawn(Particle& particle, const TickInputs& inputs, const RandomRange& random)
{
    const auto& d = definition_;
    const auto& emitter = inputs.emitters[next_emitter_];
    particle = Particle{};
    const Value speed_range = d.initialvelocity.Evaluate(inputs.animation_frame);
    const float speed = static_cast<float>(Draw(random, Integer(speed_range[0]), Integer(speed_range[1])));
    // Six initial-rotation range calls precede the shape calls in retail.
    // The supported Definition subset leaves all six endpoint pairs at zero.
    for (int i = 0; i < 6; ++i) (void)Draw(random, 0, 0);
    particle.initial_global_rotation[2] = static_cast<float>(inputs.owner_face)*.00390625f;
    const float shape_a = Angle(static_cast<float>(Draw(random, 0, 360)));
    const float shape_b = Angle(static_cast<float>(Draw(random, 0, 360)));
    const int half_size = Integer(static_cast<float>(d.emittersize))/2;
    const float radius = static_cast<float>(Draw(random, -half_size, half_size));
    Vec3 offset{};
    switch (d.emittertype) {
    case 0:
        offset = {static_cast<float>(std::cos(static_cast<long double>(shape_a))*radius),
                  static_cast<float>(std::cos(static_cast<long double>(shape_b))*std::sin(static_cast<long double>(shape_a))*radius),
                  static_cast<float>(std::sin(static_cast<long double>(shape_b))*radius)};
        break;
    case 1:
        for (float& value : offset) value = static_cast<float>(Draw(random, -half_size, half_size));
        break;
    case 2:
        offset = {static_cast<float>(std::cos(static_cast<long double>(shape_a))*radius),
                  static_cast<float>(std::sin(static_cast<long double>(shape_a))*radius), 0};
        break;
    case 3:
        offset = {static_cast<float>(Draw(random,-half_size,half_size)),
                  static_cast<float>(Draw(random,-half_size,half_size)), 0};
        break;
    }
    Vec3 position = Transform(emitter.matrix, offset);
    // Pulse's type-5 track entry falls through its default arm. Initialize
    // only sets the upper endpoint to the maximum authored lifespan key.
    const Value life_range = d.lifespan.IsCurve()
        ? Value{d.lifespan.constant[0], Maximum(d.lifespan,1), 0}
        : d.lifespan.constant;
    particle.lifespan = Draw(random, Integer(life_range[0]), Integer(life_range[1]));
    const int spread = Integer(d.spread.Evaluate(inputs.animation_frame)[0]);
    const int azimuth = Integer(d.azimuth.Evaluate(inputs.animation_frame)[0]);
    const float s = Angle(static_cast<float>(Draw(random,-spread/2,spread/2)));
    const float a = Angle(static_cast<float>(Draw(random,-azimuth/2,azimuth/2)));
    const long double face = static_cast<long double>(inputs.owner_face)*kTurn*.00390625f;
    const float horizontal = static_cast<float>(face+s);
    const float vertical = static_cast<float>(face+a-kQuarter);
    const float shifted = static_cast<float>(static_cast<long double>(horizontal)+kQuarter);
    Vec3 velocity{
        static_cast<float>(std::cos(static_cast<long double>(shifted))*speed),
        static_cast<float>(std::cos(static_cast<long double>(vertical))*std::sin(static_cast<long double>(shifted))*speed),
        static_cast<float>(std::sin(static_cast<long double>(vertical))*speed)};
    velocity[2] = static_cast<float>(static_cast<long double>(velocity[2])
        + static_cast<long double>(inputs.owner_face)*.00390625f);
    velocity = Transform(emitter.matrix, velocity);
    float charvel = d.charvel.constant[0];
    const float relvel = d.relvel.constant[0];
    if (charvel == -1000) charvel = relvel;
    for (int i = 0; i < 3; ++i)
        velocity[i] = static_cast<float>(static_cast<long double>(velocity[i])-emitter.position[i]
            +(static_cast<long double>(emitter.position[i])-previous_emitter_position_[i])*relvel);
    if (inputs.has_invoker)
        for (int i = 0; i < 2; ++i)
            velocity[i] = static_cast<float>(static_cast<long double>(velocity[i])
                +(static_cast<long double>(inputs.invoker_position[i])-previous_invoker_position_[i])*charvel);
    position = RotateZ(position, static_cast<float>(face));
    for (int i = 0; i < 3; ++i) position[i] += inputs.owner_position[i];
    // Retail always samples optional scale variation here; its supported
    // default endpoints are zero, so no CRT random value is consumed.
    particle.scale_delta = static_cast<float>(Draw(random,0,0))*.001f;
    if (emitter.scale[0] == 0 || emitter.scale[1] == 0 || emitter.scale[2] == 0) return;
    particle.position = position;
    particle.velocity = velocity;
    particle.local_rotation = d.localrotation.constant;
    particle.random_local_rotation = d.rlocalrotation.constant;
    particle.global_rotation = d.globalrotation.constant;
    particle.random_global_rotation = d.rglobalrotation.constant;
    particle.color = d.color.constant;
    particle.alpha = Initial(d.alpha);
    particle.scale = Initial(d.scale);
    particle.friction = Initial(d.friction);
    particle.gravity = Initial(d.gravity);
    particle.bounce_min = Initial(d.bounce);
    particle.bounce_max = Initial(d.bounce,1);
    particle.alive = true;
}

void State::Update(Particle& particle, const TickInputs& inputs, const RandomRange& random)
{
    const auto& d = definition_;
    const int percent = particle.age*100/std::max(1,particle.lifespan);
    Apply(d.localrotation,percent,particle.local_rotation);
    Apply(d.rlocalrotation,percent,particle.random_local_rotation);
    Apply(d.globalrotation,percent,particle.global_rotation);
    Apply(d.rglobalrotation,percent,particle.random_global_rotation);
    Apply(d.color,percent,particle.color);
    Apply(d.alpha,percent,particle.alpha);
    Apply(d.scale,percent,particle.scale);
    Apply(d.gravity,percent,particle.gravity);
    Apply(d.friction,percent,particle.friction);
    if (d.bounce.IsCurve()) {
        const auto bounce = d.bounce.Evaluate(percent);
        particle.bounce_min=bounce[0]; particle.bounce_max=bounce[1];
    }
    particle.velocity[2] -= particle.gravity;
    // Zero-width random alpha, scale and XYZ positional jitters are calls in
    // retail but do not consume rand(); unsupported fields remain absent.
    (void)Draw(random,0,0);
    particle.scale = static_cast<float>(static_cast<long double>(particle.scale)+particle.scale_delta);
    (void)Draw(random,0,0);
    for(int i=0;i<3;++i) (void)Draw(random,0,0);
    Vec3 rotation{};
    for(int i=0;i<3;++i) rotation[i] = static_cast<float>(
        (static_cast<long double>(Draw(random,Integer(particle.global_rotation[i]),
         Integer(static_cast<long double>(particle.global_rotation[i])+particle.random_global_rotation[i])))*kTurn
         + particle.initial_global_rotation[i])*kDegree);
    rotation[2] = static_cast<float>(static_cast<long double>(rotation[2])
        + static_cast<long double>(inputs.owner_face)*.00390625f);
    particle.velocity = RotateX(particle.velocity,rotation[0]);
    particle.velocity = RotateY(particle.velocity,rotation[1]);
    particle.velocity = RotateZ(particle.velocity,rotation[2]);
    for (float& value : particle.velocity) value = TowardZero(value,particle.friction);
    if (inputs.ground_height && inputs.ground_height(Integer(particle.position[0]),
        Integer(particle.position[1]),Integer(particle.position[2])) > particle.position[2]) {
        const int bounce = Draw(random,Integer(particle.bounce_min),Integer(particle.bounce_max));
        particle.velocity[2] = static_cast<float>(static_cast<long double>(bounce)
            * .009999999776482582f * particle.velocity[2]);
    }
    for (int i=0;i<3;++i) particle.position[i] += particle.velocity[i];
    if (++particle.age >= particle.lifespan) particle.alive=false;
}

void State::Advance(const TickInputs& inputs, const RandomRange& random)
{
    if (inputs.emitters.size()!=definition_.objects.size() || inputs.emitters.empty()) return;
    const float rate=definition_.pps.IsCurve()
        ? definition_.pps.Evaluate(inputs.animation_frame)[0] : initialized_rate_;
    emission_credit_=static_cast<float>(static_cast<long double>(rate)*kTick+emission_credit_);
    for (auto& particle : particles_) {
        if(particle.alive) Update(particle,inputs,random);
        if(!particle.alive && emission_credit_>=1) {
            Spawn(particle,inputs,random);
            next_emitter_=(next_emitter_+1)%inputs.emitters.size();
            emission_credit_-=1;
        }
    }
    previous_emitter_position_=inputs.emitters[next_emitter_].position;
    previous_invoker_position_=inputs.invoker_position;
}

RenderSample State::SampleRender(std::size_t slot, const RandomRange& random) const
{
    const auto& particle=particles_.at(slot);
    RenderSample sample;
    sample.position=particle.position;
    // Retail .rdata 5a351c..28: z/(1.46-z/30000)*1.038.
    sample.position[2]=static_cast<float>(static_cast<long double>(sample.position[2])
        /(static_cast<long double>(1.460000038146973f)
           -static_cast<long double>(sample.position[2])*.0033333334140479565f*.009999999776482582f)
        *1.037999987602234f);
    for(int i=0;i<3;++i) sample.rotation[i]=Angle(
        static_cast<long double>(Draw(random,Integer(particle.local_rotation[i]),
        Integer(static_cast<long double>(particle.local_rotation[i])+particle.random_local_rotation[i])))
        +particle.initial_local_rotation[i]);
    for(int i=0;i<3;++i) sample.color[i]=static_cast<float>(Integer(particle.color[i]));
    sample.alpha=static_cast<float>(Integer(static_cast<long double>(particle.alpha)*255.0f))/255.0f;
    sample.scale=particle.scale;
    sample.blendmode=definition_.blendmode;
    return sample;
}
}
