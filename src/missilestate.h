#pragma once

#include <cstdint>
#include <functional>

// Original retail 0x470920 Move and 0x510220 missile Pulse. World queries and
// aiming are explicit inputs; fixed-point motion is owned here, never Submit.
namespace missile_state
{
struct Point
{
    int32_t x=0,y=0,z=0;
    bool operator==(const Point& other) const { return x==other.x&&y==other.y&&z==other.z; }
    bool operator!=(const Point& other) const { return !(*this==other); }
};

constexpr int32_t Rollover=65536;
constexpr uint32_t Immobile=1,Moving=8,Weightless=0x10000,NoCollision=0x1000000;
constexpr uint32_t Kill=0x1000,Pulse=0x8000;
constexpr uint32_t Moved=1,Blocked=2,Falling=4;

struct Inputs
{
    bool in_inventory=false;
    int object_class=25;
    Point next_move_fixed{};
    bool launch_ready=false;
    bool animator_present=false;
    std::function<int32_t(const Point&)> ground_height;
    std::function<int()> aim_angle;
    std::function<Point(int,int32_t)> convert_vector;
    // The caller excludes source/dead/friendly entities and uses its original
    // distance lookup. Returning true means a live foe is within 32 units.
    std::function<bool(const Point&)> character_hit;
};

struct Result
{
    uint32_t move_bits=0;
    bool launched=false,impacted=false,kill_requested=false;
};

struct State
{
    Point position{},velocity_fixed{},accumulator{};
    uint32_t flags=Immobile;
    int32_t state=0,range=32768,speed_fixed=8*Rollover;
    int angle=0;
    uint32_t Move(const Inputs&);
    Result Advance(const Inputs&);
};
}
