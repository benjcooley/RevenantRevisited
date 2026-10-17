#!/usr/bin/env python3
"""Verify fixed native fixture and unreseeded game-provider Blood random streams."""
import argparse,json,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "bloodcore.h"
#include <cassert>
#include <cstdio>
#include <cstring>
uint32_t global_rng=2;unsigned calls=0;
int Live(int a,int b){assert(a<b);++calls;global_rng=global_rng*214013u+2531011u;return a+int((global_rng>>16)&32767u)%(b-a+1);}
void Equal(const blood_core::State&a,const blood_core::State&b){assert(std::memcmp(a.slots.data(),b.slots.data(),1080)==0);assert(a.done==b.done);}
int main(){blood_core::State fixture(2),live(999,{},4,Live);Equal(fixture,live);assert(global_rng==fixture.rng && live.rng==999 && calls==fixture.random_draws);for(int t=0;t<90;++t){fixture.Advance();live.Advance();Equal(fixture,live);assert(global_rng==fixture.rng && calls==fixture.random_draws);}assert(fixture.done && fixture.ticks==60);const uint32_t next_seed=global_rng;const unsigned before=calls;blood_core::State second(123,{},4,Live),control(next_seed);Equal(second,control);assert(global_rng==control.rng && calls==before+control.random_draws);const unsigned unchanged=calls;assert(second.Random(0,0)==0 && calls==unchanged);assert(second.Random(8,3)>=3 && calls==unchanged+1);assert(second.rng==123);std::printf("PASS %u %u %u\n",calls,before,next_seed);}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'rng.cpp';driver.write_text(MAIN);binary=output/'rng';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,calls,before,seed=q.stdout.split();r=dict(status='pass',accepted=False,provider_draws=int(calls),first_lifetime_draws=int(before),unreseeded_next_state_seed=int(seed),core_sha256=sha((ROOT/'src/bloodcore.h').read_bytes()),sanitizers='ASan/UBSan',scope='Fixed seed2 whole60-tick slots equal callback-provider stream; next spray begins at previous terminal global state. Private seed is untouched with provider; equal range skips RNG; reversed ranges normalize. Actual native controller/Metal appearance remain separately pinned.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(q.stdout.strip());return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
