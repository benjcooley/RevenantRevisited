#pragma once
#include <cstdint>
// Exact shipped YAttack aeaeeb34 / Magic/yattack.i3d.
namespace retail_yattack {
struct Object {const char* name;int material,vertices,faces;int keys[1];uint64_t key_hash[1],vertex_hash,face_hash;int face_counts[3];};
inline constexpr Object objects[]={
{"#$flare02",0,22,20,{119},{0x923ac6e5e830fd07ull},0x933c5d9d3c1f9f88ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare03",0,22,20,{119},{0xe1f9bb4857e34ed8ull},0xea893b85ad5e7a08ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare04",0,22,20,{119},{0x24f2f7d1e28dab30ull},0x500d11c93338f983ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare05",0,22,20,{119},{0xa878737fa3b1cc5bull},0x6f06a4bf97d73d64ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare06",0,22,20,{119},{0xa797449502a627b5ull},0xa4f1cef2686f4f02ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare07",0,22,20,{119},{0x142643ea217c2db1ull},0x8db0eba0716888e5ull,0x0943908a719e9dafull,{0,20,0}},
{"#$flare08",0,22,20,{119},{0xbea1f32775c6ee71ull},0x2dcf170e11bb45ecull,0x0943908a719e9dafull,{0,20,0}},
{"rectangle01",1,4,2,{94},{0x2468144eba42d84dull},0x5a2f2b4bef586841ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle02",1,4,2,{94},{0x754627996c3dff51ull},0x3859bdfcb74a836bull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle03",1,4,2,{94},{0xaac5a647c83e8c39ull},0xfdf02ea213f9d220ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle04",1,4,2,{94},{0xcc0481b536be6953ull},0x0a255023c4d529c1ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle05",1,4,2,{94},{0x16cfa6401dcc0942ull},0xc00884c81a9b9941ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle06",1,4,2,{94},{0x0074d4278896606dull},0xf43a9cb1d0da1d75ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle07",1,4,2,{94},{0x3d25d43c454e62f3ull},0x520664cfa4d66881ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle08",1,4,2,{94},{0xc98bc7d0c816c7c1ull},0xc1ede3859af7a646ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle09",1,4,2,{94},{0xf6b9740082fc58f6ull},0xa6e6c7c4eb2eb1eeull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle10",1,4,2,{94},{0x0caf342188a28154ull},0xe019fcebe9eaa72cull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle11",1,4,2,{94},{0x74ae22aae61acbd7ull},0x8ad2c7828945ab00ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle12",1,4,2,{94},{0xdb8993e07f367244ull},0x8dd46d7bd14fa65cull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle13",1,4,2,{94},{0x1d3f7ac2f8a3ae50ull},0xffe178295597211bull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle14",1,4,2,{94},{0xf38d9a8226a3f9efull},0x1f6bce0c20eaaf9eull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle15",1,4,2,{94},{0x1cab9a44bb089889ull},0x83200ee1da8d729aull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle16",1,4,2,{94},{0xd131b209f01734a3ull},0x6f7b5840a3bbbdb3ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle17",1,4,2,{96},{0xd2b186623a8fbf8bull},0x10a5fb9a685d3e69ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle18",1,4,2,{96},{0x5e1159f22512c1e5ull},0x7f1a6426c3ccc60aull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle19",1,4,2,{96},{0xf9e8dfbe732d2c98ull},0x265b79c2b6d69048ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle20",1,4,2,{96},{0xb1920beb12d6643full},0x750c53bef8bf71dcull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle21",1,4,2,{96},{0x667e50eb476649ecull},0xaeea4b9f05a937a4ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle22",1,4,2,{96},{0x104dde115034bce1ull},0x8c42b8e46742f72full,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle23",1,4,2,{96},{0xbea09a3fb6262bd2ull},0x84b8f06efb9f1353ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle24",1,4,2,{96},{0xe28b1b41d18b8614ull},0x1930c0c905a9b4d0ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle25",1,4,2,{96},{0x21494a87d947a7f7ull},0x8a22797520c58ec6ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle26",1,4,2,{96},{0xd16d22fcb4e3457dull},0x6f7589371cf2e55cull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle27",1,4,2,{96},{0x9044aadb65f8e6bcull},0xb0cc0c28f4dfc0f3ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle28",1,4,2,{96},{0x74bbe7425a9e4689ull},0x34bbe3fcd6b5d2b5ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle29",1,4,2,{96},{0x82d39bdc6fa84f94ull},0xc59901b5cd87ca90ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle30",1,4,2,{96},{0x3414c5427369e60full},0x47d30d59b55b0358ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle31",1,4,2,{96},{0xd52ce412bdcb4345ull},0x73ea882590cc9c52ull,0x8de70ea438f81391ull,{0,0,2}},
{"rectangle32",1,4,2,{96},{0x5f6c3873f36a1150ull},0x619b56035dedcaf6ull,0x8de70ea438f81391ull,{0,0,2}},
};
inline constexpr uint64_t materials[]={0xa35c0bef94183ad3ull,0xa35c0bef94183ad3ull};
inline constexpr int material_textures[]={0,1};
}
