"""The HUD's surroundings in the booted retail world: what TPlayScreen and the
game provide to HUD panes, set up by original code where retail has it and
by explicit fixture inputs where the game would supply live state.

- `HudScene(world)`: loads a pane's archives through retail's own loader
  (`FUN_0047f670`, cdecl (name, -1, 0) -> archive blob) into the globals
  TPlayScreen::Initialize fills, and sets the sidebar width global
  (`DAT_0066615c`, 188 with the sidebar shown).
- `character(...)`: a fixture character in guest memory -- a copy of the
  real TPlayer / TCharacter vtable whose HUD input slots are answered from
  the case: current health/fatigue/mana `+0x1c0/+0x1c8/+0x1d0`, their maxima
  `+0x1d8/+0x1e0/+0x1e8`, level `+0x354`, portrait bitmap `+0x130`. Class id
  at `+4` (0x0b player / 0x0c character), name pointer at `+0x38`, the
  current action at `+0xe0` (a target is `action[0x11]` when the action
  type is 3 or 0x19). Any other slot runs retail's own method.
- `frame(pane)`: one frame of a pane the way the screen drives it -- slot 19
  (state tick), slot 20 (compose), slot 7 inside T3DScene BeginScene /
  EndScene (texture pass), slot 23 (2D pass).
- `capture()`: the display's back buffer (RGB565) and `png()` to look at it.
"""
from __future__ import annotations

import struct
import zlib
from dataclasses import dataclass, field

from unicorn.x86_const import UC_X86_REG_ECX

LOAD_ARCHIVE = 0x0047f670
SCENE, BEGIN_SCENE, END_SCENE = 0x0065a57c, 0x00416f70, 0x00416fb0
DISPLAY_POINTER, DISPLAY_BACK, SURFACE_DDRAW = 0x005d79e0, 0x8c, 0x68
G_PLAYER = 0x00667fcc
G_SIDEBAR_WIDTH = 0x0066615c
SIDEBAR_WIDTH = 188
PLAYER_VTABLE, CHARACTER_VTABLE = 0x005b4f30, 0x005a7848
PLAYER_SIZE, CHARACTER_SIZE = 0x674, 0x2a0
CLASS_PLAYER, CLASS_CHARACTER = 0x0b, 0x0c
VTABLE_COPY = 0x400                          # bytes of the real vtable copied (256 slots)
O_CLASS, O_NAME, O_ACTION = 0x04, 0x38, 0xe0
BM_RGB565 = 0x4
ARCHIVE_DATA = 0x404                         # an archive blob's dataOff table
ACTION_SIZE, ACTION_COMBAT, ACTION_TARGET = 0x64, 3, 0x44

# HUD input slots of a character: vtable offset -> field of `Stats`
STAT_SLOTS = {0x1c0: 'health', 0x1c8: 'fatigue', 0x1d0: 'mana',
              0x1d8: 'max_health', 0x1e0: 'max_fatigue', 0x1e8: 'max_mana', 0x354: 'level'}
PORTRAIT_SLOT = 0x130
SLOT_STATE, SLOT_COMPOSE, SLOT_TEXTURE_PASS, SLOT_2D_PASS = 19, 20, 7, 23


@dataclass
class Stats:
    health: int = 100
    max_health: int = 100
    fatigue: int = 100
    max_fatigue: int = 100
    mana: int = 100
    max_mana: int = 100
    level: int = 1


@dataclass
class Character:
    address: int
    name: str
    stats: Stats = field(default_factory=Stats)
    portrait: int = 0                        # guest TBitmap*, 0 = none


class HudScene:
    def __init__(self, world):
        self.world = world
        self.vm = world.vm
        self.characters: dict[int, Character] = {}
        self.archives = {}
        self.vm.put_u32(G_SIDEBAR_WIDTH, SIDEBAR_WIDTH)
        for offset, stat in STAT_SLOTS.items():
            self._stub(f'character+0x{offset:x}', lambda args, s=stat: self._stat(s))
        self._stub(f'character+0x{PORTRAIT_SLOT:x}', lambda args: self._this().portrait)

    # ---- plumbing ------------------------------------------------------

    def _stub(self, name, handler):
        self.vm.handlers[name] = (0, handler)             # thiscall, no stack arguments
        self.vm.api_dlls[name] = {'hud.fixture'}
        return self.vm.api_address('hud.fixture', name)

    def _this(self):
        return self.characters[self.vm.uc.reg_read(UC_X86_REG_ECX)]

    def _stat(self, stat):
        return getattr(self._this().stats, stat) & 0xffffffff

    def string(self, text):
        address = self.vm.allocate(len(text) + 1)
        self.vm.write(address, text.encode('cp1252') + b'\0')
        return address

    # ---- archives ------------------------------------------------------

    def load_archive(self, name, global_address=None):
        """Retail's loader; the blob pointer is also stored at the global
        TPlayScreen::Initialize keeps it in."""
        blob = self.world.call(LOAD_ARCHIVE, (self.string(name), 0xffffffff, 0),
                               instruction_limit=200_000_000)
        if not blob:
            raise RuntimeError(f'Retail loader returned nothing for {name}')
        if global_address is not None:
            self.vm.put_u32(global_address, blob)
        self.archives[name] = blob
        return blob

    def bitmap_names(self):
        """{pointer: 'Archive:Name'} over every archive loaded. An archive
        blob is `{count; nameOff[256]; dataOff[256]}`, each offset relative
        to its own slot (FUN_0046d710)."""
        names = {}
        for archive, blob in self.archives.items():
            stem = archive.rsplit('.', 1)[0]
            for i in range(self.vm.u32(blob)):
                name_slot, data_slot = blob + 4 + 4 * i, blob + ARCHIVE_DATA + 4 * i
                name = self.vm.string(name_slot + self.vm.u32(name_slot))
                names[data_slot + self.vm.u32(data_slot)] = f'{stem}:{name}'
        return names

    # ---- characters ----------------------------------------------------

    def bitmap(self, data):
        """A TBitmap's bytes as stored (the 0x48-byte header with its
        relative offsets, then the data and blocks) in guest memory -> its
        address."""
        address = self.vm.allocate(len(data))
        self.vm.write(address, data)
        return address

    @staticmethod
    def rgb565_bitmap(width, height, pixels):
        """The bytes of a 16-bit RGB565 TBitmap (flags 0x4) of `pixels`."""
        if len(pixels) != width * height * 2:
            raise ValueError('bitmap pixels must be width*height RGB565 values')
        return struct.pack('<18I', width, height, 0, 0, BM_RGB565, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           len(pixels)) + pixels

    def character(self, name, player=False, stats=None, portrait=0):
        size = PLAYER_SIZE if player else CHARACTER_SIZE
        address = self.vm.allocate(size)
        vtable = self.vm.allocate(VTABLE_COPY)
        real = PLAYER_VTABLE if player else CHARACTER_VTABLE
        self.vm.write(vtable, bytes(self.vm.uc.mem_read(real, VTABLE_COPY)))
        for offset in list(STAT_SLOTS) + [PORTRAIT_SLOT]:
            self.vm.put_u32(vtable + offset, self.vm.api_address('hud.fixture', f'character+0x{offset:x}'))
        self.vm.put_u32(address, vtable)
        self.vm.write(address + O_CLASS, struct.pack('<H', CLASS_PLAYER if player else CLASS_CHARACTER))
        self.vm.put_u32(address + O_NAME, self.string(name))
        self.characters[address] = Character(address, name, stats or Stats(), portrait)
        return self.characters[address]

    def set_player(self, character):
        self.vm.put_u32(G_PLAYER, character.address if character else 0)

    def set_target(self, player, target):
        """The player's current action: a combat action aimed at `target`,
        or none."""
        if target is None:
            self.vm.put_u32(player.address + O_ACTION, 0)
            return
        action = self.vm.allocate(ACTION_SIZE)
        self.vm.put_u32(action, ACTION_COMBAT)
        self.vm.put_u32(action + ACTION_TARGET, target.address)
        self.vm.put_u32(player.address + O_ACTION, action)

    # ---- frames --------------------------------------------------------

    def slot(self, pane, index):
        return self.vm.u32(self.vm.u32(pane) + 4 * index)

    def frame(self, pane):
        call = self.world.call
        call(self.slot(pane, SLOT_STATE), this=pane, instruction_limit=50_000_000)
        call(self.slot(pane, SLOT_COMPOSE), this=pane, instruction_limit=200_000_000)
        call(BEGIN_SCENE, this=SCENE, instruction_limit=50_000_000)
        call(self.slot(pane, SLOT_TEXTURE_PASS), this=pane, instruction_limit=200_000_000)
        call(END_SCENE, this=SCENE, instruction_limit=50_000_000)
        call(self.slot(pane, SLOT_2D_PASS), this=pane, instruction_limit=200_000_000)

    # ---- capture -------------------------------------------------------

    def screen_surface(self, pointer):
        """'display' or 'back_buffer' for the display's surfaces, else None."""
        display = self.vm.u32(DISPLAY_POINTER)
        if pointer == display:
            return 'display'
        if pointer == self.vm.u32(display + DISPLAY_BACK):
            return 'back_buffer'
        return None

    def back_buffer(self):
        """The surface retail renders frames into: the display object's
        back buffer (TDisplay `+0x8c`), whose DirectDraw surface is at
        TSurface `+0x68`."""
        back = self.vm.u32(self.vm.u32(DISPLAY_POINTER) + DISPLAY_BACK)
        return self.world.ddraw.surface(self.vm.u32(back + SURFACE_DDRAW))

    def clear(self, color=0):
        """Fill the back buffer (RGB565 `color`) so a capture holds only
        what the panes draw."""
        s = self.back_buffer()
        self.vm.write(s.memory, struct.pack('<H', color) * (s.pitch // 2 * s.height))

    def capture(self, surface=None):
        """(width, height, RGB565 rows as bytes) of a fake surface."""
        s = surface or self.back_buffer()
        rows = [bytes(self.vm.uc.mem_read(s.memory + y * s.pitch, s.width * 2)) for y in range(s.height)]
        return s.width, s.height, rows


def png(width, height, rows):
    """RGB565 rows -> PNG bytes (8-bit RGB, bit-replicated)."""
    raw = bytearray()
    for row in rows:
        raw.append(0)
        for (value,) in struct.iter_unpack('<H', row):
            r, g, b = value >> 11, (value >> 5) & 0x3f, value & 0x1f
            raw += bytes(((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)))

    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(bytes(raw), 6)) + chunk(b'IEND', b''))
