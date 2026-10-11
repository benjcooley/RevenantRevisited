"""Actual narrow loader copy of five keys versus original barrier poses."""
import argparse
import json
import struct
import subprocess
import zipfile
from pathlib import Path
from barrier_profile_probe import DRIVER
from barrier_wall_native_preflight import ROOT, PROFILES, NativeBarrierWall
from speed_profile_contract import sha

COPY=r'''
#include <array>
std::array<SAniKey,5> Copy(const SOld3DImageryBody*mesh,bool barrier_keys){
 std::array<SAniKey,5> out{};
 for(int c=0;c<5;++c){
  bool ribbon_keys=false,vortex_keys=false,smoke_keys=false,fire_keys=false;int d=0;
  void*ptr=nullptr;int count=0;
  struct{void**anikeys;int*numanikeys;}obj{&ptr,&count};
  auto GetAniLength=[](int){return 1;};
  COPY_BRANCH
  memcpy(&out[c],ptr,8);delete[]static_cast<SAniKey*>(ptr);
 }
 return out;
}
'''
POSE=r'''
 auto keys=Copy(m,true);auto zeros=Copy(m,false);
 assert(vortex_legacy::Hash(zeros.data(),40)==vortex_legacy::Hash(std::array<SAniKey,5>{}.data(),40));
 for(int i=0;i<5;++i){const auto&k=keys[i];printf("POSE %d %.9g %.9g %.9g %.9g %.9g %.9g 1 1 1\n",i,float(ak_x(k))/ANIKEY_POSSCALE,float(ak_y(k))/ANIKEY_POSSCALE,float(ak_z(k))/ANIKEY_POSSCALE,float(ak_rx(k))/ANIKEY_ANGSCALE,float(ak_ry(k))/ANIKEY_ANGSCALE,float(ak_rz(k))/ANIKEY_ANGSCALE);}
'''


def run(output):
    output.mkdir(parents=True,exist_ok=False)
    source=(ROOT/'src/3dimage.cpp').read_text()
    a=source.index('obj.anikeys[d] = new SAniKey[GetAniLength(d)];')
    branch=source[a:source.index('\n            }',a)]
    driver=DRIVER.replace('int main(int argc',COPY.replace('COPY_BRANCH',branch)+'\nint main(int argc')
    driver=driver.replace('assert(matches(path,body.size(),1,1,0x2001));',
                          'assert(matches(path,body.size(),1,1,0x2001));'+POSE,1)
    cpp=output/'guard.cpp';cpp.write_text(driver);binary=output/'guard'
    q=subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',
                      '-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),
                      str(cpp),'-o',str(binary)],capture_output=True,text=True)
    (output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
    cases=[]
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        for profile in PROFILES:
            name,_,_,_,digest=profile
            data=archive.read(next(n for n in archive.namelist()
                                  if n.lower()=='imagery/misc/'+name.lower()+'.i3d'))
            asset=output/(name+'.fixture');asset.write_bytes(data)
            q=subprocess.run([str(binary),str(asset),'misc/'+name.lower()+'.i3d'],capture_output=True,text=True)
            (output/(name+'.log')).write_text(q.stdout+q.stderr);assert q.returncode==0,q.stdout+q.stderr
            poses={int(p[1]):list(map(float,p[2:])) for line in q.stdout.splitlines()
                   if (p:=line.split())[0]=='POSE'}
            f=NativeBarrierWall(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,profile)
            v=f.vm;vectors=v.allocate(36);maximum=0
            for i in range(5):
                assert v.call(0x409950,(i,0,0,vectors,vectors+12,vectors+24),this=f.imagery)==1
                native=list(struct.unpack('<9f',v.uc.mem_read(vectors,36)))
                maximum=max(maximum,max(abs(a-b)for a,b in zip(poses[i],native)))
            assert maximum<1e-6,maximum
            cases.append(dict(name=name,pose_channels=45,max_error=maximum,asset_sha256=digest))
    report=dict(status='pass',cases=cases,loader_branch_sha256=sha(branch.encode()),
                scope='Actual production loader branch copies five eight-byte keys, independently compared with original409950 on both resources. ASan/UBSan complete-body negatives and unchanged zero-key default controls. Full Render matrices/UV/Metal remain separate.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS90pose channels')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    run(p.parse_args().output)
