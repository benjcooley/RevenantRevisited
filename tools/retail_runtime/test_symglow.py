"""Regression for native per-vertex UV stores and render/update separation."""
import struct
import tempfile
import unittest
import zipfile
from pathlib import Path
from unicorn import UC_HOOK_CODE

from symglow_probe import ROOT, SymGlowFixture, parse_asset, build_port


class SymGlowNativeComparison(unittest.TestCase):
    def test_original_render_selects_mode2_and_prevents_object_override(self):
        with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
            mesh=parse_asset(archive.read('Imagery/Misc/symglow.i3d'))
        native=SymGlowFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe',mesh)
        native.reset();native.advance();native.render_state()
        v=native.vm
        self.assertEqual(v.u32(native.obj),0x2040)
        self.assertEqual(v.u32(0x5e8790),2)
        calls=[]
        def observe(uc,address,size,user): calls.append(address)
        h=v.uc.hook_add(UC_HOOK_CODE,observe,begin=0x417d60,end=0x417d60)
        try:
            v.put_u32(v.call_sp+0xc8,native.obj)
            v.call(0x40adfc,stop_address=0x40aeba)
        finally: v.uc.hook_del(h)
        self.assertEqual(calls,[])
        self.assertEqual(v.u32(0x5e8790),2)

    def test_production_preserves_native_uv_stores_with_skipped_renders(self):
        with zipfile.ZipFile(ROOT/'data/imagery.rvi') as archive:
            mesh = parse_asset(archive.read('Imagery/Misc/symglow.i3d'))
        for stride in (1, 3):
            with self.subTest(render_stride=stride), tempfile.TemporaryDirectory() as directory:
                output = Path(directory)
                native = SymGlowFixture(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe', mesh)
                native.reset()
                states = []
                # Two direction reversals plus delayed draw: native Render,
                # rather than a transcribed UV formula, supplies the oracle.
                for tick in range(82):
                    if tick:
                        native.advance()
                    states.append(native.render_state(not tick % stride))
                rng = output/'native-rng.txt'
                rng.write_text(''.join('%d %d %d\n' % row for row in native.random))
                production, _ = build_port(output, mesh, rng, 81, stride)
                for tick, expected in enumerate(states):
                    actual = production[tick]
                    self.assertEqual(expected['timer'], actual['timer'])
                    self.assertEqual(expected['rng_count'], actual['rng_count'])
                    for field in ('zscale', 'dz', 'step'):
                        self.assertEqual(struct.pack('<f', expected[field]), struct.pack('<f', actual[field]), (stride, tick, field))
                    self.assertEqual(
                        b''.join(struct.pack('<2f', *uv) for uv in expected['uvs']),
                        b''.join(struct.pack('<2f', *uv) for uv in actual['uvs']),
                        (stride, tick, 'native vertex UV stores'))


if __name__ == '__main__':
    unittest.main()
