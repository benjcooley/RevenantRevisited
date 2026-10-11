"""Loaded original CombatFlash start1 owner/controller/full software rendering."""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, sha
from lit_software_probe import LitSoftwareFixture
from software_probe import save_rgb565_png
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP


def run(boot, output):
    output.mkdir(parents=True, exist_ok=False)
    executable = ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    context = AuthenticCharacters(executable, boot)
    context.select_screen_prefix()
    world, vm = context.world, context.vm
    # The software scene must exist before original imagery/animator creation,
    # as in the qualified Dragon reference. Loading under the booted hardware
    # scene and switching afterward is a distinct, unqualified control.
    software = LitSoftwareFixture(executable, 640, 480, vm=vm)
    software.lighting((38, 38, 38), (1, 1, 1))
    owner = context.new_object('combatflash', (0, 0, 0), 25, 0x5a85ac)
    world.call(0x58c575, (1,))
    assert world.call(0x46e8b0, this=owner, instruction_limit=100000000) == 1
    animator = vm.u32(owner + 0x58)
    assert vm.u32(animator) == 0x5a370c
    initial = dict(owner_state=vm.u32(owner + 0xc) & 0xffff,
                   cached_state=vm.u32(animator + 0xc),
                   controller_count=vm.u32(animator + 0x58))
    entered = dict(owner_state=vm.u32(owner + 0xc) & 0xffff,
                   cached_state=vm.u32(animator + 0xc),
                   controller_count=vm.u32(animator + 0x58))
    original = world.ddraw._device_method
    material = [0]
    for name, args in world.ddraw.device_calls:
        if name == 'IDirect3DDevice3::SetLightState' and args[0] == 1:
            material[0] = args[1]

    def device(interface, method, args):
        if interface == 'IDirect3DDevice3' and method in ('GetLightState', 'SetLightState') and args[1] == 1:
            if method == 'GetLightState':
                vm.put_u32(args[2], material[0])
            else:
                material[0] = args[2]
            return 0
        return original(interface, method, args)

    world.ddraw._device_method = device
    draws, rows = [], []

    def observe(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = [vm.u32(sp + 4 + i*4) for i in range(7)]
        stride = 32 if args[1] in (0x112, 0x1e2) else 0
        assert stride, ('Unexpected FVF', args)
        draws.append(dict(arguments=args,
            vertices_hex=bytes(vm.uc.mem_read(args[2], args[3]*stride)).hex(),
            indices_hex=bytes(vm.uc.mem_read(args[4], args[5]*2)).hex(),
            matrix=list(struct.unpack('<16f', vm.uc.mem_read(0x675ed0, 64)))))

    vm.uc.hook_add(UC_HOOK_CODE, observe, begin=0x56eb30, end=0x56eb30)
    tls = world.call(0x58fbf1)
    for tick in range(1, 65):
        vm.put_u32(0x65cb38, tick)
        if not vm.u32(owner + 8) & 0x1000:
            world.call(0x470cc0, this=owner)
            world.call(0x4de800, this=owner)
        software.clear()
        draws.clear()
        if not vm.u32(owner + 8) & 0x1000:
            world.call(0x40e460, (0,), this=animator)
            world.call(0x416f70, this=0x65a57c)
            world.call(0x40e740, this=animator)
            vm.call(0x54de20, (software.screen.address, software.depth.address), this=software.camera)
            vm.call(0x54dd70, (0, 639, 0, 479, 640), this=software.camera)
            world.call(0x40e8d0, this=animator, instruction_limit=100000000)
            world.call(0x40eb00, this=animator)
            world.call(0x416fb0, this=0x65a57c)
        pixels = vm.surface_bytes('screen')
        image = output / f'native-{tick:03d}.png'
        save_rgb565_png(image, pixels, 640, 480)
        rows.append(dict(tick=tick,
            frame=struct.unpack('<h', vm.uc.mem_read(owner + 0x5c, 2))[0],
            flags=vm.u32(owner + 8), seed=vm.u32(tls + 0x14),
            draws=list(draws), image=str(image), image_sha256=sha(image),
            nonblack_pixels=sum(x != (0,) for x in struct.iter_unpack('<H', pixels))))
        print(tick, rows[-1]['frame'], len(draws), rows[-1]['nonblack_pixels'], flush=True)
    report = dict(status='native_loaded_reference_complete', accepted=False,
        type_id='0xad92bd29', state=0, seed=1, rows=rows,
        constructor_state=initial, animator_state=entered,
        executable_sha256=sha(executable), boot_image_sha256=sha(boot),
        probe_sha256=sha(Path(__file__)),
        scope='Genuine loaded combatflash/class/default animator, nullspell state0/start1; original NextFrame/whole owner Pulse/controller Pulse/Animate/PreRender/Render/PostRender/normal producer/software raster. Explicit24Hz framecounter/isolated640x480 RAM framebuffer/camera0/sourceambient38+dir1, materialhandle COM get/set. No owner Pulse, mesh/texture/color/RNG/raster substitution. Other17states/naturalcombat/audio/device and port comparison remain open.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--boot-image', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.boot_image, args.output)
