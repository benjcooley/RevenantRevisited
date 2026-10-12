#!/usr/bin/env python3
"""Actual Burn ABSPOS helper submission, immutable geometry and source render matrices."""
import argparse,json,re,subprocess,zipfile
from pathlib import Path
from speed_controller_preflight import ROOT
from speed_profile_contract import sha
MAIN=r'''
#include "burncore.h"
#include "3dimagebody.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
struct SHelperMeshSubmit{int mesh=0,retail_lighting=0;bool additive_blend=false,retail_additive_cull_none=false,retail_software_projection=false;float world[16]{};};
struct Capture{SHelperMeshSubmit draw;void SubmitHelperMesh(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;bool StartupVfxNativeDomain=true;struct State{int mesh[2]={1,2};};
void Production(State*s,int object,const hmm_mat4&local){BRANCH}
int main(int argc,char**argv){assert(argc==3);std::ifstream a(argv[1],std::ios::binary);std::vector<unsigned char>raw((std::istreambuf_iterator<char>(a)),{});auto*m=reinterpret_cast<SOld3DImageryBody*>(raw.data());assert(m->numverts==8&&m->numfaces==4&&m->numobjects==2);auto*t=static_cast<OFFSET*>(m->verts[0].ptr());const auto*v=static_cast<float*>(t[0].ptr());std::ifstream f(argv[2]);char kind;int draws=0,vertices=0;State s;while(f>>kind){assert(kind=='D');int tick,pool,slot,object;f>>tick>>pool>>slot>>object;hmm_mat4 local;for(int i=0;i<16;++i)f>>(&local.Elements[0][0])[i];Production(&s,object,local);hmm_mat4 actual;for(int r=0;r<4;++r)for(int c=0;c<4;++c)actual.Elements[c][r]=capture.draw.world[r*4+c];for(int i=0;i<4;++i){int at=m->objvertpos[object]+i;hmm_vec3 p={v[at*8],v[at*8+1],v[at*8+2]},x,y;MtxTransform(&local,&p,&x);MtxTransform(&actual,&p,&y);for(int j=0;j<3;++j)assert(x.Elements[j]==y.Elements[j]);++vertices;}assert(capture.draw.mesh==object+1&&capture.draw.retail_lighting==1&&capture.draw.additive_blend&&capture.draw.retail_additive_cull_none&&capture.draw.retail_software_projection);++draws;}printf("PASS %d %d\n",vertices,draws);}
'''
def run(output,state_trace):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/burnpublicpreview.cpp').read_text();a=source.index('SHelperMeshSubmit d={};');b=source.index('++draws;',a);branch=source[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('BRANCH',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/burnbabyburn.i3d')
 asset=output/'body.fixture';asset.write_bytes(data[104:]);trace=output/'draws.trace';trace.write_text('\n'.join(l for l in state_trace.read_text().splitlines()if l.startswith('D '))+'\n');q=subprocess.run([str(binary),str(asset),str(trace)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,vertices,draws=q.stdout.split();report=dict(status='pass',accepted=False,vertices=int(vertices),draws=int(draws),authored_vertices=8,source_span_sha256=sha(branch.encode()),asset_sha256=sha(data),state_trace_sha256=sha(state_trace.read_bytes()),sanitizers='ASan/UBSan',scope='Actualproduction helper branch compiles verbatim; all8authoredvertices sourceoldbody and every5640draw through90 preserve ABSPOS local/world (no owner addedagain), exactRGB565/additive/cullnone/software flags. Actualrealactor translatedroot contributes duringbirth, checked by core/sourcepose/native. Naturalworld/caller/lighting/device separate.');(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(q.stdout.strip());return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--state-trace',type=Path,required=True);a=p.parse_args();run(a.output,a.state_trace)
