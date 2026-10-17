r"""The play slot's world: the shipped game playing, one tick at a time.

Built on the HUD slot's machine (`slots/hud/hudworld.py`): its cached boot
image holds retail stopped in WinMain right after game init (0x00486e7f),
with the command line `NOSOUND`. `PlayWorld.start` continues that WinMain;
`step` runs one tick. Everything the game does is its own code; the host
answers Windows and the boundaries listed here.

Start (WinMain from 0x00486e7f):
- QUICKSTART. Retail parses its command line once, in GetParameters
  0x00483bc0 (called from WinMain before game init): `QUICKSTART[="name"]`
  sets 0x00668184 = 1 and copies the name to 0x0066603c (260 bytes, ""
  for a new game). Nothing reads either before WinMain 0x00486f8a, after
  game init, which picks the start: TPlayScreen 0x0065caf0's start mode
  (0x0047f4c0: 0 new game, 1 load by name) and the screen loop. So the
  host writes the two parsed values at the stop point, where a boot with
  `NOSOUND QUICKSTART[="name"]` would have them, and WinMain's copy of its
  command line (0x0066819c, read only by LASTOPTIONS and the crash
  report) gets the full line. The boot image stays shared with the HUD
  slot.
- Activation. Windows sends WM_ACTIVATEAPP(TRUE) when the fullscreen
  window is shown; the HUD machine records ShowWindow without messages,
  so the play slot posts it before the first frame. The WndProc stores
  wParam in 0x006582c8; TimerLoop pumps in a busy loop while it is 0
  (0x00491252) and the mouse handlers ignore clicks.
- A slot start also needs the save manager's slot list (0x0065a618).
  Retail fills it only in 0x0048d260 (the Load / Save panes, QuickSave
  and the console's loadgame call it), never in game init, and
  TPlayScreen::Initialize's start mode 1 looks the name up in it without
  a rescan (0x0047aedb): so retail's own QUICKSTART="name" from a cold
  start finds no slot and starts a new game ("Unable to load saved
  game..."). The host calls 0x0048d260(0) once before WinMain continues,
  as the title's Load Game screen does before it loads.
- Seed: retail seeds the CRT's per-thread rand (0x0058c582) once, srand
  (0x0058c575) of time() in WinMain 0x0048662d; the boot's virtual clock
  makes that seed fixed. `start(seed=N)` calls retail's srand(N) on the
  main thread before continuing, as a clock that read N would have.
- The game runs until its main thread first waits for a tick in
  TPlayScreen's TimerLoop: loading the module and the game happen inside
  TPlayScreen::Initialize, before that wait.

The tick (`step`): post the tick's input (`userinput.py`), advance the
virtual clock by 1000/24 ms, let winmm's timer thread fire retail's
callback once (`mmtimer.py`), and run every guest thread until the machine
is quiet again: the main thread waiting on the tick event (0x006687f0)
with no thread runnable. With the clock moving exactly one tick per
frame, TimerLoop's frame-rate estimate (GetTickCount deltas over five
frames, 0x0049133d..0x00491401) stays at or above 24 and its frame-skip
test (`accumulated <= 23.5`, 0x0049140d) never skips: every tick is one
drawn frame (TimerTick(1), one Flip).

Boundaries (original entries the host answers, recorded in the HUD
world's `boundary_calls`): the HUD slot's (boot log, network init).
Anything else unimplemented stops the run by name.

The `random()` draw counter: every call of the CRT rand 0x0058c582, the
game's one random stream (random() 0x00483300 draws through it when its
bounds differ, and 32 other callers draw directly), counted by a code hook
at its entry.

The game's own text, read where retail makes it (code hooks that only
read): text-bar lines at TTextBar::AddLine 0x0054d0c0 once its empty /
leading-space test passed (0x0054d0e3, the line in EBP), speech at
TCharacter::Say 0x004d0610 (thiscall: the speaker, its name at +0x38; the
text is the first argument), and the choices of a response entry: the
dialog entry constructor 0x00533f10 (thiscall, mode the third argument;
DIALOG.md §4.2) marks a response entry, whose texts (+0x5c count, +0x60,
the lines the player reads) are read once the tick is over.
They collect in `game_text` as (tick, kind, text); the server logs them
and the observation reports the text bar's.
Frames: `front_buffer` / `write_frame` (frames.py). The HUD machine's GDI
records text without drawing it; `gditext.py` draws approximate glyphs so
retail's own text compose has pixels to work on.
"""
from __future__ import annotations

import copy
import dataclasses
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE.parent / 'hud'))
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE  # noqa: E402
from unicorn.x86_const import UC_X86_REG_EBP, UC_X86_REG_ECX, UC_X86_REG_EIP, UC_X86_REG_ESP  # noqa: E402
from hudworld import HudWorld, RETAIL_INSTALL, GAME_INITIALIZED  # noqa: E402
from machine import PlayMachine  # noqa: E402
from mmtimer import TimerThread  # noqa: E402
from overlay import SaveOverlay  # noqa: E402
from userinput import UserInput  # noqa: E402
from gditext import GdiText  # noqa: E402
import frames  # noqa: E402

QUICKSTART_FLAG = 0x00668184
QUICKSTART_NAME, QUICKSTART_SIZE = 0x0066603c, 260
COMMAND_COPY = 0x0066819c
BOOT_COMMAND_LINE = 'NOSOUND'               # the HUD world's (hudworld.COMMAND_LINE)
CURRENT_SCREEN = 0x00667fd0                 # TScreen *CurrentScreen
SCREEN_FRAME = 0x48                         # TScreen +0x48: frames run (TimerLoop 0x00491454 / 0x004917f7)
PLAY_SCREEN = 0x0065caf0                    # the TPlayScreen
TICK_EVENT = 0x006687f0                     # TTimer's tick event handle
CLOCK_TICKS = 0x006687ec                    # TTimer clockticks
WM_ACTIVATEAPP = 0x1c
RAND, SRAND = 0x0058c582, 0x0058c575
SAVE_MANAGER, SCAN_SLOTS = 0x0065a618, 0x0048d260   # thiscall (multiplayer = 0): rebuild the slot list
TEXTBAR_LINE = 0x0054d0e3                   # TTextBar::AddLine past its empty-line test; EBP = text
SAY = 0x004d0610                            # TCharacter::Say(text, frames, anim, sound), thiscall
DIALOG_ENTRY = 0x00533f10                   # TDialogEntry ctor(pane, chr, mode, ..., count, texts, labels, duration)
RESPONSES = 3                               # entry mode: the player's choices
ENTRY_TEXTS, ENTRY_TEXT = 0x5c, 0x60        # dialog entry: text count, char *texts[8]
OBJ_NAME = 0x38                             # TObjectInstance: char *name
STEP_BUDGET = 4_000_000_000                 # instructions; a frame is far below this
START_BUDGET = 20_000_000_000
PEEK_STACK = 0x10000                        # the stack `peek` calls run on
SCRATCH = 0x100                             # observation's argument / result buffers


def machine_ini(machine, install=RETAIL_INSTALL):
    """The Revenant.ini a machine boots with (None: the install's own).

    - `software` (the default): the install's INI with `[Options]
      Software3D = Yes`, retail's own software renderer option (read by
      0x00484ae0 into 0x0066818c, written back by the Options screen).
      DirectDraw init then takes the NoTex path (0x006680c8 = 1): the map
      blitted in 2D, the 2D HUD, meshes by the internal rasterizer -- all
      of it retail's code, as a player without a 3D card saw it.
    - `hud`: the HUD slot's machine unchanged, a declared hybrid (texture
      overlays drawn by the internal rasterizer, docs/ui/HUD_REBUILD.md
      §3). Retail never ran that way, and it shows: its internal
      rasterizer draws the map's ARGB1555 background texture (the 768x768
      scroll buffer, TSurface format 0x20000) with its ARGB4444 span
      0x0056bc30, so the floor is noise.
    """
    if machine == 'hud':
        return None
    if machine != 'software':
        raise ValueError(f'machine {machine!r}: software or hud')
    ini = (Path(install) / 'revenant.ini').read_bytes()
    changed, count = re.subn(rb'(?im)^(Software3D\s*=\s*)\w+', rb'\1Yes', ini)
    if count != 1:
        raise ValueError('The install\'s Revenant.ini has no single [Options] Software3D line')
    return changed


class PlayWorld:
    """`PlayWorld.boot(executable)`, then `start()`, then `step()` per tick.
    A world saved with `save` comes back with `PlayWorld.load`."""

    @classmethod
    def boot(cls, executable, build=None, install=RETAIL_INSTALL, machine='software'):
        hud = HudWorld.open(executable, install, build, ini=machine_ini(machine, install))
        main = hud.vm.scheduler.threads[1]
        if main.state != 'stopped' or hud.vm.uc.reg_read(UC_X86_REG_EIP) != GAME_INITIALIZED:
            raise RuntimeError('The HUD boot image is not stopped after game init')
        return cls(hud)

    @classmethod
    def load(cls, executable, path, build=None, install=RETAIL_INSTALL, machine='software'):
        hud = HudWorld(executable, install, image_path=Path(path), build=build,
                       ini=machine_ini(machine, install))
        if 'play' not in hud.image_extra:
            raise ValueError(f'{path} is not a play slot image')
        return cls(hud, hud.image_extra['play'])

    def __init__(self, hud, state=None):
        self.hud = hud
        self.vm = vm = hud.vm
        self.hwnd = hud.windows.created[0][0]
        self.machine = PlayMachine(vm)
        self.text = GdiText(vm, hud.gdi, hud.ddraw)      # glyphs for GDI text (approximate; gditext.py)
        # A played game moves on and is saved as disk images: no in-place
        # checkpoints (their write hook halves guest speed), and the only
        # runnable thread runs without per-instruction counting (threads.py).
        vm.disable_checkpoints()
        vm.scheduler.solo = True
        vm.trace_apis = False                  # counted (vm.api_calls), not recorded call by call
        vm.heaps.reuse = True                  # freed heap blocks come back (memory.py): play never ends
        if state is None:
            # Host memory for observation, taken once before play so that
            # observing never moves the allocator the game's heap shares.
            self.peek_stack = vm.allocate(PEEK_STACK) + PEEK_STACK - 0x100
            self.scratch = vm.allocate(SCRATCH)
            self.input = UserInput(vm, self.hwnd)
            self.overlay = SaveOverlay(vm, hud.fs)
            self.timer = None                  # created at start, as winmm's thread
            self.draws = 0
            self.tick = None                   # session tick; None before start
            self.start_info = None
        else:
            self.peek_stack, self.scratch = state['peek_stack'], state['scratch']
            self.input = UserInput(vm, self.hwnd, state['input'])
            self.overlay = SaveOverlay(vm, hud.fs, state['overlay'])
            self.timer = TimerThread(vm, state=state['timer']) if state['timer'] else None
            self.draws = state['draws']
            self.tick = state['tick']
            self.start_info = state['start_info']
        self.tick_event = vm.u32(TICK_EVENT)
        self.game_text = list(state['game_text']) if state else []   # (tick, kind, text)
        self._responses = []                  # response entries built this tick
        vm.uc.hook_add(UC_HOOK_CODE, self._draw, begin=RAND, end=RAND)
        vm.uc.hook_add(UC_HOOK_CODE, self._textbar, begin=TEXTBAR_LINE, end=TEXTBAR_LINE)
        vm.uc.hook_add(UC_HOOK_CODE, self._say, begin=SAY, end=SAY)
        vm.uc.hook_add(UC_HOOK_CODE, self._entry, begin=DIALOG_ENTRY, end=DIALOG_ENTRY)

    def _draw(self, uc, address, size, user):
        self.draws += 1

    def _text(self, address):
        try:
            return self.vm.string(address, 1024)
        except ValueError:
            return '<unterminated>'

    def _textbar(self, uc, address, size, user):
        self.game_text.append((self.tick or 0, 'textbar', self._text(uc.reg_read(UC_X86_REG_EBP))))

    def _entry(self, uc, address, size, user):
        mode = self.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 12)
        if mode == RESPONSES:
            self._responses.append(uc.reg_read(UC_X86_REG_ECX))

    def _note_responses(self):
        for entry in self._responses:
            count = min(self.vm.u32(entry + ENTRY_TEXTS), 8)
            texts = struct.unpack(f'<{count}I', self.vm.uc.mem_read(entry + ENTRY_TEXT, 4 * count))
            self.game_text.append((self.tick or 0, 'choices',
                                   ' | '.join(f'{i + 1}. {self._text(t)}' for i, t in enumerate(texts))))
        self._responses.clear()

    def _say(self, uc, address, size, user):
        text = self.vm.u32(uc.reg_read(UC_X86_REG_ESP) + 4)
        if text:
            name = self.vm.u32(uc.reg_read(UC_X86_REG_ECX) + OBJ_NAME)
            speaker = self._text(name) if name else '?'
            self.game_text.append((self.tick or 0, 'say', f'{speaker}: {self._text(text)}'))

    # ---- state ----------------------------------------------------------

    def state(self):
        return dict(peek_stack=self.peek_stack, scratch=self.scratch,
                    input=self.input.state(), overlay=self.overlay.state(),
                    timer=self.timer.state() if self.timer else None, draws=self.draws,
                    tick=self.tick, start_info=self.start_info, game_text=list(self.game_text))

    def save(self, path):
        """The whole machine to disk (image.py), the play state with it."""
        return self.hud.save(Path(path), more=dict(play=self.state()))

    # ---- start ----------------------------------------------------------

    def start(self, slot=None, seed=None):
        """Continue WinMain into play: a new game, or the save `slot`
        (its name in Save\\Single). Returns once the play screen waits for
        its first tick."""
        if self.tick is not None:
            raise RuntimeError('Already started')
        vm = self.vm
        name = (slot or '').encode('cp1252')
        if len(name) >= QUICKSTART_SIZE or b'"' in name:
            raise ValueError(f'Slot name {slot!r} does not fit QUICKSTART="name"')
        vm.put_u32(QUICKSTART_FLAG, 1)
        vm.write(QUICKSTART_NAME, name + b'\0')
        line = BOOT_COMMAND_LINE + (f' QUICKSTART="{slot}"' if slot else ' QUICKSTART')
        vm.write(COMMAND_COPY, line.encode('cp1252') + b'\0')
        if slot:
            self._main_thread_call(SCAN_SLOTS, (0,), this=SAVE_MANAGER)
        if seed is not None:
            self._main_thread_call(SRAND, (seed & 0xffffffff,))
        vm.post_message(self.hwnd, WM_ACTIVATEAPP, 1, 0, *self.input.cursor)
        self.timer = TimerThread(vm, self.hud.system.timers)
        self._run(START_BUDGET)
        self._note_responses()
        if vm.u32(CURRENT_SCREEN) != PLAY_SCREEN:
            raise RuntimeError(f'Start reached screen 0x{vm.u32(CURRENT_SCREEN):08x}, not the play screen')
        self.tick = 0
        self.draws = 0                         # draws count from play's tick 0
        self.start_info = dict(slot=slot, seed=seed, command_line=line)

    def _main_thread_call(self, address, args, this=None):
        """Call a retail function on the main thread while WinMain is
        stopped (before start), keeping WinMain's context."""
        vm = self.vm
        main = vm.scheduler.threads[1]
        if main.state != 'stopped':
            raise RuntimeError('The main thread is not stopped')
        context = vm.uc.context_save()
        vm.call(address, args, this=this, instruction_limit=100_000_000)
        vm.uc.context_restore(context)
        main.context, main.state = context, 'stopped'

    # ---- the tick -------------------------------------------------------

    def step(self, events=()):
        """One tick: `events` (userinput.deliver's) first, then the timer."""
        if self.tick is None:
            raise RuntimeError('Start first')
        self.input.deliver(events)
        self.vm.advance(1)
        self.timer.fire()
        self._run(STEP_BUDGET)
        self._note_responses()
        self.tick += 1

    def _quiet(self):
        scheduler = self.vm.scheduler
        main = scheduler.threads[1]
        return (main.state == 'waiting' and main.wait['kind'] == 'objects'
                and main.wait['handles'] == [self.tick_event]
                and not any(scheduler.threads[tid].state == 'ready' for tid in scheduler.ready))

    def _run(self, budget):
        vm = self.vm
        try:
            vm.scheduler.resume(budget, self._quiet)
        finally:
            vm.scheduler.trace.clear()         # per-slice records would grow without bound

    # ---- reading --------------------------------------------------------

    def peek(self, address, args=(), this=None):
        """Call a retail function that only reads game state (a getter), in
        a quiet machine, without disturbing it: on the main thread's TIB with
        a stack of its own (the blocked main thread's frames stay intact),
        the main thread and scheduler put back afterwards, and every byte it
        wrote outside the host's own stack and scratch buffer put back too
        (getters write: IsEnemy lowercases names into a static buffer at
        0x00666648, TMapPane::OnObject narrows a surface's clip rect and
        widens it again). A call that makes an API call or draws a random
        number is not a getter: it fails."""
        vm = self.vm
        scheduler = vm.scheduler
        if not self._quiet():
            raise RuntimeError('peek needs a quiet machine (between ticks)')
        main = scheduler.threads[1]
        saved = (dataclasses.replace(main, wait=copy.deepcopy(main.wait), tls=dict(main.tls)),
                 vm.call_sp, list(scheduler.ready), vm.uc.context_save())
        calls, draws = vm.api_calls, self.draws
        original = {}                          # address -> its byte before the call's first write to it

        def wrote(uc, access, at, size, value, user):
            for a, byte in zip(range(at, at + size), uc.mem_read(at, size)):
                original.setdefault(a, byte)
        hooks = [vm.uc.hook_add(UC_HOOK_MEM_WRITE, wrote, begin=begin, end=end)
                 for begin, end in self._game_memory()]
        vm.call_sp = self.peek_stack
        try:
            result = vm.call(address, args, this=this, instruction_limit=100_000_000)
        finally:
            for hook in hooks:
                vm.uc.hook_del(hook)
            scheduler.threads[1], vm.call_sp, scheduler.ready, context = saved
            scheduler.current = 1
            vm.uc.context_restore(context)
            for a, byte in original.items():
                vm.uc.mem_write(a, bytes([byte]))
        if vm.api_calls != calls or self.draws != draws:
            raise RuntimeError(f'0x{address:08x} is not a getter: it made API calls or drew numbers')
        return result

    def _game_memory(self):
        """The address ranges outside the host's own peek stack and scratch
        buffer, as (first, last) pairs for Unicorn's hook ranges."""
        own = sorted([(self.peek_stack + 0x100 - PEEK_STACK, self.peek_stack + 0x100),
                      (self.scratch, self.scratch + SCRATCH)])
        ranges, start = [], 0
        for begin, end in own:
            if begin > start:
                ranges.append((start, begin - 1))
            start = max(start, end)
        ranges.append((start, 0xffffffff))
        return ranges

    @property
    def frame(self):
        """TScreen +0x48 of the current screen: TimerLoop frames run."""
        return self.vm.u32(self.vm.u32(CURRENT_SCREEN) + SCREEN_FRAME)

    def front_buffer(self):
        """The 640x480 RGB565 front buffer -- the primary surface, which
        retail presents to by blitting its back buffer (and Flip, in the
        fade's black frames) -- rows packed, 640 * 2 bytes each."""
        ddraw = self.hud.ddraw
        s = ddraw.surface(ddraw.primary)
        if s.pitch == s.width * 2:
            return bytes(self.vm.uc.mem_read(s.memory, s.pitch * s.height))
        return b''.join(bytes(self.vm.uc.mem_read(s.memory + y * s.pitch, s.width * 2))
                        for y in range(s.height))

    def textbar_lines(self, start=0):
        """The text-bar lines from the `start`-th on, as (tick, kind, text)."""
        return [entry for entry in self.game_text if entry[1] == 'textbar'][start:]

    def write_frame(self, path, level=6):
        return frames.write_png(path, self.front_buffer(), level=level)
