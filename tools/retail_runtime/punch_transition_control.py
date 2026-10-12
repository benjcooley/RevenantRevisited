"""Actual GPU negative control: omitted owner previous-state caller vs native."""
import argparse,json,re,struct
from pathlib import Path
from character_native_preflight import sha

def compare(rows,folder):
 log=(folder/'run.log').read_text();poses={(int(t),int(o)):[float(x)for x in values.split(',')]for t,f,o,values in re.findall(r'\[punch-pose\] tick=(\d+) frame=(\d+) object=(\d+) matrix=([^\n]+)',log)};errors=[];maximum=0;fields=0
 for row in rows[1:]:
  assert(row['state']['previous_state'],row['state']['previous_frame'])==(0,0)
  for d in row['draws']:
   current=poses[(row['tick'],d['object'])].copy()
   for j in (12,13):current[j]=struct.unpack('<f',struct.pack('<f',current[j]+3185))[0]
   delta=max(abs(a-b)for a,b in zip(current,d['matrix']));maximum=max(maximum,delta);fields+=16
   if delta>.0003:errors.append(dict(tick=row['tick'],object=d['object'],maximum_error=delta))
 return dict(fields=fields,max_error=maximum,errors=errors,binary_sha256=json.loads((folder/'manifest.json').read_text())['binary_sha256'],manifest_sha256=sha(folder/'manifest.json'),log_sha256=sha(folder/'run.log'))
def run(native,positive,negative,output):
 rows=json.loads((native/'manifest.json').read_text())['records'];a=compare(rows,positive);b=compare(rows,negative);assert not a['errors'] and len(b['errors'])==26 and set(x['tick']for x in b['errors'])=={2,3,583,584}
 report=dict(status='pass',positive=a,negative=b,native_manifest_sha256=sha(native/'manifest.json'),scope=__doc__,cause='Original PreRender supplies previous state0/frame0 to409950, whose literal first-five-frame interpolation runs after same-state wrap. Port animator normally suppresses that same-state transition. Leaf native-domain adapter supplies actual owner previous-state/frame through existing SetPrevState; ordinary camera submission unchanged.')
 output.write_text(json.dumps(report,indent=2)+'\n');print(a['max_error'],len(b['errors']))
if __name__=='__main__':
 p=argparse.ArgumentParser();[p.add_argument('--'+x,type=Path,required=True)for x in ('native','positive','negative','output')];a=p.parse_args();run(a.native,a.positive,a.negative,a.output)
