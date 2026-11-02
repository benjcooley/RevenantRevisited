#pragma once
struct S3DPoint;
namespace strike_meteor_preview {
struct State;
State* SpawnFixture(const S3DPoint&);
State* SpawnConfigured(const S3DPoint&);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
