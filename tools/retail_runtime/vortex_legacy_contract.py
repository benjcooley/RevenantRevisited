#!/usr/bin/env python3
"""Actual Vortex legacy admission/keycopy, native pose and default-policy controls."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from vortex_native_preflight import NativeVortex,ROOT,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include "vortexlegacykeys.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace vortex_legacy;
uint32_t U(const std::vector<unsigned char>&b,size_t p){uint32_t v;memcpy(&v,b.data()+p,4);return v;}
std::array<SAniKey,3> Copy(const SOld3DImageryBody*mesh,bool vortex_keys){std::array<SAniKey,3>out{};for(int c=0;c<3;++c){bool ribbon_keys=false,smoke_keys=false,barrier_keys=false,fire_keys=false;int d=0;void*ptr=nullptr;int count=0;struct {void**anikeys;int*numanikeys;} obj{&ptr,&count};auto GetAniLength=[](int){return 1;};COPY_BRANCH memcpy(&out[c],ptr,8);delete[] static_cast<SAniKey*>(ptr);}return out;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});size_t start=20+U(b,16);std::vector<unsigned char>raw(b.begin()+start,b.end());auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());assert(Matches("magic/vortex.i3d",m,raw.size(),1,1,0x2001));puts("accept literal");auto decoded=Copy(m,true);for(int i=0;i<3;++i){auto&k=decoded[i];printf("POSE %d %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",i,float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);}
for(const char*p:{"", "magic/ribbon.i3d", "misc/flies.i3d", "magic/vortex.i3d.bak"}){assert(!Matches(p,m,raw.size(),1,1,0x2001));auto zeros=Copy(m,false);assert(Hash(zeros.data(),24)==Hash(std::array<SAniKey,3>{}.data(),24));puts("reject default_oldbody");}
for(int n:{0,1,7131}){assert(!Matches("magic/vortex.i3d",m,n,1,1,0x2001));puts("reject size");}assert(!Matches(nullptr,m,raw.size(),1,1,0x2001));assert(!Matches("magic/vortex.i3d",nullptr,raw.size(),1,1,0x2001));for(int i=0;i<3;++i){assert(!Matches("magic/vortex.i3d",m,raw.size(),i==0?2:1,i==1?2:1,i==2?0x2000:0x2001));puts("reject header");}
for(const char*p:{"MAGIC/VORTEX.I3D","magic\\vortex.i3d","/assets/imagery/magic/vortex.i3d"}){assert(Matches(p,m,raw.size(),1,1,0x2001));puts("accept path");}
auto reject=[&](auto mutate){auto bytes=raw;auto*bad=reinterpret_cast<SOld3DImageryBody*>(bytes.data());mutate(bad,bytes);assert(!Matches("magic/vortex.i3d",bad,bytes.size(),1,1,0x2001));puts("reject field");};
for(int i=0;i<6;++i)reject([&](auto*m,auto&){int32_t*fields[]={reinterpret_cast<int32_t*>(&m->flags),&m->numverts,&m->numfaces,&m->nummaterials,&m->numtextures,&m->numobjects};++*fields[i];});
for(int i=0;i<3;++i){reject([&](auto*m,auto&){m->objname[i][0]='?';});reject([&](auto*m,auto&){++m->objmaterial[i];});reject([&](auto*m,auto&){++m->objvertpos[i];});reject([&](auto*m,auto&){++m->objvertnum[i];});for(int t=0;t<2;++t){reject([&](auto*m,auto&){++m->objfacepos[i][t];});reject([&](auto*m,auto&){++m->objfacenum[i][t];});}}
for(int i=0;i<6;++i)reject([&](auto*m,auto&){static_cast<uint32_t*>(m->anikeys[0].ptr())[i]^=0x100;});for(int i=0;i<205*8;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->verts[0].ptr());static_cast<float*>(t[0].ptr())[i]+=.03125f;});for(int i=0;i<304*3;++i)reject([&](auto*m,auto&){static_cast<uint16_t*>(m->faces.ptr())[i]^=1;});for(int i=0;i<160/4;++i)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(m->material)[i]^=0x100;});reject([](auto*m,auto&){++m->textureframes[0];});reject([](auto*m,auto&){++m->texturedesc[0].width;});reject([](auto*m,auto&){++m->texturedesc[0].height;});for(int i=0;i<5;++i)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(&m->texturedesc[0].pixelFormat.dwRGBBitCount)[i]^=1;});for(int i=0;i<1024;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->texturebits[0].ptr());static_cast<uint16_t*>(t[0].ptr())[i]^=1;});reject([](auto*m,auto&){m->texturebits[0]=0u;});reject([](auto*m,auto&b){m->texturebits[0].set(b.data()+b.size()-2);});reject([](auto*m,auto&){m->anikeys[0]=0u;});reject([](auto*m,auto&b){m->anikeys[0].set(b.data()+b.size()-8);});reject([](auto*m,auto&){m->faces=0u;});reject([](auto*m,auto&){m->verts[0]=0u;});}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();a=source.index('obj.anikeys[d] = new SAniKey[GetAniLength(d)];');b=source.index('\n            }',a);branch=source[a:b];driver=output/'guard.cpp';driver.write_text(MAIN.replace('COPY_BRANCH',branch));binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/vortex.i3d'))
 fixture=output/'Vortex.fixture';fixture.write_bytes(data);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;poses={int(p[1]):list(map(float,p[2:]))for l in q.stdout.splitlines()if(p:=l.split())[0]=='POSE'};f=NativeVortex(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;vectors=v.allocate(36);maximum=0
 for i in range(3):
  assert v.call(0x409950,(i,0,0,vectors,vectors+12,vectors+24),this=f.imagery)==1;native=list(struct.unpack('<9f',v.uc.mem_read(vectors,36)));maximum=max(maximum,max(abs(a-b)for a,b in zip(native,poses[i])));assert maximum<1e-7
 report=dict(status='pass',accept=q.stdout.count('accept '),reject=q.stdout.count('reject '),native_pose_channels=27,max_pose_error=maximum,asset_sha256=ASSET_SHA,source_guard_sha256=sha((ROOT/'src/game/effects/vortexlegacykeys.h').read_bytes()),actual_loader_branch_sha256=sha(branch.encode()),sanitizers='ASan/UBSan',accepted=False,scope='Actual strict Vortex legacy keycopy branch and all payload/bounds/descriptor/header controls;3real8bytekeys versus original409950. Wrong otherlegacy paths preservezero-key policy. No globallegacyloading policy change.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['accept'],report['reject'],maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
