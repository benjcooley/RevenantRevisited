#pragma once
#include "game/app/revenant.h"
#include "game/ab/vfxtest.h"
namespace fire_installed {
struct State;
State* Spawn(const S3DPoint&,uint8_t facing);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
