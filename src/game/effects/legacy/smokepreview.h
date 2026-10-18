#pragma once
struct S3DPoint;
namespace smoke_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
