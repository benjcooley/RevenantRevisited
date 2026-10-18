"""Executed original IceBolt ten-unit world march versus compiled production endpoint helper."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from icebolt_visual_reference import IceScene
from speed_controller_preflight import ROOT,sha
DRIVER=r'''
#include <cstdio>
#include "iceboltcore.h"
int main(){int face,length;while(std::scanf("%d%d",&face,&length)==2){float value=icebolt_core::MarchLength(0,0,uint8_t(face),[&](int x,int y){return double(x)*x+double(y)*y>=double(length)*length;});std::printf("%.9g\n",value);}}
'''
p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);source=a.output/'march.cpp';source.write_text(DRIVER);binary=a.output/'march';subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/icebolt.i3d')
cases=[]
for face,length in((0,250),(13,250),(64,250),(91,330),(128,100),(201,499)):
 f=IceScene(a.executable,data,wall_length=length,facing=face);n=f.state();c=float(subprocess.check_output([str(binary)],input=f'{face} {length}\n',text=True));cases.append(dict(face=face,world_boundary_radius=length,native_length=n['length'],port_length=c,native_owner=n['position'],match=n['length']==c))
r=dict(status='pass'if all(x['match']for x in cases)else'differences',cases=cases,source_sha256=sha((ROOT/'src/game/effects/iceboltcore.h').read_bytes()),scope='Actual original Initialize4eb570 march, authored +50Z lift, world query returns height10000 at radial endpoint boundary/1 before it, no live enemy. Compiled production float step/integer query helper. Facing13 demonstrates260 length for radius250 rather than a fitted Euclidean span.')
(a.output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2));assert r['status']=='pass'
