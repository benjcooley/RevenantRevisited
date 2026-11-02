#pragma once
#include <cstdint>
namespace retail_ytelekinesis {
struct Object{const char* name;int material,vertices,faces;uint64_t vertex_hash,face_hash,key_hash;};
inline constexpr Object objects[]={
{"piece2",0,123,58,0x3a49c3e934c6ba27ull,0xf78a8a38359cebf5ull,0x5b583602e887f40bull},
{"piece1",1,138,52,0x61977cf75c3528ebull,0x1ca48b9bc4966ee3ull,0x47756d172ad87af9ull},
{"piece3",2,132,68,0xb38b0ff6961c42f9ull,0x8408e0db14ba8eddull,0x568e6dfc2083f4e1ull},
{"piece4",3,96,44,0xaf419db5e6bf5622ull,0xf01cb4362fd5e6d5ull,0xb5bb3fdc9c85b2bbull},
};
}
