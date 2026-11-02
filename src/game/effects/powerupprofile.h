#pragma once
#include <cstdint>
// Exact shipped PowerUp aeaefb27 / Misc/Powerup.i3d.
namespace retail_powerup {
struct Object {const char* name;int material,vertices,faces;int keys[2];uint64_t key_hash[2],vertex_hash,face_hash;int face_counts[5];};
inline constexpr Object objects[]={
{"ring03",0,48,48,{49,9},{0xac57524299efca56ull,0x6ef9c91b621f923cull},0x890e0fd7ac930ecaull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"trail06",1,42,40,{71,9},{0x8ba05ee9b6c1ab3bull,0x2bd97fa128638450ull},0xbe9dff193871399eull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"ring01",0,48,48,{49,9},{0x35ac8882e657cdcdull,0xcb66975ea2e35cf5ull},0xeacff7d51bd90dcbull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"trail05",1,42,40,{71,9},{0x7795229a771f1b09ull,0xd3a94cbbf52e1c63ull},0xbe9dff193871399eull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"ring05",0,48,48,{49,9},{0x92b4ee233fd87973ull,0x82a1d9cd31cda578ull},0x8559a8f668541c9aull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"ring06",0,48,48,{49,9},{0x6d86794754229f15ull,0x91ea085dd68eaf15ull},0x10b08706b591d478ull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"ring02",0,48,48,{49,9},{0x8817ddf3fcd4d52bull,0x5794ee15d3f9d828ull},0x53a218ddad226639ull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"ring04",0,48,48,{49,9},{0x309c2f9f40b2d7dbull,0xff5b39be37af51b5ull},0x67d2a2a7f84bb13aull,0xb2c22cb834f54363ull,{0,48,0,0,0}},
{"trail01",1,42,40,{71,9},{0x3e7f202b40629f0eull,0xbfe92f70f8422b9cull},0xbe9dff193871399eull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"trail02",1,42,40,{71,9},{0x78e12435ed993958ull,0x9626f787890d269eull},0x42cd36d7f401733dull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"trail03",1,42,40,{71,9},{0x64ca7500847fd5a1ull,0x37612ea18d426377ull},0xbe9dff193871399eull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"trail04",1,42,40,{71,9},{0x0e700b4df727dbffull,0x37612ea18d426377ull},0xbe9dff193871399eull,0xd94a423304be74ebull,{0,0,40,0,0}},
{"clockwisecone01",2,22,20,{334,9},{0x38f421f7200e6e34ull,0xc802e83004040448ull},0xc3d1175c1275c32aull,0xdb9ab368aa7ee7a2ull,{0,0,0,20,0}},
{"counterclockwisecone01",2,22,20,{334,9},{0x59582c45ea753d95ull,0x40b2c3f19e8f1126ull},0xc3d1175c1275c32aull,0xdb9ab368aa7ee7a2ull,{0,0,0,20,0}},
{"pulsingglow01",3,25,24,{89,9},{0x50cf2f62a00f1901ull,0x0f9b10c9766b6cb8ull},0x284077341aa3f5a9ull,0x9d476e549fa54443ull,{0,0,0,0,24}},
};
inline constexpr uint64_t materials[]={0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull,0xaa234e3981f0b7b3ull};
inline constexpr int material_textures[]={0,1,2,3};
}
