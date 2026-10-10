#!/usr/bin/env python3
"""Compile actual map material tint, texture selection and renderer batching.

GPU/map animation is checked separately by map_texture_probe.py. This test
guards material color/alpha and differently phased owners sharing immutable
geometry in one gather.
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
renderer_header = (ROOT/'src/renderer.h').read_text()
lighting_policy = function(renderer_header, 'bool UsesRetailSoftwareMeshLighting()')
selection = function(map_source, 'static TTextureHandle MapMeshTexture(')
material = function(map_source, 'void LoadObjectMaterial(')
tint_start = map_source.index('            m.tint[0] = m.tint[1] = m.tint[2] = 1.0f;')
tint = map_source[tint_start:map_source.index('            m.obj_id = obj_id;', tint_start)]
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
struct Color{float r=0,g=0,b=0,a=1;};
struct S3DMat{struct{Color diffuse,ambient,specular,emissive;float power=0;}matdesc;};
struct S3DObj{int material=0;};
struct T3DImagery{S3DTex texture;S3DMat material;int textures=1,material_index=0;
 int NumTextures(){return textures;}void GetTexture(int,S3DTex*t){*t=texture;}
 int NumMaterials(){return 1;}void GetObject(int,S3DObj*o){o->material=material_index;}
 void GetMaterial(int,S3DMat*m){*m=material;}};
struct SMeshSubmit{uint32_t mesh=1,texture_override=0;float tint[4]{};int retail_lighting=0;bool retail_positive_face_cull=false;};
struct SHelperMeshSubmit{float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0;};
constexpr int OBJCLASS_EFFECT=7;
struct Owner{int kind;uint32_t id=0;int ObjClass()const{return kind;}uint32_t ObjId()const{return id;}};
struct Asset{int objnum=0;};
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
 RETAIL_POLICY
 uint32_t TextureImage(uint32_t t){return t;}void DrainMeshQueue();};
TRenderer tint_renderer;TRenderer*Renderer=&tint_renderer;
'''.replace('RETAIL_POLICY', lighting_policy)
checks = r'''
int main(){
 T3DImagery barrier;barrier.textures=0;barrier.material.matdesc.diffuse={1,0,0,0.25f};
 SMeshSubmit red;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,red);
 assert(red.tint[0]==1&&red.tint[1]==0&&red.tint[2]==0&&red.tint[3]==0.2f);
 for(uint32_t id:{0xad92bc10u,0xad92bc37u,0xad92bc38u}){
  SMeshSubmit source;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,source,id);
  assert(source.tint[0]==1&&source.tint[1]==1&&source.tint[2]==1&&source.tint[3]==0.8f);
 assert(source.retail_lighting==1);}
 tint_renderer.light.mode=1;SMeshSubmit modern;
 ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,modern,0xad92bc37u);
 assert(modern.tint[1]==0&&modern.tint[3]==0.2f&&modern.retail_lighting==0);
 tint_renderer.light.mode=0;tint_renderer.retail_mesh_rgb_enabled=false;SMeshSubmit positional;
 ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,positional,0xad92bc38u);
 assert(positional.tint[1]==0&&positional.tint[3]==0.2f&&positional.retail_lighting==0);
 tint_renderer.retail_mesh_rgb_enabled=true;
 SMeshSubmit unknown;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,unknown,0xad92bc11u);
 assert(unknown.tint[1]==0&&unknown.tint[3]==0.2f&&unknown.retail_lighting==0);
 SMeshSubmit non_effect;ApplyTint(&barrier,0,0.8f,non_effect,0xad92bc10u);
 assert(non_effect.tint[1]==1&&non_effect.retail_lighting==0);
 barrier.textures=1;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,red);
 assert(red.tint[0]==1&&red.tint[1]==1&&red.tint[2]==1&&red.tint[3]==0.8f);
 SMeshSubmit textured;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,textured,0xad92bc37u);
 assert(textured.tint[1]==1&&textured.tint[3]==0.8f&&textured.retail_lighting==0);
 barrier.textures=0;ApplyTint(&barrier,0,0.8f,red);assert(red.tint[1]==1&&red.tint[3]==0.8f);
 barrier.material_index=2;ApplyTint(&barrier,OBJCLASS_EFFECT,0.8f,red);
 assert(red.tint[0]==1&&red.tint[1]==1&&red.tint[2]==1&&red.tint[3]==0.8f);
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
 std::puts("PASS: actual material color/alpha/fallback, per-owner texture selection and immutable batches");
}
'''
with tempfile.TemporaryDirectory(prefix='revenant-map-texture-') as temp:
    source=Path(temp)/'test.cpp';binary=Path(temp)/'test'
    tint_function = '\nvoid ApplyTint(T3DImagery*meshimg,int kind,float draw_alpha,SMeshSubmit&m,uint32_t id=0){Owner owner{kind,id};Owner*oi=&owner;Asset asset;\n'+tint+'}\n'
    source.write_text(prelude+selection+'\n'+material+tint_function+drain+'\n'+checks)
    subprocess.run(['clang++','-std=c++17',str(source),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
