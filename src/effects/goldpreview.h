#pragma once

struct S3DPoint;
enum class EFxDebugMode : unsigned char;

// Matched-backdrop adapter only: actual Gold owner, default animator and tags.
namespace gold_authored_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
