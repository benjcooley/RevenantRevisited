#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace lightstrip_original {
struct State;
// Literal null-spell owner; target is the boundary of an explicit terrain wall.
State* Spawn(const S3DPoint& source, const S3DPoint& target);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
