"""Actual Funnel owner motion/lifetime versus the production adapter advance body."""
import argparse,json,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,sha,function
PRE=r'''
#include <cmath>
#include <cstdio>
#include <memory>
#include <cstdint>
struct S3DPoint{int x=0,y=0,z=0;};
constexpr unsigned OF_KILL=4096,OF_PULSE=32768;
struct TTime{static constexpr double LegacyFrameSeconds=1.0/24.0;};
void ConvertToVector(int face,int speed,S3DPoint&v){if(face!=64 || speed!=4)std::abort();v={4,0,0};}
struct Owner{S3DPoint pos{-180,0,78};int state=0,frame=0,face=64;bool done=false;unsigned flags=16385;
unsigned GetFlags(){return flags;}void SetFlags(unsigned x){flags=x;}void NextFrame(){int n=state==1?30:10;if(++frame>=n){frame=n-1;done=true;}}
int GetFace(){return face;}void Face(int x){face=x;}int GetState(){return state;}void SetState(int x){state=x;frame=0;done=false;}bool CommandDone(){return done;}
S3DPoint Pos(){return pos;}void ForcePos(S3DPoint p){pos=p;}void SetFrame(int x){frame=x;}void SetCommandDone(bool b){done=b;}void Pulse(){}
};
struct State{std::unique_ptr<Owner>owner{new Owner};S3DPoint source{-180,0,78},target{180,0,78},velocity{};int counter=0,ground_z=0;double accumulated=0;
GROUND
};
'''
def run(trace,out):
 out.mkdir(parents=True,exist_ok=True);s=(ROOT/'src/game/effects/legacy/funneloriginal.cpp').read_text();advance=function(s,'void Advance(');ground=function(s,'    int Ground(');driver=PRE.replace('GROUND',ground)+advance+r'''
int main(){State s;for(int i=0;i<149;++i){Advance(&s,1.0/24.0);auto&o=*s.owner;printf("%d %d %d %d %d %d %d %d\n",i,o.state,o.frame,s.counter,o.pos.x,o.pos.y,o.pos.z,int(bool(o.flags&OF_KILL)));}}
'''
 p=out/'owner.cpp';p.write_text(driver);exe=out/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',str(p),'-o',str(exe)],capture_output=True,text=True);(out/'compile.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('compile failed')
 q=subprocess.run([str(exe)],capture_output=True,text=True);(out/'source.trace').write_text(q.stdout);assert q.returncode==0,q.stderr
 rows=[list(map(int,l.split()))for l in q.stdout.splitlines()];native=json.loads(trace.read_text())['records'];errors=[]
 for r,p in zip(native,rows):
  a=r['after'];expected=[r['tick'],a['state'],a['frame'],a['counter'],*a['position'],int(bool(a['flags']&4096))]
  if expected!=p:errors.append(dict(tick=r['tick'],native=expected,source=p))
 report=dict(status='fail'if errors else'pass',ticks=min(len(rows),len(native)),fields=min(len(rows),len(native))*8,errors=errors,advance_sha256=sha(advance.encode()),ground_sha256=sha(ground.encode()),trace_sha256=sha(trace.read_bytes()),scope='Production Advance and terrain query extracted unchanged. Independent10/30/10 NextFrame adapter; face64 original4-unit velocity input. Full native reference executes original NextFrame/SetState/owner Pulse. Damage calls remain external.')
 (out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
