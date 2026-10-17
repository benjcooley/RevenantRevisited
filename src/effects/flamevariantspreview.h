#pragma once
#include <cstdint>
struct S3DPoint;
class TObjectInstance;
class T3DImagery;
enum class EFxDebugMode : uint8_t;
namespace flame_variants_preview {
struct State;
bool Matches(const TObjectInstance*,T3DImagery*);
State* Spawn(uint32_t type_id,const S3DPoint& origin);
void Submit(State*,EFxDebugMode);
void Destroy(State*);
}
