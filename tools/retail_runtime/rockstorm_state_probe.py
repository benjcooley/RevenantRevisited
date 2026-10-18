"""Compare the complete production level-one RockStorm pool with original bytes."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser();p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);native=json.loads(a.trace.read_text());rows=[native['initial']]+[r['state']for r in native['records']]
s=r'''
#include <cstdio>
#include "rockstormcore.h"
uint32_t seed=1,draws=0;int random_range(int lo,int hi){++draws;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
int main(){rockstorm_core::State s;s.Initialize(random_range);for(int i=0;i<TICKS;++i){if(i)s.Pulse();fwrite(&draws,4,1,stdout);fwrite(&seed,4,1,stdout);fwrite(&s.owner_stage,4,1,stdout);int alive=s.alive,events=s.damage_events;fwrite(&alive,4,1,stdout);fwrite(&events,4,1,stdout);fwrite(&s.pool,sizeof(s.pool),1,stdout);}}
'''.replace('TICKS',str(len(rows)))
f=a.output/'core.cpp';f.write_text(s);b=a.output/'core';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-iquote',str(ROOT/'src'),str(f),'-o',str(b)],check=True);raw=subprocess.check_output([str(b)]);(a.output/'source.bin').write_bytes(raw);errors=[];checks=0
stride=20+0x114
for t,row in enumerate(rows):
 c=raw[t*stride:(t+1)*stride];draws,seed,owner,alive,events=struct.unpack_from('<5I',c)
 assert(draws,seed,owner,bool(alive),events)==(row['rng_draws'],row['seed'],row['owner_phase'],not bool(row['flags']&0x1000),len(row['damage_events'])),(t,draws,seed,owner,alive,events,row)
 n=bytearray.fromhex(row['animator_hex']);n[12:16]=bytes(4)
 for off in range(0,0x114,4):
  checks+=1
  if n[off:off+4]!=c[20+off:24+off] and len(errors)<100:errors.append(dict(tick=t,offset=hex(off+0xfc),native=struct.unpack_from('<f',n,off)[0],source=struct.unpack_from('<f',c,20+off)[0]))
r=dict(status='pass'if not errors else'fail',fields=checks,errors=errors,trace_sha256=sha(a.trace.read_bytes()),source_sha256=sha((ROOT/'src/game/effects/rockstormcore.h').read_bytes()),scope='Named Rock Storm level-one, explicit stationary target radius24. Complete live pool except target pointer identity, actual owner-stage/damage-callback cadence, actual kill, all RNG seeds.');(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,errors[:8]);assert not errors
