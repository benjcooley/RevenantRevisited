#!/usr/bin/env python3
"""Actual IrisFlare whole-resource admission and original legacy key controls."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from irisflare_native_probe import NativeIris,ROOT,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include "irisflareprofile.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace irisflare_profile;
unsigned U(const std::vector<unsigned char>&b,size_t p){unsigned v;memcpy(&v,b.data()+p,4);return v;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});size_t start=20+U(b,16);std::vector<unsigned char>raw(b.begin()+start,b.end());auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());assert(Matches("misc/irisflare.i3d",m,raw.size(),1,1,0x2001));puts("accept literal");
for(const char*p:{"MISC/IRISFLARE.I3D","misc\\irisflare.i3d","/assets/imagery/misc/irisflare.i3d"}){assert(Matches(p,m,raw.size(),1,1,0x2001));puts("accept path");}
for(const char*p:{"", "misc/teleporter.i3d", "magic/ribbon.i3d", "magic/vortex.i3d", "misc/irisflare.i3d.bak"}){assert(!Matches(p,m,raw.size(),1,1,0x2001));puts("reject path");}
for(size_t n:{size_t(0),size_t(7132),sizeof(SOld3DImageryBody)-1}){assert(!Matches("misc/irisflare.i3d",m,n,1,1,0x2001));puts("reject size");}assert(!Matches(nullptr,m,raw.size(),1,1,0x2001));assert(!Matches("misc/irisflare.i3d",nullptr,raw.size(),1,1,0x2001));
for(int i=0;i<3;++i){assert(!Matches("misc/irisflare.i3d",m,raw.size(),i==0?2:1,i==1?2:1,i==2?0x2000:0x2001));puts("reject state");}
const auto*keys=static_cast<const SAniKey*>(m->anikeys[0].ptr());for(int i=0;i<2;++i){auto&k=keys[i];printf("POSE %d %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",i,float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);}
auto reject=[&](auto mutate){auto bytes=raw;auto*bad=reinterpret_cast<SOld3DImageryBody*>(bytes.data());mutate(bad,bytes);assert(!Matches("misc/irisflare.i3d",bad,bytes.size(),1,1,0x2001));puts("reject field");};
for(int i=0;i<6;++i)reject([&](auto*m,auto&){int32_t*fields[]={reinterpret_cast<int32_t*>(&m->flags),&m->numverts,&m->numfaces,&m->nummaterials,&m->numtextures,&m->numobjects};++*fields[i];});
for(int i=0;i<2;++i){reject([&](auto*m,auto&){m->objname[i][0]='?';});reject([&](auto*m,auto&){++m->objmaterial[i];});reject([&](auto*m,auto&){++m->objvertpos[i];});reject([&](auto*m,auto&){++m->objvertnum[i];});reject([&](auto*m,auto&){*static_cast<int8_t*>(m->objparent[i].ptr())=0;});reject([&](auto*m,auto&){m->objparent[i]=0u;});for(int t=0;t<3;++t){reject([&](auto*m,auto&){++m->objfacepos[i][t];});reject([&](auto*m,auto&){++m->objfacenum[i][t];});}}
for(int i=0;i<4;++i)reject([&](auto*m,auto&){static_cast<uint32_t*>(m->anikeys[0].ptr())[i]^=0x100;});for(int i=0;i<186*8;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->verts[0].ptr());static_cast<float*>(t[0].ptr())[i]+=.03125f;});for(int i=0;i<204*3;++i)reject([&](auto*m,auto&){static_cast<uint16_t*>(m->faces.ptr())[i]^=1;});for(int i=0;i<160/4;++i)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(m->material)[i]^=0x100;});
for(int t=0;t<2;++t){reject([&](auto*m,auto&){++m->textureframes[t];});reject([&](auto*m,auto&){++m->texturedesc[t].width;});reject([&](auto*m,auto&){++m->texturedesc[t].height;});for(int i=0;i<5;++i)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(&m->texturedesc[t].pixelFormat.dwRGBBitCount)[i]^=1;});for(int i=0;i<4096;++i)reject([&](auto*m,auto&){auto*frames=static_cast<OFFSET*>(m->texturebits[t].ptr());static_cast<uint16_t*>(frames[0].ptr())[i]^=1;});reject([&](auto*m,auto&){m->texturebits[t]=0u;});reject([&](auto*m,auto&b){m->texturebits[t].set(b.data()+b.size()-2);});}
reject([](auto*m,auto&){m->anikeys[0]=0u;});reject([](auto*m,auto&b){m->anikeys[0].set(b.data()+b.size()-8);});reject([](auto*m,auto&){m->faces=0u;});reject([](auto*m,auto&){m->verts[0]=0u;});}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'profile.cpp';driver.write_text(MAIN);binary=output/'profile';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/irisflare.i3d')
 fixture=output/'asset.fixture';fixture.write_bytes(data);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;poses={int(p[1]):list(map(float,p[2:]))for l in q.stdout.splitlines()if(p:=l.split())[0]=='POSE'};f=NativeIris(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;p=v.allocate(36);maximum=0
 for i in range(2):
  assert v.call(0x409950,(i,0,0,p,p+12,p+24),this=f.imagery)==1;maximum=max(maximum,max(abs(a-b)for a,b in zip(struct.unpack('<9f',v.uc.mem_read(p,36)),poses[i])));assert maximum<1e-7
 report=dict(status='pass',accepted=False,accept=q.stdout.count('accept '),reject=q.stdout.count('reject '),native_pose_channels=18,max_pose_error=maximum,sanitizers='ASan/UBSan',source_sha256=sha((ROOT/'src/irisflareprofile.h').read_bytes()),asset_sha256=ASSET_SHA,scope='Actual production own-profile whole-resource matcher compiled with real legacy serialized types. Exact names/roots/materials/face runs, all16 raw key bytes/186vertices/204faces/materials and both complete ARGB4444 payloads, descriptor/range/default path mutations. Original409950 confirms18 legacy channels. No engine old-loader policy change; custom matrices override keys and omit authored POS flags. This is admission/key evidence, not appearance acceptance.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['accept'],report['reject'],maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
