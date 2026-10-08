#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace punch_authored_preview {
struct State;
State* Spawn(const S3DPoint&);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
