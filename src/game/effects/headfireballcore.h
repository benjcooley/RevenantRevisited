#pragma once
// Original RunFireHeadN single-child527140/526fd0. The character-query boundary
// is a declared negativeY plane, not an inferred physical collision algorithm.
namespace retail_headfireball {
struct State {
    int state=0,frame=0,x=0,y=0,z=0,target_y=0;
    int velocity_x=0,velocity_y=0,velocity_z=0;
    bool alive=true;
    void Initialize(int px,int py,int pz){
        state=frame=0;x=px;y=py;z=pz;target_y=py-120;
        velocity_x=velocity_y=velocity_z=0;alive=true;
    }
    void Pulse(){
        if(!alive)return;
        ++frame;
        if(state==0 && frame>=19){state=1;frame=0;velocity_y=-8*65536;}
        if(state==1){
            if(frame>=14)frame=0;
            y-=8;
            if(y<=target_y){state=2;frame=0;}
        }
        if(state==2 && frame>=54)alive=false;
    }
};
}
