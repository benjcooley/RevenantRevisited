"""Compile production key/matrix/PartSys code and compare genuine Nakrnoth trace."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import PRELUDE,CLASS,function,sha,ROOT
MAIN=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b);assert(ValidateNakrnoth(im));
++im.frames;assert(!ValidateNakrnoth(im));--im.frames;
im.textures[0].desc.pixelFormat.dwRGBAlphaBitMask=0xf000;assert(!ValidateNakrnoth(im));im.textures[0].desc.pixelFormat.dwRGBAlphaBitMask=0;
++im.materials[0].texture;assert(!ValidateNakrnoth(im));--im.materials[0].texture;
for(int o=0;o<63;++o){im.parent[o]=0;assert(!ValidateNakrnoth(im));im.parent[o]=-1;auto*word=reinterpret_cast<uint32_t*>(im.keys[o].data());*word^=1;assert(!ValidateNakrnoth(im));*word^=1;}
auto tag=im.tags[1].str;im.tags[1].str="obj=wrong,particle=wrong";assert(!ValidateNakrnoth(im));im.tags[1].str=tag;assert(ValidateNakrnoth(im));
fprintf(stderr,"130 exact-profile rejection controls PASS\n");
authored_partsys::Definition d;std::string err;assert(authored_partsys::ParseDefinition(im.tags[1].str,d,err));authored_partsys::State sim;authored_partsys::TickInputs in;in.owner_position={3185,3185,16};in.emitters.resize(10);in.ground_height=[](int,int,int){return 0;};
auto pose=[&](int frame,bool emit){for(int o=0;o<im.nobj;++o){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(o,0,frame,p,r,s,true));hmm_mat4 m;MtxClear(&m);if(im.objects[o].name[0]=='#')BuildRetailSpeedCameraMatrix(m,p,r,s);else MakeMatrix(m,p,r,s);
if(o>=12&&o<=21){auto&e=in.emitters[o-12];memcpy(e.matrix.data(),m.Elements,64);e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};e.position={p.X,p.Y,p.Z};e.scale={s.X,s.Y,s.Z};}
if(emit && s.X>1e-5f&&s.Y>1e-5f&&s.Z>1e-5f){printf("M %d %d",frame,o);for(int k=0;k<16;++k)printf(" %.9g",(&m.Elements[0][0])[k]);puts("");}}
in.animation_frame=frame;};
pose(0,false);hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(0,0,0,p,r,s,true));in.has_object_zero_origin=true;in.object_zero_origin={p.X,p.Y,p.Z};assert(sim.Initialize(d,in,0,err));uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767)% (b-a+1);};
for(int tick=1;tick<=774;++tick){sim.Advance(in,rng);for(size_t i=0;i<sim.Particles().size();++i)if(sim.Particles()[i].alive)sim.SampleRender(i,rng);
printf("F %d %u %.9g\n",tick,seed,sim.EmissionCredit());for(size_t i=0;i<sim.Particles().size();++i){const auto&q=sim.Particles()[i];if(!q.alive)continue;printf("P %d %zu %d %d",tick,i,q.age,q.lifespan);for(float v:{q.position[0],q.position[1],q.position[2],q.velocity[0],q.velocity[1],q.velocity[2],q.scale,q.color[0],q.color[1],q.color[2],q.alpha})printf(" %.9g",v);puts("");}pose(tick,true);}}
'''
def compile_source(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();render=(ROOT/'src/engine/render/render3d_types.h').read_text()
 sigs=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in sigs};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"\n#include "authoredpartsys.h"\n#include "nakrnothprofile.h"')
 cls=CLASS.replace(',3>',',64>').replace(',2>',',8>').replace('i<3;++i){const size_t o=','i<im.nobj;++i){const size_t o=').replace('t<3;++t','t<im.ntex+1;++t').replace('i<2;++i){const size_t p=tex','i<im.ntex;++i){const size_t p=tex').replace('i<3;++i){memcpy(&im.materials','i<im.nmat;++i){memcpy(&im.materials').replace('void Select(){speed_partsys_profile=ValidateRetailSpeedPartSysProfile();}','').replace('i<2;++i){const size_t p=tags','i<im.ntags;++i){const size_t p=tags')
 cls=cls.replace('bool ValidateRetailSpeedPartSysProfile();','void GetObject(int i,S3DObj*o){*o=objects[i];}void GetMaterial(int i,S3DMat*m){*m=materials[i];}void GetTexture(int i,S3DTex*t){*t=textures[i];}')
 methods['validation']=function((ROOT/'src/game/effects/legacy/nakrnothoriginal.cpp').read_text(),'static bool ValidateNakrnoth(')
 driver=output/'source.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'source'
 q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),*[str(ROOT/'src'/p)for p in('math3d.cpp','authoredpartsys.cpp','partsysdefinition.cpp')],'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0,'See compile.log'
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/nakrnoth.i3d'))
 path=output/'asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'source.log').write_text(q.stderr);assert q.returncode==0,'See source.log'
 return q.stdout

def compare(trace,native,output):
 frames={};particles={};matrices={}
 for line in trace.splitlines():
  p=line.split();t=int(p[1])
  if p[0]=='F':frames[t]=(int(p[2]),float(p[3]))
  elif p[0]=='P':particles[(t,int(p[2]))]=list(map(float,p[3:]))
  else:matrices[(t,int(p[2]))]=list(map(float,p[3:]))
 errors=[];maximum=0;fields=0;matrix_max=0;matrix_fields=0
 for row in json.loads(native.read_text())['records']:
  t=row['tick']
  if t not in frames:continue
  c=next(x for x in row['controllers']if x['vtable']=='0x5a3544');pool=bytes.fromhex(c['pool_hex'])
  if frames[t][0]!=row['state']['seed']or abs(frames[t][1]-c['credit'])>1e-6:errors.append(dict(tick=t,seed=[frames[t][0],row['state']['seed']],credit=[frames[t][1],c['credit']]))
  for slot in range(c['capacity']):
   off=slot*252;alive=bool(struct.unpack_from('<I',pool,off+0xe4)[0]);current=particles.get((t,slot))
   if alive!=(current is not None):errors.append(dict(tick=t,slot=slot,alive=alive));continue
   if not alive:continue
   original=[*struct.unpack_from('<2I',pool,off+0xd0),*[struct.unpack_from('<f',pool,off+x)[0]for x in(0xb8,0xbc,0xc0,0xc4,0xc8,0xcc,0x64,0x50,0x54,0x58,0x60)]];delta=max(abs(a-b)for a,b in zip(original,current));maximum=max(maximum,delta);fields+=len(original)
   if delta>0.0021:errors.append(dict(tick=t,slot=slot,error=delta,native=original,source=current))
  for o,original in enumerate(row['state']['matrices']):
   if o==0:continue # controller mutates its private particle prototype during Render
   current=matrices.get((t,o))
   if current is None:continue
   delta=max(abs(a-b)for a,b in zip(original,current));matrix_max=max(matrix_max,delta);matrix_fields+=16
   if delta>0.0002:errors.append(dict(tick=t,object=o,matrix_error=delta))
 report=dict(status='fail'if errors else'pass',position_tolerance=0.0021,position_limit='Accumulated float world-coordinate rounding at3185; all RNG/alive/age/lifespan values exact',prototype_limit='Object0 is mutated by controller Render and is excluded only from ordinary base-matrix comparison',fields=fields,max_error=maximum,matrix_fields=matrix_fields,matrix_max=matrix_max,errors=errors,native_sha256=sha(native.read_bytes()),source_sha256={p:sha((ROOT/'src'/p).read_bytes())for p in('3dimage.cpp','math3d.cpp','authoredpartsys.cpp','partsysdefinition.cpp','effects/nakrnothoriginal.cpp','nakrnothprofile.h','cometsoftwarebounds.h')},driver_sha256=sha((output/'source.cpp').read_bytes()),accepted=False)
 (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,matrix_fields,matrix_max,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args();trace=compile_source(a.output)
 if a.native:compare(trace,a.native,a.output)
