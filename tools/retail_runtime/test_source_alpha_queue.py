"""Compile actual queue admission: source alpha order and old Gold policy."""
from pathlib import Path
import subprocess,tempfile,unittest
from speed_profile_contract import ROOT,function

class SourceAlphaQueueTests(unittest.TestCase):
 def test_separate_admission_and_order(self):
  source=(ROOT/'src/engine/render/renderer.cpp').read_text()
  methods='\n'.join(function(source,s) for s in ('void TRenderer::SubmitGoldFlareAfterFx(','void TRenderer::SubmitRetailSourceAlphaAfterFx('))
  driver='''#include <vector>
#include <cassert>
struct SHelperMeshSubmit {unsigned mesh=0;bool additive_blend=false;int retail_lighting=0;bool retail_source_alpha=false;};
struct TRenderer {std::vector<int> meshes={0,0,0};std::vector<SHelperMeshSubmit> gold_flare_after_fx_queue;
void SubmitGoldFlareAfterFx(const SHelperMeshSubmit&);void SubmitRetailSourceAlphaAfterFx(const SHelperMeshSubmit&);};
'''+methods+'''
int main(){TRenderer r;SHelperMeshSubmit a{};a.mesh=1;a.retail_lighting=3;a.retail_source_alpha=true;
r.SubmitRetailSourceAlphaAfterFx(a);a.mesh=3;r.SubmitRetailSourceAlphaAfterFx(a);assert(r.gold_flare_after_fx_queue.size()==2);
assert(r.gold_flare_after_fx_queue[0].mesh==1&&r.gold_flare_after_fx_queue[1].mesh==3);
for(int mode:{0,1,2,4}){a.retail_lighting=mode;r.SubmitRetailSourceAlphaAfterFx(a);}a.retail_lighting=3;
a.retail_source_alpha=false;r.SubmitRetailSourceAlphaAfterFx(a);a.retail_source_alpha=true;a.additive_blend=true;r.SubmitRetailSourceAlphaAfterFx(a);
a.additive_blend=false;for(unsigned mesh:{0u,4u}){a.mesh=mesh;r.SubmitRetailSourceAlphaAfterFx(a);}assert(r.gold_flare_after_fx_queue.size()==2);
a.mesh=2;r.SubmitGoldFlareAfterFx(a);a.additive_blend=true;r.SubmitGoldFlareAfterFx(a);assert(r.gold_flare_after_fx_queue.size()==2);
a.retail_lighting=1;r.SubmitGoldFlareAfterFx(a);assert(r.gold_flare_after_fx_queue.size()==3&&r.gold_flare_after_fx_queue.back().mesh==2);
a.additive_blend=false;r.SubmitGoldFlareAfterFx(a);assert(r.gold_flare_after_fx_queue.size()==3);}
'''
  with tempfile.TemporaryDirectory(prefix='source-alpha-queue-')as temp:
   p=Path(temp);(p/'test.cpp').write_text(driver)
   subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True,capture_output=True)
   subprocess.run([str(p/'test')],check=True,capture_output=True)
if __name__=='__main__':unittest.main()
