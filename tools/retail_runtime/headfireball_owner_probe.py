"""Compare the compiled single-child core to the complete original fire-head trap Tick."""
import argparse,json,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,sha
DRIVER=r'''
#include "headfireballcore.h"
#include <cstdio>
int main(){retail_headfireball::State s;s.Initialize(0,60,78);for(int t=0;t<110;++t){s.Pulse();printf("%d %d %d %d %d %d %d %d %d %d\n",t,s.state,s.frame,s.x,s.y,s.z,s.velocity_x,s.velocity_y,s.velocity_z,int(s.alive));}}
'''
def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);source=output/'owner.cpp';source.write_text(DRIVER);binary=output/'owner'
 subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
 rows=json.loads(trace.read_text())['records'];q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);native=[]
 for r in rows:
  s=r['after'];native.append([r['tick'],s['state'],s['frame'],*s['position'],*s['velocity'],int(not(s['flags']&4096))])
 port=[[int(v)for v in l.split()]for l in q.stdout.splitlines()];assert native==port
 report=dict(status='pass',rows=len(rows),integer_fields=sum(map(len,native)),kill_record=next(r['tick']for r in rows if r['after']['flags']&4096),trace_sha256=sha(trace.read_bytes()),source_sha256=sha((ROOT/'src/headfireballcore.h').read_bytes()),scope='Original RunFireHeadN single-child caller, face0, declared character-query planeY=-60. Actual launch/frame wrap/8-unit movement/impact/end-frame54 kill. Stat arming, natural two-child scheduling, character damage and arbitrary targets remain interfaces.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
