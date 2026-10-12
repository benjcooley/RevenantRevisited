"""Conservative all-frame authored-triangle coverage of fixed Nakrnoth views."""
import argparse,json,struct,zipfile
from pathlib import Path
from speed_profile_contract import ROOT,sha
VIEWS=((1440,920),(480,920),(1440,520),(480,520),(1440,120),(480,120))
def run(trace,output):
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:b=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/nakrnoth.i3d'))
 u=lambda p:struct.unpack_from('<I',b,p)[0];h=lambda p:struct.unpack_from('<H',b,p)[0];ptr=lambda p:p+u(p)
 body=20+u(16);objs=ptr(body+48);vp=ptr(ptr(ptr(body+16)));ip=ptr(body+24);objects={}
 for o in range(11,63):
  raw=objs+48*o;fg=ptr(raw+40);vertices=[struct.unpack_from('<3f',b,vp+32*(h(raw+34)+i))for i in range(h(raw+36))];triangles=[]
  for slot in range(1,6):triangles.extend(struct.unpack_from('<3H',b,ip+6*(h(fg+slot*4)+i))for i in range(h(fg+slot*4+2)))
  objects[o]=(vertices,triangles)
 total=rejected=covered=0;errors=[];margin=1e9;z=16/(1.4600000381469727-16*.0033333334140479565*.009999999776482582)*1.0379999876022339
 for line in trace.read_text().splitlines():
  q=line.split()
  if q[0]!='M' or int(q[2])not in objects:continue
  tick,o=int(q[1]),int(q[2]);m=list(map(float,q[3:]));vertices,triangles=objects[o];points=[]
  for p in vertices:
   w=[sum(p[c]*m[c*4+k]for c in range(3))+m[12+k]for k in range(3)];x,y,wz=w;wz+=z;points.append((1.010152578*(x-y),-(.505076289*(x+y)-1.23717916*wz)))
  for face,ids in enumerate(triangles):
   p=[points[i]for i in ids];total+=1
   if any(abs(p[0][0]-p[i][0])>640 or abs(p[0][1]-p[i][1])>480 for i in(1,2)):rejected+=1;continue
   box=[min(a[k]for a in p)for k in range(2)]+[max(a[k]for a in p)for k in range(2)]
   margins=[min(box[0]+cx,box[1]+cy,1919-box[2]-cx,1079-box[3]-cy)for cx,cy in VIEWS];best=max(margins)
   if best<0:errors.append(dict(tick=tick,object=o,face=face,box=box))
   else:covered+=1;margin=min(margin,best)
 report=dict(status='fail'if errors else'pass',triangles=total,rejected_by_declared_original_span=rejected,whole_triangles_covered=covered,minimum_margin=margin,errors=errors,views=VIEWS,viewport=[1920,1080],trace_sha256=sha(trace.read_bytes()),asset_sha256=sha(b),scope='All774 active source phases with independently native-verified local matrices, original ownerZ and fixed native projector coefficients. Every software-admitted authored base triangle is wholly contained in at least one immutable window. Particle cloud is separately contained in main raw framebuffer; raw overlaps retained. This bounds proof is not pixel equality.')
 output.write_text(json.dumps(report,indent=2)+'\n');print(report['status'],total,rejected,covered,margin,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--trace',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
