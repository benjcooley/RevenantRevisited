#!/usr/bin/env python3
"""Verbatim Aura owner-relative helper branch and exact four authored vertices."""
import argparse,json,subprocess,zipfile,struct
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "auracore.h"
#include "retailsoftwaretransform.h"
#include <cassert>
#include <cstdio>
#include <fstream>
struct SHelperMeshSubmit{int mesh=0,retail_lighting=0;bool additive_blend=false,retail_additive_cull_none=false,retail_software_projection=false;float world[16]{};};
struct Capture{SHelperMeshSubmit draw;void SubmitHelperMesh(const SHelperMeshSubmit&d){draw=d;}}capture,*Renderer=&capture;bool StartupVfxNativeDomain=true;struct State{int mesh=1;};
void Production(State*s,const hmm_mat4&local,hmm_mat4&owner){BRANCH}
int main(int argc,char**argv){assert(argc==3);float v[32];std::ifstream a(argv[1],std::ios::binary);a.read(reinterpret_cast<char*>(v),sizeof(v));assert(a.gcount()==sizeof(v));int draws=0,vertices=0;State s;for(const S3DPoint pos:{S3DPoint{0,0,16},S3DPoint{1000,2000,16}}){hmm_mat4 owner;BuildRetailSoftwareOwner(owner,pos.x,pos.y,pos.z,0);std::ifstream f(argv[2]);char kind;while(f>>kind){assert(kind=='D');int tick,pool,slot,object;f>>tick>>pool>>slot>>object;assert(object==0);hmm_mat4 local;for(int i=0;i<16;++i)f>>(&local.Elements[0][0])[i];Production(&s,local,owner);hmm_mat4 expected,actual;MtxMultiply(&expected,&local,&owner);for(int r=0;r<4;++r)for(int c=0;c<4;++c)actual.Elements[c][r]=capture.draw.world[r*4+c];for(int i=0;i<4;++i){hmm_vec3 p={v[i*8],v[i*8+1],v[i*8+2]},x,y;MtxTransform(&expected,&p,&x);MtxTransform(&actual,&p,&y);for(int j=0;j<3;++j)assert(x.Elements[j]==y.Elements[j]);++vertices;}assert(capture.draw.mesh==1&&capture.draw.retail_lighting==1&&capture.draw.additive_blend&&capture.draw.retail_additive_cull_none&&capture.draw.retail_software_projection);++draws;}}printf("PASS %d %d\n",vertices,draws);}
'''
def run(output,trace):
 output.mkdir(parents=True,exist_ok=False);s=(ROOT/'src/game/effects/legacy/aurapublicpreview.cpp').read_text();a=s.index('hmm_mat4 world;MtxMultiply');b=s.index('++draws;',a);branch=s[a:b];driver=output/'owner.cpp';driver.write_text(MAIN.replace('BRANCH',branch));binary=output/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/engine/core/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read('Imagery/Magic/aura.i3d')
 asset=read_asset(data);assert asset['vertices']==4 and asset['faces']==2 and len(asset['objects'])==1;fixture=output/'vertices.fixture';fixture.write_bytes(bytes.fromhex(asset['objects'][0]['vertex_hex']));draws=output/'draws.trace';draws.write_text('\n'.join(l for l in trace.read_text().splitlines()if l.startswith('D '))+'\n');q=subprocess.run([str(binary),str(fixture),str(draws)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,verts,count=q.stdout.split();r=dict(status='pass',accepted=False,vertices=int(verts),draws=int(count),sanitizers='ASan/UBSan',source_span_sha256=sha(branch.encode()),asset_sha256=sha(data),state_trace_sha256=sha(trace.read_bytes()),scope='Verbatim production owner-relative helper branch preserves every four authored positions under all source-core draw matrices at origin and declared translatedowner(1000,2000,16); normal RGB565/additive/cullnone/software flags. Core/native local matrices proved separately. This transformed-owner contract is not a natural actor/moving-camera appearance certification.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(q.stdout.strip());return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--state-trace',type=Path,required=True);a=p.parse_args();run(a.output,a.state_trace)
