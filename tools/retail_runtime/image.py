"""Save a Runtime to disk and load it back: a booted game in seconds.

`save(vm, path, extra)` writes every mapped guest page with its protection,
the CPU context, the core's host state (heap position, clock, handles,
import stubs, scheduler threads and objects, heaps, virtual memory
reservations, process inputs) and `extra` (a fixture's own picklable host
state). Files whose contents are immutable `bytes` are read-only mounts:
only their names are stored, and the caller mounts them again before use
(`load` checks every name is present). Files the guest created or changed
(`bytearray`) are stored.

`load(executable, path, mount)` builds a fresh Runtime for the same
executable (SHA checked), calls `mount(vm)` to mount the read-only files,
maps and fills the saved regions, then restores the state. Handlers are
code, so they are not stored: the caller installs its handlers again
before running guest code (the core's own come with the fresh Runtime).
A load leaves no checkpoint; the caller takes one.
"""
from __future__ import annotations

import hashlib
import pickle
import zlib
from pathlib import Path

from runtime import Runtime

FORMAT = 1
CORE_ATTRS = ('heap_next', 'ticks', 'tick_rate', 'sleep_ms', 'rng_seed', 'handles', 'next_handle',
              'file_handle_limit', 'surfaces', 'messages', 'windows', 'timers', 'next_timer',
              'stubs', 'code_ranges', 'call_sp', 'api_dlls')


def save(vm, path, extra=None):
    regions = [(begin, end, perms, bytes(vm.uc.mem_read(begin, end - begin + 1)))
               for begin, end, perms in vm.uc.mem_regions()]
    state = dict(
        format=FORMAT,
        executable_sha256=hashlib.sha256(vm.image).hexdigest(),
        regions=regions,
        cpu=vm.uc.context_save(),
        core={name: getattr(vm, name) for name in CORE_ATTRS},
        read_only_files=sorted(name for name, data in vm.files.items() if isinstance(data, bytes)),
        files={name: bytes(data) for name, data in vm.files.items() if not isinstance(data, bytes)},
        scheduler=vm.scheduler.snapshot(),
        heaps=dict(heaps=vm.heaps.heaps, blocks=vm.heaps.blocks),
        virtual_memory=vm.virtual_memory.snapshot(),
        process=dict(command_line=vm.process.command_line,
                     command_pointer=vm.process.command_pointer,
                     environment_pointer=vm.process.environment_pointer,
                     standard=vm.process.standard),
        extra=extra)
    data = zlib.compress(pickle.dumps(state, protocol=pickle.HIGHEST_PROTOCOL), 1)
    Path(path).write_bytes(data)
    return len(data)


def load(executable, path, mount=None):
    state = pickle.loads(zlib.decompress(Path(path).read_bytes()))
    if state['format'] != FORMAT:
        raise ValueError(f'Image format {state["format"]}, expected {FORMAT}')
    vm = Runtime(executable)
    if hashlib.sha256(vm.image).hexdigest() != state['executable_sha256']:
        raise ValueError('Image was saved from a different executable')
    if mount is not None:
        mount(vm)
    missing = [name for name in state['read_only_files'] if name not in vm.files]
    if missing:
        raise ValueError(f'{len(missing)} read-only files not mounted, e.g. {missing[0]}')
    mapped = list(vm.uc.mem_regions())
    for begin, end, perms, data in state['regions']:
        if not any(b <= begin and end <= e for b, e, _ in mapped):
            vm.uc.mem_map(begin, end - begin + 1, perms)
        else:
            vm.uc.mem_protect(begin, end - begin + 1, perms)
        vm.uc.mem_write(begin, data)
        vm._invalidate_written_code(begin, len(data))
    for name, value in state['core'].items():
        setattr(vm, name, value)
    for name, data in state['files'].items():
        vm.files[name] = bytearray(data)
    vm.scheduler.restore(state['scheduler'])
    vm.heaps.heaps, vm.heaps.blocks = state['heaps']['heaps'], state['heaps']['blocks']
    vm.virtual_memory.next_address = state['virtual_memory']['next_address']
    vm.virtual_memory.regions = state['virtual_memory']['regions']
    for name, value in state['process'].items():
        setattr(vm.process, name, value)
    vm.uc.context_restore(state['cpu'])
    return vm, state['extra']
