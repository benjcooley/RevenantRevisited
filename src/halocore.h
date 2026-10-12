#pragma once
#include "math3d.h"
namespace halo_core {
struct State {
    int totalframes, frame=0;
    float step, scale=0;
    bool done=false;
    State(int total,float delta):totalframes(total),step(delta){}
    void Advance() {
        if(done)return;
        scale=frame<totalframes/2?scale+step:scale-step;
        if(++frame>totalframes)done=true;
    }
    bool Matrix(hmm_mat4& m) const {
        if(done || scale<=0)return false;
        MtxClear(&m);hmm_vec3 size={scale,scale,scale};MtxScale(&m,&size);
        MtxRotateX(&m,-1.5707963705062866f);
        MtxRotateZ(&m,-.7853981852531433f);
        hmm_vec3 p={-5,-5,5};MtxTranslate(&m,&p);
        return true;
    }
};
}
