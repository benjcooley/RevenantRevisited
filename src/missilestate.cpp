#include "missilestate.h"

#include <stdexcept>

namespace missile_state
{
namespace
{
// Define the original 32-bit wrap explicitly instead of C++ signed overflow.
int32_t Add(int32_t a,int32_t b)
{
    const uint32_t value=uint32_t(a)+uint32_t(b);
    return value<=0x7fffffff ? int32_t(value) : int32_t(int64_t(value)-0x100000000ll);
}
int64_t Magnitude(int32_t value) { return value<0 ? -int64_t(value) : value; }
int32_t Subtract(int32_t a,int32_t b)
{
    const uint32_t value=uint32_t(a)-uint32_t(b);
    return value<=0x7fffffff ? int32_t(value) : int32_t(int64_t(value)-0x100000000ll);
}
void Roll(int32_t& accumulated,int32_t& position)
{
    if(Magnitude(accumulated)<Rollover)return;
    const int32_t units=accumulated/Rollover;
    position=Add(position,units);
    accumulated=Subtract(accumulated,int32_t(int64_t(units)*Rollover));
}
}

uint32_t State::Move(const Inputs& input)
{
    if((flags&Immobile)||input.in_inventory||input.object_class==9||
       input.object_class==10||input.object_class==14)return 0;
    if(!input.ground_height)throw std::invalid_argument("Missile movement requires a ground query");
    Point next=position;
    uint32_t result=0;
    const int32_t ground=Add(input.ground_height(position),1);
    if((position.z<ground||ground==1)&&!(flags&NoCollision))return Blocked;
    if(ground<position.z&&velocity_fixed.z>-50*Rollover&&!(flags&Weightless))
    {
        velocity_fixed.z=Add(velocity_fixed.z,-6*Rollover);
        result|=Falling;
        // Literal original comparison mixes stored position and fixed velocity.
        if(Add(position.z,velocity_fixed.z)<Add(ground,1))
            velocity_fixed.z=Subtract(Add(ground,1),position.z);
    }
    accumulator.x=Add(Add(accumulator.x,input.next_move_fixed.x),velocity_fixed.x);
    accumulator.y=Add(Add(accumulator.y,input.next_move_fixed.y),velocity_fixed.y);
    accumulator.z=Add(Add(accumulator.z,input.next_move_fixed.z),velocity_fixed.z);
    Roll(accumulator.x,next.x);Roll(accumulator.y,next.y);Roll(accumulator.z,next.z);
    if(next.z<ground)
    {
        velocity_fixed.z=Magnitude(velocity_fixed.z)>6*Rollover ? -(velocity_fixed.z/4) : 0;
        next.z=ground;accumulator.z=0;
    }
    if(input.object_class==12&&next.z<ground)next.z=ground;
    if(next==position&&velocity_fixed==Point{})return 0;
    position=next;
    return result|Moved;
}

Result State::Advance(const Inputs& input)
{
    Result result;result.move_bits=Move(input);
    switch(state)
    {
        case 0:
            if(input.launch_ready)
            {
                if(speed_fixed)
                {
                    if(!input.aim_angle||!input.convert_vector)
                        throw std::invalid_argument("Missile launch requires aim and vector inputs");
                    const int32_t speed=speed_fixed/Rollover;
                    if(!speed)throw std::invalid_argument("Nonzero missile speed must contain a whole fixed-point unit");
                    angle=input.aim_angle();velocity_fixed=input.convert_vector(angle,speed_fixed);
                    flags=(flags&~Immobile)|Moving|Weightless;
                    velocity_fixed.z=-(speed_fixed/16);
                    range=480/speed;
                }
                state=1;result.launched=true;
            }
            break;
        case 1:
        {
            range=Add(range,-1);
            bool explode=range<=0;
            if(result.move_bits&Blocked)explode=true;
            else if(input.character_hit&&input.character_hit(position))explode=true;
            if(explode)
            {
                flags=(flags&~Moving&~Weightless)|Immobile;
                state=2;result.impacted=true;
            }
            break;
        }
        case 2:
            if(!input.animator_present)
            {
                flags|=Kill|Pulse;result.kill_requested=true;
            }
            break;
    }
    return result;
}
}
