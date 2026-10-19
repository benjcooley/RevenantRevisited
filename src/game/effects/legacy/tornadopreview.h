#pragma once
struct S3DPoint;
namespace tornado_preview {
struct State;
// Explicit exact Tornado owner plus genuine actors/loaded Tornado variant.
// The named spell naturally selects Funnel; this is not a natural cast claim.
State* SpawnTargetFixture(const S3DPoint&);
State* SpawnConfiguredTarget(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
