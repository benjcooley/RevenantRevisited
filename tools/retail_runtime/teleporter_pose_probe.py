"""Compile actual Teleporter null-spell core against genuine loaded full draws."""
import argparse,json,subprocess,struct
from pathlib import Path
from character_native_preflight import ROOT,sha

def run(native,output):
 output.mkdir(parents=True,exist_ok=False)
 driver=output/'pose.cpp';driver.write_text(r'''#include "teleportercore.h"
#include "retailsoftwaretransform.h"
#include <cstdio>
int main(){retail_teleporter::State s;for(int tick=1;tick<=60;++tick){s.Pulse();printf("S %d %d %d %d %d %.9g %d\n",tick,s.ticks,s.life,s.phase,s.iterations,s.rotation,int(s.alive));hmm_mat4 owner;BuildRetailSoftwareOwner(owner,0,0,0,224);s.Render([&](int i,const hmm_mat4&local){hmm_mat4 m;MtxMultiply(&m,const_cast<hmm_mat4*>(&local),&owner);printf("D %d %d",tick,i);for(int j=0;j<16;++j)printf(" %.9g",(&m.Elements[0][0])[j]);puts("");});}}''')
 binary=output/'pose';subprocess.run(['clang++','-std=c++20','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True)
 text=subprocess.check_output([str(binary)],text=True);(output/'source.txt').write_text(text);states={};draws={}
 for line in text.splitlines():
  v=line.split();tick=int(v[1])
  if v[0]=='S':states[tick]=list(map(float,v[2:]))
  else:draws.setdefault(tick,[]).append(list(map(float,v[3:])))
 rows=json.loads(native.read_text())['records'];checks=0;maximum=0;errors=[]
 for r in rows:
  st=r['state'];a=st['custom_state'];want=[a[17],st['owner_tail'][3],st['owner_tail'][4],a[16],struct.unpack('<f',struct.pack('<I',a[0]))[0],int(not st['flags']&4096)]
  for i,(x,y)in enumerate(zip(states[r['tick']],want)):
   if abs(x-y)>2e-6:errors.append([r['tick'],'state',i,x,y])
  actual=draws.get(r['tick'],[])
  if len(actual)!=len(r['draws']):errors.append([r['tick'],'draw_count',len(actual),len(r['draws'])]);continue
  for x,d in zip(actual,r['draws']):
   err=max(abs(a-b)for a,b in zip(x,d['matrix']));checks+=16;maximum=max(maximum,err)
   if err>4e-5:errors.append([r['tick'],'matrix',err])
 report=dict(status='pass'if not errors else'fail',native_sha256=sha(native),core_sha256=sha(ROOT/'src/teleportercore.h'),matrix_channels=checks,lifecycle_channels=len(rows)*6,maximum_matrix_error=maximum,errors=errors);(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert not errors
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
