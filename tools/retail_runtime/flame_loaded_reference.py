"""Complete loaded base Flame owner lifecycle and original software rendering.

The original class factory, imagery decoder, custom animator, owner Pulse and
all render phases execute. Only the output buffers, camera and scene lighting
are selected fixture inputs. This is not a natural-map or wall-clock proof.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from character_native_preflight import AuthenticCharacters, ROOT, sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from tornado_target_visual_reference import TornadoTargetScene
from unicorn import UC_HOOK_CODE


class LoadedFlame:
    def __init__(self, executable, boot):
        actors = AuthenticCharacters(executable, boot)
        actors.select_screen_prefix()
        self.world, self.vm = actors.world, actors.vm
        v, call = self.vm, self.world.call
        self.owner = actors.new_object('Flame', (0, 0, 0), 25, 0x5a85ac)
        assert call(0x46e8b0, this=self.owner, instruction_limit=100000000) == 1
        imagery = v.u32(self.owner + 0x54)
        assert v.string(v.u32(imagery + 4)).replace('/', '\\').lower() == 'magic\\flame.i3d'
        self.animator = v.u32(self.owner + 0x58)
        assert v.u32(self.animator) == 0x5a9dec
        assert v.u32(0x5a9dec + 0x2c) == 0x4e4ef0  # Pulse, not Animate.
        assert v.u32(0x5a9dec + 0x30) == 0x40e460
        self.material = 0
        for method, args in self.world.ddraw.device_calls:
            if method == 'IDirect3DDevice3::SetLightState' and args[0] == 1:
                self.material = args[1]
        original = self.world.ddraw._device_method

        def device(interface, method, args):
            if interface == 'IDirect3DDevice3' and method in ('GetLightState', 'SetLightState') and args[1] == 1:
                if method == 'GetLightState':
                    v.put_u32(args[2], self.material)
                else:
                    self.material = args[2]
                return 0
            return original(interface, method, args)

        self.world.ddraw._device_method = device
        self.software = LitSoftwareFixture(executable, 640, 480, camera=(0, 0, 0), vm=v)
        self.software.lighting((38, 38, 38), (1, 1, 1))
        self.draws, self.states, self.current_object = [], {}, -1
        self.pulses = 0
        v.uc.hook_add(UC_HOOK_CODE, self.observe_pulse, begin=0x4e4ef0, end=0x4e4ef0)
        for address, method in ((0x56eb30, self.observe_draw), (0x56d400, self.observe_state), (0x40a8f0, self.observe_object)):
            v.uc.hook_add(UC_HOOK_CODE, method, begin=address, end=address)
        call(0x40e460, (0,), this=self.animator)

    observe_draw = TornadoTargetScene.observe_draw
    observe_state = TornadoTargetScene.observe_state
    observe_object = TornadoTargetScene.observe_object

    def observe_pulse(self, *_):
        self.pulses += 1

    def step(self, tick):
        v, call = self.vm, self.world.call
        before = self.pulses
        v.put_u32(0x65cb38, tick)
        call(0x470cc0, this=self.owner)
        call(0x4de800, this=self.owner)
        assert self.pulses == before + 1
        call(0x40e460, (0,), this=self.animator)
        assert v.u32(self.animator + 0xfc) == tick % 18

    def render(self):
        self.draws = []
        v, s, call = self.vm, self.software, self.world.call
        s.clear()
        call(0x416f70, this=0x65a57c)
        call(0x40e740, this=self.animator)
        # PreRender rebinds the real display. Redirect only the declared
        # output memory, without replacing a projection or raster function.
        call(0x54de20, (s.screen.address, s.depth.address), this=s.camera)
        call(0x54dd70, (0, 639, 0, 479, 640), this=s.camera)
        call(0x417d60, (1, 1), this=0x65a57c)
        before = self.pulses
        call(0x4e4f20, this=self.animator, instruction_limit=100000000)
        assert self.pulses == before, 'Rendering must not advance the custom counter'
        call(0x40eb00, this=self.animator)
        call(0x416fb0, this=0x65a57c)
        return v.surface_bytes('screen')


def run(executable, boot, output, ticks=36):
    if ticks < 36:
        raise ValueError('At least two complete 18-tick cycles are required')
    output.mkdir(parents=True, exist_ok=False)
    f = LoadedFlame(executable, boot)
    rows = []
    for tick in range(ticks + 1):
        if tick:
            f.step(tick)
        raw = f.render()
        assert len(f.draws) == 1 and f.draws[0]['fvf'] == 0x1e2
        path = output / f'native-{tick:03d}.png'
        save_rgb565_png(path, raw, 640, 480)
        rows.append(dict(tick=tick, internal_frame=f.vm.u32(f.animator + 0xfc),
            native_custom_pulses=f.pulses, draws=f.draws, image=str(path), image_sha256=sha(path),
            rgb565_sha256=hashlib.sha256(raw).hexdigest(),
            pixels=sum(p != (0,) for p in struct.iter_unpack('<H', raw))))
    hashes = [r['rgb565_sha256'] for r in rows]
    assert hashes[:18] == hashes[18:36] and hashes[0] == hashes[36]
    assert len(set(hashes)) == 8 and all(r['pixels'] > 0 for r in rows)
    report = dict(status='complete_loaded_reference', accepted=False, type_id='0x50ba373b',
        source_sha256=sha(__file__), executable_sha256=sha(executable), boot_sha256=sha(boot),
        viewport=[640, 480], owner=[0, 0, 0], camera=[0, 0, 0], ambient=[38]*3, directional=[1]*3,
        frames=rows, scope=__doc__, cadence='Explicit single lifecycle step per tick; natural timer pacing remains separate')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(report['status'], len(rows), 'frames, 8 atlas images, two identical cycles', flush=True)
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--boot-image', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--ticks', type=int, default=36)
    a = p.parse_args()
    run(ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe', a.boot_image, a.output, a.ticks)
