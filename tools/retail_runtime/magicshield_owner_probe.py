#!/usr/bin/env python3
"""Verbatim MagicShield submission, all active authored geometry and material inputs."""
import argparse,json,struct,zipfile,subprocess
from pathlib import Path
from speed_controller_preflight import ROOT,read_asset
from speed_profile_contract import sha
MAIN=r'''
#include "magicshieldcore.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cstring>
struct Material{float diffuse[4],ambient[4],specular[4],emissive[4],power;};struct Mat{Material matdesc;};
struct SHelperMeshSubmit{int mesh=0,retail_lighting=0;bool retail_source_alpha=false,retail_alpha_source_quantize=false,retail_software_projection=false;float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0,world[16]{};};
struct Capture{SHelperMeshSubmit item;void SubmitHelperMesh(const SHelperMeshSubmit&q){item=q;}}capture,*Renderer=&capture;bool StartupVfxNativeDomain=true;struct State{int meshes[2]={1,2};Mat materials[2];};
void Production(State*s,int object,const hmm_mat4&world){BRANCH}
int main(int argc,char**argv){assert(argc==3);std::ifstream f(argv[1],std::ios::binary);std::vector<float>vertices(406*8);f.read(reinterpret_cast<char*>(vertices.data()),vertices.size()*4);assert(f.good());State s;f.read(reinterpret_cast<char*>(s.materials),sizeof(s.materials));assert(f.gcount()==sizeof(s.materials));std::ifstream trace(argv[2]);int count=0,draws=0;char tag;while(trace>>tag){assert(tag=='D');int tick,object,slot;trace>>tick>>object>>slot;assert(object==0||object==1);hmm_mat4 world;for(int i=0;i<16;++i)trace>>(&world.Elements[0][0])[i];Production(&s,object,world);auto&q=capture.item;assert(q.mesh==object+1&&q.retail_lighting==3&&q.retail_source_alpha&&q.retail_alpha_source_quantize&&q.retail_software_projection);assert(!memcmp(q.diffuse,s.materials[object].matdesc.diffuse,16));assert(!memcmp(q.ambient,s.materials[object].matdesc.ambient,16));assert(!memcmp(q.specular,s.materials[object].matdesc.specular,16));assert(!memcmp(q.emissive,s.materials[object].matdesc.emissive,16));assert(q.power==s.materials[object].matdesc.power);hmm_mat4 actual;for(int r=0;r<4;++r)for(int c=0;c<4;++c)actual.Elements[c][r]=q.world[r*4+c];for(int i=0;i<(object?4:402);++i){int k=(object?402:0)+i;hmm_vec3 p={vertices[k*8],vertices[k*8+1],vertices[k*8+2]},a,b;MtxTransform(&world,&p,&a);MtxTransform(&actual,&p,&b);for(int j=0;j<3;++j)assert(a.Elements[j]==b.Elements[j]);++count;}++draws;}printf("PASS %d %d\n",count,draws);}
'''
def run(out,state_trace):
 out.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/magicshieldpreview.cpp').read_text();a=source.index('SHelperMeshSubmit q={};');b=source.index('++draws;',a);branch=source[a:b];driver=out/'owner.cpp';driver.write_text(MAIN.replace('BRANCH',branch));binary=out/'owner';q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I'+str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(binary)],capture_output=True,text=True);(out/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:raw=z.read('Imagery/Magic/mshield.i3d')
 asset=read_asset(raw);assert asset['vertices']==1218 and asset['faces']==2076 and asset['objects'][0]['texture_runs'][3]==690;body=asset['body_offset'];mat=body+32+struct.unpack_from('<I',raw,body+32)[0];materials=b''.join(raw[mat+i*80+4:mat+i*80+72]for i in(0,1));assert len(materials)==136;fixture=out/'resource.fixture';fixture.write_bytes(b''.join(bytes.fromhex(asset['objects'][i]['vertex_hex'])for i in(0,1))+materials);trace=out/'draws.trace';trace.write_text('\n'.join(l for l in state_trace.read_text().splitlines()if l.startswith('D '))+'\n');q=subprocess.run([str(binary),str(fixture),str(trace)],capture_output=True,text=True);(out/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr;_,verts,draws=q.stdout.split();r=dict(status='pass',accepted=False,vertices=int(verts),draws=int(draws),source_span_sha256=sha(branch.encode()),asset_sha256=sha(raw),state_trace_sha256=sha(state_trace.read_bytes()),sanitizers='ASan/UBSan',scope='Verbatim production helper branch, all402sphere+4dot authored positions for every active draw through170; exact source material diffuse/ambient/specular/emissive/power, ARGB4444/sourcealpha/software flags and ABSPOS world (no duplicateowner). Native core matrices separately compared. Natural caller/device/exactraster open.');(out/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(q.stdout.strip());return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--state-trace',type=Path,required=True);a=p.parse_args();run(a.output,a.state_trace)
