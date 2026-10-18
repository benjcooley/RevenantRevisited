"""Compile production Dragonfire core against independent original pool/RNG trace."""
import argparse,json,struct,subprocess,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def run(native,out):
 out.mkdir(parents=True,exist_ok=False);ref=json.loads(native.read_text());rows=ref['records'];n=len(rows)-1
 cpp=out/'probe.cpp';cpp.write_text(r'''
#include "dragonfirecore.h"
#include <cstdio>
#include <cstdlib>
int main(int argc,char**argv){retail_dragonfire::State s;unsigned seed=1,calls=0;auto random=[&](int lo,int hi){seed=seed*214013u+2531011u;++calls;return lo+int((seed>>16)&32767u)%(hi-lo+1);};
for(int tick=0;tick<=std::atoi(argv[1]);++tick){if(tick){s.Advance(random);if(argc>2 && std::atoi(argv[2])==2)s.Advance(random);if(argc>2 && std::atoi(argv[2])==1)for(auto& p:s.fire.particles)if(p.used && p.life==0){p.pos.x+=18;p.pos.y-=50;p.pos.z+=15;}}unsigned h[]={unsigned(s.frame),unsigned(s.done),unsigned(s.alive),seed,calls};fwrite(h,4,5,stdout);fwrite(&s.fire,1,sizeof(s.fire),stdout);fwrite(&s.smoke,1,sizeof(s.smoke),stdout);}}
''')
 binary=out/'probe';subprocess.run(['clang++','-std=c++17','-ffp-contract=off','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)],check=True)
 raw=subprocess.check_output([str(binary),str(n)]);stride=20+2*300*88;assert len(raw)==stride*len(rows);fields=0;maximum=0.;fail=[]
 for tick,row in enumerate(rows):
  state=row['state'];frame,done,alive,seed,calls=struct.unpack_from('<5I',raw,tick*stride)
  assert (frame,done,alive,seed,calls)==(state['frame'],state['done'],int(not(state['flags']&0x1000)),state['seed'],state['rng_calls']),(tick,(frame,done,alive,seed,calls),state)
  for pool_index,name in enumerate(('fire','smoke')):
   native_bytes=bytes.fromhex(state['pools'][name]);source_bytes=raw[tick*stride+20+pool_index*26400:tick*stride+20+(pool_index+1)*26400]
   for slot in range(300):
    a=struct.unpack_from('<18f4i',native_bytes,slot*88);b=struct.unpack_from('<18f4i',source_bytes,slot*88)
    assert a[21]==b[21],(tick,name,slot,'used');fields+=1
    if not a[21]:continue
    # Original stack flicker is unspecified and unused: both Render arguments
    # arefalse. Dead/cold storage likewise has no simulation/render meaning.
    for j in range(18):
     e=abs(a[j]-b[j]);maximum=max(maximum,e);fields+=1
     if e>1e-5:fail.append((tick,name,slot,j,a[j],b[j]))
    assert a[19:21]==b[19:21],(tick,name,slot,'life/span');fields+=2
 assert not fail,fail[:10]
 negatives={}
 for name,mode in [('snapshot_birth_offsets',1),('double_animate',2)]:
  wrong=subprocess.check_output([str(binary),'1',str(mode)]);reference=raw[stride:2*stride];different=sum(a!=b for a,b in zip(wrong[stride:2*stride],reference));assert different>0;negatives[name]=dict(rejected=True,differing_bytes=different)
 result=dict(negative_controls=negatives,status='pass',ticks=n,fields=fields,max_float_error=maximum,native_manifest_sha256=hashlib.sha256(native.read_bytes()).hexdigest(),core_sha256=hashlib.sha256((ROOT/'src/game/effects/dragonfirecore.h').read_bytes()).hexdigest(),scope=__doc__,ignored='Uninitialized cold/dead storage and unused per-particle flicker; Render(false,false).')
 (out/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
