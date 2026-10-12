"""Exact genuine Burn controller/profile/helper contracts."""
import tempfile,unittest,subprocess
from pathlib import Path
from test_vfxreviewspawn import function,ROOT
from burn_profile_probe import run as profile
from burn_state_probe import run as state
from burn_owner_probe import run as owner
EVIDENCE=Path('/Users/benjamincooley/RevenantRetailLab/research/vfx-next-burn-aura-20261011')
class BurnPublicTests(unittest.TestCase):
 def test_integer_pose_restores_shared_interpolation_on_failure(self):
  helper=function((ROOT/'src/effects/burnpublicpreview.cpp').read_text(),'static bool IntegerBone(')
  driver=r'''
#include <cassert>
struct hmm_mat4{int value=0;};bool Interpolate=true;
struct Object{hmm_mat4 matrix;};
struct Image{bool succeeds=true;bool CalcObjectMatrix(Object*o,int state,int frame,void*root,bool parents){assert(!Interpolate&&state==14&&frame==0&&!root&&parents);o->matrix.value=42;return succeeds;}};
struct T3DAnimator{Object object;Image image;bool missing=false;int NumObjects(){return 22;}Object*GetObject(int){return missing?nullptr:&object;}Image*Get3DImagery(){return &image;}};
'''+helper+r'''
int main(){T3DAnimator a;hmm_mat4 m;assert(!IntegerBone(nullptr,0,m));assert(!IntegerBone(&a,-1,m));assert(!IntegerBone(&a,22,m));for(bool policy:{false,true}){Interpolate=policy;assert(IntegerBone(&a,0,m)&&m.value==42&&Interpolate==policy);a.image.succeeds=false;assert(!IntegerBone(&a,0,m)&&Interpolate==policy);a.image.succeeds=true;a.missing=true;assert(!IntegerBone(&a,0,m)&&Interpolate==policy);a.missing=false;}}
'''
  driver=driver.replace('#include <cassert>','#include <cassert>\n#include <initializer_list>')
  with tempfile.TemporaryDirectory(prefix='burn-integer-api-')as tmp:
   p=Path(tmp);(p/'check.cpp').write_text(driver)
   subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'check.cpp'),'-o',str(p/'check')],check=True)
   subprocess.run([str(p/'check')],check=True)
 def test_exact_legacy_resource(self):
  with tempfile.TemporaryDirectory(prefix='burn-profile-')as t:r=profile(Path(t)/'proof')
  self.assertEqual((r['status'],r['accept'],r['reject']),('pass',4,815))
 def test_full_original_state_matrix_rng_lifetime(self):
  with tempfile.TemporaryDirectory(prefix='burn-state-')as t:r=state(Path(t)/'proof',EVIDENCE/'burn-native-final90/manifest.json',EVIDENCE/'source-actor-pose-locals-final/run.log')
  self.assertEqual(r['status'],'pass');self.assertEqual((r['state_fields'],r['matrix_fields'],r['rng_fields']),(108750,90240,90));self.assertFalse(r['errors']);self.assertLess(r['max_state_error'],3e-5)
 def test_all_authored_vertices_actual_absoluteposition_submission(self):
  with tempfile.TemporaryDirectory(prefix='burn-owner-')as t:r=owner(Path(t)/'proof',EVIDENCE/'burn-state-first/source.trace')
  self.assertEqual((r['status'],r['vertices'],r['draws']),('pass',22560,5640))
if __name__=='__main__':unittest.main()
