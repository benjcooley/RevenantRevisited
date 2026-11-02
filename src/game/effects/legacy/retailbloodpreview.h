#pragma once

struct S3DPoint;
enum class EFxDebugMode : unsigned char;

// Gallery adapter only: one actual "Blood" owner (TBloodEffect + the retail
// TBloodAnimator) with the parameters TCharacter::EffectBurst passes for a
// blow (height 50, heading facing + 128, elevation 3, spreads 5), driven at
// the 24 Hz tick and drawn through the animator's own submission.
namespace retail_blood_preview {
struct State;
State* Spawn(const S3DPoint& origin);
void Advance(State*, double seconds);
void Submit(State*, EFxDebugMode);
bool Alive(State*);
void Destroy(State*);
}
