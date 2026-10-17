#!/usr/bin/env python3
"""Compare actual engine Strike owner/RNG and live particle trace to retail."""
import argparse,json,re,struct,hashlib
from pathlib import Path
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(reference,log,output,allow_partial=False):
 n=json.loads(reference.read_text());positions={tuple(o['position']):i for i,o in enumerate(n['initial']['owners'])};expected={};owners={};rng={}
 for row in n['records']:
  tick=row['tick'];rng[tick]=row['state']['rng_calls']
  for i,o in enumerate(row['state']['owners']):
   if o['flags']&0x1000:continue
   owners[tick,i]=(o['frame'],o['wait'],o['lifetime'],int(bool(o['flags']&0x4000)))
   if not(o['flags']&0x4000):continue
   for prototype,c in zip((0,5,2),(c for c in o['pools']if c['vtable']=='0x5a3544')):
    pool=bytes.fromhex(c['pool_hex'])
    for slot in range(c['capacity']):
     off=slot*252
     if struct.unpack_from('<I',pool,off+0xe4)[0]:expected[tick,i,prototype,slot]=struct.unpack_from('<6f',pool,off+0xb8)
 seen=set();errors=[];checks=0;maximum=0;tick=0;actual_rng={};seen_owners=set()
 for line in log.read_text().splitlines():
  m=re.search(r'\[strike-phase\] tick=(\d+) manager=(\d+) rng=(\d+) alive=(\d+)',line)
  if m:tick=int(m[1]);actual_rng[tick]=int(m[3]);continue
  m=re.search(r'\[strike-owner\] tick=(\d+) object=(\d+) frame=(\d+) expected=(\d+) wait=(\d+) life=(\d+) animate=(\d+)',line)
  if m:
   t,i,f,claimed,w,life,a=map(int,m.groups());key=t,i;seen_owners.add(key);checks+=5
   if (f,w,life,a)!=owners.get(key) or f!=claimed:errors.append(dict(owner=key,actual=(f,w,life,a),expected=owners.get(key)))
   continue
  m=re.search(r'\[strike-render\] tick=(\d+) owner=(\d+) frame=(\d+) live=(\d+) rng=(\d+)',line)
  if m:actual_rng[int(m[1])]=int(m[5]);continue
  m=re.search(r'\[strike-particle\] owner=(-?\d+),(-?\d+),(-?\d+) frame=(\d+) prototype=(\d+) slot=(\d+) pos=([^ ]+) vel=([^ ]+)',line)
  if m:
   x,y,z,frame,prototype,slot=map(int,m.groups()[:6]);i=positions[x,y,z];key=tick,i,prototype,slot;seen.add(key);values=list(map(float,(m[7]+','+m[8]).split(',')));original=expected.get(key)
   if original is None:errors.append(dict(unexpected_particle=key));continue
   for a,b in zip(values,original):
    d=abs(a-b);maximum=max(maximum,d);checks+=1
    if d>.002:errors.append(dict(particle=key,native=b,source=a,error=d))
 for t,r in actual_rng.items():
  checks+=1
  if r!=rng[t]:errors.append(dict(tick=t,native_rng=rng[t],source_rng=r))
 missing=set(expected)-seen
 if not allow_partial and missing:errors.append(dict(missing_particles=len(missing)))
 assert seen_owners==set(owners),(len(seen_owners),len(owners))
 result=dict(status='fail'if errors else'pass',checks=checks,live_particle_rows=len(seen),native_live_particle_rows=len(expected),maximum_error=maximum,error_count=len(errors),errors=errors[:40],partial_trace=allow_partial,reference_sha256=sha(reference),log_sha256=sha(log),source_sha256=sha(__file__),scope=__doc__)
 output.write_text(json.dumps(result,indent=2)+'\n');print(result['status'],checks,len(seen),len(errors));return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--log',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--allow-partial',action='store_true');a=p.parse_args();run(a.reference,a.log,a.output,a.allow_partial)
