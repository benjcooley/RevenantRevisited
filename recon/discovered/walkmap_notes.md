# Walkmap / TCharacter::Move retail vs. source diff

Anchors:
- `recon/discovered/cls_0x5a7b98_TCharacter_Move_4c46d0.cpp` — vtable wrapper
- `recon/discovered/cls_0x5a7b98_TCharacter_MoveStep_4c3bc0.cpp` — body of move
- `recon/discovered/cls_0x5a7b98_TCharacter_Blocked_4c39d0.cpp` — collision test
- `recon/discovered/cls_0x5a7b98_TCharacter_CharBlocking_4d4db0.cpp` — char-vs-char
- `recon/discovered/cls_TMapPane_GetWalkHeight_452e10.cpp` — walkmap cell read
- `recon/discovered/cls_TMapPane_GetWalkHeightRadius_4530a0.cpp` — radius walk
- Source side: `src/character.cpp:612` (Blocked), `src/character.cpp:658` (Move),
  `src/mappane.cpp:1969` (GetWalkHeight), `src/sector.h:163` (ReturnWalkmap).

## TCharacter::Move (0x4c46d0) — retail-only outer loop

Retail's vtable Move is a 10-step retry wrapper around MoveStep:

```c
iVar4 = 10;
while (true) {
    uVar3 = MoveStep();
    iVar4--;
    if (movetopos == 0) break;
    if (pos == movepos) break;          // reached target
    if ((uVar3 & 1) == 0) break;        // didn't move (MOVE_MOVED clear)
    if (iVar4 < 1) break;
}
```

Our `TCharacter::Move` is a single MoveStep equivalent — there is **no**
"call MoveStep up to 10x" loop. This matters mainly for `MoveTo()`
(scripted/teleport-to-target) movement: retail will iterate the entire
move pipeline up to 10 times in one tick to close the gap; ours does
one pipeline iteration plus the inner `nummoves` sub-step loop. Not
the cause of stuck-at-rocks, but worth porting for movetopos parity.

## Field-offset map (TCharacter, retail)

| offset | param[i] | meaning                                |
|-------:|---------:|----------------------------------------|
| 0x10   | [0x04]   | pos.x                                  |
| 0x14   | [0x05]   | pos.y                                  |
| 0x18   | [0x06]   | pos.z                                  |
| 0x1c   | [0x07]   | vel.x                                  |
| 0x20   | [0x08]   | vel.y                                  |
| 0x24   | [0x09]   | vel.z                                  |
| 0x28   | [0x0a]   | accum.x                                |
| 0x2c   | [0x0b]   | accum.y                                |
| 0x30   | [0x0c]   | accum.z                                |
| 0x36   | (short)  | face (moveangle is at +0xb0)           |
| 0xb0   | [0x2c]   | moveangle (used for shove probes)      |
| 0xd8   | [0x36]   | doing (TActionBlock*)                  |
| 0xec   | [0x3b]   | movetopos (bool)                       |
| 0xf0   | [0x3c]   | movepos.x                              |
| 0xf4   | [0x3d]   | movepos.y                              |
| 0xf8   | [0x3e]   | movepos.z                              |
| 0xfc   | [0x3f]   | chardata                               |
| 0x10c  | [0x43]   | forcenomove (1 = clear & return MOVE_NOTHING) |
| 0x11c  | [0x47]   | shovedir — stored as **offset** (-0x20/+0x20/-0x40/+0x40), not angle |

Chardata-relative speeds referenced inside MoveStep:
- chardata + 0x1ec — `combatwalkspeed` (when current root != "sneak"/"walk")
- chardata + 0x1f0 — `walkspeed`
- chardata + 0x1f4 — `runspeed` (`*0x1f4 = 500` decimal)

## Blocked (0x4c39d0) — equivalent to ours

Same shape: GetWalkHeightRadius, threshold 0x21 (= MAXZMOVE+1 = 33), then
`CharBlocking` tail-call (with a few extra gating bits we don't have:
`param_1[0x15]` flag word, `OF_PARALIZE` bit `0x80`, `param_1[0x3b]==0`
i.e. only test char-vs-char when not in movetopos mode).

Retail extra gate: `if (param_1[0x3b] != 0) skip char-blocking` — i.e.
when movetopos is active, we ignore other characters as obstacles. Our
version honors char-blocking unconditionally.

## Walkmap cell encoding — confirmed packed

`TMapPane::GetWalkHeight(pos, face, *out)` (0x452e10) does:

```
cell = sector->walkmap[(uy<<6) + ux];   // ushort
*out = ((uint16_t)cell) >> 10;          // top 6 bits = height
```

So the on-disk and in-memory layout is:
- bits 15..10 (6 bits) = walk height (0..63), units of "tile-Z snap"
- bits 9..0 (10 bits)  = group / connectivity / passability flags

Our `TSector::ReturnWalkmap` (sector.h:163) returns the **raw uint16**.
However, our pipeline doesn't load retail walkmap bytes off disk — we
build the walkmap fresh at sector load time by stamping each tile's
imagery walk byte (`uint8_t* walk` from `GetWalkMap(state)`) into the
sector grid via `TSector::WalkmapHandler(WALK_TRANSFER, ...)`
(`sector.cpp:420`). That handler computes:

```
walkval = walk_byte + zpos;     // 0..255 + zpos
walkval = clamp(walkval, 1, 0xffff);
*start = walkval;                // raw, NOT shifted
```

So our in-memory walkmap stores raw heights (0..0xffff), and our
`Move`/`Blocked` reads them as such. This is **internally consistent**
with the pre-release source — it just doesn't match retail's packed
layout. As long as we don't try to consume retail-packed walkmap dumps,
this is **not** the source of the stuck-at-rocks symptom.

> NB: if we ever want to read retail save/serialised walkmap data, we
> must either (a) shift `>> 10` on read, or (b) re-encode the stamper
> to pack height into bits 15..10. Mixing these is the failure mode.

## Shove logic (Move @ src/character.cpp:815-859 vs MoveStep @ 0x4c3bc0:304-369)

Both do "pick an angle offset from moveangle, probe Blocked at a few
units forward, commit if a path opens; remember the choice in
`shovedir` so we keep going around the same side". Differences:

| step | source (`character.cpp:815`)         | retail (`MoveStep:304`)                               |
|------|--------------------------------------|-------------------------------------------------------|
| try-set | offsets `±16, ±32` (= `±WALKMAPGRANULARITY << (i>>1)`) | offsets `±32, ±64` (`-0x20, +0x20, -0x40, +0x40`)     |
| order   | `-16, +16, -32, +32`                 | `-32, +32, -64, +64`                                  |
| distance probe | single test at distance 4 | inner loop 2 / 4 / 6 — record FURTHEST unblocked     |
| store   | `shovedir = (moveangle ± 64) & 255`  | `shovedir = ±0x20` or `±0x40` (the offset itself)     |
| commit  | move 4 units in stored angle         | move N (best-distance) units, set `local_70 = 1`     |
| outer loop | continues sub-step loop          | breaks sub-step loop on success (commits one shove per tick) |
| pos snap | snap z to walk-height after move    | snap z to walk-height after move                     |
| char-blocking during shove | yes               | yes (same Blocked path)                              |

Key behavioral implications:

1. **±32/±64 vs ±16/±32**. A 16-unit nudge sideways often puts the
   character right back into the same obstacle on the next sub-step.
   Retail's larger ±32 first try gives a much better chance to clear
   the corner of a rock or wall. **This is the most likely culprit
   for "stuck at rocks".**
2. **Distance probing 2/4/6**. Retail keeps trying further out at the
   same offset and picks the farthest spot that's still free. Lets the
   character commit to a longer sidestep when there is room, instead
   of always sliding 4 units.
3. **Commit-one-shove-and-break**. Retail breaks the sub-step loop the
   instant a successful shove is committed. Ours continues with the
   next sub-step on the same frame, which can re-trigger Blocked from
   a position that's only marginally cleared. The retail behavior
   gives the AdvanceAngles / face-correction path a frame to actually
   re-aim before the next collision test.

There is **no** A* / pathfinder behind retail. There is **no** "give
up after N ticks" retry. There is **no** target-radius-broadening on
block. Retail just has a more aggressive, better-distance-tuned shove.

## Diagnosis — "monster stuck at rock"

In priority order:

1. **Shove magnitudes too small.** Bump our `WALKMAPGRANULARITY << (i>>1)`
   shift to start at `WALKMAPGRANULARITY << 1` (32) and go up to 64,
   matching retail. Likely fixes the majority of cases.
2. **Single-distance probe.** Add an inner 2/4/6 distance loop and pick
   the furthest unblocked spot (mirror retail). Important for tight
   geometry where 4 units isn't enough.
3. **Commit-and-break.** After a successful shove, break out of the
   inner `nummoves` loop instead of continuing. Stops same-tick
   re-collision and gives AI/face-rotation a frame to settle.
4. **Movetopos retry wrapper.** Port the 10x outer loop from
   `TCharacter::Move (0x4c46d0)` for `MoveTo()` calls — currently we
   step once per tick, retail steps up to 10 in one tick when warping.
5. **Char-blocking gate during movetopos.** Retail skips char-vs-char
   blocking when `param_1[0x3b] != 0`. Worth checking if any of our
   stuck cases involve another character standing between target and
   path.

Walkmap data layout (10-bit group + 6-bit height) is **not** in the hot
path of stuck-at-rocks because our walkmap is built fresh at load time
from per-tile imagery, not deserialised from retail packs.

## Retail-only nav helpers worth porting

- `TCharacter::Move (0x4c46d0)` — the 10-step wrapper described above.
- Inside MoveStep, the `param_1[0x95..0x97]` zeroing on a successful
  shove — looks like a "scripted next-move offset" cache that we
  haven't ported. Field 0x254 group; cleared whenever the shove logic
  takes over, which suggests scripts can pre-stage a delta and the
  shove handler invalidates it. Low priority — verify with script
  callers before porting.
- The `combatwalkspeed` (chardata + 0x1ec) branch in MoveStep — chosen
  when `root` is neither "walk" nor "sneak" (falls through to the
  combatwalkspeed path). Our source picks combat speed only when
  `IsDoing(ACTION_COMBATMOVE)`. Retail also uses it as the **default**
  speed for any non-walk/non-sneak/non-run root, e.g. "block", "spell",
  "throw" etc. Worth verifying — may reduce "monster goes idle while
  blocked because root mismatched" cases.
