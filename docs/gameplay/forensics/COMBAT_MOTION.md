# Forensics: combat motion (what moves a character each tick)

**Topic:** the displacement and animation-frame side of movement in the
shipped game: the per-tick order, UpdateAction / ResolveAction and the move
bits, ResolveMove, Move / MoveStep and the clear-path probe, AdvanceAngles,
the frame advance, and how the imagery's motion data becomes displacement.
The decision side (Go, ResolveCombat, ResolveCombatMove, SetFighting,
ForceCommand) is in [COMBAT_MOVEMENT.md](COMBAT_MOVEMENT.md) and is not
repeated. Feeds katas M7 (Move / MoveStep) and M8 (multi-tick sequences).
**Status:** 2026-10-08. Read from the retail disassembly only; nothing here
has been run in the emulator yet. Every address below was read in the asm
(`rdis.py`), not taken from Ghidra labels.
**Evidence:** unchanged retail image (`recon/retail_asm/baseline`), Ghidra
decompiles `recon/classes/cls_0x5a7b98.cpp` (MoveStep, Pulse; used only as
a map, the decompile of MoveStep confuses the return-bits local with ESI).
Port citations are the combat worktree **working tree** of 2026-10-08
(`character.cpp` has uncommitted edits by another session; line numbers
there will drift).

Units: positions are integer world units; `vel` / `accum` / `movedist` are
16.16 fixed point (one unit = `0x10000`, the port's `ROLLOVER`); motion
data distances are 8.8 and are shifted left 8 on read. Angles are 0..255.

## 1. Function table

| Retail | Identity | Convention | Evidence |
|---|---|---|---|
| `0x491100`..`0x491858` | main loop: one `CurrentScreen->TimerTick(draw)` (screen slot `0x44`) per iteration, then `CurrentScreen+0x48`++ (`0x491454`, `0x4917f7`) | — | asm; GetTickCount frame-skip logic around `0x491390` (draw skipped when the measured rate is under 23.5 fps; the throttle itself unverified) |
| `0x490bd0` | TScreen::TimerTick(draw) | thiscall(draw) ret 4 | asm; calls the screen slots in the order of §2 |
| `0x47bd20` | TPlayScreen::Pulse (vtable `0x5a5320` slot `0x10`) | thiscall() | asm: calls UpdateMove `0x47de30` at `0x47c155`, TScreen::Pulse `0x48fda0` at `0x47c21f` |
| `0x47b4d0` | TPlayScreen slot `0x0c`: side-pane open/close state machine, **not** the game pulse | thiscall() | asm (pane Show calls only) |
| `0x47c2c0` | TPlayScreen::Animate(draw) (slot `0x18`): TScreen::Animate `0x490030`, then GameFrame++ (`+0x680`, `0x47c37d`) and game time | thiscall(draw) ret 4 | asm |
| `0x4902c0` | TScreen slot `0x24`: each pane's slot `0x60` | thiscall() | asm |
| `0x48fda0` | TScreen::Pulse: player idle bit, then each pane's Pulse (slot `0x4c`) | thiscall() | asm, port REVSYNC agrees |
| `0x454390` | TMapPane::Pulse (vtable `0x5a5370` slot `0x4c`) | thiscall() | asm; static init stores vtable `0x5a5370` into the MapPane global `0x6668d8` |
| `0x4580f0` | TMapPane::PulseObjects | thiscall() | asm: iterator objset 4, slot `0x110` |
| `0x458390` | TMapPane::MoveObjects | thiscall() | asm: objset 1, slot `0x114` → `+0xbc`, then slot `0x118` |
| `0x457ef0` (via thunk `0x4545f0`, MapPane slot `0x60`) | TMapPane::NextFrameObjects | thiscall() | asm: objset 5, slot `0x120` |
| `0x4c1bb0` | TCharacter::Pulse (slot `0x110`) | thiscall() | vtable `0x5a7848+0x110`; asm |
| `0x518aa0` | TPlayer::Pulse (slot `0x110`; calls `0x4c1bb0` first) | thiscall() | vtable `0x5b4f30+0x110`; asm. The port calls it "TPlayer::Animate" (player.cpp:656) — wrong, slot `0x110` is Pulse |
| `0x4db190` | TComplexObject::Pulse | thiscall() | asm: `UpdateAction(+0xbc)` via slot `0x210` |
| `0x4c3260` | TCharacter::UpdateAction (slot `0x210`) | thiscall(bits) ret 4 | vtable; asm |
| `0x4db1d0` | TComplexObject::UpdateAction | thiscall(bits) ret 4 | asm |
| `0x4c3490` | TCharacter::ResolveAction (slot `0x21c`) | thiscall(bits) ret 4 | vtable; asm |
| `0x4db2b0` | TComplexObject::ResolveAction: `doing ? 0 : 3` | thiscall(bits) ret 4 | asm |
| `0x4c5e90` | ResolveMove (slot `0x31c`) | thiscall(ab, bits) ret 8 | vtable; asm |
| `0x4c83d0` | ResolveLeap (slot `0x348`) = `ResolveCombat(ab, bits)` (slot `0x328`) | thiscall(ab, bits) ret 8 | asm |
| `0x4c83f0` | ResolvePull (slot `0x34c`) = `return 0` | ret 8 | asm |
| `0x4c8400` | ResolveSay (slot `0x340`) | thiscall(ab, bits) ret 8 | asm |
| `0x4c8470` | ResolvePivot (slot `0x344`) | thiscall(ab, bits) ret 8 | asm |
| `0x519210` | TPlayer ResolveCombat override (slot `0x328`) = tail call `0x4c7980` | ret 8 | asm |
| `0x4c5ad0` | AdvanceAngles(faceang, moveang, maxturn) | thiscall ret 0xc | asm; callers ResolveMove / ResolvePivot / ResolveCombat |
| `0x4c46d0` | TCharacter::Move (slot `0x114`) | thiscall() ret 0 | vtable; asm |
| `0x518df0` | TPlayer::Move (slot `0x114`) | thiscall() | vtable `0x5b4f30+0x114`; asm |
| `0x4c3bc0` | TCharacter::MoveStep | thiscall() ret 0 | only callee of Move's loop; asm |
| `0x4c39d0` | TCharacter::FindClearPath / Blocked(pos*, newpos*, bits, height*, bychar**) | thiscall ret 0x14 | asm; returns 1 = blocked |
| `0x4530a0` | TMapPane::GetWalkHeightRadius(pos*, level, radius, maxdelta*, height*, hole*) | thiscall ret 0x18 | asm |
| `0x452e10` | TMapPane::GetWalkHeight(pos*, level, 0) | thiscall ret 0xc | callers; not disassembled |
| `0x4d4db0` | TCharacter::CharBlocking(inst, pos*, level, radius) | cdecl, 4 args | asm |
| `0x46de60` | Distance(p1*, p2*) 2D table distance | cdecl | asm (table `0x5e9200`) |
| `0x470920` | TObjectInstance::Move (base, slot `0x114`) | thiscall() | asm |
| `0x470bb0` | TObjectInstance::SetObjectMotion (slot `0x118`) = `imagery->slot 0x4c(this)` | thiscall() | asm |
| `0x40cd20` | T3DImagery::SetObjectMotion (imagery vtable `0x5a35ac` slot `0x4c`) | thiscall(inst) ret 4 | asm |
| `0x40cc40` | T3DImagery::GetMotion(state, frame, &dist, &vert, &ang, &rotx, &roty, &rotz) | thiscall ret 0x20 | asm |
| `0x470c30` | TObjectInstance::GetNextMove(S3DPoint*) | thiscall ret 4 | asm |
| `0x470bc0` | TObjectInstance::SetNextMove(S3DPoint*) | thiscall ret 4 | asm |
| `0x46db20` | ConvertToVector(angle, dist, out*, zangle) | cdecl | asm; tables `0x634d44` / `0x634f44` |
| `0x470cc0` | TObjectInstance::NextFrame (slot `0x120`) | thiscall() | asm |
| `0x46f250` | TObjectInstance::SetState (slot `0x18`) | thiscall(state) ret 4 | asm |
| `0x46f1e0` | TObjectInstance::ResetState (slot `0x140`) | thiscall() | asm |
| `0x4d6e00` | TCharacter::MoveTo (slot `0x0c`): `movetopos(+0xec)=1`, `movepos(+0xf0..f8)=p` | thiscall(p*) ret 4 | asm |
| `0x4cedb0` | Goto(x, y, item) | thiscall ret 0xc | asm |
| `0x4cee70` | Stop(name) | thiscall ret 4 | asm |
| `0x4d2be0` | Leap(angle) | thiscall ret 4 | asm (strings "leapf".."leapfl") |
| `0x4d6220` | SideStep(dir) | thiscall ret 4 | asm (string "sidestep") |
| `0x4d73f0` / `0x51b3f0` | walk root name (slot `0x30c`): "walk"; TPlayer: "torch" when the in-hand item (`+0x2b8`, or the arg) is objclass 6, else "walk" | thiscall(obj) ret 4 | asm |
| `0x4d6f00` / `0x4d6e40` | Health() (slot `0x1c0`, GetObjStat) / Radius() (slot `0x258`, GetStat) | thiscall() | asm |

Object fields used here (retail): flags `+0x08`, state `+0x0c` (u16),
level `+0x0e` (u16), pos `+0x10/14/18`, vel `+0x1c/20/24`, accum
`+0x28/2c/30`, rotx `+0x34`, roty `+0x35`, facing `+0x36`, imagery `+0x54`,
animator `+0x58`, frame `+0x5c` (s16), framerate `+0x5e` (s16), prevstate
`+0x60` (u16), prevframe `+0x62`, inventnum `+0x7c` (s16), commanddone
`+0x80`, moveangle `+0xb0`, movedist `+0xb4`, movevert `+0xb8`, movebits
`+0xbc`. TCharacter: movetopos `+0xec`, movepos `+0xf0`, chardata `+0xfc`,
forcecommanddone `+0x108`, forcenomove `+0x10c` (never set anywhere in
retail: only the ctor `0x4c18f6` and MoveStep `0x4c3c73` write it),
charflags `+0x110`, exit timestamp `+0x114`, shovedir `+0x11c`,
combatflashticks `+0x224`, pick-up item `+0x288`. SCharData: walkspeed
`+0x1ec`, runspeed `+0x1f0`, sneakspeed `+0x1f4`, combatwalkspeed `+0x1f8`.

## 2. The per-tick order

### Retail (one TimerTick, `0x490bd0`)

1. Screen slot `0x0c` (`0x47b4d0`): side panes. No gameplay.
2. Pane resize pass (pane slot `0x48`).
3. Screen slot `0x10` = **TPlayScreen::Pulse** `0x47bd20` (skipped while a
   modal has bit `0x10`), in order:
   - save/load requests, area reset;
   - **if PlayScreen `+0x5e0` (player has control): UpdateMove `0x47de30`**
     (input → Go / Stop / Leap / attacks), then `0x47e0f0` (camera keys:
     reads camera mode `0x6671f0`, key flags `0x1000/0x2000/0x4000`;
     camera only, identification from the asm, low confidence on exact role);
   - TScreen::Pulse `0x48fda0`: sets player state bit 2 after 0x78 frames
     without input (the bit Go / Leap clear), then each pane's Pulse:
     **TMapPane::Pulse `0x454390`** → (`0x45b080` level, `0x45f1e0`,
     `0x4537b0`) → **PulseObjects `0x4580f0`** → **MoveObjects
     `0x458390`** → `0x4539d0` (map position) → camera → TAreaMgr pulse.
4. Draw only: screen slot `0x14` (background).
5. Screen slot `0x18` = **TPlayScreen::Animate(draw)** `0x47c2c0` — always,
   drawing or not: TScreen::Animate → MapPane::Animate `0x454450` (object
   Animate, slot `0x11c`), then **GameFrame++** (`+0x680`, `0x47c37d`;
   skipped while paused `+0x5d4`, `0x668154`, `0x666920`, a modal with bit 8,
   or the map pane closed) and game time `+0x68c = frames*100/24`.
6. Draw only: screen slots `0x1c`, `0x20`.
7. Screen slot `0x24` (`0x4902c0`) → MapPane slot `0x60` →
   **NextFrameObjects `0x457ef0`** — always (modal bit `0x10` skips it).
8. Main loop: `CurrentScreen+0x48`++ (the "screen frame" counter that the
   exit timestamp `+0x114` and the idle bit read).

So per object, the cycle is:
`UpdateMove → Pulse (UpdateAction → Resolve* → AdvanceAngles; AI) → Move
(MoveStep) → SetObjectMotion → [draw] → GameFrame++ → NextFrame → (next tick)`.

Inside PulseObjects (`0x4580f0`, iterator flags `0xa0`, objset 4): an object
with OF_KILL (`flags & 0x1000`) whose slot `0x3c` is true and that is not the
player is deleted (`0x451840`); every other object gets Pulse (slot `0x110`).
Inside MoveObjects (`0x458390`): pass 1, every objset-1 object:
`movebits(+0xbc) = Move()`; pass 2, every objset-1 object:
`SetObjectMotion()` (slot `0x118`). Inside NextFrameObjects (`0x457ef0`,
objset 5, gated by `0x6680d8`): an object for which slot `0xb4` (in
inventory) is true is skipped unless its owner `+0x64` is the Player;
otherwise NextFrame (slot `0x120`). All three loops wrap each call in SEH
that deletes a faulting object.

### Port

`TScreen::Tick` (24 Hz legacy frames) → `TPlayScreen::Pulse` → `Update()`
→ `CurrentMode()->Tick()` (playscreen.cpp:791) → `UpdateMove()`
(runtimemode.cpp:113) → `MapPane.Pulse()` (runtimemode.cpp:119 →
mappane.cpp:2584): **NextFrameObjects (unless the screen's first frame)**,
PulseObjects, MoveObjects (Move then SetObjectMotion, same two passes);
then `++gameframes` (playscreen.cpp:812); drawing (`DrawFrame` →
`AnimateObjects` → `inst->Animate`) runs separately at render rate.

Differences in order:

- **NextFrame moved from the end of the tick to after UpdateMove.** Retail:
  `NextFrame(t−1) → UpdateMove(t) → Pulse(t)`; port: `UpdateMove(t) →
  NextFrame → Pulse(t)`. The Pulse→Move→SetObjectMotion→NextFrame cycle is
  the same, but **UpdateMove's Go / Stop / Leap / attack calls see the frame,
  `commanddone` and loop wraps one NextFrame earlier in the port than in
  retail** (retail input sees the already-advanced frame). An M8 sequence
  that injects input must reproduce retail's placement.
- The port skips NextFrame on the screen's first frame; retail runs it at the
  end of the first tick. Cyclically identical (both: P M S N P M S N …).
- Object Animate (animator Animate, slot `0x11c` → `0x470ca0`) runs once per
  tick in retail, draw or not; the port runs it per rendered frame and only
  for drawn objects. Nothing in Animate moves an object (retail
  TCharacter::Animate `0x4c36b0` = animator->Animate only).
- The port's iterator flags: retail passes `0xa0` (`0x20` no-inventory plus
  `0x80`) in all three loops; the port passes CHECK_NOINVENT only. Whether
  retail `0x80` means what the port's CHECK_LOADED means (whole loaded map
  vs the pane window) is unverified (§7).
- Game time: retail `frames*100/24` (`0x47c38d..0x47c39e`, same formula in
  TPlayer::Pulse `0x518ac5`); the port uses `kGameFrameRate = 30`
  (playscreen.cpp:243, :813), a clock 0.8× retail's. Not motion, but every
  regen / poison / recovery timestamp in Pulse reads it (kata C6).

## 3. Behaviour per function

### 3.1 TCharacter::Pulse `0x4c1bb0` — the movement-relevant spine

```
if (0x65d0c4 /*PlayScreen+0x5d4 pause*/) return;
SpellManager(+0x170).Pulse();                       // 0x540750
if (animator) animator->Pulse();                    // slot 0x2c (tags, sounds)
if (forcecommanddone) { if (doing) doing->wait = 0; commanddone = 1; forcecommanddone = 0; }
if (combatflashticks > 0) combatflashticks--;
TComplexObject::Pulse();                            // 0x4db190 -> UpdateAction(movebits)
if (root && root->action in {3 COMBAT, 0x19 BOW} && root->obj) root->obj->slot0x23c(this, obj);   // SignalHostility
if (screen+0x48 - exitstamp(+0x114) > 5)  SetFlags(flags & ~0x100000);   // OF_ONEXIT
if (screen+0x48 - exitstamp > 0x18) +0xe4 = 0;
... blood / decap / impale effects, UpdateFade (0x4d57a0), dead handling ...
[player only] auto-begin-combat (0x4c25d5..0x4c2867), below
if (charflags & 0x100000) { slot 0x234(); slot 0x230 /*AI*/(); }
else if (flags & 0x20 /*OF_AI*/) slot 0x230 /*AI*/();
... recovery, poison (slot 0x314 game time) ... +0x26c = GameFrame;
```

TComplexObject::Pulse `0x4db190`:

```
if (0x6682c4 /*UpdatingBoundingRect*/) return;
if ((doing->flags & 0x4) && (short)frame >= 5) doing->flags &= ~0x6;   // clears bit 0x4 and transition 0x2
UpdateAction(movebits /*+0xbc*/);                                      // slot 0x210
```

Bit `0x4` is set by ForceCommand (`0x4db621..0x4db62f`) as a copy of the
transition bit when the new state was reached through a transition-state
search; it and the transition bit clear themselves at frame 5. The
transition bit matters below (UpdateAction's root rule).

So **all resolving happens before AI in the same Pulse**: the AI's
SetDesired / Go of tick t is acted on by UpdateAction of tick t+1.

Player auto-begin-combat (TCharacter::Pulse `0x4c25d5`, PLAYER only):
skip when root is "combatrun" / "handrun" / "bowrun" / "run" / "sneak";
cadence `((GameFrame ^ id) & 0x1f) == 0`; `FindCharacters(&t, 1, −1,
facing, 0x20, 7)`; IsValidTarget(t); root not COMBAT / BOW / "sneak";
option table `0x65a77c[0x65a784]` bit 8 or `t` not a player;
`AutoBeginCombat` (`0x5d7a68`) and `+0xe8`; t not flagged in the has-seen
list (`+0x1c0`, 8 × 12 bytes, flag at `+0x1c8`); `Distance(t)` (slot 4,
edge to edge) `< chardata+0x158`; `|AngleDiff(facing, AngleTo(t))| < 0x30`;
PlayScreen `+0x5e0` or `+0x5d8` (`0x65d0d0` / `0x65d0c8`) → BeginFighting(t,
3) `0x4d3b90`. Nothing found and `+0xe8 == 0` → `+0xe8 = 1`.

### 3.2 UpdateAction `0x4c3260` (thiscall(bits) ret 4)

`bits` is the previous tick's Move return, stored at `+0xbc` by MoveObjects
and passed by TComplexObject::Pulse: `1` MOVED, `2` BLOCKED, `4` FALLING,
`8` NOTMOVING (§3.6).

```
f = GameFrame();                                         // 0x47e920
if (f % 24 != 0 || (commanddone && root != doing) || glimpse(+0x130) < 0 || noise(+0x134) < 0)
    ResetStealthValues();                                // 0x4cdbb0
if (root->action != 0x17 /*SLEEP*/ && Sleeping() /*slot 0x1b8*/) SetSleeping(0);   // slot 0x1bc
c = ResolveAction(bits);                                 // slot 0x21c
if (c == 0) {
    c = (commanddone || !animator || (flags & 0x80 /*INVISIBLE*/)) ? 0 : 2;
    if (doing == root && !(doing->flags & 0x2 /*transition*/)) c = 0;
    if (c == 0) doing->flags &= ~0x10;                   // priority
}
doing->flags &= ~0x1;                                    // firsttime
if (script(+0x84)) script->Continue(c == 0);             // 0x4933d0
if (bits & 4 /*FALLING*/) {
    if (HasActionAni("fall", 0) && !(doing->flags & 0x10)) {
        ab = new TActionBlock("fall", 2 /*ACTION_MOVE*/);
        if (ForceCommand(ab, 0, 0) != 2 && ab not doing/root/desired) delete ab;
    }
} else
    TryCommand(desired, bits, (charflags & 2) ? 1 : 0);  // slot 0x214
if (doing && doing->wait > 0) doing->wait--;
```

Return codes as retail uses them: **0 = done / no opinion, 2 = executing
(TryCommand waiting, ForceCommand accepted), 3 = impossible**
(`0x4db1e5`: `commanddone ? 0 : 2`; TryCommand returns 2 at `0x4db4a2`;
ForceCommand returns 2 or 3, `0x4db744`, `0x4db8c7`). The port's
COM_PENDING 0 / EXECUTING 1 / COMPLETED 2 / IMPOSSIBLE 3 (complexobj.h:19)
is a different encoding: compare by meaning, never raw.

### 3.3 ResolveAction `0x4c3490` (thiscall(bits) ret 4)

```
c = TComplexObject::ResolveAction(bits); if (c) return c;          // 3 when no doing
a = doing->action;
if (a == 2 /*MOVE*/)                                               return ResolveMove(doing, bits);
if (root && ((root->action == 3 && (root->Is("combatrun") || root->Is("handrun")))
          || (root->action == 0x19 && root->Is("bowrun"))
          ||  root->Is("run"))
    && a in {2, 4 COMBATMOVE, 0x1a BOWMOVE})                       return ResolveMove(doing, bits);
switch (a) {
 7 ATTACK: 0x320   0xc IMPACT, 0xe KNOCKDOWN, 0xd STUN: 0x338   8 BLOCK: 0x334
 4, 0x1a: 0x324 ResolveCombatMove   3, 0x19: 0x328 ResolveCombat   5 COMBATLEAP: 0x348
 0x1b BOWAIM: 0x32c   0x1c BOWSHOOT: 0x330   0x10 SAY: 0x340   0x11 PIVOT: 0x344   0x13 DEAD: 0x33c
}
return 0;                                                          // includes 0x12 PULL
```

ACTION numbering matches the port's enum (complexobj.h:26). **ACTION_PULL
(0x12) is never dispatched**; ResolvePull (`0x4c83f0`) is `return 0`.

### 3.4 ResolveMove `0x4c5e90` (thiscall(ab, bits) ret 8)

```
if (ab->flags & 0x200 /*waitpivot*/) {
    movedist = 0;                                            // Halt
    if (stricmp(ab->name, root->name) == 0) {
        if (facing != ab->angle) { AdvanceAngles(ab->angle, ab->angle, ab->turnrate); return 0; }
    } else if (!commanddone) return 0;
    // pivot done: first step, by suffix
    for sfx in "f", "l", "r":                                // "%s%s" root->name + sfx, HasActionAni(name, 0)
        if found: new = copy(doing) with that name;          // 0x4daae0(src, name, 0): 100-byte copy
    none found: SetDesired(root, 0); return 0;
    new->flags &= ~0x200; new->turnrate = 8; new->angle = new->moveangle = ab->angle;
    ForceCommand(new, 0, 0); return 0;
}
if (bits & 2 /*BLOCKED*/) {                                  // deflect off the obstacle
    a = ab->angle;
    a in [0x00,0x20) → +0x20;  [0x20,0x40] → a;  (0x40,0x60) → +0x20;  [0x60,0x7f) → −0x20;
    a in [0x7f,0xa0] → +0x20;  (0xa0,0xbf) → −0x20;  [0xbf,0xe0] → a;  (0xe0,0xff] → −0x20;
    a = (a + 0x10) & 0xff;
    ab->angle = a; facing = a; moveangle = a;
    ForceCommand(root, 0, 0);
    net 0x57d9d0(this, 0x1d, 1, 0, 0) [inert offline];  return 0;
}
if (ab->target != (0,0,0)) {                                 // goto
    dx = |pos.x − t.x|, dy = |pos.y − t.y|;
    if (dx + dy − min(dx,dy)/2 < 8) {                        // min via sar 1
        ab->target.z = pos.z; MoveTo(&ab->target);           // slot 0xc: movetopos=1, movepos=target
        ab->flags |= 0x40 /*nowaitdone*/;
        if (+0x288) { PickUp(+0x288, 0) /*0x4cfef0*/; +0x288 = 0; }
        return 0;
    }
    ab->angle = ab->moveangle = ConvertToFacing(&pos, &ab->target);   // 0x46dc60
}
moveangle = facing;                                          // byte
AdvanceAngles(ab->angle, ab->angle, ab->turnrate);
if (commanddone && !(ab->flags & 0x100 /*stop*/)) {
    new = copy(doing, name NULL, action 0);
    if      (stricmp-prefix doing->name == root->name + "l") strcpy(new->name, root + "r");   // 0x4dac80
    else if (doing->name == root->name + "r")                strcpy(new->name, root + "l");   // 0x4dac30
    ForceCommand(new, 0, 0);
}
return 2;
```

### 3.5 ResolvePivot `0x4c8470`, ResolveSay `0x4c8400`, ResolveLeap `0x4c83d0`

ResolvePivot: `movedist = 0`; same-named-as-root → `facing == ab->angle` ?
`SetDesired(0,0); return 0` : `AdvanceAngles(ab->angle, ab->angle,
ab->turnrate); return 2`; otherwise `commanddone` ? `SetDesired(0,0);
return 0` : `return 2`.
ResolveSay: `ab->wait > 0 && !(doing && doing->flags & 0x100)` → return 2;
else `ab->wait = 0; ForceCommand(root, 0, charflags & 2 ? 1 : 0); return 0`.
ResolveLeap: `return ResolveCombat(ab, bits)` through slot `0x328` (the
player's slot is `0x519210`, a tail call to `0x4c7980`).

### 3.6 AdvanceAngles `0x4c5ad0` (thiscall(faceang, moveang, maxturn) ret 0xc)

```
f  = facing (byte +0x36);  d  = (faceang & 0xff) − f;
if (d > 128) d −= 256;  if (d < −128) d += 256;          // d in [−128, 128]
d  = clamp(d, −maxturn, maxturn);                        // d<0: d <= −max → −max; d>=0: d >= max → max
nf = (f + d) & 0xff;
m  = moveangle (int +0xb0, not masked);  d2 = (moveang & 0xff) − m;
same wrap; d2 = clamp(d2, −maxturn, maxturn);
nm = (m + d2) & 0xff;
if (nf != f) facing = nf;      if (nm != m) moveangle = nm;
```

No other side effects. The port (character.cpp:1299) is the same
arithmetic; FaceOnly also rebuilds the transform (no simulation effect).

### 3.7 TCharacter::Move `0x4c46d0` and TPlayer::Move `0x518df0`

```
netremote = MP(0x66829c) && class == PLAYER && 0x67682c && this != Player && movetopos;
n = 10;
do { bits = MoveStep(); n--; }
while (movetopos && pos != movepos && (bits & 1) && n > 0);   // up to 10 steps to reach a MoveTo target
if (MP) { remote/blocked network bookkeeping; doing->flags |= 0x2000 once blocked }
movetopos = 0;
return bits;
```

TPlayer::Move: `bits = TCharacter::Move()`; if riding the hog (`+0x300`),
switch the "hog idle" / "hog drive" sounds on the edge of
`GetNextMove() != 0` (previous state in `0x66da50`); return bits. The port
(player.cpp:694) matches (its edge state is a function static).

### 3.8 MoveStep `0x4c3bc0` (thiscall() ret 0) — the displacement

```
if (flags & 1 /*IMMOBILE*/ || inventnum >= 0 || flags & 0x800000 /*PARALIZE*/) return 0;
r = 0;
h = GetWalkHeight(&pos, level, 0);  d = pos.z − h;
if (d < −16)      pos.z = h;                                         // direct write, no SetPos
else if (d > 16) {                                                   // fall: snap down to the ground
    vel.z = max(vel.z − 0x60000, −0x320000);
    SetPos(&(pos.x, pos.y, h), level, 0);  r = 4;                    // slot 8
}
if (d < 1) vel.z = 0;                                                // d of the start of the tick
if (forcenomove) { forcenomove = 0; return 8; }                      // dead: never set
if (movetopos) {
    D = (movepos − pos) << 16;  accum = 0;
    if (D == 0) return r | 8;
    shovedir = −1;  T = D;
} else {
    cd = chardata;
    if      (cd->combatwalkspeed > 0 && doing->action == 4 && !(doing->flags & 0x200))
             N = ConvertToVector(moveangle, cd->combatwalkspeed << 16, 0);
    else if (cd->walkspeed > 0 && doing->action == 2 && !waitpivot && root->Is(slot0x30c(0) /*"walk"|"torch"*/))
             N = ConvertToVector(moveangle, cd->walkspeed << 16, 0);
    else if (cd->walkspeed > 0 /*sic*/ && doing->action == 2 && !waitpivot && root->Is("sneak"))
             N = ConvertToVector(moveangle, cd->sneakspeed << 16, 0);
    else if (cd->runspeed > 0 && doing->action == 2 && !waitpivot && root->Is("run"))
             N = ConvertToVector(moveangle, cd->runspeed << 16, 0);
    else     N = GetNextMove();                                      // 0x470c30: from moveangle/movedist/movevert
    if (N == 0 && vel == 0) { accum = 0; shovedir = −1; return r | 8; }
    T = accum + N + vel;                                             // per component
}
target = pos + trunc(T >> 16);                                       // toward zero
if (|T.x| >= |T.y| && |T.x| >= 0x80000)      n = (|T.x| + 0x7ffff) >> 19;
else if (|T.y| > |T.x| && |T.y| >= 0x80000)  n = (|T.y| + 0x7ffff) >> 19;
else                                         n = 1;
S = T / n (each component, truncating);
// correction: make n steps land exactly on target
while (pos.x + trunc(n*S.x >> 16) < target.x) S.x += 100;   while (> target.x) S.x −= 100;
(same for y and z)
for (i = 0; i < n; i++) {
    np = pos;  m = S;  rollover(m → np) per component;               // |m| >= 0x10000: k = m/0x10000 (trunc)
    blocked = FindClearPath(&pos, &np, r, &h1, &by1);
    if (blocked) {
        r |= 2;
        FindClearPath(&pos, &pos, r, &h2, &by2);                     // return ignored; only by2 used
        if (by1 && by2) r &= ~2;                                     // already overlapping someone: let him move
    } else {
        shovedir = −1;  vel = 0;
        if (np.z != h1) np.z += (h1 − np.z) / 2;                     // trunc
        if (r & 8) { accum = m; return 0; }                          // unreachable: r never has 8 here
    }
    shoved = 0;
    if (r & 2) {
        if (!(root && root->action in {3, 0x19} && root->obj && by1)) {   // no shove only when a character blocks in combat
            offs = (shovedir == −1) ? {−0x20, +0x20, −0x40, +0x40} : {shovedir};
            best = 0;
            for off in offs:
                for (dd = 2; dd < 8; dd += 2) {
                    c = pos + ConvertToVector((off + moveangle) & 0xff, dd, 0);
                    if (FindClearPath(&pos, &c, 0, 0, 0)) break;
                    if (dd > best) { best = dd; shovedir = off; bestpos = c; }   // shovedir written while probing
                }
            if (best > 0) {
                np = bestpos;
                if (!FindClearPath(&pos, &np, 0, 0, 0)) {
                    r &= ~2; shoved = 1; m = 0; accum = 0; i = n;    // last substep
                } else np = pos;
            } else r |= 2;
        }
        if (r & 2) { np = pos; if (+0x254) { +0x25c = 0; +0x254 = 0; +0x258 = 0; } }
    }
    accum = m;                                                       // also when blocked
    if (np.z < GetWalkHeight(&pos, level, 0)) np.z = GetWalkHeight(&pos, level, 0);   // height at the OLD pos
    if (np != pos) SetPos(&np, level, 0);
    r |= 1;                                                          // MOVED, always, blocked or not
    if (shoved && movetopos) return r;
}
return r;
```

Consequences worth a kata case each: a blocked step still reports
`MOVED` (`r == 3`); a blocked substep does not end the loop (the next
substep probes again, now with a committed shovedir); the fractional
residual of the last substep is kept in `accum` even when blocked; the
whole move is skipped (`r | 8`, accum zeroed) when the animation gives no
motion and there is no velocity.

### 3.9 FindClearPath `0x4c39d0` (thiscall(pos*, newpos*, bits, height*, bychar**) ret 0x14)

```
*bychar = 0;  *height = pos->z;                                    // null height/bychar → scratch
if (bits & 8) { *height = GetWalkHeight(newpos, level, 0); maxdelta = 0; }
else { GetWalkHeightRadius(newpos, level, Radius(), &maxdelta, height, &hole);  if (hole) return 1; }
if (|pos->z − *height| > 0x20) return 1;
if (maxdelta > 0x20) return 1;                                     // signed
if (*height == 0) return 1;
if (Health() <= 0) return 0;
if (doing->attack && attack->flags(+0x24) & 0x2000000) return 0;
if (GetAniFlags(state) & 0x800 /*AF_FLY*/) return 0;
if (flags & 0x80 /*INVISIBLE*/) return 0;
if (movetopos) return 0;
if (MP && class == PLAYER && this != Player) return 0;
b = CharBlocking(this, newpos, level, Radius());  *bychar = b;
if (!b) return 0;
return Distance(pos, &b->pos) >= Radius(b) + Radius(this);       // not blocked while already overlapping
```

No exemption for bit 4 (falling). GetWalkHeightRadius `0x4530a0`: height at
`pos`; radius ≤ 0 or ≥ 0xf0 → height only; radius rounded up to 16 is
capped (≥ 0x80 → 0x6f); scans the 16-unit walk cells within the radius
(table distance test), `*maxdelta` = largest absolute height step between
neighbouring cells, `*hole = 1` if any cell in range has no walkmap
(`TSector::ReturnWalkmap` returns 0). Walkmap seam.

CharBlocking `0x4d4db0`: map iterator (pos, range 0x80, flags 0xe0, objset
2 characters, level); returns the **first** character `c != inst` with
`Health() > 0`, no attack flag `0x2000000`, not AF_FLY, not invisible,
`movetopos == 0`, `Distance(pos, c->pos) − radius − Radius(c) <= 0`, and
not a player whose state has bit 2 (idle; such a player is skipped).

### 3.10 Motion data → displacement (frame, SetObjectMotion, GetNextMove)

SetObjectMotion `0x470bb0` → T3DImagery::SetObjectMotion `0x40cd20`, run in
MoveObjects' second pass, after every object's Move:

```
s = state; f = (short)frame; ps = prevstate;
if (!meshinit(+0xc)) InitializeMesh (0x407510); failed → movedist = 0, return;
af = GetAniFlags(s);
if (af & 0x2000 /*NOMOTION*/ || CommandDone()) { movedist = 0; return; }
if (s != ps && (af & 0x400 /*ROOT*/)) accum = 0;
if (!GetMotion(s, f, &dist, &vert, &ang, &rx, &ry, &rz)) return;     // movedist left as it was
moveangle = (facing + ang) & 0xff;                                  // old facing
rotx = (rotx + rx) & 0xff;  roty = (roty + ry) & 0xff;  facing = (facing + rz) & 0xff;
movedist = dist;  movevert = vert;
```

GetMotion `0x40cc40`: bounds `state < header->numstates` (`[[img+4]+0x54]+4`)
and `frame < header state frames` (`(short)[hdr + state*0x4c + 0x32]`) and
`motion[state]` (`[img+0x98]`) → 8-byte SMotionData: dword0 low 16
(unsigned) `<< 8` = dist, high 16 (signed) `<< 8` = vert; dword1 bytes =
ang, rotx, roty, rotz. Out of range: all six zeroed, returns 0.

GetNextMove `0x470c30`: `x = (DistX[moveangle] * movedist) / 256`,
`y = (DistY'[|moveangle − 0x80|] * movedist) / 256` (`0x634f44`, index
`angle − 0x80` when `> 0x80`, else `0x80 − angle`), `z = movevert`;
divisions truncate toward zero; the angle is not masked. ConvertToVector
`0x46db20` is the same with `z = 0`, or, for `zangle != 0`, recursion on
`dist' = DistX[zangle]*dist/256` and `z = DistY'[zangle]*dist/256`.
SetNextMove `0x470bc0`: `moveangle = ConvertToFacing(p)`, `movedist =
table distance` (`0x5e9200`, halving until both ≤ 0xff, shifted back),
`movevert = p.z`.

Putting it together for a walking character, tick t:

1. ResolveMove (in Pulse): `moveangle = facing`, then AdvanceAngles turns
   both by at most `turnrate` toward `ab->angle` — **the motion data's
   `ang` is overwritten here**; for combat steps ResolveCombat writes
   `moveangle = ab->moveangle` (the held direction) before its AdvanceAngles
   (`0x4c7f40..0x4c7f54`).
2. Move: displacement = `ConvertToVector(moveangle, movedist)` where
   `movedist` came from SetObjectMotion of tick t−1, i.e. the motion of the
   frame that was current after tick t−1's Pulse (frame `f`, before
   NextFrame advanced it to `f+1`).
3. SetObjectMotion: reads the frame current now (`f+1`, or 0 if this
   tick's resolver changed state) → `movedist` for tick t+1; `facing +=
   rotz` (turning animations rotate here, after Move).
4. NextFrame: `frame += framerate` (±1).

**Walk speeds are dead in retail.** SCharData's ctor/clear writes −1 to
`+0x1ec..+0x204` (`0x489611` `or edx, −1` … `0x489789..0x4897ad`), and the
shipped `char.def` (in `imagery.rvi`) has no WALKSPEED / RUNSPEED /
SNEAKSPEED / COMBATWALKSPEED line (the parser knows the keys,
`0x5d9454..`). Every override in MoveStep is skipped; all displacement is
the imagery's per-frame motion data (plus `vel`).

### 3.11 NextFrame `0x470cc0`, SetState `0x46f250`, ResetState `0x46f1e0`

NextFrame: `if (flags & 0x800000) return; if (!imagery) return;` length /
flags from the inventory variants (slots `0x98` / `0x94`) when in inventory
else slots `0x90` / `0x8c`; `frame += framerate`; reverse play: `frame < 0`
→ `SetCommandDone(1)`, then LOOPING → `frame = len−1`, PINGPONG (`0x80`) →
`frame = framerate = 1`, else `animator->SetComplete(1)`, `frame = 0`;
`frame >= 0` → if `CommandDone()` then `SetCommandDone(0)`; forward: `frame >
len−1` → `SetCommandDone(1)`, LOOPING → 0, PINGPONG → `framerate = −1,
frame = len−1`, else SetComplete, `frame = len−1`; else clear CommandDone
if set; finally `animator->SetNewState(0)` (slot `0x28`). The port
(object.cpp:1762) is the same logic.

SetState: state out of range → `SetCommandDone(1)`, return 0; background
redraw; walkmap extract (`0x452750(this,3,0)`); `prevframe = frame;
prevstate = state; state = new;` `SetCommandDone(0)`; ResetState (frame 0 /
`len−1` with framerate ±1 by AF_REVERSE `0x100`, animator ResetState);
walkmap transfer; `NeedsAnimator` (imagery slot `0x38`) → `flags |= 0x4000`
else FreeAnimator (slot `0x2c`), `flags &= ~0x4000`; redraw; return 1.
**No same-state shortcut**: re-setting the current state restarts it at
frame 0 and sets `prevstate = state` (so SetObjectMotion's ROOT accum clear
does not fire for it).

### 3.12 Goto, Stop, Leap, SideStep

Goto `0x4cedb0` (x, y, item): `Go(ConvertToFacing(pos, (x,y,pos.z)))`;
fails → 0; `ab = (desired != root) ? desired : doing`; `ab->target =
(x, y, pos.z)`; `ab->flags |= 0x1000`; `item` → `+0x288`; net notify
(inert offline); return 1.

Stop `0x4cee70` (name): only when doing is MOVE / COMBATMOVE / BOWMOVE /
PIVOT or `doing->Is(name)`; MP gate; net notify; `root->flags |= 0x20`;
`root->angle = doing->angle`; `root->moveangle = doing->moveangle`;
`SetDesired(root, charflags & 2 ? 1 : 0)`; for the Player also
`0x65a9c8 |= 0x65a9c4; 0x65a9c4 = 0; MapPane slot (0x44f140)(5,0,0)`
(UI-side, unverified meaning); return 1.

Leap `0x4d2be0` (angle): root COMBAT or BOW; `Health() > 0`; unless
`charflags & 0x80000`: refuse when doing's attack has `0x2000000` or its
impact flags `0x80`; a player's state bit 2 is cleared; MP gate; `dir =
((angle − ((facing + 15) & 0xe0)) & 0xff) / 32` → suffix
`leapf, leapfr, leapr, leapbr, leapb, leapbl, leapl, leapfl`; name = root
name + suffix; `HasActionAni(name, 0)` else 0; `ab = new
TActionBlock(name, 5)`, `flags |= 0x20`, `ab->obj = doing->obj`;
`SetDesired(ab, 0)`; return 1. The block's angle / moveangle stay 0;
ResolveLeap → ResolveCombat.

SideStep `0x4d6220` (dir): refuse if `doing->name` starts with "sidestep"
(8 chars, case-blind); `dir` not `'l'`/`'r'` → `random(0,1) ? 'l' : 'r'`;
name "sidestep" + dir; `HasActionAni(name, 0)`; `ab = new
TActionBlock(name, 3 COMBAT)`; `ab->obj = doing->obj`; **`ab->angle =
doing->angle` and `facing = (byte)doing->angle` at once**; **`ab->moveangle
= (moveangle + ('l' ? 0x40 : 0xc0)) & 0xff`** (relative to the move angle,
not the facing); `flags = (flags & ~0x20) | 0x210` (clears interrupt, sets
priority 0x10 and waitpivot 0x200); `turnrate = 8`; `SetDesired(ab, 0)`
refused → free; accepted → net notify. Return value undefined (eax
leftover; treat as void).

## 4. RNG draws

Only these sites call `random` (`0x483300`) or `rand` (`0x58c582`) inside
the functions of §1 (xref of every call into both, filtered to the
function ranges):

| Site | Range | Decides | When |
|---|---|---|---|
| SideStep `0x4d628f` | (0, 1) | nonzero → `'l'`, 0 → `'r'` | only when the caller's dir is neither |
| TPlayer::Pulse `0x518c46` | (−5, 5) | torch light x jitter | player holds a torch (slot `0x250`: in-hand `+0x2b8` objclass 6) and has an animator, every tick |
| TPlayer::Pulse `0x518c60` | (−5, 5) | torch light y jitter | same, after the x draw |
| TPlayer::Pulse `0x518cd3` | (0, 2) | flicker dip on 0 | same, only when the light's intensity `(short)+0x8a` ≥ the torch's value (item slot `0x1f0`) |
| TPlayer::Pulse `0x518ce3` | (8, 0x1e) | dip size (negated) | same, only after a 0 from `0x518cd3` |

None in UpdateAction, ResolveAction, ResolveMove, ResolvePivot / Say /
Leap, AdvanceAngles, Move, MoveStep, FindClearPath, CharBlocking,
GetWalkHeightRadius, NextFrame, SetState, SetObjectMotion / GetMotion,
Goto, Stop, Leap, the MapPane loops. TCharacter::Pulse draws only inside
its callees (AI, attacks, effects): an M8 tape must cover AI's draws.
The port's TPlayer::Pulse (player.cpp:652) has no torch flicker draws, so
a torch-holding player's tape diverges every tick.

## 5. Fixture plan (seams)

Run as original (pure over guest memory):

- UpdateAction `0x4c3260`, ResolveAction `0x4c3490`, ResolveMove
  `0x4c5e90`, ResolvePivot `0x4c8470`, ResolveSay `0x4c8400`, ResolveLeap
  `0x4c83d0`, AdvanceAngles `0x4c5ad0`, TComplexObject Pulse / UpdateAction
  / ResolveAction (`0x4db190`, `0x4db1d0`, `0x4db2b0`), TryCommand /
  ForceCommand / SetDesired (green in M1-era katas), TActionBlock ctor
  `0x4da9f0`, copy ctor `0x4daae0`, name helpers `0x4dac30` / `0x4dac80` /
  `0x4dadd0` (formats into static `0x66caf4`).
- Move `0x4c46d0`, MoveStep `0x4c3bc0`, FindClearPath `0x4c39d0`,
  TPlayer::Move `0x518df0` (hog off: `+0x300 = 0`).
- Kernels: ConvertToVector `0x46db20`, ConvertToFacing `0x46dc60`, Distance
  `0x46de60`, GetNextMove `0x470c30`, SetNextMove `0x470bc0` — the tables
  must be built first (the slot already runs `0x41e2de..0x41e535`).
- NextFrame `0x470cc0`, ResetState `0x46f1e0`, T3DImagery::SetObjectMotion
  `0x40cd20` and GetMotion `0x40cc40`, over a forged imagery: `+0x0c`
  nonzero (mesh initialized, so `0x407510` is never reached), `[img+4]+0x54`
  → header with numstates at `+4` and per-state frame counts at `state*0x4c
  + 0x32`, `+0x98` → per-state SMotionData arrays from the case. The
  stand-in imagery vtable (guest.py) needs slot `0x4c` pointing at the
  original `0x40cd20`, and `0x8c` / `0x90` / `0x94` / `0x98` answered.
- Network: keep `0x676828 = 0`, `0x66829c = 0`, `0x67682c = 0`; every
  `0x676e08` call then returns at its first instruction (`0x57d9d0`,
  `0x586680` checked). They may run as original.

Seams (record the arguments, answer from the case):

| Seam | Retail | Record | Answer |
|---|---|---|---|
| walk height | GetWalkHeight `0x452e10` (pos*, level, 0) | pos, level | case height field (a function of x, y) |
| walk height in radius | `0x4530a0` (pos*, level, radius, &maxdelta, &height, &hole) | pos, level, radius | height, maxdelta, hole from the case |
| characters in the way | CharBlocking `0x4d4db0` (inst, pos*, level, radius) — or run it and seam the map iterator `0x44ceb0` / `0x44d080` | pos, radius | case characters (positions, radius, health, flags) |
| position write | SetPos slot 8 `0x46ed70` (sector move, walkmap, redraw) | new pos, level | store pos at `+0x10` |
| state change | SetState `0x46f250` (slot `0x18`) | state index | store `+0x0c`, prevstate/prevframe, ResetState as original |
| stats | Health slot `0x1c0` → GetObjStat; Radius slot `0x258` → GetStat | id | case stats |
| state lookup | HasActionAni slot `0x1f0` → FindState / FindTransitionState | name, pcnt | case state table |
| pick-up | `0x4cfef0` (item, 0) | item | — |
| stealth reset | ResetStealthValues `0x4cdbb0` | call | (or run: touches only `+0x130`/`+0x134`, unverified) |
| script | `+0x84` → keep null | — | — |
| sleeping | slots `0x1b8` / `0x1bc` | call | case |
| idle bit / TPlayer state | `0x51d680` SetPlayerState | new state | store `+0x36c` |

Globals read: GameFrame (PlayScreen `0x65caf0`: `+0x680 + +0x688 − +0x684`),
`CurrentScreen` `0x667fd0` (`+0x48` screen frames, read by Pulse only),
`0x6682c4` (UpdatingBoundingRect, 0), `0x65d0c4` (pause, 0), multiplayer
`0x66829c` (0), Player `0x667fcc`, AutoBeginCombat `0x5d7a68`, control
`0x65d0d0` / `0x65d0c8`, AI-off `0x668110`, chardata at `+0xfc` with speeds
−1 (as shipped) and, for override cases, positive speeds.

M8 order per tick, both sides: input (UpdateMove or its Go / Stop calls) →
Pulse (only the parts in §3.1 that touch motion; AI stubbed or seamed with
its draws on the tape) → Move → SetObjectMotion → GameFrame++ → NextFrame.
Compare per tick: pos, accum, vel, facing, moveangle, movedist, movebits,
state, frame, framerate, commanddone, the doing / desired / root blocks by
meaning, shovedir.

## 6. Port divergences

Port lines are the working tree of 2026-10-08.

| # | Where | Retail | Port |
|---|---|---|---|
| 1 | tick order (runtimemode.cpp:113-119, mappane.cpp:2591) | NextFrame at the end of the tick, before the next UpdateMove | NextFrame after UpdateMove: input sees frames / commanddone one advance earlier |
| 2 | UpdateAction (character.cpp:453) | `c==0` fallback adds "doing is root and not a transition → done"; priority cleared and script told "done" only on the fallback path; a resolver's nonzero return clears nothing | no root rule; a resolver returning COM_COMPLETED clears priority and continues the script |
| 3 | UpdateAction fall (character.cpp:500) | needs `HasActionAni("fall")` and no priority; block action MOVE (2) → ResolveMove; ForceCommand(ab, 0, 0); freed when refused | no gates; ACTION_ANIMATE default; passes `bits`; leaks when refused |
| 4 | UpdateAction stealth / sleep | reset also on frame%24==0 when `commanddone && root != doing` or glimpse / noise < 0; SetSleeping(0) only when Sleeping() | `% FRAMERATE` only; SetSleeping(0) unconditionally |
| 5 | TComplexObject::Pulse (complexobj.cpp:209) | clears flag bits 0x4 and 0x2 (transition) once frame ≥ 5 | missing; with #2, root-as-done timing differs |
| 6 | ResolveAction (character.cpp:512) | PULL never dispatched (ResolvePull returns 0) | dispatches PULL to a lever-pulling ResolvePull (retail does this elsewhere, unverified where) |
| 7 | ResolveMove blocked (character.cpp:1545) | deflects `ab->angle` (±0x20 by octant, then +0x10), sets facing = moveangle = it, ForceCommand(root, 0, 0), returns 0 | `Face(ab->angle)` unchanged angle, returns COM_COMPLETED |
| 8 | ResolveMove goto arrival | MoveTo, nowaitdone, pick up `+0x288`, return 0 | no pick-up (Goto drops the item arg), returns COM_COMPLETED |
| 9 | resolver return values | 0 done / 2 executing | COM_COMPLETED 2 / COM_EXECUTING 1 (ResolveMove, ResolvePivot, ResolveSay) |
| 10 | MoveStep fall (character.cpp:681) | `d > 16`: SetPos onto the ground at once, FALLING, vel.z −= 6<<16; `d < −16`: pos.z = h directly | no snap (gravity falls over ticks); ForcePos for `d < −16` |
| 11 | MoveStep no-motion | `N == 0 && vel == 0` → accum = 0, shovedir = −1, return `r | 8` before any probe | probes, returns 0 (MOVE_NOTHING) keeping the residual accum |
| 12 | MoveStep movetopos / Move loop | accum zeroed, `return r|8` if already there, shovedir reset; Move repeats MoveStep up to 10× until there; FindClearPath ignores characters while movetopos | one pass; characters still block (the port's own comment, character.cpp:618) |
| 13 | substep split (character.cpp:755) | `|x| >= |y|` picks x (ties → x); ±100 correction so n steps land on the target | `|x| > |y|` strict (equal diagonals of ≥ 8 units → one step); no correction (can end a unit short) |
| 14 | blocked rule | blocked; cleared only if both new and current position overlap a character (by1 && by2) | blocked only if the current position is clear (any reason) |
| 15 | after a blocked substep | `accum = m`, `r |= MOVED`, loop continues with the next substep | break; no MOVED (returns 2 where retail returns 3); accum untouched |
| 16 | shove gate (character.cpp:851) | skipped only when a combat/bow root with a target AND a character blocks | skipped in every combat/bow root with a target (terrain included) |
| 17 | shove commit (character.cpp:913) | zero m and accum, end the loop; vel kept; shovedir written while probing | zeros vel too; shovedir only on commit |
| 18 | still blocked | clears `+0x254/+0x258/+0x25c` | clears target_out_of_sight / sight_lost_ticks (mapping of these port fields to `+0x254..` unverified) |
| 19 | FindClearPath (character.cpp:625) | no FALLING exemption; `hole` flag; signed maxdelta; gates: dead mover, attack 0x2000000, AF_FLY, invisible, movetopos, remote MP player; overlap exemption `Distance(pos, by) >= r1 + r2` | exempts FALLING; abs(min/max delta); only attack-interactive and AF_FLY gates; no overlap exemption |
| 20 | CharBlocking (character.cpp:5057) | skips attack-0x2000000, AF_FLY, invisible, movetopos and idle (state bit 2) players | skips only self and dead (comment there calls the range 224; retail range is 0x80 = 128, flags 0xe0) |
| 21 | walk-root speed override | root "walk", player "torch" when holding a torch; sneak gated on walkspeed (retail bug); all speeds −1 in shipped data | root "walk" only; sneak on sneakspeed; port's fallback chardata (rules.cpp ~913) forces 6/12/3/4 for characters without a char.def entry, a displacement retail never has |
| 22 | SetState (object.cpp:1130) | always restarts, `prevstate = state` | looping same state: early return, frame keeps running, prevstate kept (after a re-set retail stops SetObjectMotion's ROOT accum clear, the port keeps clearing every tick) |
| 23 | SetState animator | `!NeedsAnimator` → FreeAnimator always | only when not permanent |
| 24 | SideStep (character.cpp:4602) | `ab->angle = doing->angle`, facing set to it at once, `ab->moveangle = moveangle ± 0x40`, flags priority + waitpivot, interrupt cleared | `ab->angle = face ± 0x40`, `moveangle = doing->moveangle`, sets interrupt + noroot (the port's own flag-bit comment is the pre-§5.4 mapping) |
| 25 | Leap (character.cpp:4476) | gates: root combat/bow, alive, attack/impact flags unless charflags 0x80000; clears player state bit 2 | gate is IsFighting() only |
| 26 | Stop (character.cpp:3363) | SetDesired(root, incidentals-off flag); player: map pane `0x44f140(5,0,0)` | one-arg SetDesired; no map-pane call |
| 27 | player auto-combat (Pulse) | cadence `(GameFrame ^ id) & 0x1f`, angle < 0x30, range chardata+0x158 (edge to edge), run/sneak roots excluded, control flags | `GameFrame % 24`, angle < 32, no root/control gates |
| 28 | TPlayer::Pulse (player.cpp:652) | torch flicker with 2–4 RNG draws per tick | none; and the port's comment names 0x518aa0 Animate |
| 29 | game time (playscreen.cpp:243) | frames × 100 / 24 | frames × 100 / 30 |
| 30 | ConvertToVector (object.cpp:174) | angle not masked | masks `& 255` (only matters for out-of-range callers) |

Equal on read: AdvanceAngles, NextFrame, ResetState, GetMotion /
SetObjectMotion arithmetic (3dimage.cpp:2764 / :2787; the port skips the
lazy mesh init and halts instead), base TObjectInstance::Move
(object.cpp:1669 vs `0x470920`), TPlayer::Move, ResolvePivot logic (return
codes aside), ResolveLeap, the not-blocked z easing, the shove probe
offsets and distances, MOVECHECKDIST (8 units = `0x80000`), rollover.

## 7. Open questions

- The 24 Hz throttle: where the main loop waits (`0x491100..`), and what
  the `[esp+0x10]` accumulator / 23.5 test exactly decides. Read, not
  settled.
- Retail iterator flag `0x80` (MapPane loops `0xa0`, CharBlocking `0xe0`):
  same meaning as the port's CHECK_LOADED? Decides which monsters pulse.
- `+0x254 / +0x258 / +0x25c` cleared by a blocked MoveStep: the dojo calls
  `+0x254` "combat-engage"; the port's equivalent field is unverified.
- Charflags `0x100000` (Pulse calls slot `0x234` before AI) and `0x80000`
  (interactive move, skips Leap / Go gates): names unverified.
- PlayScreen idle bit: TScreen::Pulse sets player state bit 2 after 0x78
  screen frames of no input (`0x48fdc9`, last-input frame `0x668504`); an
  idle player is passable for CharBlocking. Confirm in the lab.
- Where retail runs lever pull/push now that ResolvePull is empty.
- `0x47e0f0` after UpdateMove: camera keys, from a short read only.
- GetWalkHeight `0x452e10` and SetPos `0x46ed70` internals were not read;
  both are seams here.
