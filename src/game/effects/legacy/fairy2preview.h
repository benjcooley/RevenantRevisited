#pragma once
struct S3DPoint;
namespace fairy2_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*, double seconds);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
