"""Background original raster-span cause; no ordinary geometry deletion."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
from meteorstorm_software_bounds_probe import NativeBounds
MAIN=r'''
#include "cometsoftwarebounds.h"
#include <cstdio>
int main(){hmm_mat4 m;hmm_vec3 p[3];while(scanf("%f",&m.Elements[0][0])==1){for(int i=1;i<16;++i)scanf("%f",(&m.Elements[0][0])+i);for(auto&v:p)scanf("%f%f%f",&v.X,&v.Y,&v.Z);printf("%d\n",RetailCometTriangleAllowed(p,m));}}
'''
def run(out,base):
 out.mkdir(parents=True,exist_ok=False);a=json.loads((base/'bounds-original-first1/manifest.json').read_text())['rows'][0];b=json.loads((base/'bounds-widened-first1/manifest.json').read_text())['rows'][0];same=['rng','state_hex','draws','owner_flags','owner_frame','owner_position','animate_calls'];assert all(a[k]==b[k]for k in same)and b['pixels']>a['pixels'];d=out/'gate.cpp';d.write_text(MAIN);binary=out/'gate';q=subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(d),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_asset(z.read('Imagery/Misc/starfield.i3d'))
 v=list(struct.iter_unpack('<8f',bytes.fromhex(asset['objects'][0]['vertex_hex'])));faces=list(struct.iter_unpack('<3H',bytes.fromhex(asset['face_hex'])[:12]));world=a['draws'][-1]['world'];inputs=[];expected=[];f=NativeBounds(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
 for face in faces:
  vertices=[v[i][:3]for i in face];points=[]
  for p in vertices:
   x,y,z=[sum(p[k]*world[k*4+j]for k in range(3))+world[12+j]for j in range(3)];points.append((x-y,.5050762891769409*(x+y)-1.237179160118103*z))
  expected.append(f.test(points));inputs.append(world+[x for p in vertices for x in p]);assert not expected[-1]
 f.f.vm.write(0x5b9cc0,struct.pack('<f',6400.));f.f.vm.write(0x5b9cbc,struct.pack('<f',4800.))
 for face in faces:
  vertices=[v[i][:3]for i in face];points=[]
  for p in vertices:
   x,y,z=[sum(p[k]*world[k*4+j]for k in range(3))+world[12+j]for j in range(3)];points.append((x-y,.5050762891769409*(x+y)-1.237179160118103*z))
  assert f.test(points)
 q=subprocess.run([str(binary)],input='\n'.join(' '.join(map(str,row))for row in inputs),capture_output=True,text=True);assert q.returncode==0 and not q.stderr;assert list(map(int,q.stdout.split()))==list(map(int,expected));r=dict(status='pass',accepted=False,unchanged_fields=same,whole_draw_packets=len(a['draws']),original_pixels=a['pixels'],widened_pixels=b['pixels'],source_original_triangle_checks=len(expected),original_triangles_rejected=True,widened_triangles_accepted=True,sanitizers='ASan/UBSan',source_helper_sha256=sha((ROOT/'src/cometsoftwarebounds.h').read_bytes()),scope='Qualified Labyrinth bound3 scene unchanged. Only two native raster extentconstants640/480→6400/4800 restorebackground. Both actualauthored triangles rejectexecuted original span code andcompiled productionhelper; widened native accepts. Strictadapter appliesonly explicitnative-domain/software conjunction; ordinarybackground geometry remains registered/submitted.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],a['pixels'],b['pixels']);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--evidence',type=Path,required=True);a=p.parse_args();run(a.output,a.evidence)
