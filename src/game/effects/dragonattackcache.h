#pragma once
#include <array>

namespace retail_dragonattack {
// Original4033db reads object zero's cached matrix, then adds owner XYZ.
// Particle Render marks that prototype manual (4026e1). CalcObjectMatrix
// clears it before its zero-scale rejection; a successful sample retains its
// absolute MODELZ translation. Later Animate must not replace this cache.
struct PrototypeOrigin {
    std::array<float,3> value{};
    bool manual=false;
    void Authored(const std::array<float,3>& position,const std::array<float,3>& scale){
        if(!manual && scale[0]>9.999999747378752e-6f &&
           scale[1]>9.999999747378752e-6f && scale[2]>9.999999747378752e-6f)value=position;
    }
    void Sample(float scale,const std::array<float,3>& position){
        manual=true;
        value=scale>9.999999747378752e-6f ? position : std::array<float,3>{};
    }
};
}
