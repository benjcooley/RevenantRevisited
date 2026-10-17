"""Private profile (INI) APIs over the virtual file system.

GetPrivateProfileStringA / GetPrivateProfileIntA / WritePrivateProfileStringA
with the documented Windows semantics retail relies on:
- A file name without a directory is in the Windows directory
  (`C:\\WINDOWS`); otherwise it resolves like any other path (`winfs`).
- Section and key names compare case-insensitively; values are trimmed and
  one pair of enclosing double quotes is removed.
- A NULL section lists the section names, a NULL key the section's keys,
  each NUL-terminated, the list ending in an extra NUL.
- The returned string is truncated to nSize-1 (nSize-2 for lists); the
  return is the number of characters copied, not counting the NUL.
- GetPrivateProfileIntA parses a leading decimal (or 0x hex) integer; a
  value that does not start with a number gives 0, a missing key the default.
Writes go to the virtual file only (never the host).
"""
from __future__ import annotations

import copy
WINDOWS_DIRECTORY = 'C:\\WINDOWS'


def parse(text):
    """[(section, [(key, value), ...]), ...] in file order."""
    sections = []
    current = None
    for raw in text.splitlines():
        line = raw.strip()
        if not line or line.startswith(';'):
            continue
        if line.startswith('[') and ']' in line:
            current = (line[1:line.index(']')].strip(), [])
            sections.append(current)
        elif current is not None and '=' in line:
            key, value = line.split('=', 1)
            value = value.strip()
            if len(value) >= 2 and value[0] == value[-1] == '"':
                value = value[1:-1]
            current[1].append((key.strip(), value))
    return sections


class PrivateProfile:
    def __init__(self, vm, fs, state=None):
        self.vm = vm
        self.fs = fs
        self.reads = []                       # (file, section, key, result) in call order
        for name, argc, fn in [('GetPrivateProfileStringA', 6, self._string),
                               ('GetPrivateProfileIntA', 4, self._int),
                               ('WritePrivateProfileStringA', 4, self._write)]:
            vm.handlers[name] = (argc, fn)
        if state is not None:
            self.set_state(state)

    def _key(self, name):
        if '\\' not in name and '/' not in name:
            name = WINDOWS_DIRECTORY + '\\' + name
        return self.fs.key(name)

    def _load(self, file_name):
        data = self.vm.files.get(self._key(file_name))
        return parse(bytes(data).decode('cp1252')) if data is not None else []

    def _find(self, sections, section, key=None):
        for name, entries in sections:
            if name.lower() == section.lower():
                if key is None:
                    return entries
                for k, v in entries:
                    if k.lower() == key.lower():
                        return v
                return None
        return None

    def _text(self, pointer):
        return self.vm.string(pointer) if pointer else None

    def _string(self, args):
        section, key, default, buffer, size, file_name = args
        section, key, file_name = self._text(section), self._text(key), self._text(file_name) or ''
        sections = self._load(file_name)
        if section is None or key is None:
            names = ([name for name, _ in sections] if section is None
                     else [k for k, _ in (self._find(sections, section) or [])])
            data = b''.join(n.encode('cp1252') + b'\0' for n in names)
            if size < 2:
                return 0
            if len(data) + 1 > size:
                data = data[:size - 2] + b'\0'
            self.vm.write(buffer, data + b'\0')
            self.reads.append((file_name, section, key, names))
            return len(data)
        value = self._find(sections, section, key)
        if value is None:
            value = (self._text(default) or '').strip()
        data = value.encode('cp1252')[:max(size - 1, 0)]
        if size:
            self.vm.write(buffer, data + b'\0')
        self.reads.append((file_name, section, key, value))
        return len(data)

    def _int(self, args):
        section, key, default, file_name = args
        file_name = self._text(file_name) or ''
        value = self._find(self._load(file_name), self._text(section), self._text(key))
        if value is None:
            result = default
        else:
            text = value.strip()
            sign = -1 if text.startswith('-') else 1
            text = text.lstrip('+-')
            digits = ''
            base = 16 if text.lower().startswith('0x') else 10
            for c in text[2:] if base == 16 else text:
                if c.lower() in '0123456789abcdef'[:base]:
                    digits += c
                else:
                    break
            result = sign * int(digits, base) if digits else 0
        self.reads.append((file_name, self._text(section), self._text(key), result))
        return result & 0xffffffff

    def _write(self, args):
        section, key, value, file_name = (self._text(a) for a in args)
        path = self._key(file_name or '')
        sections = self._load(file_name or '')
        entries = self._find(sections, section) if section else None
        if section is None:
            raise ValueError('WritePrivateProfileStringA flush (NULL section) not modelled')
        if entries is None:
            entries = []
            sections.append((section, entries))
        entries[:] = [(k, v) for k, v in entries if key is None or k.lower() != key.lower()]
        if key is not None and value is not None:
            entries.append((key, value))
        if key is None:
            sections[:] = [s for s in sections if s[0].lower() != section.lower()]
        text = ''.join(f'[{name}]\r\n' + ''.join(f'{k}={v}\r\n' for k, v in items)
                       for name, items in sections)
        self.vm.files[path] = bytearray(text.encode('cp1252'))
        return 1

    STATE = ('reads',)

    def state(self):
        return copy.deepcopy({name: getattr(self, name) for name in self.STATE})

    def set_state(self, state):
        for name, value in copy.deepcopy(state).items():
            setattr(self, name, value)
