#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace ytelekinesis_original {
struct State;
State* Spawn(const S3DPoint& player);
void Advance(State*,double seconds);
void SubmitWorld(State*,EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
