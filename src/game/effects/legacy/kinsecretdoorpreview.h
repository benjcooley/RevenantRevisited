#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace kinsecretdoor_still_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*,double seconds);
void SubmitWorld(State*,EFxDebugMode);
void Destroy(State*);
}
