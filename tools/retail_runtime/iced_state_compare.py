#!/usr/bin/env python3
"""Compile unchanged current Iced state producer against the original owner/leaf trace."""
import argparse,json,subprocess
from pathlib import Path
from fizzle_probe import ROOT,span,sha,f32


def run(reference,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((reference/'manifest.json').read_text());s=(ROOT/'src/game/effects/effect.cpp').read_text();h=(ROOT/'src/game/effects/effect.h').read_text()
 constants=span(s,'constexpr float kIcedBouncePlane','constexpr float kIcedFacetSizeWu')
 types=span(h,'inline constexpr double kIcedSimTickMs','// *************************************************************************\n// * Magic family').replace('  private:','  public:').replace('_CLASSDEF(TIcedEffect_Bespoke)','')
 init=span(span(s,'TIcedEffect_Bespoke* TIcedEffect_Bespoke::SpawnForTest_BESPOKE','void TIcedEffect_Bespoke::Advance'),'    iced->frameon_=0;','    log_info(')
 advance=span(s,'void TIcedEffect_Bespoke::Advance','TIcedEffect_Bespoke::~TIcedEffect_Bespoke')
 prelude=r'''
#include <cmath>
#include <cstdio>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
#define MAX_ICED_CHUNKS 30
#define log_info(...) ((void)0)
using MeshHandle=uint32_t;struct SMeshVertex{float p[8];};struct S3DMat{};
struct TObjectImagery{};struct SObjectState{};enum class EFxDebugMode{Normal};
struct TEffect{TEffect(TObjectImagery*){};TEffect(SObjectState*,TObjectImagery*){};virtual ~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){}};
std::ifstream input;int draws=0;
int random(int a,int b){int x,y,v;if(!(input>>x>>y>>v)||a!=x||b!=y)throw std::runtime_error("Iced RNG range/order differs");++draws;return v;}
'''
 main=r'''
TIcedEffect_Bespoke::~TIcedEffect_Bespoke()=default;
int main(int argc,char**argv){input.open(argv[1]);TIcedEffect_Bespoke effect(nullptr);auto*iced=&effect;
INITIALIZE
for(int tick=0;tick<=420;++tick){if(tick)effect.Advance(1.0/24.0);
printf("S %d %d %d %.9g %d %d\n",tick,int(effect.alive_),effect.frameon_,effect.angle_,effect.donebouncing_,draws);
for(int i=0;i<30;++i){const auto&p=effect.chunks_[i];printf("P %d %d",tick,i);
for(auto v:{p.pos,p.vel,p.rot,p.ang,p.scl})printf(" %.9g %.9g %.9g",v.X,v.Y,v.Z);printf(" %d\n",p.life);}}
int a,b,v;if(input>>a>>b>>v)throw std::runtime_error("Native RNG not fully consumed");}
'''.replace('INITIALIZE',init)
 generated=output/'iced-state.cpp';generated.write_text("\n".join((prelude,constants,types,advance,main)));binary=output/'iced-state';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),'-o',str(binary)]
 r=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError('Compile failed')
 r=subprocess.run([str(binary),str(reference/'native-random-inputs.txt')],capture_output=True,text=True);(output/'trace.txt').write_text(r.stdout);(output/'run.log').write_text(r.stderr)
 if r.returncode:raise RuntimeError('Trace failed')
 errors=[];fields=0
 for line in r.stdout.splitlines():
  v=line.split();tick=int(v[1]);e=n['cases'][tick]['state']
  if v[0]=='S':
   if (bool(int(v[2])),int(v[3]),f32(float(v[4])),int(v[5]),int(v[6]))!=(e['alive'],e['frame'],f32(e['angle']),e['done'],e['rng_calls']):errors.append([tick,'owner',v[2:]])
  else:
   i=int(v[2]);wanted=sum([e[k][i]for k in ('positions','velocities','rotations','angular_velocity','scales')],[])
   for j,(x,y)in enumerate(zip(map(float,v[3:18]),wanted)):
    fields+=1
    if f32(x)!=f32(y):errors.append([tick,i,j,x,y])
   if int(v[18])!=e['life'][i]:errors.append([tick,i,'life'])
 report=dict(status='pass'if not errors else'fail',errors=errors,exact_float_fields=fields-len([e for e in errors if len(e)==5]),compared_float_fields=fields,ticks=421,
    original_rng_calls=n['cases'][-1]['state']['rng_calls'],source_spans_sha256={k:sha(v.encode())for k,v in dict(types=types,init=init,advance=advance,constants=constants).items()},reference_sha256=sha((reference/'manifest.json').read_bytes()),scope='Current initialization and Advance compiled unchanged; original range outputs replayed with strict range/order guards; all30chunk fields and ownerphase/lifetime per tick. Original owner uses explicit live victim; port standalone fixture models that phase contract. Mesh/renderer separate.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,'fields',len(errors),'errors')
 if errors:print(errors[:15]);raise SystemExit(1)

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.reference,a.output)
