"""Compile actual quad admission and all three untextured fragment policies."""
import json,subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import function
ROOT=Path(__file__).resolve().parents[2]
class PrelitQuad(unittest.TestCase):
 def test_explicit_format_and_default_admissions(self):
  source=(ROOT/'src/renderer.cpp').read_text();h=(ROOT/'src/renderer.h').read_text();a=h.index('struct SQuadDrawItem\n');record=h[a:h.index('\n};',a)+3];body=function(source,'void TRenderer::SubmitFxQuad(')
  pre=r'''
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>
using TTextureHandle=unsigned;constexpr unsigned kInvalidTexture=0;bool g_fx_wireframe_override=false;
enum class EFxDebugMode{Normal,SolidColor,FullTexture,CurrentFrame,Wireframe};enum class EFxLightMode{Unlit,LitFlat};enum class EFxBlend{Alpha,AdditiveStraight};enum class EFxDepthMode{None,TestNoWrite,TestWrite};enum class EFxPipeline{Strip};
struct SFxBatchKey{unsigned texture=1;uint8_t blend=0,depth_mode=1;uint16_t pipeline_id=0;};
struct SStripSegment{float world_a[3],world_b[3],width_a_wu,width_b_wu,color_a[4],color_b[4];};struct SStripDrawItem{SStripSegment*segments;int num_segments;SFxBatchKey key;EFxDebugMode debug_mode;};
'''
  context=r'''
struct TRenderer{bool software=true;unsigned white_texture=1;std::vector<SQuadDrawItem>fx_quad_queue;bool UsesRetailSoftwareMeshLighting()const{return software;}void SubmitFxStrip(const SStripDrawItem&){}void SubmitFxQuad(const SQuadDrawItem&);};
'''
  main=r'''
int main(){TRenderer r;SQuadDrawItem q{};assert(!q.retail_prelit_rgba5);q.color_rgba[0]=22;q.color_rgba[3]=3;r.SubmitFxQuad(q);assert(r.fx_quad_queue.back().retail_texture==0&&r.fx_quad_queue.back().key.depth_mode==1);q.retail_prelit_rgba5=true;for(bool sw:{true,false}){r.software=sw;r.SubmitFxQuad(q);auto p=r.fx_quad_queue.back();assert(p.retail_texture==5&&p.key.blend==uint8_t(EFxBlend::Alpha)&&p.key.depth_mode==uint8_t(EFxDepthMode::TestWrite)&&p.color_rgba[0]==22&&p.color_rgba[3]==3);}q.retail_prelit_rgba5=false;q.retail_argb4444=true;r.software=true;r.SubmitFxQuad(q);assert(r.fx_quad_queue.back().retail_texture==3);q.retail_argb4444=false;q.retail_rgb565_additive=true;q.key.blend=uint8_t(EFxBlend::AdditiveStraight);r.SubmitFxQuad(q);assert(r.fx_quad_queue.back().retail_texture==4);r.software=false;r.SubmitFxQuad(q);assert(r.fx_quad_queue.back().retail_texture==0);}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text(pre+record+context+body+main);subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
 def test_three_actual_shader_blocks_match(self):
  functions=[]
  for backend in('metal','glsl','hlsl'):
   s=(ROOT/f'src/shaders/fx.{backend}.h').read_text();a=s.index('// Original dynamic untextured RGBA5:');body=s[a:s.index('\n    }',a)];body=body.replace('in.color','color').replace('i.color','color').replace('v_color','color').replace('o_color = vec4(source, alpha); return;','return vec4(source, alpha);').replace('vec3','float3').replace('vec4','float4').replace('float3(','make3(').replace('float4(','make4(');functions.append(f'float4 {backend}(float4 color){{'+body+'}')
  pre=r'''
#include <cmath>
#include <cassert>
typedef float float3 __attribute__((ext_vector_type(3)));typedef float float4 __attribute__((ext_vector_type(4)));
float3 make3(float x,float y,float z){return {x,y,z};}float4 make4(float3 v,float a){return{v.x,v.y,v.z,a};}float clamp(float v,float a,float b){return std::fmin(b,std::fmax(a,v));}float3 clamp(float3 v,float a,float b){return{clamp(v.x,a,b),clamp(v.y,a,b),clamp(v.z,a,b)};}float3 floor(float3 v){return{std::floor(v.x),std::floor(v.y),std::floor(v.z)};}
'''
  main=r'''
int main(){for(int a=0;a<32;++a)for(int r=0;r<32;++r){float4 color={float(r)+.75f,float((r*7)%32)+.5f,float((r*13)%32)+.25f,float(a)+.5f};auto m=metal(color),g=glsl(color),h=hlsl(color);for(int c=0;c<4;++c)assert(m[c]==g[c]&&m[c]==h[c]);for(int c=0;c<3;++c){float max=c==1?63.f:31.f;int src=int(color[c])*(c==1?2:1);int expected=src*(a+1)/32;assert(std::lround(m[c]*m.a*max)==expected);}}}
'''
  with tempfile.TemporaryDirectory()as tmp:
   p=Path(tmp);(p/'test.cpp').write_text(pre+'\n'.join(functions)+main);subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
if __name__=='__main__':unittest.main()
