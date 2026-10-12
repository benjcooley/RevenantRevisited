"""Compile actual SetVortex Advance and native owner-Z helper against full caller."""
import argparse,json,struct,subprocess
from pathlib import Path
from character_native_preflight import ROOT,sha

def run(native,output):
 output.mkdir(parents=True,exist_ok=False);source=(ROOT/'src/effect.cpp').read_text();a=source.index('void TSetVortexEffect_Bespoke::Advance(');b=source.index('\nvoid TSetVortexEffect_Bespoke::Submit(',a);method=source[a:b]
 c=source.index('\nvoid TSetVortexEffect_Bespoke::TickAndSubmitForTest_BESPOKE',b);submit=source[b:c]
 data=json.loads(native.read_text());first=data['records'][0]['draws'][0];vertices=list(struct.iter_unpack('<8f',bytes.fromhex(first['vertices_hex'])));indices=struct.unpack('<36H',bytes.fromhex(first['indices_hex']))
 init=''.join('x.vertices_.push_back({{'+','.join(repr(q) for q in v[:3])+'},{'+','.join(repr(q) for q in v[6:8])+'}});'for v in vertices)+'x.indices_={'+','.join(map(str,indices))+'};'
 driver=output/'probe.cpp';driver.write_text('''#include "retailsoftwaretransform.h"
#include <cstdio>
#include <cmath>
#include <vector>
#define REV_FIX_Z_VALUE(x) ((x)*1.46f)
#define WORLD3D_Z_SCALE 1.5f
bool StartupVfxNativeDomain=true;
enum class EFxDebugMode{Normal};enum class EFxBlend{Alpha};enum class EFxDepthMode{TestNoWrite};
struct SQuadDrawItem {int corner_count=0,retail_texture=0;bool retail_software_projection=false;struct {int texture;uint8_t blend,depth_mode;}key;EFxDebugMode debug_mode;float color_rgba[4],world_pos[4][3],uv[4][2];};
struct FakeRenderer{std::vector<SQuadDrawItem>items;void SubmitFxQuad(SQuadDrawItem item){items.push_back(item);}}renderer,*Renderer=&renderer;
struct Vertex{float pos[3],uv[2];};
struct TSetVortexEffect_Bespoke {bool initialized_=true; double sim_accum_ms_=0;hmm_vec3 cached_d3d_position_={0,0,RetailSoftwareOwnerZ(30)};float current_height_=30;int direction_=1;void SetCommandDone(bool){}void Advance(double);std::vector<Vertex>vertices_;std::vector<uint16_t>indices_;int texture_=1;float diffuse_[4]={1,1,1,1};void Submit(EFxDebugMode)const;};
'''+method+submit+'''\nint main(){TSetVortexEffect_Bespoke x;'''+init+'''for(int t=1;t<=105;++t){x.Advance(1./24.);printf("%d %.9g %.9g %d\\n",t,x.cached_d3d_position_.Z,x.current_height_,x.direction_);renderer.items.clear();x.Submit(EFxDebugMode::Normal);for(const auto&q:renderer.items){printf("Q %d %d %d",t,q.retail_texture,int(q.retail_software_projection));for(int j=0;j<3;++j){for(float f:q.world_pos[j])printf(" %.9g",f);for(float f:q.uv[j])printf(" %.9g",f);}puts("");}}}
''')
 exe=output/'probe';subprocess.run(['clang++','-std=c++20','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),'-I',str(ROOT/'thirdparty/handmademath'),str(driver),str(ROOT/'src/math3d.cpp'),'-o',str(exe)],check=True);lines=subprocess.check_output([str(exe)],text=True).splitlines();quads=[line for line in lines if line.startswith('Q ')];lines=[line for line in lines if not line.startswith('Q ')];assert len(quads)==105*12;data=json.loads(native.read_text());assert len(lines)==len(data['records'])==105
 maximum=0
 for line,r in zip(lines,data['records']):
  t,z,h,d=line.split();assert int(t)==r['tick'] and int(d)==r['direction'];err=max(abs(float(z)-r['cached'][2]),abs(float(h)-r['height']));maximum=max(maximum,err);assert err<1e-6
  assert len(r['draws'])==1;draw=r['draws'][0];assert draw['states']['22']==1 and draw['states']['19']==5 and draw['states']['20']==6 and draw['states']['14']==0
  expected=[.5,0,0,0,0,.5,0,0,0,0,.5,0,0,0,r['cached'][2],1];assert draw['matrix']==expected
 for tick in range(1,106):
  for face,line in enumerate(quads[(tick-1)*12:tick*12]):
   values=line.split();assert list(map(int,values[1:4]))==[tick,3,1];actual=list(map(float,values[4:]));expected=[]
   for index in indices[face*3:face*3+3]:
    v=vertices[index];expected.extend([v[0]*.5,v[1]*.5,v[2]*.5+data['records'][tick-1]['cached'][2],v[6],v[7]])
   assert max(abs(x-y)for x,y in zip(actual,expected))<3e-6
 # The former linear initialization is a meaningful negative control.
 old_z=30./1.4600000381469727+.4000000059604645;assert abs(old_z-data['records'][0]['cached'][2])>.7
 report=dict(status='pass',frames=105,channels=315,submitted_triangles=1260,submitted_vertex_uv_channels=18900,max_error=maximum,original_cull_none=True,negative_linear_z_error=abs(old_z-data['records'][0]['cached'][2]),native_sha256=sha(native),source_sha256=sha(ROOT/'src/effect.cpp'));(output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--native',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();run(a.native,a.output)
