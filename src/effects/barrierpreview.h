#pragma once
#include <cstdint>
struct S3DPoint;
namespace barrier_preview {
struct State;
State* Spawn(uint32_t type_id,const S3DPoint& origin);
void Advance(State*,double seconds);
void SubmitWorld(State*);
bool Alive(const State*);
void Destroy(State*);
}
