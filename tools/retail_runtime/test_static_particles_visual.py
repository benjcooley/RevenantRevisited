"""Complete native particle caller: actual FVF, scene policy and prelit color."""
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP
from pathlib import Path
import struct
import unittest
import zipfile

from static_particles_visual_reference import NativeParticleScene, ROOT, registry, punctuation
from static_particles_profile_contract import Y_PROFILES


class NativeParticleVisualTests(unittest.TestCase):
    def test_original_stride_three_pulses_read_cached_emitter_matrices(self):
        profile = next(p for p in Y_PROFILES if p['name'] == 'nullifier')
        with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
            data = archive.read(next(n for n in archive.namelist()
                                if n.lower() == ('imagery/' + profile['asset']).lower()))
        scene = NativeParticleScene(ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe',
                                    data, profile['name'], 64, 64)
        registry(scene)
        punctuation(scene)
        scene.initialize(0)
        scene.state_trace(0, frame_offset=1, move_tick=None)
        vm = scene.vm
        reads = []
        cached_frame = 0
        expected = {}

        def remember():
            return {i: bytes(vm.uc.mem_read(obj + 0x58, 64))
                    for i, obj in enumerate(scene.animobjs) if i != scene.prototype_index}

        def observe(uc, address, size, user):
            index = vm.u32(uc.reg_read(UC_X86_REG_ESP) + 4)
            self.assertEqual(vm.u32(scene.animator + 0x14), cached_frame)
            self.assertEqual(bytes(vm.uc.mem_read(scene.animobjs[index] + 0x58, 64)),
                             expected[index])
            reads.append((tick, cached_frame, index))

        vm.uc.hook_add(UC_HOOK_CODE, observe, begin=0x40efc0, end=0x40efc0)
        expected = remember()
        initial = expected
        for tick in range(18):
            vm.call(0x403760, this=scene.controller, instruction_limit=5000000)
            self.assertEqual(remember(), expected)
            if (tick + 1) % 3 == 0:
                cached_frame = (tick + 1) % scene.asset['state0_frames']
                vm.put_u32(scene.animator + 0x14, cached_frame)
                for index, obj in enumerate(scene.animobjs):
                    if index == scene.prototype_index or index in scene.unused_rejected_pose_indices:
                        continue
                    if scene.asset['objects'][index]['states'][0]['keys']:
                        self.assertEqual(vm.call(0x40a420, (obj, 0, cached_frame, 0, 0),
                                                 this=scene.imagery), 1)
                expected = remember()
                # Complete native render caller executes only every third Pulse.
                scene.raster_enabled = False
                vm.call(0x404270, this=scene.controller, instruction_limit=5000000)
        self.assertGreater(len(reads), 20)
        self.assertTrue(any(tick % 3 != 0 for tick, _, _ in reads))
        self.assertTrue(all(frame == (tick // 3) * 3 for tick, frame, _ in reads))
        self.assertNotEqual(initial, expected)  # animated emitter negative control

    def test_full_caller_replay_and_prelit_light_independence(self):
        profile = Y_PROFILES[0]
        with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
            member = next(n for n in archive.namelist()
                          if n.lower() == ('imagery/' + profile['asset']).lower())
            data = archive.read(member)
        results = []
        for ambient, directional, incoming_cull in ((38, 1, 1), (0, 0, 3), (255, 1, 2)):
            with self.subTest(ambient=ambient, incoming_cull=incoming_cull):
                scene = NativeParticleScene(ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe',
                                            data, profile['name'], 512, 512)
                registry(scene)
                punctuation(scene)
                initial = scene.initialize(0)
                self.assertEqual(initial['resource_boundaries'], {})  # actual CopyVertices executes
                scene.state_trace(0, frame_offset=1, move_tick=None)
                scene.software.lighting((ambient,) * 3, (directional,) * 3)
                vm = scene.vm
                prototype = scene.animobjs[scene.prototype_index]
                self.assertEqual(vm.u32(prototype + 0x9c), 0x1e2)
                self.assertEqual(vm.u32(prototype) & 0xe000, 0xe000)
                vm.call(0x56d400, (22, incoming_cull))
                for tick in range(6):
                    vm.call(0x403760, this=scene.controller, instruction_limit=5000000)
                    vm.put_u32(scene.animator + 0x14, tick + 1)
                    emitter = scene.animobjs[1]
                    self.assertEqual(vm.call(0x40a420, (emitter, 0, tick + 1, 0, 0),
                                             this=scene.imagery), 1)
                    scene.software.clear()
                    scene.draws = []
                    scene.raster_enabled = tick == 5
                    with vm.bulk_writes([
                        (scene.software.screen.address, 512 * 512 * 2),
                        (scene.software.depth.address, 512 * 512 * 2),
                        (vm.STACK + vm.STACK_SIZE - 0x10000, 0x10000),
                    ]):
                        vm.call(0x404270, this=scene.controller, instruction_limit=100000000)
                    self.assertTrue(vm.u32(prototype) & 1)  # full controller re-hides prototype
                self.assertEqual(len(scene.draws), 9)
                for draw in scene.draws:
                    self.assertEqual(draw['arguments'][0:2], [4, 0x1e2])
                    self.assertEqual(draw['states'][7], 1)
                    self.assertEqual(draw['states'][14], 0)
                    self.assertEqual(draw['states'][22], 1)
                    self.assertEqual(draw['states'][19], 2)
                    self.assertEqual(draw['states'][20], 2)
                pixels = vm.surface_bytes('screen')
                self.assertGreater(sum(p != (0,) for p in struct.iter_unpack('<H', pixels)), 0)
                self.assertFalse(scene.software.locked)
                results.append((pixels, vm.surface_bytes('depth'), vm.rng_seed))
        # No material-white or normal-light carrier is supplied. Original
        # LVERTEX colors survive full lighting/cull-state negative controls.
        self.assertEqual(results[0], results[1])
        self.assertEqual(results[0], results[2])


if __name__ == '__main__':
    unittest.main()
