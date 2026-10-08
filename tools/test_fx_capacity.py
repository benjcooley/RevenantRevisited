#!/usr/bin/env python3
"""Compile actual strip/quad packing; verify capacity overflow keeps valid spans.

This checks CPU packing and its upload gate, not GPU output.
"""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source=(root/'src/renderer.cpp').read_text()
start=source.index('        std::vector<float> scratch;',source.index('    // ---- Strips',source.index('void TRenderer::DrainFxQueue()')))
end=source.index('        if (!scratch.empty() && int32_t(scratch.size() / kFxStripVertexFloats) <= kMaxFxStripVerts)',start)
packing=source[start:end]
upload_condition=source[end:source.index('\n',end)].strip()
prelude=r'''
#include <cassert>
#include <cstdint>
#include <vector>
#include <cstdio>
#include <utility>
enum class EFxDebugMode:uint8_t{Normal};enum class EFxLightMode:uint8_t{Unlit};
struct SFxBatchKey{uint16_t pipeline_id{};uint8_t blend{},depth_mode{};uint32_t texture{};};
struct SStripSegment{float world_a[3]{},world_b[3]{1,0,0};float width_a_wu=1,width_b_wu=1;
 float color_a[4]{1,1,1,1},color_b[4]{1,1,1,1};float u_a=0,u_b=1,v_left=0,v_right=1;bool uv_swapped=false;};
struct Entry{std::vector<SStripSegment>segments;SFxBatchKey key{};EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{},color_rgba[4]{1,1,1,1};SFxBatchKey key{};
 int corner_count=4,retail_texture=0;EFxDebugMode debug_mode{};EFxLightMode light_mode{};};
constexpr int kFxStripVertexFloats=16,kMaxFxStripVerts=8192;
void log_warn(const char*,...){}
struct Result{int vertices,spans,drawn,dropped;};
'''
body=r'''
Result Pack(const std::vector<Entry>&fx_strip_queue,const std::vector<SQuadDrawItem>&fx_quad_queue){
PACKING
 int drawn=0;
UPLOAD {for(const auto&s:spans){assert(s.first>=0&&s.first+s.count<=int(scratch.size()/kFxStripVertexFloats));drawn+=s.count;}}
 return {int(scratch.size()/kFxStripVertexFloats),int(spans.size()),drawn,dropped_capacity_items};
}
Entry Strip(int n){Entry e;e.segments.resize(n);return e;}
SQuadDrawItem Tri(){SQuadDrawItem q;q.corner_count=3;return q;}
int main(){
 // One excess quad must not erase the 1365 valid ones ahead of it.
 auto full=Pack({},std::vector<SQuadDrawItem>(1366));assert(full.vertices==8190&&full.drawn==8190&&full.spans==1365&&full.dropped==1);
 // Mix real strip packing with quad/triangle packing. A later triangle can fit
 // after a six-vertex item is rejected; accepted offsets must stay contiguous.
 std::vector<SQuadDrawItem> mixed(1354);mixed.push_back(Tri());mixed.push_back({});mixed.push_back(Tri());
 auto mix=Pack({Strip(10)},mixed);assert(mix.vertices==8190&&mix.drawn==8190&&mix.spans==1357&&mix.dropped==1);
 // An item too large even for an empty buffer cannot suppress smaller items.
 auto huge=Pack({Strip(2000),Strip(1)},std::vector<SQuadDrawItem>(1));assert(huge.vertices==12&&huge.drawn==12&&huge.spans==2&&huge.dropped==1);
 // Under-cap normal packing and a new gather retain all submissions.
 auto small=Pack({Strip(2)},std::vector<SQuadDrawItem>(3));assert(small.vertices==30&&small.drawn==30&&small.spans==4&&small.dropped==0);
 auto again=Pack({},std::vector<SQuadDrawItem>(1));assert(again.drawn==6&&again.dropped==0);
}
'''
strip_guard='''            if (n > (kMaxFxStripVerts - first_v) / 6)
            {
                ++dropped_capacity_items;
                continue;
            }
'''
quad_guard='''            if (ds.count > kMaxFxStripVerts - ds.first)
            {
                ++dropped_capacity_items;
                continue;
            }
'''
assert strip_guard in packing and quad_guard in packing
with tempfile.TemporaryDirectory(prefix='revenant-fx-capacity-')as tmp:
 path=Path(tmp)/'regression.cpp';binary=Path(tmp)/'regression'
 def compile_run(packing_text):
  path.write_text(prelude+body.replace('PACKING',packing_text).replace('UPLOAD',upload_condition))
  subprocess.run(['clang++','-std=c++17',str(path),'-o',str(binary)],check=True)
  return subprocess.run([str(binary)],capture_output=True)
 fixed=compile_run(packing);assert fixed.returncode==0,fixed.stderr.decode()
 before=compile_run(packing.replace(strip_guard,'').replace(quad_guard,''));assert before.returncode!=0,'Pre-fix all-frame overflow unexpectedly passed'
print('PASS: actual strip/quad packing preserves accepted spans at8192, admits smaller later items, rejects oversized items, resets next gather; pre-fix fails.')
