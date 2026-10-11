"""Compile actual key decoder/matrix and compare every loaded Quicksand draw."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS
from speed_profile_contract import PRELUDE,function,sha,ROOT

def run(native,output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text()
 signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(')
 methods='\n'.join(function(source,s)for s in signatures)
 prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32')
 constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes))
 records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cls=CLASS.replace('S3DMat,3>','S3DMat,4>').replace('std::array<int,2>,40>','std::array<int,5>,40>').replace('t<2;++t','t<5;++t').replace('TYPED_METHOD','bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&,bool=false);').replace('bool null_filename=false;', '''bool null_filename=false,meshinitialized=true;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;
 void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}''')
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "quicksandcore.h"')
 main=r'''int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b,"magic/quicksand.i3d");assert(im.nobj==4&&im.frames==3);for(int t=0;t<3;++t)for(int o=0;o<4;++o){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(o,0,t,p,r,s,true));hmm_mat4 m;retail_quicksand::Matrix(m,p,r,s);printf("%d %d",t,o);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}retail_quicksand::State core;for(int tick=1;tick<=60;++tick){core.Pulse();printf("C %d %d %d %d\n",tick,core.ticks,core.frame,int(core.alive));}}'''
 driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+methods+'\n'+main)
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/quicksand.i3d')
 asset=output/'quicksand.i3d';asset.write_bytes(data);binary=output/'pose'
 command=['clang++','-std=c++20','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 subprocess.run(command,check=True,text=True)
 text=subprocess.check_output([str(binary),str(asset)],text=True);(output/'source.txt').write_text(text)
 poses={(int(v[0]),int(v[1])):list(map(float,v[2:]))for v in map(str.split,text.splitlines())if v[0]!='C'}
 lifecycle={int(v[1]):list(map(int,v[2:]))for v in map(str.split,text.splitlines())if v[0]=='C'}
 rows=json.loads(native.read_text())['records'];errors=[];maximum=0;checks=0
 for row in rows:
  state=row['state']
  assert lifecycle[row['tick']]==[state['animator_frame'],state['frame'],int(not(state['flags']&4096))]
  for draw in row['draws']:
   expected=poses[state['frame'],draw['object']][:]
   for j,value in enumerate(state['target_position']):expected[12+j]+=value
   error=max(abs(a-b)for a,b in zip(expected,draw['matrix']));maximum=max(maximum,error);checks+=16
   if error>3e-4:errors.append([row['tick'],draw['object'],error])
 report=dict(status='pass'if not errors else 'fail',checks=checks,lifecycle_checks=180,maximum_error=maximum,errors=errors,native_sha256=sha(native.read_bytes()),asset_sha256=sha(asset.read_bytes()));(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report));assert not errors
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
