#!/usr/bin/env python3
"""Independent compiled Skin keys/cached emitter/PartSys against whole x86 caller."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from speed_profile_contract import PRELUDE,CLASS,function,sha
from skin_visual_reference import NativeSkin,PROFILES,ROOT
MAIN=r'''
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b);
authored_partsys::Definition d;std::string err;assert(authored_partsys::ParseDefinition(im.tags[1].str,d,err));authored_partsys::State sim;authored_partsys::TickInputs in;in.emitters.resize(1);in.ground_height=[](int,int,int){return 0;};
auto pose=[&](int frame){for(int i=0;i<1;++i){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(1,0,frame,p,r,s,true));hmm_mat4 m;MtxClear(&m);MakeMatrix(m,p,r,s);auto&e=in.emitters[i];memcpy(e.matrix.data(),m.Elements,64);e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};e.position={p.X,p.Y,p.Z};e.scale={s.X,s.Y,s.Z};}in.animation_frame=frame;};
pose(0);hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(0,0,0,p,r,s,true));in.has_object_zero_origin=true;in.object_zero_origin={p.X,p.Y,p.Z};assert(sim.Initialize(d,in,0,err));uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767)% (b-a+1);};
for(int tick=0;tick<=91;++tick){if(tick)sim.Advance(in,rng);if(tick%STRIDE==0)for(size_t i=0;i<sim.Particles().size();++i)if(sim.Particles()[i].alive){sim.SampleRender(i,rng);sim.SampleTrailPositions(i);}
printf("F %d %u %.9g\n",tick,seed,sim.EmissionCredit());for(size_t i=0;i<sim.Particles().size();++i){const auto&q=sim.Particles()[i];if(!q.alive)continue;printf("P %d %zu %d %d",tick,i,q.age,q.lifespan);for(float v:{q.position[0],q.position[1],q.position[2],q.velocity[0],q.velocity[1],q.velocity[2],q.scale,q.color[0],q.color[1],q.color[2],q.alpha})printf(" %.9g",v);for(const auto&pos:q.trail_positions)for(float v:pos)printf(" %.9g",v);puts("");}if(tick%STRIDE==0)pose(tick%30);}}
'''
def run(executable,archive,output,name,stride=1):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();render=(ROOT/'src/engine/render/render3d_types.h').read_text()
 sigs=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(');methods={s:function(source,s)for s in sigs};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"\n#include "authoredpartsys.h"');cls=CLASS.replace(',3>',',17>').replace(',2>',',3>').replace('i<3;++i){const size_t o=','i<3;++i){const size_t o=').replace('t<3;++t','t<im.ntex+1;++t').replace('std::array<int,17>,17>facecounts','std::array<int,4>,17>facecounts').replace('i<2;++i){const size_t p=tex','i<im.ntex;++i){const size_t p=tex').replace('i<3;++i){memcpy(&im.materials','i<im.nmat;++i){memcpy(&im.materials').replace('void Select(){speed_partsys_profile=ValidateRetailSpeedPartSysProfile();}','').replace('i<2;++i){const size_t p=tags','i<im.ntags;++i){const size_t p=tags')
 driver=output/'state.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+'\n'.join(methods.values())+'\n#define STRIDE '+str(stride)+'\n'+MAIN);binary=output/'state';command=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),*[str(ROOT/'src'/p)for p in('math3d.cpp','authoredpartsys.cpp','partsysdefinition.cpp')],'-o',str(binary)];q=subprocess.run(command,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('Compile failed')
 with zipfile.ZipFile(archive)as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+PROFILES[name]['asset']+'.i3d'))
 path=output/'skin.asset-input';path.write_bytes(data);q=subprocess.run([str(binary),str(path)],capture_output=True,text=True);(output/'source.trace').write_text(q.stdout);(output/'source.log').write_text(q.stderr)
 if q.returncode:raise RuntimeError('Source failed')
 port={};frames={}
 for line in q.stdout.splitlines():
  p=line.split()
  if p[0]=='F':frames[int(p[1])]=(int(p[2]),float(p[3]))
  else:port[(int(p[1]),int(p[2]))]=list(map(float,p[3:]))
 f=NativeSkin(executable,data,name);f.initialize_scene();f.raster_enabled=False;v=f.vm;c=f.controllers[1];pool=v.u32(c+0x2c);errors=[];maximum=0;fields=0;native=[]
 for tick in range(92):
  if tick:f.advance()
  if tick%stride==0:f.render()
  credit=struct.unpack('<f',v.uc.mem_read(c+0x16c,4))[0]
  if frames[tick][0]!=v.rng_seed or abs(frames[tick][1]-credit)>1e-6:errors.append(dict(tick=tick,seed=[frames[tick][0],v.rng_seed],credit=[frames[tick][1],credit]))
  for slot in range(v.u32(c+0xf8)):
   p=pool+slot*252;alive=bool(v.u32(p+0xe4));current=port.get((tick,slot));assert alive==(current is not None)
   if not alive:continue
   row=[v.u32(p+0xd0),v.u32(p+0xd4),*[struct.unpack('<f',v.uc.mem_read(p+off,4))[0]for off in(0xb8,0xbc,0xc0,0xc4,0xc8,0xcc,0x64,0x50,0x54,0x58,0x60)]];row+=list(struct.unpack('<'+'f'*(v.u32(p+0xf8)*3),v.uc.mem_read(v.u32(p+0xf4),v.u32(p+0xf8)*12)));assert len(row)==len(current);delta=max(abs(a-b)for a,b in zip(row,current));maximum=max(maximum,delta);fields+=len(row);native.append([tick,slot,*row])
   if delta>2e-4:errors.append(dict(tick=tick,slot=slot,error=delta,native=row,source=current))
 report=dict(float_tolerance=0.0002,float_limit='Independent particles and every render-history position; exact RNG, age, lifespan and alive',status='fail'if errors else'pass',ticks=92,render_stride=stride,fields=fields,max_error=maximum,errors=errors,asset_sha256=sha(data),driver_sha256=sha(driver.read_bytes()),source_sha256={p:sha((ROOT/'src'/p).read_bytes())for p in('authoredpartsys.cpp','partsysdefinition.cpp','partsysdefinition.h')},accepted=False)
 (output/'native.json').write_text(json.dumps(native));(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],fields,maximum,len(errors));return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--name',choices=PROFILES,required=True);p.add_argument('--stride',type=int,choices=(1,3),default=1);a=p.parse_args();run(a.executable,a.archive,a.output,a.name,a.stride)
