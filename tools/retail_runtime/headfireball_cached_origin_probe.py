"""Actual cached-origin bridge versus original full/skipped/early-only Render."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,function,sha
DRIVER=r'''
#include <cstdio>
#include <fstream>
#include <cstdint>
#include "math3d.h"
struct Owner {int state=0;int GetState(){return state;}uint32_t ObjId(){return 0xd000f005u;}};
struct Image {bool HasRetailHeadFireBallProfile(){return true;}};
struct T3DAnimator {Owner owner;Owner*inst=&owner;Image image;bool headfireball_object_zero_sampled=false;hmm_vec3 headfireball_object_zero_origin{};Image*Get3DImagery(){return &image;}
void SetHeadFireBallObjectZeroOrigin(const hmm_vec3&);void NoteHeadFireBallPrototypeSample();};
METHODS
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream f(argv[1],std::ios::binary);int n;f.read((char*)&n,4);T3DAnimator a;
for(int t=0;t<n;++t){int render,sampled;f.read((char*)&a.owner.state,4);hmm_vec3 p{},s{};f.read((char*)&render,4);f.read((char*)&sampled,4);f.read((char*)&p,12);f.read((char*)&s,12);
if(render){if(s.X>1e-5f&&s.Y>1e-5f&&s.Z>1e-5f)a.SetHeadFireBallObjectZeroOrigin(p);if(sampled)a.NoteHeadFireBallPrototypeSample();}
const auto q=a.headfireball_object_zero_origin;printf("%.9g %.9g %.9g\n",q.X,q.Y,q.Z);}
return f?0:3;}
'''
def run(traces,poses,out):
 out.mkdir(parents=True,exist_ok=True);production=(ROOT/'src/3dimage.cpp').read_text();methods=[function(production,s)for s in['void T3DAnimator::SetHeadFireBallObjectZeroOrigin(','void T3DAnimator::NoteHeadFireBallPrototypeSample(']]
 src=out/'cached.cpp';src.write_text(DRIVER.replace('METHODS','\n'.join(methods)));exe=out/'cached';subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(src),'-o',str(exe)],check=True)
 keys={tuple(map(int,p[:3])):list(map(float,p[3:]))for l in poses.read_text().splitlines()if(p:=l.split())};cases=[]
 for path in traces:
  j=json.loads(path.read_text());rows=j['records'];raw=struct.pack('<i',len(rows));initials=[];laststate=0
  for r in rows:
   s=r['after'];pose=keys[s['state'],0,s['frame']];sampled=any(struct.unpack_from('<i',bytes.fromhex(c['pool_hex']),i*252+228)[0]for c in s['controllers'][:1]for i in range(c['capacity']))
   raw+=struct.pack('<3i6f',s['state'],int(r['rendered']),int(sampled),*pose[:3],*pose[6:9])
   if s['state']!=laststate and s['controllers']:
    c=s['controllers'][0];p=r['before']['pulse_inputs'][str(c['address'])];initials.append((r['tick'],[p['origin'][i]-p['position'][i]for i in range(3)]))
   laststate=s['state']
  inp=out/(path.parent.name+'.bin');inp.write_bytes(raw);q=subprocess.run([str(exe),str(inp)],capture_output=True,text=True,check=True);port=[list(map(float,l.split()))for l in q.stdout.splitlines()];errors=[]
  for r,current in zip(rows,port):
   if r['after']['state']==2:continue # no later Initialize consumes teardown prototype matrices
   expected=r['after']['object_matrices'][0][12:15]
   if max(abs(a-b)for a,b in zip(expected,current))>1e-6:errors.append(dict(tick=r['tick'],native=expected,source=current))
  for t,origin in initials:
   if max(abs(a-b)for a,b in zip(origin,port[t-1]if t else[0,0,0]))>1e-6:errors.append(dict(tick=t,initialize=origin,source=port[t-1]))
  cases.append(dict(trace=str(path),trace_sha256=sha(path.read_bytes()),fields=3*sum(r['after']['state']<2 for r in rows)+3*len(initials),initial_origins=initials,errors=errors,status='pass'if not errors else'fail'))
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,production_sha256=[sha(m.encode())for m in methods],pose_sha256=sha(poses.read_bytes()),scope='Original4033db/40efc0 reads cached prototype matrix;4026e1 marks manual transform. Full/stride3/early-only controls cover every outgoing-flight cached origin and all subsequent controller initialization inputs. Later teardown matrices are not consumed by another initialization and are not claimed equivalent. No fitted velocity.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],sum(c['fields']for c in cases));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('traces',type=Path,nargs='+');p.add_argument('--poses',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.traces,a.poses,a.output)
