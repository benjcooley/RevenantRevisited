#pragma once
struct S3DPoint;
// Explicit modified-retail diagnostic only. Never a Sandswirl retail mapping.
namespace sandswirl_qualified_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
