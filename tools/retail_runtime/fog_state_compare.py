#!/usr/bin/env python3
"""Compile the production Fog state and compare every field to actual x86."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT
from fizzle_probe import sha


def compare(reference,output):
 output.mkdir(parents=True,exist_ok=False)
 native=json.loads((reference/'manifest.json').read_text());assert native['warm_replays']==2
 source=output/'fog-state.cpp'
 source.write_text(r'''
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include "effects/fog_grid.h"
int main(int argc,char**argv){
 std::ifstream f(argv[1]),g(argv[1]);int count=0;
 auto rng=[&](int a,int b){int x,y,z;if(!(f>>x>>y>>z)||a!=x||b!=y)throw std::runtime_error("rng order");++count;return z;};
 auto other=[&](int a,int b){int x,y,z;if(!(g>>x>>y>>z)||a!=x||b!=y)throw std::runtime_error("partition rng order");return z;};
 retail_fog::State state,partition;state.Initialize(rng);partition.Initialize(other);
 for(int t=0;t<=120;++t){
  if(t){state.Advance(1.0/24,rng);partition.Advance(1.0/48,other);partition.Advance(1.0/48,other);}
  if(std::memcmp(state.vertices.data(),partition.vertices.data(),sizeof(state.vertices)))return 4;
  printf("%d %d",t,count);
  for(auto&v:state.vertices)printf(" %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g",v.c,v.a,v.offset[0],v.offset[1],v.offset[2],v.velocity[0],v.velocity[1],v.velocity[2],v.cv,v.av);
  puts("");
 }
 int unused;if(f>>unused)return 5;
}
''')
 binary=output/'fog-state';command=['clang++','-std=c++17','-ffp-contract=off','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)]
 compiled=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(compiled.stdout+compiled.stderr);assert compiled.returncode==0
 result=subprocess.run([str(binary),str(reference/'native-random-inputs.txt')],capture_output=True,text=True);(output/'trace.txt').write_text(result.stdout);assert result.returncode==0,result.stderr
 f32=lambda x:struct.unpack('<f',struct.pack('<f',x))[0]
 fields=0
 for c,line in zip(native['cases'],result.stdout.splitlines()):
  tick,count,*values=map(float,line.split());assert tick==c['tick']and count==c['state']['rng_calls'];v=c['state'];expected=[]
  for i in range(36):expected+=v['colors'][2*i:2*i+2]+v['offsets'][3*i:3*i+3]+v['velocities'][3*i:3*i+3]+[v['color_velocity'][i],v['alpha_velocity'][i]]
  assert list(map(f32,values))==expected,f'Float mismatch at tick{tick}'
  fields+=len(expected)
 assert len(result.stdout.splitlines())==len(native['cases'])==121
 report=dict(status='pass',float_fields=fields,ticks=120,states=121,rng_calls=native['cases'][-1]['state']['rng_calls'],
  dt_partition='1/24 and two1/48 calls equal every tick',native_manifest_sha256=sha((reference/'manifest.json').read_bytes()),
  production_header_sha256=sha((ROOT/'src/effects/fog_grid.h').read_bytes()),compiled_driver_sha256=sha(source.read_bytes()),
  compiled_binary_sha256=sha(binary.read_bytes()),command=command,scope='Actual production State Initialize/Step/Advance; exact original RNG range/order/values. Rendering compared separately.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',fields,'fields',report['rng_calls'],'RNG calls')

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();compare(a.reference,a.output)
