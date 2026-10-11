"""Compile actual FillInputs: Pulse must use the cached static emitter pose."""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

from speed_profile_contract import function, ROOT


class CachedParticlePoseTests(unittest.TestCase):
    def test_actual_fill_inputs_cached_pose_and_unrelated_fallback(self):
        source = (ROOT / 'src/3dimage.cpp').read_text()
        fill = function(source, '    static void FillInputs(')
        make = function(source, 'static void MakeMatrix(')
        driver = r'''
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>
#include "math3d.h"
#include "cometmatrix.h"
#include "fgeysermatrix.h"
#include "dustmatrix.h"
#include "poisonmatrix.h"
struct S3DPoint { int x=0,y=0,z=0; };
struct V { float x=0,y=0,z=0; };
struct Pose { std::array<float,16> matrix{}; V origin,position,scale; };
struct Inputs { V owner_position,object_zero_origin; int owner_face=0,animation_frame=0; bool has_object_zero_origin=false; std::vector<Pose> emitters{1}; };
struct Controller { Inputs inputs; struct {std::vector<int> emitters{1};} track; };
struct S3DAnimObj { hmm_vec3 pos{},scl{}; };
struct Imagery {
 int sampled=-1,expected_state=0;bool lightstrip=false,comet=false,fgeyser=false,dust=false,poison=false;
 bool HasRetailLightStripProfile(){return lightstrip;}
 bool HasRetailCometProfile(){return comet;}
 bool HasRetailFGeyserProfile(){return fgeyser;}
 bool HasRetailDustProfile(){return dust;}
 bool HasRetailPoisonProfile(){return poison;}
 bool HasRetailMightPartSysProfile(){return false;}
 bool HasRetailImmortalmightPartSysProfile(){return false;}
 bool HasRetailFmasteryPartSysProfile(){return false;}
 bool HasRetailSpeedFamilyPartSysProfile(uint32_t){return false;}
 bool HasRetailCameraParticleProfile(uint32_t){return false;}
 bool HasRetailStaticParticleProfile(uint32_t id){return id==0xad92bd38u;}
 bool GetUninterpolatedAniKey(int object,int state,int frame,hmm_vec3& p,hmm_vec3& r,hmm_vec3& s,bool inclusive=false){
  assert(object==1 && state==expected_state && inclusive==(lightstrip||comet||fgeyser||dust||poison));sampled=frame;p={float(frame),0,0};r={0,0,0};s={float(frame+1),1,1};return true;
 }
};
struct TObjectInstance { uint32_t id=0xad92bd38u;int state=0,frame=8;S3DPoint Pos(){return {};};int GetFace(){return 0;}uint32_t ObjId(){return id;}int GetState(){return state;}int GetFrame(){return frame;} };
struct T3DAnimator { Imagery imagery; S3DAnimObj bone;int cached=7;
 Imagery* Get3DImagery(){return &imagery;}int GetFrame(){return cached;}
 S3DAnimObj* GetObject(int){return &bone;}
 bool GetObjectMatrix(int,hmm_mat4* m){MtxClear(m);m->Elements[3][0]=8;bone.pos={8,0,0};bone.scl={9,1,1};return true;}
 bool CameraParticleEmitterLocalMatrix(int,hmm_mat4&,hmm_vec3*,hmm_vec3*){assert(false);return false;}
 bool SpeedEmitterLocalMatrix(hmm_mat4&,hmm_vec3*,hmm_vec3*){assert(false);return false;}
};
MAKE
struct Extracted {
FILL
};
int main(){
 for(int variant=0;variant<3;++variant){
  TObjectInstance owner;T3DAnimator animator;Controller c;
  if(variant==1)owner.id=0x12345678;
  if(variant==2)owner.state=1;
  Extracted::FillInputs(c,animator,owner);
  const auto& p=c.inputs.emitters[0];
  const float expected=variant==0?7:8;
  assert(c.inputs.animation_frame==expected);
  assert(p.position.x==expected && p.scale.x==expected+1);
  assert(p.origin.x==expected && p.matrix[12]==expected);
  assert(animator.imagery.sampled==(variant==0?7:-1));
 }
 // Exact LightStrip uses the same cached pose in all three authored states.
 for(int state=0;state<3;++state){TObjectInstance owner;owner.id=0x10da54d0u;owner.state=state;
  T3DAnimator animator;animator.imagery.lightstrip=true;animator.imagery.expected_state=state;Controller c;
  for(owner.frame=8;owner.frame<=10;++owner.frame){Extracted::FillInputs(c,animator,owner);
   assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);}}
 // Comet's three emitters obey the same render-cached frame despite skipped draws.
 {TObjectInstance owner;owner.id=0xf32bcfacu;T3DAnimator animator;animator.imagery.comet=true;Controller c;
  for(owner.frame=8;owner.frame<=10;++owner.frame){Extracted::FillInputs(c,animator,owner);
   assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);}}
 // Geyser's cached emitters remain stable across skipped draws as well.
 {TObjectInstance owner;owner.id=0xad92bd31u;T3DAnimator animator;animator.imagery.fgeyser=true;Controller c;
  for(owner.frame=8;owner.frame<=10;++owner.frame){Extracted::FillInputs(c,animator,owner);
   assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);}}
 {TObjectInstance owner;owner.id=0x0c052637u;T3DAnimator animator;animator.imagery.dust=true;Controller c;
  for(owner.frame=8;owner.frame<=10;++owner.frame){Extracted::FillInputs(c,animator,owner);
   assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);}}
 {TObjectInstance owner;owner.id=0xe0a3bc42u;T3DAnimator animator;animator.imagery.poison=true;Controller c;
  for(owner.frame=8;owner.frame<=10;++owner.frame){Extracted::FillInputs(c,animator,owner);
   assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);}}
 // Three authoritative Pulses before Animate retain its cached pose.
 TObjectInstance owner;T3DAnimator animator;Controller c;
 for(owner.frame=8;owner.frame<=10;++owner.frame){
  Extracted::FillInputs(c,animator,owner);
  assert(c.inputs.animation_frame==7 && c.inputs.emitters[0].position.x==7);
 }
 animator.cached=10;
 Extracted::FillInputs(c,animator,owner);
 assert(c.inputs.animation_frame==10 && c.inputs.emitters[0].position.x==10);
}
'''.replace('MAKE', make).replace('FILL', fill)
        with tempfile.TemporaryDirectory(prefix='static-cached-pose-') as directory:
            p = Path(directory)
            (p / 'test.cpp').write_text(driver)
            command = [shutil.which('clang++') or 'c++', '-std=c++17',
                       '-fsanitize=address,undefined', '-iquote', str(ROOT / 'src'),
                       '-I' + str(ROOT / 'thirdparty/handmademath'),
                       str(p / 'test.cpp'), str(ROOT / 'src/math3d.cpp'), '-o', str(p / 'test')]
            compiled = subprocess.run(command, capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            result = subprocess.run([str(p / 'test')], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
