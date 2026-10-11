"""Unmodified retail D3DVERTEX transform, normal illumination and raster path.

Unlike SoftwareFixture.draw, callers supply authored XYZ/normal/UV records.
Scene light descriptors and camera are explicit inputs; no vertex colors or
host lighting/projection calculation are inserted into the native branch.
"""
import struct
from software_probe import SoftwareFixture


class LitSoftwareFixture(SoftwareFixture):
    def __init__(self, executable, width=512, height=512, camera=(0, 0, 0)):
        super().__init__(executable, width, height)
        v = self.vm
        self.authored = v.allocate(4096 * 32)
        self.project([(0, 0, 0)], camera=camera)
        # Existing RAM-backed vector capacity, with its actual size established
        # before each call. No allocator or original vertex code is intercepted.
        v.put_u32(0x675fbc, self.vertices + 4096 * 36)
        v.put_u32(0x675fc0, width // 2)
        v.put_u32(0x675fc4, height // 2)
        v.write(0x676055, b'\1')  # Explicit active software scene.
        v.write(0x67604a, b'\0')
        v.call(0x56c730, (65536, 16), this=0x675e90, instruction_limit=5000000)
        # Original startup 56cc32..56cc45 also binds both blend tables to
        # the camera. Scene-selected RGB565 additive kernels read these.
        v.call(0x54df70, (v.u32(0x675e90), v.u32(0x675e94)), this=self.camera)
        v.put_u32(0x670674, v.u32(0x675e90))
        v.put_u32(0x67067c, v.u32(0x675e94))
        self.directional = None

    def lighting(self, ambient_rgb, directional_rgb=None, direction=(0, -.78125, -.625)):
        v = self.vm
        if self.directional is not None:
            v.call(0x56d120, (self.directional,))
            self.directional = None
        v.call(0x56d570, ((ambient_rgb[0] << 16) | (ambient_rgb[1] << 8) | ambient_rgb[2],))
        if directional_rgb is not None:
            self.directional = v.allocate(80)
            record = bytearray(80)
            struct.pack_into('<2I4f', record, 0, 80, 3, *directional_rgb, 1)
            struct.pack_into('<3f', record, 0x24, *direction)
            v.write(self.directional, bytes(record))
            v.call(0x56cf00, (self.directional,))

    def draw_authored(self, vertices, indices, world_matrix, cull=3, z_enabled=True,
                      z_write=False, raster=True, instruction_limit=5000000):
        if not vertices or len(vertices) > 4096 or len(indices) > 16384:
            raise ValueError('Authored fixture capacity exceeded')
        v = self.vm
        v.write(self.authored, b''.join(struct.pack('<8f', *p) for p in vertices))
        v.write(self.indices, struct.pack('<' + 'H' * len(indices), *indices))
        v.put_u32(0x675fb8, self.vertices + len(vertices) * 36)
        matrix = v.allocate(64)
        v.write(matrix, struct.pack('<16f', *world_matrix))
        v.call(0x56d5f0, (1, matrix))
        for key, value in ((7, int(z_enabled)), (14, int(z_write)), (22, cull)):
            v.call(0x56d400, (key, value))
        # D3DFVF_VERTEX=0x112; actual retail dispatch and Illuminate execute.
        v.call(0x56eb30, (4, 0x112, self.authored, len(vertices), self.indices,
                         len(indices) if raster else 0, 0), instruction_limit=instruction_limit)
        transformed = list(struct.iter_unpack('<3f4I2f', v.uc.mem_read(self.vertices, len(vertices) * 36)))
        return v.surface_bytes('screen'), v.surface_bytes('depth'), transformed
