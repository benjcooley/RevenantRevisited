#!/usr/bin/env python3
"""Compile the exact whole Blood resource matcher with ASan/UBSan."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "bloodprofile.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});assert(b.size()==68716);assert(blood_profile::Matches("misc/blood.i3d",b.data(),b.size(),1,16,0x2001));puts("accept literal");for(const char*p:{"MISC/BLOOD.I3D","misc\\blood.i3d","/assets/imagery/misc/blood.i3d"}){assert(blood_profile::Matches(p,b.data(),b.size(),1,16,0x2001));puts("accept path");}for(const char*p:{"","magic/blood.i3d","misc/foreignblood.i3d","misc/blood.i3d.bak"}){assert(!blood_profile::Matches(p,b.data(),b.size(),1,16,0x2001));puts("reject path");}assert(!blood_profile::Matches(nullptr,b.data(),b.size(),1,16,0x2001));assert(!blood_profile::Matches("misc/blood.i3d",nullptr,b.size(),1,16,0x2001));for(int i=0;i<4;++i){assert(!blood_profile::Matches("misc/blood.i3d",b.data(),i==0?b.size()-1:b.size(),i==1?2:1,i==2?15:16,i==3?0x2000:0x2001));puts("reject header");}for(size_t i=0;i<b.size();i+=32){b[i]^=1;assert(!blood_profile::Matches("misc/blood.i3d",b.data(),b.size(),1,16,0x2001));b[i]^=1;puts("reject body");}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);driver=output/'profile.cpp';driver.write_text(MAIN);binary=output/'profile';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/blood.i3d')
 assert sha(data)=='77e445a5c5d34d32e4948f596d048b15d82c0107bdcf1938642e6db805c3a119';body=20+struct.unpack_from('<I',data,16)[0];fixture=output/'body.fixture';fixture.write_bytes(data[body:]);q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;r=dict(status='pass',accepted=False,accept=q.stdout.count('accept '),reject=q.stdout.count('reject '),source_sha256=sha((ROOT/'src/bloodprofile.h').read_bytes()),asset_sha256=sha(data),body_size=len(data)-body,sanitizers='ASan/UBSan',scope='Actual production whole68716byte body hash and exactpath/state16/looping guard. Immutable own version1/8roots/32vertices/16faces/legacy8bytekeys/materials/RGB565+ARGB4444 fullpayloads pinned; positivepaths/negativeheaders andevery32-byte bodyregion independently mutated. No arbitrary-profile admission.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],r['accept'],r['reject']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
