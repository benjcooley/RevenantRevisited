"""Compile production CFire key/matrix/profile code against the genuinely loaded failed-preset path."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import PRELUDE,CLASS,function,sha,ROOT
MAIN=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b);assert(ValidateCFire(im));
++im.frames;assert(!ValidateCFire(im));--im.frames;
im.textures[0].desc.pixelFormat.dwRGBAlphaBitMask=0;assert(!ValidateCFire(im));im.textures[0].desc.pixelFormat.dwRGBAlphaBitMask=0xf000;
++im.materials[0].texture;assert(!ValidateCFire(im));--im.materials[0].texture;
for(int o=0;o<4;++o){im.parent[o]=0;assert(!ValidateCFire(im));im.parent[o]=-1;auto*w=reinterpret_cast<uint32_t*>(im.keys[o].data());*w^=1;assert(!ValidateCFire(im));*w^=1;}
for(int i=0;i<2;++i){auto str=im.tags[i].str;im.tags[i].str="obj=fake,particle=fake";assert(!ValidateCFire(im));im.tags[i].str=str;}
assert(ValidateCFire(im));fprintf(stderr,"13 exact-profile rejection controls PASS\n");
for(int tick=1;tick<30;++tick)for(int o=0;o<4;++o){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(o,0,tick,p,r,s,true));hmm_mat4 m;if(im.objects[o].name[0]=='#')BuildRetailSpeedCameraMatrix(m,p,r,s);else {MtxClear(&m);MtxRotateX(&m,r.X);MtxRotateY(&m,r.Y);MtxRotateZ(&m,r.Z);MtxScale(&m,&s);MtxTranslate(&m,&p);}printf("M %d %d",tick,o);for(int k=0;k<16;++k)printf(" %.9g",(&m.Elements[0][0])[k]);puts("");}}
'''

def compile_source(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();render=(ROOT/'src/engine/render/render3d_types.h').read_text()
 sigs=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in sigs};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"\n#include "authoredpartsys.h"\n#include "cfireprofile.h"')
 cls=CLASS.replace(',3>',',64>').replace(',2>',',8>').replace('i<3;++i){const size_t o=','i<im.nobj;++i){const size_t o=').replace('t<3;++t','t<im.ntex+1;++t').replace('i<2;++i){const size_t p=tex','i<im.ntex;++i){const size_t p=tex').replace('i<3;++i){memcpy(&im.materials','i<im.nmat;++i){memcpy(&im.materials').replace('void Select(){speed_partsys_profile=ValidateRetailSpeedPartSysProfile();}','').replace('i<2;++i){const size_t p=tags','i<im.ntags;++i){const size_t p=tags')
 cls=cls.replace('bool ValidateRetailSpeedPartSysProfile();','void GetObject(int i,S3DObj*o){*o=objects[i];}void GetMaterial(int i,S3DMat*m){*m=materials[i];}void GetTexture(int i,S3DTex*t){*t=textures[i];}')
 methods['validation']=function((ROOT/'src/game/effects/legacy/cfireoriginal.cpp').read_text(),'static bool ValidateCFire(')
 driver=output/'source.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'source'
 q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),*[str(ROOT/'src'/p)for p in('math3d.cpp','authoredpartsys.cpp','partsysdefinition.cpp')],'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,'See compile.log'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/misc/cfire.i3d'))
 path=output/'asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'source.log').write_text(q.stderr);assert q.returncode==0,'See source.log'
 return q.stdout

def compare(trace,native,output):
 r=json.loads(native.read_text());matrices={(int(p[1]),int(p[2])):list(map(float,p[3:]))for p in map(str.split,trace.splitlines())};maximum=0;fields=0
 for row in r['records']:
  tick=row['tick'];assert row['state']['seed']==1 and row['state']['rng_draws']==0
  assert bool(row['state']['flags']&4096)==(tick>=30)
  assert len(row['draws'])==(2 if tick<30 else 0)
  for o,original in enumerate(row['state']['matrices']):
   if tick>=30:continue
   current=matrices[(tick,o)];err=max(abs(a-b)for a,b in zip(original,current));maximum=max(maximum,err);fields+=16;assert err<0.00002,(tick,o,err,original,current)
 report=dict(status='pass',matrix_fields=fields,matrix_max=maximum,original_kill=30,rng_draws=0,controllers=r['controllers'],failed_preset_opens=[x for x in r['preset_opens']if x.get('result')=='0x0'],native_sha256=sha(native.read_bytes()),driver_sha256=sha((output/'source.cpp').read_bytes()),scope='Production exact resource validation and integer keys/matrices under ASan/UBSan; native actual owner flags and empty RNG retained. Appearance separate.')
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report);return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();trace=compile_source(a.output);compare(trace,a.native,a.output)
