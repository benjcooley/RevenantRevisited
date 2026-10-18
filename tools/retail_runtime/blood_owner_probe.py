#!/usr/bin/env python3
"""Extract actual Blood production submission and verify all immutable geometry."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "bloodcore.h"
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
struct SHelperMeshSubmit{int mesh=0;float world[16]{},diffuse[4]{};int retail_lighting=0;bool retail_source_alpha=false,retail_software_projection=false,additive_blend=false,retail_additive_cull_none=false;};
struct Capture{SHelperMeshSubmit draw;void SubmitHelperMesh(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;
struct Scene{bool GetClosestLights(int,int,int,int&,int&,int&){return false;}void GetLightBrightness(int,int,int,int,float&){assert(false);}}Scene3D;
struct Position{int x,y,z;};bool StartupVfxNativeDomain=true;
struct State{blood_core::State core{2};int meshes[8]={1,2,3,4,5,6,7,8};int ambient=52;};
void Production(State*s,int slot,int object,int pass,const hmm_mat4&local,const hmm_mat4&root,const Position&p){COMPOSITION}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);float raw[256];f.read(reinterpret_cast<char*>(raw),1024);assert(f.gcount()==1024);int checks=0,draws=0;for(int translated=0;translated<2;++translated){Position p={translated?100:0,translated?200:0,translated?30:56};hmm_mat4 root;BuildRetailSoftwareOwner(root,p.x,p.y,p.z,0);State s;for(int tick=1;tick<=60;++tick){s.core.Advance();s.core.Render([&](int slot,int object,int pass,const hmm_mat4&local){Production(&s,slot,object,pass,local,root,p);hmm_mat4 submitted;for(int r=0;r<4;++r)for(int c=0;c<4;++c)submitted.Elements[c][r]=capture.draw.world[r*4+c];for(int i=object*4;i<object*4+4;++i){hmm_vec3 v={raw[i*8],raw[i*8+1],raw[i*8+2]},a,b,result;MtxTransform(&local,&v,&a);MtxTransform(&root,&a,&b);MtxTransform(&submitted,&v,&result);for(int c=0;c<3;++c)assert(fabs(result.Elements[c]-b.Elements[c])<.0001f);++checks;}assert(capture.draw.retail_lighting==(pass?6:3));assert(capture.draw.retail_source_alpha==!pass&&capture.draw.additive_blend==bool(pass)&&capture.draw.retail_additive_cull_none==bool(pass)&&capture.draw.retail_software_projection);assert(capture.draw.diffuse[0]==31&&capture.draw.diffuse[1]==31&&capture.draw.diffuse[2]==19&&capture.draw.diffuse[3]==31);++draws;});}}printf("PASS %d %d\n",checks,draws);}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/game/effects/legacy/bloodpreview.cpp').read_text();a=source.index('hmm_mat4 world;MtxMultiply(');b=source.index('++count;',a);branch=source[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('COMPOSITION',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Misc/blood.i3d')
 fixture=output/'vertices.fixture';fixture.write_bytes(b''.join(bytes.fromhex(o['vertex_hex'])for o in read_asset(data)['objects']));q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,checks,draws=q.stdout.split();r=dict(status='pass',accepted=False,geometry_checks=int(checks),draw_checks=int(draws),authored_vertices=32,source_span_sha256=sha(branch.encode()),asset_sha256=sha(data),sanitizers='ASan/UBSan',scope='Extracted actual production whole8-quad submission and realmatrix APIs across all60ticks, origin andtranslated100,200,30. Every authored vertex transformed sequentially agrees with submitted composedworld; exacttwo-pass texture/cull/blend/reference policies and derivedPackedColor52=ffffff98/RGBA5 31,31,19,31 retained. Declared empty pointlight array; original localmatrices separatelyexecuted, actualGPU andnaturalmap context separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(r['status'],checks,draws);return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
