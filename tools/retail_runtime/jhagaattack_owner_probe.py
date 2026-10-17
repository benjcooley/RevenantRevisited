"""Compare the compiled null-spell owner to complete original custom Pulse."""
import argparse,json,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,sha
DRIVER=r'''
#include "jhagaattackcore.h"
#include <cstdio>
int main(){retail_jhagaattack::State s;s.Initialize(0,60,78);for(int t=0;t<100;++t){s.Pulse();printf("%d %d %d %d %d %d %d\n",t,s.phase,s.counter,s.x,s.y,s.z,int(s.alive));}}
'''
def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);source=output/'owner.cpp';source.write_text(DRIVER);binary=output/'owner'
 subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
 rows=json.loads(trace.read_text())['records'];q=subprocess.run([str(binary)],capture_output=True,text=True,check=True);native=[]
 for r in rows:
  s=r['after'];native.append([r['tick'],s['phase'],s['counter'],*s['position'],int(not(s['flags']&4096))])
 port=[[int(v)for v in l.split()]for l in q.stdout.splitlines()];assert native==port
 report=dict(status='pass',rows=len(rows),integer_fields=sum(map(len,native)),kill_record=next(r['tick']for r in rows if r['after']['flags']&4096),trace_sha256=sha(trace.read_bytes()),source_sha256=sha((ROOT/'src/jhagaattackcore.h').read_bytes()),scope='Null spell/empty actor query, actual timed negativeY8-unit flight. No arbitrary facing, terrain impact or actor damage claim.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
