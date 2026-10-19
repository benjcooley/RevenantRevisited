#pragma once
#include <cstdint>
// Exact shipped GVortex ad99bd33 / Misc/gvortex.i3d.
namespace retail_gvortex {
struct Object {const char* name;int material,vertices,faces;int keys[1];uint64_t key_hash[1],vertex_hash,face_hash;int face_counts[4];};
inline constexpr Object objects[]={
{"box01",0,4,2,{583},{0x8d0114028f2b81b8ull},0x5e2c11647bb424b9ull,0xf44208a16a617ed8ull,{0,2,0,0}},
{"blast 03",1,20,18,{431},{0xb377b19eb248cccbull},0xb533241e60018b45ull,0xdd6762b343804edaull,{0,0,18,0}},
{"blast 04",1,20,18,{385},{0xaaa0e8c67a28d919ull},0x378eee12792f499dull,0xdd6762b343804edaull,{0,0,18,0}},
{"blast 05",1,20,18,{321},{0x3b5c4c3820dd9d42ull},0x04d4ec294db9a2c6ull,0xdd6762b343804edaull,{0,0,18,0}},
{"cylinder07",2,22,20,{948},{0xa55891312830b621ull},0x17d59d36bf49db6full,0xdb9ab368aa7ee7a2ull,{0,0,0,20}},
{"cylinder06",2,22,20,{948},{0xdd3fdcbd5e9b4865ull},0xd066370b9f797aa0ull,0xdb9ab368aa7ee7a2ull,{0,0,0,20}},
{"blast 01",1,20,18,{477},{0xdadb2c0bfaccb392ull},0x45bf55fd81d87bb7ull,0xdd6762b343804edaull,{0,0,18,0}},
{"blast 02",1,20,18,{463},{0x04e87a3160569228ull},0xb533241e60018b45ull,0xdd6762b343804edaull,{0,0,18,0}},
{"#$flare03",0,48,24,{387},{0x705f4831ff3d7750ull},0x18d381a441958cc1ull,0x67de1102d0bd1a83ull,{0,24,0,0}},
{"#$flare04",0,48,24,{132},{0x96a05842a515e6adull},0x24b7348ac6d0ef7aull,0x67de1102d0bd1a83ull,{0,24,0,0}},
{"#$flare01",0,48,24,{261},{0xf6d2832b601e438cull},0x87a88dbb766a9026ull,0x67de1102d0bd1a83ull,{0,24,0,0}},
{"#$flare02",0,48,24,{325},{0x5a39b95d2a4372afull},0x0c7efc638f80e37cull,0x67de1102d0bd1a83ull,{0,24,0,0}},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={0,1,2};
}
