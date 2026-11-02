#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace cataclysm_original {
struct State;
State* Spawn(const S3DPoint& source,bool yellow_variant=false);
void Advance(State*,double seconds);
void SubmitWorld(State*,EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
