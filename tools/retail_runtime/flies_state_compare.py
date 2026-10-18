#!/usr/bin/env python3
"""Full original Flies pool/RNG versus extracted actual production methods."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import function,sha
from flies_visual_reference import ROOT,read_mesh
PRELUDE=r'''
#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
enum class EFxDebugMode:uint8_t{Normal};enum class EFxBlend:uint8_t{Alpha};enum class EFxDepthMode:uint8_t{TestNoWrite};enum class EFxLightMode:uint8_t{Unlit};constexpr uint32_t OF_KILL=0x1000;constexpr int OBJCLASS_EFFECT=25;
struct SQuadDrawItem{float world_pos[4][3]{},uv[4][2]{};struct{TTextureHandle texture{};uint8_t blend{},depth_mode{};}key;EFxLightMode light_mode{};EFxDebugMode debug_mode{};bool retail_argb4444=false,retail_software_projection=false;};
struct TRenderer{std::vector<SQuadDrawItem>draws;void SubmitFxQuad(const SQuadDrawItem&q){draws.push_back(q);}}renderer;TRenderer*Renderer=&renderer;
struct TObjectImagery{virtual~TObjectImagery()=default;};struct SObjectDef{};struct S3DTex{TTextureHandle htexture=1;};S3DVertex fixture_vertices[4];
struct T3DImagery:TObjectImagery{void GetTexture(int,S3DTex*){}void GetObjVerts(int,S3DVertex*v,int,int){memcpy(v,fixture_vertices,sizeof(fixture_vertices));}}imagery;
namespace retail_flies{constexpr uint32_t Id=0xa21d9d00;bool ready=true;bool Matches(T3DImagery*){return ready;}}
struct TObjectInstance{virtual~TObjectInstance()=default;virtual void OffScreen(){}};struct TEffect:TObjectInstance{hmm_mat4 world{};uint32_t flags=0;TEffect(TObjectImagery*){MtxClear(&world);}TEffect(SObjectDef*,TObjectImagery*){MtxClear(&world);}uint32_t Flags()const{return flags;}int ObjClass()const{return 25;}uint32_t ObjId()const{return retail_flies::Id;}int GetMapIndex()const{return 1;}TObjectImagery*GetImagery()const{return&imagery;}void SetCommandDone(bool){}void KillThisEffect(){flags|=OF_KILL;}struct TransformType{const hmm_mat4&world;const hmm_mat4&Matrix()const{return world;}};TransformType Transform()const{return{world};}template<class T>void AddComponent(std::unique_ptr<T>){} };
template<class T>struct TWaterReferenceComponent{void Configure(TTextureHandle,int,int,int,int,int,float,float,bool,bool){}};
struct TTime{static constexpr double LegacyFrameSeconds=1.0/24.0;static double DeltaTime(){return LegacyFrameSeconds;}};void log_info(const char*,...){}
std::ifstream rng;int rng_count=0;int random(int low,int high){int l,h,v;if(!(rng>>l>>h>>v)||l!=low||h!=high)throw std::runtime_error("Original/source Flies range/order differs");++rng_count;return v;}
'''
MAIN=r'''
int main(int argc,char**argv){assert(argc==2||argc==3);rng.open(argv[1]);TFlies effect(nullptr);
retail_flies::ready=false;effect.Initialize(false);assert(!effect.initialized_&&rng_count==0);retail_flies::ready=true;effect.Initialize(false);assert(rng_count==60);effect.Initialize(false);assert(rng_count==60);
if(argc==3){int fps=atoi(argv[2]);effect.Advance(NAN);effect.Advance(INFINITY);effect.Advance(-1);effect.Advance(0);assert(effect.ticks_==0&&rng_count==60);for(int i=0;i<fps*3;++i)effect.Advance(1.0/fps);printf("clock %d %lld %d\n",fps,(long long)effect.ticks_,rng_count);return 0;}
for(int tick=0;tick<=90;++tick){if(tick)effect.Advance(1.0/24.0);renderer.draws.clear();const auto before=rng_count;effect.Submit(EFxDebugMode::Normal);assert(before==rng_count);printf("S %d %d %zu\n",tick,rng_count,renderer.draws.size());for(const auto&p:effect.flies_)printf("P %d %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %d\n",tick,p.pos.X,p.pos.Y,p.pos.Z,p.scl.X,p.scl.Y,p.scl.Z,p.rot.X,p.rot.Y,p.rot.Z,int(p.used));}
int l,h,v;assert(!(rng>>l>>h>>v));}
'''.replace('TFlies','TFlyEffect_Bespoke')
def run(native,output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/effect.cpp').read_text();header=(ROOT/'src/game/effects/effect.h').read_text();types=function(header,'class TFlyEffect_Bespoke :').replace('  private:','  public:')+';';particle=function(header,'struct SFlyParticleBespoke\n')+';';constants='\n'.join(l for l in header.splitlines()if l.startswith('inline constexpr ')and 'kFlyBespoke'in l).replace('FLY_COUNT','20').replace('FLY_RANGE_X','30').replace('FLY_RANGE_Y','30').replace('FLY_RANGE_Z','30');methods={s:function(source,s)for s in ['void TFlyEffect_Bespoke::Initialize(','void TFlyEffect_Bespoke::Advance(','void TFlyEffect_Bespoke::Submit(']}
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/flies.i3d'))
 asset=read_mesh(data);raw=bytes.fromhex(asset['vertex_hex']);initial=''.join('{'+'{'+','.join(format(x,'.9g')+'f'if '.'in format(x,'.9g')or'e'in format(x,'.9g')else format(x,'.9g')+'.0f'for x in v[:3])+'},{'+','.join(format(x,'.9g')+'f'if'.'in format(x,'.9g')or'e'in format(x,'.9g')else format(x,'.9g')+'.0f'for x in v[3:6])+'},'+','.join(format(x,'.9g')+'f'if'.'in format(x,'.9g')or'e'in format(x,'.9g')else format(x,'.9g')+'.0f'for x in v[6:])+'},'for v in struct.iter_unpack('<8f',raw));driver=output/'flies.cpp';driver.write_text(PRELUDE.replace('S3DVertex fixture_vertices[4];','S3DVertex fixture_vertices[4]={'+initial+'};')+'\n'+constants+'\n'+particle+'\n'+types+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'flies';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Flies production extraction compilation failed')
 q=subprocess.run([str(binary),str(native/'rng-inputs.txt')],capture_output=True,text=True);(output/'source-trace.txt').write_text(q.stdout);(output/'run.log').write_text(q.stderr)
 if q.returncode or q.stderr:raise RuntimeError('Flies source state failed')
 frames={}
 for line in q.stdout.splitlines():
  p=line.split();tick=int(p[1])
  if p[0]=='S':frames[tick]=dict(random_count=int(p[2]),draws=int(p[3]),particles=[])
  else:frames[tick]['particles'].append(dict(values=[float(x)for x in p[2:11]],used=int(p[11])))
 trace=json.loads((native/'state-trace.json').read_text());errors=[];checks=0
 for row in trace:
  actual=frames[row['tick']];checks+=2
  if row['random_count']!=actual['random_count']or actual['draws']!=sum(p['used']for p in row['particles']):errors.append(dict(tick=row['tick'],field='RNG/draw count'))
  for i,(p,q)in enumerate(zip(row['particles'],actual['particles'])):
   checks+=10
   if p['used']!=q['used']or any(struct.pack('<f',x)!=struct.pack('<f',y)for x,y in zip(p['values'],q['values'])):errors.append(dict(tick=row['tick'],particle=i))
 clocks=[]
 for fps in(6,24,30,60,144):
  q=subprocess.run([str(binary),str(native/'rng-inputs.txt'),str(fps)],capture_output=True,text=True);assert q.returncode==0 and q.stderr==''and q.stdout.strip()==f'clock {fps} 72 4452';clocks.append(dict(fps=fps,ticks=72,rng=4452))
 result=dict(status='pass'if not errors else'differences_found',accepted=False,errors=errors,checks=checks,frames=91,particles=20,clocks=clocks,sanitizers='ASan/UBSan',source_spans={s:sha(m.encode())for s,m in methods.items()},header_sha256=sha(header.encode()),driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),native_manifest_sha256=sha((native/'manifest.json').read_bytes()),scope='Actual full production Initialize/Advance/Submit compiled; all91×20 complete9float+used states bit-exact to full native, original ordered rangeRNG requests and drawcount align. Render consumesnoRNG; lazy retry/idempotent init and invalidelapsed/6,24,30,60,144fps clocks verified. Readonly geometry and profile provider/component installation are declared boundaries, real profile ASan and actualMetal/map separately verified.');(output/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(result['status'],checks);return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
