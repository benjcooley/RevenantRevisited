#pragma once
#include "engine/core/revtypes.h"
namespace regeneration_spell_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*,double seconds);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
