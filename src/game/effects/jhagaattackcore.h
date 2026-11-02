#pragma once
// Original505840 null-spell owner. The original fixed facing0 generates
// (0,-8,0) via Angle; empty character query equals null invoker and therefore
// skips actor/terrain impacts. These are timed phases, not a collision model.
namespace retail_jhagaattack {
struct State {
    int phase=0,counter=0,x=0,y=0,z=0;
    bool alive=true;
    void Initialize(int px,int py,int pz){phase=counter=0;x=px;y=py;z=pz;alive=true;}
    void Pulse(){
        if(!alive)return;
        if(phase==0){if(++counter<20)return;phase=1;counter=0;}
        if(phase==1){y-=8;if(++counter<15)return;phase=2;counter=0;}
        if(phase==2 && ++counter>=55)alive=false;
    }
};
}
