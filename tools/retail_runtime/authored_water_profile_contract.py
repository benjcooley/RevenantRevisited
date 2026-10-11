#!/usr/bin/env python3
"""Sanitized exact water admission plus actual four-type classification gate."""
import argparse,json,subprocess
from pathlib import Path
from static_particles_profile_contract import run as exact_guard,ROOT
from speed_profile_contract import function,sha

def run(archive,output):
 profiles=json.loads((ROOT/'tools/retail_runtime/authored_water_profiles.json').read_text())['profiles'];r=exact_guard(archive,output,profiles)
 header=(ROOT/'src/3dimage.h').read_text();generic=function(header,'bool HasRetailStaticParticleProfile(');water=function(header,'bool HasRetailAuthoredWaterProfile(')
 cpp='#include <cassert>\n#include <cstdint>\n#include "staticpartsysprofiles.h"\nstruct T3DImagery{uint32_t static_particles_profile=0;'+generic+'\n'+water+'};\nint main(){T3DImagery im;for(const auto&p:retail_static_particles::profiles){im.static_particles_profile=p.id;for(const auto&q:retail_static_particles::profiles){bool expected=p.id==q.id&&(q.id==0xd0c0f036u||q.id==0xd0c0f037u||q.id==0xd0c0f038u||q.id==0xd0c0f039u);assert(im.HasRetailAuthoredWaterProfile(q.id)==expected);}assert(!im.HasRetailAuthoredWaterProfile(0xdeadbeefu));}im.static_particles_profile=0;for(const auto&p:retail_static_particles::profiles)assert(!im.HasRetailAuthoredWaterProfile(p.id));}\n'
 driver=output/'water-gate.cpp';driver.write_text(cpp);binary=output/'water-gate';command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(driver),'-o',str(binary)];q=subprocess.run(command,capture_output=True,text=True);(output/'water-gate-compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Actual water gate failed compilation')
 q=subprocess.run([str(binary)],capture_output=True,text=True);(output/'water-gate.log').write_text(q.stdout+q.stderr)
 if q.returncode or q.stderr:raise RuntimeError('Actual water gate failed negative/positive classification')
 r.update(water_gate_source_sha256=sha(water.encode()),water_gate_driver_sha256=sha(driver.read_bytes()),water_gate_binary_sha256=sha(binary.read_bytes()),wrapper_sha256=sha(Path(__file__).read_bytes()),preserved_profile_count=9,water_profile_count=4,water_gate_pair_checks=169,water_gate_unknown_checks=13,water_gate_unselected_checks=13)
 (output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');return r
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.archive,a.output);print(r['status'],sum(c['positive_cases']for c in r['cases']),sum(c['rejection_cases']for c in r['cases']))
