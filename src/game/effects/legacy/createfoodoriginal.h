#pragma once
#include "game/app/revenant.h"
namespace createfood_original {struct State;State*Spawn(const S3DPoint&);bool Alive(State*);void Advance(State*,double);void SubmitWorld(State*);void Destroy(State*);}
