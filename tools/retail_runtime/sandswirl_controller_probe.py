"""Compile parent-only Sandswirl controller and compare genuine qualified trace.

Particle allocation/seeking/render and missing intended child imagery are not
certified by this source check. Original modified-child execution is retained
separately; input actors/query are read from its genuine map caller record.
"""
import argparse,json,struct,subprocess,hashlib
from pathlib import Path
from character_native_preflight import ROOT,sha

def run(trace,output):
 output.mkdir(parents=True,exist_ok=False);native=json.loads(trace.read_text())
 assert native['named_spell_control']=='Quicksand' and native['guest_code_modified']
 assert len(native['records'])==310 and len(native['queries'])==1
 actors={index:(i+1,a) for i,(index,a) in enumerate(zip(native['actor_map_indices'],native['actors_before']))}
 query=[actors[index] for index in native['queries'][0]['indices']]
 origin=native['source'];assign=[]
 for i,(identity,a) in enumerate(query):
  x,y,z=a['position'];assign.append(f'caller.found[{i}]={{{identity},{{{x},{y},{z}}},{a["health"]}}};')
 driver=output/'driver.cpp'
 driver.write_text('''#include "effects/sandswirlcontroller.h"
#include <cstdio>
#include <cstring>
#include <cstdint>
using namespace retail_sandswirl;
uint32_t bits(float f){uint32_t b;std::memcpy(&b,&f,4);return b;}
int main(){Controller c;if(!c.Start(true))return 2;Caller caller;caller.valid=true;caller.caster=1;
'''+f'caller.origin={{{origin[0]},{origin[1]},{origin[2]}}};caller.count={len(query)};'+''.join(assign)+'''
for(int tick=1;tick<=310;++tick){auto r=c.Advance(caller);const auto&t=c.Targets();
std::printf("%d %d %d %u %d %u %u %u %u %u %u %u %u %d\\n",tick,c.Frame(),c.Alive(),t.count,t.seeking,bits(t.turn),bits(t.speed),bits(t.autorange),bits(t.hitrange),bits(t.positions[0][0]),bits(t.positions[0][1]),bits(t.positions[0][2]),r.spawn_count,r.kill);}}
''')
 binary=output/'controller';cmd=['clang++','-std=c++17','-O2','-Wall','-Wextra','-Werror','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)]
 subprocess.run(cmd,check=True,capture_output=True,text=True);lines=subprocess.run([str(binary)],check=True,capture_output=True,text=True).stdout.splitlines()
 count=0;failures=[]
 for line,actual in zip(lines,native['records']):
  values=list(map(int,line.split()));raw=bytes.fromhex(actual['state']['parameters_hex']);u=lambda p:struct.unpack_from('<I',raw,p)[0]
  tick=actual['tick'];expected=[tick,actual['state']['parent_frame'],int(not actual['state']['owner_flags']&4096),u(96),int(bool(u(88))),u(160),u(164),u(168),u(172),u(100),u(104),u(108),int(tick==75),int(tick==300)]
  count+=len(expected)
  if values!=expected:failures.append(dict(tick=tick,source=values,native=expected))
 assert len(lines)==len(native['records']);assert not failures,failures[:5]
 # Executable safeguards and filter cases are independent of trace comparison.
 test=output/'guards';subprocess.run(['clang++','-std=c++17','-Wall','-Wextra','-Werror','-iquote',str(ROOT/'src'),str(ROOT/'tools/test_sandswirl_controller.cpp'),'-o',str(test)],check=True,capture_output=True,text=True)
 guard=subprocess.run([str(test)],check=True,capture_output=True,text=True).stdout.strip()
 report=dict(status='pass',accepted=False,parent_controller_only=True,fields=count,frames=len(lines),failures=failures,
             trace_sha256=sha(trace),source_sha256=sha(ROOT/'src/effects/sandswirlcontroller.h'),
             probe_sha256=sha(__file__),driver_sha256=sha(driver),guard_test_sha256=sha(ROOT/'tools/test_sandswirl_controller.cpp'),
             guard_result=guard,compile_command=cmd,scope=__doc__,limits='The new source contract chooses no asset and has no default runtime admission. Missing child/caller fail-closed behavior intentionally avoids original null dereferences. Source particle seeking/pool/geometry/RNG and original sand resource remain unproven.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
