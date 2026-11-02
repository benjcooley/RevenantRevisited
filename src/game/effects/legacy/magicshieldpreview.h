#pragma once
#include "engine/core/revtypes.h"
namespace magicshield_preview {struct State;State* Spawn(const S3DPoint&,bool fire_flash);void Advance(State*,double);void SubmitWorld(State*);bool Alive(State*);void Destroy(State*);}
