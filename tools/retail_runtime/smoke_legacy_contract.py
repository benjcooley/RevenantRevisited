#!/usr/bin/env python3
"""Own Smoke old-body admission/copy and original all10 legacy keys."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from smoke_visual_reference import NativeSmoke,ROOT,ASSET_SHA,sha
MAIN=r'''
#include "smokelegacykeys.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
using namespace smoke_legacy;
std::array<SAniKey,10> Copy(const SOld3DImageryBody*mesh,bool smoke_keys){std::array<SAniKey,10>out{};int offset=0;for(int d=0;d<3;++d){int c=0;bool ribbon_keys=false,vortex_keys=false,barrier_keys=false,fire_keys=false;void*ptr[3]={};int count[3]={};struct{void**anikeys;int*numanikeys;}obj{ptr,count};auto GetAniLength=[](int state){return state==2?8:1;};COPY_BRANCH memcpy(out.data()+offset,ptr[d],size_t(GetAniLength(d))*8);offset+=GetAniLength(d);delete[]static_cast<SAniKey*>(ptr[d]);}return out;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});uint32_t rel;memcpy(&rel,b.data()+16,4);std::vector<unsigned char>raw(b.begin()+20+rel,b.end());auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());auto match=[&](auto*p,size_t n){return Matches("misc/smoke.i3d",p,n,3,1,1,8,0x2000);};assert(match(m,raw.size()));auto keys=Copy(m,true);int offset=0;for(int state=0;state<3;++state)for(int frame=0;frame<(state==2?8:1);++frame){const auto&k=keys[offset++];printf("POSE %d %d %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",state,frame,float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);}
for(auto path:{"misc/fire.i3d","misc/smoke.i3d.bak","magic/smoke.i3d",""})assert(!Matches(path,m,raw.size(),3,1,1,8,0x2000));auto zero=Copy(m,false);for(auto&k:zero)assert(k.data[0]==0&&k.data[1]==0);
for(int n:{0,1,7131})assert(!match(m,n));assert(!match(static_cast<SOld3DImageryBody*>(nullptr),raw.size()));for(int field=0;field<5;++field)assert(!Matches("misc/smoke.i3d",m,raw.size(),field==0?2:3,field==1?2:1,field==2?2:1,field==3?9:8,field==4?0x2001:0x2000));
int rejects=0;auto reject=[&](auto mutate){auto copy=raw;auto*bad=reinterpret_cast<SOld3DImageryBody*>(copy.data());mutate(bad,copy);assert(!match(bad,copy.size()));++rejects;};
for(int state=0;state<3;++state)for(int i=0;i<(state==2?64:8);++i)reject([&](auto*m,auto&){static_cast<unsigned char*>(m->anikeys[state].ptr())[i]^=1;});
for(int i=0;i<128;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->verts[0].ptr());static_cast<unsigned char*>(t[0].ptr())[i]^=1;});for(int i=0;i<12;++i)reject([&](auto*m,auto&){static_cast<unsigned char*>(m->faces.ptr())[i]^=1;});for(int i=0;i<80;++i)reject([&](auto*m,auto&){reinterpret_cast<unsigned char*>(m->material)[i]^=1;});
for(int i=0;i<8192;++i)reject([&](auto*m,auto&){auto*t=static_cast<OFFSET*>(m->texturebits[0].ptr());static_cast<unsigned char*>(t[0].ptr())[i]^=1;});for(int state=0;state<3;++state)reject([&](auto*m,auto&b){m->anikeys[state].set(b.data()+b.size()-4);});reject([](auto*m,auto&){m->verts[0]=0u;});reject([](auto*m,auto&){m->faces=0u;});reject([](auto*m,auto&){m->texturebits[0]=0u;});printf("REJECTS %d\n",rejects);}
'''
def run(out):
 out.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();a=source.index('obj.anikeys[d] = new SAniKey[GetAniLength(d)];');b=source.index('\n            }',a);branch=source[a:b];driver=out/'guard.cpp';driver.write_text(MAIN.replace('COPY_BRANCH',branch));binary=out/'guard';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert not q.returncode
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/smoke.i3d'))
 fixture=out/'Smoke.fixture';fixture.write_bytes(data);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(out/'run.log').write_text(q.stdout+q.stderr);assert not q.returncode and not q.stderr;poses={(int(p[1]),int(p[2])):list(map(float,p[3:]))for line in q.stdout.splitlines()if(p:=line.split())[0]=='POSE'};f=NativeSmoke(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);v=f.vm;vectors=v.allocate(36);maximum=0
 for (state,frame),pose in poses.items():
  assert v.call(0x409950,(0,state,frame,vectors,vectors+12,vectors+24),this=f.imagery)==1;n=struct.unpack('<9f',v.uc.mem_read(vectors,36));maximum=max(maximum,max(abs(a-b)for a,b in zip(n,pose)));assert maximum<1e-7
 report=dict(status='pass',pose_channels=90,max_error=maximum,mutation_rejections=int(q.stdout.split('REJECTS ')[1]),default_zero_unchanged=True,source_sha256=sha((ROOT/'src/game/effects/smokelegacykeys.h').read_bytes()),loader_branch_sha256=sha(branch.encode()),scope='Exact own body/key/geometry/texture admission, actualproduction loadercopy underASan/UBSan, all10keys vsoriginal409950; otheroldbody defaultzero unchanged. No generic legacy policy broadening.');(out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
