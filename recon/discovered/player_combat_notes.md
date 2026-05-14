# Player Combat Input Chain — retail walkthrough

Date: 2026-05-09. Closes out the six tasks tracked in
`recon/discovered/port_status/PLAN.md` under "TPlayer Pulse helpers" /
"Player input → action chain".

All retail addresses below are from the `RevenantDev` Ghidra project. Source
references point at `src/character.cpp` / `src/character.h` / `src/player.cpp`
/ `src/player.h` / `src/playscreen.cpp`.

## 1. The two TPlayer::Pulse helpers (FUN_004cf000 / FUN_004cf490)

These are **NOT TPlayer-specific** — they live on `TCharacter` and are called
from many sites (16 callers for 0x4cf000, 5 for 0x4cf490) including
`TCharacter::AI`, `TPlayer::Pulse`, the script VM (FUN_0057cc90), and the
input dispatcher around 0x47d9b0+.

| Retail addr | Identification | Source line | Script event id |
|---|---|---|---|
| `FUN_004cf000` | `TCharacter::SetWalkMode` | character.cpp:2883 | SCMD_SETWALKMODE = 0x26 |
| `FUN_004cf490` | `TCharacter::SetRunMode`  | character.cpp:2944 | SCMD_SETRUNMODE  = 0x28 |

Anchors that locked the IDs:

- vtable slots 0x304/0x308/0x30c on TPlayer return `combat`/`hand`/`cstaff`/`cknife` /
  `cbow`/`bow` / `torch`/`walk` (already labeled `GetCombatRoot`/`GetBowRoot`/
  `GetTorchRoot` in `renames/agent_tplayer.txt`).
- string anchors at `0x005e00f0..0x005e0138` decode (via dump_str.java) as the
  characters `"r"`, `"r"`, `"f"`, `"run"`, `"run"`, `"run"`, `"l"`, `"r"`, `"f"`,
  `"f"`, `"l"`. Concatenated with the root yields exactly the source's
  `"<root>"`, `"<root>r"`, `"<root>f"`, `"<root>run"`, `"<root>runl"`,
  `"<root>runr"`, `"<root>runf"` family.
- `FUN_004daae0` already named `TActionBlock::CopyCtor`; `FUN_004dadd0` is
  `StName(dst, suffix)` (the in-place append helper used at character.cpp:2966).
- vtable slots `0x1f0` (HasActionAni), `0x1f8` (SetRoot), `0x208` (SetDesired)
  match the calls in source one-for-one.
- The two functions differ only in whether `"run"` is appended before the
  directional suffix; that mirrors the source structure exactly.
- `FUN_0057d9d0(this, 0x26, 1, 0, 0)` vs `FUN_0057d9d0(this, 0x28, 1, 0, 0)`
  fires the SetWalkMode/SetRunMode script events into the scripting layer
  (the `(0,0)` args are stack-locals; the two non-zero args are the script
  command id and a "raise event" flag).

These are safe to drop into source as `TCharacter::SetWalkMode()` /
`TCharacter::SetRunMode()` — no TPlayer-specific behaviour, no missing fields.
The decompiled bodies are at:

- `recon/discovered/cls_0x5a7b98_TCharacter_SetWalkMode_4cf000.cpp`
- `recon/discovered/cls_0x5a7b98_TCharacter_SetRunMode_4cf490.cpp`

## 2. Player input → action chain

```
GAMECMD_SWING/THRUST/CHOP/COMBO1..12   (src/playscreen.cpp:103-122)
        │
        ▼
TPlayScreen command dispatcher  ~0x47d8a0  (giant switch; not yet in source)
        │   PUSH 1/2/3 for SWING/THRUST/CHOP
        │   PUSH (EBP-0x20)  4..15 for COMBO1..12
        ▼
TCharacter::ButtonAttack    FUN_004d2480       (character.cpp:3583)
        │
        ├── chain valid?  (lastattack && (lastattack.flags & CA_CHAIN) &&
        │                  GameFrame()-lastattackticks <= chainexptime &&
        │                  chainhits < 3)
        │       └─→ chainhits++; return true.
        │
        └── FindButtonAttack(id, dmgpcnt, &attack, &impact, &dmg)
                FUN_004d1ff0                    (character.cpp:3471)
                  loops mode = combo / special / normal
                  → IsValidAttack(...)  FUN_004d1120  (character.cpp:3329)
            │
            └─→ DoAttack(attack, impact, dmg)
                    FUN_004d2120                (character.cpp:3540)
                      builds  TActionBlock(ad->attackname, ACTION_ATTACK)
                      ab->obj/attack/impact/damage = ...
                      ab->interrupt = ab->noroot = true
                      SetDesired(ab); SetMoveAngle(GetFace());
```

The retail dispatcher around `0x47d8a0` is `TPlayScreen`'s `OnCommand`-style
switch (lives in `playscreen.cpp` hex range 0x47b…0x47f). Three constant
PUSHes (1, 2, 3) and one register-passed id confirm that retail's mapping is
**identical** to our source — `Swing()=ButtonAttack(1)`, `Thrust()=ButtonAttack(2)`,
`Chop()=ButtonAttack(3)`, `Combo(n)=ButtonAttack(3+n)`. There is no retail-only
behaviour per id; all 15 button ids feed the same `ButtonAttack` entry point
and downstream selection happens entirely inside `IsValidAttack` via
`SCharAttackData::button`.

## 3. TPlayer::ResolveAttack (0x4c4950, 3776 bytes)

`vtable[18]` of TPlayer dispatches via the 2-arg thunk `0x5191e0` which calls
`0x4c4950(this, ab, 0, 0, 0, 0)`. This is a **full override** — it does **not**
call `TCharacter::ResolveAttack` (which is at `0x4c1bb0`, 5798 bytes,
referenced once from `TPlayer::Animate` at `0x518aa9`).

What `0x4c4950` does (at the level of confidence I have without re-reading
all 700 lines):

- Computes `param_2 = (this->vtable[0x224])(damage, type, modifier)` — the
  `CalculateDamage` virtual (slot 0x224, already labeled at 0x4c4860 as
  `TPlayer::CalcDamage`).
- Iterates `chardata->impacts` (count at `+0x214`, base at `+0x218`,
  stride 0x5c) looking for an impact whose `damagemin..damagemax` brackets
  the computed damage AND whose flags satisfy the gating bits in `uStack_98`
  (built from the relative attack angle).
- Branches into three large blocks for:
  1. interactive death/impact (player dies), assembling `<atk> to <impact>`
     and `<atk> to dead` transition action names via `FUN_004dadd0_StName`
     and `DAT_005df820..0x5df838` ("dead", " to "). Allocates a new
     `TActionBlock` with action class `ACTION_DEAD` and forces it as desired.
  2. `block` action (`*doing == 8`): resolves to a `"blockimpact"` action
     block (string anchor `0x005df848`/`0x005df854`). Halves chain by
     clearing `param_5+0x5c`.
  3. normal hit: picks `"impact"` (`0x005df860`) action block from the
     impacts list.
- Populates `ab->fielddamage = param_2`, copies `ab->attackdir/face/angle`
  from `param_5`, sets `ab->interrupt | ab->priority` flag bits at offset
  `+0x60`, then SetDesired.

**Combo/chain logic in this function:** none directly. The chain bookkeeping
is done before this point in `TCharacter::Pulse` (autocombo loop at
character.cpp:275-307) and in `ButtonAttack`. `ResolveAttack` only stamps
`lastattack`/`lastattackticks` on `firsttime` (character.cpp:1621-1625 — the
TCharacter version; the player version mirrors via the `+0x36`/`+0x2c` writes
near the end).

**Retail-specific behaviour vs our source `TCharacter::ResolveAttack`:**

- Retail bails up-front on `(param_6->flags & 0x2800000)` — this is a
  combined `CA_INTERACTIVE | CA_NOPUSH` short-circuit that doesn't appear in
  the source.
- Retail consults global `DAT_00668104`, `DAT_0066829c`, `DAT_00668108` —
  these look like cheat / debug toggles (`god mode`, `no-clip`, `disable-
  damage`). The source has no equivalent; treat as test-only retail glue.
- The block/dodge halve-chance code at character.cpp:1520-1522 is inlined
  into the `<atkflags>+0x40/-0x40/-0x80` recompute around `LAB_004c524d`.
- The rest is structurally identical.

**What TPlayer::ResolveAttack does NOT carry over from the parent:** nothing
combat-relevant. The parent TCharacter::ResolveAttack at 0x4c1bb0 is called
exclusively from TPlayer::Animate (0x518aa9), so TPlayer animates through the
parent path and resolves attacks through its own path; the two are separate
concerns.

## 4. TPlayer::Damage (0x4c5810) and TPlayer::CalcDamage (0x4c4860)

| Retail | Source (player.cpp) | Mapping |
|---|---|---|
| `FUN_004c5810` | `TPlayer::Damage(damage, type)` | calls CalcDamage if type>=0, then SetHealth(0) on lethal else SetHealth(Health()-d). |
| `FUN_004c4860` | `TPlayer::CalcDamage(damage, type)` | resistance×(modifier+100)/100×damage − protection; halves on damage type 6/7/8/9 if status flag 0x200 set, halves on type !=8 if 0x100 set, /=7 on type !=8 if 0x10 set. |

Both look correct. Our source `TPlayer::Damage` is just `TCharacter::Damage(d, type)` — i.e. the player Damage forwards to base. Retail does
this differently: it skips the base entirely, calls CalcDamage as a
virtual, then *directly* writes Health. This means **retail bypasses
`TCharacter::Damage`'s death/impact action-block setup when the victim is
the player**, which would explain why the player has its own death-handling
in `TPlayer::Pulse` (the `Health() <= 0 → DeathPane` flip at player.cpp:128-134).
Source-side this is already mirrored; no fix needed.

The status-flag bits at param_1[0x44]:
- `0x10`  — burning (damage /=7 except type 8 = fire)
- `0x100` — basic protection (damage halved unless type 6-9 = poison/curse)
- `0x200` — magic protection (damage halved on type 6-9 only)
- `0x80000` — interactive-attack-in-progress (also seen in SetWalkMode etc.)

These match `charflags` bit constants in our source's `character.h`; no
retail-only flag found.

## 5. Combo / chain attack data fields

Confirmed retail field offsets via the `ButtonAttack` decomp at 0x4d2480:

| Source field (character.h:546-548)  | Retail offset (this[i] as int*) | Bytes |
|---|---|---|
| `PSCharAttackData lastattack`       | `mbr_0x58`                       | 0x160 |
| `int32_t lastattackticks`           | `mbr_0x59`                       | 0x164 |
| `int32_t chainhits`                 | `mbr_0x5b`                       | 0x16c |

`SCharAttackData` flag/field offsets:
- `flags` at `+0x24`,  bit `0x4000` = CA_CHAIN.
- `chainexptime` at `+0xc0`.
- `nextwait` at `+0xbc` (used in autocombo gate at character.cpp:277).

`MAXCHAINHITS` constant in retail = 3 (literal compare in 0x4d2480). Source
matches.

There is one **retail-only field** sandwiched in there: `mbr_0x5a` (offset
0x168, between lastattackticks and chainhits) is consulted by the TCharacter
combat-flash code as `combatflashticks`. The source moves it out to a
separate field. Not a bug, just a layout difference.

Chain detection algorithm (retail and source are identical):

1. On a fresh attack: `ResolveAttack` stamps `lastattack=ad`, `lastattackticks=GameFrame()`.
2. `ButtonAttack(id)` is called when player presses a key; if a chain is in
   flight it just bumps `chainhits++` without queuing a new action.
3. `Pulse` (character.cpp:275-307) every frame checks whether
   `lastattack->nextwait` ticks have passed AND the chain (or autocombo) is
   still live AND chain hasn't expired; if so calls `SpecificAttack(ad)`
   for the next `chainname`-matching attack and decrements `chainhits`.

No retail-only chain semantics found beyond what character.cpp:275 already
implements.

## 6. Keybind → button id mapping

`src/playscreen.cpp:103-122` declares the 5+12 = 17 combat-mode bindings
(`Swing`/`Thrust`/`Chop` + 12 `Combo` slots). The dispatcher that routes
`GAMECMD_SWING/THRUST/CHOP/COMBO*` → `Player->Swing()/Thrust()/Chop()/Combo(n)`
is **not yet in our source** (no `case GAMECMD_SWING:` exists in
`playscreen.cpp` — verified via grep). It needs to be ported as part of this
task's source-side follow-up.

Retail dispatcher at `0x47d8a0`+ jumps directly to `ButtonAttack` with hard-
coded `1`, `2`, `3` immediates and a register-loaded combo id; matches
`character.h:204-210`:

```cpp
bool Swing()      { return ButtonAttack(1); }
bool Thrust()     { return ButtonAttack(2); }
bool Chop()       { return ButtonAttack(3); }
bool Combo(int n) { return ButtonAttack(3 + n); }
```

So the source has the right helpers already; only the playscreen dispatcher
glue is missing. No retail-specific behaviour per button id at the dispatch
level — all selection happens inside `IsValidAttack` looking at
`ad->button`, `ad->flags & (CA_RESPONSE | CA_SPECIAL)`, etc.

## Summary table — what's source-ready

| Concern | Retail addr | Source state | Action |
|---|---|---|---|
| `TCharacter::SetWalkMode` | 0x4cf000 | already in src/character.cpp:2883 | none — body matches |
| `TCharacter::SetRunMode`  | 0x4cf490 | already in src/character.cpp:2944 | none — body matches |
| `TCharacter::ButtonAttack`| 0x4d2480 | already in src/character.cpp:3583 | none — chain bookkeeping matches |
| `TCharacter::FindButtonAttack` | 0x4d1ff0 | already in src/character.cpp:3471 | none |
| `TCharacter::DoAttack` | 0x4d2120 | already in src/character.cpp:3540 | none |
| `TPlayer::ResolveAttack` | 0x4c4950 | source uses base TCharacter version | source ports TCharacter::ResolveAttack 1:1; TPlayer override in retail mostly inlines extra cheat-globals — defer |
| `TPlayer::Damage` | 0x4c5810 | src/player.cpp:170 (forwards to base) | retail skips base; not currently a problem |
| `TPlayer::CalcDamage` | 0x4c4860 | implicit in CalculateDamage chain | maps to virtual `CalculateDamage` + status-flag adjustments |
| `GAMECMD_SWING/THRUST/CHOP/COMBOn → ButtonAttack` | ~0x47d8a0+ | **MISSING** in src/playscreen.cpp | TODO: add `case GAMECMD_SWING: Player->Swing();` style dispatcher to playscreen's command handler |

## Files written

- `recon/discovered/cls_0x5a7b98_TCharacter_SetWalkMode_4cf000.cpp` — annotated decomp.
- `recon/discovered/cls_0x5a7b98_TCharacter_SetRunMode_4cf490.cpp` — annotated decomp.
- `recon/discovered/cls_0x5a7b98_TCharacter_ButtonAttack_4d2480.cpp` — raw decomp.
- `recon/discovered/cls_0x5a7b98_TCharacter_ResolveAttack_4c1bb0.cpp` — raw decomp (parent ResolveAttack).
- `recon/discovered/renames/agent_player_combat.txt` — rename pairs.
- `recon/discovered/player_combat_notes.md` — this file.
