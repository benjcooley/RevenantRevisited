"""Original mapped Ogrok watcher cadence, with no raster or clock substitutions.

The cached startup has no background-update worker. Its published scroll pair
is explicitly set to the output of original SetMapPos, allowing the actual map
AnimateObjects screen rectangle to admit the declared object. This is a viewport
boundary, not an owner/frame/rate write. Full map object loops remain original.
"""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, SOURCE, sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX

MAP = 0x6668d8


def run(boot, output, ticks=240):
    output.mkdir(parents=True, exist_ok=False)
    executable = ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    f = AuthenticCharacters(executable, boot)
    f.select_screen_prefix()
    v, call = f.vm, f.world.call
    position = v.allocate(12)
    v.write(position, struct.pack('<3i', *SOURCE))
    call(0x450d20, (position,), this=MAP)
    call(0x459220, this=MAP, instruction_limit=100000000)
    # Publish precisely SetMapPos's scroll result, as the background updater
    # would. No background raster thread is run in this cadence-only fixture.
    viewport = bytes(v.uc.mem_read(MAP+0x34, 8))
    v.write(MAP+0x88, viewport)
    owners = []
    for name in ('ogrokwatcher1', 'ogrokwatcher2', 'ogrokwatcher3'):
        matches = []
        for i in range(v.u32(0x659d30)):
            record = v.u32(v.u32(0x659d40)+4*i)
            if v.string(v.u32(record)).lower() == name and v.uc.mem_read(record+0x22, 1)[0] == 25:
                matches.append(struct.unpack('<H', v.uc.mem_read(record+0x20, 2))[0])
        assert len(matches) == 1
        definition = v.allocate(0x34)
        v.write(definition, struct.pack('<HH', 25, matches[0]))
        v.write(definition+12, struct.pack('<3i', *SOURCE))
        index = call(0x450e40, (definition, 0xffffffff), this=MAP, instruction_limit=100000000)
        owner = call(0x452690, (index, 0), this=MAP)
        assert owner and v.u32(owner) == 0x5a85ac
        assert call(0x46e8b0, this=owner, instruction_limit=100000000) == 1
        animator = v.u32(owner+0x58)
        assert v.u32(animator) == 0x5a370c
        owners.append(dict(name=name, owner=owner, animator=animator, map_index=index))
    events = []
    def observe(uc, address, *_):
        receiver = uc.reg_read(UC_X86_REG_ECX)
        for i, row in enumerate(owners):
            if receiver in (row['owner'], row['animator']):
                events.append([i, hex(address)])
    for address in (0x4de800, 0x4708e0, 0x40e460, 0x470cc0):
        v.uc.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)
    def state():
        return [dict(frame=struct.unpack('<h', v.uc.mem_read(o['owner']+0x5c, 2))[0],
                     increment=struct.unpack('<h', v.uc.mem_read(o['owner']+0x5e, 2))[0],
                     animator_frame=v.u32(o['animator']+0x14), flags=v.u32(o['owner']+8)) for o in owners]
    def step(next_frame=True):
        events.clear()
        for address, args in ((0x4580f0, ()), (0x458390, ()), (0x458750, (0,))):
            call(address, args, this=MAP, instruction_limit=100000000)
        if next_frame:
            call(0x457ef0, this=MAP, instruction_limit=100000000)
        return dict(state=state(), events=events[:])
    initial = state()
    rows = []
    for tick in range(1, ticks+1):
        row = step()
        for i, s in enumerate(row['state']):
            assert s['frame'] == tick % 4 and s['increment'] == 1, (tick, i, s)
            assert s['animator_frame'] == (tick-1) % 4, (tick, i, s)
            assert [e[1] for e in row['events'] if e[0] == i] == ['0x4de800','0x4708e0','0x40e460','0x470cc0']
        rows.append(dict(tick=tick, **row))
    before = state()
    no_next = [step(False) for _ in range(12)]
    assert all([s['frame'] for s in r['state']] == [s['frame'] for s in before] for r in no_next)
    assert all(not any(e[1] == '0x470cc0' for e in r['events']) for r in no_next)
    # Actual exported global animation pause: both original map consumers read
    # this gate. Restore it afterward; no simulation or frame value is patched.
    old_pause = v.u32(0x6680d8)
    v.put_u32(0x6680d8, 1)
    paused = [step() for _ in range(12)]
    v.put_u32(0x6680d8, old_pause)
    assert all([s['frame'] for s in r['state']] == [s['frame'] for s in before] for r in paused)
    assert all(not any(e[1] in ('0x470cc0','0x40e460') for e in r['events']) for r in paused)
    resumed = step()
    assert all(s['frame'] == (ticks+1)%4 for s in resumed['state'])
    report = dict(status='pass', scope=__doc__, executable_sha256=sha(executable),
                  boot_sha256=sha(boot), source_sha256=sha(__file__), owners=owners,
                  position=list(SOURCE), published_scroll=list(struct.unpack('<2i', viewport)),
                  initial=initial, records=rows, no_next_frame=no_next, animation_paused=paused,
                  resumed=resumed, map_ticks=ticks, owner_next_frame_calls=ticks*3,
                  cycles_per_owner=ticks//4,
                  conclusion='Each exact watcher advances one of four frames per original map tick. No owner frame hold; rate1 is frame increment. At independently documented original24Hz scheduler this is6loops/second.',
                  limits='No wall-clock timer, drawing, background thread, natural map placement or device pacing executed; 24Hz is original scheduler contract, not measured emulator runtime.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({k:report[k] for k in ('status','map_ticks','owner_next_frame_calls','cycles_per_owner','conclusion')}))


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--boot-image', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--ticks', type=int, default=240)
    a=p.parse_args()
    run(a.boot_image, a.output, a.ticks)
