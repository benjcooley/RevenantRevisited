#!/usr/bin/env python3
"""Extract Halo production matrix/submission and preserve all authored vertices."""
import argparse,json,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha,function
MAIN=r'''
#include "halocore.h"
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <initializer_list>
struct SHelperMeshSubmit{int mesh=0;float world[16]{};int retail_lighting=0;bool retail_source_alpha=false,additive_blend=false,retail_software_projection=false,retail_software_pixel_grid=false,retail_alpha_source_quantize=false;};
struct Capture{SHelperMeshSubmit draw;void SubmitHelperMesh(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;
bool StartupVfxNativeDomain=true;struct State{int mesh=1,raster_policy=3;};
RASTER_POLICY
void Production(State*s,const hmm_mat4&local,const hmm_mat4&root){BRANCH}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);float raw[96];f.read(reinterpret_cast<char*>(raw),384);assert(f.gcount()==384);int vertices=0,draws=0;for(int mode=0;mode<2;++mode)for(int translated=0;translated<2;++translated){halo_core::State core(mode?20:60,mode?.08f:.04f);hmm_mat4 root;BuildRetailSoftwareOwner(root,translated?100:0,translated?200:0,translated?30:16,0);State s;for(int t=1;t<=70;++t){core.Advance();hmm_mat4 local;if(!core.Matrix(local))continue;Production(&s,local,root);hmm_mat4 submitted;for(int r=0;r<4;++r)for(int c=0;c<4;++c)submitted.Elements[c][r]=capture.draw.world[r*4+c];for(int i=0;i<12;++i){hmm_vec3 v={raw[8*i],raw[8*i+1],raw[8*i+2]},a,b,result;MtxTransform(&local,&v,&a);MtxTransform(&root,&a,&b);MtxTransform(&submitted,&v,&result);for(int c=0;c<3;++c)assert(fabs(result.Elements[c]-b.Elements[c])<.0001f);++vertices;}assert(capture.draw.retail_lighting==3 && capture.draw.retail_source_alpha && !capture.draw.additive_blend && capture.draw.retail_software_projection);++draws;}}unsetenv("REV_HALO_RASTER_DIAGNOSTIC");assert(RasterPolicy()==3);for(const char* bad:{"","4","-1","12"}){setenv("REV_HALO_RASTER_DIAGNOSTIC",bad,1);assert(RasterPolicy()==3);}for(int p=0;p<4;++p){char value[2]={char('0'+p),0};setenv("REV_HALO_RASTER_DIAGNOSTIC",value,1);assert(RasterPolicy()==p);for(int native=0;native<2;++native){StartupVfxNativeDomain=native;State s;s.raster_policy=p;SHelperMeshSubmit d;ConfigureRaster(d,&s);assert(d.retail_software_projection==bool(native));assert(d.retail_software_pixel_grid==bool(native&&(p&1)));assert(d.retail_alpha_source_quantize==bool(native&&(p&2)));}}printf("PASS %d %d\n",vertices,draws);}
'''
def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/halopreview.cpp').read_text();a=source.index('    MtxMultiply(&world,&local,&root);');b=source.index('    Renderer->SubmitHelperMesh(draw);',a)+len('    Renderer->SubmitHelperMesh(draw);');branch='hmm_mat4 world;'+source[a:b].replace('&local','const_cast<hmm_mat4*>(&local)').replace('&root','const_cast<hmm_mat4*>(&root)');driver=output/'owner.cpp';driver.write_text(MAIN.replace('BRANCH',branch).replace('RASTER_POLICY',function(source,'static int RasterPolicy()')+'\n'+function(source,'static void ConfigureRaster(SHelperMeshSubmit& draw,const State* s)')));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/halo.i3d')
 fixture=output/'vertices.fixture';fixture.write_bytes(bytes.fromhex(read_asset(data)['objects'][0]['vertex_hex']));q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,vertices,draws=q.stdout.split();r=dict(status='pass',accepted=False,vertices=int(vertices),draws=int(draws),source_span_sha256=sha(branch.encode()),asset_sha256=sha(data),sanitizers='ASan/UBSan',scope='Actual production composedworld/helper flags, both declared publicparameter cases, all12authoredvertices throughout finite cycles, origin16 andtranslated100,200,30. Sequential local→owner matches actualsubmittedworld; own source alpha lookup routing matches original ARGB4444 software kernel independent of native generic SetAddBlendState flags. Naturalhardware blend/caller separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(q.stdout.strip());return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
