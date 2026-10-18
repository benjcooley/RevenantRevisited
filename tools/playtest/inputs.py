"""Compose a step's input events (docs/playtest/PROTOCOL.md §2).

Controls are pressed with the keys the game itself reads: `Bindings` parses
the `[Controls]` section of the Revenant.ini in use, with the key names of
the control mapper (src/game/ui/ctrlmap.cpp: `CTRL-BS`, `PGUP`, `NUM5`, ...).

    plan = Plan(state)                       # state: the pointer and held keys so far
    t = plan.press(0, bindings['CombatSwing'])
    plan.click(t + 2, 412, 233)
    host.step(plan.ticks(8), plan.events())
    state = plan.state                       # carried into the next step
"""
from __future__ import annotations

import configparser
from dataclasses import dataclass, field, replace
from pathlib import Path

from rec import BUTTON_LEFT, BUTTON_MIDDLE, BUTTON_RIGHT, SCREEN, VK, vk_code

# The control mapper's key names that differ from the VK names.
_INI_NAMES = {
    'BS': 'BACK', 'CTRL': 'CONTROL', 'ALT': 'MENU', 'PGUP': 'PRIOR', 'PGDN': 'NEXT',
    'INS': 'INSERT', 'DEL': 'DELETE', 'CAPS': 'CAPITAL', 'NUMMULT': 'MULTIPLY',
    'NUMADD': 'ADD', 'NUMSUB': 'SUBTRACT', 'NUMDEC': 'DECIMAL', 'NUMDIV': 'DIVIDE',
    **{f'NUM{n}': f'NUMPAD{n}' for n in range(10)},
}
_MODIFIERS = ('CONTROL', 'SHIFT', 'MENU')

# Keys a typed character comes from, for the key down/up around its WM_CHAR.
_CHAR_KEYS = {' ': 'SPACE', '\r': 'RETURN', '\b': 'BACK', '\x1b': 'ESCAPE',
              '-': 'OEM_MINUS', '.': 'OEM_PERIOD', ',': 'OEM_COMMA', "'": 'OEM_7'}


@dataclass(frozen=True)
class Chord:
    """Keys pressed together: modifiers first, the main key last."""
    keys: tuple[str, ...]

    def __str__(self) -> str:
        return '+'.join(self.keys)


def chord(text: str) -> Chord:
    """`A`, `CTRL-A`, `CTRL-BS` (a control mapper name) or `CONTROL+A`."""
    parts = text.replace('+', '-').upper().split('-')
    keys = tuple(_INI_NAMES.get(p, p) for p in parts if p)
    for key in keys:
        vk_code(key)
    if not keys:
        raise ValueError(f'empty chord {text!r}')
    return Chord(keys)


class Bindings:
    """Control name -> its first keyboard chord, from `[Controls]`. Joystick
    and unbound entries are skipped."""

    def __init__(self, controls: dict[str, Chord]):
        self.controls = controls

    @classmethod
    def from_ini(cls, path: Path, defaults: Path | None = None) -> Bindings:
        """The INI's bindings over `defaults`' (the shipped Revenant.ini): an
        INI without a `[Controls]` section plays with the shipped keys, as
        the game does."""
        controls = cls._read(defaults) if defaults else {}
        controls.update(cls._read(path))
        return cls(controls)

    @staticmethod
    def _read(path: Path) -> dict[str, Chord]:
        ini = configparser.ConfigParser(interpolation=None, strict=False)
        ini.optionxform = str
        ini.read_string(Path(path).read_text(encoding='cp1252'))
        if not ini.has_section('Controls'):
            return {}
        controls = {}
        for name, value in ini.items('Controls'):
            for text in value.split(','):
                text = text.strip()
                if not text or 'JOY' in text.upper():
                    continue
                try:
                    controls[name] = chord(text)
                except ValueError:
                    continue
                break
        return controls

    def __getitem__(self, control: str) -> Chord:
        try:
            return self.controls[control]
        except KeyError:
            raise KeyError(f'control {control!r} has no keyboard binding') from None

    def __contains__(self, control: str) -> bool:
        return control in self.controls


@dataclass(frozen=True)
class InputState:
    """What the game has been told so far: the pointer and the keys down."""
    x: int = SCREEN[0] // 2
    y: int = SCREEN[1] // 2
    buttons: int = 0
    held: frozenset[str] = frozenset()


@dataclass
class Plan:
    """The events of one step, at ticks relative to its start, kept in the
    order they were added within a tick."""
    state: InputState = field(default_factory=InputState)
    _events: list[tuple[int, int, dict]] = field(default_factory=list)

    # -- keys --
    def key(self, t: int, name: str, down: bool) -> None:
        vk_code(name)
        held = self.state.held | {name} if down else self.state.held - {name}
        self.state = replace(self.state, held=held)
        self._add(t, {'key': name, 'down': down})

    def hold(self, t: int, keys: Chord) -> None:
        for name in keys.keys:
            self.key(t, name, True)

    def release(self, t: int, keys: Chord) -> None:
        for name in reversed(keys.keys):
            self.key(t, name, False)

    def press(self, t: int, keys: Chord, hold: int = 2) -> int:
        """Down at `t`, up `hold` ticks later; returns the tick after."""
        self.hold(t, keys)
        self.release(t + hold, keys)
        return t + hold + 1

    def release_all(self, t: int) -> None:
        for name in sorted(self.state.held, key=lambda n: n in _MODIFIERS):
            self.key(t, name, False)

    # -- text --
    def char(self, t: int, code: int) -> None:
        if not 0 < code < 0x100:
            raise ValueError(f'not a Windows-1252 code: {code}')
        self._add(t, {'char': code})

    def type(self, t: int, text: str, gap: int = 1) -> int:
        """Each character as Windows delivers it: its key down, the
        character, its key up; one character every `gap` ticks."""
        for ch in text:
            code = ch.encode('cp1252')[0]
            key = _CHAR_KEYS.get(ch) or (ch.upper() if ch.upper() in VK else None)
            shifted = ch.isupper()
            if shifted:
                self.key(t, 'SHIFT', True)
            if key:
                self.key(t, key, True)
            self.char(t, code)
            if key:
                self.key(t, key, False)
            if shifted:
                self.key(t, 'SHIFT', False)
            t += gap
        return t

    # -- pointer --
    def move(self, t: int, x: int, y: int) -> None:
        if not (0 <= x < SCREEN[0] and 0 <= y < SCREEN[1]):
            raise ValueError(f'pointer ({x}, {y}) is off the 640x480 screen')
        self.state = replace(self.state, x=x, y=y)
        self._pointer(t)

    def button(self, t: int, button: int, down: bool) -> None:
        if button not in (BUTTON_LEFT, BUTTON_RIGHT, BUTTON_MIDDLE):
            raise ValueError(f'unknown button {button}')
        buttons = self.state.buttons | button if down else self.state.buttons & ~button
        self.state = replace(self.state, buttons=buttons)
        self._pointer(t)

    def click(self, t: int, x: int, y: int, button: int = BUTTON_LEFT, hold: int = 1) -> int:
        """Move there at `t`, press at `t + 1`, release `hold` ticks later
        (a press and its move in one tick would arrive in one message
        burst; a tick between them is how a hand clicks). Returns the tick
        after the release."""
        self.move(t, x, y)
        self.button(t + 1, button, True)
        self.button(t + 1 + hold, button, False)
        return t + hold + 2

    # -- output --
    def events(self) -> list[dict]:
        return [dict(event, t=t) for t, _, event in sorted(self._events, key=lambda e: (e[0], e[1]))]

    def ticks(self, at_least: int = 1) -> int:
        """A step length that covers every event."""
        last = max((t for t, _, _ in self._events), default=-1)
        return max(at_least, last + 1)

    def _pointer(self, t: int) -> None:
        self._add(t, {'mouse': [self.state.x, self.state.y], 'buttons': self.state.buttons})

    def _add(self, t: int, event: dict) -> None:
        if t < 0:
            raise ValueError('events are relative to the step: t >= 0')
        self._events.append((t, len(self._events), event))
