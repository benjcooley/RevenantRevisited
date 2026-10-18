#pragma once
#include "engine/render/retailsoftwaretransform.h"
#include <cmath>

// Original textured raster56e34a..56e3e1 rejects either edge from vertex0
// when its projected |dx|>640 or |dy|>480. These are literal .rdata limits,
// not the viewport dimensions. Keep the authored triangles and order intact.
inline bool RetailCometProjectedTriangleAllowed(const hmm_vec3 points[3])
{
    for(int i=1;i<3;++i)
        if(std::fabs(points[0].X-points[i].X)>640.0f ||
           std::fabs(points[0].Y-points[i].Y)>480.0f)return false;
    return true;
}
inline bool RetailCometTriangleAllowed(const hmm_vec3 vertices[3],const hmm_mat4& native_world)
{
    hmm_mat4 view,combined;BuildRetailSoftwareViewProjection(view,0,0,0);
    MtxMultiply(&combined,&native_world,&view);
    hmm_vec3 points[3];for(int i=0;i<3;++i)MtxTransform(&combined,&vertices[i],&points[i]);
    return RetailCometProjectedTriangleAllowed(points);
}
