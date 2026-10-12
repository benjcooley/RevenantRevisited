"""Read-only exact Fire/Dust literal references and shipped text inventory.

Absence of a literal reference does not exclude computed/script-selected names.
Returned text hits require caller review; this tool awards no visual credit.
"""
import argparse
import hashlib
import json
import re
import struct
import zipfile
from pathlib import Path
from character_native_preflight import ROOT, sha


def run(output, names=('Fire', 'Dust'), type_ids=(0x119f01e3, 0x0c052637)):
    exe = ROOT / 'recon/retail_asm/baseline/Revenant.rebuilt.exe'
    data = exe.read_bytes()
    pe = struct.unpack_from('<I', data, 60)[0]
    count = struct.unpack_from('<H', data, pe + 6)[0]
    table = pe + 24 + struct.unpack_from('<H', data, pe + 20)[0]
    sections = []
    for i in range(count):
        p = table + 40*i
        _, va, size, raw = struct.unpack_from('<4I', data, p + 8)
        sections.append((data[p:p+8].split(b'\0')[0].decode(), va + 0x400000, raw, size))
    def address(offset):
        for name, va, raw, size in sections:
            if raw <= offset < raw + size:
                return va + offset - raw
        raise ValueError(offset)
    def code_refs(value):
        refs = []
        needle = struct.pack('<I', value)
        for name, va, raw, size in sections:
            if name != '.text':
                continue
            start = raw
            while True:
                pos = data.find(needle, start, raw + size)
                if pos < 0:
                    break
                refs.append(hex(address(pos)))
                start = pos + 1
        return refs
    literals = []
    # Include suffix-pooled strings: a pointer may address Fire inside DragonFire.
    for name in dict.fromkeys(names):
        for match in re.finditer(re.escape(name.encode('ascii')) + b'\0', data, re.IGNORECASE):
            at = address(match.start())
            literals.append(dict(text=match.group()[:-1].decode(), address=hex(at), code_refs=code_refs(at)))
    archives = [ROOT/'data/imagery.rvi', ROOT/'data/resources.rvr']
    archives += sorted((ROOT/'data/Modules').glob('*.rvm'))
    inventories = []
    alternatives = '|'.join(re.escape(name) for name in names)
    exact = re.compile(r'(?i)"(?:' + alternatives + r')"')
    command = re.compile(r'(?i)\b(?:new|create|effect|attacheffect|spawn)\b.*\b(?:' + alternatives + r')\b')
    for archive in archives:
        hits = []
        text_count = 0
        with zipfile.ZipFile(archive) as z:
            for name in z.namelist():
                if not name.lower().endswith(('.s', '.def', '.txt')):
                    continue
                text_count += 1
                for line, text in enumerate(z.read(name).decode('latin1').splitlines(), 1):
                    if exact.search(text) or command.search(text):
                        hits.append(dict(file=name, line=line, text=text.strip()))
        inventories.append(dict(archive=str(archive), sha256=sha(archive),
                                text_files=text_count, candidate_lines=hits))
    result = dict(status='literal_inventory_complete', accepted=False,
                  executable_sha256=sha(exe), probe_sha256=sha(__file__),
                  nul_terminated_strings_including_suffixes=literals,
                  selected_names=list(names),
                  type_id_code_references={hex(x):code_refs(x) for x in type_ids},
                  archives=inventories, scope=__doc__)
    output.write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps(dict(strings=literals, ids=result['type_id_code_references'],
                          candidate_lines=sum(len(a['candidate_lines']) for a in inventories))))
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--name', action='append', help='Exact ASCII name; default Fire and Dust')
    parser.add_argument('--type-id', action='append', type=lambda x:int(x,0), help='Exact type ID; default Fire and Dust')
    args=parser.parse_args()
    if bool(args.name)!=bool(args.type_id): parser.error('--name and --type-id must be supplied together')
    run(args.output, args.name or ('Fire','Dust'), args.type_id or (0x119f01e3,0x0c052637))
