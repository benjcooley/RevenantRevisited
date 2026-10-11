#pragma once
#include "3dimage.h"
#include <cctype>
#include <cstring>
#include <string>
namespace retail_flies {
inline constexpr uint32_t Id=0xa21d9d00u;
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline bool Matches(T3DImagery*im){
 if(!im||!im->GetResFilename()||im->NumStates()!=1||im->GetAniLength(0)!=100||im->NumObjects()!=1||im->NumMaterials()!=1||im->NumTextures()!=1||im->NumTags()!=0||im->NumObjVerts(0)!=4||im->NumObjFaces(0)!=2)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));const char*suffix="misc/flies.i3d";const size_t n=std::strlen(suffix);if(path.size()<n||path.compare(path.size()-n,n,suffix)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 S3DObj o={};im->GetObject(0,&o);if(std::strcmp(o.name,"box01")||o.material!=0||o.parent[0]!=-1)return false;
 S3DVertex vertices[4]={};S3DFace faces[2]={};int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};im->GetObjVerts(0,vertices,0,0);im->GetObjFaces(0,faces,starts,counts);if(counts[0]!=0||counts[1]!=2||Hash(vertices,sizeof(vertices))!=0xc8ce3c6e6db5b0beull||Hash(faces,sizeof(faces))!=0xf44208a16a617ed8ull)return false;
 S3DMat m={};im->GetMaterial(0,&m);if(m.texture!=0||Hash(&m.matdesc.diffuse,17*sizeof(float))!=0x2d6e7c4f0d54f390ull)return false;
 S3DTex t={};im->GetTexture(0,&t);const auto&p=t.desc.pixelFormat;return t.numframes==1&&t.desc.width==8&&t.desc.height==8&&p.dwRGBBitCount==16&&p.dwRBitMask==0xf00&&p.dwGBitMask==0xf0&&p.dwBBitMask==0xf&&p.dwRGBAlphaBitMask==0xf000;
}
}
