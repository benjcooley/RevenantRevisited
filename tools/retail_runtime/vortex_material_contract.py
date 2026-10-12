"""Compile the actual Vortex material forwarding against both own descriptors."""
import argparse,json,struct,subprocess,zipfile
from pathlib import Path
from vortex_native_preflight import ROOT,ASSET_SHA
from speed_profile_contract import sha

def run(output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effects/vortexpreview.cpp').read_text();start=source.index('  const auto copy=[]');end=source.index('  // Original untextured kernel:',start);body=source[start:end]
 driver=output/'material.cpp';driver.write_text(r'''
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <fstream>
struct SRenderColor{float r,g,b,a;};
struct S3DMaterial{uint32_t size;SRenderColor diffuse,ambient,specular,emissive;float power;uint32_t texture,ramp;};
struct Submit{float diffuse[4]{},ambient[4]{},specular[4]{},emissive[4]{},power=0;};
void Apply(Submit&mesh,const S3DMaterial&m){struct{S3DMaterial material;}part{m};BODY}
int main(int argc,char**argv){assert(argc==2);static_assert(sizeof(S3DMaterial)==80);std::ifstream f(argv[1],std::ios::binary);S3DMaterial m[2];f.read(reinterpret_cast<char*>(m),sizeof(m));assert(f.gcount()==sizeof(m));for(auto&v:m){Submit s;assert(s.diffuse[3]==0&&v.diffuse.a==1);Apply(s,v);const float*expected=&v.diffuse.r;const float*got=s.diffuse;for(int j=0;j<17;++j)assert(expected[j]==got[j]);}puts("PASS 2 materials 34 channels; zero-alpha baseline rejected");}
'''.replace('BODY',body))
 with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:data=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/vortex.i3d'))
 assert sha(data)==ASSET_SHA;offset=20+struct.unpack_from('<I',data,16)[0]+276;payload=data[offset:offset+160];fixture=output/'materials.fixture';fixture.write_bytes(payload);binary=output/'material'
 q=subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',str(driver),'-o',str(binary)],capture_output=True,text=True);(output/'compile.log').write_text(q.stdout+q.stderr);assert q.returncode==0
 q=subprocess.run([str(binary),str(fixture)],capture_output=True,text=True);(output/'run.log').write_text(q.stdout+q.stderr);assert q.returncode==0 and not q.stderr
 r=dict(status='pass',accepted=False,materials=2,channels=34,asset_sha256=ASSET_SHA,source_body_sha256=sha(body.encode()),material_sha256=sha(payload),source_sha256=sha(source.encode()),scope=__doc__,limits='Material carrier contract only; software shader ignores these descriptors, modern consumes them. Actual policy films and native context separate.');(output/'manifest.json').write_text(json.dumps(r,indent=2)+'\n');print(q.stdout.strip());return r
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.output)
