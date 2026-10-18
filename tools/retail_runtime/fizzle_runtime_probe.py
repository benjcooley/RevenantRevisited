#!/usr/bin/env python3
"""Compile the actual map Fizzle simulation and compare all live state/RNG calls."""
import argparse
import json
from pathlib import Path
import subprocess
from fizzle_probe import ROOT,span,sha,f32


def run(reference,output):
    output.mkdir(parents=True,exist_ok=False)
    original=json.loads((reference/'manifest.json').read_text())
    if original['status']!='pass':raise AssertionError('Original prerequisite failed')
    source=(ROOT/'src/game/effects/legacy/fizzle.cpp').read_text()
    body=span(source,'constexpr int32_t kDustCount','// =========================================================================\n// Render')
    prelude=r'''
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include "particlefx.h"
#include "math3d.h"
#undef min
#undef max
struct T3DImagery {};
std::ifstream inputs;int draws=0;
int random(int lo,int hi){int a,b,v;if(!(inputs>>a>>b>>v)||a!=lo||b!=hi)
 throw std::runtime_error("Original/map RNG range/order mismatch");++draws;return v;}
'''
    trailer=r'''
int main(int argc,char**argv){inputs.open(argv[1]);State s;
 for(int tick=0;tick<40;++tick){if(tick)Animate(&s);
  std::printf("S %d %d %d %.9g %d\n",tick,int(s.alive),s.frame_count,s.add,draws);
  for(int system=0;system<3;++system)for(int slot=0;slot<30;++slot){auto&p=s.systems[system].p[slot];
   std::printf("P %d %d %d %d %d",tick,system*30+slot,int(p.used),p.life_span,int(p.flicker));
   std::printf(" %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",
     p.pos[0],p.pos[1],p.pos[2],p.vel[0],p.vel[1],p.vel[2],p.scl,p.scl,p.scl,p.rot_z_deg,p.spin_rate,p.max_scl);
  }
 }
 int a,b,v;if(inputs>>a>>b>>v)throw std::runtime_error("Unconsumed original RNG");
}
'''
    generated=output/'map-fizzle.cpp';generated.write_text(prelude+body+trailer)
    binary=output/'map-fizzle';cmd=['clang++','-std=c++17','-I',str(ROOT/'thirdparty/handmademath'),
        '-iquote',str(ROOT/'src'),str(generated),'-o',str(binary)]
    result=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(result.stdout+result.stderr)
    if result.returncode:raise RuntimeError('Map simulation compilation failed')
    result=subprocess.run([str(binary),str(reference/'native-random-inputs.txt')],capture_output=True,text=True)
    (output/'trace.txt').write_text(result.stdout);(output/'run.log').write_text(result.stderr)
    if result.returncode:raise RuntimeError('Map simulation execution failed')
    cases={};errors=[];fields=0
    for line in result.stdout.splitlines():
        v=line.split();tick=int(v[1]);expected=original['cases'][tick]['original_state']
        if v[0]=='S':
            actual=(bool(int(v[2])),int(v[3]),f32(float(v[4])),int(v[5]))
            want=(expected['alive'],expected['frame_count'],f32(expected['add']),expected['random_count'])
            if actual!=want:errors.append([tick,'effect'])
            cases[tick]=dict(tick=tick,alive=actual[0],frame_count=actual[1],rng_draws=actual[3])
        else:
            p=expected['particles'][int(v[2])]
            if bool(int(v[3]))!=p['used']:errors.append([tick,v[2],'used'])
            if not p['used']:continue
            if (int(v[4]),bool(int(v[5])))!=(p['phase'],p['flicker']):errors.append([tick,v[2],'phase/flicker'])
            for i,(a,b)in enumerate(zip(map(float,v[6:]),p['values'])):
                fields+=1
                if f32(a)!=f32(b):errors.append([tick,v[2],i])
    report=dict(status='pass'if not errors else'fail',errors=errors,cases=list(cases.values()),
        exact_float_fields=fields,original_rng_calls=original['original_rng_calls'],
        source_body_sha256=sha(body.encode()),compiled_source_sha256=sha(generated.read_bytes()),
        reference_manifest_sha256=sha((reference/'manifest.json').read_bytes()),
        compiler_command=cmd,scope='Actual src/game/effects/legacy/fizzle.cpp constants/state/snap_random/SpawnParticle/Animate compiled unchanged. Original range outputs are common explicit inputs with strict call range/order validation; shared engine random() supplies original MSVC generator in live runtime. All used-particle state and whole birth/death cadence compared; renderer/map scene remain separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    if errors:raise AssertionError(str(errors[:12]))
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();r=run(a.reference,a.output);print('PASS',r['exact_float_fields'],'map particle fields,',r['original_rng_calls'],'original RNG calls')
