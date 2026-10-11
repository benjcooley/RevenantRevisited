r"""winmm's periodic timer for the play slot: retail's tick, fired by the host.

Retail's timer (TTimer, `0x0049e370..0x0049e480`; docs/TIME.md):
`timeSetEvent(41, 5, TimerCallback 0x0049e2f0, 0, TIME_PERIODIC)`. The
callback adds one to `clockticks` (0x006687ec) and to the ticks since the
last wait (0x006687e8), sets TickOccurred (0x006682a4) and pulses the
auto-reset tick event (0x006687f0). `WaitForTick` 0x0049e450 waits on that
event unless a tick has already occurred; TimerLoop 0x004911b0 waits once
per frame and clears TickOccurred on the frames it draws (0x00491766).

The HUD machine records the timer and never fires it (system.py). Here a
guest thread stands in for winmm's timer thread: it waits on an auto-reset
event the host sets once per tick, then calls the recorded callback as
winmm does (stdcall: timer id, 0, dwUser, 0, 0) and waits again. One
`fire()` is one firing at the tick's virtual time; the 41 ms period is not
used to schedule (lockstep, docs/playtest/PLAN.md §5). The thread exists
from the play slot's start, as winmm's would from timeSetEvent; its id and
handles come from the same deterministic counters as the game's own.
"""
from __future__ import annotations

import copy
import struct

INFINITE = 0xffffffff
STACK_SIZE = 0x10000


class TimerThread:
    def __init__(self, vm, timers=None, state=None):
        """`timers`: system.py's record {id: (callback, period, user)}; the
        game sets exactly one."""
        self.vm = vm
        if state is not None:
            self.set_state(state)
            return
        if len(timers) != 1:
            raise RuntimeError(f'Expected retail\'s one multimedia timer, found {len(timers)}')
        (self.timer_id, (self.callback, self.period, self.user)), = timers.items()
        scheduler = vm.scheduler
        self.event = scheduler.create_event((0, 0, 0, 0))            # auto-reset, not signaled
        wait = vm.api_address('kernel32.dll', 'WaitForSingleObject')
        self.stub = vm.allocate(64)
        code = (b'\x68' + struct.pack('<I', INFINITE)                 # top: push INFINITE
                + b'\x68' + struct.pack('<I', self.event)             # push event
                + b'\xb8' + struct.pack('<I', wait) + b'\xff\xd0'     # call WaitForSingleObject
                + b'\x6a\x00\x6a\x00'                                 # push dw2, dw1 (0)
                + b'\x68' + struct.pack('<I', self.user)              # push dwUser
                + b'\x6a\x00'                                         # push uMsg (0)
                + b'\x68' + struct.pack('<I', self.timer_id)          # push uTimerID
                + b'\xb8' + struct.pack('<I', self.callback) + b'\xff\xd0')  # call (stdcall)
        code += b'\xeb' + bytes([(-(len(code) + 2)) & 0xff])          # jmp top
        vm.write_code(self.stub, code)
        out = vm.allocate(4)
        self.handle = scheduler.create_thread((0, STACK_SIZE, self.stub, 0, 0, out))
        self.tid = vm.u32(out)
        self.fired = 0

    STATE = ('timer_id', 'callback', 'period', 'user', 'event', 'stub', 'handle', 'tid', 'fired')

    def state(self):
        return copy.deepcopy({name: getattr(self, name) for name in self.STATE})

    def set_state(self, state):
        for name, value in copy.deepcopy(state).items():
            setattr(self, name, value)

    def fire(self):
        """Let the timer thread make one call; the scheduler runs it."""
        self.vm.scheduler.set_event((self.event,))
        self.fired += 1

    @property
    def idle(self):
        """Waiting for the next firing, with none pending."""
        thread = self.vm.scheduler.threads[self.tid]
        return (thread.state == 'waiting' and thread.wait['handles'] == [self.event]
                and not self.vm.scheduler.objects[self.event]['signaled'])
