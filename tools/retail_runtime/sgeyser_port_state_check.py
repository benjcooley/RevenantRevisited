"""Actual engine steam particles, ground calls, owner frame and RNG vs original."""
import argparse,json,re,struct,hashlib
from pathlib import Path
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def run(native,log,out):
 n=json.loads(native.read_text());expected={};phases={};ground=[]
 for row in n['rows']:
  tick=row['tick'];phases[tick]=(row['frame'],row['rng_calls']);ground.extend((tick,*q['point'])for q in row['ground']);pool=bytes.fromhex(row['pool_hex'])
  if row['flags']&0x1000:continue
  for slot in range(104):
   if struct.unpack_from('<I',pool,252*slot+0xe4)[0]:expected[tick,slot]=struct.unpack_from('<6f',pool,252*slot+0xb8)
 seen=set();actual_ground=[];errors=[];checks=0;maximum=0;phase_count=0
 for line in log.read_text().splitlines():
  m=re.search(r'\[sgeyser-particle\] tick=(\d+) slot=(\d+) pos=([^ ]+) vel=([^ ]+)',line)
  if m:
   key=int(m[1]),int(m[2]);seen.add(key);values=list(map(float,(m[3]+','+m[4]).split(',')));original=expected.get(key);assert original is not None,key
   for a,b in zip(values,original):
    e=abs(a-b);maximum=max(maximum,e);checks+=1
    if e>.002:errors.append(dict(key=key,native=b,source=a))
  m=re.search(r'\[sgeyser-(phase|kill)\] tick=(\d+) frame=(\d+) (?:particles=\d+ )?rng=(\d+)',line)
  if m:
   t,frame,rng=map(int,m.groups()[1:]);phase_count+=1;checks+=2
   if (frame,rng)!=phases[t]:errors.append(dict(tick=t,native=phases[t],source=(frame,rng)))
  m=re.search(r'\[sgeyser-ground\] tick=(\d+) point=(-?\d+),(-?\d+),(-?\d+)',line)
  if m:actual_ground.append(tuple(map(int,m.groups())))
 assert seen==set(expected),(len(seen),len(expected));assert actual_ground==ground,(len(actual_ground),len(ground));assert phase_count==45,phase_count
 result=dict(status='fail'if errors else'pass',fields=checks,live_particle_rows=len(seen),ground_queries_exact=len(ground),owner_rng_phases=phase_count,max_error=maximum,errors=errors[:30],error_count=len(errors),native_sha256=sha(native),log_sha256=sha(log),accepted=False,scope=__doc__);out.write_text(json.dumps(result,indent=2)+'\n');print(result)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--log',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.log,a.output)
