#pragma once
struct S3DPoint;
namespace manadrain_preview {
struct State;
// Explicit genuine Acolytes at3185/3281,3185,16; actual Mana Drain variant.
// Display relocation is separate from actor positions and is not an endpoint API.
State* SpawnTargetFixture(const S3DPoint&);
State* SpawnConfiguredTarget(const S3DPoint&);
// Shipped Essence Drain spell selects ManaDrain and its110..115 variant.
// This named recipe is separate from the static Essencedrain/Worm prototype.
State* SpawnEssenceDrainTargetFixture(const S3DPoint&);
State* SpawnEssenceDrainConfiguredTarget(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
