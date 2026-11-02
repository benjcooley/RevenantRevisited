#pragma once
struct S3DPoint;
namespace skin_preview {
struct State;
State* Spawn(const S3DPoint& origin,bool iron);
void Advance(State*, double seconds);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
