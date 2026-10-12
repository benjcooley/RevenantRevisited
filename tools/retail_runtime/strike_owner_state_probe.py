"""Compiled narrow Meteor Storm launch/owner cadence versus genuine full caller."""
import argparse,json,re,subprocess,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native,out):
 out.mkdir(parents=True,exist_ok=False);ref=json.loads(native.read_text())
 source=(ROOT/'src/object.cpp').read_text();vector=source[source.index('void ConvertToVector('):source.index('\nint32_t ConvertToFacing(')]
 source=(ROOT/'src/colortable.cpp').read_text();tables=source[source.index('    // make this as explicit'):source.index('    signed char *coscosidx')]
 cpp=out/'probe.cpp';cpp.write_text('#include "strikemeteorcore.h"\n#include <cstdio>\n#include <cmath>\n#include <cstdlib>\nusing S3DPoint=strike_meteor::Point;\nshort DistX[256],DistY[256];\nconstexpr double M_2PI=6.283185308;\nint Round(double v){return int(v)+(v-int(v)>=.5);}\n'+vector+'\nvoid tables(){'+tables+'}\n'+r'''
int main(int argc,char**argv){tables();int mode=argc>1?std::atoi(argv[1]):0;unsigned seed=1,calls=0;auto draw=[&](){seed=seed*214013u+2531011u;++calls;return int((seed>>16)&32767u);};auto random=[&](int a,int b){return a+draw()%(b-a+1);};auto index=[&](){if(mode==1)return;seed=draw();(void)draw();};auto vector=[](int angle,int dist){S3DPoint p;ConvertToVector(angle,dist,p,0);return p;};
strike_meteor::State s;s.Initialize({3185,3185,16},random,index,vector);if(mode==3)for(auto&o:s.owners)o.wait=0;printf("%u %u\n",seed,calls);
for(int t=0;t<=100;++t){if(t){s.Advance();if(mode==2)s.Advance();}for(const auto&o:s.owners)printf("%d %d %d %d %d %d %d %d %d\n",t,s.manager_frame,o.position.x,o.position.y,o.position.z,o.frame,o.wait,o.lifetime,int(o.alive));}}
''')
 binary=out/'probe';subprocess.run(['clang++','-std=c++17','-iquote',str(ROOT/'src'),str(cpp),'-o',str(binary)],check=True);lines=subprocess.check_output([str(binary)],text=True).splitlines();seed,calls=map(int,lines[0].split());assert(seed,calls)==(ref['initial']['seed'],ref['initial']['rng_calls'])
 expected=[dict(tick=0,state=ref['initial'])]+ref['records'];rows=[list(map(int,l.split()))for l in lines[1:]];checks=2
 for record in expected:
  t=record['tick'];state=record['state']
  for i,o in enumerate(state['owners']):
   a=[t,state['spell_frame'],*o['position'],o['frame'],o['wait'],o['lifetime'],int(not(o['flags']&0x1000))];b=rows[t*4+i];assert a==b,(t,i,a,b);checks+=len(a)
 negative={}
 for mode,name in((1,'omit_intrinsic_index_rng'),(2,'double_manager_pulse'),(3,'omit_original_start_delays')):
  wrong=subprocess.check_output([str(binary),str(mode)],text=True).splitlines();differences=sum(a!=b for a,b in zip(lines,wrong));assert differences>0;negative[name]=dict(rejected=True,different_rows=differences)
 report=dict(negative_controls=negative,status='pass',checks=checks,records=len(expected),native_sha256=sha(native),core_sha256=sha(ROOT/'src/strikemeteorcore.h'),compiled_source_sha256=sha(cpp),scope=__doc__,limits='Launch/manager/owner state only; controller pools/poses/rendering need separate proof. Uses declared clock0 intrinsic index RNG, not global map index changes.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
