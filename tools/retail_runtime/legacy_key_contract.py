#!/usr/bin/env python3
"""Actual two-word legacy keys and production decoder vs original409430."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import PRELUDE,function,sha
from speed_controller_preflight import NativeSpeed,read_asset,ROOT
MAIN=r'''
int main(int argc,char**argv){static_assert(sizeof(SAniKey)==8);assert(argc==2);
// Setters preserve unrelated fields and the unused top bit across every
// representable signed endpoint, including Z's cross-word high/low boundary.
// Original ak_setrz additionally clears the unused topbit; retain that behavior.
for(int axis=0;axis<6;++axis){const int low=axis<3?-2048:-256,high=axis<3?2047:255;
for(int value=low;value<=high;++value){SAniKey key{{int32_t(0xabcdef12u),int32_t(0xd2345678u)}};const uint64_t before=uint64_t(uint32_t(key.data[0]))|(uint64_t(uint32_t(key.data[1]))<<32);int decoded=0,shift=axis<3?axis*12:36+(axis-3)*9,bits=axis<3?12:9;
switch(axis){case0:ak_setx(key,value);decoded=ak_x(key);break;case1:ak_sety(key,value);decoded=ak_y(key);break;case2:ak_setz(key,value);decoded=ak_z(key);break;case3:ak_setrx(key,value);decoded=ak_rx(key);break;case4:ak_setry(key,value);decoded=ak_ry(key);break;case5:ak_setrz(key,value);decoded=ak_rz(key);break;}
const uint64_t after=uint64_t(uint32_t(key.data[0]))|(uint64_t(uint32_t(key.data[1]))<<32),mask=(((uint64_t(1)<<bits)-1)<<shift)|(axis==5?(uint64_t(1)<<63):0);assert(decoded==value);assert((before&~mask)==(after&~mask));if(axis==5)assert(!(after&(uint64_t(1)<<63)));}}
std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b);
for(int object=0;object<im.nobj;++object)for(int frame=0;frame<im.frames;++frame){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(object,0,frame,p,r,s));printf("%d %d",object,frame);for(float x:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z})printf(" %.9g",x);puts("");}}
'''.replace('case0','case 0').replace('case1','case 1').replace('case2','case 2').replace('case3','case 3').replace('case4','case 4').replace('case5','case 5')
CLASS=r'''
class T3DImagery{public:
bool meshinitialized=true,retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;uint32_t static_particles_profile=0;
int flags=0,nobj=0,frames=0,nstates=0;std::array<S3DObj,64>objects;std::array<std::vector<SAniKey>,64>keys;std::array<void*,64>keyptr{};std::array<int,64>count{};
int NumStates(){return nstates;}int GetAniLength(int){return frames;}void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&,bool=false);};
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t x;memcpy(&x,b.data()+p,4);return x;}size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
void Load(T3DImagery&im,const std::vector<unsigned char>&b){size_t body=20+U(b,16),objects=P(b,body+48);im.flags=U(b,body);im.nobj=U(b,body+44);im.nstates=U(b,24);uint16_t frames;memcpy(&frames,b.data()+70,2);im.frames=frames;assert(!(im.flags&I3D_ANIKEY32)&&im.nobj<=64);
for(int i=0;i<im.nobj;++i){size_t st=P(b,objects+i*48+44),key=P(b,st+4);assert(key+frames*sizeof(SAniKey)<=b.size());im.keys[i].resize(frames);memcpy(im.keys[i].data(),b.data()+key,frames*sizeof(SAniKey));im.keyptr[i]=im.keys[i].data();im.count[i]=frames;im.objects[i].anikeys=&im.keyptr[i];im.objects[i].numanikeys=&im.count[i];}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);body=(ROOT/'src/3dimagebody.h').read_text();source=(ROOT/'src/3dimage.cpp').read_text();methods={s:function(source,s)for s in ['static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(']};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));oldmacros=body[body.index('#define ak_x('):body.index('\n',body.index('#define ak_setrz('))];prelude=PRELUDE[:PRELUDE.index('struct SAniKey {')]+PRELUDE[PRELUDE.index('#define POSCHANGED'):];records=function(body,'struct SAniKey\n')+';\n'+function(body,'struct SAniKey32\n')+';\n'
 driver=output/'legacy-key.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+oldmacros+'\n'+CLASS+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'legacy-key';cmd=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',str(driver),'-o',str(binary)];q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Legacy key contract compilation failed')
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/iced.i3d'))
 path=output/'Snow.asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source-trace.txt').write_text(q.stdout);(output/'run.log').write_text(q.stderr)
 if q.returncode or q.stderr:raise RuntimeError('Legacy source decoder/setters failed')
 poses={(int(p[0]),int(p[1])):[float(x)for x in p[2:]]for line in q.stdout.splitlines()if(p:=line.split())};asset=read_asset(data);asset['name']='Snow';native=NativeSpeed(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',asset,particle_required=False);v=native.vm;vectors=v.allocate(36);errors=[];maximum=0;channels=0
 for object in range(50):
  for frame in range(100):
   assert v.call(0x409430,(object,0,frame,vectors,vectors+12,vectors+24),this=native.imagery)==1;original=struct.unpack('<9f',v.uc.mem_read(vectors,36));port=poses[(object,frame)];error=max(abs(x-y)for x,y in zip(original,port));maximum=max(maximum,error);channels+=9
   if error>3e-6:errors.append(dict(object=object,frame=frame,error=error,native=original,source=port))
 report=dict(status='pass'if not errors else'differences_found',accepted=False,errors=errors,name='Snow',channels=channels,objects=50,frames=100,max_absolute_error=maximum,serialized_key_size=8,setter_cases=3*4096+3*512,sanitizers='ASan/UBSan',asset_sha256=sha(data),header_sha256=sha(body.encode()),source_decoder_sha256=sha(methods['bool T3DImagery::GetUninterpolatedAniKey('].encode()),source_spans={s:sha(m.encode())for s,m in methods.items()},driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),native_constructor=native.constructor,scope='Actual serialized two32-bit word record/getters/setters and production GetUninterpolatedAniKey against original409430 for all50×100×9 exact Snow channels. Every representable getter/setter value and preservation of all otherfields and original RZ unusedtopbit clear underASan/UBSan. Loader allocation/copy automatically uses asserted8byte sizeof; old-body zero-key fallback policy unchanged. No images/pixel/naturalcaller acceptance from decoder proof.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],channels,maximum);return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=run(a.output);raise SystemExit(r['status']!='pass')
