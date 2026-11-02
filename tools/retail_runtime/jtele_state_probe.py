#!/usr/bin/env python3
"""Independent production keys/cache/PartSys versus complete native Jtele caller."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from jtele_native_preflight import NativeJtele,ROOT,PROFILES
from jtele_pose_probe import CLASS
from speed_profile_contract import PRELUDE,function,sha
MAIN=r'''
uint32_t U(const std::vector<unsigned char>&b,size_t p){assert(p+4<=b.size());uint32_t v;memcpy(&v,b.data()+p,4);return v;}size_t P(const std::vector<unsigned char>&b,size_t p){return p+U(b,p);}
int main(int argc,char**argv){assert(argc==3);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});size_t bo=20+U(b,16),objs=P(b,bo+48);T3DImagery im;im.flags=U(b,bo);im.frames=50;for(int i=0;i<8;++i){size_t st=P(b,objs+i*48+44);im.counts[i]=U(b,st+4);im.keys[i].resize(im.counts[i]);memcpy(im.keys[i].data(),b.data()+P(b,st+8),im.counts[i]*4);im.ptrs[i]=im.keys[i].data();im.objects[i].numanikeys=&im.counts[i];im.objects[i].anikeys=&im.ptrs[i];}
authored_partsys::Definition d;std::string err;assert(authored_partsys::ParseDefinition(argv[2],d,err));authored_partsys::State sim;authored_partsys::TickInputs in;in.emitters.resize(3);in.ground_height=[](int,int,int){return 0;};in.has_object_zero_origin=true;in.object_zero_origin={0,0,0};for(auto&e:in.emitters){e.scale={0,0,0};e.matrix.fill(0);}assert(sim.Initialize(d,in,0,err));uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767)% (b-a+1);};
for(int tick=1;tick<=53;++tick){if(tick<50){for(int index=0;index<3;++index){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(index+2,0,tick-1,p,r,s,true));auto&e=in.emitters[index];e.position={p.X,p.Y,p.Z};e.scale={s.X,s.Y,s.Z};if(s.X>1e-5f&&s.Y>1e-5f&&s.Z>1e-5f){hmm_mat4 m;BuildRetailSpeedCameraMatrix(m,p,r,s);memcpy(e.matrix.data(),m.Elements,64);e.origin={m.Elements[3][0],m.Elements[3][1],m.Elements[3][2]};}}in.animation_frame=tick-1;sim.Advance(in,rng);for(size_t i=0;i<sim.Particles().size();++i)if(sim.Particles()[i].alive)sim.SampleRender(i,rng);}
printf("F %d %u %.9g\n",tick,seed,sim.EmissionCredit());for(size_t i=0;i<sim.Particles().size();++i){const auto&q=sim.Particles()[i];if(!q.alive)continue;printf("P %d %zu %d %d",tick,i,q.age,q.lifespan);for(float v:{q.position[0],q.position[1],q.position[2],q.velocity[0],q.velocity[1],q.velocity[2],q.scale,q.color[0],q.color[1],q.color[2],q.alpha})printf(" %.9g",v);puts("");}}}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/imagery/3dimage.cpp').read_text();body=(ROOT/'src/engine/assets/3dimagebody.h').read_text();signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(');methods={s:function(source,s)for s in signatures};prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"\n#include "authoredpartsys.h"');driver=output/'state.cpp';driver.write_text(prelude+'\n'+constants+'\n'+function(body,'struct SAniKey32\n')+';\n'+CLASS.split('uint32_t U(')[0]+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'state';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),*[str(ROOT/'src'/p)for p in('math3d.cpp','authoredpartsys.cpp','partsysdefinition.cpp')],'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0;cases=[]
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
  for name,id,digest in PROFILES:
   folder=output/name;folder.mkdir();data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+name+'.i3d'));asset=folder/'asset.fixture';asset.write_bytes(data);f=NativeJtele(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',data,name,False);f.initialize_scene();tag=next(t['parameters']for t in f.asset['tags']if t['name']=='partsys');q=subprocess.run([str(binary),str(asset),tag],capture_output=True,text=True);(folder/'source.trace').write_text(q.stdout);(folder/'source.log').write_text(q.stderr);assert q.returncode==0 and not q.stderr;frames={};port={}
   for line in q.stdout.splitlines():
    p=line.split()
    if p[0]=='F':frames[int(p[1])]=(int(p[2]),float(p[3]))
    else:port[(int(p[1]),int(p[2]))]=list(map(float,p[3:]))
   v=f.vm;c=f.controller;pool=v.u32(c+0x2c);maximum=0;fields=0;errors=[];lifecycle=[];rows=[];f.raster_enabled=False
   for tick in range(1,54):
    f.step();_,record=f.render();lifecycle.append(dict(tick=tick,frame=record['frame'],owner_flags=record['owner_flags'],draws=len(record['draws']),rng_after=record['rng_after']));credit=struct.unpack('<f',v.uc.mem_read(c+0x16c,4))[0]
    if v.rng_seed!=frames[tick][0]or abs(credit-frames[tick][1])>1e-6:errors.append(dict(tick=tick,kind='seed_credit',native=[v.rng_seed,credit],source=frames[tick]))
    for slot in range(v.u32(c+0xf8)):
     p=pool+slot*252;alive=bool(v.u32(p+0xe4));current=port.get((tick,slot))
     if alive!=(current is not None):errors.append(dict(tick=tick,slot=slot,kind='alive'));continue
     if not alive:continue
     row=[v.u32(p+0xd0),v.u32(p+0xd4),*[struct.unpack('<f',v.uc.mem_read(p+off,4))[0]for off in(0xb8,0xbc,0xc0,0xc4,0xc8,0xcc,0x64,0x50,0x54,0x58,0x60)]];delta=max(abs(a-b)for a,b in zip(row,current));maximum=max(maximum,delta);fields+=13;rows.append([tick,slot,*row])
     if delta>3e-5:errors.append(dict(tick=tick,slot=slot,kind='numeric',error=delta,native=row,source=current))
   assert all(not(int(r['owner_flags'],16)&0x1000)for r in lifecycle[:49]);assert all(int(r['owner_flags'],16)&0x1000 and not r['draws']for r in lifecycle[49:]);cases.append(dict(name=name,type_id=id,status='pass'if not errors else'fail',fields=fields,max_error=maximum,errors=errors,lifecycle=lifecycle,asset_sha256=digest));(folder/'native.json').write_text(json.dumps(rows)+'\n');print(name,cases[-1]['status'],fields,maximum,len(errors),flush=True)
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',accepted=False,cases=cases,driver_sha256=sha(driver.read_bytes()),source_sha256={p:sha((ROOT/'src'/p).read_bytes())for p in('authoredpartsys.cpp','partsysdefinition.cpp','partsysdefinition.h')},strict_numeric_tolerance=3e-5,scope='Full original defaultfactory/wholeInitialize/8actualSetupObjects/Pulse/Animate/Render/RNG deviceboundary versus independently compiled source decoder/cache/PSRuntime. Initialzeros are observed originalconstructor cache; all later emitterposes execute source decoder/#matrixhelper, nativezero-scale matrixrejects retainpreviouscache. Literal tagframe15 doesnotdelaynativeemission; finite50owner gate testedbeforeStep/Render. No templateoverride or hostnativepose injection. Fullnative/Metal pixels separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
