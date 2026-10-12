#!/usr/bin/env python3
"""Actual Burn ABSPOS helper submission, immutable geometry and source render matrices."""
import argparse,json,re,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "burncore.h"
#include "effects/burnprojection.h"
#include "3dimagebody.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
struct SHelperMeshSubmit{int mesh=0,retail_lighting=0;bool additive_blend=false,retail_additive_cull_none=false,retail_software_projection=false;float retail_normal_z_scale=1;float world[16]{};};
struct Capture{SHelperMeshSubmit draw;void SubmitHelperMesh(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;bool StartupVfxNativeDomain=true;struct State{int mesh[2]={1,2};burn_core::State core;};
void Production(State*s,int pool,int slot,int object,const hmm_mat4&local){BRANCH}
int main(int argc,char**argv){assert(argc==3);std::ifstream a(argv[1],std::ios::binary);std::vector<unsigned char>raw((std::istreambuf_iterator<char>(a)),{});auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());assert(m->numverts==8&&m->numfaces==4&&m->numobjects==2);auto*t=static_cast<OFFSET*>(m->verts[0].ptr());const auto*v=static_cast<float*>(t[0].ptr());std::ifstream f(argv[2]);std::string line;int draws=0,vertices=0,common_vertices=0;State s;while(std::getline(f,line)){std::istringstream row(line);char kind;row>>kind;if(kind=='S'){int tick,pool,slot,used;row>>tick>>pool>>slot>>used;auto&p=(pool?s.core.smoke:s.core.fire)[slot];p.used=used;if(used)row>>p.position.X>>p.position.Y>>p.position.Z;continue;}if(kind!='D')continue;int tick,pool,slot,object;row>>tick>>pool>>slot>>object;hmm_mat4 local;for(int i=0;i<16;++i)row>>(&local.Elements[0][0])[i];assert((pool?s.core.smoke:s.core.fire)[slot].used);for(bool policy:{true,false}){StartupVfxNativeDomain=policy;Production(&s,pool,slot,object,local);hmm_mat4 actual;for(int r=0;r<4;++r)for(int c=0;c<4;++c)actual.Elements[c][r]=capture.draw.world[r*4+c];const auto center=(pool?s.core.smoke:s.core.fire)[slot].position;for(int i=0;i<4;++i){int at=m->objvertpos[object]+i;hmm_vec3 p={v[at*8],v[at*8+1],v[at*8+2]},x,y;MtxTransform(&local,&p,&x);MtxTransform(&actual,&p,&y);assert(x.X==y.X&&x.Y==y.Y);if(policy){assert(x.Z==y.Z);++vertices;}else{const float expected=center.Z+(x.Z-local.Elements[3][2])*WORLD3D_Z_SCALE;assert(std::fabs(y.Z-expected)<.0001f);++common_vertices;}}assert(capture.draw.mesh==object+1&&capture.draw.retail_lighting==1&&capture.draw.additive_blend&&capture.draw.retail_additive_cull_none&&capture.draw.retail_software_projection==policy);assert(capture.draw.retail_normal_z_scale==(policy?1.f:1.f/WORLD3D_Z_SCALE));}++draws;}printf("PASS %d %d %d\n",vertices,draws,common_vertices);}
'''
def run(output,state_trace):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/burnpublicpreview.cpp').read_text();a=source.index('const auto&particle=(pool?');b=source.index('++draws;',a);branch=source[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('BRANCH',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0, q.stderr
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/burnbabyburn.i3d')
 asset=output/'body.fixture';asset.write_bytes(data[104:]);trace=output/'draws.trace';trace.write_text('\n'.join(l for l in state_trace.read_text().splitlines()if l.startswith(('D ','S ')))+'\n');q=subprocess.run([str(binary),str(asset),str(trace)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr,q.stderr;_,vertices,draws,common_vertices=q.stdout.split();report=dict(status='pass',accepted=False,vertices=int(vertices),common_vertices=int(common_vertices),draws=int(draws),authored_vertices=8,source_span_sha256=sha(branch.encode()),projection_sha256=sha((ROOT/'src/effects/burnprojection.h').read_bytes()),asset_sha256=sha(data),state_trace_sha256=sha(state_trace.read_bytes()),sanitizers='ASan/UBSan',scope='Actual production submission compiles verbatim. All eight authored vertices/every draw preserve exact native ABSPOS matrices; common branch preserves XY, raw per-slot map-Z center and local MODELZ offsets times existing WORLD3D_Z_SCALE, plus inverse normal scale. No owner re-addition or fitted screen coordinates. Original meaningful slot/state/matrix/RNG comparison remains separate; natural moving actor, caller, lighting/device and framebuffer equality remain open.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(q.stdout.strip());return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--state-trace',type=Path,required=True);a=p.parse_args();run(a.output,a.state_trace)
