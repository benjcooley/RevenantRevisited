#pragma once
namespace prelit_quad_control {
struct State;
State* Spawn();
void SubmitWorld(State*);
void Destroy(State*);
}
