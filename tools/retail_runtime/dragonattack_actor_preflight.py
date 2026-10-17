"""Original Dragonattack owner with genuine characters and a named spell.

The declared characters are constructed and Fighting is entered by original
code. They are not inserted into map sectors: the resulting original queries
see empty terrain and no collision/damage targets. This is not a natural
dragon/bone/spell-cast integration claim.
"""
import argparse
import json
import struct
from pathlib import Path
from character_native_preflight import AuthenticCharacters, ROOT, sha
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX

SOURCE, TARGET = (3185, 3185, 16), (3465, 3185, 16)


class DragonAttackActors(AuthenticCharacters):
    def __init__(self, executable, boot_image):
        super().__init__(executable, boot_image)
        self.select_screen_prefix()
        v, call = self.vm, self.world.call
        self.invoker = self.new_character('Acolyte', SOURCE)
        self.target = self.new_character('Acolyte', TARGET)
        call(0x4d3b90, (self.target, 3), this=self.invoker)
        self.actors_before = [self.dump(x) for x in (self.invoker, self.target)]
        assert self.actors_before[0]['target'] == hex(self.target)
        self.initial_invoker_face = v.uc.mem_read(self.invoker + 0x36, 1)[0]
        assert call(0x53ead0, this=0x667c38, instruction_limit=100000000) == 1
        selected = []
        for i in range(v.u32(0x667c3c)):
            sd = v.u32(v.u32(0x667c4c) + 4*i)
            for j in range(v.u32(sd)):
                variant = v.u32(v.u32(sd + 0x10) + 4*j)
                if v.string(variant + 4) == 'Sid Dragon Attack':
                    selected.append((sd, variant))
        assert len(selected) == 1
        targets, source = v.allocate(4), v.allocate(12)
        v.put_u32(targets, self.target)
        v.write(source, struct.pack('<3i', *SOURCE))
        self.spell = call(0x542200, (self.invoker, targets, 1, source,
                                   *selected[0], 0), this=0x670630)
        self.owner = self.new_object('dragonattack', SOURCE, 25, 0x5b16ac)
        v.put_u32(self.owner + 0xd8, self.spell)  # Original inline SetSpell.
        self.tick = -1
        self.owner_entries, self.query_results = [], []
        for address in (0x505a90, 0x45f770, 0x4de800, 0x53f560, 0x4c4950,
                        0x44ceb0, 0x4d4db0):
            v.uc.hook_add(UC_HOOK_CODE, self.observe_owner,
                          begin=address, end=address)
        for address in (0x505c3b, 0x505ded, 0x505e13):
            v.uc.hook_add(UC_HOOK_CODE, self.observe_query,
                          begin=address, end=address)

    def observe_owner(self, uc, address, size, user):
        self.owner_entries.append(dict(tick=self.tick, address=hex(address)))

    def observe_query(self, uc, address, size, user):
        self.query_results.append(dict(tick=self.tick, address=hex(address),
                                       result=uc.reg_read(UC_X86_REG_EAX)))

    def state(self):
        v, e = self.vm, self.owner
        return dict(tick=self.tick, state=v.u32(e + 0xc) & 65535,
                    frame=struct.unpack('<h', v.uc.mem_read(e + 0x5c, 2))[0],
                    position=list(struct.unpack('<3i', v.uc.mem_read(e + 0x10, 12))),
                    velocity=list(struct.unpack('<3i', v.uc.mem_read(e + 0x1c, 12))),
                    phase=v.u32(e + 0x188), counter=v.u32(e + 0x184),
                    facing=v.u32(e + 0x18c), invoker=hex(v.u32(e + 0x190)),
                    damage_pending=v.u32(e + 0x194), flags=v.u32(e + 8))

    def step(self):
        self.tick += 1
        if not self.vm.u32(self.owner + 8) & 0x1000:
            self.world.call(0x470cc0, this=self.owner)
            self.world.call(0x505a90, this=self.owner, instruction_limit=10000000)
        return self.state()


def run(executable, boot_image, output):
    output.mkdir(parents=True, exist_ok=False)
    f = DragonAttackActors(executable, boot_image)
    rows = []
    for tick in range(110):
        rows.append(f.step())
        if tick % 10 == 0:
            print(tick, rows[-1]['phase'], rows[-1]['position'], flush=True)
    transitions = [(r['tick'], r['state']) for i, r in enumerate(rows)
                   if not i or r['state'] != rows[i-1]['state']]
    assert transitions == [(0, 0), (11, 1), (32, 2)]
    assert next(r['tick'] for r in rows if r['flags'] & 4096) == 86
    assert all(q['result'] == 0 for q in f.query_results)
    assert not any(r['address'] in ('0x53f560', '0x4c4950') for r in f.owner_entries)
    actors_after = [f.dump(x) for x in (f.invoker, f.target)]
    assert actors_after == f.actors_before
    for r in rows[86:]:
        assert {k:v for k,v in r.items() if k != 'tick'} == {
            k:v for k,v in rows[86].items() if k != 'tick'}
    report = dict(status='pass_owner_dependency_preflight', accepted=False,
                  visual_pass=False, type_id='0xba09fead',
                  executable_sha256=sha(executable), boot_image_sha256=sha(boot_image),
                  initial_invoker_face=f.initial_invoker_face,
                  actors_before=f.actors_before, actors_after=actors_after,
                  records=rows, entered=f.owner_entries, query_results=f.query_results,
                  inherited_startup_boundaries=f.world.boundary_calls,
                  added_character_or_world_boundaries=[],
                  scope='Original typed Dragonattack factory509d50/ownerVT5b16ac, '
                        'complete Pulse505a90 and generic Effect Pulse, actual Acolyte '
                        'constructors/BeginFighting, original Sid Dragon Attack spell '
                        'creator and inline SetSpell. Declared source/target, original '
                        'screen prefix and empty map query results; no character Pulse, '
                        'map residency, natural dragon/bone attachment, damage, audio '
                        'or renderer/appearance claim.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable', type=Path,
                   default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.executable.resolve(), a.boot_image.resolve(), a.output.resolve())
