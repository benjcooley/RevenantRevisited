# Forensics: spell casting, spell damage, missiles and arrows

**Topic:** what the shipped game does from a quick-spell key press to the
damage roll: talisman checks, the cast gates (mana, cooldown, skill roll),
building the spell, its timers and effects, how spell effects decide a hit
and apply damage, spell missiles, and arrows. Effect visuals belong to VFX;
this doc covers the gameplay side only.
**Status:** 2026-10-08. Read from the retail disassembly; nothing here has
been run in the emulator yet (katas S1 to S4 of
[../COMBAT_DOJO.md](../COMBAT_DOJO.md)). Statements marked **unverified**
were not settled in the asm.
**Evidence:** the unchanged retail image (`recon/retail_asm/baseline`,
read with the dojo's `rdis.py`), raw `E8` call scans and `.rdata` vtable
scans of `.text`, builder registrations (`push "Name"; mov ecx, static;
call 0x46df00 / 0x40db90; mov [static], vtable`), the stat registrations
(`recon/scripts/object_stats.py`), and the shipped `spell.def` /
`class.def` (from `resources.rvr` / `imagery.rvi`). Ghidra decompiles in
`recon/classes/cls_0x41c7f0.cpp`, `cls_0x5b99c0.cpp` and `cls_0x5b401c.cpp`
were used as a map only. Their stack arguments are garbled and their class
names are wrong (§9).

Stat ids (PLAYER object stats): Health 3, Mana 5, ManaCostPct 31,
SpellDamageInc 32, Invoke 42, InvokeExp 53; DmgResMagical is 12 (copy index
6 + 6). The stat accessors read the id from a static. ManaCostPct is
`[0x66d9d0]`, SpellDamageInc `[0x66da30]`, Health `[0x66ca4c]`, Mana
`[0x66ca38]`, Poisoned `[0x66ca5c]`.

## 0. Where spells tick (answer for the port's tick/draw split)

**Every spell timer runs on the game tick, the Pulse path.** None of them
runs in Animate.

- `TCharacter::Pulse` `0x4c1bb0` (TCharacter vtable `0x5a7848` slot
  `0x110`; TPlayer's `0x518aa0` calls it first) begins with
  `TSpellManager::Pulse(this + 0x170)` (`0x4c1be1`). That call is the
  only caller of `0x540750`. The whole Pulse is skipped while
  `[0x65d0c4]` (PlayScreen `+0x5d4`) is set.
- Slot `0x11c` is `TCharacter::Animate(bool)` `0x4c36b0` (ret 4). It forwards to the
  animator (`0x470ca0` → animator `+0x30`) and never touches spells. Slot
  `0x114` is Move `0x4c46d0`.
- The map drives both passes. MapPane::Pulse `0x454390` runs the loop
  `0x4580f0` over OBJSET_PULSE and calls slot `0x110`. MapPane::Animate(draw)
  `0x454450` runs `0x458750`, which calls slot `0x11c` and OnScreen
  (`0x470e40`, slot `0x124`). OnScreen creates the animator (slot `0x28`,
  `0x46e8b0`) and calls its Initialize (`+0x18`). The game frame counter
  (`+0x680`) advances in TPlayScreen slot `0x18` `0x47c2c0`. Retail runs one
  Pulse pass and one Animate pass per main-loop iteration.
- **TSpellManager::Pulse `0x540750`.** It decrements the cooldown `wait`
  (`+0x14`, the variant's NEXTSPELLWAIT) when it is above 0. It then walks
  the spell slots up to the count taken at entry. For each live spell it
  calls `vt+8` Pulse and then `vt+4` Timer. When Timer returns true it calls
  `vt+0xc` Kill, deletes the spell (`vt+0`, 1) and nulls the slot (`0x41cb40`:
  the used count drops, trailing empty slots are trimmed, nothing is
  compacted). An empty slot is skipped.
- **TSpell::Timer `0x53f410`.**
  - `timer` (`+0x110`) is decremented when it is above 0.
  - DELAY (`wait` `+0x124`, from spelldata `+0xa4`) is decremented when it
    is above 0. At 0 the effect is created (§2.10) and `wait` becomes −1.
  - It returns `timer == 0`.
  - `timer` starts at **−1**, so nothing times out by itself. A spell ends
    when something calls Kill, which sets `timer = 0`: the effect (FireFlash
    after 100 pulses), or the Strike class's DURATION (§2.13).
- **TSpell::Pulse `0x53f800`** (only while `[0x65d0c4] == 0`): for each
  effect in the spell's list (`+0x13c`) whose `+0x44` is 0 (meaning
  unverified; it is a pointer), it calls effect slot `0x110` (Pulse) and
  then slot `0x8c`. **This is how spell effects get pulsed.** The retail
  TEffect constructor (`0x4de770`, inlined in the effect creators) ORs
  `def.flags | 0x48001` (IMMOBILE | ANIMATE | NOTIFY), with no OF_PULSE. The
  def TSpell::Timer builds has flags 0, so these effects are pulsed by
  their spell inside the caster's TCharacter::Pulse. That class.def /
  object-type flags never add OF_PULSE is **unverified**. The port's TEffect
  constructor sets OF_PULSE. If the map also pulses them, they pulse twice.
- **Where each damage call runs:**

| Damage site | Function (slot) | Class (builder name) | Path |
|---|---|---|---|
| `0x4e3063` | `0x4e3020` Pulse (`0x110`) | `BURN` effect, vtable `0x5a9470` | tick (spell pulse, or map pulse when not a spell's) |
| `0x4e9ec0` | `0x4e9d60` Pulse (`0x110`) | `FIRECONE`, vtable `0x5aaaa4` (spell Napalm) | tick |
| `0x4ed84f` | `0x4ed3d0` Pulse (`0x110`) | `Quicksand`, vtable `0x5ab4c4` (Quicksand, Swamp Pit) | tick |
| `0x4faf58`, `0x4fb0c5` | `0x4fadb0` Pulse (`0x110`) | `Paralize1`, vtable `0x5adca4` (Physical / Neural / Full Paralysis) | tick |
| `0x5017d9` | `0x501790` Pulse (`0x110`) | `RockStorm`, vtable `0x5b033c` | tick |
| `0x5036c1` | `0x503580` Pulse (`0x110`) | `LightningStorm`, vtable `0x5b0a98` | tick |
| `0x5059f2` | `0x505840` Pulse (`0x110`) | `JhagaAttack`, vtable `0x5b14ac` | tick |
| `0x505edc` | `0x505a90` Pulse (`0x110`) | `DragonAttack`, vtable `0x5b16ac` | tick |
| `0x52295b` | `0x522800` Pulse (`0x110`) | `WindStrip`, vtable `0x5b5f20` | tick |
| `0x540e11` | `0x540d70` (TSpell `vt+8` Pulse) | spell class `Strike`, vtable `0x5b9bdc` | tick (REPEATDAMAGE only: unused by shipped data, §2.13) |
| `0x4fd149` | `0x4fcda0` animator Pulse (`+0x2c`) | `Puke` 3D animator, vtable `0x5ae610` | tick: TObjectInstance::Pulse `0x4708e0` calls animator `+0x2c`. The animator exists only after OnScreen in the Animate pass. |
| `0x4e17d2` | `0x4e1770` (effect slot `0x1fc`) | `FireFlash`, vtable `0x5a8f90` | **Animate path, once.** Called by the `FireFlash` animator's Initialize `0x4e18c0` (`+0x18`), when the effect first gets its animator (OnScreen). |
| (area) | `0x4de3c0` AreaDamage | called by FireBall `0x510c10` (missile Pulse) and 8 more sites (§2.15) | tick for FireBall; other callers unverified |

  The Lightning strip and Puke damage the port puts in Animate belong on
  the tick, except FireFlash's single hit, which is tied to animator
  creation.
  Several sites (`0x4e9ec0`, `0x4fd149`, `0x5059f2`, `0x52295b`) follow an
  early `ret` in their function, with no other reference to the code after
  that `ret`. They were taken as a later block of the same function
  (reached by a jump). That is the likely reading and was checked only by
  the absence of other references.

## 1. Function table

Conventions are taken from `ret N`. "thiscall(a, b) ret 8" means `ecx = this`.

| Address | Identity | Convention | Confidence, evidence |
|---|---|---|---|
| `0x51b5d0` | TPlayer::InvokeQuickSpell(button) | thiscall ret 4 | high: SPLNOTAL / SPLMISTAL / SPLCASTOK / SPLCASTFAIL / "Fizzle" strings; called by quick-spell panes |
| `0x51b7c0` | TPlayer::HasTalismans(str) | thiscall ret 4 | high: "Spell Pouch" / "spellpouch", TALISMAN class `0x66dedc`, stat "Code" |
| `0x4d5b90` | TCharacter::CastByName(name, targets, numtargs, sourcepos) | thiscall ret 0x10 | high: variant-by-name `0x53f010`, then `0x4d5c20` with variant+0x24 |
| `0x4d5c20` | TCharacter::CastByTalismans(tal, targets, numtargs, sourcepos) | thiscall ret 0x10 | high: calls `0x53fe80` on `this+0x170`, network send `0x584870`, "fizzle" |
| `0x53fe80` | TSpellManager::CastByTalismans(tal, invoker, targets, numtargs, sourcepos, master) | thiscall ret 0x18 | high: matches variant `+0x24` (talismans); SPLMANA / SPLLVLNEG / SPLLVLLOW. The brief's "CastByName core" label is wrong. |
| `0x540460` | TSpellManager cast with no gates (network receive) | thiscall ret 0x18 | medium: only caller `0x5818ec` (network); no mana, skill or wait checks |
| `0x53f010` | TSpellList::GetVariantDataByName | thiscall ret 4 | high: stricmp variant `+0x04`, first match |
| `0x53ef90` | TSpellList::GetVariantDataByTalismans | thiscall ret 4 | high: stricmp variant `+0x24`, first match |
| `0x53ede0` | TSpellList::GetSpellDataByName | thiscall ret 4 | medium: used by `0x53f250` (TSpell::SetByName) |
| `0x540ad0` | TSpellVariantArray::Get(i) (`items[i]`, or the default `+0x14` when null) | thiscall ret 4 | high |
| `0x53f680` | TSpell::ManaDrain | thiscall ret 0 | high: Mana `0x1d0` / SetMana `0x1d4` / MaxMana `0x1e8`, ManaCostPct |
| `0x4d5900` | TCharacter::SetCast(anim, target, delay) | thiscall ret 0xc | high: "inv", "invoke", ACTION 0xb, ForceCommand |
| `0x53f090` | TSpell::TSpell(invoker, targets, numtargs, sourcepos, spelldata, variant, master) | thiscall ret 0x1c | high: vtable `0x5b99c0`, POISONCHANCE roll |
| `0x53f410` | TSpell::Timer (vt+4) | thiscall ret 0 | high |
| `0x53f800` | TSpell::Pulse (vt+8) | thiscall ret 0 | high |
| `0x53f860` | TSpell::Kill (vt+0xc) | thiscall ret 0 | high |
| `0x53f730` | TSpell::ObjectRemoved(mode, obj) | thiscall ret 8 | medium: called per spell by `0x5408d0` |
| `0x53f560` | TSpell::Damage(target) | thiscall ret 4 | high: 13 callers |
| `0x53f1f0` | TSpell::GetSourcePos(out) | thiscall ret 4 | high: source or (0,0,0); always returns 1 |
| `0x53f250` | TSpell::SetByName(name) | thiscall ret 4 | medium |
| `0x540750` | TSpellManager::Pulse | thiscall ret 0 | high |
| `0x5407d0` / `0x540820` | TSpellManager::GetDefense / GetOffense (Σ spell `+0x134` / `+0x138`) | thiscall | high |
| `0x542200` / `0x542290` | spell-class creators "Spell" (TSpell, 0x150 bytes) / "Strike" (0x154 bytes) | thiscall ret 0x1c | high: registry `0x670220[0x67021c]`, names at `0x540b80` / `0x540ba0` |
| `0x540d70` | TStrikeSpell::Pulse (vt+8) | thiscall | high (vtable `0x5b9bdc` +8) |
| `0x540eb0` | TStrikeSpell::Timer (vt+4) | thiscall | high (vtable +4); body only partly read |
| `0x540bc0` | TStrikeSpell::BuildEffectDef(target, def*) | thiscall ret 8 | high |
| `0x542340` | TStrikeSpell vt+0xc (Kill override) | thiscall | slot only; body not read |
| `0x53e4e0` | SSpellData::Load(name, t) | thiscall ret 8 | high: tag strings |
| `0x53dd10` | SSpellData::LoadControlData(t, variant*) | thiscall ret 8 | high: CONTROLDATA tag strings |
| `0x53ead0` | TSpellList::Load | — | not read (brief's address; unverified) |
| `0x4de3c0` | AreaDamage(attacker, pos*, radius, min, max, type, minradius) | cdecl | high: iterator, IsEnemy, Damage slot `0x228`, kill exp |
| `0x4de560` | AreaBurn(attacker, pos*, radius) | cdecl | high: Burn `0x4d3590` on enemies in radius |
| `0x4de610` | area knockback of **dead** enemies (`0x4de720` = random(7,12), random(6,16), KnockBack) | cdecl | medium |
| `0x510220` | TMissileEffect::Pulse (base missile, vtable `0x5b3e18` +0x110) | thiscall ret 0 | high |
| `0x5101b0` | TMissileEffect init (`+0x1fc`) | thiscall | high |
| `0x510400` / `0x510450` | Photon init (`+0x1fc`) / Pulse (`+0x110`), vtable `0x5b3c18` | thiscall | high (builder "Photon" `0x5b3c14`) |
| `0x510bd0` / `0x510c10` | FireBall init / Pulse, vtable `0x5b408c` | thiscall | high (builder "FireBall" `0x5b4088`) |
| `0x4c8130` / `0x4c80c0` / `0x4c01f0` | bow shoot (slot `0x330`) / bow aim / arrow hit | — | **not read** (§2.18) |

## 2. Behaviour

### 2.1 TPlayer::InvokeQuickSpell `0x51b5d0`

Quick spells are 5 talisman strings of 6 bytes at TPlayer `+0x2cc + 6·i`.

1. `button >= 5` → return 0. Health (slot `0x1c0`) `<= 0` → 0.
2. Unless charflags (`+0x110`) has `0x80000`: return 0 when doing (`+0xd8`)
   has an attack with CA `0x2000000` (`attack+0x24`) or an impact with
   flag `0x80` (`impact+0x24`).
3. Empty slot: the main player (`== [0x667fcc]`) gets the text `SPLNOTAL`.
   Then CastByName("Fizzle", 0, 0, 0). Return 1.
4. Not HasTalismans(slot):
   - Multiplayer client (`[0x66829c] && ![0x67682c]`): return 1 with nothing
     done.
   - Otherwise the main player gets `SPLMISTAL`, then CastByName("Fizzle").
     Return 1.
5. Has the talismans: CastByTalismans(slot, **no targets**, 0, 0).
   - Fails: in single player, CastByName("Fizzle"), then `SPLCASTFAIL` for
     the main player.
   - Succeeds: `SPLCASTOK` for the main player.
   - On a multiplayer client, neither text. Return 1.
   - When it fails, `0x4d5c20` has usually cast "fizzle" already (§2.4). The
     second Fizzle cast then fails on the cooldown it just set, unless
     Fizzle's WAIT_NEXT (20) has elapsed.

Text goes through the string table (`0x49d800` on `0x65d4d0`) and
`0x54d170(0x65c5d0, "%s", s)` (text bar): a seam.

### 2.2 TPlayer::HasTalismans `0x51b7c0`

- pouch = FindInventory("Spell Pouch") (slot `0xa8`). If that is null it
  looks up "spellpouch" but **discards the result**, so no pouch means no
  counts.
- With a pouch, for each TALISMAN type *i* (`0x66deec` count, `0x66defc`
  types) it counts the pouch items whose name (`+0x38`) stricmp-equals the
  type's name.
- For each character of the string and each type, the type's `Code` stat
  (class.def: Sun 65 'A', Life 'B', Ocean 'C', Law 'D', Soul 'E', Stars 'F',
  Death 'G', Chaos 'H', Sky 'I', Earth 'J', Ward 'K', Moon 'L') is compared
  **case-sensitively** with the character. A match decrements that type's
  count, and a count going below 0 makes the answer 0.
- Codes M, N, O, P (monster spells, Fizzle "PEP") have no type, so they
  never decrement anything: a string made only of them passes.

Port `TPlayer::HasTalismans` (player.cpp:1109): same algorithm, including
the discarded "spellpouch" lookup.

### 2.3 TCharacter::CastByName `0x4d5b90`

1. The gates of §2.4 steps 1 to 3.
2. v = GetVariantDataByName(name) (`0x53f010`: stricmp on variant `+0x04`,
   **variant names only**, first in file order). Null → 0.
3. Return CastByTalismans(v->talismans (`+0x24`), targets, numtargs,
   sourcepos).

So a cast by name casts **the first variant whose talisman string equals
the named variant's** (stricmp). No shipped talisman string is duplicated
(checked over all VARIANT lines), so this resolves back to the same variant.
Callers: script `cast` (`0x42114c`), monster AI "Priest Fireball"
(`0x4cd010`), the quick-spell Fizzles, and the spell panes (`0x546232`,
`0x5462f7`). A monster's magic attack inlines the same lookup (`0x4d227d`:
`attack+0x38` spell name, `attack+0x58` sourcepos, `&target`).

### 2.4 TCharacter::CastByTalismans `0x4d5c20`

1. Health `<= 0` → 0.
2. Unless charflags `0x80000`: doing's attack has CA `0x2000000`, or its
   impact has flag `0x80` → 0.
3. Object flags (`+8`) `& 1` or `& 0x2800000` → 0.
4. Player with state bit 2 (`+0x36c`): SetPlayerState(state & ~2)
   (`0x51d680`).
5. Network authority (slot `0x178`, `0x4d6020`):
   - With `+0x264 == 0` it returns `[0x676838]`, the session's `+0x100`.
     The session reset `0x578210` sets that to 3. It must be ≥ 2 for a
     single-player cast; that the runtime value is 3 is **unverified**.
   - In multiplayer, `< 1` without `[0x676e5c]` → 0.
6. Authority ≥ 2 (local):
   - ok = `spells(+0x170).CastByTalismans(tal, this, targets, numtargs,
     sourcepos, 0)`.
   - **ok:** network echo `0x584870` (a no-op without a session,
     `[0x676828] == 0`). For a player, v = GetVariantDataByTalismans(tal);
     when `v->mana > 0`, net message `SPLCASTOK` (`0x587280`, a no-op in
     single player). Return 1.
   - **fails:** for a player, v = GetVariantDataByTalismans(tal). When v and
     `v->mana > 0`: net `SPLCASTFAIL`, the gates of steps 1 to 3 again, then
     CastByTalismans(GetVariantDataByName("fizzle")->talismans, 0, 0, 0),
     recursively. Fizzle costs 0 mana, which ends the recursion. Return 0.
     Non-players return 0 with no fizzle.
7. Authority 1 (multiplayer client): send the request (`0x584870`), return 1.

### 2.5 TSpellManager::CastByTalismans `0x53fe80` (the cast engine)

`this` = the character's spell manager (`+0x170`): count `+0x0`, items
`+0x10`, cooldown `wait` `+0x14`.

1. spelldata = the first SSpellData with a variant whose talismans
   (`+0x24`) stricmp-equal `tal`. variant = the first such variant
   (variants in order, `0x540ad0`). Either null → 0. **Order-sensitive,
   case-insensitive, first match wins.**
2. `wait != 0 && [0x668154] == 0` → 0 (`0x668154` is a debug no-wait flag,
   0 in play).
3. cost = `variant->mana` (`+0x4c`). With an invoker:
   1. Player: `cost += trunc(-(ManaCostPct · cost) / 100)` (slot `0x3c4`,
      ManaCostPct, TPlayer only).
   2. `Mana()` (slot `0x1d0`) `< cost` and `[0x66810c] == 0` (no-mana
      cheat): the main player gets text `SPLMANA` (the multiplayer server
      variant goes through `0x587280`). Return 0 for every invoker.
   3. `variant->skill` (`+0x5c`) `>= 0` and no cheat:
      - **r = random(1, 100)**, drawn for every invoker.
      - For a player: `d = Invoke (stat 42) − skill`. `chance` is 100 for
        `d ≥ 0`, 80 for −1, 40 for −2, 20 for −3, 0 for ≤ −4.
      - `chance < r` fails, **for the main player only**: text `SPLLVLNEG`
        (chance 0) or `SPLLVLLOW`, return 0. Any other invoker, including a
        remote player, goes on.
   4. Player: reads InvokeExp (stat 53; the value is unused), then
      Invoke (42).
      - When `skill > 0`: `base = ((5·Invoke + 15)·20) / 8`,
        `k = clamp(skill − Invoke, −9, 9) + 10`,
        `exp = (k · base · 10) / 100`.
      - AddSkillExp(2, exp) (slot `0x418`, TPlayer `0x51ac90`).
      - This is awarded **before** the spell exists and whether or not it
        works.
4. Spell class: `name = variant->controldata ? controldata->name ("Strike")
   : "spell"`. It is looked up by stricmp in the registry `0x670220`
   (count `0x67021c`; entries `+4` = name; registered: "Spell" vtable
   `0x5b9bd4`, "Strike" `0x5b9bd8`). It is assumed to be found (a miss
   would call through null).
5. **Buff replacement**: when the variant has a STATLINE (`+0x74`) and the
   invoker has charflags `0x20` (a spell stat effect is active):
   - TMapIterator(invoker, 0, 5 (OBJSET_ANIMATE), 0, −1) (`0x44cf10`) over
     the invoker's surroundings (range **unverified**).
   - For each EFFECT (class `0x19`) whose spell (`+0xd8`) variant has a
     STATLINE: when that variant's controldata has a sound (`+0x74`), find
     it (`0x49c430`) and stop it (`0x49bd90`, unverified name). When that
     spell's invoker is the casting invoker (**arg 2**), kill the effect
     (`0x4defe0`).
6. spell = class->create(invoker, targets, numtargs, sourcepos, spelldata,
   variant, master): the TSpell constructor, §2.8.
7. `spell+0x11c` = spelldata and `spell+0x120` = variant, set again by
   repeating the talisman searches.
8. With an invoker: SetCast(spelldata->invoke (`+0x84`), targets ?
   targets[0] : 0, variant->ani_delay (`+0x6c`)).
9. `this->wait = variant->nextspellwait` (`+0x50`, **raw, not ×24**).
10. ManaDrain (§2.7), add the spell to the list, return 1.

RNG order inside one cast: the skill roll (step 3.3, when skill ≥ 0), then
the constructor's poison rolls (one per target, §2.8).

### 2.6 `0x540460` (network cast)

Same lookups, buff replacement, create, SetCast, wait, list add. No wait
gate, mana gate, skill roll or ManaDrain call (body skimmed; order not
read). Only caller: network receive `0x5818ec`.

### 2.7 TSpell::ManaDrain `0x53f680`

- No invoker → return. Cheat `[0x66810c]` with a player invoker → return.
- `cost = mana`. A player gets the same ManaCostPct reduction as §2.5.
- SetMana(Mana() − cost) (slot `0x1d4`); if Mana() > MaxMana() (slot
  `0x1e8`), SetMana(MaxMana()).
- No status-bar push.

### 2.8 TSpell::TSpell `0x53f090` (vtable `0x5b99c0`, 0x150 bytes, zero-filled by `0x482fb0`)

1. `+0x13c` = an empty pointer array (`0x41c7f0`, size 14, grow 4) of the
   spell's effects.
2. `+4` = invoker. numtargs is clamped to `[1, 64]` into `+0xc`.
3. `targets == NULL`: `+0x10` (targets[0]) = invoker. **No poison roll.**
4. Otherwise the targets are copied to `+0x10`. When `spelldata->
   poisonchance` (`+0xc8`) `!= 0`, for each target: **random(0, 100)**,
   and when `< poisonchance`: `target->slot 0x1b4(1)` (SetPoisoned: writes
   Poisoned via `0x4d6eb0` / SetObjStat). This is called on whatever the
   target is; no class check.
5. `+0x120` = variant, `+0x11c` = spelldata, `+0x110` timer = −1,
   `+0x118` = master, `+0x124` wait = spelldata->delay (`+0xa4`).
6. `variant->statline` (`+0x74`) and invoker is a player: invoker charflags
   `|= 0x20`, then AddStatEffect(statline) (`0x51c2c0`, see
   [PLAYER_STATS.md](PLAYER_STATS.md) §4). This applies to the **invoker**,
   not the target.
7. `sourcepos` → `+0x128/+0x12c/+0x130`, else −1, −1, −1.

### 2.9 TSpell layout (retail)

`+0x00` vtable, `+0x04` invoker, `+0x08` last effect created, `+0x0c`
targetnum, `+0x10` targets[64], `+0x110` timer, `+0x114` frame (Strike),
`+0x118` master, `+0x11c` spelldata, `+0x120` variant, `+0x124` wait
(DELAY), `+0x128` source xyz, `+0x134` magic defense, `+0x138` magic
offense (summed by `0x5407d0` / `0x540820`), `+0x13c` effect list (count
`+0x13c`, items `+0x14c`), `+0x150` Strike target (Strike only, size
0x154).

### 2.10 TSpell::Timer `0x53f410`: effect creation

At `wait == 0`, with `wait = −1` set first:

- An SObjectDef (0x34 bytes, zeroed): objclass `0x19`, level `[0x666970]`,
  pos = invoker pos (`+0x10/14/18`) + source (or + 0).
- **No right-hand position and no HEIGHT fallback** (the 1998 code used
  both).
- facing = `variant->facing` (`+0x68`) ? invoker facing (`+0x36`) : 0.
- objtype = EffectClass (`0x66cc68`).FindObjType(variant->effect (`+0x2a`),
  0).
- `effect = MapPane.GetInstance(MapPane.NewObject(&def, −1, 0))`
  (`0x450e40` / `0x452690`), stored in `+8`. When it exists:
  effect->SetSpell(this) (`0x4de7d0`), then add it to `+0x13c`.
- There is no "no effect → timer = 0" rule.

### 2.11 TSpell::Kill `0x53f860`

- `timer = 0`.
- For each effect in the list:
  - When `spelldata->poisonchance` and `targetnum > 0` and targets[0]:
    `targets[0]->slot 0x1b4(0)`. **Ending a poison spell cures its first
    target.**
  - effect flags `|= 0x1000` (OF_KILL) via slot `0x40`.
  - effect->SetSpell(0).
  - Remove it from the list.

### 2.12 TSpell::Damage `0x53f560`

```
min = variant+0x54; max = variant+0x58
if (!target) return
if (target+0x190 (float) > 0.0f)            // magic resistance; TCharacter ctor sets 0
    min -= ftol(res*min); max -= ftol(res*max)
d = random(min, max)                        // RNG
inv = this+4
if (inv && inv is player) d = ((SpellDamageInc + 100) * d) / 100       // slot 0x3cc
if (target is player)     d = ((100 - Resist(6)) * d) / 100            // slot 0x2c8 = copy[12] DmgResMagical
target->Damage(d, spelldata->damagetype (+0x80), 0, 0, inv)            // slot 0x228 = 0x4c4950
if (inv && inv is player) inv->slot 0x414(target)                      // kill experience 0x51a630
```

Damage `0x4c4950` then runs CalculateDamage (`0x4c4860`, resist by type,
armor) when type ≥ 0. With the attacker passed, it refuses non-enemies
(IsEnemy `0x4c89c0`) and attackers flagged `0x2800000` (kata C2). The
DmgResMagical step and the CalculateDamage resist both apply to a player
target. DT_NONE (−1) spells skip CalculateDamage.

### 2.13 The Strike spell class (CONTROLDATA "Strike", vtable `0x5b9bdc`)

**Pulse `0x540d70`.**

1. TSpell::Pulse, then `frame` (`+0x114`) += 1.
2. REPEATDAMAGE (controldata `+0xb0`) set and `frame % 24 == 0` and an
   invoker:
   - tgt = invoker's root block obj (`root+0x44`) when the root action is
     3 or `0x19`, else 0.
   - When `+0x150` is set: tgt `!= +0x150` → Kill. `+0x150`'s Health
     `<= 0` → Kill. Else Damage(tgt).
   - **No shipped variant has REPEATDAMAGE**, so this damage never runs
     with retail data.
3. DURATION (`+0x2c`) `!= −1` and `frame >= DURATION`: Kill. Then, when the
   invoker's root is combat with an obj, `spelldata->poisonchance` is set,
   and the obj isn't the invoker: obj->SetPoisoned(0).
   - Poison / SpiderPoison: `DURATION 1080 24`, so the poison lasts 1080
     ticks (45 s at 24 Hz) and is then cured.

**Timer `0x540eb0`** (partly read):

1. Counts down `timer` / `wait` like the base.
2. At `wait == 0`, it picks the anchor target.
   - HITTARGET and not ONCASTER: the invoker's root obj while it is in
     combat (`0x45f770(3)` or root action `0x19`). **If there is none, it
     returns 1** (the spell ends without effect or damage).
   - No invoker: targets[0].
   - Otherwise: the invoker.
3. Builds the effect def (`0x540bc0`).
4. `+0x150` = the combat obj (or the target); targets[0] = `+0x150`.
5. Plays PLAY / PLAYONCE.
6. Creates one effect per HITS (ATTACHMULTIPLE), with IMAGERY, FOLLOW
   (`0x4def90`), DURATION on the effect (`+0xe8`, `+0x12c`), `+0x134` =
   DURATION2 and **random(0, WAIT)** into effect `+0x138` per effect.
7. MULTIPLETARGETS walks characters in RADIUS around the invoker.
8. PATTERN placement draws more randoms (`0x54146e`, `0x5416bb`,
   `0x5416c9`, `0x54194b`, ...).

The full branch structure, and the vt+0xc override `0x542340`, are
**unverified**.

**BuildEffectDef `0x540bc0`.**

- objclass `0x19`, level `[0x666970]`, pos = target pos plus:
  - POS (`+0xac`): + `(+0xb8, +0xbc, +0xc0)`;
  - else ATTACH (`+0xa4`): + the target animator's object-map position of
    the ATTACH name (`+0x50`);
  - else: + (50, 50, 50);
  - then + the source position (GetSourcePos `0x53f1f0`, always taken).
- facing as §2.10. objtype = FindObjType(variant->effect).

### 2.14 Per-effect damage (the call sites)

Spell names are from `spell.def` (effect name → variant). Only the
behaviour read in the asm is listed.

- **FireFlash** `0x4e1770` (variants "Fire Flash" LI, "Priest Fire Flash"
  MA):
  - tgt = the caster's root obj when the caster's root action is 3 or `0x19`.
    It then calls Burn(tgt) (`0x4d3590`, "onfire" + a `BURN` effect),
    KnockBack(tgt, effect pos, −1) (`0x4d3750`) and spell->Damage(tgt).
    `+0x184 = 0`.
  - Runs once from the animator's Initialize (§0).
  - The effect's Pulse `0x4e17f0` counts `+0x184` to 100. Then it cures
    targets[0] when the spell has poisonchance, sets OF_KILL and
    `0x8000`, and calls spell->Kill.
- **BURN** `0x4e3020` (Pulse; target `+0x184` set by `0x4e2fa0`):
  - ++`+0x188`. **r = random(0, 34)** every pulse.
  - `r < 2 && +0x188 < 50`: with a spell, spell->Damage(target); without
    one, target->Damage(**random(1, 2)**, 7, 0, 0, 0).
  - Burn spawned by Burn() has no spell, so it is the second branch.
- **FireBall** missile `0x510c10`: §2.16. AreaDamage radius 150 on
  explode.
- **FIRECONE** `0x4e9d60` (Napalm), **Quicksand** `0x4ed3d0` (Quicksand,
  Swamp Pit), **Paralize1** `0x4fadb0` (Paralysis ×3; 2 sites),
  **RockStorm** `0x501790`, **LightningStorm** `0x503580` (variants not
  traced; "LightningStorm" is a class.def type), **JhagaAttack**
  `0x505840`, **DragonAttack** `0x505a90` (monster attacks), **WindStrip**
  `0x522800` (lightning strip, "lightstrip" variants Electric / Priest /
  HighPriest Bolt), **Puke** animator `0x4fcda0`: each calls TSpell::Damage
  from its Pulse. Their target selection, hit tests and own RNG are **not
  read**.
- The port's Paralyze / Puke / ManaDrain (effect2.cpp), RockStorm
  (effect3.cpp) and Lightning / WindStrip (stripeffect.cpp) must each be
  checked against these bodies.
- **Strike** `0x540d70`: §2.13 (REPEATDAMAGE, unused).

### 2.15 AreaDamage `0x4de3c0` and its siblings

`AreaDamage(attacker, pos*, radius, min, max, type, minradius)`, cdecl.

- attacker counts only when it is a character or player.
- TMapIterator(pos, radius, `0xe0`, 2 (characters), 0, level).
- For each character other than the attacker, with
  `minradius <= Distance(pos, its pos) <= radius` (`0x46de60`), Health > 0,
  an enemy of the attacker (when there is one), and doing action `!= 0xc`:
  1. d = **random(min, max)**, once per target.
  2. Attacker is a player: × (100 + SpellDamageInc) / 100.
  3. Target is a player: × (100 − DmgResMagical) / 100.
  4. **KnockBack(target, pos, −1) first**, then Damage(d, type, 0, 0,
     attacker), then kill exp for a player attacker.
  - No magic-resistance (`+0x190`) step.
- Callers: `0x4dee7c` (in TEffect::Pulse `0x4de800`), `0x4e232b` (`0x4e2290`,
  vtable `0x5a9310`), `0x4fcbd5` (`0x4fca70`, vtable `0x5ae3c0`),
  `0x4fd5ec` / `0x4fd62e`, `0x4fead6` (`0x4fea00`), `0x50676c` (`0x5066e0`),
  FireBall `0x510cb8`, `0x5131ed` (`0x513140`), `0x52263d`, Strike Timer
  `0x54155c`. These are not mapped to classes yet.
- `0x4de560` AreaBurn(attacker, pos, radius): Burn() on living enemies in
  range.
- `0x4de610`: for **dead** enemies in range, KnockBack with
  `random(7, 12)`, `random(6, 16)` (`0x4de720`) and kill exp.

### 2.16 Missiles (TMissileEffect, object vtable `0x5b3e18`)

The base vtable is stored first by every missile creator: Photon `0x514950`,
FireBall `0x514af0`, FIRECOLUMN `0x514ce0`, FLAMEDISC `0x514e70`. The
brief's `0x5b401c` is the Photon **animator** vtable (builder `0x5b4018`).

Fields: `+0x184` life / counter, `+0x188` speed (16.16), `+0x18c` "fly"
flag, `+0x190` Photon sound-once flag, `+0x194` FireBall damage-armed flag.

- **Init** (`+0x1fc`): SetState(0); `+0xe0 = 0`; `+0x184 = 0x8000`;
  `+0x18c = 1`; speed:
  - base `0x5101b0`: `0x100000` (16/tick);
  - Photon `0x510400`: `0x100000`, plus `+0x190 = 1` and the "lightning"
    sound loaded;
  - FireBall `0x510bd0`: `0x80000` (8/tick), plus `+0x194 = 1`.
  - Who calls `+0x1fc` on missiles is **unverified** (for FireFlash it is
    the animator's Initialize).
- **Pulse `0x510220`.** It first calls Move (slot `0x114`). The low byte of
  Move's return is the blocked bits. Then it switches on state (`+0xc`):
  - **0 (launch)**, when `+0x18c`:
    - With speed: velocity from speed and facing (`0x4df070` →
      `0x46db20`); flags `= (flags & ~1) | 0x10008` (moving, weightless);
      `+0x24 = −(speed / 16)`;
      **life `+0x184 = 480 / (speed >> 16)`** (30 ticks at 16, 60 at 8).
    - Then SetState(1).
  - **1 (fly)**:
    1. `life -= 1`; `timeout = life <= 0`.
    2. Move blocked (`& 2`) → explode.
    3. Otherwise caster = spell ? spell->invoker : 0, and it walks
       TMapIterator(pos, 256, `0xe0`, characters, 0, level). The first
       character `c` other than the caster with Health > 0 and
       `Distance(this, c)` (slot 4) `<= 32` explodes the missile when there
       is no caster or `IsEnemy(caster, c)`.
    4. No hit and timeout → explode.
    5. Explode: `flags = (flags & ~0x10008) | 1`; SetState(2).
  - **2 (exploding)**: when the animator (`+0x58`) is gone, the object is
    killed (`0x4defe0`).
  - Every path ends in TEffect::Pulse `0x4de800`.
- **The base missile does no damage.** FireBall's Pulse `0x510c10`, in
  state 2 with `+0x194` and a spell: pos = missile pos + the animator's
  float offset (`+0x4ac/4b0/4b4`), then AreaDamage(invoker, pos, **150**,
  variant min, variant max, spelldata type, 0), then `+0x194 = 0`. Photon's
  Pulse `0x510450` only plays the "lightning" sound on the first fly tick.
  Whether Photon damages elsewhere (its animator `0x5104c0`) is
  **unverified**. FIRECOLUMN / FLAMEDISC were not read.
- No RNG in the missile base.

### 2.17 spell.def: SSpellData and SSpellVariant layouts

**SSpellData** (loader `0x53e4e0`, thiscall(name, t) ret 8).

| Offset | Field | Source |
|---|---|---|
| `+0x00` | variant array: count `+0`, items `+0x10`, default `+0x14` | VARIANT |
| `+0x18` | flags | FLAGS |
| `+0x1c` | name[31] (+NUL at `+0x3b`) | SPELL "name" |
| `+0x3c` | objname | NAME %30s |
| `+0x5c` | icon name | ICONNAME %30s |
| `+0x7c` | description (malloc) | DESCRIPTION |
| `+0x80` | damage type | DAMAGETYPE |
| `+0x84` | invoke animation | ANIMATION %30s |
| `+0xa4` | delay (effect start) | DELAY |
| `+0xa8..+0xc7` | light: color bytes `+0xaa/+0xa9/+0xa8`, INT `+0xac`, MULT `+0xb0`, POS `+0xb4..+0xbc`, FADEIN `+0xc0`, FADEOUT `+0xc4` | LIGHT |
| `+0xc8` | poison chance | POISONCHANCE |

The LIGHT field order is read from the push order; verify before use.
Any other tag → "Invalid spell tag %s" (fatal).

**SSpellVariant** (0x78 bytes, malloc'd copy).

| Offset | Field |
|---|---|
| `+0x00` | flags (0) |
| `+0x04` | name[32] |
| `+0x24` | talismans[6] (5 chars max) |
| `+0x2a` | effect name[32] |
| `+0x4c` | mana |
| `+0x50` | nextspellwait (**raw**) |
| `+0x54` | min damage |
| `+0x58` | max damage |
| `+0x5c` | skill |
| `+0x60` | type |
| `+0x64` | height |
| `+0x68` | facing |
| `+0x6c` | ani_delay |
| `+0x70` | controldata* (or 0) |
| `+0x74` | statline* (malloc 100, or 0) |

The VARIANT line is
`%s, %i, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i` = name, type, talismans,
effect, mana, wait, min, max, skill, height, facing, ani_delay.

**ControlData** (0xc4 bytes, zeroed; HITS = 1, `+0x2c` / `+0x40` / `+0x94`
= −1), loaded by `0x53dd10`.

| Offset | Field |
|---|---|
| `+0x00` | class name ("Strike") |
| `+0x20` | flags: 2 SHAKE, 4 PLAY, 8 PLAYONCE |
| `+0x24` | RADIUS |
| `+0x28` | HITS |
| `+0x2c` | DURATION a |
| `+0x30` | WAIT |
| `+0x34` | DURATION b |
| `+0x38` | PATTERN: 1 RANDOM, 2 CIRCLE, 3 LINE, 4 CLUSTER, 5 X, 6 SPIRAL |
| `+0x3c` | SHAKE |
| `+0x44` | REPEATDAMAGE |
| `+0x48` / `+0x4c` | RANGEDAMAGE |
| `+0x50` | ATTACH name |
| `+0x70` | ATTACHMULTIPLE names (HITS × 32 bytes; HITS ≤ 20) |
| `+0x74` | PLAY / PLAYONCE sound name |
| `+0x94` | IMAGERY handle (`0x446aa0`) |
| `+0x9c` | HITTARGET |
| `+0xa0` | ONCASTER |
| `+0xa4` | ATTACH set |
| `+0xa8` | FOLLOW |
| `+0xac` | POS set |
| `+0xb0` | REPEATDAMAGE set |
| `+0xb4` | MULTIPLETARGETS |
| `+0xb8..+0xc0` | POS xyz |

STATLINE (`variant+0x74`) is the rest of the line, up to 100 characters.

Shipped data notes:

- RANGEDAMAGE (Meteor Storm 150 30 / 150 25, Cataclysm 500 35), DURATION
  (Poison) and STATLINE buffs are used.
- REPEATDAMAGE isn't used.
- Quicksand and Swamp Pit have WAIT 10; Meteor Storm WAIT 48.

### 2.18 Arrows (not read)

`0x4c8130` (slot `0x330`), `0x4c80c0` and `0x4c01f0` were not
disassembled for this doc. The brief's string anchors for `0x4c01f0` are
ice_arrow, Solifuge, skywalk, damagemod, Poison, Fire_Flash and
FULLARROWDMG/BASEARROW, with 4 random calls. The port's `TArrow3D::Move`
(`ammo.cpp:306`) does `Damage(random(20, 50), DAMAGE_PIERCING)`, which is
certainly not retail. **Open: a follow-up pass.**

## 3. RNG draws (all `random()` `0x483300` unless noted)

| Site | Range | Decides | When |
|---|---|---|---|
| `0x540037` | (1, 100) | cast skill roll | every cast with `variant->skill >= 0` and no cheat, after the mana check, **for every invoker** (only the main player can fail) |
| `0x53f121` | (0, 100) | poison per target: `< POISONCHANCE` | in the constructor, only with an explicit targets array and POISONCHANCE ≠ 0; once per target |
| `0x53f5c7` | (min', max') | spell damage | TSpell::Damage, after the magic-resistance cut |
| `0x4e303c` | (0, 34) | BURN tick: `< 2` hits | every BURN pulse |
| `0x4e307d` | (1, 2) | BURN damage without a spell | when it hits |
| `0x4de495` | (min, max) | AreaDamage per target | each target in range |
| `0x4de724`, `0x4de72d` | (7, 12), (6, 16) | corpse knockback | `0x4de610` |
| `0x5412bb` and others in `0x540eb0` | (0, WAIT) and pattern draws | Strike effect stagger / placement | Strike Timer at effect start (**unverified count**) |
| effect Pulses (§2.14 not read) | ? | ? | unverified |

Quick-spell flow order: HasTalismans (none) → CastByTalismans: the skill
roll → the constructor's poison rolls → (later ticks) the effect's
rolls → Damage's roll.

## 4. Fixture plan (seams)

**Run as original code:**

- `0x51b7c0` (with the inventory iterator seamed or a real pouch), the
  variant lookups `0x53ef90` / `0x53f010` / `0x540ad0` over a guest
  TSpellList built from shipped spell.def (or run the loader `0x53e4e0`
  over the file bytes with the TToken seam).
- `0x53fe80`, `0x53f680`, `0x53f090`, `0x53f560` (with Damage as a seam),
  `0x540750`, `0x53f410` (with NewObject as a seam), `0x53f860`, `0x510220`
  (Move, iterator, Distance, IsEnemy seamed), `0x4de3c0`.

**Seams and what they must record:**

| Seam | Retail | Record |
|---|---|---|
| object stats | GetObjStat slot `0xdc`, and the accessors `0x1c0` Health, `0x1d0` Mana, `0x1d4` SetMana, `0x1e8` MaxMana, `0x3c4` ManaCostPct, `0x3cc` SpellDamageInc, `0x2c8` Resist | stat id, value; SetMana value |
| text bar | `0x49d800` + `0x54d170` | the key (SPLMANA, SPLLVLLOW, SPLLVLNEG, SPLNOTAL, SPLMISTAL, SPLCASTOK, SPLCASTFAIL) |
| network | `0x584870`, `0x587280`, slot `0x178` | calls (expect none in single player); answer 3 |
| skill exp | slot `0x418` (2, exp) | args |
| kill exp | slot `0x414` (target) | target |
| stat effect | `0x51c2c0` AddStatEffect | line |
| animation | SetCast → BuildActionName `0x4ce1b0` (slot `0x310` prefix), HasActionAni slot `0x1f0`, ForceCommand slot `0x218` | action name, wait, obj |
| map | NewObject `0x450e40` / GetInstance `0x452690`, FindObjType `0x475210`, SetSpell `0x4de7d0`, TMapIterator `0x44cf10` / `0x44ceb0` / `0x44d080`, kill object `0x4defe0` | the def (class, type name, pos, facing, level) |
| damage | slot `0x228` (d, type, a3, a4, attacker), SetPoisoned slot `0x1b4`, Burn `0x4d3590`, KnockBack `0x4d3750` | args, order |
| sound | `0x667548` methods `0x49c430` / `0x49bd90` / `0x49b650` / `0x49b990` | name |
| motion | Move slot `0x114` (return bits), Distance slot 4 / `0x46de60` | answers |

**Globals and realistic values:**

| Global | Meaning | Value in play |
|---|---|---|
| `0x667c38` | TSpellList (`+4` count, `+0x14` items) | loaded from spell.def |
| `0x670220` / `0x67021c` | spell-class registry | "Spell", "Strike" |
| `0x667fcc` | main player | the player |
| `0x668154` | no-wait cheat | 0 |
| `0x66810c` | no-mana / no-skill cheat | 0 |
| `0x66829c` | multiplayer | 0 |
| `0x676828`, `0x67682c` | session up / server | 0 |
| `0x676838` | local authority | 3 (unverified, must be ≥ 2) |
| `0x65d0c4` | PlayScreen `+0x5d4` (pause) | 0 |
| `0x666970` | map level | case input |
| `0x66cc68` | EffectClass | |
| `0x66dedc` / `0x66deec` / `0x66defc` | TALISMAN class, type count, types | class.def |

## 5. Port divergences

Port files: `src/spell.cpp/.h`, `src/character.cpp`, `src/player.cpp`,
`src/ammo.cpp`, `src/missileeffect.cpp`. Line numbers are from 2026-10-08.

1. **Talisman matching.**
   - Retail matches the talisman string exactly (stricmp, order-sensitive)
     and takes the first variant in file order.
   - The port (`GetSpellDataByTalismans` / `GetVariantDataByTalismans`,
     spell.cpp:246/326) compares talisman *counts* (multiset) and keeps the
     **last** match. "IL" casts Fire Flash in the port and nothing in
     retail.
2. **Cooldown ×24.** The port multiplies NEXTSPELLWAIT by 24 at load
   (spell.cpp:119); retail stores it raw. The port's cooldowns are 24 times
   longer (Fire Flash 1200 ticks instead of 50).
3. **No skill roll, no skill exp.** The port's `TSpellManager::CastBy*`
   has no `random(1, 100)` fail roll (chance table 100/80/40/20/0 by
   Invoke − skill), no SPLLVLLOW / SPLLVLNEG, no AddSkillExp(2, ...).
4. **Mana.**
   - The port ignores ManaCostPct in both the gate and the drain, and
     ignores the no-mana cheat.
   - The port's ManaDrain pushes the StaminaBar, which retail doesn't.
5. **CastByName.**
   - Retail resolves the name to a variant, then casts by that variant's
     talismans.
   - The port's `GetSpellDataByName` also matches the *spell* name
     (spell.cpp, `GetSpellDataByName`), which retail never does.
   - The port's CastByName / CastByTalismans in TCharacter
     (character.cpp:5401/5407) skip all of retail's gates: dead,
     attack/impact flags, object flags `0x2800000`, player state bit 2.
   - The port does not fizzle inside `CastByTalismans` on failure.
6. **Buff replacement and stat effects.**
   - The port has no STATLINE / controldata, so it has no AddStatEffect on
     cast, no charflag `0x20`, and does not kill the old buff effect.
   - CONTROLDATA blocks are skipped (spell.cpp:130 onward). The Strike
     class (DURATION, HITTARGET, ATTACHMULTIPLE, MULTIPLETARGETS, PATTERN,
     WAIT, RANGEDAMAGE, PLAY) does not exist.
7. **TSpell::Damage** (spell.cpp:502):
   - missing SpellDamageInc for a player caster;
   - missing the player target's DmgResMagical cut;
   - missing the attacker argument to Damage, which drops IsEnemy and
     friendly-fire behaviour;
   - missing the kill-experience call.
   - The port casts the target to TCharacter without retail's null check.
8. **Timer / Kill.**
   - The port's Timer kills a spell whose effect failed to spawn
     (`!effect && wait < 0`); retail doesn't.
   - The port places the effect at the right hand, with a HEIGHT fallback;
     retail uses `invoker pos + source` only.
   - The port's Pulse loop deletes a spell without calling Kill. Retail
     calls `vt+0xc` Kill first: it cures a poison target, sets OF_KILL on
     the effects and detaches them.
9. **Poison roll.** The port (spell.cpp:396) now rolls `random(0, 100)` per
   target like retail, but only SetPoisoned's TCharacter targets (retail
   calls slot `0x1b4` on any object), and in retail's order relative to
   the skill roll only once (3) lands.
10. **SetCast** (character.cpp:5233):
    - retail derives the name generically (`<prefix>inv<last char>`;
      prefix from slot `0x310` for the player, "c" for others), with a
      "walk"-root `w` variant;
    - the port hand-codes invoke1..5 (invoke5 writes `inv1`, a typo);
    - retail clears the priority flag, the port sets priority = true;
    - retail sets `block->wait = delay` and `+0x188 = delay`.
11. **Tick path.** Retail ticks spells in TCharacter::Pulse (§0). This is
    now ported (character.cpp:229). The skip while `[0x65d0c4]` is set is
    not. Effects in the port carry OF_PULSE (effect.h:50); in retail the
    spell pulses them.
12. **Missiles.** `missileeffect.cpp` is `#if 0`. The retail base missile
    is described in §2.16: life 480 / speed, hit at ≤ 32 on a living enemy,
    explode on blocked or timeout, FireBall area damage 150.
13. **Arrows.** `TArrow3D::Move` (ammo.cpp:306) deals `random(20, 50)`
    piercing. The retail formula is in `0x4c01f0`, which was not read
    (§2.18).
14. **InvokeQuickSpell** (player.cpp:1079):
    - the texts are hard-coded English instead of SPL* keys;
    - on cast failure the port fizzles once, retail fizzles twice (§2.1);
    - the port omits the Health and attack/impact gates;
    - the port prints the failure text after its Fizzle, the same order as
      retail.

## 6. Open questions

1. Is the quick-spell / spell-pane talisman string always in the variant's
   order (copied from the spellbook)? If the player can compose in any
   order, retail's stricmp makes order matter. Check the spell panes
   `0x545f10` / `0x542842` and SetQuickSpell.
2. `[0x676838]` (local authority) in a single-player run: 3 expected.
3. Effect `+0x44`, the pulse filter in TSpell::Pulse: meaning?
4. Who calls effect slot `0x1fc` for missiles (init)? FireFlash's comes
   from its animator.
5. Target selection and RNG in FIRECONE, Quicksand, Paralize1, RockStorm,
   LightningStorm, JhagaAttack, DragonAttack, WindStrip, Puke; the full
   Strike Timer; the Strike Kill `0x542340`.
6. Arrows: `0x4c8130`, `0x4c80c0`, `0x4c01f0`.
7. Can class.def effect types carry OF_PULSE, so that some spell effects
   are pulsed by the map as well as by their spell?

## 7. Recon labels corrected

- `0x53fe80` is TSpellManager::**CastByTalismans** (matches `+0x24`), not
  a by-name core. `0x4d5c20` is TCharacter::CastByTalismans, `0x4d5b90`
  TCharacter::CastByName.
- `cls_0x5b401c` is the Photon *animator* vtable; the missile object base
  vtable is `0x5b3e18` and its Pulse is `0x510220`.
- `cls_0x41c7f0` is the pointer-array helper; its `0x5407d0` / `0x540820`
  / `0x540750` / `0x5408d0` are TSpellManager methods.
- The `TPlayScreen::meth_*` labels on `0x4d5c20`, `0x4d3590`, `0x4d3750`
  and `0x4de7d0` are TCharacter / TEffect methods.
