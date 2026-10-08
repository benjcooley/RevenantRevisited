#!/usr/bin/env python3
"""Private retail workspaces and versioned, named experimental executable builds."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import uuid


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def build_name(name):
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_.-]{0,79}', name):
        raise ValueError('Build name must use 1-80 letters, digits, dots, underscores or hyphens')
    return name


class Workspace:
    def __init__(self, path):
        self.path = Path(path).resolve()
        self.manifest = json.loads((self.path/'workspace.json').read_text())

    @classmethod
    def create(cls, path, baseline, source=None):
        path = Path(path).resolve()
        path.mkdir(parents=True, exist_ok=True)
        if any(path.iterdir()):
            raise ValueError('New workspace must be empty')
        baseline = Path(baseline).resolve()
        expected = sha256(baseline)
        private = path/'baseline.exe'
        # copyfile creates new storage: never a shared writable hardlink.
        shutil.copyfile(baseline, private)
        if sha256(private) != expected or sha256(baseline) != expected:
            raise ValueError('Baseline changed while creating workspace')
        private.chmod(0o444)
        if source is not None:
            source = Path(source).resolve()
            for pattern in ('*.asm', '*.inc'):
                for original in source.rglob(pattern):
                    target = path/'source'/original.relative_to(source)
                    target.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copyfile(original, target)
        (path/'workspace.json').write_text(json.dumps(dict(
            baseline_sha256=expected, baseline_path=str(private),
            original_baseline_path=str(baseline), private_source=str(path/'source'), builds=[]), indent=2)+'\n')
        workspace = cls(path)
        workspace.import_variant('baseline', private, dict(kind='unchanged-baseline'))
        return workspace

    def _check_baseline(self):
        if sha256(self.manifest['baseline_path']) != self.manifest['baseline_sha256']:
            raise ValueError('Private baseline changed')

    def import_variant(self, name, executable, patch_manifest):
        """Publish a copied artifact; edits are made to inputs, never old builds."""
        name = build_name(name)
        self._check_baseline()
        executable = Path(executable).resolve()
        data = executable.read_bytes()
        if len(data) < 64 or data[:2] != b'MZ':
            raise ValueError('Variant is not a PE executable')
        offset = int.from_bytes(data[60:64], 'little')
        if data[offset:offset+6] != b'PE\0\0\x4c\x01':
            raise ValueError('Variant must be an x86 PE executable')
        digest = hashlib.sha256(data).hexdigest()
        identifier = f'{name}-{digest[:16]}-{uuid.uuid4().hex[:8]}'
        directory = self.path/'builds'/identifier
        directory.mkdir(parents=True)
        target = directory/f'Revenant.{name}.exe'
        target.write_bytes(data)
        target.chmod(0o444)
        record = dict(build_id=identifier, name=name, executable=str(target), executable_sha256=digest,
            baseline_sha256=self.manifest['baseline_sha256'], patch_manifest=patch_manifest,
            workspace=str(self.path))
        (directory/'build.json').write_text(json.dumps(record, indent=2)+'\n')
        self.manifest['builds'].append(record)
        self._check_baseline()
        (self.path/'workspace.json').write_text(json.dumps(self.manifest, indent=2)+'\n')
        return record

    def build_variant(self, name, patch_manifest, nasm='nasm', timeout=120):
        """Assemble only this workspace's source and publish a new named version."""
        build_name(name)
        source = self.path/'source'
        if not (source/'retail.asm').is_file():
            raise ValueError('Workspace has no private source/retail.asm')
        sources = {str(p.relative_to(source)): sha256(p)
            for p in sorted(source.rglob('*')) if p.is_file()}
        with tempfile.TemporaryDirectory(prefix='assembly-', dir=self.path) as temporary:
            output = Path(temporary)/'candidate.exe'
            result = subprocess.run([nasm, '-f', 'bin', '-o', str(output), 'retail.asm'],
                cwd=source, capture_output=True, text=True, timeout=timeout)
            if result.returncode:
                raise RuntimeError('Private assembly failed: '+result.stderr)
            after = {str(p.relative_to(source)): sha256(p)
                for p in sorted(source.rglob('*')) if p.is_file()}
            if after != sources:
                raise RuntimeError('Private source changed while assembly ran; no build published')
            manifest = dict(changes=patch_manifest, source_sha256=sources,
                assembler=nasm, assembler_output=result.stderr)
            return self.import_variant(name, output, manifest)

    def find(self, build_id):
        return next(record for record in self.manifest['builds'] if record['build_id'] == build_id)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    create = commands.add_parser('create')
    create.add_argument('workspace', type=Path)
    create.add_argument('--baseline', type=Path, required=True)
    create.add_argument('--source', type=Path)
    for command in ('build', 'import'):
        child = commands.add_parser(command)
        child.add_argument('workspace', type=Path)
        child.add_argument('--name', required=True)
        child.add_argument('--patch-manifest', type=Path, required=True)
        if command == 'import':
            child.add_argument('--executable', type=Path, required=True)
        else:
            child.add_argument('--nasm', default='nasm')
    listing = commands.add_parser('list')
    listing.add_argument('workspace', type=Path)
    args = parser.parse_args()
    if args.command == 'create':
        result = Workspace.create(args.workspace, args.baseline, args.source).manifest
    else:
        workspace = Workspace(args.workspace)
        if args.command == 'list':
            result = workspace.manifest['builds']
        else:
            patch = json.loads(args.patch_manifest.read_text())
            result = workspace.build_variant(args.name, patch, args.nasm) if args.command == 'build' else \
                workspace.import_variant(args.name, args.executable, patch)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
