#!/usr/bin/env python3
"""Compile the actual quad queue policy; software alpha must preserve fallback."""
from pathlib import Path
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]
source=(ROOT/'src/renderer.cpp').read_text()
start=source.index('    SQuadDrawItem queued = item;',source.index('void TRenderer::SubmitFxQuad('))
end=source.index('\n}',start)
body=source[start:end]
driver=r'''
#include <cassert>
#include <cstdint>
#include <vector>
enum class EFxBlend:uint8_t {Alpha=0,AdditiveStraight=1};
enum class EFxPipeline:uint16_t {Strip=3};
constexpr int kInvalidTexture=0;
struct SQuadDrawItem {
 bool retail_argb4444=false;int retail_texture=0;
 struct {int texture=1;uint8_t blend=1,depth_mode=2;uint16_t pipeline_id=0;} key;
};
struct Renderer {
 bool software=false;int white_texture=2;
 std::vector<SQuadDrawItem> fx_quad_queue;
 bool UsesRetailSoftwareMeshLighting()const{return software;}
 void Submit(const SQuadDrawItem&item){BODY}
};
int main(){
 for(bool software:{false,true})for(bool argb:{false,true})for(int fallback:{0,1}) {
  Renderer renderer;renderer.software=software;
  SQuadDrawItem source;source.retail_texture=fallback;source.retail_argb4444=argb;
  renderer.Submit(source);
  const auto&q=renderer.fx_quad_queue.at(0);
  assert(q.retail_texture==(software&&argb?3:fallback));
  assert(q.key.blend==(software&&argb?0:1));
  assert(q.key.depth_mode==source.key.depth_mode&&q.key.texture==source.key.texture);
  assert(source.retail_texture==fallback&&source.key.blend==1);
 }
}
'''.replace('BODY',body)
with tempfile.TemporaryDirectory() as directory:
    path=Path(directory);(path/'test.cpp').write_text(driver)
    subprocess.run(['clang++','-std=c++17',str(path/'test.cpp'),'-o',str(path/'test')],check=True)
    subprocess.run([str(path/'test')],check=True)
print('PASS actual quad queue source/modern alpha policy (8 cases)')
