#!/usr/bin/env python3
"""Original named Paralize1 owner with genuine constructed fighting characters.

This executes the owner and its character methods, not the effect renderer.
The only explicit binding write is TEffect::SetSpell's inline pointer assignment.
"""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, SOURCE, TARGET, sha
from unicorn import UC_HOOK_CODE


def run(executable, boot_image, output, ticks=280):
    output.mkdir(parents=True, exist_ok=False)
    f = AuthenticCharacters(executable, boot_image)
    f.select_screen_prefix()
    v, call = f.vm, f.world.call
    a, b = f.new_character('Acolyte', SOURCE), f.new_character('Acolyte', TARGET)
    before = [f.dump(a), f.dump(b)]
    call(0x4d3b90, (b, 3), this=a)
    assert f.dump(a)['target'] == hex(b)
    assert call(0x53ead0, this=0x667c38, instruction_limit=100000000) == 1
    selected = []
    for i in range(v.u32(0x667c3c)):
        sd = v.u32(v.u32(0x667c4c) + 4 * i)
        for j in range(v.u32(sd)):
            variant = v.u32(v.u32(sd + 0x10) + 4 * j)
            if v.string(variant + 4) == 'Physical Paralysis':
                selected.append((sd, variant))
    assert len(selected) == 1
    targets, source = v.allocate(4), v.allocate(12)
    v.put_u32(targets, b)
    v.write(source, struct.pack('<3i', *SOURCE))
    spell = call(0x542200, (a, targets, 1, source, *selected[0], 0), this=0x670630)
    effect = f.new_object('paralize1', SOURCE, 25, 0x5adca4)
    v.put_u32(effect + 0xd8, spell)  # Original inline TEffect::SetSpell interface.
    rng_calls = []
    v.uc.hook_add(UC_HOOK_CODE, lambda *args: rng_calls.append(1), begin=0x58c582, end=0x58c582)
    damage_calls = []
    for address in (0x53f560, 0x4c4950):
        v.uc.hook_add(UC_HOOK_CODE, lambda uc, addr, size, user: damage_calls.append(hex(addr)), begin=address, end=address)
    rows = []
    for tick in range(1, ticks + 1):
        if not v.u32(effect + 8) & 0x1000:
            call(0x470cc0, this=effect)
            call(0x4fadb0, this=effect)
        data = bytes(v.uc.mem_read(effect + 0x184, 0x3c4))
        row = dict(tick=tick, flags=v.u32(effect + 8), phase=v.u32(effect + 0x3ac),
                   count=v.u32(effect + 0x444), variant=v.u32(effect + 0x404),
                   target_count=v.u32(effect + 0x544), owner_hex=data.hex(),
                   positions=[list(x) for x in struct.iter_unpack('<3f', data[0x448-0x184:0x4fc-0x184])],
                   scale=struct.unpack_from('<f', data, 0x504-0x184)[0],
                   knot=struct.unpack_from('<f', data, 0x500-0x184)[0],
                   rng_calls=len(rng_calls), target_flags=v.u32(b + 8),
                   target_charflags=v.u32(b + 0x110))
        rows.append(row)
        if tick == 1 or tick % 25 == 0:
            print(tick, row['phase'], row['count'], row['scale'], hex(row['flags']), flush=True)
    assert all(r['variant'] == 1 and r['target_count'] == 1 for r in rows)
    deaths = [r['tick'] for r in rows if r['flags'] & 0x1000]
    assert deaths and deaths[0] > 200 and not damage_calls
    last_live = rows[deaths[0]-1]
    assert all(r['owner_hex'] == last_live['owner_hex'] and r['rng_calls'] == last_live['rng_calls'] for r in rows[deaths[0]:])
    after = [f.dump(a), f.dump(b)]
    assert after[1]['health'] == before[1]['health'] and after[1]['position'] == list(TARGET)
    report = dict(status='pass_owner_dependency_preflight', accepted=False, visual_pass=False,
                  type_id='0xad92bc14', boot_image_sha256=sha(boot_image),
                  executable_sha256=sha(executable), actors_before=before, actors_after=after,
                  kill_tick=deaths[0], damage_calls=damage_calls, records=rows,
                  entered=f.entered, inherited_startup_boundaries=f.world.boundary_calls,
                  added_character_boundaries=[],
                  scope='Genuine loaded CHARACTER Acolytes, original class factory/constructor/stats/actions; original BeginFighting; original74-spell file load and Spell creator; inline SetSpell binding; complete named Physical Paralysis target-bound owner Pulse to kill. Original Disable/Stop/paralysis flags execute; DT_NONE/variant1 makes no damage call. Original ShowScreen prefix only; map/character Pulse and renderer/appearance/attachment/audio/device not certified.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(report['status'], report['kill_tick'])
    return report


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable',type=Path,default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--ticks',type=int,default=280)
    a=p.parse_args();run(a.executable.resolve(),a.boot_image.resolve(),a.output.resolve(),a.ticks)
