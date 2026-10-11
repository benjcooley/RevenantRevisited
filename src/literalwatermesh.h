#pragma once
#include "3dimage.h"
#include "renderer.h"
#include <cctype>
#include <cstring>
#include <string>
namespace literal_water_mesh {
inline uint64_t Hash(const void*p,size_t n){uint64_t h=1469598103934665603ull;const auto*b=static_cast<const uint8_t*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline bool Matches(T3DImagery*im){
 if(!im||!im->GetResFilename()||im->NumObjects()!=1||im->NumMaterials()!=1||im->NumTextures()!=1||im->NumStates()!=1||im->GetAniLength(0)!=16||im->NumTags()!=0||im->NumObjVerts(0)!=4||im->NumObjFaces(0)!=2)return false;
 std::string path(im->GetResFilename());for(char&c:path)c=c=='\\'?'/' : char(std::tolower(static_cast<unsigned char>(c)));const char*suffix="misc/water.i3d";const size_t n=std::strlen(suffix);if(path.size()<n||path.compare(path.size()-n,n,suffix)||(path.size()!=n&&path[path.size()-n-1]!='/'))return false;
 S3DObj o={};im->GetObject(0,&o);if(std::strcmp(o.name,"box01")||o.material!=0||o.parent[0]!=-1||im->IsHidden(0,0))return false;
 S3DVertex vertices[4]={};S3DFace faces[2]={};int starts[MAXTEXTURES+1]={},counts[MAXTEXTURES+1]={};im->GetObjVerts(0,vertices,0,0);im->GetObjFaces(0,faces,starts,counts);if(counts[0]!=0||counts[1]!=2||Hash(vertices,sizeof(vertices))!=0x7496795e49964853ull||Hash(faces,sizeof(faces))!=0xa91d8d6a3ebc3558ull)return false;
 S3DMat m={};im->GetMaterial(0,&m);if(m.texture!=0||Hash(&m.matdesc.diffuse,17*sizeof(float))!=0x2d6e7c4f0d54f390ull)return false;
 S3DTex t={};im->GetTexture(0,&t);const auto&p=t.desc.pixelFormat;return t.numframes==1&&t.desc.width==64&&t.desc.height==64&&p.dwRGBBitCount==16&&p.dwRBitMask==0xf800&&p.dwGBitMask==0x7e0&&p.dwBBitMask==0x1f&&p.dwRGBAlphaBitMask==0;
}
inline MeshHandle Cache(TRenderer&renderer,T3DImagery*im,TTextureHandle texture){
 if(!Matches(im)||texture==kInvalidTexture)return 0;S3DVertex authored[4]={};im->GetObjVerts(0,authored,0,0);SMeshVertex vertices[4]={};for(int i=0;i<4;++i){std::memcpy(vertices[i].pos,&authored[i].pos,3*sizeof(float));std::memcpy(vertices[i].normal,&authored[i].normal,3*sizeof(float));vertices[i].uv[0]=authored[i].tu;vertices[i].uv[1]=authored[i].tv;}const uint16_t indices[]={1,0,2,2,3,1};auto mesh=renderer.RegisterMeshAsset(0x4c57415400000000ull^uint32_t(im->ImageryId()),vertices,4,indices,6,texture);if(mesh)renderer.AddMeshAssetRef(mesh);return mesh;
}
inline void Submit(TRenderer&renderer,MeshHandle mesh,const hmm_mat4&world,bool native_domain){
 SHelperMeshSubmit item={};item.mesh=mesh;item.additive_blend=true;item.retail_lighting=1;item.retail_software_projection=native_domain;item.retail_normal_z_scale=native_domain?1.0f:1.0f/WORLD3D_Z_SCALE;for(int row=0;row<4;++row)for(int col=0;col<4;++col)item.world[row*4+col]=world.Elements[col][row];renderer.SubmitHelperMesh(item);
}
}
