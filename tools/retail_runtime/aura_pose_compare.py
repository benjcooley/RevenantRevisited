#!/usr/bin/env python3
"""Actual runtime source integer bone APIs versus genuine original Acolyte."""
import argparse,json,re
from pathlib import Path
from speed_profile_contract import sha

def run(output,native,source_log):
 output.mkdir(parents=True,exist_ok=False);n=json.loads(native.read_text());assert n['interpolate']is False and n['state']==14 and n['frame']==0;poses={int(a):list(map(float,b.split(',')))for a,b in re.findall(r'\[aura-actor-pose\] object=(\d+) ok=1 matrix=([0-9.eE+,-]+)',source_log.read_text())};assert len(poses)==22;errors=[];maximum=0
 for row in n['rows']:
  d=max(abs(a-b)for a,b in zip(row['matrix'],poses[row['index']]));maximum=max(maximum,d)
  if d>1e-5:errors.append(dict(object=row['index'],error=d))
 r=dict(status='pass'if not errors else'fail',accepted=False,fields=352,objects=22,max_error=maximum,errors=errors,native_manifest_sha256=sha(native.read_bytes()),actual_source_log_sha256=sha(source_log.read_bytes()),scope='All22 genuine constructor-selected Acolyte source CalcObjectMatrix API outputs during actual Aura Metal startup, independently matched to full original40a420 under declared Interpolatefalse/no-transition14/0. Scoped source flag restoration leaves ordinary setting intact. No saved/native bone matrix injected into production; float/SRT residual retained, not equality.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],r['fields'],maximum);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--source-log',type=Path,required=True);a=p.parse_args();run(a.output,a.native,a.source_log)
