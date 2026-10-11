#pragma once
struct S3DPoint;
namespace paralize_preview {
struct State;
// Only this genuine-actor configuration has an original whole-lifecycle
// reference. Gallery placement translates its display; it is not an arbitrary
// endpoint, target-radius, attachment or skeletal-pose API.
struct TargetConfiguration {
    const char* actor_type;
    int source_x,source_y,source_z;
    int target_dx,target_dy,target_dz;
    int expected_radius;
};
inline constexpr TargetConfiguration kConfiguredTarget={"Acolyte",3185,3185,16,96,0,0,16};
State* Spawn(const S3DPoint&);
State* SpawnTargetFixture(const S3DPoint&);
State* SpawnConfiguredTarget(const S3DPoint& display_origin);
void Advance(State*,double);
void SubmitWorld(State*);
bool Alive(State*);
void Destroy(State*);
}
