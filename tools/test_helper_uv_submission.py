#!/usr/bin/env python3
"""Compile actual helper emission: atlas UV uniforms, blend and immutable mesh."""
from pathlib import Path
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]


def function(source,signature):
    start=source.index(signature);opening=source.index('{',start);depth=1;end=opening+1
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]


source=(ROOT/'src/renderer.cpp').read_text()
header=(ROOT/'src/renderer.h').read_text()
record=header[header.index('struct SHelperMeshSubmit\n'):]
record=record[:record.index('\n};')+3]
emit=function(source,'void TRenderer::EmitTransparentHelper(')
prelude=r'''
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>
#include <cstdio>
#include "effects/warp_atlas.h"
using MeshHandle=uint32_t;using TTextureHandle=uint32_t;
constexpr uint32_t kInvalidTexture=0;
enum class EHelperMeshShade:uint8_t{Material=0,Texture=1,TextureLit=2};
struct Resource{uint32_t id=1;};using sg_pipeline=Resource;
struct sg_range{const void*ptr;size_t size;};
struct sg_bindings{Resource vertex_buffers[1],index_buffer;uint32_t fs_images[1]{};};
constexpr int SG_SHADERSTAGE_VS=0,SG_SHADERSTAGE_FS=1;
float vertex_uniforms[40]{};int draws=0;uint32_t texture=0,pipeline=0;
void sg_apply_pipeline(Resource r){pipeline=r.id;}
void sg_apply_bindings(const sg_bindings*b){texture=b->fs_images[0];}
void sg_apply_uniforms(int stage,int,const sg_range*r){
 if(stage==SG_SHADERSTAGE_VS){assert(r->size==sizeof(vertex_uniforms));std::memcpy(vertex_uniforms,r->ptr,r->size);}}
void sg_draw(int,int count,int instances){assert(count==9&&instances==1);++draws;}
'''
context=r'''
struct TRenderer{
 sg_pipeline helper_mesh_back_pipeline{1},helper_mesh_front_pipeline{2},
 helper_mesh_add_back_pipeline{3},helper_mesh_add_front_pipeline{4},
 helper_gold_no_depth_back_pipeline{5},helper_gold_no_depth_front_pipeline{6};
 struct SMeshEntry{Resource vbuf,ibuf;uint32_t albedo=77;int num_indices=9;};
 std::vector<SMeshEntry>meshes{SMeshEntry{}};
 struct{float ox=320,oy=170,z_near=0,zspan=1024,kcam_forward=1925,reserved=0,
 center_wx=0,center_wy=0,zoom=1;}recon;
 struct{float dir[3]{0,0,1},intensity=1,color[3]{1,1,1},ambient=1;}light;
 int width=640,height=340;static constexpr int kGBufPad=16;
 float retail_mesh_ambient[3]{1,1,1},retail_mesh_directional[3]{};
 uint32_t TextureImage(uint32_t handle){return handle;}
 void EmitTransparentHelper(const SHelperMeshSubmit&);
};
int main(){
 TRenderer renderer;SHelperMeshSubmit draw;draw.mesh=1;draw.world[0]=draw.world[5]=draw.world[10]=draw.world[15]=1;
 renderer.EmitTransparentHelper(draw);assert(vertex_uniforms[36]==0&&vertex_uniforms[37]==0&&draws==2);
 retail_warp::State warp;warp.Step();retail_warp::ConfigureDraw(draw,warp);draw.texture_override=88;
 draws=0;renderer.EmitTransparentHelper(draw);
 assert(vertex_uniforms[36]==.25f&&vertex_uniforms[37]==0&&draws==2);
 assert(texture==88&&pipeline==2&&renderer.meshes[0].albedo==77);
 for(int i=1;i<4;++i)warp.Step();retail_warp::ConfigureDraw(draw,warp);
 renderer.EmitTransparentHelper(draw);assert(vertex_uniforms[36]==0&&vertex_uniforms[37]==1);
 assert(vertex_uniforms[35]==2); // mode2 selects wrapped-nearest unlit alpha
 std::puts("PASS: actual helper UV uniform ABI, alpha path, all9 indices and immutable albedo");
}
'''
for snippet in ('o.uv = in.uv + p.uv_shift.xy','v_uv = uv + uv_shift.xy','o.uv = i.uv + uv_shift.xy',
                'hsh.vs.uniform_blocks[0].size = 10 * sizeof(float) * 4;'):
    assert snippet in source,snippet
with tempfile.TemporaryDirectory(prefix='revenant-helper-uv-')as temp:
    cpp=Path(temp)/'test.cpp';binary=Path(temp)/'test'
    cpp.write_text(prelude+record+context[:context.index('int main()')]+emit+'\n'+context[context.index('int main()'):])
    subprocess.run(['clang++','-std=c++17','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
