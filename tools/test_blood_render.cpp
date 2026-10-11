#include "../src/i3dtexturealpha.h"
#include "../src/fxsubmissionorder.h"
#include <cassert>
#include <iostream>

int main()
{
    // Transparent, partial and opaque BLACK are distinct authored ARGB4444
    // texels. RGB-only black-key inference must not erase their coverage.
    std::vector<uint8_t> alpha={0,0,0,0, 0,0,0,34, 0,0,0,255, 148,0,0,255};
    const auto original=alpha; ApplyI3DBlackKey(alpha,true); assert(alpha==original);
    auto rgb=original; ApplyI3DBlackKey(rgb,false);
    assert(rgb[3]==0 && rgb[7]==0 && rgb[11]==0 && rgb[15]==255 && rgb[12]==148);
    // Preserve the existing strictly-greater-than-20% RGB heuristic.
    std::vector<uint8_t> boundary={0,0,0,255, 1,0,0,255, 1,0,0,255, 1,0,0,255, 1,0,0,255};
    auto before=boundary; ApplyI3DBlackKey(boundary,false); assert(boundary==before);
    struct Draw { int texture; bool ordered; };
    auto sort=[](auto& q) { SortFxUnorderedRuns(q.begin(),q.end(),
        [](const Draw& d){return d.ordered;},[](const Draw&a,const Draw&b){return a.texture<b.texture;}); };
    std::vector<Draw> normal={{4,false},{2,false},{3,false}};sort(normal);
    assert(normal[0].texture==2 && normal[1].texture==3 && normal[2].texture==4);
    std::vector<Draw> blood={{4,false},{2,false},{13,true},{12,true},{13,true},{12,true},{7,false},{5,false}};
    sort(blood); const int expected[]={2,4,13,12,13,12,5,7};
    for (int i=0;i<8;++i) assert(blood[i].texture==expected[i]);
    std::cout<<"Blood authored-alpha/RGB black-key and ordered mask/color tests passed\n";
}
