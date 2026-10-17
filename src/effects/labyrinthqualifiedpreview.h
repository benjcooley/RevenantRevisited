#pragma once
#include "../revtypes.h"
namespace labyrinth_qualified_preview {struct State;State* Spawn(const S3DPoint&);void Advance(State*,double);void SubmitWorld(State*);bool Alive(State*);void Destroy(State*);}
