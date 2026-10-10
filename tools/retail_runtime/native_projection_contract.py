#!/usr/bin/env python3
"""Independent owner/view matrices versus executed retail and visible controls."""
import argparse,json,struct,subprocess
from pathlib import Path
from firecone_render_probe import FireConeRenderFixture,ROOT,sha
from firecone_owner_probe import native_owner
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png

DRIVER=r'''
#include "retailsoftwaretransform.h"
#include <cstdio>
int main(){int x,y,z,face,cx,cy,cz;float rx,ry;
 while(scanf("%d%d%d%d%d%d%d%f%f",&x,&y,&z,&face,&cx,&cy,&cz,&rx,&ry)==9){
 hmm_mat4 owner,view,combined;BuildRetailSoftwareOwner(owner,x,y,z,uint8_t(face),rx,ry);
 BuildRetailSoftwareViewProjection(view,cx,cy,cz);MtxMultiply(&combined,&owner,&view);
 for(const auto&m:{owner,view,combined})for(int r=0;r<4;++r)for(int c=0;c<4;++c)printf("%.9g ",m.Elements[r][c]);puts("");}}
'''

def run(executable,output,selected_cases=None):
 output.mkdir(parents=True,exist_ok=False);cpp=output/'driver.cpp';cpp.write_text('#include <initializer_list>\n'+DRIVER);binary=output/'driver'
 command=['clang++','-std=c++17','-O1','-ffp-contract=off','-fsanitize=address,undefined','-I'+str(ROOT/'thirdparty/handmademath'),'-iquote',str(ROOT/'src'),str(cpp),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 result=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(result.stdout+result.stderr)
 if result.returncode:raise RuntimeError('Independent matrix compile failed')
 inputs=[(x,y,z,face,cx,cy,cz,rx,ry)for face in(0,37,64,128,192,255)for x,y,z,cx,cy,cz,rx,ry in((0,0,0,0,0,0,0,0),(10000,10000,16,10000,10000,0,0,0),(10016,9992,20,10000,10000,0,0,0),(10000,10000,116,10000,10000,0,0,0),(10016,9992,-40,10000,10000,-20,.2,-.1))]
 if selected_cases is not None:inputs=[inputs[i] for i in selected_cases]
 result=subprocess.run([str(binary)],input='\n'.join(' '.join(map(str,r))for r in inputs)+'\n',capture_output=True,text=True)
 if result.returncode or result.stderr:raise RuntimeError(result.stderr)
 candidates=[[float(p)for p in line.split()]for line in result.stdout.splitlines()];cases=[]
 f=FireConeRenderFixture(executable,0);nv=f.vm
 vertices=[(-24,-16,0,0,-.6,-.8,0,0),(24,-16,0,0,-.6,-.8,1,0),(24,16,20,0,-.6,-.8,1,1),(-24,16,20,0,-.6,-.8,0,1)]
 for index,(row,candidate)in enumerate(zip(inputs,candidates)):
  x,y,z,face,cx,cy,cz,rx,ry=row
  nv.write(f.owner+16,struct.pack('<3i',x,y,z));nv.write(f.owner+0x36,bytes([face]));nv.write(f.animator+0x84,struct.pack('<2f',rx,ry));owner=native_owner(f)
  software=LitSoftwareFixture(executable,512,512,camera=(cx,cy,cz));v=software.vm;software.lighting((38,38,38),(1,1,1))
  view=struct.unpack('<16f',v.uc.mem_read(0x676000,64));projection=struct.unpack('<16f',v.uc.mem_read(0x675f70,64));buffer=v.allocate(64)
  v.call(0x43aa90,(buffer,0x676000,0x675f70));native_vp=struct.unpack('<16f',v.uc.mem_read(buffer,64))
  points=[vertex[:3]for vertex in vertices];screen=software.project(points,world_matrix=owner,camera=(cx,cy,cz));combined=candidate[32:]
  portscreen=[(sum((*p,1)[r]*combined[r*4]for r in range(4))+256,-sum((*p,1)[r]*combined[r*4+1]for r in range(4))+256,sum((*p,1)[r]*combined[r*4+2]for r in range(4)))for p in points]
  images=[];colors=[]
  for matrix in(owner,candidate[:16]):
   software.clear();v.call(0x56d3a0,(0,0));image,depth,transformed=software.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1)
   images.append(image);colors.append([list(q[3:7])for q in transformed])
  wrong=list(candidate[:16])
  for r in range(3):wrong[r*4+2]*=1.5
  wrong[14]=float(z)
  software.clear();wrong_image,_,wrong_vertices=software.draw_authored(vertices,[0,1,2,0,2,3],wrong,cull=1)
  negative=sum(a!=b for a,b in zip(struct.iter_unpack('<H',images[0]),struct.iter_unpack('<H',wrong_image)))
  save_rgb565_png(output/f'control{index:02d}-old-common-owner.png',wrong_image,512,512)
  save_rgb565_png(output/f'control{index:02d}-native.png',images[0],512,512);save_rgb565_png(output/f'control{index:02d}-cpp.png',images[1],512,512)
  cases.append(dict(input=row,old_common_owner_differing_pixels=negative,old_common_owner_colors=[list(q[3:7])for q in wrong_vertices],owner_max_error=max(abs(a-b)for a,b in zip(owner,candidate)),view_projection_max_error=max(abs(a-b)for a,b in zip(native_vp,candidate[16:32])),projected_max_error=max(abs(a-b)for p,q in zip(screen,portscreen)for a,b in zip(p,q)),differing_pixels=sum(a!=b for a,b in zip(struct.iter_unpack('<H',images[0]),struct.iter_unpack('<H',images[1]))),nonzero_pixels=sum(p!=(0,)for p in struct.iter_unpack('<H',images[0])),native_colors=colors[0],production_colors=colors[1],native_owner=owner,native_view_projection=native_vp))
 report=dict(status='pass'if any(c['old_common_owner_differing_pixels']for c in cases) and all(c['owner_max_error']<.001 and c['view_projection_max_error']<.005 and c['projected_max_error']<.005 and c['differing_pixels']==0 and c['nonzero_pixels']for c in cases)else'differences_found',cases=cases,source_sha256=sha((ROOT/'src/retailsoftwaretransform.h').read_bytes()),scope='30 constant authored quad controls: moving integer XYZ, six facings, nonzero XY rotations and cameraZ; independent C++ owner/view versus actual40e470/40e740/56cdc0/411640/56d5f0, original normal-lit pixels. Integration into actual Metal remains separate.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.executable,a.output);print(r['status'],len(r['cases']))
