#!/usr/bin/env python3
"""Actual character construction/actions from source-matched original startup.

No character, stat, imagery-state, pose, combat or damage handlers are replaced.
The screen prefix is explicit: original ShowScreen selects the genuine static
PlayScreen and clears its frame counter, stopping before BeginScreen/map startup.
This is actor-dependency evidence, not a map, animation or effect visual pass.
"""
import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / 'slots/hud'))
from hudworld import HudWorld
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX

ROOT = HERE.parents[1]
CHARACTER, PLAYSCREEN = 12, 0x65caf0
SOURCE, TARGET = (3185, 3185, 16), (3281, 3185, 16)


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


class AuthenticCharacters:
    def __init__(self, executable, boot_image):
        if boot_image.name != HudWorld.image_path(executable).name:
            raise ValueError('Boot image does not match current executable/boot sources')
        self.world = HudWorld(executable, image_path=boot_image)
        self.vm = self.world.vm
        self.entered = []
        for address in (0x474bb0, 0x46e1f0, 0x4db000, 0x4c18a0, 0x4da9f0,
                        0x48c780, 0x4d3b90, 0x4d4790, 0x4cefb0, 0x4cee70):
            self.vm.uc.hook_add(UC_HOOK_CODE, self.observe, begin=address, end=address)

    def observe(self, uc, address, size, user):
        self.entered.append(dict(address=hex(address),
                                 receiver=hex(uc.reg_read(UC_X86_REG_ECX))))

    def select_screen_prefix(self):
        self.world.call(0x4909d0, (PLAYSCREEN, 1), stop_address=0x4909f0)
        assert self.vm.u32(0x667fd0) == PLAYSCREEN
        assert self.vm.u32(PLAYSCREEN + 0x48) == 0

    def new_object(self, name, position, kind_required, vtable_required):
        v = self.vm
        for index in range(v.u32(0x659d30)):
            record = v.u32(v.u32(0x659d40) + 4 * index)
            kind = v.uc.mem_read(record + 0x22, 1)[0]
            if kind != kind_required or v.string(v.u32(record)).lower() != name.lower():
                continue
            typ = struct.unpack('<H', v.uc.mem_read(record + 0x20, 2))[0]
            cls = v.u32(0x65a148 + 4 * kind)
            definition = v.allocate(0x34)
            v.write(definition, struct.pack('<HH', kind, typ))
            v.write(definition + 12, struct.pack('<3i', *position))
            actor = self.world.call(0x474bb0, (definition, 0xffffffff, 1),
                                    this=cls, instruction_limit=100000000)
            assert actor and v.u32(actor) == vtable_required
            return actor
        raise KeyError('No exact shipped object type ' + name)

    def new_character(self, name, position):
        return self.new_object(name, position, CHARACTER, 0x5a7848)

    def dump(self, actor):
        v, call = self.vm, self.world.call
        vt, root = v.u32(actor), v.u32(actor + 0xe0)
        return dict(address=hex(actor), vtable=hex(vt),
                    position=list(struct.unpack('<3i', v.uc.mem_read(actor + 0x10, 12))),
                    health=call(v.u32(vt + 0x1c0), this=actor),
                    radius=call(0x4d6e40, this=actor),
                    no_paralyze=v.u32(v.u32(actor + 0xfc) + 0x454),
                    chardata=hex(v.u32(actor + 0xfc)), root=hex(root),
                    action=v.u32(root), action_name=v.string(root + 4),
                    target=hex(v.u32(root + 0x44)), flags=hex(v.u32(actor + 8)))


def run(executable, boot_image, output):
    output.mkdir(parents=True, exist_ok=False)
    negative = AuthenticCharacters(executable, boot_image)
    assert negative.vm.u32(0x667fd0) == 0
    try:
        negative.new_character('Acolyte', SOURCE)
        raise AssertionError('No-screen control unexpectedly returned')
    except RuntimeError as error:
        assert negative.world.last_fault[:2] == (0x4c18ea, 0x48)
        no_screen = dict(error=str(error), entered=negative.entered)
    del negative
    f = AuthenticCharacters(executable, boot_image)
    f.select_screen_prefix()
    a, b = f.new_character('Acolyte', SOURCE), f.new_character('Acolyte', TARGET)
    before = [f.dump(a), f.dump(b)]
    assert all(x['health'] == 102 and x['radius'] == 16 and x['no_paralyze'] == 0 for x in before)
    call = f.world.call
    fighting_result = call(0x4d3b90, (b, 3), this=a)
    fighting = f.dump(a)
    assert fighting['action'] == 3 and fighting['target'] == hex(b)
    disabled_result = call(0x4cefb0, this=b)
    disabled = f.dump(b)
    assert int(disabled['flags'], 16) & 0x40
    stop_result = call(0x4cee70, (0,), this=b)
    stopped = f.dump(b)
    assert stopped['position'] == list(TARGET) and stopped['health'] == 102
    report = dict(status='pass', accepted=False, visual_pass=False,
                  executable_sha256=sha(executable), boot_image_sha256=sha(boot_image),
                  boot_image_name=boot_image.name, actors_before=before,
                  fighting_result=fighting_result, invoker_fighting=fighting,
                  disable_result=disabled_result, disabled=disabled,
                  stop_result=stop_result, stopped=stopped,
                  no_screen_negative=no_screen, entered=f.entered,
                  inherited_startup_boundaries=f.world.boundary_calls,
                  added_character_boundaries=[],
                  scope='Original loaded classes/rules/imagery, CHARACTER NewObject and complete base/complex/ClearChar, actual Health/Radius/BeginFighting/Disable/Stop. Declared two Acolytes at normal fixed coordinates. Original ShowScreen prefix only; BeginScreen/map/character Pulse/visual attachment/damage/natural caller are not certified.')
    (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(report['status'])
    return report


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--executable', type=Path, default=ROOT/'recon/retail_asm/baseline/Revenant.rebuilt.exe')
    p.add_argument('--boot-image', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.executable.resolve(), a.boot_image.resolve(), a.output.resolve())
