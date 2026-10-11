"""Authored production Photon quad positions/UV/frame order versus native RenderObject packets."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from photon_visual_reference import read_photon,ROOT,sha
DRIVER=r'''
#include <cstdio>
#include "photoncore.h"
#include "retailsoftwaretransform.h"
int main(){std::array<hmm_vec3,4>a;for(auto&v:a)if(std::scanf("%f%f%f",&v.X,&v.Y,&v.Z)!=3)return 2;int x,y,z;float s,px,py,pz;while(std::scanf("%d%d%d%f%f%f%f",&x,&y,&z,&s,&px,&py,&pz)==7){hmm_mat4 owner;BuildRetailSoftwareOwner(owner,x,y,z,0);auto q=photon_core::BuildQuad(a,s,{px,py,pz},owner);for(auto&v:q)std::printf("%.9g %.9g %.9g ",v.X,v.Y,v.Z);std::puts("");}}
'''
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--trace',type=Path,action='append',required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 source=a.output/'geometry.cpp';source.write_text(DRIVER);binary=a.output/'geometry';subprocess.run(['clang++','-std=c++17','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(source),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],check=True)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:asset=read_photon(z.read('Imagery/Magic/photon.i3d'))
 verts=list(struct.iter_unpack('<8f',bytes.fromhex(asset['objects'][0]['vertex_hex'])));prefix=' '.join(str(x)for v in verts for x in v[:3])+'\n';errors=[];checks=0;maximum=0;cases=[]
 for trace in a.trace:
  t=json.loads(trace.read_text());packets=[];lines=[]
  for row in t['records']:
   if not row['draws']:continue
   s=row['state'];slots=[i for i in range(32)if s['frame'][i]>=0 and s['scale'][i]>0]
   assert len(slots)==len(row['draws']),(row['tick'],slots,len(row['draws']))
   for slot,draw in zip(slots,row['draws']):
    checks+=1
    if draw['texture_frame']!=s['frame'][slot]%5:errors.append(dict(tick=row['tick'],field='frame',slot=slot))
    if draw['vertices_hex']!=asset['objects'][0]['vertex_hex']or draw['indices_hex']!=asset['face_hex']:errors.append(dict(tick=row['tick'],field='authored_source',slot=slot))
    lines.append(' '.join(map(str,[*row['motion']['position'],s['scale'][slot],*s['p'][slot]]))+'\n');packets.append((row['tick'],slot,draw))
  values=[list(map(float,l.split()))for l in subprocess.check_output([str(binary)],input=prefix+''.join(lines),text=True).splitlines()]
  for (tick,slot,draw),v in zip(packets,values):
   matrix=draw['matrix'];expected=[sum([*vertex[:3],1][k]*matrix[k*4+axis]for k in range(4))for vertex in verts for axis in range(3)]
   for component,(x,y)in enumerate(zip(expected,v)):
    checks+=1;delta=abs(x-y);maximum=max(maximum,delta)
    if delta>5e-5:errors.append(dict(tick=tick,slot=slot,component=component,native=x,port=y,delta=delta))
  cases.append(dict(trace=str(trace.resolve()),sha256=sha(trace.read_bytes()),packets=len(packets)))
 report=dict(status='pass'if not errors else'differences',checks=checks,max_world_component_error=maximum,errors=errors,cases=cases,source_sha256={x:sha((ROOT/x).read_bytes())for x in('src/photoncore.h','src/math3d.cpp','src/retailsoftwaretransform.h')},scope='Actual original world matrix times authored vertex compared to compiled production BuildQuad and opcode-derived owner. Actual five-frame texture selection and source vertex/UV/index bytes checked. Facing0 controlled flight; shared owner rotation independently verified elsewhere.')
 (a.output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],checks,maximum,'errors',len(errors));print(errors[:3])
 if errors:raise SystemExit(1)
if __name__=='__main__':main()
