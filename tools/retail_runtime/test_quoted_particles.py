"""Quoted particle grammar, exact admission, and independent authored poses."""
import tempfile,unittest
from pathlib import Path
from static_particles_profile_contract import ROOT,Y_PROFILES,run as profile_contract
from quoted_particles_parser_probe import run as parser_contract
from quoted_particles_pose_probe import run as pose_contract

class QuotedParticleContracts(unittest.TestCase):
    def test_whole_native_decimal_integer_tokens(self):
        with tempfile.TemporaryDirectory(prefix='quoted-particle-parser-')as tmp:
            report=parser_contract(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp))
        self.assertEqual(report['status'],'pass')
        self.assertEqual([c['native_size']for c in report['cases']],[0,0,5,5,0,-5,0,0,None])
        self.assertTrue(all(c['equal']for c in report['cases']))
        for case in report['cases'][1:8]:
            self.assertTrue(case['native_events'])
            self.assertTrue(all(e['token_type']==8 for e in case['native_events']))
        self.assertFalse(report['accepted'])

    def test_four_literal_profiles_fail_closed(self):
        with tempfile.TemporaryDirectory(prefix='quoted-particle-profile-')as tmp:
            report=profile_contract(ROOT/'data/imagery.rvi',Path(tmp),Y_PROFILES)
        self.assertEqual(report['status'],'pass')
        self.assertEqual([c['positive_cases']for c in report['cases']],[4]*4)
        self.assertEqual([c['rejection_cases']for c in report['cases']],[148,148,562,604])
        self.assertTrue(all(c['categories']['C reject vertex_float']==32 for c in report['cases']))
        self.assertFalse(report['accepted'])

    def test_actual_decoder_and_live_emitter_matrices(self):
        with tempfile.TemporaryDirectory(prefix='quoted-particle-pose-')as tmp:
            report=pose_contract(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',ROOT/'data/imagery.rvi',Path(tmp))
        self.assertEqual(report['status'],'pass')
        self.assertEqual(sum(c['key_channels']for c in report['cases']),32805)
        self.assertEqual(sum(c['matrix_channels']for c in report['cases']),51040)
        self.assertTrue(all(not c['errors']for c in report['cases']))
        self.assertFalse(report['accepted'])

if __name__=='__main__':unittest.main()
