#!/usr/bin/env python3
"""Actual source decoder/default pose branch versus every native Worm root."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from essencedrain_native_preflight import NativeEssence,ROOT,ASSET_SHA
from jtele_pose_probe import CLASS
from speed_profile_contract import PRELUDE,function,sha
MAIN=r'''
namespace essencedrain_profile {HEADER_PREFIX}
struct Owner{int frame;int GetFrame(){return frame;}};
struct State{Owner*owner;};
bool ProductionPose(T3DImagery*im,int object,int frame,hmm_mat4&out){Owner owner{frame};State holder{&owner};auto*s=&holder;struct{int object;}part{object};for(int once=0;once<1;++once){POSE_BRANCH out=local;return true;}return false;}
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t v;memcpy(&v,b.data()+p,4);return v;}size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});size_t bo=20+U(b,16),objs=P(b,bo+48);T3DImagery im;im.flags=U(b,bo);im.frames=200;for(int i=0;i<29;++i){size_t st=P(b,objs+i*48+44);im.counts[i]=U(b,st+4);im.keys[i].resize(im.counts[i]);memcpy(im.keys[i].data(),b.data()+P(b,st+8),im.counts[i]*4);im.ptrs[i]=im.keys[i].data();im.objects[i].numanikeys=&im.counts[i];im.objects[i].anikeys=&im.ptrs[i];}
for(int o=0;o<29;++o)for(int t=0;t<200;++t){hmm_vec3 p{},r{},scale{};bool key=im.counts[o]>0&&im.GetUninterpolatedAniKey(o,0,t,p,r,scale,true);hmm_mat4 m;MtxClear(&m);bool valid=ProductionPose(&im,o,t,m);printf("%d %d %d %d",o,t,int(key),int(valid));for(float v:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,scale.X,scale.Y,scale.Z})printf(" %.9g",v);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(');methods={s:function(source,s)for s in signatures};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"');cls=CLASS.split('uint32_t U(')[0].replace('array<S3DObj,8>','array<S3DObj,29>').replace('array<int,8>','array<int,29>').replace('array<void*,8>','array<void*,29>').replace('array<std::vector<SAniKey32>,8>','array<std::vector<SAniKey32>,29>');header=(ROOT/'src/game/effects/essencedrainprofile.h').read_text();header=header[header.index('struct Object'):header.index('inline bool Matches')];preview=(ROOT/'src/game/effects/legacy/essencedrainpreview.cpp').read_text();a=preview.index('hmm_mat4 local;MtxClear(');b=preview.index(' // Original zero-key roots',a);branch=preview[a:b];driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+function(body,'struct SAniKey32\n')+';\n'+cls+'\n'+'\n'.join(methods.values())+'\n'+MAIN.replace('HEADER_PREFIX',header).replace('POSE_BRANCH',branch));binary=output/'pose';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/worm.i3d'))
 asset=output/'asset.fixture';asset.write_bytes(data);q=subprocess.run([str(binary),str(asset)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);assert q.returncode==0 and not q.stderr;f=NativeEssence(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data);f.initialize();v=f.vm;ptr=v.allocate(36);key_fields=matrix_fields=0;key_max=matrix_max=0;rejected=aux=0;errors=[]
 for line in q.stdout.splitlines():
  p=line.split();o,t,key,valid=map(int,p[:4]);values=list(map(float,p[4:]));nk=v.call(0x409430,(o,0,t,ptr,ptr+12,ptr+24),this=f.imagery)
  if key and not nk:errors.append(dict(object=o,frame=t,kind='key_return'))
  if key:
   native=struct.unpack('<9f',v.uc.mem_read(ptr,36));delta=max(abs(a-b)for a,b in zip(native,values[:9]));key_fields+=9;key_max=max(key_max,delta)
   if delta>3e-5:errors.append(dict(object=o,frame=t,kind='key_numeric',error=delta))
  ok=v.call(0x40a420,(f.animobjs[o],0,t,0,0),this=f.imagery)
  if bool(ok)!=bool(valid):errors.append(dict(object=o,frame=t,kind='matrix_return',native=ok,source=valid))
  if not ok:rejected+=1;continue
  native=struct.unpack('<16f',v.uc.mem_read(f.animobjs[o]+0x58,64));delta=max(abs(a-b)for a,b in zip(native,values[9:]));matrix_fields+=16;matrix_max=max(matrix_max,delta)
  if not key:aux+=1
  if delta>2e-4:errors.append(dict(object=o,frame=t,kind='matrix_numeric',error=delta))
 report=dict(status='pass'if not errors else'fail',accepted=False,asset_sha256=ASSET_SHA,key_fields=key_fields,matrix_fields=matrix_fields,zero_key_identity_matrices=aux,native_scale_rejections=rejected,max_key_error=key_max,max_matrix_error=matrix_max,errors=errors,sanitizers='ASan/UBSan',actual_pose_span_sha256=sha(branch.encode()),source_spans={s:sha(b.encode())for s,b in methods.items()},scope='All3×200 keyed409430 channels and all29×200 original40a420 matrix results versus compiled actual production decoder/pose span. Zero-key original409430 succeeds with zero scale, while the existing source generic fallback supplies scale1; that API difference remains separately open. The actual typed pose bypasses that API on those26roots and matches every actual native40a420 identity matrix. All26zero-key roots remain visible identity matrices; only keyed zero/negative scales reject. No native pose injection or identity replacement for keyed failures. Whole caller/UV/lifetime/native raster/actualMetal separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],key_fields,matrix_fields,aux,rejected,key_max,matrix_max,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
