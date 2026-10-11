#!/usr/bin/env python3
"""Actual production integer decoder and matrices versus all Jtele native poses."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from jtele_native_preflight import NativeJtele,ROOT,PROFILES
from speed_profile_contract import PRELUDE,function,sha
CLASS=r'''
class T3DImagery{public:
 bool meshinitialized=true,retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;uint32_t static_particles_profile=0;int flags=0,frames=0;
 std::array<S3DObj,8>objects;std::array<int,8>counts{};std::array<void*,8>ptrs{};std::array<std::vector<SAniKey32>,8>keys;
 int NumStates(){return 1;}int GetAniLength(int){return frames;}void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}
 bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&,bool=false);
};
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t v;memcpy(&v,b.data()+p,4);return v;}size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});size_t bo=20+U(b,16),objs=P(b,bo+48);T3DImagery im;im.flags=U(b,bo);uint16_t frames;memcpy(&frames,b.data()+70,2);im.frames=frames;assert(U(b,bo+44)==8&&frames==50);
for(int i=0;i<8;++i){size_t st=P(b,objs+i*48+44);im.counts[i]=U(b,st+4);im.keys[i].resize(im.counts[i]);memcpy(im.keys[i].data(),b.data()+P(b,st+8),im.counts[i]*4);im.ptrs[i]=im.keys[i].data();im.objects[i].numanikeys=&im.counts[i];im.objects[i].anikeys=&im.ptrs[i];}
for(int o=0;o<8;++o)for(int t=0;t<50;++t){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(o,0,t,p,r,s,true));hmm_mat4 m;if(o==0||(o>=2&&o<=4))BuildRetailSpeedCameraMatrix(m,p,r,s);else {MtxClear(&m);MakeMatrix(m,p,r,s);}printf("%d %d",o,t);for(float v:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z})printf(" %.9g",v);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in signatures};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"');driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+function(body,'struct SAniKey32\n')+';\n'+CLASS.split('uint32_t U(')[0]+'\n'+'\n'.join(methods.values())+'\nuint32_t U('+CLASS.split('uint32_t U(')[1]);binary=output/'pose';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name+'.i3d'));assert sha(data)==digest;asset=output/(name+'.fixture');asset.write_bytes(data);q=subprocess.run([str(binary),str(asset)],capture_output=True,text=True);(output/(name+'.trace')).write_text(q.stdout);assert q.returncode==0 and not q.stderr;port={(int(p[0]),int(p[1])):[struct.unpack('<f',struct.pack('<f',float(x)))[0]for x in p[2:]]for line in q.stdout.splitlines()if(p:=line.split())};f=NativeJtele(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name,False);f.initialize_scene();v=f.vm;vectors=v.allocate(36);key_max=matrix_max=0;key_fields=matrix_fields=0;rejected=[];errors=[]
   for(o,t),cur in port.items():
    assert v.call(0x409430,(o,0,t,vectors,vectors+12,vectors+24),this=f.imagery)==1;native=struct.unpack('<9f',v.uc.mem_read(vectors,36));delta=max(abs(a-b)for a,b in zip(native,cur[:9]));key_max=max(key_max,delta);key_fields+=9
    if delta>3e-5:errors.append(dict(object=o,frame=t,key_error=delta))
    ok=v.call(0x40a420,(f.animobjs[o],0,t,0,0),this=f.imagery)==1
    if not ok:rejected.append([o,t]);continue
    native=struct.unpack('<16f',v.uc.mem_read(f.animobjs[o]+0x58,64));delta=max(abs(a-b)for a,b in zip(native,cur[9:]));matrix_max=max(matrix_max,delta);matrix_fields+=16
    if delta>2e-4:errors.append(dict(object=o,frame=t,matrix_error=delta))
   cases.append(dict(name=name,type_id=id,status='pass'if not errors else'fail',key_fields=key_fields,matrix_fields=matrix_fields,max_key_error=key_max,max_matrix_error=matrix_max,native_rejected_zero_scale_matrices=rejected,errors=errors,asset_sha256=digest));print(name,cases[-1]['status'],key_fields,matrix_fields,len(errors),flush=True)
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',accepted=False,cases=cases,source_spans={s:sha(v.encode())for s,v in methods.items()},driver_sha256=sha(driver.read_bytes()),sanitizers='ASan/UBSan',scope='All8×50×9ownJtele/JteledTRSchannels via compiled actual source integerdecoder with explicitnativeinclusive caller versus409430; source normalMakeMatrix or audited#cameraSRT helper vsactual40a420 flags createdbyoriginalSetupObjects. Nativezero-scale matrixrejections retained, no substitute/cacheoverwrite. This is key/matrix evidence, not pixels or controller equivalence.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
