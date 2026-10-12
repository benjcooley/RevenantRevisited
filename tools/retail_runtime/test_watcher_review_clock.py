"""Exercise the actual texture clock under review and reference cadences."""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]

class WatcherClockTests(unittest.TestCase):
    def test_review_rate_and_reference_clock(self):
        source = (ROOT / 'src/effect.cpp').read_text()
        start = source.index('void TAuthoredStaticMeshEffect::TickAndSubmitForTest_BESPOKE')
        end = source.index('// Explicit watcher comparison route', start)
        actual = source[start:end]
        prefix = r'''
#include <cassert>
#include <strings.h>
#define stricmp strcasecmp
enum class EFxDebugMode { Normal };
char StartupTestMode[32]="vfx-review";
float StartupVfxReviewWatcherRate=.25f;
struct TTime {static double dt; static double DeltaTime(){return dt;}};
double TTime::dt=1.0/60;
struct TAuthoredStaticMeshEffect {
 bool animate_textures_=true;double texture_tick_seconds_=0;int advances=0;
 void NextFrame(){++advances;}
 void TickAndSubmitForTest_BESPOKE(EFxDebugMode);
};
'''
        main = r'''
int main(){
 TAuthoredStaticMeshEffect quarter;
 for(int i=0;i<600;++i)quarter.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);
 assert(quarter.advances==60);
 StartupVfxReviewWatcherRate=.5f;TAuthoredStaticMeshEffect half;
 for(int i=0;i<600;++i)half.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);
 assert(half.advances==120);
 StartupVfxReviewWatcherRate=1;TAuthoredStaticMeshEffect retail;
 for(int i=0;i<600;++i)retail.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);
 assert(retail.advances==240);
 __builtin_strcpy(StartupTestMode,"vfx");StartupVfxReviewWatcherRate=.25f;
 TTime::dt=1.0/24;TAuthoredStaticMeshEffect reference;
 for(int i=0;i<240;++i)reference.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);
 assert(reference.advances==240);
 TAuthoredStaticMeshEffect fixed;fixed.animate_textures_=false;
 fixed.TickAndSubmitForTest_BESPOKE(EFxDebugMode::Normal);assert(fixed.advances==0);
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            cpp = Path(tmp)/'clock.cpp'; binary = Path(tmp)/'clock'
            cpp.write_text(prefix+actual+main)
            subprocess.run(['clang++','-std=c++17','-fsanitize=address,undefined',str(cpp),'-o',str(binary)],check=True)
            subprocess.run([str(binary)],check=True)

if __name__ == '__main__':
    unittest.main()
