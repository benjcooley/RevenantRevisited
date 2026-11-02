#!/usr/bin/env python3
"""Compiled production UV arithmetic and immutable own geometry against qualified original."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "riftqualifiedcore.h"
#include <cstdio>
#include <fstream>
#include <cassert>
int main(int argc,char**argv){assert(argc==2);float verts[32];std::ifstream f(argv[1],std::ios::binary);f.read(reinterpret_cast<char*>(verts),sizeof(verts));assert(f.gcount()==sizeof(verts));for(int t=1;t<=104;++t){float uv[2];rift_qualified_core::Offset(t,uv);printf("H %d %.9g %.9g\n",t,uv[0],uv[1]);if(t<100)for(int i=0;i<4;++i)printf("V %d %d %.9g %.9g\n",t,i,verts[i*8+6]+uv[0],verts[i*8+7]+uv[1]);}}
'''
def run(out,native):
 out.mkdir(parents=True,exist_ok=False)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:raw=z.read('Imagery/Misc/rift1.i3d')
 a=read_asset(raw);assert a['sha256']=='58fbb47a8d293744458363f73996e971223118a53e3c4abc76ddc9de17b1263f';fixture=out/'vertices.fixture';fixture.write_bytes(bytes.fromhex(a['objects'][0]['vertex_hex']));driver=out/'uv.cpp';driver.write_text(MAIN);binary=out/'uv';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);assert q.returncode==0 and not q.stderr;(out/'source.trace').write_text(q.stdout);uv={};heads={}
 for line in q.stdout.splitlines():
  p=line.split()
  if p[0]=='V':uv[int(p[1]),int(p[2])]=list(map(float,p[3:]))
  else:heads[int(p[1])]=list(map(float,p[2:]))
 n=json.loads(native.read_text());assert n['patched_original']and n['patch']['address']=='0x5c59b4';fields=0;maximum=0;errors=[];source_positions=list(struct.iter_unpack('<8f',fixture.read_bytes()))
 for r in n['records']:
  t=r['tick'];expected=1 if t<100 else 0
  if len(r['draws'])!=expected:errors.append(dict(tick=t,kind='draw_count'))
  for d in r['draws']:
   v=list(struct.iter_unpack('<8f',bytes.fromhex(d['vertices_hex'])));assert d['indices_hex']==a['face_hex'];assert len(v)==4
   for i,x in enumerate(v):
    err=max(abs(x[k+6]-uv[t,i][k])for k in(0,1));maximum=max(maximum,err);fields+=8
    if err>2e-6 or any(x[k]!=source_positions[i][k]for k in range(6)):errors.append(dict(tick=t,vertex=i,error=err))
 r=dict(status='pass'if not errors else'fail',accepted=False,qualified_modified_original=True,vertex_channels=fields,maximum_uv_error=maximum,errors=errors,source_offsets=heads,asset_sha256=sha(raw),native_manifest_sha256=sha(native.read_bytes()),source_core_sha256=sha((ROOT/'src/game/effects/riftqualifiedcore.h').read_bytes()),sanitizers='ASan/UBSan',scope='Compiled actual production offset preserves executed original row arithmetic and all4authored positions/normals/UVs/2triangles through99, realkill100. One qualified original parser-literal patch only; no unmodified retail claim. Negative/>1 V values and wrapped4diagonal appearances deliberately retained.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],fields,maximum,len(errors));return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);a=p.parse_args();run(a.output,a.native)
