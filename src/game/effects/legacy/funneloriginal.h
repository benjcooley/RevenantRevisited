#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace funnel_original {
struct State;
// Declared spell owner; target marks zero-height ground. Damage/audio stay external.
State* Spawn(const S3DPoint& source, const S3DPoint& target);
void Advance(State*, double seconds);
bool IsAlive(const State*);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
