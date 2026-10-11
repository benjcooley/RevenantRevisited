#!/usr/bin/env python3
"""Actual narrow old-loader Ribbon key branch,18native channels and negatives."""
import argparse,json,subprocess,zipfile,struct
from pathlib import Path
from speed_profile_contract import sha
from ribbon_native_preflight import NativeRibbon,ROOT,ASSET_SHA
MAIN=r'''
#include "ribbonlegacykeys.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace ribbon_legacy;
uint32_t U(const std::vector<unsigned char>&b,size_t p){uint32_t v;memcpy(&v,b.data()+p,4);return v;}
std::array<SAniKey,2> Copy(const SOld3DImageryBody*mesh,bool ribbon_keys){std::array<SAniKey,2>out{};for(int c=0;c<2;++c){bool vortex_keys=false;int d=0;void*ptr=nullptr;int count=0;struct {void**anikeys;int*numanikeys;} obj{&ptr,&count};auto GetAniLength=[](int){return 1;};COPY_BRANCH memcpy(&out[c],ptr,8);delete[] static_cast<SAniKey*>(ptr);}return out;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});const size_t start=20+U(b,16);std::vector<unsigned char>raw(b.begin()+start,b.end());auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());assert(Matches("Magic/Ribbon.I3D",m,raw.size(),1,1,0x2001));puts("accept literal");auto decoded=Copy(m,true);for(int i=0;i<2;++i){auto&k=decoded[i];printf("POSE %d %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",i,float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);}
auto reject=[&](auto mutate){auto bytes=raw;auto*bad=reinterpret_cast<SOld3DImageryBody*>(bytes.data());mutate(bad,bytes);assert(!Matches("magic/ribbon.i3d",bad,bytes.size(),1,1,0x2001));puts("reject field");};
for(const char*p:{"", "magic/ribbonb.i3d", "misc/flies.i3d", "magic/ribbon.i3d.bak"}){assert(!Matches(p,m,raw.size(),1,1,0x2001));auto zeros=Copy(m,false);assert(Hash(zeros.data(),16)==Hash(std::array<SAniKey,2>{}.data(),16));puts("reject default_oldbody");}
assert(!Matches(nullptr,m,raw.size(),1,1,0x2001));assert(!Matches("magic/ribbon.i3d",nullptr,raw.size(),1,1,0x2001));for(int n:{0,1,7131})assert(!Matches("magic/ribbon.i3d",m,n,1,1,0x2001));assert(!Matches("magic/ribbon.i3d",m,raw.size(),2,1,0x2001));assert(!Matches("magic/ribbon.i3d",m,raw.size(),1,2,0x2001));assert(!Matches("magic/ribbon.i3d",m,raw.size(),1,1,0x2000));puts("reject null_header_range");
for(int i=0;i<3;++i){std::string p="magic/ribbon.i3d";if(i==0)for(char&c:p)c=char(std::toupper((unsigned char)c));if(i==1)for(char&c:p)if(c=='/')c='\\';if(i==2)p="/assets/imagery/"+p;assert(Matches(p.c_str(),m,raw.size(),1,1,0x2001));puts("accept path");}
reject([](auto*m,auto&){++m->flags;});reject([](auto*m,auto&){--m->numverts;});reject([](auto*m,auto&){--m->numfaces;});reject([](auto*m,auto&){--m->nummaterials;});reject([](auto*m,auto&){--m->numtextures;});reject([](auto*m,auto&){--m->numobjects;});for(int i=0;i<2;++i){reject([&](auto*m,auto&){m->objname[i][0]='?';});reject([&](auto*m,auto&){++m->objmaterial[i];});reject([&](auto*m,auto&){++m->objvertpos[i];});reject([&](auto*m,auto&){++m->objvertnum[i];});reject([&](auto*m,auto&){++m->textureframes[i];});reject([&](auto*m,auto&){--m->texturedesc[i].width;});reject([&](auto*m,auto&){--m->texturedesc[i].height;});for(int k=0;k<5;++k)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(&m->texturedesc[i].pixelFormat.dwRGBBitCount)[k]^=1;});}
for(int i=0;i<4;++i)reject([&](auto*m,auto&){static_cast<uint32_t*>(m->anikeys[0].ptr())[i]^=0x100;});for(int i=0;i<186*8;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->verts[0].ptr());static_cast<float*>(t[0].ptr())[i]+=.03125f;});for(int i=0;i<204*3;++i)reject([&](auto*m,auto&){static_cast<uint16_t*>(m->faces.ptr())[i]^=1;});for(int i=0;i<160/4;++i)reject([&](auto*m,auto&){reinterpret_cast<uint32_t*>(m->material)[i]^=0x100;});reject([](auto*m,auto&){m->anikeys[0]=0u;});reject([](auto*m,auto&b){m->anikeys[0].set(b.data()+b.size()-8);});reject([](auto*m,auto&){m->faces=0u;});reject([](auto*m,auto&){m->verts[0]=0u;});}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/3dimage.cpp').read_text();a=source.index('obj.anikeys[d] = new SAniKey[GetAniLength(d)];');b=source.index('\n            }',a);branch=source[a:b];driver=output/'guard.cpp';driver.write_text(MAIN.replace('COPY_BRANCH',branch));binary=output/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('compile failed')
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/ribbon.i3d'))
 asset=output/'Ribbon.fixture';asset.write_bytes(data);q=subprocess.run([str(binary),str(asset)],capture_output=True,text=True);(output/'trace').write_text(q.stdout);(output/'run.log').write_text(q.stderr)
 if q.returncode or q.stderr:raise RuntimeError('actualguard/keycopy negative failed')
 poses={int(p[1]):list(map(float,p[2:]))for l in q.stdout.splitlines()if(p:=l.split())[0]=='POSE'};f=NativeRibbon(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;vectors=v.allocate(36);maximum=0
 for i in range(2):
  assert v.call(0x409950,(i,0,0,vectors,vectors+12,vectors+24),this=f.imagery)==1;native=list(struct.unpack('<9f',v.uc.mem_read(vectors,36)));maximum=max(maximum,max(abs(a-b)for a,b in zip(native,poses[i])));assert maximum<1e-7
 report=dict(status='pass',accept=q.stdout.count('accept '),reject=q.stdout.count('reject '),native_pose_channels=18,max_pose_error=maximum,asset_sha256=ASSET_SHA,source_guard_sha256=sha((ROOT/'src/ribbonlegacykeys.h').read_bytes()),actual_loader_branch_sha256=sha(branch.encode()),driver_sha256=sha(driver.read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual narrow readonly Ribbon oldbody guard and extracted original productionOldInitializeMesh keycopy branch.16rawbytes/2oneframe8bytekeys vs executed original409950 pose retrieval; everypayload/descriptor/bounds/defaultotheroldpath control independent. No broadlegacyloader change.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['accept'],report['reject'],maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
