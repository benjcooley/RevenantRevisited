#pragma once
#include "engine/core/math3d.h"
#include "game/effects/energyspraycore.h"
#include <cmath>
namespace retail_energyspray {
inline hmm_mat4 RadialMatrix(const State&s,int i){hmm_mat4 m;MtxClear(&m);const hmm_vec3 scale{s.scales[i],s.scales[i],s.scales[i]};MtxScale(&m,&scale);MtxRotateZ(&m,.74210852384567260742f);MtxRotateY(&m,64.f);const hmm_vec3 p{float(std::cos(double(s.radial_angles[i]))*s.radii[i]),float(std::sin(double(s.radial_angles[i]))*s.radii[i]),0};MtxTranslate(&m,&p);return m;}
inline hmm_mat4 FlareMatrix(float scale){hmm_mat4 m;MtxClear(&m);const hmm_vec3 scl{scale,scale,scale};MtxScale(&m,&scl);MtxRotateZ(&m,.74210852384567260742f);MtxRotateY(&m,1.1131627559661865234f);return m;}
inline hmm_mat4 TriangleMatrix(const State&s,int i){hmm_mat4 m;MtxClear(&m);const hmm_vec3 p{-52,0,0};MtxTranslate(&m,&p);const hmm_vec3 scl{s.lengths[i],.5f,.5f};MtxScale(&m,&scl);MtxRotateX(&m,s.angles_b[i]*.5f);MtxRotateY(&m,s.angles_b[i]);MtxRotateZ(&m,float(double(s.angles_a[i])+1.5707963705062866));return m;}
inline uint32_t PackedColor(const State&s,int i){return (uint32_t(int32_t(int8_t(s.colors[0][i])))<<16)|(uint32_t(int32_t(int8_t(s.colors[1][i])))<<8)|uint32_t(int32_t(int8_t(s.colors[2][i])));}
}
