#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace jhagaattack_original {
struct State;
// Literal null-spell owner: timed120-unit negativeY flight from this source.
State* Spawn(const S3DPoint& source);
void Advance(State*, double seconds);
bool IsAlive(const State*);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
