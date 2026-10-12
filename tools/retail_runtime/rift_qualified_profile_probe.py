#!/usr/bin/env python3
"""Compile the exact whole Rift resource matcher with ASan/UBSan."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "riftqualifiedprofile.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});assert(b.size()==33329);assert(rift_qualified_profile::Matches("misc/rift1.i3d",b.data(),b.size(),1,100,0x2000));puts("accept literal");for(const char*p:{"MISC/RIFT1.I3D","misc\\rift1.i3d","/assets/imagery/misc/rift1.i3d"}){assert(rift_qualified_profile::Matches(p,b.data(),b.size(),1,100,0x2000));puts("accept path");}for(const char*p:{"","misc/aura.i3d","magic/foreignaura.i3d","misc/rift1.i3d.bak"}){assert(!rift_qualified_profile::Matches(p,b.data(),b.size(),1,100,0x2000));puts("reject path");}assert(!rift_qualified_profile::Matches(nullptr,b.data(),b.size(),1,100,0x2000));assert(!rift_qualified_profile::Matches("misc/rift1.i3d",nullptr,b.size(),1,100,0x2000));for(int i=0;i<4;++i){assert(!rift_qualified_profile::Matches("misc/rift1.i3d",b.data(),i==0?b.size()-1:b.size(),i==1?2:1,i==2?99:100,i==3?0x2001:0x2000));puts("reject header");}for(size_t i=0;i<b.size();i+=512){b[i]^=1;assert(!rift_qualified_profile::Matches("misc/rift1.i3d",b.data(),b.size(),1,100,0x2000));b[i]^=1;puts("reject body");}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'profile.cpp';driver.write_text(MAIN);binary=output/'profile';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/rift1.i3d')
 assert sha(data)=='58fbb47a8d293744458363f73996e971223118a53e3c4abc76ddc9de17b1263f';body=20+struct.unpack_from('<I',data,16)[0];fixture=output/'body.fixture';fixture.write_bytes(data[body:]);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;r=dict(status='pass',accepted=False,accept=q.stdout.count('accept '),reject=q.stdout.count('reject '),source_sha256=sha((ROOT/'src/riftqualifiedprofile.h').read_bytes()),asset_sha256=sha(data),body_size=len(data)-body,sanitizers='ASan/UBSan',scope='Actual production whole33329byte body hash and exactpath/runtime cached state100/flags2000 guard. Immutable own modern quad/root/ARGB4444 materials+payloads/full keys pinned; positivepaths/negativeheaders andevery512-byte bodyregion independently mutated. No arbitrary-profile admission.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],r['accept'],r['reject']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
