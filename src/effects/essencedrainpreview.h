#pragma once
#include "../revenant.h"
namespace essencedrain_preview{struct State;State*Spawn(const S3DPoint&);bool Alive(State*);void Advance(State*,double);void SubmitWorld(State*);void Destroy(State*);}
