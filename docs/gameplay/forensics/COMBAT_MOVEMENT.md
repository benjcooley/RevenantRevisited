# Forensics: combat movement and orbit

**Topic:** how a character moves in combat in the shipped game: holding a
direction (Go), the per-tick combat resolvers (ResolveCombat /
ResolveCombatMove), facing the opponent while moving (the orbit), the
action-state core under them, and how far apart two characters are.
**Status:** 2026-10-07. Retail behaviour measured with the combat dojo's
katas M3 and M5 ([../COMBAT_DOJO.md](../COMBAT_DOJO.md)); port not yet
changed. Every difference below comes from running the original code in
the emulator beside the port on the same case.
**Evidence:** the retail disassembly (`recon/retail_asm/baseline`, the
unchanged executable), Ghidra decompiles in `recon/discovered/` and
`recon/classes/cls_0x5a7b98.cpp` (labels checked against the asm; several
are wrong, §7), the A/B dumps (`build/retail_ab/combat-go/`,
`combat-resolve/`).

Angles are 0–255 (64 = +x, 0 = −y). "Turn rate" is the action block's
`turnrate`, consumed by AdvanceAngles (`0x4c5ad0`).

## 1. CombatFace: facing the opponent while moving

`Revenant.ini` `[Options] CombatFace = Yes` (default; global `0x5d7a64`,
initialised 1, read by GetINISettings at `0x484500`'s option block, also
written by the Options screen `0x53aa90`). It gates three places:

- Go (`0x4ce350`), combat path: the new step's facing is
  `AngleTo(doing->obj)` when the target is valid and (CombatFace or the
  mover is a monster) and the character isn't combat-engaged (`+0x254`);
  otherwise the held direction.
- ResolveCombat (`0x4c7980`): a moving player keeps re-facing the target
  each tick only with CombatFace; monsters always.
- Go's clear-path probe (§2.4): in a combat root with CombatFace it always
  probes; without it (and outside combat) only when the held direction is
  within 16 of the facing (`0x4ce523`).

With it on, holding a direction sideways to the opponent plays the strafe
step for the angle between moving and facing (GetAngleMoveAnim `0x4d39f0`:
`f fr r br b bl l fl`, falling back to `f`) while the facing stays on the
opponent. **That is the orbit.** The port has no CombatFace; its Go always
faces the target when one is in range, its ResolveCombat is the 1998 one.

## 2. Go(angle) — what holding a direction does

Retail order (`0x4ce350`, thiscall, ret 4):

1. The move action for the root (`1,2→2`, `3,4→4`, `0x19,0x1a→0x1a`).
2. A player with state bit 2 (`+0x36c`) gets it cleared (TPlayer
   `0x51d680`).
3. Unless the character is in an interactive move (`charflags & 0x80000`,
   which skips these gates), returns 0 when doing isn't the root or move
   action, when **Health < 1**, when the doing block's attack has CA
   `0x2000000` or its impact flags `0x80`; in multiplayer when not allowed.
4. Probes 4 units ahead (FindClearPath `0x4c39d0`) per §1; blocked →
   returns 0.
5. Looks for a target in the direction held (FindCharacters
   `(1, −1, angle, 32, ENEMY|HEAR|SEE)`). As decompiled: it considers a
   switch only when that finds someone other than the current target and
   the current target passes IsValidTarget; it keeps the current one when
   the new one is more than 47 away and no nearer than the current one,
   else SetFighting(new).
6. Facing angle per §1, rounded to 8 directions for the pivot test only;
   the new block keeps the **unrounded** facing.
7. **Players never pivot in combat**: the step block is made at once
   (`objclass == 0xb` short-circuits the pivot test at `0x4ce9ad`).
   Monsters pivot first (root animation, `waitpivot`) when the facing is
   off by ≥ 65, or ≥ 33 while standing.
8. Already stepping in the same direction: returns 1 with no change;
   stepping in another direction: the current block is changed in place
   (moveangle, turn rate, interrupt) — the animation swap is left to
   ResolveCombat.
9. Turn rate `(max(0, |Δ| − 32) / 32) · 4 + 8`.
10. SetDesired (`0x208`); if refused, the new block is freed.
11. **Always, on success: `root->angle = root->moveangle = angle`**
    (`0x4cec63`), so the root remembers the last direction held.

Port (`TCharacter::Go`, character.cpp): the 1998 shape. Differences the
A/B shows (762 cases, 0 matching):

| | Retail | Port |
|---|---|---|
| Player, opponent to the side, standing | strafe step at once (`combatl`, `combatr`, …), turning on the way | pivot first (`combat` + waitpivot), step after |
| CombatFace off | faces the move direction (`combatf`, turn rate 8) | faces the target anyway |
| Already stepping | changes the step in place | new transition block at once |
| Root heading | set to the held angle | never touched |
| Dead mover | refuses | walks |
| Probe ahead | FindClearPath 4 units | none |
| Health, radii | read (Distance, IsValidTarget) | not read |

## 3. ResolveCombat / ResolveCombatMove — every tick

ResolveCombat (`0x4c7980`, thiscall (ab, bits), ret 8):

- Clears the goto target only when `+0x288` (the item to pick up) is 0.
- **Retargets on a cadence**: the player when `(GameFrame ^ id) & 7 == 0`,
  monsters when `& 0x1f == 0` and the AI isn't off (`0x668110`). Off
  cadence it keeps the target it has.
- A target is kept while IsValidTarget (`0x4cd990`) holds: not
  `charflags & 0x8000`, alive, `+0x1a4` clear, not object flag `0x80`,
  within `chardata+0x15c` (Distance, §5), and — for the player as a target —
  the player has control (PlayScreen `+0x5e0`, §6) or `+0x5d8`.
- Switching to a closer target needs it within 45° (`< 0x21`) of the move
  direction.
- Facing: the target's bearing when it's visible (CanSeeCharacter
  `0x4cd540`) and §1 allows, else the block's angle.
- Pivot blocks (`flags & 0x200`) turn in place; a moving player whose
  facing changes gets a new step block with turn rate
  `(max(0, |Δ| − 32) / 32) · 8 + 16`; standing or monsters get
  `· 4 + 8` in place.

ResolveCombatMove (`0x4c7f80`, ret 8): `bits & 2` (blocked) or the stop
flag → back to the root; if desired is the root, re-desire the step; the
goto/pick-up arrival test (`max + min/2 < 8`, then SetPos, pick up
`0x4cfef0`); then ResolveCombat.

A/B (654 cases, 0 matching): turn rates on a new strafe step (retail 24,
port 12), retargeting on cadence vs every tick, no sight check in the
port, and the port keeping a target retail drops.

## 4. The action-state core

- SetDesired (`0x4db3a0`): with a pending desired block that isn't doing,
  retail refuses while **doing** has priority; the port tests the
  **desired** block's priority. An interrupting block goes to
  ForceCommand (slot `0x218`) in both.
- Action block flags (§6.2 of the dojo): retail `firsttime 0x1`,
  `transition 0x2`, `priority 0x10`, `interrupt 0x20`, `nowaitdone 0x40`,
  `dontforce 0x80`, `stop 0x100`, `waitpivot 0x200`, `noroot 0x400`,
  `goto 0x1000`; `0x4`, `0x8`, `0x800` unidentified. The port's 1998
  bitfield lacks one bit below `priority`.
- SetState (`0x46f250`): retail always restarts the state; the port
  (object.cpp) returns early when the same looping state is set again (a
  fix for a visual pop). Movement steps are looping states, so the step
  cycle's timing differs. Kata of its own.

## 5. Distance is edge to edge

Retail TCharacter::Distance (`0x4d61b0`, slot 4): the centre distance
minus the mover's Radius (`0x4d6e40`, class.def STATS `Radius`) and, for a
character or player target, the target's; never below 0. The port's
characters use TObjectInstance::Distance, centre to centre. Combat range,
attack min/max distance and retargeting all read Distance, so port
characters engage, orbit and swing at the wrong separation (by
`r1 + r2`: 32 for Locke and an Araknid at radius 16 each, 36 against an
Arakna at 20).

## 6. The player-control flag

PlayScreen `+0x5e0` (`0x65d0d0`) is "the player has control": the setter
`0x47c580` writes it and its inverse to the control-off flag `0x666924`.
While a script holds control, IsValidTarget refuses the player as a
target, so monsters drop him. Fixtures default it on.

## 7. Recon labels corrected

- `0x4db450` is TryCommand and `0x4db4d0` ForceCommand (slots `0x214` /
  `0x218`); the `recon/discovered/` file names have them swapped.
- `0x4ce350` (`..._HasActionAni_4ce350.cpp`) is Go(angle); HasActionAni is
  `0x4d6c20` (slot `0x1f0`).
- Slot `0xd8` (`0x4d74d0`) is GetStat (type stats, Radius), slot `0xdc`
  (`0x4d7520`; TPlayer `0x51ae30`) GetObjStat (Health, Fatigue, Mana).

## 8. Verification rig

Katas M3 (`combat-go`) and M5 (`combat-resolve`):
`python3 tools/retail_ab/retail_ab.py combat-go` (combat worktree), cases
in `tools/retail_ab/combat_targets.py`, retail fixture
`tools/retail_runtime/slots/combat/combat_call.py` (main checkout), port
`src/retailab_combat.cpp`. ~0.7 ms per retail case.

## 9. Open

- What moves the character between ticks: Move / MoveStep (`0x4c46d0` /
  `0x4c3bc0`) and the imagery's motion data — kata M7.
- The sequence: the same N ticks of input on both sides, positions and
  facings per tick — kata M8.
- The CombatFace option in the port (Options screen + INI) and the order
  of the port fixes.
