#pragma once
#include "engine/core/revtypes.h"
namespace burn_public_preview {struct State;State* Spawn(const S3DPoint&);void Advance(State*,double);void SubmitWorld(State*);bool Alive(State*);void Destroy(State*);}
