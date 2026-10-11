#!/usr/bin/env python3
"""Literal four-effect integer pose and ordinary emitter matrix contract."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from static_particles_profile_contract import CLASS,Y_PROFILES,CAMERA_PROFILES,ROOT
from speed_profile_contract import PRELUDE,function,sha
from speed_controller_preflight import NativeSpeed,read_asset
from static_particles_probe import registry,punctuation

MAIN=r'''
int main(int argc,char**argv){assert(argc==4);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char>b((std::istreambuf_iterator<char>(f)),{});T3DImagery im;Load(im,b,argv[3]);im.Select();assert(im.static_particles_profile==std::strtoul(argv[2],nullptr,0));
Owner owner;owner.id=im.static_particles_profile;T3DAnimator animator{&im,&owner};for(int obj=0;obj<im.nobj;++obj)for(int frame=0;frame<im.frames;++frame){hmm_vec3 p{},r{},s{};assert(im.GetUninterpolatedAniKey(obj,0,frame,p,r,s));hmm_mat4 m;MtxClear(&m);bool camera=false;for(const auto&profile:retail_static_particles::profiles)if(profile.id==im.static_particles_profile)camera=profile.camera_emitters;if(camera&&im.objects[obj].name[0]=='#'){animator.frame=frame;assert(animator.CameraParticleEmitterLocalMatrix(obj,m));}else MakeMatrix(m,p,r,s);printf("%d %d",obj,frame);for(float v:{p.X,p.Y,p.Z,r.X,r.Y,r.Z,s.X,s.Y,s.Z})printf(" %.9g",v);for(int i=0;i<16;++i)printf(" %.9g",(&m.Elements[0][0])[i]);puts("");}}
'''

def run(executable,archive,output,profiles=Y_PROFILES):
 output.mkdir(parents=True,exist_ok=True);paths=('src/3dimage.cpp','src/3dimagebody.h','src/staticpartsysprofiles.h','src/math3d.cpp','src/math3d.h','src/3dimage.h','src/speedauthoredmatrix.h','src/goldauthoredmatrix.h');pinned={p:sha((ROOT/p).read_bytes())for p in paths};source=(ROOT/'src/3dimage.cpp').read_text();body=(ROOT/'src/3dimagebody.h').read_text();render=(ROOT/'src/render3d_types.h').read_text()
 signatures=('static inline int32_t SkipAniKey32(','static inline void GetAniKey32(','bool T3DImagery::GetUninterpolatedAniKey(','static void MakeMatrix(','uint32_t T3DImagery::ValidateRetailStaticParticleProfile()','bool T3DAnimator::CameraParticleEmitterLocalMatrix(');methods={s:function(source,s)for s in signatures}
 prefixes=('SMALLKEY','ANIKEY32_','ANIFLAG32_','ANICODE32_','ANIKEY_POSSCALE','ANIKEY_ANGSCALE','I3D_ANIKEY32');constants='\n'.join(l for l in body.splitlines()if l.startswith('#define ')and any(l.startswith('#define '+p)for p in prefixes));records='\n'.join(function(t,s)+';'for t,s in((body,'struct SAniKey32\n'),(body,'struct S3DFace\n'),(render,'struct S3DVertex\n'),(render,'struct SRenderColor\n'),(body,'struct S3DMaterial\n')))
 cls=CLASS.replace('TYPED_METHOD','bool GetUninterpolatedAniKey(int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&);const char*GetObjectName(int i){return objects[i].name;}'+function((ROOT/'src/3dimage.h').read_text(),'bool HasRetailStaticParticleProfile(')+function((ROOT/'src/3dimage.h').read_text(),'bool HasRetailCameraParticleProfile(')).replace('bool null_filename=false;', '''bool null_filename=false,meshinitialized=true;
 bool retail_punch_keys=false,retail_mpappear_start_profile=false,retail_shadowfist_profile=false,retail_warriorborn_profile=false,retail_teleportation_profile=false,might_partsys_profile=false,immortalmight_partsys_profile=false,fmastery_partsys_profile=false,speed_partsys_profile=false,quicksilver_partsys_profile=false;
 void*GetBody(){assert(false);return nullptr;}bool InitializeMesh(S3DImageryBody*){assert(false);return false;}''')
 prelude=PRELUDE.replace('struct hmm_vec3 {float X=0,Y=0,Z=0;};','#include "math3d.h"\n#include "speedauthoredmatrix.h"')
 animator='''
struct Owner{uint32_t id=0;int GetState(){return 0;}uint32_t ObjId(){return id;}};
struct T3DAnimator{T3DImagery* image;Owner*inst;int frame=0;struct ObjectSlots{bool Used(int i){return i>=0&&i<40;}}animobjs;T3DImagery*Get3DImagery(){return image;}int GetFrame(){return frame;}bool CameraParticleEmitterLocalMatrix(int,hmm_mat4&,hmm_vec3* =nullptr,hmm_vec3* =nullptr);};
''';driver=output/'pose.cpp';driver.write_text(prelude+'\n'+constants+'\n'+records+'\n'+cls+'\n'+animator+'\n'+'\n'.join(methods.values())+'\n'+MAIN);binary=output/'pose';cmd=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)];q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr)
 if q.returncode:raise RuntimeError('pose compilation failed')
 cases=[]
 with zipfile.ZipFile(archive)as z:
  for profile in profiles:
   data=z.read(next(n for n in z.namelist()if n.lower()==('imagery/'+profile['asset']).lower()));assert sha(data)==profile['sha256'];asset=read_asset(data);asset['name']=profile['name'];path=output/(profile['name']+'.asset-input');path.write_bytes(data);q=subprocess.run([str(binary),str(path),profile['id'],profile['asset']],capture_output=True,text=True);(output/(profile['name']+'.log')).write_text(q.stderr);(output/(profile['name']+'.trace')).write_text(q.stdout)
   if q.returncode or q.stderr:raise RuntimeError(profile['name']+' source pose failed')
   port={(int(p[0]),int(p[1])):[float(v)for v in p[2:]]for l in q.stdout.splitlines()if(p:=l.split())};fixture=output/(profile['name']+'-source-poses.json');fixture.write_text(json.dumps(dict(source_sha256=pinned,asset_sha256=profile['sha256'],type_id=profile['id'],poses=[dict(object=o,frame=f,trs=p[:9],matrix=p[9:])for(o,f),p in port.items()]),indent=2)+'\n');native=NativeSpeed(executable,asset);registry(native);punctuation(native);v=native.vm;vectors=v.allocate(36);errors=[];key_checks=matrix_checks=0;key_max=matrix_max=0
   for obj,o in enumerate(asset['objects']):
    for frame in range(asset['state0_frames']):
     if v.call(0x409430,(obj,0,frame,vectors,vectors+12,vectors+24),this=native.imagery)!=1:raise AssertionError('native key rejected')
     original=struct.unpack('<9f',v.uc.mem_read(vectors,36));current=port[(obj,frame)];d=max(abs(a-b)for a,b in zip(original,current[:9]));key_max=max(key_max,d);key_checks+=9
     if d>3e-6:errors.append(dict(object=obj,frame=frame,key_error=d))
     # '#' camera and '*' hidden helpers have distinct branches; the
     # particle frontend tests those separately. All named emitters are roots.
     if o['name'].startswith('*')or(o['name'].startswith('#')and profile not in CAMERA_PROFILES):continue
     if v.call(0x40a420,(native.animobjs[obj],0,frame,0,0),this=native.imagery)!=1:raise AssertionError('native matrix rejected')
     original=struct.unpack('<16f',v.uc.mem_read(native.animobjs[obj]+0x58,64));d=max(abs(a-b)for a,b in zip(original,current[9:]));matrix_max=max(matrix_max,d);matrix_checks+=16
     if d>6e-5:errors.append(dict(object=obj,frame=frame,matrix_error=d,native=original,source=current[9:]))
   cases.append(dict(name=profile['name'],status='fail'if errors else'pass',key_channels=key_checks,matrix_channels=matrix_checks,max_key_error=key_max,max_matrix_error=matrix_max,errors=errors,asset_sha256=profile['sha256'],source_pose_fixture_sha256=sha(fixture.read_bytes()),native_constructor=native.constructor))
 if pinned!={p:sha((ROOT/p).read_bytes())for p in paths}:raise AssertionError('source mutation')
 report=dict(status='pass'if all(c['status']=='pass'for c in cases)else'fail',cases=cases,source_sha256=pinned,source_spans={s:sha(m.encode())for s,m in methods.items()},driver_sha256=sha(driver.read_bytes()),binary_sha256=sha(binary.read_bytes()),probe_sha256=sha(Path(__file__).read_bytes()),retail_sha256=sha(executable.read_bytes()),sanitizers='ASan/UBSan',accepted=False,scope='Actual integer decoder and ordinary MakeMatrix linked with production math; every object/frame TRS compared to original409430, all root named-emitter matrix elements to original40a420. Camera prototype and hidden helpers tested by separate rendered frontend. No interpolated keys, parent/owner hierarchy or natural caller acceptance.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('executable',type=Path);p.add_argument('--archive',type=Path,default=ROOT/'data/imagery.rvi');p.add_argument('--output',type=Path,required=True);p.add_argument('--camera-particles',action='store_true');a=p.parse_args();r=run(a.executable,a.archive,a.output,CAMERA_PROFILES if a.camera_particles else Y_PROFILES);print(json.dumps({k:r[k]for k in('status','cases')},indent=2));raise SystemExit(r['status']!='pass')
