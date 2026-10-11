#pragma once
#include <cmath>
#include <cstdint>

// Original QueenArrow owner504d20 and custom trail Render505000. World hit
// selection is an explicit callback; actor reaction/damage remain caller APIs.
namespace arrow_core {
struct Point { int32_t x=0,y=0,z=0; };
struct Draw { float scale=0,x=0,y=0,z=0; int object=0; };
struct State {
    Point position{};float vx=0,vy=0;int angle=0,grow=6,color=2,countdown=-1;
    bool initial=true,alive=true;
    template<class Hit,class Random>void Pulse(Point launch,int facing,Hit hit,Random random) {
        if(!alive)return;
        if(initial){position=launch;angle=(facing-64)&255;position.z+=40;initial=false;
            const double a=double(angle)*double(.024736950173974037f);
            vx=float(std::cos(a)*23.0);vy=float(std::sin(a)*23.0);}
        if(countdown>=0){if(countdown==0){alive=false;return;}--countdown;}
        if(hit(position) && grow==0 && countdown<0){random(0,2);random(24,38);countdown=4;}
        if(countdown>=0){if(countdown==0){alive=false;return;}--countdown;}
        if(countdown<0){position.x+=int32_t(vx);position.y+=int32_t(vy);}
        if(grow>0)--grow;
    }
    template<class Random,class Emit>void Render(Random random,Emit emit)const {
        if(initial || !alive)return;
        const int n=int(double(6-grow)*double(.1666666716337204f)*20.0);
        const float spacing=countdown<0?3.f:float(3.0/double(5-countdown));
        for(int i=0;i<n;++i){
            const float t=float(double(i)*double(.05000000074505806f));
            Draw d;d.object=color;d.scale=float(double(1.100000023841858f)-t);
            d.x=float(double(position.x)-double(vx)*t*spacing+random(-4,4));
            d.y=float(double(position.y)-double(vy)*t*spacing+random(-4,4));d.z=float(position.z);emit(d);
        }
    }
};
}
