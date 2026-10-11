#pragma once
#include <cstdint>
struct S3DPoint;
namespace camera_particles_preview {
struct State;
State* Spawn(const S3DPoint& origin,uint32_t type_id);
void Advance(State* state,double seconds);
void SubmitWorld(State* state);
void Destroy(State* state);
}
