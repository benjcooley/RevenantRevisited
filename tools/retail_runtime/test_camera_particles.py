"""Two literal camera-emitter profiles and original per-frame source matrices."""
import tempfile,unittest
from pathlib import Path
from static_particles_profile_contract import ROOT,CAMERA_PROFILES,run as profile_contract
from quoted_particles_pose_probe import run as pose_contract

class CameraParticleTests(unittest.TestCase):
    def test_exact_visible_base_geometry_rejects_mutations(self):
        with tempfile.TemporaryDirectory(prefix='camera-particle-profile-')as tmp:
            report=profile_contract(ROOT/'data/imagery.rvi',Path(tmp),CAMERA_PROFILES)
        self.assertEqual(report['status'],'pass')
        self.assertEqual([c['positive_cases']for c in report['cases']],[4,4])
        self.assertEqual([c['rejection_cases']for c in report['cases']],[907,665])
        for case in report['cases']:
            self.assertEqual(case['categories']['C reject base_vertex_float'],96)
            self.assertEqual(case['categories']['C reject base_face_index'],18)
        self.assertFalse(report['accepted'])

    def test_actual_source_camera_helper_and_all_authored_integer_keys(self):
        with tempfile.TemporaryDirectory(prefix='camera-particle-pose-')as tmp:
            report=pose_contract(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp),CAMERA_PROFILES)
        self.assertEqual(report['status'],'pass')
        self.assertEqual([c['key_channels']for c in report['cases']],[9000,3780])
        self.assertEqual([c['matrix_channels']for c in report['cases']],[12800,3840])
        self.assertTrue(all(not c['errors']for c in report['cases']))
        self.assertIn('bool T3DAnimator::CameraParticleEmitterLocalMatrix(',report['source_spans'])
        self.assertFalse(report['accepted'])

if __name__=='__main__':unittest.main()
