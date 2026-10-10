#!/usr/bin/env python3
"""Execute original full parser/Initialize vs actual compiled curve destinations.

These tags are deliberately labeled scalar-default diagnostics, not shipped
effect variants. Exact Speed objects supply the required real prototype and
emitter identities; no native parser or Initialize call is replaced.
"""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import struct
import subprocess

from speed_controller_preflight import NativeSpeed, ROOT

PREFIX = 'obj=(#speedflare),particle=#speedo,pps=30,lifespan=20:25'
CASES = (
    ('absent', ''),
    ('scale-literal', 'scale=1.5'),
    ('scale-curve', 'scale=[0:1.5,100:0]'),
    ('alpha-literal', 'alpha=0.75'),
    ('alpha-curve', 'alpha=[0:0.75,100:0]'),
    ('bounce-literal', 'bounce=25:75'),
    ('bounce-curve', 'bounce=[0:(25,75),100:(1,2)]'),
)
DRIVER = r'''
#include "partsysdefinition.h"
#include <cstdio>
int main(int argc,char**argv){authored_partsys::Definition d;std::string diag;
 if(argc!=2||!authored_partsys::ParseDefinition(argv[1],d,diag))return 2;
 printf("%.9g %.9g %.9g %.9g\n",d.scale.constant[0],d.alpha.constant[0],
 d.bounce.constant[0],d.bounce.constant[1]);}
'''


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(executable, asset_file, output):
    output.mkdir(parents=True, exist_ok=True)
    base = json.loads(asset_file.read_text())[0]
    source = output/'parser_driver.cpp';source.write_text(DRIVER)
    binary = output/'parser_driver'
    command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',
        '-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),
        str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)]
    p=subprocess.run(command,capture_output=True,text=True)
    (output/'compile.log').write_text(p.stdout+p.stderr)
    if p.returncode:raise RuntimeError('Actual production parser compile failed')
    rows=[];errors=[]
    for name, field in CASES:
        parameters=PREFIX+(','+field if field else '')
        asset=copy.deepcopy(base);asset['tags'][1]['parameters']=parameters
        native=NativeSpeed(executable,asset);native.initialize(0)
        c=native.controller;v=native.vm
        original=[struct.unpack('<f',v.uc.mem_read(c+o,4))[0]for o in(0xa8,0xa4,0xf0,0xf4)]
        p=subprocess.run([str(binary),parameters],capture_output=True,text=True)
        if p.returncode or p.stderr:raise RuntimeError('Actual compiled parser execution failed')
        modern=[float(x)for x in p.stdout.split()]
        if original!=modern:errors.append(name)
        rows.append(dict(name=name,parameters=parameters,native=original,production=modern,
            equal=original==modern,original_call_counts=native.calls))
    report=dict(status='pass'if not errors else'fail',errors=errors,cases=rows,
        destination_order=['scale','alpha','bounce_min','bounce_max'],comparisons=len(rows)*4,
        evidence='Original4010f3 MOV [EBX],0 before literal/curve selection4010fc; '
            'curve branch never writes the additional caller destinations. '
            'Whole literal40d750/4042e0/4010d0/403320 run in every diagnostic.',
        source_sha256=sha(ROOT/'src/partsysdefinition.cpp'),probe_sha256=sha(__file__),
        generated_source_sha256=sha(source),binary_sha256=sha(binary),
        original_executable_sha256=sha(executable),sanitizers='ASan/UBSan',
        accepted=False,new_rendered_ab_cases=0,
        scope='Seven explicit default/literal/curve diagnostics. Whole original native '
            'parser/Initialize; decoded exact Speed object/provider interfaces and '
            'prototype copy boundary declared in speed_controller_preflight. '
            'This establishes parser destinations, not a rendered named-effect case.')
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable',type=Path);p.add_argument('--asset-triage',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);args=p.parse_args()
    r=run(args.executable,args.asset_triage,args.output)
    print(json.dumps({k:v for k,v in r.items()if k!='cases'},indent=2))
    if r['errors']:raise SystemExit(1)
