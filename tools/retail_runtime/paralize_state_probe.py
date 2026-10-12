#!/usr/bin/env python3
"""Compile independent named no-target Paralize1 core against complete original owner."""
import argparse,json,math,struct,subprocess
from pathlib import Path
from speed_profile_contract import ROOT,sha
DRIVER=r'''
#include "paralizecore.h"
#include <cstdio>
int main(){paralize_core::State state;unsigned seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767u)%(b-a+1);};
for(int t=0;t<32;++t){state.Tick(rng);unsigned alive=state.alive;fwrite(&seed,4,1,stdout);fwrite(&alive,4,1,stdout);fwrite(&state,0x3c4,1,stdout);}}
'''
def run(trace,output):
 output.mkdir(parents=True,exist_ok=True);src=output/'state.cpp';src.write_text(DRIVER);exe=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(src),'-o',str(exe)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 q=subprocess.run([str(exe)],capture_output=True);(output/'source.bin').write_bytes(q.stdout);(output/'run.log').write_bytes(q.stderr);assert q.returncode==0
 native=json.loads(trace.read_text());errors=[];maximum=0;checks=0
 float_ranges=[(0x184,0x3ac),(0x448,0x508)]
 for i,r in enumerate(native['records']):
  ref=r['state'];off=i*(0x3c4+8);seed,alive=struct.unpack_from('<II',q.stdout,off);assert seed==ref['seed'],(i,seed,ref['seed']);assert alive==int(not(ref['flags']&0x1000)),i
  expected=bytes.fromhex(ref['owner_hex']);current=q.stdout[off+8:off+8+0x3c4]
  for j in range(0,0x3c4,4):
   address=j+0x184;floating=any(lo<=address<hi for lo,hi in float_ranges);fmt='<f'if floating else'<I';a=struct.unpack_from(fmt,expected,j)[0];b=struct.unpack_from(fmt,current,j)[0];checks+=1
   if floating:
    if math.isnan(a)and math.isnan(b):continue
    error=abs(a-b);maximum=max(maximum,error);okay=error<=max(.003,abs(a)*2e-6)
   else:okay=a==b;error=abs(a-b)
   if not okay:errors.append(dict(tick=i+1,field=hex(address),native=a,source=b,error=error))
 controls=[]
 for label,old,new in [('snapshot_duration','count=240','count=200'),('snapshot_knot_step','knot-=.6f','knot-=.5f')]:
  header=(ROOT/'src/paralizecore.h').read_text();assert old in header;bad=output/(label+'.h');bad.write_text(header.replace(old,new));driver=output/(label+'.cpp');driver.write_text(DRIVER.replace('"paralizecore.h"','"'+bad.name+'"'));binary=output/label
  qbad=subprocess.run(['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);assert qbad.returncode==0,qbad.stderr
  qbad=subprocess.run([str(binary)],capture_output=True);assert qbad.returncode==0;differences=sum(a!=b for a,b in zip(q.stdout,qbad.stdout));assert differences>0;controls.append(dict(name=label,different_bytes=differences))
 report=dict(status='fail'if errors else'pass',fields=checks,maximum_float_error=maximum,error_count=len(errors),errors=errors[:100],source_sha256=sha((ROOT/'src/paralizecore.h').read_bytes()),native_trace_sha256=sha(trace.read_bytes()),sanitizers='ASan/UBSan',negative_controls=controls,accepted=False);(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],len(errors),maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('trace',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.trace,a.output)
