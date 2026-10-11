#pragma once
namespace argb_source_control {
struct State;
State* Spawn();
void SubmitWorld(State*);
void Destroy(State*);
}
