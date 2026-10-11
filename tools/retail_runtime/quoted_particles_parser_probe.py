#!/usr/bin/env python3
"""Whole original parser token/integer semantics vs compiled production parser."""
import argparse,copy,json,struct,subprocess,zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from speed_controller_preflight import NativeSpeed,read_asset,ROOT
from static_particles_profile_contract import sha

CASES=(('absent',None),('quarter','0.25'),('integer','5'),('decimal_integer','5.0'),('negative_quarter','-0.25'),('negative_integer','-5'),('near_integer','0.99999999'),('zero_decimal','0.0'),('scientific_reject','1e2'))
DRIVER=r'''
#include "partsysdefinition.h"
#include <cstdio>
int main(int argc,char**argv){authored_partsys::Definition d;std::string error;
 if(argc!=2)return 2;if(!authored_partsys::ParseDefinition(argv[1],d,error)){printf("reject\n");return 0;}
 printf("accept %d %d %zu %s %.9g\n",d.emittersize,d.emittertype,d.objects.size(),d.particle.c_str(),d.pps.constant[0]);}
'''

def run(executable,archive,output):
 output.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(archive)as z:
  ns={n.lower():n for n in z.namelist()};data=z.read(ns['imagery/magic/yenergy.i3d'])
 if sha(data)!='89f1f9e88c8d64ba1278f54d3bc7a10275204feb7463c4ee3b9145dff1cd9be2':raise ValueError('Exact YEnergy fixture changed')
 base=read_asset(data);source=output/'parser.cpp';source.write_text(DRIVER);binary=output/'parser';command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(binary)];p=subprocess.run(command,text=True,capture_output=True);(output/'compile.log').write_text(p.stdout+p.stderr)
 if p.returncode:raise RuntimeError('Production parser compile failed')
 rows=[];errors=[]
 for name,literal in CASES:
  a=copy.deepcopy(base);a['name']='YEnergy';tag=next(t for t in a['tags']if t['name']=='partsys');tag['parameters']='obj="emitter",particle="#particle",emittertype="sphere"'+(''if literal is None else',emittersize='+literal)+',pps=45,lifespan=50:60'
  f=NativeSpeed(executable,a);events=[]
  def observe(uc,address,size,user):
   v=f.vm;sp=uc.reg_read(UC_X86_REG_ESP);token=v.u32(sp+0x30);events.append(dict(address=hex(address),token_type=v.u32(token+0x10),integer_view=struct.unpack('<i',v.uc.mem_read(token+0x14,4))[0]))
  for addr in(0x404432,0x404443):f.vm.uc.hook_add(UC_HOOK_CODE,observe,begin=addr,end=addr)
  accepted=True
  try:f.initialize(0)
  except AssertionError:accepted=False
  value=struct.unpack('<i',f.vm.uc.mem_read(f.controller+0x108,4))[0]if accepted else None
  p=subprocess.run([str(binary),tag['parameters']],text=True,capture_output=True)
  if p.returncode or p.stderr:raise RuntimeError('Production parser execution/sanitizer failed')
  values=p.stdout.split();modern_accept=values[0]=='accept';modern=int(values[1])if modern_accept else None
  equal=accepted==modern_accept and value==modern
  if modern_accept:equal=equal and values[2:5]==['0','1','#particle']and float(values[5])==45
  if not equal:errors.append(name)
  rows.append(dict(name=name,literal=literal,parameters=tag['parameters'],native_accepted=accepted,native_size=value,native_events=events,production_accepted=modern_accept,production_size=modern,equal=equal))
 report=dict(status='fail'if errors else'pass',errors=errors,cases=rows,source_sha256=sha((ROOT/'src/partsysdefinition.cpp').read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),driver_sha256=sha(source.read_bytes()),binary_sha256=sha(binary.read_bytes()),original_executable_sha256=sha(executable.read_bytes()),sanitizers='ASan/UBSan',accepted=False,new_rendered_ab_cases=0,scope='Nine explicit quoted-name/decimal-integer diagnostics on exact YEnergy object resources. Whole native tokenization/ParseItem/Initialize vs actual production parser. Original404432 tests numeric token8,404443 copies integer view to controller+108; unchanged downstream PPS45 proves whole parser progress. No diagnostic tag is credited as a shipped effect.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.executable,a.archive,a.output);print(json.dumps(r,indent=2))
 if r['errors']:raise SystemExit(1)
