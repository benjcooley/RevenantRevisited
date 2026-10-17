#pragma once
#include <cmath>

// Original505a90 owner. Muzzle XY is the original Angle(face,20) result;
// target XY comes from the actual invoker's Fighting target. Ground and the
// character query are declared world inputs, not an inferred collision model.
namespace retail_dragonattack {
struct State {
    int state=0,frame=0,counter=0,x=0,y=0,z=0;
    int vx=0,vy=0,vz=0,source_x=0,source_y=0,source_z=0;
    int muzzle_x=0,muzzle_y=0,target_x=0,target_y=0,target_ground=0;
    bool alive=true;

    bool Initialize(int px,int py,int pz,int mx,int my,int tx,int ty,int ground){
        *this=State{};source_x=x=px;source_y=y=py;source_z=z=pz;
        muzzle_x=mx;muzzle_y=my;target_x=tx;target_y=ty;target_ground=ground;
        return double(tx)-(double(px)+mx)!=0 || double(ty)-(double(py)+my)!=0 ||
               double(ground)-(double(pz)+100)!=0;
    }

    void Pulse(int ground,bool character_hit=false){
        if(!alive)return;
        ++frame;
        if(state==0){
            if(counter==0){
                x=source_x+muzzle_x;y=source_y+muzzle_y;z=source_z+100;
                const double dx=double(target_x)-x,dy=double(target_y)-y,dz=double(target_ground)-z;
                const double distance=std::sqrt(dx*dx+dy*dy+dz*dz);
                // Original5b18a8 is20*65536; _ftol truncates toward zero.
                vx=int(dx/distance*1310720.0);vy=int(dy/distance*1310720.0);vz=int(dz/distance*1310720.0);
            }
            if(counter>=11){state=1;counter=0;frame=0;}
        }
        if(state==1){
            // Native copies velocity into locals and drops each fractional
            // remainder after this Pulse; it does not carry a fixed accumulator.
            x+=vx/65536;y+=vy/65536;z+=vz/65536;
            if(counter>=14)frame=0;
            if(z<ground-32 || character_hit){state=2;counter=0;frame=0;}
        }
        if(state==2 && counter>=54)alive=false;
        ++counter;
    }
};
}
