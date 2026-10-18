#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace headfireball_original {
struct State;
// Actual RunFireHeadN single-child caller: facing0 and declared targetY=sourceY-120.
State* Spawn(const S3DPoint& source);
void Advance(State*, double seconds);
bool IsAlive(const State*);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
