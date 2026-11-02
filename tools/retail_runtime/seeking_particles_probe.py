"""Compile the unbound seeking core against full genuine qualified Sandswirl pools.

The original guest has an explicit sand->Sparks diagnostic patch. Quicksand is
an explicit real named-spell control, not a natural Sandswirl caller. This proves
particle/controller mechanics, never intended sand imagery or appearance.
"""
import argparse,hashlib,json,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
DRIVER=r'''#include "game/effects/legacy/seekingparticles.h"
#include <cstdio>
#include <cstring>
using namespace retail_seeking_particles;
template<class T> void read(T&x){if(fread(&x,sizeof x,1,stdin)!=1)std::abort();}
template<class T> void write(const T&x){fwrite(&x,sizeof x,1,stdout);}
int main(){Params p;Random r;State s;unsigned objects,neutral;read(objects);read(neutral);read(p);read(r.seed);read(r.calls);if(!s.Init(p,objects,r))return 2;
for(int t=1;t<=310;++t){unsigned reset;read(reset);if(reset){uint32_t seed;uint64_t calls;read(seed);read(calls);read(p);
// Only genuine Quicksand map allocation at75 is outside this particle core.
if(t==75 && !neutral){r.seed=seed;r.calls=calls;}else if(seed!=r.seed||calls!=r.calls)return 4;
if(!s.ResetTargets(p,r))return 3;}
s.Advance();write(r.seed);write(r.calls);unsigned kill=s.killed;write(kill);
for(const auto&q:s.pool)write(q);
struct Packet{int32_t variant;Vec pos;};std::vector<Packet> packets;
s.Render([&](int i,Vec v){packets.push_back({i,v});});unsigned n=packets.size();write(n);for(auto&v:packets)write(v);
}}
'''
def run(trace,output,baseline=None):
 output.mkdir(parents=True,exist_ok=False);a=json.loads(trace.read_text())
 assert (a['guest_code_modified'] or a.get('legacy_sand_restoration')) and a['named_spell_control']=='Quicksand' and len(a['records'])==310
 calls=a['particle_calls'];assert calls[0]['address']=='0x4e55b0' and len(calls)==76
 objects=1 if a.get('legacy_sand_restoration') else 4
 inp=struct.pack('<II',objects,int(bool(a.get('source_map_allocation_control'))))+bytes.fromhex(calls[0]['parameters_hex'])+struct.pack('<IQ',calls[0]['seed'],calls[0]['rng_calls'])
 for tick in range(1,311):
  c=next((c for c in calls[1:] if c['tick']==tick),None);inp+=struct.pack('<I',bool(c))
  if c:inp+=struct.pack('<IQ',c['seed'],c['rng_calls'])+bytes.fromhex(c['parameters_hex'])
 (output/'input.bin').write_bytes(inp);driver=output/'driver.cpp';driver.write_text(DRIVER)
 cmd=['clang++','-std=c++17','-O2','-ffp-contract=off','-Wall','-Wextra','-Werror','-iquote',str(ROOT/'src'),str(driver),'-o',str(output/'driver')]
 subprocess.run(cmd,check=True,capture_output=True,text=True)
 r=subprocess.run([str(output/'driver')],input=inp,capture_output=True,check=True).stdout;(output/'result.bin').write_bytes(r)
 offset=fields=packets=0
 for rec in a['records']:
  seed,rc,kill=struct.unpack_from('<IQI',r,offset);offset+=16;st=rec['state']
  assert (seed,rc,kill)==(st['seed'],st['rng_calls'],int(bool(st['child_flags']&4096))),rec['tick']
  fields+=3;pools={k:bytes.fromhex(v) for k,v in st['pool'].items()}
  for n in range(a['initial']['particles']):
   expected=pools['position'][n*12:(n+1)*12]+pools['velocity'][n*12:(n+1)*12]+b''.join(pools[k][n*4:(n+1)*4] for k in ('life','start','variant','target'))
   assert r[offset:offset+40]==expected,(rec['tick'],n);offset+=40;fields+=10
  count=struct.unpack_from('<I',r,offset)[0];offset+=4
  expected=rec['particle_packets'];assert count==len(expected),(rec['tick'],count,len(expected))
  for packet in expected:
   native=struct.pack('<i',packet['variant'])+bytes.fromhex(packet['position_hex'])
   assert r[offset:offset+16]==native,(rec['tick'],packet);offset+=16;packets+=1
 assert offset==len(r)
 unchanged=None
 moved=None
 if a.get('actor_moves'):
  assert [m['tick'] for m in a['actor_moves']]==[60,85]
  sampled=a['records'][74]['actors'][1]['position'];source=a['source']
  expected_target=(sampled[0]-source[0],sampled[1]-source[1],sampled[2]-source[2]-32)
  for row in a['records'][74:]:
   assert struct.unpack_from('<3f',bytes.fromhex(row['state']['parameters_hex']),100)==expected_target
  assert a['records'][84]['actors'][1]['position']!=sampled
  moved=dict(public_actor_moves=a['actor_moves'],sampled_tick=75,target=expected_target,post85_actor_move_does_not_retarget=True)
 if baseline:
  old=json.loads(baseline.read_text());assert len(old['records'])==310
  for oldrow,newrow in zip(old['records'],a['records']):
   for key,value in oldrow.items():assert newrow[key]==value,(oldrow['tick'],key)
  unchanged=dict(frames=310,all_original_record_fields_identical=True,baseline_sha256=sha(baseline),pngs=sum('image' in x for x in old['records']))
 test=output/'guards';subprocess.run(['clang++','-std=c++17','-O2','-ffp-contract=off','-Wall','-Wextra','-Werror','-iquote',str(ROOT/'src'),str(ROOT/'tools/test_seeking_particles.cpp'),'-o',str(test)],check=True,capture_output=True,text=True)
 guards=subprocess.run([str(test)],check=True,capture_output=True,text=True).stdout.strip()
 report=dict(status='pass',objects=objects,legacy_sand_restoration=a.get('legacy_sand_restoration'),accepted=False,visual_pass=False,frames=310,pool_and_rng_fields_exact=fields,render_packets_exact=packets,max_float_error=0,guards=guards,observer_negative_control=unchanged,moving_target_control=moved,trace_sha256=sha(trace),source_sha256=sha(ROOT/'src/game/effects/legacy/seekingparticles.h'),test_sha256=sha(ROOT/'tools/test_seeking_particles.cpp'),probe_sha256=sha(__file__),driver_sha256=sha(driver),result_sha256=sha(output/'result.bin'),compile_command=cmd,scope=__doc__,limits='No runtime factory/admission, missing intended sand imagery, no natural Sandswirl variant. Original seekz=41 is retained only as this control input. External real MapPane allocation seed/call-count event at75 is injected and declared; all other Init/Reset/Animate/Render RNG remains independently source-derived. Source x87 float-store boundaries are explicit; tested ABI/compiler only. Moving actors are proven only when a moving_target_control record is present; multi-target scenarios remain unproven.')
 if a.get('source_map_allocation_control'):report['limits']=report['limits'].replace('External real MapPane allocation seed/call-count event at75 is injected and declared;', 'Explicit RNG-neutral allocator configuration: no seed/call-count injection at75;')
 if a.get('legacy_sand_restoration'):
  report['scope']=report['scope'].replace('The original guest has an explicit sand->Sparks diagnostic patch.', 'The original guest has the evidenced legacy Sand registry/builder restored, using its own shipped asset.')
  report['limits']=report['limits'].replace('missing intended sand imagery,', 'explicit restored legacy Sand binding with its own shipped asset,').replace('Original seekz=41 is retained only as this control input.', f"Original raw seekz={a['initial']['seekz_raw']} is retained; only its nonzero Boolean semantics are tested.")
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--baseline',type=Path);a=p.parse_args();run(a.trace,a.output,a.baseline)
