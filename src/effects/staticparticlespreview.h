#pragma once
#include <cstdint>
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace static_particles_preview {
struct State;
State* Spawn(const S3DPoint&, uint32_t type_id);
void Advance(State*, double);
void SubmitWorld(State*, EFxDebugMode);
void Destroy(State*);
}
