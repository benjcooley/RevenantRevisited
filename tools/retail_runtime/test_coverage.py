#!/usr/bin/env python3
"""Ensure fixture routing never promotes partial evidence to full acceptance."""
import argparse
import json
from pathlib import Path
import time
import unittest
from coverage import generate,canonical_id


class CoverageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.coverage=generate()

    def test_real_ids_route_by_exact_identity_and_counts_are_separate(self):
        counts=self.coverage['counts']
        self.assertEqual(counts['ledger_rows'],176)
        self.assertEqual(counts['exposed_real_type_ids'],37)
        rows=list(self.coverage['types'].values())
        self.assertEqual(counts['fully_accepted_rows'],sum(bool(r['ledger_acceptance']['accepted']) for r in rows))
        self.assertEqual(counts['reviewed_visual_pass_rows'],sum(bool(r['ledger_acceptance']['visual_fidelity']) for r in rows))
        self.assertEqual(counts['reviewed_visual_pass_rows']+counts['remaining_visual_review_rows'],counts['ledger_rows'])
        self.assertEqual(self.coverage['exposed_without_ledger'],[])
        for type_id in self.coverage['rendered_without_direct_adapter']:
            self.assertIsNone(self.coverage['types'][type_id]['control'])
            self.assertEqual(self.coverage['types'][type_id]['gates']['pixel_frontend']['status'],'shared_original_raster_pass')
        base=self.coverage['types']['0x562c7439']
        self.assertIsNone(base['control'])
        colored=self.coverage['types']['0x562c743a']
        self.assertEqual(colored['control']['profile'],'colored_ribbon')
        self.assertEqual(canonical_id('0xAB8800DD'),'0xab8800dd')

    def test_frontend_pixel_pass_does_not_clear_runtime_gpu_or_full_gates(self):
        row=self.coverage['types']['0x50ba373b']
        self.assertEqual(row['gates']['pixel_frontend']['status'],'shared_original_raster_pass')
        self.assertEqual(row['gates']['gpu']['status'],'differences_found')
        self.assertEqual(row['gates']['full_acceptance']['status'],'open')
        self.assertFalse(row['gates']['runtime']['natural_context_verified'])
        self.assertEqual(row['gates']['visual_review']['status'],'pass_for_tested_configuration')
        fog=self.coverage['types']['0x180674ba']
        self.assertEqual(fog['gates']['gpu']['status'],'not_checked')
        self.assertEqual(fog['gates']['full_acceptance']['status'],'open')
        self.assertEqual(fog['gates']['visual_review']['status'],'pending')

    def test_current_report_formats_count_pixels_without_promoting_visual_reviews(self):
        self.assertEqual(self.coverage['counts']['evidence_rows_with_shared_raster_pixel_pass'],
                         self.coverage['counts']['ledger_reported_render_frontend_rows'])
        for type_id in ('0xad92bc28','0xad92bc37','0xad92bd36'):
            row=self.coverage['types'][type_id]
            self.assertEqual(row['gates']['pixel_frontend']['status'],'shared_original_raster_pass')
            self.assertEqual(row['gates']['visual_review']['status'],'pending')

    def test_fireball_route_requires_actual_moving_distinct_endpoint_scenario(self):
        control=self.coverage['types']['0x63fd382a']['control']
        self.assertEqual(control['profile'],'fireball_head')
        self.assertEqual(control['kind'],'projectile')
        actions=control['example_actions']
        actors=[action for action in actions if action['op']=='create_entity']
        self.assertEqual(len(actors),2)
        self.assertNotEqual(actors[0]['position'],actors[1]['position'])
        self.assertTrue(any(action['op']=='validate_execution_contract' for action in actions))
        self.assertFalse(any(action['op']=='create_effect' for action in actions))
        self.assertEqual(self.coverage['types']['0x63fd382a']['gates']['full_acceptance']['status'],'open')

    def test_auxiliary_state_kernels_never_receive_fabricated_leaf_ids(self):
        self.assertIn('shared_missile_base',self.coverage['auxiliary_profiles'])
        self.assertIn('authoredpartsys',self.coverage['auxiliary_profiles'])
        self.assertFalse(any(row['control'] and row['control']['profile']=='shared_missile_base'
            for row in self.coverage['types'].values()))

    def test_new_static_and_texture_proofs_keep_natural_and_device_gates_open(self):
        for type_id in ('0xadbcef14','0xaeaeeb23','0xaeaeeb24','0xaeaeeb25','0xadbcef13','0xadbcef19',
                        '0x5975abde','0x63fd3827'):
            row=self.coverage['types'][type_id]
            self.assertEqual(row['gates']['pixel_frontend']['status'],'shared_original_raster_pass')
            self.assertEqual(row['gates']['full_acceptance']['status'],'open')
            self.assertFalse(row['gates']['runtime']['natural_context_verified'])


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path);args=parser.parse_args()
    start=time.perf_counter();result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(CoverageTests))
    report=dict(status='pass' if result.wasSuccessful() else 'fail',tests=result.testsRun,failures=len(result.failures),
        errors=len(result.errors),elapsed_seconds=time.perf_counter()-start,scope='Exact-ID routing and independent state/pixel/runtime/GPU/full gates; no new acceptance granted')
    if args.output:
        args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n')
    raise SystemExit(0 if result.wasSuccessful() else 1)
