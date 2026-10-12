#pragma once
struct S3DPoint;
namespace pulp_preview {
struct State;
// Explicit genuine Acolyte public-Set fixture, velocity(0,0,8), counts5/5/5.
// Empty terrain/decal rejection and legacy map-ID clock0 are checked scope.
State* SpawnActorFixture(const S3DPoint&);
State* SpawnConfiguredActor(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
