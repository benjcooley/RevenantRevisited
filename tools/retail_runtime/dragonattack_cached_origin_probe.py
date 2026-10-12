"""Genuine Dragon cached prototype matrix versus the production cache helper."""
import argparse,json,struct,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,sha
DRIVER=r'''
#include "dragonattackcache.h"
#include "authoredpartsys.cpp"
#include <cstdio>
#include <fstream>
int main(int argc,char**argv){if(argc!=2)return 2;std::ifstream f(argv[1],std::ios::binary);int n;f.read((char*)&n,4);retail_dragonattack::PrototypeOrigin c;
for(int t=0;t<n;++t){std::array<float,3> p{},s{};f.read((char*)p.data(),12);f.read((char*)s.data(),12);c.Authored(p,s);int samples;f.read((char*)&samples,4);
for(int j=0;j<samples;++j){float scale;f.read((char*)&scale,4);f.read((char*)p.data(),12);c.Sample(scale,authored_partsys::RenderPosition(p));}
printf("%.9g %.9g %.9g\n",c.value[0],c.value[1],c.value[2]);}return f?0:3;}
'''
def run(trace,poses,out):
 out.mkdir(parents=True,exist_ok=True);source=out/'cache.cpp';source.write_text(DRIVER);binary=out/'cache'
 subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)],check=True)
 keys={tuple(map(int,p[:3])):list(map(float,p[3:]))for l in poses.read_text().splitlines()if(p:=l.split())}
 records=json.loads(trace.read_text())['records'];raw=struct.pack('<i',len(records));initials=[];previous=set()
 for r in records:
  a=r['after'];pose=keys[a['state'],0,a['frame']];samples=[]
  for c in a['controllers']:
   identity=(c['state'],c['address'])
   if identity not in previous:
    inp=r['before']['pulse_inputs'][str(c['address'])];initials.append((r['tick'],[inp['origin'][i]-inp['position'][i]for i in range(3)]))
   if c['state']!=a['state'] or a['flags']&4096:continue
   if c is not a['controllers'][0]:continue
   b=bytes.fromhex(c['pool_hex'])
   for i in range(c['capacity']):
    o=i*252
    if struct.unpack_from('<i',b,o+228)[0]:samples.append((struct.unpack_from('<f',b,o+100)[0],*struct.unpack_from('<3f',b,o+184)))
  previous={(c['state'],c['address'])for c in a['controllers']}
  raw+=struct.pack('<6fi',*pose[:3],*pose[6:9],len(samples))+b''.join(struct.pack('<4f',*p)for p in samples)
 inp=out/'inputs.bin';inp.write_bytes(raw);q=subprocess.run([str(binary),str(inp)],check=True,capture_output=True,text=True);port=[list(map(float,l.split()))for l in q.stdout.splitlines()];errors=[]
 for r,p in zip(records,port):
  n=r['after']['object_matrices'][0][12:15]
  if max(abs(x-y)for x,y in zip(n,p))>.00002:errors.append(dict(tick=r['tick'],native=n,source=p))
 for t,n in initials:
  p=port[t-1]if t else[0,0,0]
  if max(abs(x-y)for x,y in zip(n,p))>.00002:errors.append(dict(tick=t,initial=n,source=p))
 result=dict(status='fail'if errors else'pass',fields=len(records)*3+len(initials)*3,errors=errors,initials=initials,trace_sha256=sha(trace.read_bytes()),source_sha256=sha((ROOT/'src/dragonattackcache.h').read_bytes()))
 (out/'manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(result['status'],result['fields'],errors[:3]);return result
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--poses',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.poses,a.output)
