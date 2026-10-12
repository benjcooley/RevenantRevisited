#pragma once
#include "../revtypes.h"
namespace halo_preview {
struct State;
State* Spawn(const S3DPoint& origin,int totalframes,float step);
void Advance(State*,double seconds);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
