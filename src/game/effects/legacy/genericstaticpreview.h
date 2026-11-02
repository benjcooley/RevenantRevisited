#pragma once
#include <cstdint>
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace generic_static_preview {
struct State;
State* Spawn(const S3DPoint& origin,uint32_t type_id);
void Advance(State* state,double seconds);
void SubmitWorld(State* state,EFxDebugMode mode);
void Destroy(State* state);
}
