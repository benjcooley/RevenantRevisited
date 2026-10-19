#pragma once
#include "engine/core/math3d.h"
#include <array>
#include <cstddef>
namespace legacy_bone_compose {
// All inputs are the current local matrix mirrors. Resolve parents before
// composing a child, even when a serialized child precedes its parent.
template<size_t Capacity,class Get,class ComposeParent> bool Compose(int count,Get object,ComposeParent compose_parent) {
    if(count<0 || size_t(count)>Capacity)return false;
    std::array<unsigned char,Capacity> marks{};
    auto finish=[&](auto&& self,int index)->bool {
        auto* node=object(index);if(!node)return true;
        if(marks[index]==2)return true;
        if(marks[index]==1)return false;
        marks[index]=1;
        if(node->parent && compose_parent(index)) {
            int parent=-1;
            for(int i=0;i<count;++i)if(object(i)==node->parent){parent=i;break;}
            if(parent<0 || !self(self,parent))return false;
            MtxMultiply(&node->matrix,&node->matrix,&node->parent->matrix);
        }
        marks[index]=2;return true;
    };
    for(int i=0;i<count;++i)if(!finish(finish,i))return false;
    return true;
}
template<size_t Capacity,class Get> bool Compose(int count,Get object) {
    return Compose<Capacity>(count,object,[](int){return true;});
}
}
