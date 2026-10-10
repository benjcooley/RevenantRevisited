"""Read-only exact retail association diagnostic; no rendered acceptance."""
import argparse
import hashlib
import json
import re
import struct
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/retail_asm'))
from reconstruct import pe_layout
from function_hook import file_offset

RETAIL_SHA = '28bec27387bf53a553da320dd4883d00ff5bcc5a5f3d2658cca8e8ed586372b5'
ASSET_SHA = 'f697b802a7b0edb540c3b4f23ffd864b9b5e1253979af65a1a51c95ff70f30c5'


def inspect(executable):
    image = Path(executable).read_bytes()
    digest = hashlib.sha256(image).hexdigest()
    if digest != RETAIL_SHA:
        raise ValueError('Exact unchanged retail executable required')
    layout = pe_layout(image)
    def read(address, size):
        offset = file_offset(layout, address)
        return image[offset:offset + size]
    def u32(address):
        return struct.unpack('<I', read(address, 4))[0]
    registrations = []
    for name, address, string, builder, vtable in (
        ('IrisFlare', 0x4e5fa0, 0x5e110c, 0x5a9f28, 0x5a9f2c),
        ('Teleporter', 0x4e6a40, 0x5e1124, 0x5aa3fc, 0x5aa400)):
        code = read(address, 26)
        if code[0] != 0x68 or struct.unpack_from('<I', code, 1)[0] != string:
            raise ValueError('Registration name changed')
        if struct.unpack_from('<I', code, 21)[0] != builder:
            raise ValueError('Registration builder changed')
        if read(string, len(name) + 1) != name.encode() + b'\0':
            raise ValueError('Registration string changed')
        registrations.append(dict(name=name, registration=hex(address),
            registration_sha256=hashlib.sha256(code).hexdigest(), builder=hex(builder),
            factory=hex(u32(builder)), vtable=hex(vtable),
            initialize=hex(u32(vtable + 24)), animate=hex(u32(vtable + 44)),
            render=hex(u32(vtable + 52))))
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        asset = archive.read('Imagery/Misc/irisflare.i3d')
        registry = archive.read('class.def')
    if hashlib.sha256(asset).hexdigest() != ASSET_SHA:
        raise ValueError('Exact shipped IrisFlare asset required')
    body = 20 + struct.unpack_from('<I', asset, 16)[0]
    flags, numverts = struct.unpack_from('<2I', asset, body)
    numfaces = struct.unpack_from('<I', asset, body + 264)[0]
    if flags != 4 or numverts != 186 or numfaces <= 0:
        raise ValueError('Legacy header changed; do not parse as body2')
    bindings = []
    for line in registry.decode('latin1').splitlines():
        match = re.match(r'\s*"(IrisFlare|Teleporter)"\s+"([^\"]+)"\s+(0x[0-9a-fA-F]+)', line)
        if match:
            bindings.append(dict(name=match[1], path=match[2], class_id=match[3]))
    if {b['name'] for b in bindings} != {'IrisFlare', 'Teleporter'}:
        raise ValueError('Expected exact type bindings absent')
    source = (ROOT / 'src/vfxtest.cpp').read_text()
    start = source.index('e.id            = "TFlareAnimator_BESPOKE__Teleporter";')
    block = source[start:source.index('VfxTest::DeferredRegister(e);', start)]
    if 'FlareBespokeVariantSpawn<kVariantIrisFlare_I3D>' not in block:
        raise ValueError('Preview wiring changed; reassess diagnostic')
    return dict(status='deferred_unsupported_association', full_effect_accepted=False,
        rendered_pairs=0, retail_sha256=digest, asset_sha256=ASSET_SHA,
        registry_sha256=hashlib.sha256(registry).hexdigest(), registrations=registrations,
        exact_type_bindings=bindings, asset=dict(body_offset=hex(body), flags=flags,
            format='SOld3DImageryBody: I3D_3DIMAGEBODY2 bit absent', numverts=numverts,
            numfaces=numfaces, byte_length=len(asset)),
        preview=dict(id='TFlareAnimator_BESPOKE__Teleporter',
            controller='TFlareEffect_Bespoke', behavior='ten bouncing particles',
            association_sha256=hashlib.sha256(block.encode()).hexdigest()),
        native_iris=dict(classification='stationary compound animator, no missile trajectory',
            behavior='five morphing flares plus cylinder; reset after tick200',
            mapping_correction='0x5a9f2c is TIrisFlareAnimator, not TFlareAnimator'),
        blockers=['preview controller is not exact retail animator',
            'legacy mesh/material/object/key decoder needed before authored render fixture',
            'Teleporter gameplay source/destination/caster payload requires distinct context'],
        next_action='Mark current Teleporter Flare preview unsupported as a retail comparison; '
            'retain it as a placeholder. Later add exact IrisFlare legacy-asset/controller '
            'adapter and test Teleporter separately with source/destination/caster context.',
        native_execution=False, engine_changed=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    report = inspect(args.executable)
    report['probe_sha256'] = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
