"""Full original Dragonattack owner/default renderer in its genuine actor VM.

The declared RAM target is bound after original PreRender, which otherwise
rebinds the real HUD display. No I3D resource, animator, vertex or texture data
is synthesized. Non-sampled ticks may omit only the final software draw call;
all original owner/controller/Animate/PreRender/Render/PostRender calls run.
"""
import argparse
import json
import struct
from pathlib import Path
from dragonattack_actor_preflight import DragonAttackActors, ROOT, sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from headfireball_visual_reference import HeadFireBallScene
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP


class DragonAttackScene(DragonAttackActors):
    controller_state = HeadFireBallScene.controller_state
    observe_pulse = HeadFireBallScene.observe_pulse

    def __init__(self, executable, boot_image, width=1280, height=960,
                 center=(640, 480)):
        super().__init__(executable, boot_image)
        self.width, self.height, self.center = width, height, center
        v, call = self.vm, self.world.call
        self.software = LitSoftwareFixture(executable, width, height,
                                          camera=(3185, 3185, 0), vm=v)
        self.software.lighting((38, 38, 38), (1, 1, 1))
        original_device = self.world.ddraw._device_method
        self.material_state = 0
        self.material_calls = []

        def device(interface, method, args):
            if method in ('GetLightState', 'SetLightState'):
                assert args[1] == 1
                if method == 'GetLightState':
                    v.put_u32(args[2], self.material_state)
                else:
                    self.material_state = args[2]
                self.material_calls.append((method, self.material_state))
                return 0
            return original_device(interface, method, args)

        self.world.ddraw._device_method = device
        call(0x58c575, (1,))  # Original srand, before real animator creation.
        self.thread_data = call(0x58fbf1)
        self.rng_draws = 0
        v.uc.hook_add(UC_HOOK_CODE, self.observe_rng, begin=0x58c582, end=0x58c582)
        assert call(0x46e8b0, this=self.owner) == 1
        self.animator = v.u32(self.owner + 0x58)
        self.imagery = v.u32(self.owner + 0x54)
        assert v.u32(self.animator) == 0x5a370c
        assert v.u32(self.animator + 0x44) == 5
        self.animobjs = [v.u32(v.u32(self.animator + 0x54) + 4*i) for i in range(5)]
        self.pulse_inputs, self.render_states = {}, {}
        self.draws, self.points = [], []
        self.current_object, self.raster_enabled = -1, True
        for address, observer in ((0x403760, self.observe_pulse),
                                  (0x40a8f0, self.observe_object),
                                  (0x56eb30, self.observe_draw),
                                  (0x56d400, self.observe_render_state)):
            v.uc.hook_add(UC_HOOK_CODE, observer, begin=address, end=address)

    def observe_rng(self, *args):
        self.rng_draws += 1

    def observe_render_state(self, uc, *args):
        sp = uc.reg_read(UC_X86_REG_ESP)
        self.render_states[self.vm.u32(sp + 4)] = self.vm.u32(sp + 8)

    def observe_object(self, uc, *args):
        obj = self.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 4)
        self.current_object = self.animobjs.index(obj)

    def observe_draw(self, uc, *unused):
        v, sp = self.vm, uc.reg_read(UC_X86_REG_ESP)
        args = [v.u32(sp + 4 + i*4) for i in range(7)]
        assert args[1] in (0x112, 0x1e2)
        vertices = bytes(v.uc.mem_read(args[2], args[3]*32))
        indices = bytes(v.uc.mem_read(args[4], args[5]*2))
        matrices = [list(struct.unpack('<16f', v.uc.mem_read(p, 64)))
                    for p in (0x675ed0, 0x676000, 0x675f70)]
        for index, in struct.iter_unpack('<H', indices):
            point = [*struct.unpack_from('<3f', vertices, index*32), 1.0]
            for matrix in matrices:
                point = [sum(point[k]*matrix[i+4*k] for k in range(4)) for i in range(4)]
            self.points.append((self.center[0]+point[0], self.center[1]-point[1]))
        self.draws.append(dict(object=self.current_object, arguments=args,
                               states=dict(self.render_states), vertices_hex=vertices.hex(),
                               indices_hex=indices.hex(), matrix=matrices[0]))
        if not self.raster_enabled:
            uc.reg_write(UC_X86_REG_EAX, 0)
            uc.reg_write(UC_X86_REG_EIP, v.u32(sp))
            uc.reg_write(UC_X86_REG_ESP, sp + 4 + 28)

    def state(self):
        result = super().state()
        if not hasattr(self, 'animator'):
            return result
        v, a = self.vm, self.animator
        result.update(cached_state=v.u32(a + 0xc), cached_frame=v.u32(a + 0x14),
                      seed=v.u32(self.thread_data + 0x14), rng_draws=self.rng_draws,
                      controllers=self.controller_state(), pulse_inputs=dict(self.pulse_inputs),
                      object_matrices=[list(struct.unpack('<16f', v.uc.mem_read(p+0x58, 64)))
                                       for p in self.animobjs])
        return result

    def render(self):
        self.draws, self.points = [], []
        s, v, call = self.software, self.vm, self.world.call
        s.clear()
        if not v.u32(self.owner + 8) & 4096:
            call(0x40e460, (0,), this=self.animator)
            call(0x416f70, this=0x65a57c)
            call(0x40e740, this=self.animator)
            # Both BeginScene and PreRender rebind the HUD target. Keep the
            # actual scene lifecycle, then select only this declared RAM target.
            call(0x54de20, (s.screen.address, s.depth.address), this=s.camera)
            call(0x54dd70, (0, self.width-1, 0, self.height-1, self.width), this=s.camera)
            v.put_u32(0x675fc0, self.center[0])
            v.put_u32(0x675fc4, self.center[1])
            call(0x40e8d0, this=self.animator, instruction_limit=100000000)
            call(0x40eb00, this=self.animator)
            call(0x416fb0, this=0x65a57c)
        return v.surface_bytes('screen')

    def draw_bounds(self):
        return ([min(p[0] for p in self.points), min(p[1] for p in self.points),
                 max(p[0] for p in self.points), max(p[1] for p in self.points)]
                if self.points else None)


def run(executable, boot_image, output, samples, frames=110, state_only=False):
    output.mkdir(parents=True, exist_ok=False)
    f = DragonAttackScene(executable, boot_image)
    rows, initial = [], f.state()
    for tick in range(frames):
        f.step()
        before = f.state()
        f.raster_enabled = not state_only and tick in samples
        pixels = f.render()
        if f.raster_enabled:
            save_rgb565_png(output/f'native-{tick:03}.png', pixels, f.width, f.height)
        rows.append(dict(tick=tick, before=before, after=f.state(),
                         draw_bounds=f.draw_bounds(), draws=f.draws if f.raster_enabled else []))
        print(tick, before['state'], before['frame'], before['position'],
              len(f.draws), f.rng_draws, flush=True)
    result = dict(effect='dragonattack', type_id='0xba09fead', initial=initial,
                  records=rows, actors_before=f.actors_before,
                  actors_after=[f.dump(x) for x in (f.invoker, f.target)],
                  query_results=f.query_results, owner_entries=f.owner_entries,
                  material_calls=f.material_calls, viewport=[f.width, f.height],
                  center=list(f.center), camera=[3185,3185,0], samples=samples,
                  executable_sha256=sha(executable), boot_image_sha256=sha(boot_image),
                  scope='Genuine original loaded actors, named Sid Dragon Attack spell '
                        'and typed owner/default animator/I3D resource/texture COM. '
                        'Complete owner Pulse and Animate/PreRender/Render/PostRender '
                        'with original Begin/EndScene; declared RAM target, camera and '
                        'ambient38/dir1. Non-sampled raster dispatch may be omitted; '
                        'all renderer-side controller/RNG calls execute. Original srand1 '
                        'before animator creation, unmodified original rand. Empty map '
                        'queries, nonresident actors, natural dragon/bone/characterPulse/'
                        'damage/audio/device/exactpixel boundaries remain open.')
    (output/'trace.json').write_text(json.dumps(result, indent=2)+'\n')
    return result


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable', type=Path, default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--frames', type=int, default=110)
    p.add_argument('--samples', type=int, nargs='+', default=list(range(1,98,4)))
    p.add_argument('--state-only', action='store_true')
    a = p.parse_args()
    run(a.executable, a.boot_image, a.output, a.samples, a.frames, a.state_only)
