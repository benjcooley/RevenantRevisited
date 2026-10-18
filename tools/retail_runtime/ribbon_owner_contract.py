#!/usr/bin/env python3
"""Compile actual Ribbon submit composition against all 186 authored vertices."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from ribbon_native_preflight import ROOT,read_ribbon,ASSET_SHA
from speed_profile_contract import sha
MAIN=r'''
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>
struct S3DAnimObj {hmm_mat4 matrix;};
struct SHelperMeshSubmit {int mesh;float world[16];int retail_lighting;bool retail_source_alpha,retail_software_projection;};
struct Capture {SHelperMeshSubmit draw;void SubmitRetailSourceAlphaAfterFx(const SHelperMeshSubmit&d){draw=d;}} capture,*Renderer=&capture;
bool StartupVfxNativeDomain=false;
void ProductionSubmit(const hmm_mat4&local,const hmm_mat4&root){S3DAnimObj o{local};int mesh=1;COMPOSITION}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<float>raw(186*8);f.read(reinterpret_cast<char*>(raw.data()),5952);assert(f.gcount()==5952);int checks=0;
for(int native=0;native<2;++native)for(int translated=0;translated<2;++translated)for(int pose=0;pose<3;++pose){StartupVfxNativeDomain=native;hmm_mat4 local,root;MtxClear(&local);MtxRotateZ(&local,.2f*pose);const hmm_vec3 scale={.5f+pose,.75f+pose,1.f+pose};MtxScale(&local,&scale);const hmm_vec3 offset={.25f,-.25f,float(pose)};MtxTranslate(&local,&offset);if(native)BuildRetailSoftwareOwner(root,translated?10000:0,translated?10000:0,translated?16:0,0);else{MtxClear(&root);const hmm_vec3 owner={translated?10000.f:0.f,translated?10000.f:0.f,translated?24.f:0.f};MtxTranslate(&root,&owner);}ProductionSubmit(local,root);hmm_mat4 submitted;for(int r=0;r<4;++r)for(int c=0;c<4;++c)submitted.Elements[c][r]=capture.draw.world[r*4+c];if(!translated)for(int r=0;r<4;++r)for(int c=0;c<4;++c)assert(submitted.Elements[c][r]==local.Elements[c][r]);for(int i=0;i<186;++i){hmm_vec3 p={raw[i*8],raw[i*8+1],raw[i*8+2]},a,b,result;MtxTransform(&local,&p,&a);MtxTransform(&root,&a,&b);MtxTransform(&submitted,&p,&result);for(int c=0;c<3;++c)assert(fabs(result.Elements[c]-b.Elements[c])<.002f);++checks;}assert(capture.draw.retail_lighting==3&&capture.draw.retail_source_alpha&&capture.draw.retail_software_projection==bool(native));}printf("geometry_checks %d\n",checks);}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/ribbonpreview.cpp').read_text();a=source.index('hmm_mat4 world;MtxMultiply(');b=source.index('\n}',a);branch=source[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('COMPOSITION',branch));binary=output/'owner';cmd=['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)];q=subprocess.run(cmd,capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/ribbon.i3d'))
 fixture=output/'vertices.fixture';fixture.write_bytes(bytes.fromhex(read_ribbon(data)['vertex_hex']));q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr
 report=dict(status='pass',geometry_checks=2232,authored_vertices=186,asset_sha256=ASSET_SHA,composition_sha256=sha(branch.encode()),driver_sha256=sha(driver.read_bytes()),math_source_sha256=sha((ROOT/'src/engine/core/math3d.cpp').read_bytes()),sanitizers='ASan/UBSan',scope='Extracted actual Ribbon production submit composition, existing real source matrix functions, every authored vertex under three nontrivial local poses, normal and native-domain owner matrices at origin and translated10000/10000/16. Independent sequential local then owner transform matches composed submit; origin-zero local matrix unchanged exactly. This contract does not certify original actor context or GPU equality.',accepted=False);(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();print(run(a.output)['status'])
