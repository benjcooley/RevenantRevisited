#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace dragonattack_original {
struct State;
// Declared genuine Fighting caller: face96 muzzle(14,14), targetX=sourceX+280, ground0.
State* Spawn(const S3DPoint& source);
void Advance(State*, double seconds);
bool IsAlive(const State*);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
