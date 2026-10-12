"""A supplied initialized VM retains its heap and executes the same raster."""
from pathlib import Path
import struct
import unittest
from runtime import Runtime
from lit_software_probe import LitSoftwareFixture


class ExistingVM(unittest.TestCase):
    def test_initialized_vm_preserves_objects_and_default_raster(self):
        exe = Path(__file__).resolve().parents[2]/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
        vm = Runtime(exe)
        vm.call(0x58ed0d, stop_address=0x58ed8e)
        marker = vm.allocate(32)
        vm.write(marker, b'preserved authentic object bytes')
        original_call = vm.call
        entries = []
        def observed(address, *args, **kwargs):
            entries.append(address)
            return original_call(address, *args, **kwargs)
        vm.call = observed
        shared = LitSoftwareFixture(exe, 64, 64, vm=vm)
        self.assertIs(shared.vm, vm)
        self.assertNotIn(0x58ed0d, entries)
        self.assertEqual(bytes(vm.uc.mem_read(marker, 32)), b'preserved authentic object bytes')
        independent = LitSoftwareFixture(exe, 64, 64)
        vertices = [(x,y,0,0,0,1,u,v) for x,y,u,v in
                    [(-10,-10,0,0),(10,-10,1,0),(10,10,1,1),(-10,10,0,1)]]
        matrix = [1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
        for fmt,texel in [('RGB565',0xfbcf),('ARGB4444',0x8f73)]:
            output = []
            for fixture in (independent,shared):
                fixture.lighting((38,38,38),(1,1,1))
                fixture.set_texture(4,4,struct.pack('<16H',*[texel]*16),format=fmt)
                fixture.clear()
                output.append(fixture.draw_authored(vertices,[0,1,2,0,2,3],matrix,cull=1))
            self.assertEqual(output[0],output[1],fmt)
            self.assertNotEqual(set(output[0][0]),{0},fmt)

if __name__ == '__main__':unittest.main()
