#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace comet_original {
struct State;
// Shipped MeteorStorm Comet leaf, null spell and declared flat walk-height1.
State* Spawn(const S3DPoint& source);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
