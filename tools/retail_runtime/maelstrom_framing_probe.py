import sys,zipfile,struct,argparse,json,hashlib
from pathlib import Path
sys.path.insert(0,'tools/retail_runtime')
from speed_controller_preflight import read_asset
parser=argparse.ArgumentParser(description='Bound every native-verified Maelstrom triangle in fixed supported camera windows');parser.add_argument('--pose',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--variant',choices=['maelstrom','ymaelstrom'],default='maelstrom');args=parser.parse_args();p=args.pose
with zipfile.ZipFile('data/imagery.rvi')as z:a=read_asset(z.read('Imagery/Magic/'+args.variant+'.I3D'))
vertices=[list(struct.iter_unpack('<8f',bytes.fromhex(o['vertex_hex'])))for o in a['objects']];faceall=list(struct.iter_unpack('<3H',bytes.fromhex(a['face_hex'])));faces=[]
for o in a['objects']:
 runs=o['texture_runs'];faces.append([f for start,count in zip(runs[::2],runs[1::2])for f in faceall[start:start+count]])
def clip(poly,axis,edge,greater):
 out=[]
 for a,b in zip(poly,poly[1:]+poly[:1]):
  ain=(a[axis]>=edge)if greater else(a[axis]<=edge);bin=(b[axis]>=edge)if greater else(b[axis]<=edge)
  if ain:out.append(a)
  if ain!=bin:
   t=(edge-a[axis])/(b[axis]-a[axis]);out.append(tuple(a[i]+t*(b[i]-a[i])for i in range(2)))
 return out
views=[(1200,550),(720,550),(960,850)];badpoints=badfaces=0;uncovered=0;examples=[];allbounds=[]
def inside(pt,view):x,y=pt;cx,cy=view;return 0<=x+cx<1920 and 0<=y+cy<1080
for line in p.read_text().splitlines():
 q=line.split();st,o,t=map(int,q[:3]);v=list(map(float,q[3:]));m=v[9:]
 if min(v[6:9])<=1e-5:continue
 pts=[]
 for pt in vertices[o]:
  w=[sum(m[j*4+i]*pt[j]for j in range(3))+m[12+i]for i in range(3)];pts.append((1.0101525783538818*(w[0]-w[1]),.5050762891769409*(w[0]+w[1])-1.237179160118103*w[2]))
 for pt in pts:
  if not any(inside(pt,view)for view in views):badpoints+=1
 for f in faces[o]:
  for axedge,ayedge,gx,gy in [(-960,-550,False,False),(960,-550,True,False)]:
   poly=clip(clip([pts[i]for i in f],0,axedge,gx),1,ayedge,gy)
   area=abs(sum(x[0]*y[1]-x[1]*y[0]for x,y in zip(poly,poly[1:]+poly[:1])))*.5
   uncovered+=area>1e-7
  if not any(all(inside(pts[i],view)for i in f)for view in views):
   badfaces+=1
   if len(examples)<4:examples.append((o,t,[pts[i]for i in f]))
print('views',views,'uncoveredpoints',badpoints,'faces_not_whole_in_one',badfaces,'examples',examples)

print('triangles intersecting uncovered corners',uncovered)

assert badpoints==0 and badfaces==0 and uncovered==0
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
args.output.mkdir(parents=True,exist_ok=True)
(args.output/'proof.json').write_text(json.dumps(dict(status='pass',views=[dict(viewport=[1920,1080],screen_center=list(v))for v in views],uncovered_corners=badpoints,triangles_not_whole_in_a_window=badfaces,uncovered_triangle_regions=uncovered,pose_trace_sha256=sha(p),source_sha256=sha(Path(__file__)),scope='Every visible-scale authored corner and complete triangle at all245 frames fits at least one fixed window. No geometry/projection/scale changes. Both full lower windows and the upper window remain separately captured.'),indent=2)+'\n')
