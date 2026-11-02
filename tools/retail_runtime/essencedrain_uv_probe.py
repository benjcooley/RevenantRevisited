#!/usr/bin/env python3
"""Exact live three-cylinder scroll cache against compiled production UV offset."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from essencedrain_native_preflight import NativeEssence,ROOT,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include <cstdio>
struct SHelperMeshSubmit{float uv_offset[2]{};};struct Object{int keys;};struct State{int renders;};
int main(){State holder;auto*s=&holder;Object profile{49};SHelperMeshSubmit draw;for(int tick=1;tick<200;++tick){s->renders=tick;UV_BRANCH printf("%d %.9g\n",tick,draw.uv_offset[0]);}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/essencedrainpreview.cpp').read_text();a=source.index('if(profile.keys)draw.uv_offset[0]=');b=source.index(';',a)+1;branch=source[a:b];driver=output/'uv.cpp';driver.write_text(MAIN.replace('UV_BRANCH',branch));binary=output/'uv';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary)],capture_output=True,text=True);assert q.returncode==0 and not q.stderr;offsets={int(p[0]):float(p[1])for l in q.stdout.splitlines()if(p:=l.split())}
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/worm.i3d'))
 f=NativeEssence(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);init=f.initialize();f.raster_enabled=False;v=f.vm;fields=0;maximum=0;errors=[];rows=[]
 for tick in range(1,202):
  f.step();_,r=f.render_whole()
  if tick>=200:assert not r['draws'];continue
  for o in(0,27,28):
   obj=f.animobjs[o];ptr=v.u32(obj+0xa4);verts=list(struct.iter_unpack('<8f',bytes.fromhex(f.asset['objects'][o]['vertex_hex'])));native=list(struct.iter_unpack('<8f',v.uc.mem_read(ptr,len(verts)*32)));uv=[]
   for i,(orig,cur)in enumerate(zip(verts,native)):
    source_u=struct.unpack('<f',struct.pack('<f',orig[6]+offsets[tick]))[0];delta=max(abs(cur[6]-source_u),abs(cur[7]-orig[7]));fields+=2;maximum=max(maximum,delta);uv.append([cur[6],cur[7]])
    if delta>2e-5:errors.append(dict(tick=tick,object=o,vertex=i,error=delta))
   rows.append(dict(tick=tick,object=o,uv=uv))
 report=dict(status='pass'if not errors else'fail',accepted=False,asset_sha256=ASSET_SHA,fields=fields,max_error=maximum,tolerance=2e-5,errors=errors,actual_source_uv_span_sha256=sha(branch.encode()),sanitizers='ASan/UBSan',native_initialization=init,rows=rows,scope='Original full default Init/scroll controller CopyVertices/owner Pulse/wholeRender everylive tick1–199 andkill200, all144actual cylinder vertices. Source compiled actual uniform UV offset plus authored UV versus original per-vertex float accumulation. Tiny float accumulation residual remains measured, not numeric/pixel equality; unchanged26zero-key auxiliaries do not receive scroll. Original pixels/actualMetal separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
