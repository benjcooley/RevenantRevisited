#!/usr/bin/env python3
"""Attach same-binary Ghidra names/notes without changing any emitted bytes."""
import argparse
import hashlib
import json
from pathlib import Path
import re


def sanitize(value):
    return re.sub(r'[^A-Za-z0-9_]', '_', value)


def annotate(root, repository):
    manifest=json.loads((root/'manifest.json').read_text())
    ghidra_input=repository/'data/Revenant.exe'
    if hashlib.sha256(ghidra_input.read_bytes()).hexdigest()!=manifest['input_sha256']:
        raise ValueError('Ghidra input differs from this assembly image; refusing mixed addresses')
    symbols={}
    functions={}
    def add(address,name,path,line,kind,description=''):
        if not 0x400000<=address<0x80000000:
            return
        alias=sanitize(name)+f'_{address:08x}'
        record=dict(address=f'0x{address:08x}',name=name,alias=alias,
                    provenance=str(path.relative_to(repository)),line=line,kind=kind,description=description)
        symbols.setdefault((address,alias),record)
        if kind in ('ghidra_method','identified_function'):
            functions.setdefault(address,[]).append(record)
    files=list((repository/'recon/classes_readable').glob('*.cpp'))
    files+=list((repository/'recon/ghidra').glob('*.cpp'))
    for path in sorted(files):
        lines=path.read_text(errors='replace').splitlines()
        for i,line in enumerate(lines):
            match=re.match(r'\s*// Function at ([0-9a-fA-F]{8})\b',line)
            if match:
                address=int(match.group(1),16)
                following=' '.join(lines[i+1:i+16]).split('{',1)[0]
                method=re.search(r'([A-Za-z_][\w]*(?:::[~\w]+)+)\s*\(',following)
                name=method.group(1) if method else f'{path.stem}::function_{address:08x}'
                add(address,'ghidra_'+name,path,i+1,'ghidra_method',
                    'Recovered class association; generic meth names are not a semantic identification.')
    free=repository/'recon/docs/FREE_FUNCTION_IDENTIFICATIONS.md'
    for i,line in enumerate(free.read_text().splitlines()):
        if line.startswith('|'):
            fields=line.split('|')
            match=re.search(r'@ `0x([0-9a-fA-F]+)`',fields[1] if len(fields)>1 else '')
            if match and len(fields)>3:
                address=int(match.group(1),16)
                identity=fields[2].strip().replace('`','')
                add(address,'identified_'+identity.split('(')[0].strip()+f'_{address:08x}',
                    free,i+1,'identified_function',identity)
    listing=repository/'recon/ghidra/_data.txt'
    for i,line in enumerate(listing.read_text(errors='replace').splitlines()):
        match=re.match(r'\s+([A-Za-z_][\w]*_([0-9a-fA-F]{8}))\s+XREF',line)
        if match:
            add(int(match.group(2),16),'ghidra_'+match.group(1),listing,i+1,'ghidra_data_label')
    # Our verified partsys entry identifications are useful even where OOAnalyzer
    # never assigned a readable class. Keep the original note as the authority.
    parts=repository/'docs/vfx/forensics/PARTSYS_RUNTIME.md'
    for i,line in enumerate(parts.read_text().splitlines()):
        match=re.match(r'- `0x([0-9a-fA-F]+)`: ([^,;]+)',line)
        if match:
            address=int(match.group(1),16)
            add(address,f'noted_PartSys_{match.group(2).strip()}_{address:08x}',parts,i+1,'identified_function',line[2:])
    entry=manifest['layout']['image_base']+manifest['layout']['entry_rva']
    alias_lines=['; Same-image symbol aliases; comments/names only.','; Source classifications are hypotheses unless their provenance identifies them.']
    alias_lines.append(f'retail_entry equ 0x{entry:08x}')
    for (address,alias),record in sorted(symbols.items()):
        description=record['description'].replace('\n',' ')[:180]
        alias_lines.extend([f'; {record["kind"]}: {record["provenance"]}:{record["line"]} {description}',
                            f'{alias} equ 0x{address:08x}'])
    (root/'symbols.inc').write_text('\n'.join(alias_lines)+'\n')
    main=root/'retail.asm';text=main.read_text()
    if '%include "symbols.inc"' not in text:
        text=text.replace('BITS 32\n','BITS 32\n%include "symbols.inc"\n',1)
    main.write_text(text)
    code_file=next(p for p in (root/'sections').glob('*.asm') if 'section_0' in p.name)
    original=code_file.read_text()
    # Remove only our own annotations on repeated runs.
    original=re.sub(r'; BEGIN RECOVERED FUNCTION[^\n]*\n(?:; INFO [^\n]*\n)*','',original)
    matched=set()
    def note(match):
        address=int(match.group(1),16)
        records=functions.get(address,[])
        if not records:
            return match.group(0)
        matched.add(address)
        lines=[f'; BEGIN RECOVERED FUNCTION 0x{address:08x}']
        lines += [f'; INFO {r["name"]} | {r["provenance"]}:{r["line"]}' for r in records]
        return '\n'.join(lines)+'\n'+match.group(0)
    code_file.write_text(re.sub(r'^va_([0-9a-fA-F]{8}):[^\n]*',note,original,flags=re.M))
    report=dict(status='annotations_generated_verification_pending',same_binary_sha256=manifest['input_sha256'],
        symbol_aliases=len(symbols)+1,function_start_addresses=len(functions),
        function_starts_at_existing_decoder_boundaries=len(matched),
        function_starts_inside_linear_sweep_records=[f'0x{x:08x}' for x in sorted(set(functions)-matched)],
        scope='Optional same-hash Ghidra/notes annotations. No code bytes or data classification changed. Unaligned starts are retained as aliases and require function-aware relifting; heuristic names do not prove semantic recovery.',
        provenance=[str(ghidra_input.relative_to(repository)),str(free.relative_to(repository)),str(parts.relative_to(repository))])
    (root/'symbols.json').write_text(json.dumps(list(symbols.values()),indent=2)+'\n')
    (root/'annotation-report.json').write_text(json.dumps(report,indent=2)+'\n')
    manifest['annotations']=report
    manifest['source_sha256']={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest()
                              for p in sorted(root.rglob('*')) if p.suffix in ('.asm','.inc')}
    (root/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print(f'Added {len(symbols)+1} symbol aliases; {len(matched)} function annotations; entry=0x{entry:08x}.')


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('assembly',type=Path);p.add_argument('--repository',type=Path,required=True)
    a=p.parse_args();annotate(a.assembly.resolve(),a.repository.resolve())
