#!/usr/bin/env python3
"""Compare compiled current Heal initialization/Advance with executed original state."""
import argparse,json,subprocess
from pathlib import Path
from fizzle_probe import ROOT,span,sha,f32

def run(reference,output):
 output.mkdir(parents=True,exist_ok=False);n=json.loads((reference/'manifest.json').read_text());s=(ROOT/'src/game/effects/effect.cpp').read_text();h=(ROOT/'src/game/effects/effect.h').read_text()
 types=span(h,'class THealEffect_Bespoke :','_CLASSDEF(TMistEffect_Bespoke)').replace('  private:','  public:')
 spawn=span(s,'THealEffect_Bespoke* THealEffect_Bespoke::SpawnForTest_BESPOKE','void THealEffect_Bespoke::Advance')
 init=span(spawn,'    heal->heal_num_','    log_info(')
 advance=span(s,'void THealEffect_Bespoke::Advance','THealEffect_Bespoke::~THealEffect_Bespoke')
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
using MeshHandle=uint32_t;struct SMeshVertex{float p[8];};struct S3DMat{};
struct TObjectImagery{};struct SObjectState{};enum class EFxDebugMode{Normal};
struct TEffect{TEffect(TObjectImagery*){};TEffect(SObjectState*,TObjectImagery*){};virtual ~TEffect()=default;virtual void OffScreen(){};void KillThisEffect(){}};
std::ifstream input;int draws=0;
int random(int a,int b){int x,y,v;if(!(input>>x>>y>>v)||a!=x||b!=y)throw std::runtime_error("Heal RNG range/order differs");++draws;return v;}
'''
 main=r'''
THealEffect_Bespoke::~THealEffect_Bespoke()=default;
int main(int argc,char**argv){input.open(argv[1]);THealEffect_Bespoke effect(nullptr);auto*heal=&effect;
constexpr auto kHealBubbles=THealEffect_Bespoke::kHealBubbles;constexpr auto kHealingRadius=THealEffect_Bespoke::kHealingRadius;
INITIALIZE
for(int tick=0;tick<=90;++tick){if(tick)effect.Advance(1.0/24.0);
printf("S %d %d %d %.9g %d\n",tick,int(effect.alive_),effect.activebubbles_,effect.rotation_,draws);
for(int i=0;i<60;++i)printf("P %d %d %.9g %.9g %.9g %.9g %.9g %d\n",tick,i,effect.p_[i].X,effect.p_[i].Y,effect.p_[i].Z,effect.scale_[i],effect.rise_[i],effect.framenum_[i]);}
int a,b,v;if(input>>a>>b>>v)throw std::runtime_error("Native RNG not fully consumed");}
'''.replace('INITIALIZE',init)
 generated=output/'heal-state.cpp';generated.write_text(prelude+types+advance+main);binary=output/'heal-state';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(generated),'-o',str(binary)]
 r=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError('Compile failed')
 r=subprocess.run([str(binary),str(reference/'native-random-inputs.txt')],capture_output=True,text=True);(output/'trace.txt').write_text(r.stdout);(output/'run.log').write_text(r.stderr)
 if r.returncode:raise RuntimeError('Trace failed')
 errors=[];fields=0
 for line in r.stdout.splitlines():
  v=line.split();tick=int(v[1]);e=n['cases'][tick]['state']
  if v[0]=='S':
   if (bool(int(v[2])),int(v[3]),f32(float(v[4])),int(v[5]))!=(e['alive'],e['active'],f32(e['rotation']),e['rng_calls']):errors.append([tick,'owner'])
  else:
   i=int(v[2]);wanted=e['positions'][i]+[e['scales'][i],e['rise'][i]]
   for j,(x,y)in enumerate(zip(map(float,v[3:8]),wanted)):
    fields+=1
    if f32(x)!=f32(y):errors.append([tick,i,j,x,y])
   if int(v[8])!=e['frames'][i]:errors.append([tick,i,'frame'])
 report=dict(status='pass'if not errors else'fail',errors=errors,exact_float_fields=fields,ticks=91,
    original_rng_calls=n['cases'][-1]['state']['rng_calls'],source_spans_sha256={k:sha(v.encode())for k,v in dict(types=types,init=init,advance=advance).items()},reference_sha256=sha((reference/'manifest.json').read_bytes()),scope='Actual current initialization statements and Advance compiled unchanged; real original range outputs replayed with strict order/range guard; complete60-slot state every tick. Asset loading, owner, renderer separate.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,'fields',len(errors),'errors')
 if errors:print(errors[:10]);raise SystemExit(1)

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.reference,a.output)
