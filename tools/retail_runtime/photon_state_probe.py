"""Compiled production Photon state versus complete original animator, both launch cadences."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from photon_visual_reference import PhotonScene,ROOT,sha

DRIVER=r'''
#include <cstdio>
#include "photoncore.h"
uint32_t seed=1,draws=0;
int random_range(int lo,int hi){++draws;seed=seed*214013u+2531011u;return lo+int((seed>>16)&32767u)%(hi-lo+1);}
void emit(const photon_core::State&s){
 std::printf("{\"seed\":%u,\"rng_draws\":%u,\"active\":%d,\"oldstate\":%d,\"ready\":%d",seed,draws,s.active,s.old_state,int(s.ready));
 for(int a=0;a<2;++a){std::printf(",\"%s\":[",a?"v":"p");for(int i=0;i<32;++i){auto v=a?s.velocity[i]:s.position[i];std::printf("%s[%.9e,%.9e,%.9e]",i?",":"",v.X,v.Y,v.Z);}std::printf("]");}
 std::printf(",\"scale\":[");for(int i=0;i<32;++i)std::printf("%s%.9e",i?",":"",s.scale[i]);
 std::printf("],\"frame\":[");for(int i=0;i<32;++i)std::printf("%s%d",i?",":"",s.frame[i]);std::puts("]}");
}
int main(int argc,char**argv){photon_core::State s;s.Initialize(random_range);if(argc>1)s.Animate(0,{0,0,0},random_range);emit(s);int state,x,y,z;while(std::scanf("%d%d%d%d",&state,&x,&y,&z)==4){s.Animate(state,{x,y,z},random_range);emit(s);}}
'''
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 effect=(ROOT/'src/effect.cpp').read_text();assert 'particles_.Initialize('in effect and 'particles_.Animate(missile_.state,'in effect
 source=a.output/'driver.cpp';source.write_text(DRIVER);binary=a.output/'driver'
 subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/photon.i3d')
 cases=[];errors=[];checks=0
 for first in (False,True):
  f=PhotonScene(a.executable,data,first);original=[f.initial];motions=[]
  for tick in range(101):motions.append(f.step());original.append(f.state())
  inputs=''.join(' '.join(map(str,[m['state'],*m['velocity_fixed']]))+'\n'for m in motions)
  port=[json.loads(line)for line in subprocess.check_output([str(binary)]+(['charge']if first else[]),input=inputs,text=True).splitlines()]
  assert len(original)==len(port)
  for tick,(n,c)in enumerate(zip(original,port)):
   for field in ('seed','rng_draws','active','oldstate','ready','p','v','scale','frame'):
    def check(x,y,path):
     nonlocal checks
     if isinstance(x,list):
      assert len(x)==len(y)
      for i,(xx,yy)in enumerate(zip(x,y)):check(xx,yy,path+[i])
     else:
      checks+=1;equal=struct.pack('<f',x)==struct.pack('<f',y)if field in('p','v','scale')else x==y
      if not equal:errors.append(dict(animate_first=first,tick=tick,field=field,path=path,native=x,port=y))
    check(n[field],c[field],[])
  cases.append(dict(animate_first=first,original=original,port=port,motion=motions))
 report=dict(status='pass'if not errors else'differences',checks=checks,errors=errors,cases=cases,executable_sha256=sha(a.executable.read_bytes()),asset_sha256=__import__('hashlib').sha256(data).hexdigest(),source_sha256={x:sha((ROOT/x).read_bytes())for x in('src/photoncore.h','src/effect.cpp','src/effect.h')},scope='Complete original 5104c0/510600 state versus compiled production helper; both explicit Pulse-first and one-Animate-before-Pulse cadences. Shared original missile supplies movement; natural spell/reaper/audio remain outside fixture.')
 (a.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,'errors',len(errors));print(json.dumps(errors[:12],indent=2))
 if errors:raise SystemExit(1)
if __name__=='__main__':main()
