#!/usr/bin/env python3
"""Exact single-state Maelstrom admission and source/native key/matrix proof."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS
from speed_profile_contract import PRELUDE,function,sha,ROOT
from maelstrom_visual_reference import MaelstromScene

def run(executable,output):
 output.mkdir(parents=True,exist_ok=True);source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text()
 signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in signatures};methods["static bool ValidateMaelstrom("]=function((ROOT/"src/effects/maelstromoriginal.cpp").read_text(),"static bool ValidateMaelstrom(")
 prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes))
 records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cls=CLASS.replace('#include "staticpartsysprofiles.h"','#include "maelstromprofile.h"\n#include "maelstrommatrix.h"')
 cls=cls.replace('std::array<int,40>parent{},keycount{};std::array<void*,40>keyptr{};', 'std::array<std::array<int,3>,40>parent{},keycount{};std::array<std::array<void*,3>,40>keyptr{};int lengths[3]{},af[3]{};')
 cls=cls.replace('std::array<std::vector<SAniKey32>,40>keys;', 'std::array<std::array<std::vector<SAniKey32>,3>,40>keys;')
 cls=cls.replace('int GetAniLength(int){return frames;}int GetAniFlags(int){return aflags;}', 'int GetAniLength(int s){return lengths[s];}int GetAniFlags(int s){return af[s];}')
 cls=cls.replace('void Select(){static_particles_profile=ValidateRetailStaticParticleProfile();}\n uint32_t ValidateRetailStaticParticleProfile();','bool ValidateRetailMaelstromProfile();')
 cls=cls.replace('TYPED_METHOD','bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&,bool=false);').replace('bool null_filename=false;', '''bool null_filename=false,meshinitialized=true;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;
 void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}''')
 start=cls.index(' memcpy(obj.name');end=cls.index('obj.material=',start)
 cls=cls[:start]+''' memcpy(obj.name,b.data()+o,32);
 for(int s=0;s<1;++s){const size_t st=P(b,o+44)+12*s;im.parent[i][s]=int32_t(U(b,st));im.keycount[i][s]=U(b,st+4);im.keys[i][s].resize(im.keycount[i][s]);memcpy(im.keys[i][s].data(),b.data()+P(b,st+8),4*im.keycount[i][s]);im.keyptr[i][s]=im.keys[i][s].data();}
 obj.parent=im.parent[i].data();obj.numanikeys=im.keycount[i].data();obj.anikeys=im.keyptr[i].data();'''+cls[end:]
 cls=cls.replace('S3DTex,1>','S3DTex,4>').replace('S3DMat,3>','S3DMat,4>').replace('std::array<int,2>,40>facecounts','std::array<int,5>,40>facecounts').replace('t<2;++t','t<5;++t').replace(',40>',',60>')
 cls=cls.replace('S3DTag,2>','S3DTag,5>').replace('std::string,2>','std::string,5>')
 # Four textures preserve all five authored face bins.
 cls=cls.replace(' auto&t=im.textures[0];',' for(int st=0;st<1;++st){im.lengths[st]=H(b,70+76*st);im.af[st]=H(b,68+76*st);}\n for(int k=0;k<4;++k){const size_t tex=P(b,body+40)+k*120;auto&t=im.textures[k];').replace('U(b,tex+100)};','U(b,tex+100)};}')
 cls=cls.replace('bool ValidateRetailMaelstromProfile();','void GetObject(int i,S3DObj* o){*o=objects[i];}void GetMaterial(int i,S3DMat* m){*m=materials[i];}void GetTexture(int i,S3DTex* t){*t=textures[i];}')
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n')
 main=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b,"magic/maelstrom.i3d");assert(ValidateMaelstrom(im));
int controls=0;
auto reject=[&](auto mutate){T3DImagery bad;Load(bad,b,"magic/maelstrom.i3d");mutate(bad);assert(!ValidateMaelstrom(bad));++controls;};
reject([](auto&v){v.textures[0].desc.pixelFormat.dwRGBAlphaBitMask=0xf000;});
reject([](auto&v){v.tags[0].str="filename vapor";});
reject([](auto&v){++v.tags[0].frame;});
reject([](auto&v){v.materials[1].texture=-1;});
reject([](auto&v){reinterpret_cast<unsigned char*>(v.verts[1].data())[0]^=1;});
reject([](auto&v){reinterpret_cast<unsigned char*>(v.faces[1].data())[0]^=1;});
for(int o=0;o<51;++o)for(int st=0;st<1;++st){im.parent[o][st]=0;assert(!ValidateMaelstrom(im));im.parent[o][st]=-1;++controls;for(int k=0;k<im.keycount[o][st];++k){auto*p=(uint32_t*)&im.keys[o][st][k];*p^=1;assert(!ValidateMaelstrom(im));*p^=1;++controls;}}
for(int st=0;st<1;++st){++im.lengths[st];assert(!ValidateMaelstrom(im));--im.lengths[st];++controls;}
int defaults=0,changed=0;for(int st=0;st<1;++st)for(int o=0;o<im.nobj;++o)for(int t=0;t<im.lengths[st];++t){hmm_vec3 p{},r{},s{},dp{},dr{},ds{},fp{},fr{},fs{};
assert(im.GetUninterpolatedAniKey(o,st,t,dp,dr,ds));assert(im.GetUninterpolatedAniKey(o,st,t,fp,fr,fs,false));assert(!memcmp(&dp,&fp,sizeof dp)&&!memcmp(&dr,&fr,sizeof dr)&&!memcmp(&ds,&fs,sizeof ds));++defaults;
assert(im.GetUninterpolatedAniKey(o,st,t,p,r,s,true));changed+=memcmp(&p,&dp,sizeof p)||memcmp(&r,&dr,sizeof r)||memcmp(&s,&ds,sizeof s);
hmm_mat4 m;BuildRetailMaelstromMatrix(m,o,p,r,s);
printf("%d %d %d",st,o,t);for(float v:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z})printf(" %.9g",v);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}
fprintf(stderr,"profile_mutations=%d default_unchanged=%d inclusive_changed=%d\n",controls,defaults,changed);}
'''
 driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+'\n'.join(methods.values())+'\n'+main);binary=output/'pose'
 command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)]
 q=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Source compile failed; see compile.log')
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/maelstrom.I3D')
 path=output/'asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'controls.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Source controls failed; see controls.log')
 port={(int(p[0]),int(p[1]),int(p[2])):[struct.unpack('<f',struct.pack('<f',float(x)))[0]for x in p[3:]]for l in q.stdout.splitlines()if(p:=l.split())}
 f=MaelstromScene(executable,data);v=f.vm;vectors=v.allocate(36);key_max=matrix_max=0;key_fields=matrix_fields=0;errors=[]
 for(st,o,t),current in port.items():
  assert v.call(0x409430,(o,st,t,vectors,vectors+12,vectors+24),this=f.imagery)==1
  original=struct.unpack('<9f',v.uc.mem_read(vectors,36));delta=max(abs(a-b)for a,b in zip(original,current[:9]));key_max=max(key_max,delta);key_fields+=9
  if delta>3e-5:errors.append(dict(state=st,object=o,tick=t,key_error=delta))
  accepted=v.call(0x40a420,(f.animobjs[o],st,t,0,0),this=f.imagery)==1
  if accepted!=all(s>1e-5 for s in current[6:9]):errors.append(dict(state=st,object=o,tick=t,scale_gate=accepted))
  if not accepted:continue
  original=struct.unpack('<16f',v.uc.mem_read(f.animobjs[o]+0x58,64));delta=max(abs(a-b)for a,b in zip(original,current[9:]));matrix_max=max(matrix_max,delta);matrix_fields+=16
  if delta>2e-4:errors.append(dict(state=st,object=o,tick=t,matrix_error=delta))
 report=dict(status='fail'if errors else'pass',key_fields=key_fields,matrix_fields=matrix_fields,max_key_error=key_max,max_matrix_error=matrix_max,errors=errors,controls=q.stderr,asset_sha256=sha(data),source_spans={s:sha(b.encode())for s,b in methods.items()},driver_sha256=sha(driver.read_bytes()),production_sha256={p:sha((ROOT/p).read_bytes())for p in('src/3dimage.cpp','src/maelstromprofile.h','src/maelstrommatrix.h','src/effects/maelstromoriginal.cpp','src/goldauthoredmatrix.h','src/speedauthoredmatrix.h','src/math3d.cpp')},sanitizers='ASan/UBSan',accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],key_fields,matrix_fields,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.executable,a.output)
