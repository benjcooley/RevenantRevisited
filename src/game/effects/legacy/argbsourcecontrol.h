#pragma once
namespace argb_source_control {
struct State;
State* Spawn(bool energy=false);
void SubmitWorld(State*);
void Destroy(State*);
}
