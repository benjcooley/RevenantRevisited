#pragma once
struct S3DPoint;
enum class EFxDebugMode : unsigned char;
namespace arrow_original {
struct State;
// QueenArrow: explicit non-character launch source and stationary collision plane.
State* SpawnQueen(const S3DPoint& source,const S3DPoint& target);
void Advance(State*,double seconds);
void SubmitWorld(State*,EFxDebugMode);
bool IsAlive(const State*);
void Destroy(State*);
}
