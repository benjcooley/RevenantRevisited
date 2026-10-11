r"""The play slot's write overlay: saves, the sector working set and INI
writes, in memory over the read-only install.

The install is mounted read-only (`winfs.py`: immutable `bytes`, shared by
checkpoints). Retail writes, by its own paths (Revenant.ini [Paths]:
SaveGamePath `.\Save`, CurMapPath `.`; docs/gameflow/forensics/SAVE_GAME.md
§2):
- `Save\Single\<slot>\game.sav`, `ss.bmp`, `CurMap\*.DAT` (a save);
- `CurMap\<level>_<sx>_<sy>.DAT` (sectors as they unload);
- `ss.bmp` (the thumbnail of the last frame, copied into the next save);
- INI files (`WritePrivateProfileStringA`, profile.py, writes its own copy).

CopyFileA (the map pane copies the sector working set into a save,
0x0044cd9f) and MoveFileA (the CRT's rename) work inside the overlay too.
A file created or opened for writing in those places becomes a mutable
`bytearray` in `vm.files`: a new file starts empty, an existing read-only
one is copied first (copy on write). Image saves keep every mutable file
(`image.py`), so the overlay persists across steps and checkpoints, and a
restore brings back exactly the files of that moment. A write anywhere
else fails by name: nothing in the install changes, on the host or in the
guest.

`place_slot` puts a slot directory from the host into `Save\Single\`
(the start of a `slot` session); `export_slot` copies one out. The slot
the game wrote most recently (`@last`, docs/playtest/PROTOCOL.md §5: a
quick save picks its own "Quick Save N") is the one whose file was last
opened for writing.
"""
from __future__ import annotations

import copy
import posixpath
from pathlib import Path

ROOT = 'c:/revenant'
SINGLE = f'{ROOT}/save/single'
WRITABLE_DIRECTORIES = (f'{ROOT}/save/', f'{ROOT}/curmap/')
WRITABLE_FILES = (f'{ROOT}/ss.bmp',)
GENERIC_WRITE = 0x40000000
ERROR_FILE_NOT_FOUND, ERROR_FILE_EXISTS, ERROR_ALREADY_EXISTS = 2, 80, 183


class OverlayError(RuntimeError):
    pass


def writable(key):
    return (key.startswith(WRITABLE_DIRECTORIES) or key in WRITABLE_FILES
            or key.endswith('.ini'))


class SaveOverlay:
    def __init__(self, vm, fs, state=None):
        self.vm = vm
        self.fs = fs
        self.written = []                     # keys opened for writing, in first-write order
        self.last_slot = None
        if state is not None:
            self.set_state(state)
        create = vm.handlers['CreateFileA'][1]   # winfs: canonical names, then the core
        delete = vm.handlers['DeleteFileA'][1]
        vm.handlers['CreateFileA'] = (7, lambda a: self._create(create, a))
        vm.handlers['DeleteFileA'] = (1, lambda a: self._delete(delete, a))
        vm.handlers['CopyFileA'] = (3, self._copy)
        vm.handlers['MoveFileA'] = (2, self._move)

    STATE = ('written', 'last_slot')

    def state(self):
        return copy.deepcopy({name: getattr(self, name) for name in self.STATE})

    def set_state(self, state):
        for name, value in copy.deepcopy(state).items():
            setattr(self, name, value)

    # ---- Win32 ---------------------------------------------------------

    def _create(self, create, args):
        access, disposition = args[1], args[4]
        if access & GENERIC_WRITE or disposition in (1, 2, 5):      # CREATE_NEW/ALWAYS, TRUNCATE
            key = self.fs.key(self.vm.string(args[0]))
            if not writable(key):
                raise OverlayError(f'Write outside the save overlay: {key}')
            data = self.vm.files.get(key)
            if isinstance(data, bytes):
                self.vm.files[key] = bytearray(data)            # copy on write
            self._note(key)
        return create(args)

    def _delete(self, delete, args):
        key = self.fs.key(self.vm.string(args[0]))
        if key in self.vm.files and not writable(key):
            raise OverlayError(f'Delete outside the save overlay: {key}')
        return delete(args)

    def _target(self, pointer):
        name = self.vm.string(pointer)
        key = self.fs.key(name)
        if not writable(key):
            raise OverlayError(f'Write outside the save overlay: {key}')
        return key, name

    def _copy(self, args):
        """CopyFileA(existing, new, fail_if_exists)."""
        source = self.fs.key(self.vm.string(args[0]))
        key, name = self._target(args[1])
        if source not in self.vm.files:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        if args[2] and key in self.vm.files:
            self.vm.scheduler.set_error((ERROR_FILE_EXISTS,))
            return 0
        self.vm.files[key] = bytearray(self.vm.files[source])
        self.fs.names.setdefault(key, self.fs.last_component(name))
        self._note(key)
        return 1

    def _move(self, args):
        """MoveFileA(existing, new): a file, within the overlay."""
        source = self.fs.key(self.vm.string(args[0]))
        key, name = self._target(args[1])
        if not writable(source):
            raise OverlayError(f'Move out of the install: {source}')
        if source not in self.vm.files:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        if key in self.vm.files:
            self.vm.scheduler.set_error((ERROR_ALREADY_EXISTS,))
            return 0
        self.vm.files[key] = self.vm.files.pop(source)
        self.fs.names.pop(source, None)
        self.fs.names[key] = self.fs.last_component(name)
        self._note(key)
        return 1

    def _note(self, key):
        if key not in self.written:
            self.written.append(key)
        if key.startswith(SINGLE + '/'):
            self.last_slot = key[len(SINGLE) + 1:].split('/')[0]

    # ---- slots ---------------------------------------------------------

    def slot_files(self, name):
        """{relative path: bytes} of Save\\Single\\<name>, each component
        in the case the game created it with (winfs `names`)."""
        prefix = f'{SINGLE}/{name.lower()}'
        files = {}
        for key, data in self.vm.files.items():
            if key.startswith(prefix + '/'):
                parts, cursor = [], prefix
                for part in key[len(prefix) + 1:].split('/'):
                    cursor += '/' + part
                    parts.append(self.fs.names.get(cursor, part))
                files['/'.join(parts)] = bytes(data)
        return files

    def slot_name(self, name):
        """The slot's name as the game created it (winfs keeps the case of
        created names); `@last`: the slot written most recently."""
        if name == '@last':
            if self.last_slot is None:
                raise OverlayError('No slot written yet')
            name = self.last_slot
        return self.fs.names.get(f'{SINGLE}/{name.lower()}', name)

    def export_slot(self, name, to):
        name = self.slot_name(name)
        files = self.slot_files(name)
        if not files:
            raise OverlayError(f'No save slot {name!r}')
        target = Path(to)
        for relative, data in sorted(files.items()):
            path = target / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        return name, target

    def place_slot(self, source, name):
        """Copy the host directory `source` into Save\\Single\\<name>."""
        source = Path(source)
        files = sorted(p for p in source.rglob('*') if p.is_file())
        if not any(p.name.lower() == 'game.sav' for p in files):
            raise OverlayError(f'{source} holds no game.sav')
        prefix = f'{SINGLE}/{name.lower()}'
        self.fs.directories.add(prefix)
        self.fs.names[prefix] = name
        for path in files:
            relative = path.relative_to(source).as_posix()
            key = posixpath.join(prefix, relative.lower())
            self.vm.files[key] = bytearray(path.read_bytes())
            self.fs.names[key] = posixpath.basename(relative)
        for directory in {p.parent.relative_to(source).as_posix() for p in files} - {'.'}:
            key = posixpath.join(prefix, directory.lower())
            self.fs.directories.add(key)
            self.fs.names[key] = posixpath.basename(directory)
        return len(files)
