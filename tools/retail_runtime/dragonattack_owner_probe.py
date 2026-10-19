"""Compile the actual Dragonattack owner core and compare its original trace."""
import argparse
import json
import subprocess
from pathlib import Path
from character_native_preflight import ROOT, sha

DRIVER = r'''
#include "dragonattackcore.h"
#include <cassert>
#include <cstdio>
int main(){
 retail_dragonattack::State s;
 assert(s.Initialize(3185,3185,16,14,14,3465,3185,0));
 for(int t=0;t<110;++t){s.Pulse(0);printf("%d %d %d %d %d %d %d %d %d %d %d\n",
  t,s.state,s.frame,s.counter,s.x,s.y,s.z,s.vx,s.vy,s.vz,int(s.alive));}
 retail_dragonattack::State invalid;
 assert(!invalid.Initialize(0,0,0,0,0,0,0,100));
}
'''


def run(trace, output):
    output.mkdir(parents=True, exist_ok=False)
    source, binary = output/'owner.cpp', output/'owner'
    source.write_text(DRIVER)
    subprocess.run(['clang++', '-std=c++17', '-O1', '-fsanitize=address,undefined',
                    '-iquote', str(ROOT/'src'), str(source), '-o', str(binary)], check=True)
    result = subprocess.run([str(binary)], capture_output=True, text=True, check=True)
    original = json.loads(trace.read_text())
    rows = original['records']
    native = [[r['tick'], r['state'], r['frame'], r['counter'], *r['position'],
               *r['velocity'], int(not(r['flags'] & 4096))] for r in rows]
    port = [[int(x) for x in line.split()] for line in result.stdout.splitlines()]
    assert native == port
    report = dict(status='pass', rows=len(rows), integer_fields=sum(map(len, native)),
                  kill_record=next(r['tick'] for r in rows if r['flags'] & 4096),
                  trace_sha256=sha(trace), source_sha256=sha(ROOT/'src/game/effects/dragonattackcore.h'),
                  scope='Declared actual Acolyte Fighting/Sid Dragon Attack caller, '
                        'face96 and original Angle20 muzzle(14,14), empty-map ground0. '
                        'Actual owner initialization, velocity, truncated movement, '
                        'three-state timing and real kill. No actor damage or renderer credit.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('trace', type=Path)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.trace, a.output)
