#!/usr/bin/env python3
"""Execute complete native constructors/factories before admitting simple VFX."""
import argparse
import json
from pathlib import Path
import re
import struct
import zipfile
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESP
from default_static_probe import original_registry
from static_mesh_probe import ROOT, StaticFixture
from fizzle_probe import sha


PROFILES = (
    ('energyspray', '0xad92bc1c', 'Magic\\Energyspray.I3D',
     'bdca48f08b388c385c78acf6e3b01e35ca5d7969ff910b8a9dd2dbd66f1ac75f',
     0x66cff8, 0x509390, 0x5b0a28,
     ((0x502c50, 0x502d9c), (0x502da0, 0x503105), (0x503110, 0x5034f2)),
     '20 color/angle slots plus25 radial particles; owner phase/actor-hand anchoring and Render RNG'),
    ('Createfood', '0x838cffba', 'Magic\\Createfood.I3D',
     '54bcee48d62c0d16995c26f034425dd1c04b355478f83f3a8a726e0c088c043d',
     0x66cdb8, 0x4f4d00, 0x5a8aa4,
     ((0x4e0040, 0x4e025c), (0x4e0260, 0x4e064e), (0x4e0650, 0x4e0a5a)),
     'Custom particle/food animator with inventory and actor calls; v1 key layout'),
    ('Labback', '0xdcc4011d', 'Misc\\Labback.I3D',
     '1291c57862c12eec8e691585228905a0c1a0257a823704e203ac02df8b914dbc',
     0x5e8508, 0x40dc00, 0x5a370c, (),
     'Generic animator, but partsys/blendcont tags and three408-word animated sphere streams'),
    ('Quick', '0xbabfface', 'Magic\\Quick.i3d',
     '2cf07d02d4839b63de3d38706312c0b0dcc0b96d8282b18262b0480d20242198',
     0x5e8508, 0x40dc00, 0x5a370c, (),
     'Generic animator, but play/partsys/blendcont tags and six animated quad streams'),
)


def asset_summary(data):
    u = lambda p: struct.unpack_from('<I', data, p)[0]
    r = lambda p: p+u(p)
    body = 20+u(16)
    flags, version = u(body), u(body+4)
    if version not in (1, 2, 3):
        raise ValueError('Candidate imagery version changed')
    objects = r(body+48)
    roots = []
    for i in range(u(body+44)):
        obj = objects+48*i
        parent, keys = struct.unpack_from('<2i', data, r(obj+44))
        roots.append(dict(name=data[obj:obj+32].split(b'\0')[0].decode(),
                          parent=parent, key_count=keys,
                          material_and_vertex_run=list(struct.unpack_from('<4H', data, obj+32))))
    # V1 stores old uncompressed keys: do not reinterpret them as scalar words.
    tags = []
    if u(body+52):
        base = r(body+56)
        for i in range(u(body+52)):
            record = base+16*i
            name, value = r(record+8), r(record+12)
            string = lambda p: data[p:data.index(0, p)].decode('latin1')
            tags.append(dict(state=u(record), object=u(record+4), name=string(name), value=string(value)))
    return dict(flags=hex(flags), version=version, vertices=u(body+12), faces=u(body+20),
                materials=u(body+28), textures=u(body+36), objects=u(body+44),
                tags=tags, roots=roots, states=u(24), first_state_frames=struct.unpack_from('<h', data, 70)[0])


def native_dispatch(vm):
    # Recover registration sites using the existing pinned byte scanner, then
    # reset the registry so only complete constructors constitute final evidence.
    scanned = original_registry(vm, [])['registrations']
    vm.put_u32(0x5e872c, 0)
    vm.call(0x406160)  # Full default EFFECT builder constructor, including vtable.
    for row in scanned:
        entry = int(row['call'], 16)-10
        vm.call(entry)
        builder = int(row['builder_address'], 16)
        row.update(complete_constructor=hex(entry), vtable=hex(vm.u32(builder)),
                   factory=hex(vm.u32(vm.u32(builder))))
    if vm.u32(0x5e872c) != 93 or vm.u32(0x5e8508) != 0x5a359c:
        raise AssertionError('Complete93-builder registry changed')
    return scanned


def run(executable, archive, output):
    output.mkdir(parents=True, exist_ok=True)
    fixture = StaticFixture(executable, 512, 512, 5000000)
    vm = fixture.vm
    registrations = native_dispatch(vm)
    calls = []
    # Factories execute original allocation and their entire leaf constructor.
    # Base owner attachment and two array constructors are explicit API bounds;
    # they do not select or overwrite the leaf vtable.
    def external(uc, address, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        calls.append(dict(function=hex(address), args=[vm.u32(sp+4+i*4) for i in range(1 if address==0x445940 else 2)]))
        cleanup = 4 if address==0x445940 else 8
        uc.reg_write(UC_X86_REG_EIP, vm.u32(sp))
        uc.reg_write(UC_X86_REG_ESP, sp+4+cleanup)
    hooks = [vm.uc.hook_add(UC_HOOK_CODE, external, begin=a, end=a) for a in (0x445940, 0x41c7f0)]
    candidates = []
    with zipfile.ZipFile(archive) as z:
        members = {n.lower():n for n in z.namelist()}
        class_bytes = z.read('class.def')
        text = class_bytes.decode('latin1')
        section = re.search(r'CLASS\s+"EFFECT"(.*?)(?=\bCLASS\s+"|\Z)', text, re.S)[1]
        for name, ident, asset, digest, builder, factory, leaf, methods, reason in PROFILES:
            binding = re.findall(r'^\s*"'+re.escape(name)+r'"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)', section, re.M)
            if binding != [(asset, ident)]:
                raise AssertionError('Exact CLASS EFFECT binding changed')
            data = z.read(members[('Imagery/'+asset.replace('\\', '/')).lower()])
            if sha(data) != digest:
                raise AssertionError('Exact shipped candidate asset changed')
            ptr = vm.allocate(len(name)+1)
            vm.write(ptr, name.encode()+b'\0')
            actual = vm.call(0x40dca0, (ptr,))
            if actual != builder or vm.u32(vm.u32(actual)) != factory:
                raise AssertionError('Complete constructor selects unexpected factory')
            owner = vm.allocate(0x200)
            calls.clear()
            animator = vm.call(factory, (owner,), this=builder)
            if vm.u32(animator) != leaf or len(calls) != 3 or calls[0]['args'] != [owner]:
                raise AssertionError('Actual factory leaf/base boundary changed')
            entries = [vm.u32(leaf+i) for i in (0x18, 0x2c, 0x34)]
            if methods and entries != [a for a, _ in methods]:
                raise AssertionError('Actual leaf Init/Animate/Render changed')
            candidates.append(dict(name=name, type_id=ident, asset=asset, asset_sha256=digest,
                topology=asset_summary(data), builder=hex(actual), builder_vtable=hex(vm.u32(actual)),
                factory=hex(factory), animator_vtable=hex(vm.u32(animator)),
                methods=[hex(a) for a in entries], method_spans=[dict(start=hex(a), end=hex(b), bytes=b-a,
                    sha256=sha(bytes(vm.uc.mem_read(a, b-a)))) for a,b in methods],
                factory_api_calls=list(calls), decision='defer', reason=reason))
    for h in hooks:
        vm.uc.hook_del(h)
    report = dict(status='pass', decision='no_new_easy_static_admission', candidates=candidates,
        retail_sha256=sha(vm.image), class_def_sha256=sha(class_bytes),
        probe_sha256=sha(Path(__file__).read_bytes()), complete_named_constructors=registrations,
        default_constructor='0x406160', registry_count=93, factory_allocations='original0x482fb0',
        api_boundaries=['base owner attachment0x445940', 'array constructors0x41c7f0'],
        original_render_executed=False, port_comparison_executed=False, visible_pixel_credit=False,
        full_acceptance_granted=False, dosbox_used=False,
        scope='Exact shipped CLASS EFFECT/asset and full original static registration plus actual allocation/leaf factory admission only. Init/Animate/Render bodies not executed; no visual or frontend-pass credit.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    return report


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('executable', type=Path)
    p.add_argument('--archive', type=Path, default=ROOT/'data/imagery.rvi')
    p.add_argument('--output', type=Path, required=True)
    args = p.parse_args()
    result = run(args.executable, args.archive, args.output)
    print(json.dumps(dict(status=result['status'], decision=result['decision'], registry_count=93,
        candidates=[dict(name=c['name'], factory=c['factory'], decision=c['decision'], reason=c['reason']) for c in result['candidates']]), indent=2))


if __name__ == '__main__':
    main()
