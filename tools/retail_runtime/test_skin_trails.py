"""Exact malformed key, constant trails, render-clock history and default controls."""
import subprocess,tempfile,unittest,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
CONTROL=r'''
#include "authoredpartsys.h"
#include <cassert>
#include <string>
using namespace authored_partsys;
int main(){
 const std::string tag="obj=(stone),particle=#particle,pps=25,lifespan=10:10,initialvelocity=4:5,spread=360,azimuth=360,localrotation=[0:(0,0,0),100(0,0,0)]";
 Definition d;std::string error;assert(ParseDefinition(tag+",numtrails=3:3",d,error));
 assert(d.localrotation.IsCurve()&&d.localrotation.keys.size()==2);
 for(auto& k:d.localrotation.keys)assert(k.percent==0&&k.value==Value{});
 for(auto bad:{"1e-100:1e-100","3:4","-1:-1","3.1:3.1","65:65","[0:3,100:3]"}){
  Definition out=d;assert(!ParseDefinition(tag+",numtrails="+bad,out,error));assert(out.numtrails.constant[0]==3);
 }
 for(auto malformed:{"[0:(0,0,0),100(1,0,0)]","[0:(0,0,0),100(1e-100,0,0)]","[0:(0,0,0),99(0,0,0)]"}){
  Definition out;assert(!ParseDefinition(std::string("obj=e,particle=p,localrotation=")+malformed,out,error));
 }
 TickInputs in;in.emitters.resize(1);in.emitters[0].origin={0,0,40};in.emitters[0].position={0,0,40};in.emitters[0].matrix[14]=40;
 State sim;assert(sim.Initialize(d,in,0,error));uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767)%(b-a+1);};
 sim.Advance(in,rng);const auto initial=sim.Particles()[0].trail_positions;assert(initial.size()==3&&initial[0]==initial[1]&&initial[1]==initial[2]);
 sim.Advance(in,rng);assert(sim.Particles()[0].trail_positions==initial); // no render: unchanged
 const auto current=sim.SampleRender(0,rng);const auto before=seed;auto trail=sim.SampleTrailPositions(0);
 assert(trail.size()==3&&trail[0]==initial[1]&&trail[1]==initial[2]&&trail[2]==current.position&&seed==before);
 auto repeated=sim.SampleTrailPositions(0);assert(repeated[0]==trail[1]&&repeated[1]==trail[2]&&repeated[2]==current.position&&seed==before);
 Definition zero;assert(ParseDefinition(tag,zero,error)&&zero.numtrails.constant[0]==0);State no_trail;assert(no_trail.Initialize(zero,in,0,error));no_trail.Advance(in,rng);assert(no_trail.SampleTrailPositions(0).empty());
}
'''
TRACE=r'''
#include "authoredpartsys.h"
#include <fstream>
#include <iterator>
#include <cassert>
#include <cstdio>
using namespace authored_partsys;
void word(const void*p,size_t n){assert(fwrite(p,1,n,stdout)==n);}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1]);std::string tag((std::istreambuf_iterator<char>(f)),{});Definition d;std::string error;assert(ParseDefinition(tag,d,error));TickInputs in;in.emitters.resize(d.objects.size());in.has_object_zero_origin=true;State sim;assert(sim.Initialize(d,in,0,error));uint32_t seed=1;auto rng=[&](int a,int b){seed=seed*214013u+2531011u;return a+int((seed>>16)&32767)%(b-a+1);};
for(int tick=0;tick<95;++tick){in.animation_frame=tick%60;sim.Advance(in,rng);
for(size_t i=0;i<sim.Particles().size();++i){const auto&p=sim.Particles()[i];if(!p.alive)continue;auto sample=sim.SampleRender(i,rng);
#ifdef NEW_API
assert(sim.SampleTrailPositions(i).empty());
#endif
word(&i,sizeof i);word(&p.age,sizeof p.age);word(&p.lifespan,sizeof p.lifespan);
for(float v:{p.position[0],p.position[1],p.position[2],p.velocity[0],p.velocity[1],p.velocity[2],p.scale,p.alpha,p.color[0],p.color[1],p.color[2],sample.position[0],sample.position[1],sample.position[2],sample.rotation[0],sample.rotation[1],sample.rotation[2],sample.scale,sample.alpha,sample.color[0],sample.color[1],sample.color[2]})word(&v,4);
}word(&seed,4);}}
'''
class SkinTrailTests(unittest.TestCase):
 def test_parser_and_render_history_controls(self):
  with tempfile.TemporaryDirectory()as t:
   p=Path(t);(p/'test.cpp').write_text(CONTROL)
   subprocess.run(['clang++','-std=c++17','-O1','-fsanitize=address,undefined','-iquote',str(ROOT/'src'),str(p/'test.cpp'),str(ROOT/'src/authoredpartsys.cpp'),str(ROOT/'src/partsysdefinition.cpp'),'-o',str(p/'test')],check=True)
   subprocess.run([str(p/'test')],check=True)
 def test_default_zero_existing_profiles_byte_identical_to_before_change(self):
  with tempfile.TemporaryDirectory()as t:
   p=Path(t);old=p/'old';old.mkdir();(p/'trace.cpp').write_text(TRACE)
   for name in ('authoredpartsys.cpp','authoredpartsys.h','partsysdefinition.cpp','partsysdefinition.h'):
    (old/name).write_bytes(subprocess.check_output(['git','show','c692baf5:src/'+name],cwd=ROOT))
   for label,source in [('old',old),('new',ROOT/'src')]:
    args=['clang++','-std=c++17','-O1','-iquote',str(source),str(p/'trace.cpp'),str(source/'authoredpartsys.cpp'),str(source/'partsysdefinition.cpp'),'-o',str(p/(label+'-trace'))]
    if label=='new':args.insert(1,'-DNEW_API')
    subprocess.run(args,check=True)
   from speed_controller_preflight import read_asset
   with zipfile.ZipFile(ROOT/'data/imagery.rvi')as z:
    for asset in ('speed','restorelife','dexterity','antimagic'):
     d=z.read(next(n for n in z.namelist()if n.lower()=='imagery/magic/'+asset+'.i3d'));tag=next(t['parameters']for t in read_asset(d)['tags']if t['name']=='partsys');tagfile=p/'tag';tagfile.write_text(tag)
     self.assertEqual(subprocess.check_output([str(p/'old-trace'),str(tagfile)]),subprocess.check_output([str(p/'new-trace'),str(tagfile)]),asset)
if __name__=='__main__':unittest.main()
