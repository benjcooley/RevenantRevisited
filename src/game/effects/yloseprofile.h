#pragma once
#include <cstdint>
// Exact shipped YLose aeaeeb31 / Magic/ylose.i3d.
namespace retail_ylose {
struct Object {const char* name;int material,vertices,faces;int keys[1];uint64_t key_hash[1],vertex_hash,face_hash;int face_counts[5];};
inline constexpr Object objects[]={
{"spheretop05",0,27,39,{34},{0xcb05e5a9b9dcc162ull},0x4f899c9ae4894e83ull,0xca7992ebbc043388ull,{0,39,0,0,0}},
{"spheretop02",0,27,39,{34},{0x35743ff026d287faull},0xe17422efb429cf8full,0xca7992ebbc043388ull,{0,39,0,0,0}},
{"boltring02",1,24,22,{34},{0xcd1c4b7113531d8aull},0x0e620db4a2c6ca9full,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"boltring03",1,24,22,{34},{0x8c140540ff6f6b46ull},0x88e1baa4be0afc4bull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"spheretop04",0,27,39,{34},{0x81fcd7db2e8b847aull},0x364bc0d7740d1653ull,0xca7992ebbc043388ull,{0,39,0,0,0}},
{"spheretop03",0,27,39,{34},{0x755e78fd68748402ull},0x5891136da96a0089ull,0xca7992ebbc043388ull,{0,39,0,0,0}},
{"boltring04",1,24,22,{34},{0x0b5ef9ff8940a735ull},0xd782775f1aa7447eull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"boltring05",1,24,22,{34},{0xa06ecd66739c6710ull},0x59f5e806b3e168d1ull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"sphere02",2,72,130,{22},{0x412b59405c475467ull},0x5a1a940c1f6e7664ull,0x3c2607a8c82595fdull,{0,0,0,130,0}},
{"#ring02",0,26,26,{35},{0x85600a7afe3da689ull},0xc4bc745446c622feull,0x628b6034c817552aull,{0,26,0,0,0}},
{"boltring06",1,24,22,{34},{0x335db280e54bd3baull},0x286b94d1f9fc029full,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"boltring07",1,24,22,{34},{0x209076a5cc511ff8ull},0x7ca40e970cf7df49ull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"boltring08",1,24,22,{34},{0x34dd032c8fdc7cd5ull},0x3df99ad023fba647ull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"#ring03",0,26,26,{35},{0x172be20795cd9aa3ull},0x7c923a5639f3ae6bull,0x628b6034c817552aull,{0,26,0,0,0}},
{"boltring09",1,24,22,{34},{0x98036a927a3c5b40ull},0x55489f0333b07b0cull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"boltring10",1,24,22,{34},{0xa545b0ae298d33eaull},0x93f2349d789652daull,0x6d3cf87514fe35ebull,{0,0,22,0,0}},
{"circle01",3,28,26,{33},{0xaa014e7292fcbfd3ull},0xaffc053af6aa13c0ull,0x0628c4e3204938a1ull,{0,0,0,0,26}},
};
inline constexpr uint64_t materials[]={0xd255f18bf062cb9cull,0xd255f18bf062cb9cull,0xd255f18bf062cb9cull,0xd255f18bf062cb9cull};
inline constexpr int material_textures[]={0,1,2,3};
inline constexpr const char* fade_definition="obj (\"boltring02\",\"boltring03\",\"spheretop04\",\"spheretop03\",\"boltring04\",\"boltring05\",\"sphere02\",\"#ring02\",boltring06\",\"boltring07\",boltring08\",\"#ring03\",\"boltring09\",\"boltring10\",\"circle01\"),color [0:(100,100,100),5:(180,180,180),13:(0,0,0)]";
}
