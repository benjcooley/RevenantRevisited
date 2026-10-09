"""Guard against upgrading FireSwarm's empty early reference images to credit."""
import struct
import tempfile
import unittest
from pathlib import Path
from fireswarm_render_scope import classify,visible_pixels
from software_probe import save_rgb565_png


class FireSwarmRenderScopeTests(unittest.TestCase):
    def test_empty_matches_do_not_receive_appearance_credit(self):
        base=dict(state_errors=[],max_corner_error=0,differing_rgb565_pixels=0,image_hashes=['a','a','z','z'])
        report=dict(cases=[dict(base,tick=0),dict(base,tick=30),dict(base,tick=36,image_hashes=['a','a','z','different'])])
        result=classify(report,{0:0,30:12,36:9})
        self.assertEqual(result['visible_frontend_ticks'],[30])
        self.assertEqual(result['empty_matching_ticks'],[0])
        self.assertEqual(result['failed_sample_ticks'],[36])
        self.assertFalse(result['early_nonempty_appearance_verified'])
        self.assertFalse(result['complete_effect_accepted'])
        self.assertFalse(result['independent_device_or_metal_parity'])

    def test_generated_png_visibility_counts_actual_rgb(self):
        with tempfile.TemporaryDirectory()as tmp:
            path=Path(tmp)/'fixture.png';save_rgb565_png(path,struct.pack('<4H',0,0xffff,0,0x001f),2,2)
            self.assertEqual(visible_pixels(path),2)


if __name__=='__main__':unittest.main()
