#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace fgeyser_original {
struct State;
// Shipped fgeyser leaf, null spell and declared flat walk-height1.
State* Spawn(const S3DPoint& source);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
