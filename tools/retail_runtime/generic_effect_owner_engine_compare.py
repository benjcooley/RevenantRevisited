#!/usr/bin/env python3
"""Compare the real source spell-owner fixture log with original owner fields."""
import argparse,hashlib,json,re
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--capture',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
native=json.loads(a.native.read_text());capture=json.loads((a.capture/'manifest.json').read_text());assert capture['status']=='pass',capture.get('error')
text=(a.capture/'run.log').read_text();rows={};fields=['0xe8','0x12c','0x138','0x13c','0x140','0x144','0x148'];compared=0
for line in text.splitlines():
 m=re.search(r'\[regeneration-owner\] step=(\d+) done=(\d+) (.*)',line)
 if not m:continue
 step,done=int(m[1]),int(m[2]);assert step not in rows,step;rows[step]=m[3];expected=native['rows'][step];assert expected['tick']==step and expected['done']==done,(step,done)
 if not expected['after']:assert m[3]=='absent=1',(step,m[3]);continue
 n=expected['after'][0]
 state=re.fullmatch(r'flags=([0-9a-f]+) pos=(-?\d+),(-?\d+),(-?\d+) follow=(-?\d+) remaining=(-?\d+) delay=(-?\d+) light=(-?\d+),(-?\d+),(-?\d+),(-?\d+) resident=(\d+) frame=(\d+)',m[3]);assert state,(step,m[3])
 actual=[int(state[1],16),*[int(state[i])for i in range(2,12)]]
 wanted=[int(n['flags'],16),*n['position'],*[n['fields'][key]for key in fields]]
 assert actual==wanted,(step,actual,wanted)
 assert state[12]=='1',('nonresident owner',step)
 compared+=len(actual)
assert list(rows)==list(range(2880)),(len(rows),min(rows),max(rows))
end=re.search(r'\[regeneration-spell-end\] step=(\d+) flags=([0-9a-f]+) bound=(\d+)',text);assert end and end[1]=='2879'and int(end[2],16)&0x1000 and end[3]=='0'
assert '[shutdown] complete'in text
report=dict(status='pass',steps=len(rows),owner_fields=compared,native_sha256=hashlib.sha256(a.native.read_bytes()).hexdigest(),capture_manifest_sha256=hashlib.sha256((a.capture/'manifest.json').read_bytes()).hexdigest(),binary_sha256=capture['binary_sha256'],scope='Real source loaded classes, resident transient map, shipped Strike creator, production Timer/Pulse/Kill, generic animator/SubmitPartSys and actor SetPos. Explicit fixture scheduling; no actor AI, natural cast, particles/device visual equivalence or broader spell-control claim.')
a.output.write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
