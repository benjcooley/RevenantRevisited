#pragma once
#include <cstdint>
struct S3DPoint;
namespace jtele_preview {
struct State;
State* Spawn(const S3DPoint&,uint32_t);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
