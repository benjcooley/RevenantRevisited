#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace cure_retail
{
constexpr int kParticleCount = 80;
constexpr int kBallCount = 5;
using RandomRange = std::function<int(int, int)>;
struct Vec3 { float x = 0, y = 0, z = 0; };
struct Particle
{
    Vec3 pos{}, pivot{}, angvel{}, angle{};
    int state = 0;
    float size = 0, dist = 0, maindist = 0, disttheta = 0;
    int starttime = 0, whichball = 0, color = 0, linedup = 0;
};
static_assert(sizeof(Particle) == 84, "Retail Cure record size");
struct State
{
    int framenum = 0;
    std::array<Particle, kParticleCount> sm{};
    std::array<Particle, kBallCount> balls{};
    int numactiveballs = 0, ballslinedup = 0;
    float facing = 0;
    int killing = 0;
    bool alive = true;
};
void Initialize(State&, const RandomRange&);
void AdvanceOneTick(State&, const RandomRange&);
int VisibleCount(const State&);
}

// The pure state functions can be compiled independently for the extracted
// original-source oracle without bringing up the engine or graphics device.
#ifndef CURE_STATE_ONLY
#include "../effect.h"

class TCureEffect_Bespoke final : public TEffect
{
public:
    explicit TCureEffect_Bespoke(TObjectImagery* imagery);
    TCureEffect_Bespoke(SObjectDef* definition, TObjectImagery* imagery);
    ~TCureEffect_Bespoke() override;
    void Initialize(bool attach = true);
    static TCureEffect_Bespoke* SpawnForTest_BESPOKE(const S3DPoint& origin);
    void Pulse() override;
    void Advance(double seconds);
    // Helper-mesh world phase: call after BeginTilePass. Preview tick and
    // submit_world callbacks must remain separate to avoid a second tick.
    void Submit(EFxDebugMode mode = EFxDebugMode::Normal) const;
    void TickAndSubmitForTest_BESPOKE(EFxDebugMode mode = EFxDebugMode::Normal);
    bool IsAlive() const;
    int SimulationTicks() const;
    int VisibleParticles() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    void FirstSpellPulse();
    void BindFacing();
    void FinishSpell();
};
#endif
