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


def run(boot, output, ticks=64, samples=None):
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
    draws, rows, render_states = [], [], {}
    objects = [vm.u32(vm.u32(animator + 0x54) + 4*i)
               for i in range(vm.u32(animator + 0x44))]
    current_object = [-1]
    key_calls, key_returns = [], {}

    def key_return(uc, address, *_):
        entry = key_returns.pop(address, None)
        if entry is not None:
            pointers = entry.pop('pointers')
            entry['pose'] = [list(struct.unpack('<3f', vm.uc.mem_read(p, 12)))
                             for p in pointers]

    key_return_hooks = {}
    matrix_calls, matrix_returns, matrix_return_hooks = [], {}, {}
    draw_returns, draw_return_hooks = {}, {}

    def draw_return(uc, address, *_):
        entry = draw_returns.pop(address, None)
        if entry is not None:
            count = entry['arguments'][3]
            entry['transformed_hex'] = bytes(vm.uc.mem_read(software.vertices, count*36)).hex()

    def matrix_return(uc, address, *_):
        entry = matrix_returns.pop(address, None)
        if entry is not None:
            obj = entry.pop('pointer')
            entry['pose'] = [list(struct.unpack('<3f', vm.uc.mem_read(obj + off, 12)))
                             for off in (0x10, 0x28, 0x40)]
            entry['matrix'] = list(struct.unpack('<16f', vm.uc.mem_read(obj + 0x58, 64)))

    def matrix_entry(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        obj, state, frame = [vm.u32(sp + 4 + i*4) for i in range(3)]
        address = vm.u32(sp)
        entry = dict(object=objects.index(obj), state=state, frame=frame, pointer=obj)
        assert address not in matrix_returns, 'Nested matrix return observation'
        matrix_calls.append(entry)
        matrix_returns[address] = entry
        if address not in matrix_return_hooks:
            matrix_return_hooks[address] = vm.uc.hook_add(UC_HOOK_CODE, matrix_return,
                begin=address, end=address)

    def key_entry(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = [vm.u32(sp + 4 + i*4) for i in range(6)]
        address = vm.u32(sp)
        entry = dict(object=args[0], state=args[1], frame=args[2], pointers=args[3:])
        assert address not in key_returns, 'Nested identical key-return observation'
        key_calls.append(entry)
        key_returns[address] = entry
        if address not in key_return_hooks:
            key_return_hooks[address] = vm.uc.hook_add(UC_HOOK_CODE, key_return,
                begin=address, end=address)

    def object_entry(uc, *_):
        obj = vm.u32(uc.reg_read(UC_X86_REG_ESP) + 4)
        current_object[0] = objects.index(obj)

    def state_entry(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        render_states[vm.u32(sp + 4)] = vm.u32(sp + 8)

    def observe(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        args = [vm.u32(sp + 4 + i*4) for i in range(7)]
        stride = 32 if args[1] in (0x112, 0x1e2) else 0
        assert stride, ('Unexpected FVF', args)
        entry = dict(object=current_object[0], states=dict(render_states), arguments=args,
            vertices_hex=bytes(vm.uc.mem_read(args[2], args[3]*stride)).hex(),
            indices_hex=bytes(vm.uc.mem_read(args[4], args[5]*2)).hex(),
            matrix=list(struct.unpack('<16f', vm.uc.mem_read(0x675ed0, 64))))
        draws.append(entry)
        address = vm.u32(sp)
        assert address not in draw_returns
        draw_returns[address] = entry
        if address not in draw_return_hooks:
            draw_return_hooks[address] = vm.uc.hook_add(UC_HOOK_CODE, draw_return,
                begin=address, end=address)

    vm.uc.hook_add(UC_HOOK_CODE, observe, begin=0x56eb30, end=0x56eb30)
    vm.uc.hook_add(UC_HOOK_CODE, object_entry, begin=0x40a8f0, end=0x40a8f0)
    vm.uc.hook_add(UC_HOOK_CODE, state_entry, begin=0x56d400, end=0x56d400)
    vm.uc.hook_add(UC_HOOK_CODE, key_entry, begin=0x409430, end=0x409430)
    vm.uc.hook_add(UC_HOOK_CODE, matrix_entry, begin=0x40a420, end=0x40a420)
    tls = world.call(0x58fbf1)
    for tick in range(1, ticks + 1):
        vm.put_u32(0x65cb38, tick)
        if not vm.u32(owner + 8) & 0x1000:
            world.call(0x470cc0, this=owner)
            world.call(0x4de800, this=owner)
        software.clear()
        draws.clear()
        key_calls.clear()
        matrix_calls.clear()
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
        if samples is None or tick in samples:
            save_rgb565_png(image, pixels, 640, 480)
        rows.append(dict(tick=tick,
            frame=struct.unpack('<h', vm.uc.mem_read(owner + 0x5c, 2))[0],
            flags=vm.u32(owner + 8), seed=vm.u32(tls + 0x14),
            draws=list(draws), keys=list(key_calls), matrices=list(matrix_calls),
            previous_state_frame=list(struct.unpack('<2h', vm.uc.mem_read(owner + 0x60, 4))),
            nonblack_pixels=sum(x != (0,) for x in struct.iter_unpack('<H', pixels))))
        if image.is_file():
            rows[-1].update(image=str(image), image_sha256=sha(image))
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
    parser.add_argument('--ticks', type=int, default=64)
    parser.add_argument('--samples', type=int, nargs='+')
    args = parser.parse_args()
    if args.ticks < 1:
        parser.error('ticks must be positive')
    run(args.boot_image, args.output, args.ticks, args.samples)
