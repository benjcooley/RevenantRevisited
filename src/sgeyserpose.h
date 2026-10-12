#pragma once
#include "speedauthoredmatrix.h"
#include <cmath>
namespace retail_sgeyser {
// Loaded default PreRender supplies previous state0/frame0. Exact initial
// transition from409950; ordinary game decoding is deliberately unchanged.
template<class ReadKey> bool Key(ReadKey read,int object,int frame,hmm_vec3& p,hmm_vec3& r,hmm_vec3& s){
 if(!read(object,frame,p,r,s))return false;
 if(frame<5){hmm_vec3 op{},orr{},os{};if(!read(object,0,op,orr,os))return false;
  const float pi=float(M_PI),twopi=float(M_PI*2);
  for(int i=0;i<3;++i){if(orr.Elements[i]-r.Elements[i]>pi)orr.Elements[i]-=twopi;else if(orr.Elements[i]-r.Elements[i]<-pi)orr.Elements[i]+=twopi;}
  if(std::fabs(orr.X-r.X)>pi*.5f || std::fabs(orr.Y-r.Y)>pi*.5f || std::fabs(orr.Z-r.Z)>pi*.5f)orr=r;
  const float weight=float(frame+1)/5.f;const auto blend=[weight](hmm_vec3& dst,const hmm_vec3& old){for(int i=0;i<3;++i)dst.Elements[i]=float(static_cast<long double>(dst.Elements[i])*weight+static_cast<long double>(old.Elements[i])*(1.L-static_cast<long double>(weight)));};
  blend(p,op);blend(r,orr);blend(s,os);
 }return true;
}
inline void Matrix(hmm_mat4& m,int object,const hmm_vec3& p,const hmm_vec3& r,const hmm_vec3& s){
 if(object==0 || object==2){BuildRetailSpeedCameraMatrix(m,p,r,s);return;}
 MtxClear(&m);MtxRotateX(&m,r.X);MtxRotateY(&m,r.Y);MtxRotateZ(&m,r.Z);MtxScale(&m,&s);MtxTranslate(&m,&p);
}
}
