#pragma once
#include <cstdint>
namespace rift_qualified_core {
// Executed401cb4..401cf1: preserve the original row-index arithmetic,
// including negative and >1 V offsets. Wrap is the original texture policy.
inline void Offset(int32_t global_frame,float result[2]){int32_t cell=global_frame%16;int32_t quotient=cell;int32_t column=quotient%4;int32_t row=quotient-4*column;result[0]=float(column)*.25f;result[1]=float(row)*.25f;}
}
