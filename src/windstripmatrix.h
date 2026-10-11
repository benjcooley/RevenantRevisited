#pragma once
#include "math3d.h"
#include "windstripcore.h"
namespace retail_windstrip {
inline hmm_mat4 BillboardMatrix(float scale,Point p){hmm_mat4 m;MtxClear(&m);hmm_vec3 s{scale,scale,scale};MtxScale(&m,&s);MtxRotateX(&m,-1.5707963705062866f);MtxRotateZ(&m,-.7853981852531433f);hmm_vec3 xyz{p.x,p.y,p.z};MtxTranslate(&m,&xyz);return m;}
inline Point StripVertex(const Strip& s,int vertex){const auto p=s.points[vertex/2];const float sign=(vertex&1)?1.f:-1.f;const float rx=7.f*float(std::cos(-60.*.017453292519943295));const double rz=7.*double(float(std::sin(-60.*.017453292519943295)));return {p.x+sign*rx,p.y,float(double(p.z)+double(sign)*rz)};}
inline std::array<unsigned short,54> StripFaces(){std::array<unsigned short,54> result{};constexpr unsigned short pattern[4][3]={{2,0,3},{1,3,0},{2,4,3},{5,3,4}};for(int i=0;i<18;++i)for(int j=0;j<3;++j)result[i*3+j]=pattern[i%4][j]+4*(i/4);return result;}
}
