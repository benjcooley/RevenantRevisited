#pragma once

struct S3DPoint;
enum class EFxDebugMode : unsigned char;

// Matched-backdrop adapter only: the actual CombatFlash owner in one of its
// nine start states (start 1..9 = Impact state 0, 2, .. 16), its default
// animator and tags. EffectCombatFlash picks the state in play.
namespace combatflash_preview {
struct State;
State* Spawn(const S3DPoint& origin, int start);
void Advance(State*, double seconds);
void SubmitWorld(State*, EFxDebugMode);
bool Alive(State*);
void Destroy(State*);
}
