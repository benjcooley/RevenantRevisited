"""What the director knows about the game, read from the game's own data.

    kb = Knowledge.load(data_dir)            # the install: resources.rvr, imagery.rvi
    kb.characters['Ogrok'].damagemods         # {'burn': 0, 'freeze': 25, ...}: % more damage taken
    kb.characters['Locke'].attacks            # every attack: button, ranges, fatigue, skills
    kb.spells['Ice Bolt'].talismans           # ('Ocean', 'Sky', ...)
    kb.monster_brief('Ogrok')                 # plain words for a Jev state

Sources: char.def (imagery.rvi), spell.def (resources.rvr), both zip
archives the game reads (docs/gameplay/COMBAT_DATA.md). The `.def` grammar
here is just what these two files use: `#define` constants, quoted names,
`|` flag expressions, `\\` continued lines, `BEGIN`/`END` blocks.
"""
from __future__ import annotations

import re
import zipfile
from dataclasses import dataclass, field
from pathlib import Path

DAMAGE_TYPES = ('misc', 'hand', 'puncture', 'cut', 'chop', 'bludgeon', 'magical', 'burn', 'freeze', 'poison')
# spell.def's talisman letters (its header comment; class.def Codes 65-76).
TALISMANS = dict(A='Sun', B='Life', C='Ocean', D='Law', E='Soul', F='Stars', G='Death', H='Chaos',
                 I='Sky', J='Earth', K='Ward', L='Moon')
# Attack buttons (char.def's ATTACK button; retail ButtonAttack 1..15,
# docs/gameplay/forensics/PLAYER_INPUT.md §3) and the controls that press them.
BUTTON_CONTROLS = {1: 'CombatSwing', 2: 'CombatThrust', 3: 'CombatChop',
                   **{3 + n: f'CombatCombo{n}' for n in range(1, 13)}}


def _archive_member(archive: Path, suffix: str) -> str:
    with zipfile.ZipFile(archive) as z:
        name = next(n for n in z.namelist() if n.lower().endswith(suffix))
        return z.read(name).decode('cp1252')


def _logical_lines(text: str) -> list[str]:
    """Comments stripped, `\\` continuations joined, blanks dropped."""
    lines, pending = [], ''
    for raw in text.splitlines():
        line = raw.split('//', 1)[0].rstrip()
        if line.endswith('\\'):
            pending += line[:-1] + ' '
            continue
        line = (pending + line).strip()
        pending = ''
        if line:
            lines.append(line)
    return lines


def _defines(lines: list[str]) -> dict[str, int]:
    out = {}
    for line in lines:
        m = re.match(r'#define\s+(\w+)\s+(-?(?:0x[0-9a-fA-F]+|\d+))', line)
        if m:
            out[m.group(1)] = int(m.group(2), 0)
    return out


def _args(text: str) -> list[str]:
    """Comma-separated arguments, quotes kept intact."""
    return [a.strip() for a in re.findall(r'"[^"]*"|[^,]+', text) if a.strip()]


def _value(token: str, defines: dict[str, int]) -> int | str:
    if token.startswith('"'):
        return token.strip('"')
    total = 0
    for part in token.split('|'):
        part = part.strip()
        if re.fullmatch(r'-?(0x[0-9a-fA-F]+|\d+)', part):
            total |= int(part, 0)
        elif part in defines:
            total |= defines[part]
        else:
            raise ValueError(f'unknown constant {part!r}')
    return total


# ---- characters --------------------------------------------------------------

@dataclass(frozen=True)
class Attack:
    name: str
    flags: int
    button: int
    blocktime: int                   # ticks the opponent has to block or counter
    impacttime: int                  # ticks from start to impact
    nextwait: int
    mindist: int
    maxdist: int
    damagemod: int                   # % added to damage
    fatigue: int
    attackskill: int
    weaponmask: int
    weaponskill: int

    @property
    def control(self) -> str | None:
        return BUTTON_CONTROLS.get(self.button)


@dataclass
class Character:
    name: str
    damagemods: dict[str, int] = field(default_factory=dict)
    groups: str = ''
    enemies: str = ''
    attacks: list[Attack] = field(default_factory=list)


def parse_characters(text: str) -> dict[str, Character]:
    lines = _logical_lines(text)
    defines = _defines(lines)
    chars: dict[str, Character] = {}
    current: Character | None = None
    for line in lines:
        m = re.match(r'CHARACTER\s+"([^"]+)"', line)
        if m:
            current = chars.setdefault(m.group(1).strip(), Character(m.group(1).strip()))
            continue
        if current is None:
            continue
        word, _, rest = line.partition(' ')
        if word == 'END':
            current = None
        elif word == 'DAMAGEMODS':
            current.damagemods = dict(zip(DAMAGE_TYPES, (int(v) for v in _args(rest))))
        elif word == 'GROUPS':
            current.groups = rest.strip().strip('"')
        elif word == 'ENEMIES':
            current.enemies = rest.strip().strip('"')
        elif word == 'ATTACK':
            a = [_value(t, defines) for t in _args(rest)]
            # name, flags, button, response, block, miss, death, blocktime, impacttime,
            # nextwait, chainexptime, mindist, maxdist, hitminrange, hitmaxrange, hitangle,
            # damagemod, fatigue, attackskill, weaponmask, weaponskill, ...
            if len(a) >= 21 and isinstance(a[0], str):
                current.attacks.append(Attack(a[0], a[1], a[2], a[7], a[8], a[9], a[11], a[12],
                                              a[16], a[17], a[18], a[19], a[20]))
    return chars


# ---- spells ------------------------------------------------------------------

@dataclass(frozen=True)
class Spell:
    name: str
    damagetype: str
    talismans: tuple[str, ...]
    mana: int
    damage: tuple[int, int]
    skill: int                       # Invoke skill needed
    pattern: str                     # CONTROLDATA PATTERN, '' for none


def parse_spells(text: str) -> dict[str, Spell]:
    lines = _logical_lines(text)
    defines = _defines(lines)
    damage_names = {value: name.removeprefix('DT_').lower()
                    for name, value in defines.items() if name.startswith('DT_')}
    spells: dict[str, Spell] = {}
    damagetype, pending = 'none', None
    for line in lines:
        word, _, rest = line.partition(' ')
        if word == 'SPELL':
            damagetype, pending = 'none', None
        elif word == 'DAMAGETYPE':
            damagetype = damage_names.get(_value(rest.strip(), defines), 'none')
        elif word == 'VARIANT':
            a = [_value(t, defines) for t in _args(rest)]
            # NAME, TYPE, TALISMANS, EFFECT_NAME, MANA, WAIT_NEXT, MIN_D, MAX_D, SKILL, ...
            codes = a[2]
            if all(c in TALISMANS for c in codes):            # M-O are enemy-only spells
                pending = Spell(a[0], damagetype, tuple(TALISMANS[c] for c in codes),
                                a[4], (a[6], a[7]), a[8], '')
                spells[pending.name] = pending
            else:
                pending = None
        elif word == 'PATTERN' and pending:
            spells[pending.name] = pending = Spell(**{**pending.__dict__, 'pattern': rest.strip().strip('"')})
    return spells


# ---- the knowledge base ------------------------------------------------------

@dataclass
class Knowledge:
    characters: dict[str, Character]
    spells: dict[str, Spell]

    @classmethod
    def load(cls, data: Path) -> Knowledge:
        data = Path(data)
        return cls(parse_characters(_archive_member(data / 'imagery.rvi', 'char.def')),
                   parse_spells(_archive_member(data / 'resources.rvr', 'spell.def')))

    def character(self, name: str) -> Character | None:
        """By type name; char.def has variants with trailing spaces."""
        return self.characters.get(name) or self.characters.get(name.strip())

    def monster_brief(self, name: str) -> dict:
        """What a fighter should know about `name`, in words: the damage it
        resists or takes extra from, and how far its attacks reach."""
        c = self.character(name)
        if not c:
            return {'kind': name, 'known': False}
        weak = [t for t, v in c.damagemods.items() if v > 0]
        resists = [t for t, v in c.damagemods.items() if v < 0]
        reach = max((a.maxdist for a in c.attacks), default=0)
        return {'kind': name, 'known': True,
                'takes_extra_damage_from': weak or ['nothing in particular'],
                'resists': resists or ['nothing in particular'],
                'attack_reach': 'long' if reach > 150 else 'medium' if reach > 60 else 'short'}

    def locke_moves(self, weaponmask: int, attackskill: int, weaponskill: int) -> list[Attack]:
        """Locke's attacks usable now: those for the weapon in hand whose
        skill needs are met, one per button (the first in file order)."""
        locke = self.characters['Locke']
        moves: dict[int, Attack] = {}
        for a in locke.attacks:
            if (a.weaponmask & weaponmask and a.attackskill <= attackskill
                    and a.weaponskill <= weaponskill and a.control and a.button not in moves):
                moves[a.button] = a
        return [moves[b] for b in sorted(moves)]
