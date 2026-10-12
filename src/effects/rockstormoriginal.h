#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace rockstorm_original {
struct State;
// Explicit level-one visual caller: stationary target at origin, radius24.
State* Spawn(const S3DPoint& origin);
void Advance(State*,double seconds);
void SubmitWorld(State*,EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
