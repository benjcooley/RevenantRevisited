"""Finite read-only audit of supplied Sand callers and archived registrations.

Raw archive searches establish only byte/token absence in the supplied inputs;
they do not infer runtime reachability or enable historical registrations.
"""
import argparse
import hashlib
import json
import re
import subprocess
import zipfile
from pathlib import Path


def sha(data):
    return hashlib.sha256(data).hexdigest()


def uncomment(text):
    return re.sub(r'/\*.*?\*/|//[^\r\n]*',
                  lambda m: re.sub(r'[^\r\n]', ' ', m.group()), text, flags=re.S)


def run(legacy, retail, output):
    output.mkdir(parents=True, exist_ok=False)
    spells = []
    for path in (legacy/'spell.def', legacy/'cbspell.def', retail/'Resources/spell.def'):
        data = path.read_bytes()
        text = uncomment(data.decode('latin1'))
        variants = re.findall(r'^\s*VARIANT\s+"([^"]+)"[^\r\n]*', text, re.M | re.I)
        sand = [line.strip() for line in text.splitlines()
                if re.search(r'\bVARIANT\b.*"(?:sand|sandswirl)"', line, re.I)]
        quick = [line.strip() for line in text.splitlines()
                 if re.search(r'\bVARIANT\b.*"quicksand"', line, re.I)]
        assert not sand and quick
        spells.append(dict(path=str(path), sha256=sha(data), variants=len(variants),
                           sand_variants=sand, quicksand_variants=quick))

    registration = re.compile(r'(?:REGISTER_[A-Z0-9_]*ANIMATOR_?|DEFINE_BUILDER)\s*\(\s*"([^"]+)"')
    bindings = []
    for name in ('effect.cpp', 'backup/effect.cpp', 'walkcode/effect.cpp'):
        data = (legacy/name).read_bytes()
        text = data.decode('latin1')
        active = uncomment(text).splitlines()
        rows = []
        for number, line in enumerate(text.splitlines(), 1):
            match = registration.search(line)
            if match and not registration.search(active[number-1]):
                rows.append(dict(line=number, name=match[1], text=line.strip()))
        bindings.append(dict(path=name, sha256=sha(data), commented_registrations=rows))

    # Search every member, including binary map/saved-state payloads; exact
    # integer hits are reported separately and never interpreted as placements.
    archives = []
    token = re.compile(rb'sandswirl|\bsand\b', re.I)
    for path in sorted((retail/'Modules').glob('*.rvm')):
        matches, ids, scripts, files = [], [], [], 0
        with zipfile.ZipFile(path) as archive:
            for info in archive.infolist():
                if info.is_dir():
                    continue
                files += 1
                data = archive.read(info)
                if token.search(data):
                    matches.append(info.filename)
                found = {name: data.count(bytes.fromhex(value)) for name, value in
                         [('Sandswirl_becefeda', 'dafecebe'), ('Sand_fecaface', 'cefacafe')]}
                if any(found.values()):
                    ids.append(dict(member=info.filename, counts=found))
                if Path(info.filename).suffix.lower() == '.s':
                    scripts.append(dict(member=info.filename, sha256=sha(data)))
        archives.append(dict(path=str(path), sha256=sha(path.read_bytes()), files=files,
                             token_matches=matches, raw_id_matches=ids, scripts=scripts))

    # All tracked legacy text files, not only the three animator snapshots.
    tracked = subprocess.check_output(['git', 'ls-files', '-z'], cwd=legacy).split(b'\0')
    literals, scanned = [], 0
    for raw in tracked:
        if not raw:
            continue
        name = raw.decode()
        path = legacy/name
        if path.suffix.lower() not in ('.cpp', '.h', '.def', '.s', '.txt') or not path.is_file():
            continue
        data = path.read_bytes()
        scanned += 1
        rows = [dict(line=i, text=line.strip()) for i, line in
                enumerate(data.decode('latin1').splitlines(), 1)
                if re.search(r'"(?:sand|sandswirl)"', line, re.I)]
        if rows:
            literals.append(dict(path=name, sha256=sha(data), matches=rows))
    report = dict(status='pass', accepted=False, visual_pass=False,
                  legacy_head=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=legacy, text=True).strip(),
                  spells=spells, commented_bindings=bindings, archives=archives,
                  legacy_text_files=scanned, exact_quoted_literals=literals,
                  tool_sha256=sha(Path(__file__).read_bytes()),
                  limits='Supplied legacy/CD inputs only. No natural caller inferred from comments, asset names, raw integer matches, or the Quicksand control. Dynamic name construction, other releases and unsupplied modules are not excluded. No registry, resource, executable or production behavior changed.')
    (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(dict(status='pass', legacy_text_files=scanned, archives=len(archives),
                         script_members=sum(len(a['scripts']) for a in archives),
                         archive_token_matches=sum(len(a['token_matches']) for a in archives),
                         archive_raw_id_matches=sum(len(a['raw_id_matches']) for a in archives)), indent=2))


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--legacy', type=Path, required=True)
    p.add_argument('--retail', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    run(a.legacy, a.retail, a.output)
