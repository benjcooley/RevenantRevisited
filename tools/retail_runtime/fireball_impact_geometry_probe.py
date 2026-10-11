"""Compare compiled impact geometry/colors with full original RenderObject packets."""
import argparse,json,struct,subprocess,zipfile
from speed_controller_preflight import sha
from pathlib import Path
R=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--effect',choices=['FireBall','YFireBall'],default='FireBall');p.add_argument('--fireball-palette-control',action='store_true');args=p.parse_args()
P=args.output;P.mkdir(parents=True,exist_ok=True)
s=P/'geometry.cpp';s.write_text(r'''
#include <cstdio>
#include "fireballquad.h"
#include "fireballring.h"
#include "retailsoftwaretransform.h"
int main(int argc,char**argv){std::array<hmm_vec3,4> v;FILE*f=fopen(argv[1],"rb");for(auto&p:v){float r[8];fread(r,4,8,f);p={r[0],r[1],r[2]};}fclose(f);int mode,frame,spin,face,x,y,z;float scale,px,py,pz;
while(scanf("%d %d %d %d %f %f %f %f %d %d %d",&mode,&frame,&spin,&face,&scale,&px,&py,&pz,&x,&y,&z)==11){
 if(mode==2){for(int i=0;i<4;++i)printf("%u ",fireball_ring::PackedColor(i,scale,argc>2));puts("");continue;}
 hmm_mat4 owner;BuildRetailSoftwareOwner(owner,x,y,z,face);hmm_vec3 pos{px,py,pz};auto q=mode?fireball_quad::BuildBurstGlow(v,frame,scale,pos,owner):fireball_quad::Build(v,frame,spin,face,scale,pos,owner);
 for(auto&p:q.position)printf("%.9g %.9g %.9g ",p.X,p.Y,p.Z);puts("");}}
''')
cmd=['clang++','-std=c++17','-O1','-I',str(R/'thirdparty/handmademath'),'-iquote',str(R/'src'),str(s),str(R/'src/fireballquad.cpp'),str(R/'src/math3d.cpp'),'-o',str(P/'geometry')];subprocess.check_call(cmd)
with zipfile.ZipFile(R/'data/imagery.rvi')as z:asset=z.read('Imagery/Magic/Yfireball.I3D' if args.effect=='YFireBall' else 'Imagery/Magic/newfireball.i3d')
from fireball_visual_reference import read_fireball
raw=bytes.fromhex(read_fireball(asset)['objects'][0]['vertex_hex'])
(P/'quad.bin').write_bytes(raw);vertices=list(struct.iter_unpack('<8f',raw));trace=json.loads(args.trace.read_text());inputs=[];expected=[]
for row in trace:
 if row['motion']['state']!=2 or not row['draws']:continue
 burst=[b for b in row['state']['burst']if b['used']];draws=[d for d in row['draws']if d['object']==0];draws=draws[-2*len(burst):]if burst else[]
 for b,pair in zip(burst,zip(draws[::2],draws[1::2])):
  for glow,d in zip([1,0],pair):
   scale=b['scale']*b['glow']if glow else b['scale'];inputs.append(' '.join(map(str,[glow,3 if glow else int(b['frame']),b['rotation'],0,scale,*b['position'],*row['motion']['position']])))
   m=d['matrix'];expected.append(dict(kind='quad',tick=row['tick'],glow=glow,values=[sum(m[j*4+i]*v[j]for j in range(3))+m[12+i]for v in vertices for i in range(3)]))
 for d in row['draws']:
  if d['object']==2:
   inputs.append(' '.join(map(str,[2,0,0,0,row['state']['ring_scale'],0,0,0,0,0,0])));raw=bytes.fromhex(d['vertices_hex']);expected.append(dict(kind='color',tick=row['tick'],values=[struct.unpack_from('<I',raw,i*24*32+16)[0]for i in range(4)]))
actual=subprocess.check_output([str(P/'geometry'),str(P/'quad.bin')]+(['YFireBall']if args.effect=='YFireBall' and not args.fireball_palette_control else[]),input='\n'.join(inputs)+'\n',text=True).splitlines();errors=[];maximum=0;checks=0
for exp,line in zip(expected,actual):
 vals=list(map(float if exp['kind']=='quad'else int,line.split()))
 assert len(vals)==len(exp['values'])
 for a,b in zip(exp['values'],vals):
  checks+=1;delta=abs(a-b)
  if exp['kind']=='quad':maximum=max(maximum,delta)
  if delta>(.0002 if exp['kind']=='quad'else 0):errors.append(dict(**exp,actual=vals));break
assert len(actual)==len(expected) and len(expected)>0
report=dict(effect=args.effect,fireball_palette_control=args.fireball_palette_control,asset_sha256=sha(asset),status='pass' if not errors else 'differences_found',checks=checks,packets=len(expected),maximum_position_error=maximum,errors=errors,native_trace_sha256=sha(args.trace.read_bytes()),source_sha256={f:sha((R/f).read_bytes())for f in('src/fireballquad.cpp','src/fireballring.h')});(P/'geometry-report.json').write_text(json.dumps(report,indent=2));print('checks',checks,'packets',len(expected),'max',maximum,'errors',len(errors));print(errors[:1])

raise SystemExit(bool(errors))
