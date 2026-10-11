r"""The observation and the check lines (docs/playtest/PROTOCOL.md §3, §7),
read from retail memory.

Every field cites where retail keeps it; a field that is not verified here
is left out, never guessed. Sources (docs/gameplay/forensics, gameflow/
forensics, ui/forensics, recon/discovered): COMBAT_ATTACK_CHOICE.md §2.4
(ACH), COMBAT_HIT.md (HIT), PLAYER_STATS.md (PS), SCREEN_SYSTEM.md (SCR),
INGAME_MENU.md (IGM), DIALOG.md (DLG), INVENTORY.md (INV), MAP_INPUT.md
(MI), TTextBar_SPEC.md (TB), COMBAT_DOJO.md (DOJO), COMBAT_MOTION.md
(CM), InventoryPane_SPEC.md (IPS), docs/playtest/CLICKWALK.md (CW),
recon/docs/OBJCLASS_IDS.md; addresses without a source were read in the
disassembly of the baseline.

Reads are plain memory reads, except the getters retail itself uses for
values it computes (maximum health / fatigue / mana, weapon type, radius,
gold, an item's stats, whether a character takes Locke for an enemy, the
object under the pointer, the panes' click tests), called through
`PlayWorld.peek`, which refuses anything that is not a getter. Observing
never changes the game.
"""
from __future__ import annotations

import math
import struct

# ---- globals ---------------------------------------------------------------
PLAYER = 0x00667fcc               # TPlayer *Player; SetMainPlayerNum 0x0051eef0 writes it (0: none)
CURRENT_SCREEN = 0x00667fd0       # TScreen *CurrentScreen (SCR §2.1)
SCREENS = {0x0065caf0: 'play', 0x0065d358: 'title', 0x0066f680: 'death',
           0x0066fa78: 'menu', 0x0066fe88: 'menu', 0x00659bd8: 'other'}   # SCR §2.5
PLAY_SCREEN = 0x0065caf0
CONTROL = PLAY_SCREEN + 0x5e0     # 1 while the player has control; SetControl 0x0047c580 (IGM)
INGAME_MENU_OPEN = 0x0066f748 + 0x40   # the in-game menu pane's open flag (IGM §4)
DIALOG_PANE = 0x00667cc8          # TDialogPane (DLG §4.1): entries +0x17c count / +0x18c data,
                                  # the response entry +0x190 (non-null while choices show)
ENTRY_SPEAKER, ENTRY_MODE, ENTRY_TEXTS, ENTRY_TEXT = 0x04, 0x08, 0x5c, 0x60   # dialog entry (DLG §4.2); mode 1 NPC
STAT_IDS = dict(health=0x0066ca4c, fatigue=0x0066ca3c, mana=0x0066ca38)  # globals holding stat ids (HIT)
LEVEL_STAT, EXP_STAT, NEXTEXP_STAT = 17, 18, 19      # PS §1 (src/charstats.h order, retail-confirmed)
ATTACKLEVEL_STAT = 20             # AttackLevel, the gate on an attack's attackskill (ACH §3.1 item 19, 0x004d1654)
ATTRIBUTE_STATS = dict(strn=34, cons=35, agil=36, rflx=37, mind=38, luck=39)   # PS §1
SKILL_NAMES = ('attack', 'defense', 'invoke', 'hands', 'knife', 'sword', 'bludgeon', 'axes', 'bows',
               'stealth', 'lockpick')                 # stats 40-50 in order (PS §1, class.def:2904-2914)
SKILL_STAT, SKILL_EXP_STAT, SKILL_NEXT_STAT = 40, 51, 62   # each skill, its experience, the next level's (PS §1)
WEAPON_SKILL_BASE = 43                                # skill of the weapon in hand: 43 + WeaponType (ACH item 19)
SECTOR_COUNT, SECTORS = 0x00668578, 0x00668588       # sector pool (AW, recon/classes/cls_0x44ceb0.cpp)
SECTOR_LEVEL, SECTOR_X, SECTOR_Y, SECTOR_WALKMAP = 0x04, 0x08, 0x0c, 0x00   # as FindSector 0x00499e10 matches;
                                  # the walkmap: 64 x 64 uint16, height in the low 10 bits (0x00499720)
WALK_SHIFT, SECTOR_SHIFT = 4, 6   # a walk cell is 16 units, a sector 64 cells (GetWalkHeight 0x00452e10)
SECTOR_MASK, SECTOR_CELLS, WALK_HEIGHT_MASK = (1 << SECTOR_SHIFT) - 1, 1 << 2 * SECTOR_SHIFT, 0x3ff
MAP_PANE = 0x006668d8             # TMapPane: rect +0x04..+0x10, scroll origin +0x78 / +0x7c (MI §1)
MAP_SCROLL_X, MAP_SCROLL_Y = 0x78, 0x7c
WORLD_TO_SCREEN = 0x0046d7a0      # cdecl (const S3DPoint*, int *sx, int *sy): map-screen pixels
IS_ENEMY = 0x004c89c0             # TCharacter::IsEnemy(TCharacter *other), thiscall (src/character.cpp)
CURSOR_X, CURSOR_Y = 0x00668510, 0x00668514   # the pointer, polled each screen pass (CW §3.3)
PICK_X, PICK_Y, PICK = 0x005e91cc, 0x005e91d0, 0x005e91d4   # the 3D pick and the point it was made at:
                                  # every DrawScene picks at the pointer (MI §2.1)
ON_OBJECT = 0x00452520            # TMapPane::OnObject(x, y, with), thiscall: the pick a click acts on (MI §2)
WALK_HEIGHT = 0x00452e10          # GetWalkHeight(const S3DPoint*, level, int *flags) (test cross-checks)

# ---- mouse routing (a button message: the dispatcher 0x0048f870, TPlayScreen::
# MouseClick 0x0047ce80, TScreen::MouseClick 0x004903b0, a pane's MouseClick) --------
CLICKS_OFF = 0x006682bc           # set: the dispatcher drops every click (0x0048f898, 0x004903b0)
SCREEN_PANE_COUNT, SCREEN_PANES = 0x04, 0x14   # TScreen: its panes, in the order a click is offered them
EXCLUSIVE_DEPTH = 0x1c            # TScreen: exclusive panes (SCR §2.3); the top one, n = depth, is
EXCLUSIVE_PANE, EXCLUSIVE_FLAGS = 0x1c, 0x2c   # its pane index at +0x1c + 4n, its flags at +0x2c + 4n
EXCLUSIVE_MOUSE = 0x01            # the top entry holds the mouse: only its pane gets clicks (0x004903f0)
FADE_IN, FADE_OUT = 0x40, 0x44    # TScreen faders (SCR §2.6), flags +0x04
FADE_FLAGS, FADE_HOLDS_MOUSE = 0x04, 0x08
VT_FADE_IN_DONE, VT_FADE_OUT_DONE, VT_FADING = 0x20, 0x24, 0x18   # 0x00491c50, 0x00491c70, 0x0046cf80
PAUSED, DEMO = 0x5d4, 0x5d8       # TPlayScreen: paused drops every click (0x0047ce83); demo mode (IGM §2.1)
PLAYER_SCRIPT, SCRIPT_TRIGGER = 0x84, 0x1c     # the player's running script and its trigger
FLAG_NAMED, PAUSE = 0x00472e30, 0x005d7694     # thiscall (name): the object's flag (+0x08) of that name in
                                  # the flag names 0x005d4770; "PAUSE"
PANE_X, PANE_Y, PANE_W, PANE_H = 0x04, 0x08, 0x0c, 0x10   # TPane rect, screen pixels
PANE_NO_CLICKS = 0x44             # TPane: set, it is offered no clicks (0x004904a2, 0x00490410)
VT_PANE_HIDDEN, VT_PANE_SHUT = 0x3c, 0x44     # TPane slots 15 / 17 (0x00445000: hidden, +0x48, or not on
                                  # the current screen; 0x00445020: +0x4c); either, no clicks (0x00490495, 0x004904ab)
SIDE_TABS = 0x0065be50            # TSideTabsPane: its buttons keep their clicks from the map (0x0044f17b)
BUTTON_AT = 0x004364d0            # TButtonPane: whether a button holds pane point (x, y), thiscall
EDITOR = 0x00668154               # set: the map pane's clicks go to the editor (0x0044f188)

# ---- the side panel's inventory (IPS) ------------------------------------------
INVENTORY_PANE = 0x0065d4f8       # TInventory (vtable 0x5a58c0), a TPane: rect (452, 306) 188 x 174 (IPS §3)
INV_CONTAINER, INV_FIRST = 0x17c, 0x188   # the container it shows; the first slot in view
INV_LEFT, INV_TOP, INV_DX, INV_DY = 8, 0x2a, 0x2d, 0x2c   # cell (0, 0) at pane (8, 42), every 45 x 44,
INV_CELL = 0x28                   # 40 x 40; slot = first + column * 3 + row: DrawGrid 0x00537cfc..0x00537d0e,
                                  # the right click's hit test 0x0053835e..0x005383c7 (IPS §4)
INV_COLUMNS, INV_ROWS = 4, 3

# ---- object layout (TObjectInstance / TCharacter / TPlayer) -----------------
OBJ_CLASS, OBJ_TYPE, OBJ_FLAGS, OBJ_LEVEL = 0x04, 0x06, 0x08, 0x0e   # int16, uint16, uint32, uint16
OBJ_POS, OBJ_FACING, OBJ_NAME, OBJ_ID = 0x10, 0x36, 0x38, 0x40       # int32 x3, uint8, char*, int32
OBJ_TYPEINFO = 0x4c               # its first field is the type's name
OBJ_STAT_COUNT, OBJ_STATS = 0xa8, 0xac   # int16 count, int32 *values (TCharacter GetObjStat 0x004d7520)
PLAYER_STAT_COUNT, PLAYER_STATS = 0x34c, 0x350   # int16, {id, value} pairs (TPlayer GetObjStat 0x0051ae30)
DOING, ROOT = 0xd8, 0xe0          # TActionBlock* (ACH §2.4)
BLOCK_ACTION, BLOCK_NAME, BLOCK_OBJ, BLOCK_ATTACK = 0x00, 0x04, 0x44, 0x48   # ACTION, char[32], obj, SCharAttackData*
ACTION_COMBAT, ACTION_ATTACK, ACTION_BOW = 3, 7, 0x19
LAST_ATTACK, LAST_ATTACK_FRAME = 0x160, 0x164   # TCharacter: the attack last started, the GameFrame it started (ACH §2.4)
GAME_FRAME = (0x680, 0x688, 0x684)      # TPlayScreen::GameFrame 0x0047e920: +0x680 + +0x688 - +0x684
EQUIPMENT, EQUIPMENT_SLOTS = 0x2a0, 11   # TPlayer equipment[EQ_*] (CW §1.2: the equip pane's held item)
EQUIPMENT_HAND = EQUIPMENT + 4 * 4       # the prime hand, EQ_PRIMEHAND (ACH)
INVENTORY_COUNT, INVENTORY = 0x68, 0x78   # TPointerArray of the items held (INV §2)
ITEM_SLOT = 0x7c                  # int16: 0..0xfe pack, 0x100.. equipment, 0x10b.. belt (INV §2)
BELT_FIRST, BELT_SLOTS = 0x10b, 5
LEVELUP = 0x360                   # TPlayer: the two attributes the next level-up raises (0 Strn .. 5 Luck)
                                  # and the cursor the train buttons write through (0x0051fe50:
                                  # 0x0051fe6a..0x0051fea1; PS §6)
STAT_EFFECT_COUNT, STAT_EFFECTS = 0x358, 0x35c   # TPlayer: int16 count, {char *line, int expiry} pairs
                                  # (RemoveStatEffect 0x0051d4a0: 0x0051d4b3..0x0051d4c2; PS §4)
PLAY_CLOCK = 0x370                # TPlayer: the play clock effects expire by, 1/100 s (vtable +0x314
                                  # 0x0051d800; PS §5)
VT_OFFENSE, VT_DEFENSE = 0x2c4, 0x2c0      # TPlayer vtable 0x005b4f30: Offense 0x0051a520, Defense 0x0051a550
VT_ARMOR, VT_WEAPON_DAMAGE = 0x2bc, 0x2d0  # ArmorValue 0x00519850, WeaponDamage 0x00520830 (GRIND §8)
VT_AMOUNT = 0x198                 # Amount(): 1, or the Amount stat of money, food, potions, ammo (INV §1;
                                  # TFood's 0x0050ed70)
OBJ_STATE = 0x0c                  # uint16 state (EXITS.md §1.2): TContainer::Use 0x004dc180 opens a
                                  # container in state 0 (0x004dc22a) with SetState(1) (0x004dc2f5)
VT_OPENABLE, VT_LOCKED = 0x1f0, 0x208      # TContainer::Use asks them (0x004dc21c, 0x004dc1d5)
CLASS_CONTAINER = 5
VALUE = 0x005e2874                # "Value": the kill formula asks a victim GetStat("Value") (0x0051a783)
ITEM_NUMBERS = {1: ('Type', 'Damage', 'MinStrength', 'Value'),           # weapon
                2: ('Protection', 'MinStrength', 'MinConstitution', 'Value'),   # armor (EqSlot: `eqslot`)
                4: ('Health', 'Mana', 'Poison'),                         # food
                18: ('Health', 'Mana', 'Poison')}                        # potion (GRIND §8)
NAME_SCRATCH = 0x40               # where get_stat puts the name it asks, past project()'s point
EQUIP_PANE = 0x0065b7e0           # TEquipPane, the side panel's upper slot (EquipPane_SPEC §0)
EQUIP_CELLS, EQUIP_CELL = 0x005e3f60, 0x28   # 11 {x, y} pane-local cell corners by EQ_* slot, 40 x 40:
                                  # MouseClick 0x005363e0 tests a press against them (0x005363fc..0x00536437)
                                  # and takes equipment[i] (0x0053644d)
BAR_INV_PANE = 0x0065b028         # TBarInvPane, the belt's boxes on the bottom bar (BarInvPane_SPEC §3)
BAR_FIRST, BAR_COUNT = 0x88, 0x8c # the first belt slot shown (Init 0x0052c970 sets 0), the boxes shown
                                  # ((w - 0xdc) / 0x2d)
BAR_LEFT, BAR_PITCH, BAR_TOP = 0xdc, 0x2d, 0x0a   # box n at pane (0xdc + 0x2d n, 0xa), 40 tall: the
                                  # hover's hit test in Animate 0x0052cd80 (0x0052cda6..0x0052ce02) and the
                                  # click's in 0x0052cfb0 (0x0052d007..0x0052d051)
OBJ_CLASS_DEF = 0x48              # TObjectClass*: its stat definitions, the object stats then the type
CLASS_OBJ_STATS, CLASS_TYPE_STATS = 0x1c, 0x14  # stats, each an int16 count and (+4) entries of 0x50 bytes,
STAT_DEF_SIZE = 0x50              # the name first, as GetStat(name) 0x00473600 searches them (0 for a name
                                  # neither holds, 0x004736df)
VT_GET_STAT = 0xd4                # GetStat(name) 0x00473600
EQSLOT = 0x005e41f4               # "EqSlot": the inventory pane's right click equips by it (0x005383e6)
VT_RADIUS = 0x258                 # Radius() 0x004d6e40: what FindClearPath tests a step over (CM §3.9:
                                  # 0x004c3a3f, then GetWalkHeightRadius 0x004c3a51)
VT_MAX = dict(health=0x1d8, fatigue=0x1e0, mana=0x1e8)   # MaxHealth / MaxFatigue / MaxMana (HIT, HUD)
VT_WEAPON_TYPE = 0x2cc            # WeaponType (ACH: player 0x00520810)
VT_INVENTORY_AMOUNT = 0x84        # GetInventoryAmount(name) 0x0046fde0, nested (INVENTORY.md §2)
GOLD = 0x005e41a0                 # "Gold", the string the inventory pane asks with (0x00537b3b)
QUICK_SPELLS = 0x2cc              # TPlayer: char[6] talisman codes, slot 0 the cursor, 1-4 the buttons
SPELL_LIST = 0x00667c38           # TSpellList: +4 pointer array of SSpellData (count +0, data +0x10)
SPELL_VARIANTS = 0x00              # SSpellData: the variants pointer array
VARIANT_NAME, VARIANT_TALISMANS = 0x04, 0x24   # SSpellVariant: char[32] spell.def VARIANT name, char[6]
                                               # (src/spell.h; slots/combat/spell_guest.py)
CLASS_NAMES = ['item', 'weapon', 'armor', 'talisman', 'food', 'container', 'lightsource', 'tool',
               'money', 'tile', 'exit', 'player', 'character', 'trap', 'shadow', 'helper', 'key',
               'invcontainer', 'potion', None, None, 'ammo', 'scroll', 'rangedweapon', None,
               'effect', 'mapscroll']     # recon/docs/OBJCLASS_IDS.md (retail 0x00483850)
CLASS_PLAYER, CLASS_CHARACTER, CLASS_HELPER = 11, 12, 15
NOT_LISTED = {9, 14, 25}          # tiles, shadows, effects
FLAG_NAMES = 0x005d4770           # char *[32]: object flag bit n's name, as `toggle <name>` looks it up
OF_INVISIBLE, OF_EDITOR = 0x80, 0x100   # bits 7 "INVISIBLE", 8 "EDITOR": outside the editor the 3D pick
                                  # (MI §2.1) and TCharAnimator::Render 0x004d7a50 skip them
OF_INVENTORY = 0x40000000        # an item, carried or not: the flag the item classes' constructors set,
                                  # all IsInventoryItem 0x00477c00 tests (an item dropped on the map keeps it)
MISSING = object()                # a value retail can't give without changing the game: left out


def signed(value):
    return value - (1 << 32) if value & 0x80000000 else value


def c_div(a, b):
    """Integer division as C's: toward zero."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


class Memory:
    def __init__(self, vm):
        self.vm = vm

    def u32(self, address):
        return self.vm.u32(address)

    def i32(self, address):
        return struct.unpack('<i', self.vm.uc.mem_read(address, 4))[0]

    def i16(self, address):
        return struct.unpack('<h', self.vm.uc.mem_read(address, 2))[0]

    def u16(self, address):
        return struct.unpack('<H', self.vm.uc.mem_read(address, 2))[0]

    def u8(self, address):
        return self.vm.uc.mem_read(address, 1)[0]

    def text(self, address, limit=256):
        if not address:
            return None
        data = bytes(self.vm.uc.mem_read(address, limit))
        return data.split(b'\0', 1)[0].decode('cp1252')

    def pointers(self, array, count):
        return struct.unpack(f'<{count}I', self.vm.uc.mem_read(array, 4 * count)) if count else ()


class Observer:
    """The observation of one world. `state()` carries what persists between
    observations: how many of the world's text-bar lines were reported."""

    def __init__(self, world, state=None):
        self.world = world
        self.m = Memory(world.vm)
        self.reported = (state or {}).get('reported', 0)

    def state(self):
        return dict(reported=self.reported)

    # ---- objects ------------------------------------------------------

    def stat(self, obj, stat):
        """GetObjStat: the player's own copy for the player (equipment and
        effects applied), the raw array for anyone else."""
        m = self.m
        if m.i16(obj + OBJ_CLASS) == CLASS_PLAYER:
            count, values = m.i16(obj + PLAYER_STAT_COUNT), m.u32(obj + PLAYER_STATS)
            return m.i32(values + stat * 8 + 4) if 0 <= stat < count else 0
        count, values = m.i16(obj + OBJ_STAT_COUNT), m.u32(obj + OBJ_STATS)
        return m.i32(values + stat * 4) if 0 <= stat < count else 0

    def stat_id(self, name):
        return self.m.i32(STAT_IDS[name])

    def virtual(self, obj, slot, *args):
        """A getter through the object's vtable (thiscall)."""
        return self.world.peek(self.m.u32(self.m.u32(obj) + slot), args, this=obj)

    def pos(self, obj):
        return list(struct.unpack('<3i', self.m.vm.uc.mem_read(obj + OBJ_POS, 12)))

    def block(self, obj, offset):
        address = self.m.u32(obj + offset)
        if not address:
            return None
        return dict(action=self.m.i32(address + BLOCK_ACTION),
                    name=self.m.text(address + BLOCK_NAME, 32),
                    obj=self.m.u32(address + BLOCK_OBJ), attack=self.m.u32(address + BLOCK_ATTACK))

    def game_frame(self):
        base, plus, minus = GAME_FRAME
        m = self.m
        return m.i32(PLAY_SCREEN + base) + m.i32(PLAY_SCREEN + plus) - m.i32(PLAY_SCREEN + minus)

    def describe_action(self, obj, entry):
        """action, action_name and -- for an attack, whose start retail keeps
        -- action_tick, game frames since it started."""
        doing = self.block(obj, DOING)
        if not doing:
            return
        entry.update(action=doing['action'], action_name=doing['name'])
        if (doing['action'] == ACTION_ATTACK and doing['attack']
                and doing['attack'] == self.m.u32(obj + LAST_ATTACK)):
            entry['action_tick'] = self.game_frame() - self.m.i32(obj + LAST_ATTACK_FRAME)

    def mode(self, obj):
        """The movement mode by the root block, as IsValidAttack's mode test
        reads it (ACH §3 item 9): sneak `root->Is("sneak")`, bow root
        action 0x19, combat root action 3 not named "walk"; else normal."""
        root = self.block(obj, ROOT)
        if not root:
            return None
        name = (root['name'] or '').lower()
        if name == 'sneak':
            return 'sneak'
        if root['action'] == ACTION_BOW:
            return 'bow'
        if root['action'] == ACTION_COMBAT and name != 'walk':
            return 'combat'
        return 'normal'

    def target(self, obj):
        """The character it fights: root->obj while root is COMBAT or BOW."""
        root = self.block(obj, ROOT)
        if root and root['action'] in (ACTION_COMBAT, ACTION_BOW) and root['obj']:
            return root['obj']
        return None

    def object_id(self, obj):
        return 0 if obj == self.player() else self.m.i32(obj + OBJ_ID)

    def player(self):
        return self.m.u32(PLAYER)

    def screen(self):
        m = self.m
        name = SCREENS.get(m.u32(CURRENT_SCREEN), 'other')
        if name == 'play':
            if m.u32(INGAME_MENU_OPEN):
                return 'menu'
            if m.u32(DIALOG_PANE + 0x190):
                return 'dialog'
        return name

    def project(self, point):
        """World to map-screen pixels by retail's WorldToScreen, less the map
        pane's scroll origin; None off the pane."""
        vm = self.world.vm
        scratch = self.world.scratch
        vm.write(scratch, struct.pack('<3i', *point))
        self.world.peek(WORLD_TO_SCREEN, (scratch, scratch + 12, scratch + 16))
        sx, sy = struct.unpack('<2i', vm.uc.mem_read(scratch + 12, 8))
        m = self.m
        x, y = sx - m.i32(MAP_PANE + MAP_SCROLL_X), sy - m.i32(MAP_PANE + MAP_SCROLL_Y)
        if 0 <= x < m.i32(MAP_PANE + PANE_W) and 0 <= y < m.i32(MAP_PANE + PANE_H):
            return [x + m.i32(MAP_PANE + PANE_X), y + m.i32(MAP_PANE + PANE_Y)]
        return None

    # ---- the mouse --------------------------------------------------------

    def mouse_route(self):
        """The play screen's panes a mouse press is offered, before any of
        them looks at the point, as retail routes a button message: the
        dispatcher 0x0048f870 (CLICKS_OFF, a fade holding the mouse),
        TPlayScreen::MouseClick 0x0047ce80 (paused; in demo mode a click
        restarts), then TScreen::MouseClick 0x004903b0: the top exclusive
        pane alone while it holds the mouse, offered the press wherever it
        is, else every pane that takes clicks, each when the point is in its
        rect (0x004904dd..0x004904ed). (exclusive, panes), or None while a
        press is dropped."""
        m = self.m
        if m.u32(CURRENT_SCREEN) != PLAY_SCREEN or m.u32(CLICKS_OFF) or self.fade_holds_mouse():
            return None
        if m.u32(PLAY_SCREEN + PAUSED) or self.demo_restart():
            return None
        panes = m.pointers(m.u32(PLAY_SCREEN + SCREEN_PANES), m.i32(PLAY_SCREEN + SCREEN_PANE_COUNT))
        depth = m.i32(PLAY_SCREEN + EXCLUSIVE_DEPTH)
        if depth > 0 and m.u32(PLAY_SCREEN + EXCLUSIVE_FLAGS + 4 * depth) & EXCLUSIVE_MOUSE:
            index = m.i32(PLAY_SCREEN + EXCLUSIVE_PANE + 4 * depth)
            top = panes[index] if 0 <= index < len(panes) else 0
            offered = top and not self.virtual(top, VT_PANE_HIDDEN) and not m.u32(top + PANE_NO_CLICKS)
            return True, [top] if offered else []
        return False, [pane for pane in panes if pane and not self.virtual(pane, VT_PANE_HIDDEN)
                       and not m.u32(pane + PANE_NO_CLICKS) and not self.virtual(pane, VT_PANE_SHUT)]

    def fade_holds_mouse(self):
        """A running fade whose flags hold the mouse: the dispatcher asks the
        fade-in while it isn't done, else the fade-out (0x0048f8a5..
        0x0048f8d9); TScreen::MouseClick asks the fade-in while it fades
        (0x004903c1..0x004903d8)."""
        m = self.m
        fade_in, fade_out = m.u32(PLAY_SCREEN + FADE_IN), m.u32(PLAY_SCREEN + FADE_OUT)
        flags = 0
        if fade_in and not self.virtual(fade_in, VT_FADE_IN_DONE):
            flags = m.u32(fade_in + FADE_FLAGS)
        elif fade_out and not self.virtual(fade_out, VT_FADE_OUT_DONE):
            flags = m.u32(fade_out + FADE_FLAGS)
        if flags & FADE_HOLDS_MOUSE:
            return True
        return bool(fade_in and self.virtual(fade_in, VT_FADING)
                    and m.u32(fade_in + FADE_FLAGS) & FADE_HOLDS_MOUSE)

    def demo_restart(self):
        """In demo mode a click while the player's script runs trigger 1,
        without the PAUSE flag, restarts the game (0x0047ce91..0x0047cf12)."""
        m = self.m
        player = self.player()
        if not m.u32(PLAY_SCREEN + DEMO) or not player:
            return False
        script = m.u32(player + PLAYER_SCRIPT)
        return bool(script and not self.world.peek(FLAG_NAMED, (PAUSE,), this=player)
                    and m.i32(script + SCRIPT_TRIGGER) == 1)

    def in_pane(self, pane, x, y):
        m = self.m
        x, y = x - m.i32(pane + PANE_X), y - m.i32(pane + PANE_Y)
        return 0 <= x < m.i32(pane + PANE_W) and 0 <= y < m.i32(pane + PANE_H)

    def press_reaches(self, pane, x, y, route):
        """Whether a press at screen (x, y) is offered to `pane` (route:
        mouse_route's)."""
        if route is None or pane not in route[1]:
            return False
        return route[0] or self.in_pane(pane, x, y)

    def world_click(self, x, y, route):
        """Whether a left click at screen (x, y) reaches the world: the map
        pane is offered the press inside its rect, none of the side tabs'
        buttons holds the point (TMapPane::MouseClick leaves those to them,
        0x0044f146..0x0044f182), the editor is off (0x0044f188) and there is
        a player (0x0044f986). Panes over the map that are not the side tabs
        (the status bar, the text bar) are offered the same press: retail
        hands it to the map pane as well."""
        m = self.m
        if (not self.press_reaches(MAP_PANE, x, y, route) or not self.in_pane(MAP_PANE, x, y)
                or m.u32(EDITOR) or not self.player()):
            return False
        tabs_x, tabs_y = x - m.i32(SIDE_TABS + PANE_X), y - m.i32(SIDE_TABS + PANE_Y)
        return not self.world.peek(BUTTON_AT, (tabs_x & 0xffffffff, tabs_y & 0xffffffff), this=SIDE_TABS)

    def cursor(self):
        """The object under the pointer as the last drawn frame picked it:
        what a click there acts on. TMapPane::OnObject at the pointer (MI
        §2), which reuses the 3D pick every DrawScene makes at the pointer
        (PICK at PICK_X / PICK_Y) and so draws nothing, then walks the map's
        2D objects, as the hover cursor (MI §3) and a click (CW §1.1) ask
        it. None off the map pane, MISSING while the pick isn't the
        pointer's (no frame has drawn since the pointer moved: OnObject
        would draw one to pick)."""
        m = self.m
        if m.u32(CURRENT_SCREEN) != PLAY_SCREEN:
            return None
        x, y = m.i32(CURSOR_X) - m.i32(MAP_PANE + PANE_X), m.i32(CURSOR_Y) - m.i32(MAP_PANE + PANE_Y)
        if not (0 <= x < m.i32(MAP_PANE + PANE_W) and 0 <= y < m.i32(MAP_PANE + PANE_H)):
            return None
        if (x, y) != (m.i32(PICK_X), m.i32(PICK_Y)):
            return MISSING
        return self.world.peek(ON_OBJECT, (x, y, 0), this=MAP_PANE) or None

    # ---- the walk map ---------------------------------------------------

    def walk_sectors(self, level, first, last):
        """(sector x, sector y) -> the sector's walkmap (64 rows of 64
        cells) or None without one, for the sectors on `level` from `first`
        to `last` (inclusive (x, y) corners) as FindSector 0x00499e10 finds
        them: the pool's first with that level and place."""
        m = self.m
        found = {}
        for sector in m.pointers(m.u32(SECTORS), m.i32(SECTOR_COUNT)):
            if not sector or m.i32(sector + SECTOR_LEVEL) != level:
                continue
            key = (m.i32(sector + SECTOR_X), m.i32(sector + SECTOR_Y))
            if key in found or not all(a <= k <= b for a, k, b in zip(first, key, last)):
                continue
            walkmap = m.u32(sector + SECTOR_WALKMAP)
            found[key] = (struct.unpack(f'<{SECTOR_CELLS}H', m.vm.uc.mem_read(walkmap, 2 * SECTOR_CELLS))
                          if walkmap else None)
        return found

    def walkgrid(self, player, radius):
        """PROTOCOL.md §7's walk grid around the player, each cell as
        GetWalkHeight 0x00452e10 reads it on his level: cell k holds world
        k * 16 ((x + 8) >> 4), its sector (k >> 6) found by FindSector, the
        height the walkmap's low 10 bits at cell (k & 63) (0x00499720), 0 in
        a sector without a walkmap. Where retail finds no sector (and would
        answer 0), -1: unknown, not "no floor". The square is the port's
        (src/observation.cpp WriteWalkGrid)."""
        x, y, _ = self.pos(player)
        gx, gy = (x - radius) >> WALK_SHIFT, (y - radius) >> WALK_SHIFT
        cells = ((2 * radius) >> WALK_SHIFT) + 1
        sectors = self.walk_sectors(self.m.u16(player + OBJ_LEVEL), (gx >> SECTOR_SHIFT, gy >> SECTOR_SHIFT),
                                    ((gx + cells - 1) >> SECTOR_SHIFT, (gy + cells - 1) >> SECTOR_SHIFT))
        heights = []
        for row in range(gy, gy + cells):
            column, end = gx, gx + cells
            while column < end:
                key = (column >> SECTOR_SHIFT, row >> SECTOR_SHIFT)
                stop = min(end, (key[0] + 1) << SECTOR_SHIFT)
                if key not in sectors:
                    heights += [-1] * (stop - column)
                elif sectors[key] is None:
                    heights += [0] * (stop - column)
                else:
                    line = (row & SECTOR_MASK) << SECTOR_SHIFT
                    run = sectors[key][line + (column & SECTOR_MASK):line + ((stop - 1) & SECTOR_MASK) + 1]
                    heights += [h & WALK_HEIGHT_MASK for h in run]
                column = stop
        return dict(origin=[gx, gy], cell=1 << WALK_SHIFT, w=cells, h=cells, heights=heights)

    # ---- check line -----------------------------------------------------

    def check(self):
        """PROTOCOL.md §3."""
        fields = dict(screen=self.screen())
        player = self.player()
        if player:
            m = self.m
            doing = self.block(player, DOING)
            fields.update(level=m.u16(player + OBJ_LEVEL), pos=self.pos(player),
                          facing=m.u8(player + OBJ_FACING),
                          health=self.stat(player, self.stat_id('health')),
                          mana=self.stat(player, self.stat_id('mana')),
                          fatigue=self.stat(player, self.stat_id('fatigue')),
                          action=doing['action'] if doing else 0)
            target = self.target(player)
            fields['target'] = None if target is None else dict(
                pos=self.pos(target), health=self.stat(target, self.stat_id('health')))
        fields['draws'] = self.world.draws
        return fields

    # ---- observation ----------------------------------------------------

    def observe(self, objects=800, frame=None, walkgrid=None):
        world, m = self.world, self.m
        result = dict(t=world.tick, screen=self.screen(), control=bool(m.u32(CONTROL)))
        player = self.player()
        if player:
            result['player'] = self.describe_player(player)
            result['objects'] = self.nearby(player, objects)
        result['equip_slots'] = self.equip_slots()
        result['belt_boxes'] = self.belt_boxes()
        on = self.cursor()
        if on is not MISSING:
            result['cursor'] = None if on is None else self.object_id(on)
        dialog = self.dialog()
        if dialog:
            result['dialog'] = dialog
        result['messages'] = self.messages()
        if player and walkgrid is not None and walkgrid >= 0:
            result['walkgrid'] = self.walkgrid(player, int(walkgrid))
        if frame:
            world.write_frame(frame)
            result['frame'] = str(frame)
        return result

    def describe_player(self, player):
        m = self.m
        info = dict(pos=self.pos(player), level=m.u16(player + OBJ_LEVEL),
                    facing=m.u8(player + OBJ_FACING), radius=signed(self.virtual(player, VT_RADIUS)))
        info['screen'] = self.project(info['pos'])
        for name in ('health', 'mana', 'fatigue'):
            info[name] = [self.stat(player, self.stat_id(name)), self.virtual(player, VT_MAX[name])]
        info['mode'] = self.mode(player)
        self.describe_action(player, info)
        target = self.target(player)
        info['target'] = None if target is None else self.object_id(target)
        info['xp'] = [self.stat(player, s) for s in (LEVEL_STAT, EXP_STAT, NEXTEXP_STAT)]
        info['attacklevel'] = self.stat(player, ATTACKLEVEL_STAT)
        info['stats'] = {name: self.stat(player, stat) for name, stat in ATTRIBUTE_STATS.items()}
        info['skills'] = {name: [self.stat(player, base + skill)
                                 for base in (SKILL_STAT, SKILL_EXP_STAT, SKILL_NEXT_STAT)]
                          for skill, name in enumerate(SKILL_NAMES)}
        info['levelup'] = [m.i32(player + LEVELUP + 4 * k) for k in range(3)]
        info['combat'] = {name: signed(self.virtual(player, slot)) for name, slot in
                          (('offense', VT_OFFENSE), ('defense', VT_DEFENSE), ('armor', VT_ARMOR),
                           ('damage', VT_WEAPON_DAMAGE))}
        info['effect'] = self.effect(player)
        weapon_type = self.virtual(player, VT_WEAPON_TYPE)
        skill = WEAPON_SKILL_BASE - SKILL_STAT + weapon_type     # the skill a kill raises (GRIND §2.1)
        hand = m.u32(player + EQUIPMENT_HAND)
        info['weapon'] = (dict(name=m.text(m.u32(hand + OBJ_NAME)), mask=1 << weapon_type)
                          if hand else dict(name=None, mask=0))
        info['weapon']['skill'] = SKILL_NAMES[skill] if 0 <= skill < len(SKILL_NAMES) else None
        info['belt'] = self.belt(player)
        info['quick_spells'] = self.quick_spells(player)
        info['gold'] = self.virtual(player, VT_INVENTORY_AMOUNT, GOLD)
        info['pack'] = self.pack(player)
        worn = m.pointers(player + EQUIPMENT, EQUIPMENT_SLOTS)
        info['equipped'] = {str(slot): m.text(m.u32(item + OBJ_NAME)) for slot, item in enumerate(worn) if item}
        info['items'] = self.items(player)
        return info

    def effect(self, player):
        """The stat effect in force -- one at a time: AddStatEffect 0x0051c2c0
        ends the others (PS §4) -- as its STATLINE's words and the ticks till
        its expiry by the play clock (1/100 s; None for one without TIME);
        None with none."""
        m = self.m
        count = m.i16(player + STAT_EFFECT_COUNT)
        if count <= 0:
            return None
        line, expires = struct.unpack('<Ii', m.vm.uc.mem_read(m.u32(player + STAT_EFFECTS) + 8 * (count - 1), 8))
        ticks = None if expires < 0 else c_div((expires - m.i32(player + PLAY_CLOCK)) * 24, 100)
        return dict(statline=' '.join((m.text(line, 1024) or '').split()), ticks=ticks)

    def held(self, holder):
        """The items in `holder`'s inventory (INV §1: +0x68 / +0x78), by slot."""
        m = self.m
        items = [item for item in m.pointers(m.u32(holder + INVENTORY), m.i32(holder + INVENTORY_COUNT)) if item]
        return sorted(items, key=lambda item: m.i16(item + ITEM_SLOT))

    def items(self, holder, bag=()):
        """Every item `holder` carries -- the pack (0..0xfe), the equipment
        (0x100..), the belt (0x10b..; INV §2) -- and what the bags among them
        hold, each with `bag`, the slots of the bags it is in, outermost
        first."""
        m, found = self.m, []
        for item in self.held(holder):
            slot, kind = m.i16(item + ITEM_SLOT), m.i16(item + OBJ_CLASS)
            entry = dict(slot=slot, name=m.text(m.u32(item + OBJ_NAME)),
                         **{'class': CLASS_NAMES[kind] if 0 <= kind < len(CLASS_NAMES) else None},
                         **self.item_numbers(item))
            if bag:
                entry['bag'] = list(bag)
            found.append(entry)
            if m.i32(item + INVENTORY_COUNT) > 0:
                found += self.items(item, bag + (slot,))
        return found

    def item_numbers(self, item):
        """An item's amount, its own equipment slot and its numbers (GRIND
        §8), each asked as the game asks an item: GetStat(name) 0x00473600,
        whose class stats are the type's after BindTypes 0x0048cab0 bound
        weapon.def and armor.def over class.def (GRIND §5.1)."""
        kind = self.m.i16(item + OBJ_CLASS)
        return dict(amount=signed(self.virtual(item, VT_AMOUNT)), eqslot=self.eqslot(item),
                    stats={name.lower(): self.get_stat(item, name) for name in ITEM_NUMBERS.get(kind, ())})

    def get_stat(self, obj, name):
        """GetStat(name) through the object's vtable (+0xd4), the name in the
        host's scratch buffer; 0 for a name its class lacks (0x004736df)."""
        address = self.world.scratch + NAME_SCRATCH
        self.world.vm.write(address, name.encode('cp1252') + b'\0')
        return signed(self.virtual(obj, VT_GET_STAT, address))

    def eqslot(self, item):
        """The item's EqSlot stat, -1 for an item whose class has none."""
        if not self.has_stat(item, self.m.text(EQSLOT)):
            return -1
        return signed(self.virtual(item, VT_GET_STAT, EQSLOT))

    def equip_slots(self):
        """The screen centre of each equipment slot's cell on the equip pane,
        by EQ_* slot, while the pane shows (else None); a cell is None where a
        press there doesn't reach the pane (a modal holds the mouse)."""
        m = self.m
        if m.u32(CURRENT_SCREEN) != PLAY_SCREEN or self.virtual(EQUIP_PANE, VT_PANE_HIDDEN):
            return None
        route, cells = self.mouse_route(), []
        for slot in range(EQUIPMENT_SLOTS):
            x, y = struct.unpack('<2i', m.vm.uc.mem_read(EQUIP_CELLS + 8 * slot, 8))
            x += m.i32(EQUIP_PANE + PANE_X) + EQUIP_CELL // 2
            y += m.i32(EQUIP_PANE + PANE_Y) + EQUIP_CELL // 2
            cells.append([x, y] if self.press_reaches(EQUIP_PANE, x, y, route) else None)
        return cells

    def belt_boxes(self):
        """The belt's boxes on the bottom bar, in step with `belt`: entry n
        the centre of the box showing belt slot n (box n - first), None where
        no box shows it or a press there doesn't reach the pane; None while
        the shelf is hidden."""
        m = self.m
        if m.u32(CURRENT_SCREEN) != PLAY_SCREEN or self.virtual(BAR_INV_PANE, VT_PANE_HIDDEN):
            return None
        route, first, count = self.mouse_route(), m.i32(BAR_INV_PANE + BAR_FIRST), m.i32(BAR_INV_PANE + BAR_COUNT)
        boxes = []
        for slot in range(BELT_SLOTS):
            box = slot - first
            x = m.i32(BAR_INV_PANE + PANE_X) + BAR_LEFT + BAR_PITCH * box + INV_CELL // 2
            y = m.i32(BAR_INV_PANE + PANE_Y) + BAR_TOP + INV_CELL // 2
            reaches = 0 <= box < count and self.press_reaches(BAR_INV_PANE, x, y, route)
            boxes.append([x, y] if reaches else None)
        return boxes

    def pack(self, player):
        """The player's items in the inventory pane's twelve cells in view
        (slots first .. first + 11, column by column, IPS §6.1), while the
        pane shows his own inventory, with each cell's screen centre while
        a press there reaches the pane (else None): a right click there
        equips the item in its own slot, its EqSlot stat (0x00538210,
        PLAYER_INPUT.md §6)."""
        m = self.m
        pane = INVENTORY_PANE
        if m.u32(pane + INV_CONTAINER) != player:
            return []
        first, route = m.i32(pane + INV_FIRST), self.mouse_route()
        cells = []
        for item in m.pointers(m.u32(player + INVENTORY), m.i32(player + INVENTORY_COUNT)):
            slot = m.i16(item + ITEM_SLOT) if item else -1
            if not first <= slot < first + INV_COLUMNS * INV_ROWS:
                continue
            column, row = divmod(slot - first, INV_ROWS)
            x = m.i32(pane + PANE_X) + INV_LEFT + column * INV_DX + INV_CELL // 2
            y = m.i32(pane + PANE_Y) + INV_TOP + row * INV_DY + INV_CELL // 2
            kind = m.i16(item + OBJ_CLASS)
            cells.append(dict(slot=slot, name=m.text(m.u32(item + OBJ_NAME)),
                              **{'class': CLASS_NAMES[kind] if 0 <= kind < len(CLASS_NAMES) else None},
                              eqslot=self.eqslot(item),
                              screen=[x, y] if self.press_reaches(pane, x, y, route) else None))
        return sorted(cells, key=lambda cell: cell['slot'])

    def has_stat(self, obj, name):
        """Whether obj's class names the stat, in its object stats or its
        type stats, as GetStat(name) 0x00473600 looks for it (stricmp)."""
        m = self.m
        definitions = m.u32(obj + OBJ_CLASS_DEF)
        for stats in (definitions + CLASS_OBJ_STATS, definitions + CLASS_TYPE_STATS):
            entries, count = m.u32(stats + 4), m.i16(stats)
            names = (m.text(entries + STAT_DEF_SIZE * i, STAT_DEF_SIZE) or '' for i in range(count))
            if entries and name.lower() in (n.lower() for n in names):
                return True
        return False

    def quick_spells(self, player):
        """The spell variant each quick slot 1-4 casts, by its spell.def
        VARIANT name: its talisman code matched exactly (stricmp) against
        the spell list's variants, first in file order, as retail casts by
        talismans (SPELLS_MISSILES.md §5.1, GetVariantDataByTalismans
        0x0053ef90)."""
        m = self.m
        spells = m.pointers(m.u32(SPELL_LIST + 4 + 0x10), m.i32(SPELL_LIST + 4))
        names = {}
        for spell in spells:
            variants = m.pointers(m.u32(spell + SPELL_VARIANTS + 0x10), m.i32(spell + SPELL_VARIANTS))
            for variant in variants:
                code = (m.text(variant + VARIANT_TALISMANS, 6) or '').lower()
                names.setdefault(code, m.text(variant + VARIANT_NAME, 32))
        slots = []
        for slot in range(1, 5):
            code = m.text(player + QUICK_SPELLS + 6 * slot, 6)
            slots.append(names.get(code.lower()) if code else None)
        return slots

    def belt(self, player):
        m = self.m
        slots = [None] * BELT_SLOTS
        count, items = m.i32(player + INVENTORY_COUNT), m.u32(player + INVENTORY)
        for item in m.pointers(items, count):
            if item:
                slot = m.i16(item + ITEM_SLOT)
                if BELT_FIRST <= slot < BELT_FIRST + BELT_SLOTS:
                    slots[slot - BELT_FIRST] = m.text(m.u32(item + OBJ_NAME))
        return slots

    def objects_on_level(self, level):
        """Every object of the loaded sectors on `level`."""
        m = self.m
        for sector in m.pointers(m.u32(SECTORS), m.i32(SECTOR_COUNT)):
            if not sector or not m.u32(sector + 0x98) or m.i32(sector + 0x04) != level:
                continue
            yield from (o for o in m.pointers(m.u32(sector + 0xc8), m.i32(sector + 0xb8)) if o)

    def listed(self, obj):
        """Whether `objects` lists `obj` (PROTOCOL.md §7, the port's
        observation's Listed): not a tile, shadow or effect, not carried,
        and nothing the player can't see or click -- an object flagged
        invisible or editor-only -- except the waypoint helpers. Carried is
        an inventory slot (+0x7c, INV §1; -1 on the map), as the port's
        IsInInventory: the item flag marks items on the ground too."""
        m = self.m
        kind, flags = m.i16(obj + OBJ_CLASS), m.u32(obj + OBJ_FLAGS)
        if kind in NOT_LISTED or m.i16(obj + ITEM_SLOT) >= 0:
            return False
        return kind == CLASS_HELPER or not flags & (OF_INVISIBLE | OF_EDITOR)

    def nearby(self, player, radius):
        m = self.m
        origin = self.pos(player)
        found = []
        for obj in self.objects_on_level(m.u16(player + OBJ_LEVEL)):
            if obj == player or not self.listed(obj):
                continue
            kind = m.i16(obj + OBJ_CLASS)
            pos = self.pos(obj)
            distance = math.dist(origin[:2], pos[:2])
            if distance > radius:
                continue
            entry = dict(id=m.i32(obj + OBJ_ID), name=m.text(m.u32(obj + OBJ_NAME)),
                         type=m.text(m.u32(m.u32(obj + OBJ_TYPEINFO))),
                         **{'class': CLASS_NAMES[kind] if 0 <= kind < len(CLASS_NAMES) else None},
                         pos=pos, screen=self.project(pos))
            if kind in (CLASS_PLAYER, CLASS_CHARACTER):
                health = self.stat_id('health')
                entry['enemy'] = bool(self.world.peek(IS_ENEMY, (player,), this=obj) & 0xff)
                entry['health'] = [self.stat(obj, health), self.virtual(obj, VT_MAX['health'])]
                # its class Value, as the kill formula asks a victim (GRIND §1.2); a
                # corpse: health below 1 (GRIND §3.1)
                entry['value'] = signed(self.virtual(obj, VT_GET_STAT, VALUE))
                entry['dead'] = entry['health'][0] < 1
                self.describe_action(obj, entry)
                target = self.target(obj)
                entry['target'] = None if target is None else self.object_id(target)
            else:
                self.describe_thing(obj, kind, entry)
            found.append((distance, entry['id'], entry))
        return [entry for _, _, entry in sorted(found, key=lambda f: f[:2])]

    def describe_thing(self, obj, kind, entry):
        """What isn't a character: a pile's amount; an item's numbers
        (IsInventoryItem 0x00477c00: the item flag); a container's state,
        whether a use opens it, its lock and what it holds (a use takes one
        item; TContainer::Use 0x004dc180)."""
        m = self.m
        amount = signed(self.virtual(obj, VT_AMOUNT))
        if amount > 1:
            entry['amount'] = amount
        if m.u32(obj + OBJ_FLAGS) & OF_INVENTORY:
            entry['item'] = self.item_numbers(obj)
        if kind == CLASS_CONTAINER:
            entry.update(open=m.u16(obj + OBJ_STATE) != 0, openable=bool(self.virtual(obj, VT_OPENABLE)),
                         locked=bool(self.virtual(obj, VT_LOCKED)),
                         contents=[dict(slot=m.i16(item + ITEM_SLOT), name=m.text(m.u32(item + OBJ_NAME)),
                                        amount=signed(self.virtual(item, VT_AMOUNT)))
                                   for item in self.held(obj)])

    def dialog(self):
        """The dialog pane while it shows choices (DLG §4): the texts of the
        response entry (what the player reads; the pane's +0x198 holds the
        choices' tags), and the latest NPC line's speaker and text."""
        m = self.m
        response = m.u32(DIALOG_PANE + 0x190)
        if not response:
            return None
        count = min(max(m.i32(response + ENTRY_TEXTS), 0), 8)
        result = dict(choices=[m.text(text, 1024) for text in m.pointers(response + ENTRY_TEXT, count)])
        entries = m.pointers(m.u32(DIALOG_PANE + 0x18c), m.i32(DIALOG_PANE + 0x17c))
        for entry in reversed([e for e in entries if e and m.i32(e + ENTRY_MODE) == 1]):
            speaker = self.find_object(m.i32(entry + ENTRY_SPEAKER))
            if speaker:
                result['speaker'] = m.text(m.u32(speaker + OBJ_NAME))
            if m.i32(entry + ENTRY_TEXTS) > 0:
                result['text'] = m.text(m.u32(entry + ENTRY_TEXT), 1024)
            break
        return result

    def find_object(self, object_id):
        """The object with map index `object_id` on the player's level."""
        player = self.player()
        if not player:
            return None
        for obj in self.objects_on_level(self.m.u16(player + OBJ_LEVEL)):
            if self.m.i32(obj + OBJ_ID) == object_id:
                return obj
        return None

    def messages(self):
        """Text-bar lines added since the previous observation, as
        TTextBar::AddLine received them (playworld.py's hook)."""
        lines = [text for _, kind, text in self.world.textbar_lines(self.reported)]
        self.reported += len(lines)
        return lines


def check(world):
    return Observer(world).check()
