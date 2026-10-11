"""Genuine loaded Drip owner/animator through native Ripple birth requests."""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX


def run(boot, output):
    output.mkdir(parents=True, exist_ok=False)
    executable = ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    actors = AuthenticCharacters(executable, boot)
    actors.select_screen_prefix()
    world, vm = actors.world, actors.vm
    owner = actors.new_object('Drip', (0, 0, 0), 25, 0x5ac630)
    vt = vm.u32(owner)
    world.call(vm.u32(vt + 0x200), (20, 128, 1), this=owner)
    assert world.call(0x46e8b0, this=owner, instruction_limit=100000000) == 1
    animator = vm.u32(owner + 0x58)
    assert vm.u32(animator) == 0x5ac83c
    tick, requests, rows = 0, [], []
    map_requests, pending, return_hooks = [], {}, {}

    def observe(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        requests.append(dict(tick=tick,
            arguments=list(struct.unpack('<4i', vm.uc.mem_read(sp + 4, 16)))))

    vm.uc.hook_add(UC_HOOK_CODE, observe, begin=0x4f1140, end=0x4f1140)

    def map_return(uc, address, *_):
        if address in pending:
            map_requests[pending.pop(address)]['result'] = uc.reg_read(UC_X86_REG_EAX)

    def map_entry(uc, *_):
        sp = uc.reg_read(UC_X86_REG_ESP)
        definition, return_address = vm.u32(sp + 4), vm.u32(sp)
        map_requests.append(dict(tick=tick,
            definition_hex=bytes(vm.uc.mem_read(definition, 52)).hex()))
        assert return_address not in pending, 'Recursive map factory observation'
        pending[return_address] = len(map_requests)-1
        if return_address not in return_hooks:
            return_hooks[return_address] = vm.uc.hook_add(UC_HOOK_CODE, map_return,
                begin=return_address, end=return_address)

    vm.uc.hook_add(UC_HOOK_CODE, map_entry, begin=0x450e40, end=0x450e40)
    error = None
    try:
        for tick in range(1, 97):
            world.call(0x470cc0, this=owner)
            world.call(0x4f11e0, this=animator)
            rows.append(dict(tick=tick, dead=bool(vm.u32(animator + 0x114)),
                time=vm.u32(animator + 0x118),
                values=list(struct.unpack('<6f', vm.uc.mem_read(animator + 0xfc, 24))),
                owner_flags=vm.u32(owner + 8)))
    except Exception as failure:
        error = str(failure)
    report = dict(status='native_loaded_lifecycle_complete' if error is None else 'native_boundary_failure',
        accepted=False, type_id='0x5975abde', executable_sha256=sha(executable),
        boot_image_sha256=sha(boot), probe_sha256=sha(Path(__file__)),
        parameters=[20, 128, 1], rows=rows, ripple_requests=requests,
        map_factory_requests=map_requests, error=error,
        scope='Actual cached startup/class/resource loading, typed Drip owner public SetParams/CreateAnimator, NextFrame and complete custom animator Pulse4f11e0 (vtable slot0x2c). Ripple birth and sound execute original instructions; request observation is read-only. Full owner Pulse and default Animate/transform remain outside this state probe. No map sectors loaded, no invented child residency, raster, port or visual acceptance claim.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(report['status'], len(rows), requests, error, flush=True)
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--boot-image', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.boot_image, args.output)
