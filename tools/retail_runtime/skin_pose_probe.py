#!/usr/bin/env python3
"""Actual source key decoder, default-path control and all Skin native poses."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS
from speed_profile_contract import PRELUDE,function,sha
from skin_visual_reference import NativeSkin,PROFILES,ROOT

MAIN=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b,"magic/skin.i3d");
int changed=0;for(int o=0;o<im.nobj;++o)for(int t=0;t<im.frames;++t){hmm_vec3 p{},r{},s{},dp{},dr{},ds{},fp{},fr{},fs{};
assert(im.GetUninterpolatedAniKey(o,0,t,dp,dr,ds));assert(im.GetUninterpolatedAniKey(o,0,t,fp,fr,fs,false));assert(!memcmp(&dp,&fp,sizeof dp)&&!memcmp(&dr,&fr,sizeof dr)&&!memcmp(&ds,&fs,sizeof ds));
assert(im.GetUninterpolatedAniKey(o,0,t,p,r,s,true));changed+=memcmp(&p,&dp,sizeof p)||memcmp(&r,&dr,sizeof r)||memcmp(&s,&ds,sizeof s);
hmm_mat4 m;MtxClear(&m);if(im.objects[o].name[0]=='#')BuildRetailSpeedCameraMatrix(m,p,r,s);else MakeMatrix(m,p,r,s);
printf("%d %d",o,t);for(float v:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z})printf(" %.9g",v);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}
fprintf(stderr,"default_unchanged=90 inclusive_changed=%d\n",changed);assert(changed>0);}
'''
def run(executable,archive,output,name):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text()
 signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in signatures}
 prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes))
 records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cls=CLASS.replace('40>','56>').replace('Tag,2>','Tag,3>').replace('string,2>','string,3>').replace('std::array<int,2>,56>','std::array<int,4>,56>').replace('t<2;++t','t<im.ntex+1;++t').replace('TYPED_METHOD','bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&,bool=false);').replace('bool null_filename=false;', '''bool null_filename=false,meshinitialized=true;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;
 void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}''')
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"')
 driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'pose'
 command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 q=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Actual source decoder compile failed')
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+PROFILES[name]['asset']+'.i3d'))
 path=output/'skin.asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'controls.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Actual source control failed')
 port={(int(p[0]),int(p[1])):list(map(float,p[2:]))for l in q.stdout.splitlines()if(p:=l.split())}
 f=NativeSkin(executable,data,name);f.initialize_scene();v=f.vm;vectors=v.allocate(36);key_max=matrix_max=0;key_fields=matrix_fields=0;errors=[]
 for(o,t),current in port.items():
  assert v.call(0x409430,(o,0,t,vectors,vectors+12,vectors+24),this=f.imagery)==1
  original=struct.unpack('<9f',v.uc.mem_read(vectors,36));delta=max(abs(a-b)for a,b in zip(original,current[:9]));key_max=max(key_max,delta);key_fields+=9
  if delta>3e-5:errors.append(dict(object=o,tick=t,key_error=delta))
  accepted=v.call(0x40a420,(f.animobjs[o],0,t,0,0),this=f.imagery)==1
  assert accepted==all(s>0 for s in current[6:9])
  if not accepted:continue
  original=struct.unpack('<16f',v.uc.mem_read(f.animobjs[o]+0x58,64));delta=max(abs(a-b)for a,b in zip(original,current[9:]));matrix_max=max(matrix_max,delta);matrix_fields+=16
  if delta>2e-4:errors.append(dict(object=o,tick=t,matrix_error=delta))
 report=dict(status='fail'if errors else'pass',key_fields=key_fields,matrix_fields=matrix_fields,max_key_error=key_max,max_matrix_error=matrix_max,errors=errors,controls=q.stderr,asset_sha256=sha(data),source_spans={s:sha(b.encode())for s,b in methods.items()},source_decoder_sha256=sha((ROOT/'src/3dimage.cpp').read_bytes()),driver_sha256=sha(driver.read_bytes()),sanitizers='ASan/UBSan',accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],key_fields,matrix_fields,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--name',choices=PROFILES,required=True);a=p.parse_args();run(a.executable,a.archive,a.output,a.name)
