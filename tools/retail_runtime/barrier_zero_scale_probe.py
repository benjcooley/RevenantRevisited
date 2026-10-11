"""Observe original barrier flare matrix rejection without replacing geometry."""
import argparse
import json
import struct
import zipfile
from pathlib import Path

from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EAX
from barrier_wall_native_preflight import NativeBarrierWall, PROFILES, ROOT
from speed_profile_contract import sha


def run(output):
    output.mkdir(parents=True, exist_ok=False)
    cases = []
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        for profile in PROFILES:
            name, type_id, _, _, digest = profile
            asset = archive.read(next(n for n in archive.namelist()
                                      if n.lower() == 'imagery/misc/' + name.lower() + '.i3d'))
            fixture = NativeBarrierWall(ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe',
                                        asset, profile)
            v = fixture.vm
            packets, returns = [], set()
            pending = []
            def observe_object(uc, address, *unused):
                stack = uc.reg_read(UC_X86_REG_ESP)
                obj = v.u32(stack + 4)
                packets.append(dict(object=v.u32(obj + 4), flags=hex(v.u32(obj)),
                                    scale=list(struct.unpack('<3f', uc.mem_read(obj + 0x40, 12)))))
            def matrix_return(uc, address, *unused):
                pending.pop()['matrix_success'] = bool(uc.reg_read(UC_X86_REG_EAX))
            def observe_matrix(uc, address, *unused):
                stack = uc.reg_read(UC_X86_REG_ESP)
                ret = v.u32(stack)
                pending.append(packets[-1])
                if ret not in returns:
                    returns.add(ret)
                    v.uc.hook_add(UC_HOOK_CODE, matrix_return, begin=ret, end=ret)
            v.uc.hook_add(UC_HOOK_CODE, observe_object, begin=0x40a8f0, end=0x40a8f0)
            v.uc.hook_add(UC_HOOK_CODE, observe_matrix, begin=0x40a420, end=0x40a420)
            fixture.raster_enabled = False
            rows = []
            for tick in range(1, 91):
                packets.clear()
                fixture.advance()
                _, record = fixture.render()
                assert not pending
                flares = [p for p in packets if p['object'] in (2, 4)]
                wall = [p for p in packets if p['object'] == 0]
                assert len(wall) == 1 and wall[0]['matrix_success']
                assert all(p['scale'][1] == 0 and not p['matrix_success'] for p in flares)
                assert len(record['draws']) == 1
                rows.append(dict(tick=tick, packets=list(packets),
                                 rng_seed=v.rng_seed,
                                 flare_types=list(struct.unpack('<30i', v.uc.mem_read(fixture.animator + 0x3cc, 120)))))
            assert any(r['packets'][0]['object'] in (2, 4) for r in rows)
            cases.append(dict(name=name, type_id=type_id, asset_sha256=digest, rows=rows))
    report = dict(status='pass_native_zero_scale_rejection', accepted=False, cases=cases,
                  source_sha256=sha(Path(__file__).read_bytes()),
                  scope='Actual custom Animate/RenderObject and actual CalcObjectMatrix returns. '
                        'Active flare slots request scaleY0 and are rejected before software draws; '
                        'wall succeeds. No host matrix/scale replacement. Port and full caller '
                        'acceptance remain unproven; no primary visual credit.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS original EW/NS:90 ticks each, active zero-Y flares rejected, wall retained')
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    run(args.output)
