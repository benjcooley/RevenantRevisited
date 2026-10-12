#pragma once
#include "../effect.h"
#include "sandswirlcontroller.h"
#include "seekingparticles.h"
#include <memory>

namespace sandswirl_runtime {
// The normal builder requires the real `sand` dependency. A caller must
// explicitly opt a single instance into the licensed Sparks diagnostic.
class Effect final : public TEffect {
public:
    Effect(TObjectImagery*);
    Effect(SObjectDef*,TObjectImagery*);
    ~Effect() override;
    void Pulse() override;
    bool UseQualifiedSparks(int32_t initialized_seekz);
    bool UseRestoredLegacySand(int32_t initialized_seekz);
    bool InitializeRuntime();
    bool Ready() const;
    int ParentFrame() const;
    TObjectInstance* ParticleChild() const;
    const retail_seeking_particles::State* ParticleState() const;
    const std::vector<int32_t>& SpawnedQuicksand() const;
    const std::vector<int32_t>& QueriedObjects() const;
private:
    struct Runtime;
    std::unique_ptr<Runtime> runtime;
};
// Explicitly named test fixture uses real source map allocation and actors.
struct MapFixture;
MapFixture* SpawnMapFixture(const S3DPoint&);
MapFixture* SpawnRestoredSandMapFixture(const S3DPoint&);
void AdvanceMapFixture(MapFixture*,double);
void SubmitMapFixture(MapFixture*);
void DestroyMapFixture(MapFixture*);
}
