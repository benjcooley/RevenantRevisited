#!/usr/bin/env python3
"""Compile the actual quality parser block; preserve default and duplicate policy."""
import argparse,json,subprocess
from pathlib import Path
from speed_profile_contract import sha
ROOT=Path(__file__).resolve().parents[2]
def run(out):
 out.mkdir(parents=True,exist_ok=False);s=(ROOT/'src/game/app/revmain.cpp').read_text();helpers=s[s.index('std::string to_lower('):s.index('template <typename T>',s.index('std::string to_lower('))];a=s.index('        if (arg_param(cmd, "partsys-quality", p))');b=s.index('\n    }',a);block=s[a:b];code='''#include "argh.h"
#include <string>
#include <cctype>
#include <cassert>
#include <stdexcept>
#include <vector>
'''+helpers+'''\nint parse(std::vector<const char*> args){argh::parser cmd;cmd.add_params({"partsys-quality"});cmd.parse(int(args.size()),args.data());int StartupPartSysQuality=1;auto FatalError=[](const char*,void*){throw std::runtime_error("invalid");};std::string p;'''+block+'''return StartupPartSysQuality;}
int main(){assert(parse({"probe"})==1);for(int q=0;q<4;++q){auto arg="--partsys-quality="+std::to_string(q);assert(parse({"probe",arg.c_str()})==q);}for(auto arg:{"--partsys-quality=4","--partsys-quality=-1","--partsys-quality=3.0","--partsys-quality=","--partsys-quality"}){bool threw=false;try{parse({"probe",arg});}catch(...){threw=true;}assert(threw);}assert(parse({"probe","--partsys-quality=1","--partsys-quality=3"})==1);assert(parse({"probe","--partsys-quality=3","--partsys-quality=1"})==3);}
''';driver=out/'quality.cpp';driver.write_text(code);binary=out/'quality';subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/argh'),str(driver),'-o',str(binary)],check=True);subprocess.run([str(binary)],check=True);report=dict(status='pass',accepted=[0,1,2,3],rejected=['4','-1','3.0','empty','missing'],default_unchanged=1,duplicates='existing first matching value wins, both orders tested',actual_parser_block_sha256=sha(block.encode()),helper_sha256=sha(helpers.encode()));(out/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);run(p.parse_args().output)
