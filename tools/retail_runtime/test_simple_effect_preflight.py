"""A base registration call must not admit a custom leaf as generic/static."""
import tempfile
import unittest
from pathlib import Path
from simple_effect_preflight import ROOT, run


class CompleteConstructorPreflightTests(unittest.TestCase):
    def test_complete_factories_and_controller_deferrals(self):
        with tempfile.TemporaryDirectory(prefix='simple-effect-native-') as tmp:
            report = run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',
                         ROOT/'data/imagery.rvi', Path(tmp))
        self.assertEqual(report['registry_count'], 93)
        self.assertEqual(len(report['complete_named_constructors']), 92)
        self.assertEqual(report['default_constructor'], '0x406160')
        rows = {r['name']:r for r in report['candidates']}
        self.assertEqual(rows['energyspray']['builder_vtable'], '0x5b0a24')
        self.assertEqual(rows['energyspray']['animator_vtable'], '0x5b0a28')
        self.assertEqual(rows['Createfood']['builder_vtable'], '0x5a8aa0')
        self.assertEqual(rows['Createfood']['animator_vtable'], '0x5a8aa4')
        self.assertEqual(rows['Labback']['builder_vtable'], '0x5a359c')
        self.assertEqual(rows['Quick']['animator_vtable'], '0x5a370c')
        self.assertEqual([t['name'] for t in rows['Labback']['topology']['tags']], ['blendcont', 'partsys'])
        self.assertEqual([t['name'] for t in rows['Quick']['topology']['tags']], ['play', 'partsys', 'blendcont'])
        self.assertEqual(rows['Createfood']['topology']['version'], 1)
        self.assertTrue(all(r['decision']=='defer' for r in rows.values()))
        self.assertFalse(report['original_render_executed'])
        self.assertFalse(report['port_comparison_executed'])
        self.assertFalse(report['visible_pixel_credit'])
        self.assertFalse(report['full_acceptance_granted'])


if __name__ == '__main__':
    unittest.main()
