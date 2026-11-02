"""Regression for the narrow cached nullifier transition sampler contract."""
import subprocess,tempfile,unittest
from pathlib import Path
from speed_profile_contract import ROOT,function
class NullifierTransition(unittest.TestCase):
 def test_exact_profile_emitter_guard_and_state_restore(self):
  body=function((ROOT/'src/game/imagery/3dimage.cpp').read_text(),'bool T3DImagery::GetRetailNullifierEmitterAniKey(')
  source=r'''
#include <cassert>
#include <cstdint>
struct hmm_vec3{};
struct T3DImagery {
 bool admitted=true,result=true;int prevstate=7,prevframe=91,calls=0;
 bool HasRetailStaticParticleProfile(uint32_t id){assert(id==0xad92bd38u);return admitted;}
 void SetPrevState(int s,int f){prevstate=s;prevframe=f;}
 bool GetAniKey(int o,int s,int f,hmm_vec3&,hmm_vec3&,hmm_vec3&){assert(o!=1&&o>=0&&o<=8);assert(s==0&&f==2&&prevstate==0&&prevframe==0);++calls;return result;}
 bool GetRetailNullifierEmitterAniKey(int32_t,int32_t,int32_t,int32_t,hmm_vec3&,hmm_vec3&,hmm_vec3&);
};
'''+body+r'''
int main(){T3DImagery im;hmm_vec3 p,r,s;
for(int admitted=0;admitted<2;++admitted)for(int result=0;result<2;++result)for(int o=-1;o<=9;++o){im.admitted=admitted;im.result=result;int before=im.calls;bool eligible=admitted&&o>=0&&o<=8&&o!=1;
bool ok=im.GetRetailNullifierEmitterAniKey(o,2,0,0,p,r,s);assert(ok==(eligible&&result));assert(im.calls==before+eligible);assert(im.prevstate==7&&im.prevframe==91);}}
'''
  with tempfile.TemporaryDirectory()as d:
   p=Path(d);(p/'probe.cpp').write_text(source);subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'probe.cpp'),'-o',str(p/'probe')],check=True);subprocess.run([str(p/'probe')],check=True)
if __name__=='__main__':unittest.main()
