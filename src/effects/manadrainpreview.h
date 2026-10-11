#pragma once
struct S3DPoint;
namespace manadrain_preview {
struct State;
// Explicit genuine Acolytes at3185/3281,3185,16; actual Mana Drain variant.
// Display relocation is separate from actor positions and is not an endpoint API.
State* SpawnTargetFixture(const S3DPoint&);
State* SpawnConfiguredTarget(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
