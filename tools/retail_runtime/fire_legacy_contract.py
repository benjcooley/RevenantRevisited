"""Strict shipped Fire old-key copy, original poses and current GPU matrices."""
import argparse,json,struct,subprocess,zipfile,re
from pathlib import Path
from speed_profile_contract import ROOT,sha
DRIVER=r'''
#include "firelegacykeys.h"
#include <cassert>
#include <fstream>
#include <vector>
#include <cstdio>
SAniKey Copy(const SOld3DImageryBody*mesh,bool fire_keys){int d=0,c=0;bool ribbon_keys=false,vortex_keys=false,smoke_keys=false,barrier_keys=false;void*ptr=nullptr;int count=0;struct{void**anikeys;int*numanikeys;}obj{&ptr,&count};auto GetAniLength=[](int){return 1;};COPY_BRANCH SAniKey result;memcpy(&result,ptr,8);delete[]static_cast<SAniKey*>(ptr);return result;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>file((std::istreambuf_iterator<char>(f)),{});uint32_t offset;memcpy(&offset,file.data()+16,4);std::vector<unsigned char>body(file.begin()+20+offset,file.end());assert(body.size()==277496);auto*m=reinterpret_cast<SOld3DImageryBody*>(body.data());
auto check=[&](const char*p,size_t n,int st,int fr,uint32_t flags){return fire_legacy::Matches(p,m,n,st,fr,flags);};for(auto path:{"misc/fire.i3d","MISC/FIRE.I3D","Misc\\Fire.I3D","/assets/imagery/misc/fire.i3d"})assert(check(path,body.size(),1,1,0x2001));
assert(!check("badmisc/fire.i3d",body.size(),1,1,0x2001));assert(!check("misc/fire.i3d.bak",body.size(),1,1,0x2001));assert(!check("misc/fire.i3d",body.size()-1,1,1,0x2001));assert(!check("misc/fire.i3d",body.size(),2,1,0x2001));assert(!check("misc/fire.i3d",body.size(),1,33,0x2001));assert(!check("misc/fire.i3d",body.size(),1,1,0x2000));assert(!check(nullptr,body.size(),1,1,0x2001));assert(!fire_legacy::Matches("misc/fire.i3d",nullptr,body.size(),1,1,0x2001));assert(!check("misc/fire.i3d",body.size()+1,1,1,0x2001));int guards=9;
for(size_t i=0;i<body.size();i+=512){body[i]^=1;assert(!check("misc/fire.i3d",body.size(),1,1,0x2001));body[i]^=1;++guards;}
auto k=Copy(m,true);auto z=Copy(m,false);unsigned char zero[8]={};assert(!memcmp(&z,zero,8));assert(!memcmp(&k,m->anikeys[0].ptr(),8));printf("pose %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);printf("guards %d\n",guards);
}
'''
def run(output,native,films):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/3dimage.cpp').read_text();a=source.index('obj.anikeys[d] = new SAniKey[GetAniLength(d)];');branch=source[a:source.index('\n            }',a)];cpp=output/'guard.cpp';cpp.write_text(DRIVER.replace('COPY_BRANCH',branch));binary=output/'guard'
 cmd=['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(cpp),'-o',str(binary)];q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/fire.i3d'))
 assert sha(data)=='70574ac537f9ec6f6a39df1adb449e969afa15691a9190c5b4450114d0ae3fee';asset=output/'asset';asset.write_bytes(data);q=subprocess.run([str(binary),str(asset)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stdout+q.stderr
 from fire_cardinal_native_probe import CardinalFire
 boot=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-wide-20261010/hard-astra/character-authentic/hud-boot-09c83243d43516b6.image');f=CardinalFire(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',boot,0);v=f.vm;vectors=v.allocate(36);assert v.call(0x409950,(0,0,0,vectors,vectors+12,vectors+24),this=f.imagery)==1;actual=list(struct.unpack('<9f',v.uc.mem_read(vectors,36)));pose=list(map(float,re.search(r'pose (.*)',q.stdout)[1].split()));delta=max(abs(a-b)for a,b in zip(actual,pose));assert delta<1e-6
 n=json.loads(native.read_text());checks=[]
 for face,manifest in films.items():
  m=json.loads(manifest.read_text());assert m['status']=='pass';rows=[r for r in n['records']if r['facing']==face];logged=re.findall(r'\[fire-installed-phase\] tick=(\d+) frame=(\d+) face=(\d+) rng=(\d+) matrix=([^\n]+)',(manifest.parent/'run.log').read_text());assert len(logged)==3
  for i,(tick,frame,facing,rng,matrix)in enumerate(logged):
   assert int(frame)==int(rng)==0 and int(facing)==face;current=list(map(float,matrix.split(',')));error=max(abs(a-b)for a,b in zip(current,rows[i]['draws'][0]['matrix']));assert error<1e-6;checks.append(dict(facing=face,tick=int(tick),matrix_error=error))
 report=dict(status='pass',profile_paths=4,profile_rejections=int(re.search(r'guards (\d+)',q.stdout)[1]),pose_fields=9,pose_max_error=delta,actual_gpu_matrix_fields=len(checks)*16,gpu_checks=checks,loader_branch_sha256=sha(branch.encode()),profile_sha256=sha((ROOT/'src/firelegacykeys.h').read_bytes()),native_sha256=sha(native.read_bytes()),asset_sha256=sha(data),sanitizers='ASan/UBSan',accepted=False,scope='Actual production strict oldbody profile and loader keycopy, real original409950 pose and actual fourcardinal sourceGPU matrix logs; original zero RNG/state/frame0. Raster/appearance remains separate, staticoneframe is not animatedfire.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['profile_rejections'],report['actual_gpu_matrix_fields']);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--native',type=Path,required=True);p.add_argument('--films',type=Path,required=True);a=p.parse_args();run(a.output,a.native,{int(k):Path(v)for k,v in json.loads(a.films.read_text()).items()})
