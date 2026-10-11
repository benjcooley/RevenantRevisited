#!/usr/bin/env python3
"""Pin Energy bounded-resource pixel regression without temporal re-alignment."""
import argparse,json,hashlib
from pathlib import Path

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def check(root,out):
 pairs=[('native140','energy-resource-domain-before','energy-resource-domain-after',140),('approved_common140','energy-metal-common-final-v2','energy-resource-common-after',140),('existing_prelit_literal','energy-resource-literal-before','energy-resource-literal-after',3),('default_shriek','energy-resource-default-before','energy-resource-default-after',3)]
 rows=[]
 for label,before,after,count in pairs:
  a=root/before/'manifest.json';b=root/after/'manifest.json';am=json.load(open(a));bm=json.load(open(b));assert am['status']==bm['status']=='pass'and len(am['frames'])==len(bm['frames'])==count
  for x,y in zip(am['frames'],bm['frames']):assert sha(Path(x['path']))==x['sha256']==y['sha256']==sha(Path(y['path']))
  rows.append(dict(name=label,identical_frames=count,before_manifest=str(a),before_sha256=sha(a),after_manifest=str(b),after_sha256=sha(b),binary_sha256=bm['binary_sha256']))
 assert len({r['binary_sha256']for r in rows})==1
 # Historical stales are a separate diagnostic, never matched to another phase.
 historical=[]
 for a,b in(('energy-metal-domain-final-v2','energy-resource-domain-before'),('energy-resource-common-before','energy-resource-common-after')):
  am=json.load(open(root/a/'manifest.json'));bm=json.load(open(root/b/'manifest.json'));aa=[f['sha256']for f in am['frames']];bb=[f['sha256']for f in bm['frames']]
  for i,(x,y)in enumerate(zip(aa,bb)):
   if x!=y:assert i>0 and x==bb[i-1];historical.append(dict(baseline=a,current=b,frame=i+1,baseline_equals_current_previous=True))
 report=dict(status='pass',comparisons=rows,historical_capture_exceptions=historical,scope='Exact same-index PNG identity; no frame matching or refit. Native140 current vs fresh old-policy baseline, common140 vs approved current configuration. Old native reference2 stale frames and fresh common baseline1 stale frame remain separately disclosed. No shader fragment/geometry/order/blend change.')
 out.write_text(json.dumps(report,indent=2)+'\n');print('pass',sum(r['identical_frames']for r in rows));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--artifacts',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();check(a.artifacts,a.output)
