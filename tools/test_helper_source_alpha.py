#!/usr/bin/env python3
"""Compile actual helper pipeline creation: existing culls and source mode2 state."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1];source=(root/'src/renderer.cpp').read_text();start=source.index('    hpip.index_type     = SG_INDEXTYPE_UINT16;');end=source.index('    // Gold',start);body=source[start:end]
prelude=r'''
#include <cassert>
#include <vector>
#include <cstdio>
enum {SG_INDEXTYPE_UINT16,SG_PRIMITIVETYPE_TRIANGLES,SG_CULLMODE_FRONT,SG_CULLMODE_BACK,SG_CULLMODE_NONE,SG_PIXELFORMAT_RGBA8,SG_PIXELFORMAT_DEPTH,SG_BLENDFACTOR_SRC_ALPHA,SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,SG_BLENDFACTOR_ONE,SG_COMPAREFUNC_LESS_EQUAL};
struct Descriptor {int index_type,primitive_type,cull_mode,color_count;struct{int pixel_format;struct{bool enabled;int src_factor_rgb,dst_factor_rgb,src_factor_alpha,dst_factor_alpha;}blend;}colors[1];struct{int pixel_format,compare;bool write_enabled;}depth;const char*label;};
std::vector<Descriptor> made;int sg_make_pipeline(Descriptor*d){made.push_back(*d);return int(made.size());}
int main(){Descriptor hpip{};int helper_mesh_back_pipeline,helper_mesh_front_pipeline,helper_source_alpha_pipeline,helper_untextured_software_pipeline,helper_mesh_add_back_pipeline,helper_mesh_add_front_pipeline;
'''
post=r'''
assert(made.size()==6&&helper_mesh_back_pipeline==1&&helper_mesh_front_pipeline==2&&helper_source_alpha_pipeline==3);
assert(made[0].cull_mode==SG_CULLMODE_FRONT&&made[1].cull_mode==SG_CULLMODE_BACK&&made[2].cull_mode==SG_CULLMODE_NONE);
assert(made[3].cull_mode==SG_CULLMODE_NONE&&made[3].depth.write_enabled);
for(int i=0;i<4;++i){auto& p=made[i];assert(p.depth.compare==SG_COMPAREFUNC_LESS_EQUAL&&p.depth.write_enabled==(i==3));assert(p.colors[0].blend.enabled);assert(p.colors[0].blend.src_factor_rgb==SG_BLENDFACTOR_SRC_ALPHA&&p.colors[0].blend.dst_factor_rgb==SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA);}
assert(made[4].cull_mode==SG_CULLMODE_FRONT&&made[5].cull_mode==SG_CULLMODE_BACK);
for(int i=4;i<6;++i){assert(!made[i].depth.write_enabled&&made[i].depth.compare==SG_COMPAREFUNC_LESS_EQUAL);assert(made[i].colors[0].blend.src_factor_rgb==SG_BLENDFACTOR_ONE&&made[i].colors[0].blend.dst_factor_rgb==SG_BLENDFACTOR_ONE);}
std::puts("PASS: actual default helper pipelines preserved; source-alpha CULL_NONE/no-write and untextured kernel depth-write pipelines");}
'''
with tempfile.TemporaryDirectory(prefix='helper-source-alpha-')as t:
 cpp=Path(t)/'test.cpp';binary=Path(t)/'test';cpp.write_text(prelude+body+post);subprocess.run(['clang++','-std=c++17',str(cpp),'-o',str(binary)],check=True);subprocess.run([str(binary)],check=True)
