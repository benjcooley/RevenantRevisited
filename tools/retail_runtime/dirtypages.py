"""Native dirty-page tracking for Runtime checkpoints (dirtypages.c).

The runtime restores a checkpoint by writing back every page changed since
it. Finding those pages took a Python callback on every guest store (~70%
of a store-heavy fixture's wall time). Here the UC_HOOK_MEM_WRITE callback
is C: it keeps each page's content before its first store and returns.

The library is compiled on first use with the system C compiler into
__pycache__/ (keyed by the source's hash, so an edit rebuilds it and
parallel sessions share it). `available()` is False when that fails; the
runtime then keeps its Python hook.
"""
from __future__ import annotations

import ctypes
import hashlib
import os
import subprocess
from pathlib import Path

from unicorn import UC_HOOK_MEM_WRITE, UcError
from unicorn.unicorn_py3.unicorn import uc_hook_h, uclib

SOURCE = Path(__file__).with_name('dirtypages.c')
FORGOTTEN = 0xffffffff
_lib = None
_error = None


def _load():
    global _lib, _error
    if _lib is not None or _error is not None:
        return _lib
    try:
        digest = hashlib.sha256(SOURCE.read_bytes()).hexdigest()[:16]
        out = SOURCE.parent / '__pycache__' / f'dirtypages-{digest}.so'
        if not out.exists():
            out.parent.mkdir(exist_ok=True)
            temporary = out.with_name(f'{out.name}.{os.getpid()}.tmp')
            subprocess.run(['cc', '-O2', '-shared', '-fPIC', '-o', str(temporary), str(SOURCE)],
                           check=True, capture_output=True)
            temporary.replace(out)
        lib = ctypes.CDLL(str(out))
    except (OSError, subprocess.CalledProcessError) as error:
        _error = error
        return None
    p, u32, i32 = ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int
    for name, restype, argtypes in (
            ('dp_create', p, [p]), ('dp_touch', None, [p, p, u32, u32]),
            ('dp_set_active', None, [p, i32]), ('dp_count', u32, [p]), ('dp_live', u32, [p]),
            ('dp_failed', i32, [p]), ('dp_destroy', None, [p]),
            ('dp_pages', ctypes.POINTER(u32), [p]), ('dp_saved', p, [p, u32]),
            ('dp_forget', None, [p, u32]), ('dp_clear', None, [p])):
        function = getattr(lib, name)
        function.restype, function.argtypes = restype, argtypes
    _lib = lib
    return lib


def available() -> bool:
    return _load() is not None


class DirtyPages:
    """Pages written since the checkpoint, each with its content before."""

    def __init__(self, uc):
        lib = _load()
        if lib is None:
            raise RuntimeError(f'dirtypages.c could not be built: {_error}')
        self.lib, self.uc = lib, uc
        self.tracker = lib.dp_create(ctypes.cast(uclib.uc_mem_read, ctypes.c_void_p))
        if not self.tracker:
            raise MemoryError('dirty-page tracker')
        self.callback = ctypes.cast(lib.dp_hook, ctypes.c_void_p)

    def hook_add(self, begin=1, end=0):
        """A write hook over [begin, end] (all memory by default)."""
        handle = uc_hook_h()
        status = uclib.uc_hook_add(self.uc._uch, ctypes.byref(handle), UC_HOOK_MEM_WRITE,
                                   self.callback, self.tracker, begin, end)
        if status:
            raise UcError(status)
        return handle.value

    def hook_del(self, handle):
        status = uclib.uc_hook_del(self.uc._uch, uc_hook_h(handle))
        if status:
            raise UcError(status)

    def set_active(self, active):
        self.lib.dp_set_active(self.tracker, int(bool(active)))

    def touch(self, address, size):
        """A host write to [address, address+size) is about to happen."""
        self.lib.dp_touch(self.tracker, self.uc._uch, address, size)

    def forget(self, page):
        self.lib.dp_forget(self.tracker, page)

    def items(self):
        """(page, content before its first write), in first-write order."""
        if self.lib.dp_failed(self.tracker):
            raise MemoryError('dirty-page tracker could not save a page')
        count = self.lib.dp_count(self.tracker)
        pages = self.lib.dp_pages(self.tracker)
        for i in range(count):
            page = pages[i]
            if page != FORGOTTEN:
                yield page, ctypes.string_at(self.lib.dp_saved(self.tracker, page), 4096)

    def __len__(self):
        return self.lib.dp_live(self.tracker)

    def __bool__(self):
        return True

    def clear(self):
        self.lib.dp_clear(self.tracker)

    def __del__(self):
        if getattr(self, 'tracker', None):
            self.lib.dp_destroy(self.tracker)
            self.tracker = None
