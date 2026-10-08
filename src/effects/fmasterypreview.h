#pragma once
struct S3DPoint;enum class EFxDebugMode:unsigned char;
namespace fmastery_authored_preview{struct State;State*Spawn(const S3DPoint&);void Advance(State*,double);void SubmitWorld(State*,EFxDebugMode);void Destroy(State*);}
