#pragma once
struct S3DPoint;
namespace sgeyser_preview {
struct State;
State* Spawn(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
