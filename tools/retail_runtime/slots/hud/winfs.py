"""Win32 file system for the HUD slot: the retail install, read-only, at the
path the shipped game runs from.

The core runtime keeps files in `vm.files` (lower-case, '/'-separated
names). This layer adds what WinMain and the CRT reach:

- Paths are canonical Windows paths. Relative names resolve against the
  virtual current directory (`C:\\REVENANT`, the install directory), `.`
  and `..` collapse, case folds.
- `mount_tree(host_dir)`: every file of the install, read-only. Contents are
  immutable `bytes`, so checkpoints share them instead of copying (the packs
  are 250 MB); a write to one fails explicitly.
- CreateFileA ignores `lpSecurityAttributes` (the CRT always passes one;
  handle inheritance means nothing in one emulated process) and delegates
  to the core with the canonical name.
- CreateDirectoryA / RemoveDirectoryA: directories are implicit in the
  file tree; created ones are kept in `directories`.
- GetModuleFileNameA, GetCurrentDirectoryA, GetFullPathNameA,
  GetFileAttributesA, SetFileAttributesA, DeleteFileA,
  FindFirstFileA/FindNextFileA/FindClose over the virtual tree.
"""
from __future__ import annotations

import fnmatch
import posixpath
import struct
from pathlib import Path

FILE_ATTRIBUTE_DIRECTORY, FILE_ATTRIBUTE_ARCHIVE = 0x10, 0x20
INVALID = 0xffffffff
ERROR_FILE_NOT_FOUND, ERROR_NO_MORE_FILES = 2, 18


class WinFileSystem:
    def __init__(self, vm, root='C:\\REVENANT', module='Revenant.exe'):
        self.vm = vm
        self.root = root
        self.cwd = root
        self.module_path = root + '\\' + module
        self.finds = {}                       # handle -> remaining matches
        self.directories = set()              # created directory keys
        self.next_find = 0x6000
        self._path_scratch = vm.allocate(1024)    # canonical name handed to the core
        core_create = vm.handlers['CreateFileA'][1]
        self._core_create = core_create
        for name, argc, fn in [
                ('CreateFileA', 7, self._create),
                ('GetModuleFileNameA', 3, self._module_file_name),
                ('GetCurrentDirectoryA', 2, self._current_directory),
                ('SetCurrentDirectoryA', 1, self._set_current_directory),
                ('GetFullPathNameA', 4, self._full_path),
                ('GetFileAttributesA', 1, self._attributes),
                ('SetFileAttributesA', 2, self._set_attributes),
                ('DeleteFileA', 1, self._delete),
                ('CreateDirectoryA', 2, self._create_directory),
                ('RemoveDirectoryA', 1, self._remove_directory),
                ('FindFirstFileA', 2, self._find_first),
                ('FindNextFileA', 2, self._find_next),
                ('FindClose', 1, self._find_close)]:
            vm.handlers[name] = (argc, fn)

    # ---- paths ---------------------------------------------------------

    def key(self, path):
        """Canonical `vm.files` key for a Windows path."""
        path = path.replace('/', '\\')
        if not (len(path) > 1 and path[1] == ':'):
            path = (self.root[:2] + path) if path.startswith('\\') else (self.cwd + '\\' + path)
        drive, rest = path[:2], path[2:].replace('\\', '/')
        return (drive + posixpath.normpath('/' + rest.lstrip('/'))).lower()

    def windows(self, key):
        return key.replace('/', '\\')

    def mount_tree(self, host_dir, at=None):
        """Mount every file under `host_dir` read-only at `at` (default the
        install root). Returns the number of files."""
        base = self.key(at or self.root)
        count = 0
        for path in sorted(Path(host_dir).rglob('*')):
            if path.is_file():
                relative = path.relative_to(host_dir).as_posix().lower()
                self.vm.files[f'{base}/{relative}'] = path.read_bytes()
                count += 1
        return count

    def mount(self, windows_path, data):
        self.vm.files[self.key(windows_path)] = bytes(data)

    def _is_directory(self, key):
        prefix = key.rstrip('/') + '/'
        return key in self.directories or any(name.startswith(prefix) for name in self.vm.files)

    # ---- APIs ----------------------------------------------------------

    def _create(self, args):
        name = self.key(self.vm.string(args[0])).encode('cp1252') + b'\0'
        if len(name) > 1024:
            raise ValueError('CreateFileA path longer than the scratch buffer')
        self.vm.write(self._path_scratch, name)
        return self._core_create((self._path_scratch, args[1], args[2], 0) + tuple(args[4:]))

    def _copy_out(self, text, buffer, size):
        data = text.encode('cp1252')
        if size <= len(data):
            return len(data) + 1              # required size, buffer untouched
        self.vm.write(buffer, data + b'\0')
        return len(data)

    def _module_file_name(self, args):
        module, buffer, size = args
        if module not in (0, self.vm.base):
            raise ValueError(f'GetModuleFileNameA for module 0x{module:x}')
        data = self.module_path.encode('cp1252')[:max(size - 1, 0)]
        self.vm.write(buffer, data + b'\0')
        return len(data)

    def _current_directory(self, args):
        size, buffer = args
        return self._copy_out(self.cwd, buffer, size)

    def _set_current_directory(self, args):
        key = self.key(self.vm.string(args[0]))
        if not self._is_directory(key):
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        self.cwd = self.windows(key).upper()
        return 1

    def _full_path(self, args):
        name, size, buffer, file_part = args
        full = self.windows(self.key(self.vm.string(name)))
        result = self._copy_out(full, buffer, size)
        if file_part and result < size:
            self.vm.put_u32(file_part, buffer + full.rfind('\\') + 1)
        return result

    def _attributes(self, args):
        key = self.key(self.vm.string(args[0]))
        if key in self.vm.files:
            return FILE_ATTRIBUTE_ARCHIVE
        if self._is_directory(key):
            return FILE_ATTRIBUTE_DIRECTORY
        self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
        return INVALID

    def _set_attributes(self, args):
        key = self.key(self.vm.string(args[0]))
        if key not in self.vm.files:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        return 1                              # attributes are not modelled

    def _delete(self, args):
        key = self.key(self.vm.string(args[0]))
        if self.vm.files.pop(key, None) is None:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        return 1

    def _create_directory(self, args):
        key = self.key(self.vm.string(args[0]))
        if key in self.vm.files or self._is_directory(key):
            self.vm.scheduler.set_error((183,))       # ERROR_ALREADY_EXISTS
            return 0
        self.directories.add(key)
        return 1

    def _remove_directory(self, args):
        key = self.key(self.vm.string(args[0]))
        if key not in self.directories:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return 0
        self.directories.discard(key)
        return 1

    def _find_data(self, out, key):
        """WIN32_FIND_DATAA: attributes, times (0), size, name."""
        block = bytearray(320)
        is_file = key in self.vm.files
        struct.pack_into('<I', block, 0, FILE_ATTRIBUTE_ARCHIVE if is_file else FILE_ATTRIBUTE_DIRECTORY)
        struct.pack_into('<I', block, 32, len(self.vm.files[key]) if is_file else 0)
        name = posixpath.basename(key).encode('cp1252')[:259]
        block[44:44 + len(name)] = name
        self.vm.write(out, bytes(block))

    def _find_first(self, args):
        pattern, out = args
        key = self.key(self.vm.string(pattern))
        directory, mask = posixpath.split(key)
        names = sorted({directory + '/' + name[len(directory) + 1:].split('/')[0]
                        for name in self.vm.files if name.startswith(directory + '/')})
        matches = [n for n in names if fnmatch.fnmatchcase(posixpath.basename(n), mask)]
        if not matches:
            self.vm.scheduler.set_error((ERROR_FILE_NOT_FOUND,))
            return INVALID
        handle = self.next_find
        self.next_find += 1
        self._find_data(out, matches[0])
        self.finds[handle] = matches[1:]
        return handle

    def _find_next(self, args):
        handle, out = args
        remaining = self.finds.get(handle)
        if not remaining:
            self.vm.scheduler.set_error((ERROR_NO_MORE_FILES,))
            return 0
        self._find_data(out, remaining.pop(0))
        return 1

    def _find_close(self, args):
        return int(self.finds.pop(args[0], None) is not None)

    def snapshot(self):
        return (self.cwd, {h: list(m) for h, m in self.finds.items()}, self.next_find,
                set(self.directories))

    def restore(self, state):
        self.cwd, finds, self.next_find, directories = state
        self.finds = {h: list(m) for h, m in finds.items()}
        self.directories = set(directories)
