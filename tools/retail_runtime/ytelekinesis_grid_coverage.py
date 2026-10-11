"""Require whole original triangles to fit immutable supported capture windows."""
import argparse,hashlib,json,struct
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();j=json.loads(a.trace.read_text());fail=[];count=0
for row in j['records']:
 if not(row['state']['flags']&4096):assert len(row['draws'])==4,'Use --retain-all-draws; every active tick is required'
 for d in row['draws']:
  m=d['matrix'];projected=[]
  for v in struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex'])):
   x,y,z=[v[0]*m[i]+v[1]*m[i+4]+v[2]*m[i+8]+m[i+12]for i in range(3)]
   projected.append((1.010152578*(x-y),.505076289*(x+y)-1.23717916*z))
  for tri in struct.iter_unpack('<3H',bytes.fromhex(d['indices_hex'])):
   count+=1;points=[projected[i]for i in tri]
   if not any(all(2<x+cx<1918 and 2<y+cy<1078 for x,y in points)for cx in [2700,1560,420]for cy in [960,420]):fail.append(dict(tick=row['tick'],triangle=tri,points=points))
assert not fail,fail[:3]
r=dict(status='pass',all_native_triangles=count,frames=len(j['records']),window=[1920,1080],x_centers=[2700,1560,420],y_centers=[960,420],margin_pixels=2,trace=dict(path=str(a.trace.resolve()),sha256=hashlib.sha256(a.trace.read_bytes()).hexdigest()),scope='Every complete original triangle at every active tick fits at least one immutable supported window. Source-derived native projector coefficients include normal owner/view transforms already present in observed matrices. Raw overlaps remain independent raster evidence.')
a.output.write_text(json.dumps(r,indent=2)+'\n');print('pass',count)
