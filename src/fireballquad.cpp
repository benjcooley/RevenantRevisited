#include "fireballquad.h"

namespace fireball_quad
{
Quad Build(const std::array<hmm_vec3,4>& authored,int frame,int spin_degrees,
           int owner_face,float scale,const hmm_vec3& local_position,const hmm_mat4& owner_world)
{
    // Literal operations/ordering from retail 0x511dd0 and 0x511fa0.
    // Facing truncates integer degrees BEFORE conversion to radians. Spin is
    // stored as an integer at animator+0x4bc, unlike its adjacent float frame.
    constexpr double radians=3.14159265358979323846/180.0;
    hmm_mat4 local{},world{};MtxClear(&local);
    if(spin_degrees)MtxRotateZ(&local,float(-double(spin_degrees)*radians));
    MtxRotateX(&local,-0.523598790168762207f);
    MtxRotateY(&local,1.047197580337524414f);
    MtxRotateZ(&local,float(double(-(owner_face*360)/256)*radians));
    const hmm_vec3 uniform{scale,scale,scale};MtxScale(&local,&uniform);
    MtxTranslate(&local,&local_position);MtxMultiply(&world,&local,&owner_world);
    Quad result;
    const float u=float(frame%4)*.25f,v=float(frame/4)*.25f;
    for(int i=0;i<4;++i)
    {
        MtxTransform(&world,&authored[size_t(i)],&result.position[size_t(i)]);
        // Retail rewrites vertices1/2 in this order; rectangular atlas input
        // to an ordinary WorldXY particle swaps them and changes the image.
        result.uv[size_t(i)]={u+float(i>>1)*.25f,v+float(i&1)*.25f};
    }
    return result;
}

Quad BuildSparkNativeD3D(const std::array<hmm_vec3,4>& authored,
                const std::array<std::array<float,2>,4>& uv,float scale,
                const hmm_vec3& absolute_position)
{
    // Literal native shared Render 0x50b2b0: ABSPOS skips owner transform.
    hmm_mat4 matrix{};MtxClear(&matrix);
    MtxRotateZ(&matrix,-1.047197580337524414f);
    MtxRotateX(&matrix,-0.785398185253143311f);
    MtxRotateY(&matrix,0.0f);
    const hmm_vec3 uniform{scale,scale,scale};MtxScale(&matrix,&uniform);
    // Retail binary FIX_Z includes terms omitted in the source snapshot's
    // simplified macro. Preserve native float literals and x87 intermediates.
    const long double z=absolute_position.Z;
    const float d3d_z=float(z/(static_cast<long double>(1.4600000381469727f)-
        static_cast<long double>(0.009999999776482582f)*
        (z*static_cast<long double>(0.0033333334140479565f)))*
        static_cast<long double>(1.0379999876022339f));
    const hmm_vec3 position{absolute_position.X,absolute_position.Y,d3d_z};
    MtxTranslate(&matrix,&position);
    Quad result;result.uv=uv;
    for(int i=0;i<4;++i)
    {
        MtxTransform(&matrix,&authored[size_t(i)],&result.position[size_t(i)]);
    }
    return result;
}

Quad BuildSpark(const std::array<hmm_vec3,4>& authored,
                const std::array<std::array<float,2>,4>& uv,float scale,
                const hmm_vec3& absolute_position)
{
    Quad result=BuildSparkNativeD3D(authored,uv,scale,absolute_position);
    for(int i=0;i<4;++i)
    {
        // Shared quad vertices are common map/world coordinates, while this
        // ABS matrix is D3D coordinates. Same REV_FIX_Z_VALUE bridge as
        // object.h and the other absolute authored effect adapters.
        result.position[size_t(i)].Z*=1.4600000381469727f;
    }
    return result;
}
}
