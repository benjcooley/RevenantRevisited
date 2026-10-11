"""Compare actual Metal-run owner/particle/cache witnesses to own native caller."""
import argparse,json,re,struct
from pathlib import Path
from speed_profile_contract import sha
PATTERN=re.compile(r'\[dragonattack-phase\] tick=(\d+) state=(\d+) frame=(\d+) position=(-?\d+),(-?\d+),(-?\d+) alive=(\d+) capacity=(\d+) random=(\d+) cache=([0-9.eE+-]+),([0-9.eE+-]+),([0-9.eE+-]+)')
def run(trace,log,output):
 original=json.loads(trace.read_text());records=original['records'];rows=PATTERN.findall(log.read_text());errors=[];checks=0;ticks=[]
 for row in rows:
  v=list(map(int,row[:9]));tick=v[0]-1
  if tick<0:continue
  a=records[tick]['after'];alive=0;capacity=0
  for c in a['controllers']:
   capacity+=c['capacity'];raw=bytes.fromhex(c['pool_hex']);alive+=sum(bool(struct.unpack_from('<i',raw,i*252+228)[0])for i in range(c['capacity']))
  expected=[a['state'],a['frame'],*a['position'],alive,capacity,a['rng_draws']]
  if v[1:]!=expected:errors.append(dict(tick=tick,source=v[1:],native=expected))
  cache=list(map(float,row[9:]));actual=a['object_matrices'][0][12:15]
  if max(abs(x-y)for x,y in zip(cache,actual))>.00002:errors.append(dict(tick=tick,source_cache=cache,native_cache=actual))
  checks+=11;ticks.append(tick)
 assert ticks,'No actual Dragon phase witnesses'
 result=dict(status='fail'if errors else'pass',checks=checks,records=len(ticks),ticks=ticks,errors=errors,trace_sha256=sha(trace.read_bytes()),log_sha256=sha(log.read_bytes()),scope='Actual process owner state/frame/position, live particles/capacity, random draw count and cached prototype origin; complete RNG seed and every pool field separately proved by native/core comparator. No image verdict.')
 output.write_text(json.dumps(result,indent=2)+'\n');print(result['status'],checks,errors[:3]);return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('log',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.trace,a.log,a.output);raise SystemExit(r['status']!='pass')
