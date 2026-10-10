"""Exact Flare registration/asset-absence preflight; no substituted imagery."""
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


def inspect(executable):
    image = Path(executable).read_bytes()
    digest = hashlib.sha256(image).hexdigest()
    if digest != RETAIL_SHA:
        raise ValueError('Exact unchanged retail image required')
    layout = pe_layout(image)
    def read(address, count):
        offset = file_offset(layout, address)
        return image[offset:offset + count]
    registration = read(0x4df380, 26)
    if registration[0] != 0x68 or struct.unpack_from('<I', registration, 1)[0] != 0x5e1064:
        raise ValueError('Registration changed')
    if struct.unpack_from('<I', registration, 21)[0] != 0x5a89b0 or read(0x5e1064, 6) != b'Flare\0':
        raise ValueError('Flare builder/name changed')
    slots = [struct.unpack('<I', read(0x5a89b4 + offset, 4))[0] for offset in (24, 44, 52)]
    if slots != [0x4df3a0, 0x4df430, 0x4df550]:
        raise ValueError('Leaf methods changed')
    if read(0x4df3b0, 5) != b'\xb9\x0f\x00\x00\x00':
        raise ValueError('Initialize pool count changed')
    if read(0x4df44f, 5) != b'\xbb\x0a\x00\x00\x00':
        raise ValueError('Active Animate count changed')
    with zipfile.ZipFile(ROOT / 'data/imagery.rvi') as archive:
        registry = archive.read('class.def')
        names = archive.namelist()
        flare_assets = [n for n in names if 'flare' in n.lower()]
        bindings = re.findall(r'"Flare"\s+"([^\"]+)"', registry.decode('latin1'))
        if bindings or any(n.lower().endswith('/flare.i3d') for n in names):
            raise ValueError('A genuine Flare binding/asset now exists; reassess defer')
        adjacent_hashes = {n: hashlib.sha256(archive.read(n)).hexdigest() for n in flare_assets}
    return dict(status='deferred_no_exact_retail_asset_binding',
        retail_sha256=digest, registry_sha256=hashlib.sha256(registry).hexdigest(),
        registration='0x4df380', builder='0x5a89b0', vtable='0x5a89b4',
        factory=hex(struct.unpack('<I', read(0x5a89b0, 4))[0]),
        initialize=hex(slots[0]), animate=hex(slots[1]), render=hex(slots[2]),
        native_pool_slots=15, active_animated_rendered_slots=10,
        classification='stationary owner; internally moving bouncing particles, no two-endpoint missile',
        exact_type_bindings=bindings, archive_entry_count=len(names), flare_named_assets=adjacent_hashes,
        existing_preview='TFlareEffect_Bespoke uses Misc/IrisFlare.I3D, guessed48-unit card, 41ms cadence and warm light',
        blockers=['no exact Flare type or Flare.i3d asset in supplied retail imagery archive',
            'IrisFlare and tflare asset substitutions have no proved Flare caller/binding',
            'authored mesh/material/card dimensions and natural caller unresolved'],
        next_action='Retain standalone Flare as a diagnostic preview; do not assign IrisFlare or Teleporter '
            'acceptance from it. Resume only if an original Flare caller/asset binding is recovered; '
            'otherwise prioritize an actual registered effect row.',
        native_execution=False, state_acceptance=False, rendered_pairs=0,
        full_effect_accepted=False, engine_changed=False)


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
