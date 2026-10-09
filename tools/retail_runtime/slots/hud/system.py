"""The machine the HUD slot's retail boot sees: fixed, deterministic answers
to the system queries WinMain makes. One documented configuration:

- Windows 98 (GetVersion comes from the core: 4.10), 64 MB RAM, a 2 GB C:
  fixed disk holding the install, no CD drive with a Revenant disk.
- Windows directory C:\\WINDOWS, system C:\\WINDOWS\\SYSTEM, temp C:\\TEMP.
- Wall clock: 1999-12-31 00:00:00 plus the virtual clock, UTC (no time
  zone, no daylight saving) -- GetLocalTime/GetSystemTime/
  GetSystemTimeAsFileTime/GetTimeZoneInformation agree.
- winmm timeBeginPeriod/timeEndPeriod accepted (the clock is virtual);
  timeSetEvent timers are recorded (`timers`: callback, period, user data)
  and never fire on their own -- the fixture advances the game itself.
- No DirectInput: DirectInputCreateA fails (E_FAIL); retail ignores the
  result and takes keyboard input from window messages.
- Locale as Windows 98's CRT sees it: GetStringTypeW / LCMapStringW fail
  with ERROR_CALL_NOT_IMPLEMENTED (as on Windows 98), so the CRT takes its
  ANSI paths: GetStringTypeA CT_CTYPE1 over cp1252 (classified from Unicode
  categories; an approximation of the Win98 tables) and LCMapStringA case
  mapping. WideCharToMultiByte ignores WC_COMPOSITECHECK / WC_SEPCHARS (the
  CRT's tzset passes them; they only affect composed characters).
- User "Player" (advapi32 GetUserNameA); ole32 CoInitialize succeeds;
  SetUnhandledExceptionFilter is accepted (no exception dispatch exists).
- MessageBoxA fails the run: retail only shows one on an error or warning
  path, and a fixture that reaches one is not a valid boot.

Values a capture could depend on are listed here, never invented elsewhere.
"""
from __future__ import annotations

import datetime
import struct
import unicodedata

TOTAL_RAM = 64 << 20
USER_NAME = b'Player'
WC_COMPOSITECHECK, WC_SEPCHARS = 0x200, 0x20
EPOCH = datetime.datetime(1999, 12, 31)
FILETIME_EPOCH = datetime.datetime(1601, 1, 1)


def case_map(byte, upper):
    """One cp1252 byte upper- or lower-cased, unchanged when the result is
    not a single cp1252 byte (length-preserving, as LCMapStringA is)."""
    try:
        char = bytes([byte]).decode('cp1252')
        mapped = (char.upper() if upper else char.lower()).encode('cp1252')
    except (UnicodeDecodeError, UnicodeEncodeError):
        return byte
    return mapped[0] if len(mapped) == 1 else byte


def ctype1(byte):
    """CT_CTYPE1 flags for a cp1252 byte (UPPER 1, LOWER 2, DIGIT 4, SPACE 8,
    PUNCT 0x10, CNTRL 0x20, BLANK 0x40, XDIGIT 0x80, ALPHA 0x100)."""
    try:
        char = bytes([byte]).decode('cp1252')
    except UnicodeDecodeError:
        return 0
    category = unicodedata.category(char)
    flags = 0
    if category == 'Lu':
        flags |= 0x1 | 0x100
    elif category == 'Ll':
        flags |= 0x2 | 0x100
    elif category in ('Lo', 'Lm'):
        flags |= 0x100
    elif category == 'Nd':
        flags |= 0x4
    elif category == 'Zs':
        flags |= 0x8 | 0x40
    elif category == 'Cc':
        flags |= 0x20
        if char in '\t\n\v\f\r':
            flags |= 0x8
        if char == '\t':
            flags |= 0x40
    elif category[0] in 'PS' or category == 'No':
        flags |= 0x10
    if char in '0123456789abcdefABCDEF':
        flags |= 0x80
    return flags


class MessageBoxReached(RuntimeError):
    pass


class SystemApis:
    def __init__(self, vm):
        self.vm = vm
        self.error_mode = 0
        self.environment = {}                 # SetEnvironmentVariableA store (upper-case names)
        self.timers = {}                      # timeSetEvent id -> (callback, period ms, user)
        self.next_timer = 1
        kernel32, winmm, user32 = {'kernel32.dll'}, {'winmm.dll', '_inmm.dll'}, {'user32.dll'}
        for name, argc, fn, dlls in [
                ('GlobalMemoryStatus', 1, self._memory_status, kernel32),
                ('SetErrorMode', 1, self._set_error_mode, kernel32),
                ('GetWindowsDirectoryA', 2, lambda a: self._path(a, 'C:\\WINDOWS'), kernel32),
                ('GetSystemDirectoryA', 2, lambda a: self._path(a, 'C:\\WINDOWS\\SYSTEM'), kernel32),
                ('GetTempPathA', 2, lambda a: self._path((a[1], a[0]), 'C:\\TEMP\\'), kernel32),
                ('GetDriveTypeA', 1, self._drive_type, kernel32),
                ('GetLogicalDrives', 0, lambda a: 0x4, kernel32),            # C:
                ('GetVolumeInformationA', 8, self._volume_information, kernel32),
                ('GetDiskFreeSpaceA', 5, self._disk_free, kernel32),
                ('lstrcpyA', 2, self._lstrcpy, kernel32),
                ('lstrcpynA', 3, self._lstrcpyn, kernel32),
                ('lstrcatA', 2, self._lstrcat, kernel32),
                ('lstrlenA', 1, lambda a: len(self.vm.string(a[0])) if a[0] else 0, kernel32),
                ('lstrcmpA', 2, lambda a: self._compare(a, False), kernel32),
                ('lstrcmpiA', 2, lambda a: self._compare(a, True), kernel32),
                ('SetEnvironmentVariableA', 2, self._set_environment, kernel32),
                ('GetEnvironmentVariableA', 3, self._get_environment, kernel32),
                ('GetLocalTime', 1, self._system_time, kernel32),
                ('GetSystemTime', 1, self._system_time, kernel32),
                ('GetSystemTimeAsFileTime', 1, self._filetime, kernel32),
                ('GetTimeZoneInformation', 1, self._time_zone, kernel32),
                ('timeBeginPeriod', 1, lambda a: 0, winmm),
                ('timeGetDevCaps', 2, self._timer_caps, winmm),
                ('timeSetEvent', 5, self._set_event, winmm),
                ('timeKillEvent', 1, self._kill_event, winmm),
                ('timeEndPeriod', 1, lambda a: 0, winmm),
                ('DirectInputCreateA', 4, self._no_dinput, {'dinput.dll'}),
                ('GetStringTypeW', 4, self._not_on_win98, kernel32),
                ('LCMapStringW', 6, self._not_on_win98, kernel32),
                ('GetStringTypeA', 5, self._string_type_a, kernel32),
                ('LCMapStringA', 6, self._map_string_a, kernel32),
                ('GetUserNameA', 2, self._user_name, {'advapi32.dll'}),
                ('CoInitialize', 1, lambda a: 0, {'ole32.dll'}),
                ('SetUnhandledExceptionFilter', 1, lambda a: 0, kernel32),
                ('MessageBoxA', 4, self._message_box, user32)]:
            vm.handlers[name] = (argc, fn)
            if dlls is not kernel32:
                vm.api_dlls[name] = dlls
        wide_to_bytes = vm.handlers['WideCharToMultiByte'][1]
        vm.handlers['WideCharToMultiByte'] = (8, lambda a: wide_to_bytes(
            (a[0], a[1] & ~(WC_COMPOSITECHECK | WC_SEPCHARS)) + tuple(a[2:])))

    # ---- lstr* (byte strings, cp1252) ----------------------------------

    def _bytes(self, pointer):
        return self.vm.string(pointer).encode('cp1252')

    def _lstrcpy(self, args):
        self.vm.write(args[0], self._bytes(args[1]) + b'\0')
        return args[0]

    def _lstrcpyn(self, args):
        dest, source, count = args
        if count:
            self.vm.write(dest, self._bytes(source)[:count - 1] + b'\0')
        return dest

    def _lstrcat(self, args):
        self.vm.write(args[0], self._bytes(args[0]) + self._bytes(args[1]) + b'\0')
        return args[0]

    def _compare(self, args, fold):
        a, b = self.vm.string(args[0]), self.vm.string(args[1])
        if fold:
            a, b = a.lower(), b.lower()
        return 0 if a == b else (0xffffffff if a < b else 1)

    def _set_environment(self, args):
        name = self.vm.string(args[0]).upper()
        if args[1]:
            self.environment[name] = self.vm.string(args[1])
        else:
            self.environment.pop(name, None)
        return 1

    def _get_environment(self, args):
        name, buffer, size = args
        value = self.environment.get(self.vm.string(name).upper())
        if value is None:
            self.vm.scheduler.set_error((203,))            # ERROR_ENVVAR_NOT_FOUND
            return 0
        return self._path((size, buffer), value)

    def _timer_caps(self, args):
        self.vm.write(args[0], struct.pack('<II', 1, 1000000))      # TIMECAPS min/max period
        return 0

    def _set_event(self, args):
        delay, _resolution, callback, user, _flags = args
        timer = self.next_timer
        self.next_timer += 1
        self.timers[timer] = (callback, delay, user)
        return timer

    def _kill_event(self, args):
        return 0 if self.timers.pop(args[0], None) is not None else 97   # MMSYSERR_INVALPARAM

    def snapshot(self):
        return dict(self.environment), self.error_mode, dict(self.timers), self.next_timer

    def restore(self, state):
        environment, self.error_mode, timers, self.next_timer = state
        self.environment, self.timers = dict(environment), dict(timers)

    def now(self):
        return EPOCH + datetime.timedelta(milliseconds=float(self.vm.elapsed_ms))

    def _system_time(self, args):
        t = self.now()
        self.vm.write(args[0], struct.pack('<8H', t.year, t.month, (t.weekday() + 1) % 7, t.day,
                                           t.hour, t.minute, t.second, t.microsecond // 1000))
        return 0

    def _filetime(self, args):
        ticks = (self.now() - FILETIME_EPOCH) // datetime.timedelta(microseconds=1) * 10
        self.vm.write(args[0], struct.pack('<Q', ticks))
        return 0

    def _time_zone(self, args):
        self.vm.write(args[0], bytes(172))                 # TIME_ZONE_INFORMATION, all UTC
        return 0                                           # TIME_ZONE_ID_UNKNOWN

    def _memory_status(self, args):
        # MEMORYSTATUS: length, load %, total/avail physical, page file, virtual.
        self.vm.write(args[0], struct.pack('<8I', 32, 20, TOTAL_RAM, TOTAL_RAM // 2,
                                           2 * TOTAL_RAM, 2 * TOTAL_RAM, 0x7ffe0000, 0x7f000000))
        return 0

    def _set_error_mode(self, args):
        previous, self.error_mode = self.error_mode, args[0]
        return previous

    def _path(self, args, value):
        size, buffer = args
        data = value.encode('cp1252')
        if size <= len(data):
            return len(data) + 1
        self.vm.write(buffer, data + b'\0')
        return len(data)

    def _drive_type(self, args):
        root = self.vm.string(args[0]).upper() if args[0] else 'C:\\'
        return 3 if root.startswith('C') else 1          # DRIVE_FIXED / DRIVE_NO_ROOT_DIR

    def _volume_information(self, args):
        root, name, name_size, serial, max_component, flags, fs_name, fs_size = args
        if root and not self.vm.string(root).upper().startswith('C'):
            self.vm.scheduler.set_error((21,))            # ERROR_NOT_READY
            return 0
        if name and name_size:
            self.vm.write(name, b'HARDDISK'[:name_size - 1] + b'\0')
        if serial:
            self.vm.put_u32(serial, 0x19991231)
        if max_component:
            self.vm.put_u32(max_component, 255)
        if flags:
            self.vm.put_u32(flags, 0x6)                    # case preserved, unicode on disk
        if fs_name and fs_size:
            self.vm.write(fs_name, b'FAT32'[:fs_size - 1] + b'\0')
        return 1

    def _disk_free(self, args):
        root, sectors, bytes_per, free_clusters, total_clusters = args
        for pointer, value in ((sectors, 8), (bytes_per, 512), (free_clusters, 262144),
                               (total_clusters, 524288)):
            if pointer:
                self.vm.put_u32(pointer, value)
        return 1

    def _user_name(self, args):
        buffer, size = args
        name = USER_NAME + b'\0'
        if self.vm.u32(size) < len(name):
            self.vm.put_u32(size, len(name))
            self.vm.scheduler.set_error((122,))      # ERROR_INSUFFICIENT_BUFFER
            return 0
        self.vm.write(buffer, name)
        self.vm.put_u32(size, len(name))
        return 1

    def _not_on_win98(self, args):
        self.vm.scheduler.set_error((120,))          # ERROR_CALL_NOT_IMPLEMENTED
        return 0

    def _string_type_a(self, args):
        _locale, info, source, count, out = args
        if info != 1:                                # CT_CTYPE1 only
            raise ValueError(f'GetStringTypeA info type {info}')
        data = bytes(self.vm.uc.mem_read(source, count)) if count != 0xffffffff else \
            self.vm.string(source).encode('cp1252') + b'\0'
        self.vm.write(out, b''.join(struct.pack('<H', ctype1(b)) for b in data))
        return 1

    def _map_string_a(self, args):
        _locale, flags, source, count, out, out_count = args
        if flags not in (0x100, 0x200):              # LCMAP_LOWERCASE / LCMAP_UPPERCASE
            raise ValueError(f'LCMapStringA flags 0x{flags:x}')
        data = bytes(self.vm.uc.mem_read(source, count)) if count != 0xffffffff else \
            self.vm.string(source).encode('cp1252') + b'\0'
        mapped = bytes(case_map(b, upper=flags == 0x200) for b in data)
        if out_count == 0:
            return len(mapped)
        if out_count < len(mapped):
            self.vm.scheduler.set_error((122,))      # ERROR_INSUFFICIENT_BUFFER
            return 0
        self.vm.write(out, mapped)
        return len(mapped)

    def _no_dinput(self, args):
        if args[2]:
            self.vm.put_u32(args[2], 0)
        return 0x80004005                                  # E_FAIL

    def _message_box(self, args):
        _, text, caption, _flags = args
        raise MessageBoxReached(f'MessageBoxA "{self.vm.string(caption) if caption else ""}": '
                                f'{self.vm.string(text) if text else ""}')
