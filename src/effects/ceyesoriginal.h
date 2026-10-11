#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace ceyes_original {
struct State;
// Shipped ceyes leaf, null spell and declared flat walk-height1.
State* Spawn(const S3DPoint& source,int initial_state=0);
bool SetState(State*,int state);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
