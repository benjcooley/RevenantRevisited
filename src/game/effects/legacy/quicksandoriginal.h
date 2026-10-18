#pragma once
struct S3DPoint;
namespace quicksand_original {
struct State;
// Declared stationary target offset(96,0,0), matching genuine Acolyte caller.
// Actor damage/reaction and arbitrary spell variants are separate integrations.
State* Spawn(const S3DPoint& source);
void Advance(State*,double);
void SubmitWorld(State*);
bool IsAlive(const State*);
void Destroy(State*);
}
