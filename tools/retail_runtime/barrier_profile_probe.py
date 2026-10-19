"""Sanitized strict admission for the two complete old barrier bodies."""
import argparse
import json
import subprocess
import zipfile
from pathlib import Path
from barrier_wall_native_preflight import ROOT, PROFILES
from speed_profile_contract import sha

DRIVER=r'''
#include "barrierlegacykeys.h"
#include <cassert>
#include <fstream>
#include <vector>
#include <cstdio>
int main(int argc,char**argv){
 assert(argc==3);std::ifstream in(argv[1],std::ios::binary);
 std::vector<unsigned char> file((std::istreambuf_iterator<char>(in)),{});
 uint32_t offset=0;memcpy(&offset,file.data()+16,4);
 std::vector<unsigned char> body(file.begin()+20+offset,file.end());
 assert(body.size()==25244);auto*m=reinterpret_cast<SOld3DImageryBody*>(body.data());
 const std::string path=argv[2];auto matches=[&](const std::string&p,size_t n,int states,int frames,uint32_t flags){return barrier_legacy::Matches(p.c_str(),m,n,states,frames,flags);};
 assert(matches(path,body.size(),1,1,0x2001));
 std::string upper=path;for(char&c:upper)c=char(std::toupper(static_cast<unsigned char>(c)));
 assert(matches(upper,body.size(),1,1,0x2001));
 std::string windows=path;for(char&c:windows)if(c=='/')c='\\';
 assert(matches(windows,body.size(),1,1,0x2001));
 assert(matches("/assets/imagery/"+path,body.size(),1,1,0x2001));
 assert(!matches("bad"+path,body.size(),1,1,0x2001));
 assert(!matches(path+".bak",body.size(),1,1,0x2001));
 assert(!matches(path,25243,1,1,0x2001));
 assert(!matches(path,body.size(),2,1,0x2001));
 assert(!matches(path,body.size(),1,2,0x2001));
 assert(!matches(path,body.size(),1,1,0x2000));
 assert(!barrier_legacy::Matches(nullptr,m,body.size(),1,1,0x2001));
 assert(!barrier_legacy::Matches(path.c_str(),nullptr,body.size(),1,1,0x2001));
 for(size_t i=0;i<body.size();++i){body[i]^=1;assert(!matches(path,body.size(),1,1,0x2001));body[i]^=1;}
 const auto other=path.find("barrierew")!=std::string::npos?"misc/barrierns.i3d":"misc/barrierew.i3d";
 assert(!matches(other,body.size(),1,1,0x2001));
 printf("pass 4 %zu\n",body.size()+9);
}
'''


def run(output):
    output.mkdir(parents=True,exist_ok=False)
    source=output/'guard.cpp';source.write_text(DRIVER);binary=output/'guard'
    q=subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined',
                      '-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),
                      str(source),'-o',str(binary)],capture_output=True,text=True)
    (output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,q.stderr
    cases=[]
    with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
        for name,type_id,_,_,digest in PROFILES:
            asset=output/(name+'.fixture')
            asset.write_bytes(archive.read(next(n for n in archive.namelist()
                                               if n.lower()=='imagery/misc/'+name.lower()+'.i3d')))
            assert sha(asset.read_bytes())==digest
            q=subprocess.run([str(binary),str(asset),'misc/'+name.lower()+'.i3d'],capture_output=True,text=True)
            (output/(name+'.log')).write_text(q.stdout+q.stderr);assert q.returncode==0,q.stdout+q.stderr
            cases.append(dict(name=name,type_id=type_id,accepted_paths=4,rejections=25253,asset_sha256=digest))
    report=dict(status='pass',cases=cases,profile_sha256=sha((ROOT/'src/game/effects/barrierlegacykeys.h').read_bytes()),
                scope='Complete immutable own-body admission; every byte mutation and header/path/null/cross-resource controls under ASan/UBSan. Does not enable a shared loader branch or grant pose/render/appearance credit.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS both own bodies')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    run(p.parse_args().output)
