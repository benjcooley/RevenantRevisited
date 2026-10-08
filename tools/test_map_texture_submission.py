#!/usr/bin/env python3
"""Compile actual per-instance texture selection and opaque renderer batching.

GPU/map animation is checked separately by map_texture_probe.py. This test
guards differently phased owners sharing immutable geometry in one gather.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def function(text, signature):
    start = text.index(signature)
    opening = text.index('{', start)
    depth, end = 1, opening + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


map_source = (ROOT/'src/maprenderer.cpp').read_text()
renderer_source = (ROOT/'src/renderer.cpp').read_text()
selection = function(map_source, 'static TTextureHandle MapMeshTexture(')
drain = function(renderer_source, 'void TRenderer::DrainMeshQueue()')
prelude = r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cstdio>
using TTextureHandle=uint32_t;constexpr uint32_t kInvalidTexture=0;
constexpr int MAXTEXTURES=8,OBJ3D_TEX=0x200000,OBJ3D_TEXFRAME=0x100000;
struct S3DAnimObj{int flags{};uint32_t htextures[8]{};int textureframe[9]{};};
struct S3DTex{int numframes=1;bool copyframes=false;uint32_t*framehtexs=nullptr;uint32_t htexture=0;};
struct T3DImagery{S3DTex texture;int NumTextures(){return 1;}void GetTexture(int,S3DTex*t){*t=texture;}};
struct SMeshSubmit{uint32_t mesh=1,texture_override=0;int retail_lighting=0;bool retail_positive_face_cull=false;};
struct Resource{uint32_t id=1;};using sg_buffer=Resource;
struct sg_range{const void*ptr;size_t size;};
struct sg_bindings{Resource vertex_buffers[2];int vertex_buffer_offsets[2]{};Resource index_buffer;uint32_t fs_images[1]{};};
struct SMeshEntry{Resource vbuf,ibuf;uint32_t albedo=77;int num_indices=6;};
constexpr int kMaxMeshInstances=100,kMeshInstanceFloats=28,SG_SHADERSTAGE_VS=0;
struct Draw{uint32_t texture;int instances;};std::vector<Draw>draws;uint32_t bound;
void sg_update_buffer(Resource,const sg_range*){}void sg_apply_pipeline(Resource){}
void sg_apply_uniforms(int,int,const sg_range*){}void sg_apply_bindings(const sg_bindings*b){bound=b->fs_images[0];}
void sg_draw(int,int,int n){draws.push_back({bound,n});}
void PackMeshInstanceRow(const SMeshSubmit&,float*,float){}
struct TRenderer{std::vector<SMeshSubmit>mesh_queue;std::vector<float>mesh_instance_scratch;
 Resource mesh_pipeline,mesh_source_cull_pipeline;bool retail_mesh_rgb_enabled=true;
 struct{int mode=0;}light;std::vector<SMeshEntry>meshes{SMeshEntry{}};
 Resource NextMeshInstanceBuffer(){return {};}void PackMeshVsUniforms(float(&)[20]){}
 uint32_t TextureImage(uint32_t t){return t;}void DrainMeshQueue();};
'''
checks = r'''
int main(){
 uint32_t frames[]{101,102,103,104,105,106};T3DImagery img;img.texture={6,false,frames,101};
 S3DAnimObj early,late;
 const auto a=MapMeshTexture(img,&early,0,1,1,88),b=MapMeshTexture(img,&late,0,1,3,88);
 assert(a==102&&b==104&&img.texture.htexture==101);
 assert(MapMeshTexture(img,&early,0,1,7,88)==102);
 early.flags=OBJ3D_TEXFRAME;early.textureframe[1]=5;assert(MapMeshTexture(img,&early,0,1,0,88)==106);
 early.flags|=OBJ3D_TEX;early.htextures[1]=999;assert(MapMeshTexture(img,&early,0,1,0,88)==999);
 early.htextures[1]=0;assert(MapMeshTexture(img,&early,0,1,0,88)==88);
 early.flags=0;assert(MapMeshTexture(img,&early,-1,0,0,88)==kInvalidTexture);
 assert(MapMeshTexture(img,nullptr,0,1,2,88)==103);
 TRenderer r;SMeshSubmit e,l,base;e.texture_override=a;l.texture_override=b;
 r.mesh_queue={l,e,base,e};r.DrainMeshQueue();
 assert(draws.size()==3);assert(draws[0].texture==77&&draws[0].instances==1);
 assert(draws[1].texture==102&&draws[1].instances==2);assert(draws[2].texture==104&&draws[2].instances==1);
 assert(r.meshes[0].albedo==77);draws.clear();l.retail_positive_face_cull=true;
 r.mesh_queue={e,l};r.DrainMeshQueue();assert(draws.size()==2);
 std::puts("PASS: actual per-owner frame/explicit overrides, immutable assets and texture-separated batches");
}
'''
with tempfile.TemporaryDirectory(prefix='revenant-map-texture-') as temp:
    source=Path(temp)/'test.cpp';binary=Path(temp)/'test'
    source.write_text(prelude+selection+'\n'+drain+'\n'+checks)
    subprocess.run(['clang++','-std=c++17',str(source),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
