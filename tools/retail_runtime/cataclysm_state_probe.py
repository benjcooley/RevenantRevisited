"""Compile the production Cataclysm pool against complete original owner/animator bytes."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,sha
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
native=json.loads(a.trace.read_text());rows=[native['initial']]+[r['state']for r in native['records']]
cpp=r'''
#include <cstdio>
#include "cataclysmcore.h"
uint32_t seed=1,draws=0;int random_range(int lo,int hi){if(lo==hi)return lo;++draws;seed=seed*214013+2531011;return lo+int((seed>>16)&32767)%(hi-lo+1);}
int main(){cataclysm_core::State s;s.Initialize(random_range);
 for(int tick=0;tick<TICKS;++tick){if(tick)s.Pulse(random_range);
 fwrite(&draws,4,1,stdout);fwrite(&seed,4,1,stdout);fwrite(&s.life,4,1,stdout);fwrite(&s.phase,4,1,stdout);fwrite(&s.velocity,12,1,stdout);fwrite(&s.position,12,1,stdout);fwrite(&s.pool,sizeof(s.pool),1,stdout);}}
'''.replace('TICKS',str(len(rows)))
source=a.output/'core.cpp';source.write_text(cpp);binary=a.output/'core';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
raw=subprocess.check_output([str(binary)]);(a.output/'port.bin').write_bytes(raw);errors=[];checks=0;exact=0
stride=40+0x13f8-0xfc
for tick,row in enumerate(rows):
 chunk=raw[tick*stride:(tick+1)*stride];draws,seed,life,phase=struct.unpack_from('<4I',chunk)
 assert(draws,seed,life,phase)==(row['rng_draws'],row['seed'],row['life'],row['phase']),(tick,(draws,seed,life,phase),row['rng_draws'],row['seed'],row['life'],row['phase'])
 assert list(struct.unpack_from('<3i',chunk,28))==row['position'],tick
 assert list(struct.unpack_from('<3f',chunk,16))==row['velocity'],tick
 n=bytes.fromhex(row['animator_hex']);c=chunk[40:]
 for pos in range(0,len(n),4):
  checks+=1
  if n[pos:pos+4]==c[pos:pos+4]:exact+=1;continue
  if len(errors)<100:errors.append(dict(tick=tick,offset=hex(pos+0xfc),native_hex=n[pos:pos+4].hex(),port_hex=c[pos:pos+4].hex(),native_float=struct.unpack_from('<f',n,pos)[0],port_float=struct.unpack_from('<f',c,pos)[0]))
r=dict(status='pass'if not errors else'differences',checks=checks,exact=exact,errors=errors,trace=dict(path=str(a.trace.resolve()),sha256=sha(a.trace.read_bytes())),source_sha256=sha((ROOT/'src/cataclysmcore.h').read_bytes()),scope='Complete 30-piece/70-dust/50-glow pool, owner position/velocity/phase/life and all RNG seeds, null-spell empty-character-world owner Pulse cadence including actual kill tick.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,'exact',exact,'errors',errors[:8]);assert not errors
