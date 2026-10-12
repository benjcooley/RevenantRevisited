"""Read-only shipped-module usage audit for the three four-frame watcher helpers.

Serialized placement and script references are separate facts. Absence in these
inputs does not establish that an executable path never creates a helper.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import zipfile

from map_effect_placements import ROOT, scan

IDS = {0xaeaeeb23, 0xaeaeeb24, 0xaeaeeb25}
NAMES = re.compile(r'ogrokwatcher[123]|(?:0x)?aeaeeb2[345]|' +
                   '|'.join(str(i) for i in sorted(IDS)) + '|' +
                   '|'.join(str(i - 0x100000000) for i in sorted(IDS)), re.I)
TEXT_SUFFIXES = {'.s', '.def', '.txt', '.ini'}


def audit(data, executable=None):
    modules = []
    scripts = []
    references = []
    errors = []
    archives = sorted((data / 'Modules').glob('*.rvm'))
    # Path spelling aliases share an inode on macOS's case-insensitive volumes.
    seen_archives = {(path.stat().st_dev, path.stat().st_ino) for path in archives}
    for path in (data / 'resources.rvr', data / 'Resources.rvr'):
        if not path.is_file():
            continue
        identity = (path.stat().st_dev, path.stat().st_ino)
        if identity not in seen_archives:
            archives.append(path)
            seen_archives.add(identity)
    for module in archives:
        if module.suffix.lower() == '.rvm':
            placement = scan(module, IDS)
            modules.append(placement)
            errors.extend(placement['errors'])
        with zipfile.ZipFile(module) as archive:
            for member in archive.namelist():
                if Path(member).suffix.lower() not in TEXT_SUFFIXES:
                    continue
                raw = archive.read(member)
                source = dict(container=str(module), member=member,
                              bytes=len(raw), sha256=hashlib.sha256(raw).hexdigest())
                scripts.append(source)
                for line, text in enumerate(raw.decode('cp1252').splitlines(), 1):
                    if NAMES.search(text):
                        references.append(dict(**source, line=line, text=text))
    # Some shipped module text (notably Demo and Deathmatch6) is external.
    loose = list((data / 'Modules').rglob('*')) + list((data / 'Resources').rglob('*'))
    for path in sorted(loose):
        if not path.is_file() or path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        raw = path.read_bytes()
        source = dict(container=str(path), member=None, bytes=len(raw),
                      sha256=hashlib.sha256(raw).hexdigest())
        scripts.append(source)
        for line, text in enumerate(raw.decode('cp1252').splitlines(), 1):
            if NAMES.search(text):
                references.append(dict(**source, line=line, text=text))
    binary = None
    if executable is not None:
        raw = executable.read_bytes()
        binary = dict(path=str(executable.resolve()), bytes=len(raw),
                      sha256=hashlib.sha256(raw).hexdigest(),
                      helper_id_literal_counts={hex(i): raw.count(struct.pack('<I', i)) for i in sorted(IDS)},
                      helper_ascii_name_counts={f'ogrokwatcher{i}': raw.lower().count(f'ogrokwatcher{i}'.encode()) for i in (1, 2, 3)},
                      limits='Byte-pattern literal audit only; computed IDs, indirect class lookup and dynamic creation are not excluded.')
    return dict(status='complete' if not errors else 'partial', accepted=False,
                selected_ids=[hex(i) for i in sorted(IDS)], modules=modules,
                scanned_texts=scripts, references=references, errors=errors,
                total_placements=sum(len(m['placements']) for m in modules),
                executable_literals=binary,
                scope=__doc__,
                limits='Top-level map objects and text-name references only. '
                       'Nested inventory objects, numeric-ID scripting, save '
                       'overrides and runtime executable creation are not '
                       'excluded. No abandoned-artifact classification.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--data', type=Path, default=ROOT / 'data')
    parser.add_argument('--executable', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError('Preserve the prior audit; use a new output path')
    result = audit(args.data, args.executable)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(status=result['status'], modules=len(result['modules']),
                          sectors=sum(m['scanned_sectors'] for m in result['modules']),
                          texts=len(result['scanned_texts']),
                          placements=result['total_placements'],
                          references=len(result['references']), errors=result['errors'])))
    raise SystemExit(1 if result['errors'] else 0)
