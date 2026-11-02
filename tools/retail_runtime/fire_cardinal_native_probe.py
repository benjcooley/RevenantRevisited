"""Original Fire default renderer at four declared cardinal construction facings.

This is a public object-definition control, not a natural caller or animation
claim. Only SObjectState.rotatez changes; the original factory, loaded asset,
owner, animator, frame selection, lighting and software raster remain intact.
"""
import argparse
import json
import struct
from pathlib import Path

from fire_authentic_preflight import FireScene, ROOT, sha
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP


class CardinalFire(FireScene):
    def __init__(self, executable, boot_image, facing):
        self.facing = facing
        super().__init__(executable, boot_image)

    def new_object(self, *args, **kwargs):
        # AuthenticCharacters constructs a real SObjectState and calls the actual
        # loaded class factory. Supply its declared rotatez before that call
        # consumes the definition; do not mutate an initialized animator.
        def configure(uc, address, size, user):
            definition = self.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 4)
            before = bytes(uc.mem_read(definition, 0x34))
            assert struct.unpack_from('<H', before)[0] == 25
            uc.mem_write(definition + 0x32, bytes([self.facing]))
            after = bytes(uc.mem_read(definition, 0x34))
            assert before[:0x32] == after[:0x32] and before[0x33:] == after[0x33:]
            self.definition = dict(before=before.hex(), after=after.hex())
        hook = self.vm.uc.hook_add(UC_HOOK_CODE, configure,
                                  begin=0x474bb0, end=0x474bb0)
        try:
            return super().new_object(*args, **kwargs)
        finally:
            self.vm.uc.hook_del(hook)


def run(executable, boot_image, output):
    output.mkdir(parents=True, exist_ok=False)
    records = []
    for facing in (0, 64, 128, 192):
        f = CardinalFire(executable, boot_image, facing)
        v = f.vm
        assert v.uc.mem_read(f.owner + 0x36, 1)[0] == facing
        for tick in range(3):
            f.world.call(0x470cc0, this=f.owner)
            f.world.call(0x4de800, this=f.owner)
            v.put_u32(0x65cb38, tick)
            pixels = f.render()
            state = f.state()
            assert state['frame'] == state['animator_frame'] == state['texture_frame'] == 0
            assert state['rng_draws'] == 0 and len(f.draws) == 1
            nonzero = [dict(x=i % f.width, y=i // f.width, rgb565=value[0])
                       for i, value in enumerate(struct.iter_unpack('<H', pixels)) if value[0]]
            assert len(nonzero) == (1 if facing in (64, 192) else 0)
            name = f'face-{facing:03}-tick-{tick:02}.png'
            save_rgb565_png(output / name, pixels, f.width, f.height)
            records.append(dict(facing=facing, tick=tick, definition=f.definition,
                                state=state, draws=f.draws, bounds=f.draw_bounds(),
                                nonzero=nonzero, image=name, sha256=sha(output / name)))
            print(facing, tick, nonzero, flush=True)
        del f
    result = dict(status='cardinal_control_complete', accepted=False,
                  visual_pass=False, effect='Fire', type_id='0x119f01e3',
                  records=records, viewport=[1280, 960], center=[640, 480],
                  source=[3185, 3185, 16], camera=[3185, 3185, 0],
                  ambient=[38, 38, 38], directional=[1, 1, 1],
                  executable_sha256=sha(executable), boot_image_sha256=sha(boot_image),
                  probe_sha256=sha(__file__),
                  dependencies={name: sha(Path(__file__).with_name(name)) for name in
                                ('fire_authentic_preflight.py', 'dragonattack_visual_reference.py',
                                 'character_native_preflight.py', 'lit_software_probe.py')},
                  scope=__doc__, conclusion='Facing64/192 produces one stationary original '
                  'software pixel; facing0/128 is blank. No texture timer, geometry or '
                  'brightness modification, natural placement, or animated-flame credit.')
    (output / 'manifest.json').write_text(json.dumps(result, indent=2) + '\n')
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path,
                        default=ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    parser.add_argument('--boot-image', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.executable, args.boot_image, args.output)
