# TExit / Doors — Retail Walkthrough vs `src/exit.cpp`

Source of truth for retail: `data/Revenant.exe` via `data/RevenantDev`
Ghidra project. Source side: `src/exit.cpp`, `src/exit.h`,
`src/character.{h,cpp}`, `src/object.cpp:887` (`TObjectInstance::SetPos`),
`src/mappane.cpp:4204` (`TMapPane::CheckPos`).

## Class identification — answers to the quick questions first

| Question                                              | Answer                                                                                              |
| ----------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| TExit class registration site                         | `FUN_0050c660` (already in `agent_inventory.txt`)                                                   |
| `TObjectClass` global for the EXIT class              | `DAT_0066d1f0` (`ExitClass`, classid 10)                                                            |
| `TExit::TExit()` instance vtable                      | not yet pinpointed — placed by `REGISTER_BUILDER(TExit)` builder; ctor lives elsewhere in 50C-range |
| Does retail have a separate `TDoor` class?            | **No.** Doors are OBJCLASS_EXIT with objtype `Door`/`Door1`/`Door2`/`Halfdoor`/`CryptDoor`/etc.     |
| How does the player trigger an exit?                  | Two paths: (a) script "user" event from `Use()`, (b) auto-trigger by walking on strip during Pulse |
| Same-map sector load vs cross-level transition        | Both go through `Player->SetPos(target,level)` → `MapPane.CheckPos` (same code path)                |
| Fade-out / fade-in overlay during transitions?        | Not in TExit. TExit just sets the player position. Fade lives in `TPlayScreen` (out of scope).      |
| `exittimestamp` mechanism on TCharacter               | **Retail uses it identically** — `TCharacter::SetOnExit @ 0x004cdf30` writes `this[0x114] = CurrentScreen->FrameCount()` and OR's the OF_ONEXIT (0x100000) flag. |

The OBJCLASS_EXIT class object lives at `0x005e1634` (ExitClass static name
buffer) / `0x0066d1f0` (ExitClass `TObjectClass` instance). Static
initialiser `FUN_0050c660` calls `FUN_004742e0(&ExitClass,"EXIT",10,0,0)` and
ten `DEFSTAT(...)` registrations follow in `0x0050c6a0..0x0050c860`.

## (a) Door open / close mechanic

### State machine (retail)

Retail expanded the state enum from four values to **six**:

| Retail id | Source equiv     | Animation-state name (with fallback)        |
| --------- | ---------------- | ------------------------------------------- |
| 0         | `EXIT_OPENING`   | `"openingout"` ↦ `"closed to open"`         |
| 1         | `EXIT_OPEN`      | `"open"`                                    |
| 2         | `EXIT_CLOSING`   | `"closingout"` ↦ `"open to closed"`         |
| 3         | `EXIT_CLOSED`    | `"closed"`                                  |
| 4         | *(new)*          | `"openingin"` ↦ `"closed to open"`          |
| 5         | *(new)*          | `"closingin"` ↦ `"open to closed"`          |

Source's enum is `{EXIT_CLOSED=0, EXIT_OPEN=1, EXIT_CLOSING=2, EXIT_OPENING=3}`.
Retail re-orders **and** doubles-up: the "IN" variants (4, 5) animate the
door swinging *toward* the user, the "OUT" variants (0, 2) swing *away*.
Doors that don't have separate IN/OUT animation tracks fall back to the
shared `"closed to open"` / `"open to closed"` track names — every door
imagery has those, so the fallback is universal.

`TExit::SetExitState (FUN_0050d530)`:

```text
es=0 -> FindState("openingout")   ?? FindState("closed to open")
es=1 -> FindState("open")
es=2 -> FindState("closingout")   ?? FindState("open to closed")
es=3 -> FindState("closed")
es=4 -> FindState("openingin")    ?? FindState("closed to open")
es=5 -> FindState("closingin")    ?? FindState("open to closed")
       (otherwise pass-through)
```

### Toggle (Use → script "user" → OnUsed)

Source does the toggle inline in `TExit::Use`:

```cpp
if (state == EXIT_OPEN || state == EXIT_OPENING) SetExitState(EXIT_CLOSING);
else                                              SetExitState(EXIT_OPENING);
```

Retail splits this into two functions:

* `TExit::Use (FUN_0050d1a0)` — only does locked-check + key-check + invokes
  `TObjectInstance::Use(user, with)` (which routes to the script system).
* `TExit::OnUsed (FUN_0050d230)` — the actual toggle. Triggered when the
  script runs the `"user"` event (the same string that `TExit::Activate`
  uses). Picks IN vs OUT based on which side of the door the user is on,
  using `UserOnFrontSide (FUN_0050d2b0)` — a forward/backward facing-vector
  dot-product test.

This is a **retail-only refactor**. Source has no per-side awareness —
every Use just toggles the single 4-state machine. The retail system can
animate a door swinging away from the player (visually correct) instead
of always swinging the same direction.

### Auto-step "transition complete" (Pulse)

Source `TExit::Pulse`:

```cpp
if (CommandDone() && Openable()) {
    if (state == EXIT_CLOSING) SetExitState(EXIT_CLOSED);
    else if (state == EXIT_OPENING) SetExitState(EXIT_OPEN);
}
```

Retail `TExit::Pulse (FUN_0050d640)` reads the **animation track name**
via `imagery->GetStateName(state)` and string-compares against `"CLOSING"` /
`"OPENING"`. So the trigger edge is now data-driven — animation tracks
themselves declare their semantic role by their name. This is more
flexible (a door can have any number of intermediate animation states; only
the ones called CLOSING / OPENING auto-advance to CLOSED / OPEN), but it
costs two stricmps per pulse per door.

### Locked feedback

Source: `TextBar.Print("It seems to be locked.")` (literal English).

Retail: `TextBar.Print(LocalizeString("DOORLOCKED"))` — the message goes
through the resource-table lookup (`FUN_0049d800` with key `"DOORLOCKED"`
at `0x005e17cc`). This is the standard retail localisation pattern.

Retail also adds: only print to TextBar **if `user == Player`**. NPCs
trying to use locked doors silently fail. Source prints regardless.

## (b) Exit activation / level transition

### Walking-on-strip activation (Pulse)

Source `TExit::Pulse` checks **only the global `Player`** against the exit
strip. If the player is over the strip and `wait++ > Delay()`, fire
`Activate()`.

Retail `TExit::Pulse` iterates **all characters on the map** (via
`TMapPane::CharacterCount` / `GetCharacter` at `FUN_0051ee70` /
`FUN_0051eea0`). NPCs can trigger exits too. **But:** the auto-activation
is gated to objtypes named `"Door1"`, `"Door2"`, `"PortEW"`, or `"PortNS"`
only. Every other exit-typed object (Halfdoor, CryptDoor, Elevator, Lever,
DragonEnt, IrisDoor, …) only fires from explicit `Use()`/script
invocation, never from walk-over.

This means in retail Demo 1: a wooden Door1 you walk into will open
on its own; a CryptDoor / IrisDoor needs a Use action; an Elevator or
PressPlate is script-driven.

### TExit::Activate call chain (retail, FUN_0050d3a0)

```text
TExit::Activate(user, forced)
  ├─ if (multiplayer && !is_server) bail                      (retail-only)
  ├─ if (Locked()) bail                                       (retail-only)
  ├─ if (exitflags & EX_FROMEXIT && !forced):
  │     ScriptTrigger("user", user)  // FUN_00492640          (retail string-trigger)
  │     return
  ├─ search exitlist for entry with our objtype name
  ├─ FUN_004d4790(0)   // misc walkmap/state hook
  ├─ user->SetPos(ref->target, ref->level, override=0)
  │     │
  │     ├─ TObjectInstance::SetPos @ 0x0046ed70
  │     │     ├─ TMapPane::CheckPos @ 0x00459f50
  │     │     │     ├─ FindSector(level, sx, sy)              // 0x00499e10
  │     │     │     └─ if cross-sector: RemoveObject + AddObject   // 0x00499250 + 0x00498fb0
  │     │     └─ updates this->pos / this->level fields
  │     └─ ...
  └─ if (multiplayer && is_server) NetSendTeleport             // 0x00586bb0
```

Source's `TExit::Activate` is much simpler:

```cpp
if (GetScript()) GetScript()->Trigger(TRIGGER_ACTIVATE);   // enum trigger
if (exitflags & EX_FROMEXIT) return false;
PSExitRef ref = FindExit(name);
if (!ref) return false;
S3DPoint targ = ref->target;
if (stricmp(GetTypeName(),"Door")==0 && Player) {
    S3DPoint vect; ConvertToVector(Player->GetFace(), 24, vect);
    targ += vect;          // push player past the door
}
Player->SetPos(targ, ref->level);
return true;
```

### Source vs retail diff summary for Activate

| Aspect                        | Source                                               | Retail                                                                                |
| ----------------------------- | ---------------------------------------------------- | ------------------------------------------------------------------------------------- |
| Multiplayer guard             | none                                                 | early bail if mp-client; broadcast after if mp-server                                 |
| Locked check                  | absent (assumed never reached if locked)             | explicit `Locked()` returns 0                                                         |
| Script trigger                | enum `TRIGGER_ACTIVATE`                              | string event `"user"` (same event Use also fires)                                     |
| `EX_FROMEXIT` early-out       | direct flag check                                    | virtual call `vt[0xdc]` (likely encapsulates the flag check)                          |
| `"Door"` push hack            | `targ += ConvertToVector(Player->face, 24)`          | **gone** — relies on exit-strip placement instead                                     |
| Default user                  | always `Player`                                      | `user` arg, defaults to `Player` if null                                              |
| Multi-character support       | only the global `Player` can be teleported           | any character (so "captured villager follows you through doors" etc. could work)      |

### Cross-level vs cross-sector

Retail's `TMapPane::CheckPos (FUN_00459f50)` decides:

* If the new (level, sx, sy) maps to a different sector index than the
  current one — `RemoveObject` from current sector + `AddObject` to the
  new sector. Same code path whether the new sector is on the same level
  or a different one. The sector list itself is reloaded by the map-pane
  scroller when the player moves outside the loaded window.

* If the new position is OUT of the loaded window AND the object is
  `OF_NONMAP` (Player is) — character is removed from sectors entirely
  and re-loaded by the next `UpdateSectors()` call. This is exactly how
  source `TMapPane::CheckPos` works (matches `src/mappane.cpp:4239`).

* The `level` field on the object is **just an integer**; setting it does
  not by itself trigger anything. The redraw is driven by
  `TMapPane::SetMapLevel(newlev)` (source: `src/mappane.h:325` —
  `if (newlev != level) RedrawAll()`). Player::SetPos doesn't call
  SetMapLevel directly; that happens in `MapPane::UpdateMapPos`
  (`src/mappane.cpp:2585`) once the player is placed.

So the answer to "multi-level map transition vs same-map sector load":
**there is no special-case path in retail**. Both flow through the same
`SetPos → CheckPos → sector-transfer + map-pane scroll` machinery. The
"cinematic" multi-level transition (a corridor in level 0 leading into a
crypt on level 1) is achieved purely by the exit's `level` field in
`exit.def` differing from the source level — the engine does the rest.

### Fade overlay

`TExit` does not implement any fade. The retail behaviour I observed:

* `TExit::Activate` is synchronous — `SetPos` returns and the player
  is at the destination on the same frame.
* The map-pane scroll catches up on the next frame (lazy load of new
  sector imagery shows briefly).
* Any "fade" or "wipe" effect would be at the `TPlayScreen` /
  `TMapRenderer` level (script-controlled, not built into TExit).

Demo 1 may want to add a simple TPlayScreen-level fade-to-black around
the SetPos call — that's a port-side enhancement, not a retail feature
to recover.

## (c) `exittimestamp` anti-bounce

Source `src/character.h:526`:

```cpp
int32_t exittimestamp;  // When timestamp is +2 frames from current frame, OF_ONEXIT is cleared
```

`src/character.cpp:90, 2692` — set on construction and on `SetOnExit()`.
`src/character.cpp:227` — cleared if `exittimestamp < CurrentScreen->FrameCount() - 2`.

Retail `TCharacter::SetOnExit (FUN_004cdf30)`:

```c
void SetOnExit(TCharacter *this) {
    this->vt[0x40](this, this->flags | 0x100000);   // OR OF_ONEXIT
    this->mbr_0x114 = CurrentScreen->mbr_0x48;       // exittimestamp = framecount
}
```

Field offsets match: `flags` is the first member of `TObjectInstance`,
`exittimestamp` lives at offset 0x114 in TCharacter, framecount lives
at offset 0x48 in TScreen. **Same mechanism, identical to source**. No
porting work needed there — source-side `exittimestamp` and the
two-frame OF_ONEXIT clear are already wire-compatible with retail.

## SExitRef layout (retail-confirmed)

```
struct SExitRef {           // 36 bytes
    char *name;             // 0x00
    S3DPoint target;        // 0x04..0x0F
    int32_t level;          // 0x10
    int32_t mapindex;       // 0x14
    int32_t ambient;        // 0x18
    SColor  ambcolor;       // 0x1C..0x1F (rgb + 1 pad/alpha)
    SExitRef *next;         // 0x20
};                          // sizeof == 0x24
```

The yaml mapping `recon/mappings/SExitRef_cls_0x460620_map.yaml` is
**incorrect** — `cls_0x460620` is the Module-system class
(`module.def`/`area.def`/`exit.def` writer), not SExitRef. The real
SExitRef is anonymous in retail (only the head-pointer
`DAT_0066d1c4_TExit_exitlist` and the field-offsets above identify it).
Recommend deleting or correcting that yaml when next reorganising
mappings.

## What's missing / needs follow-up

* The TExit instance vtable address is not yet pinned — needed if we
  want to confirm vtable slots 0xdc / 0x208 / 0x248 / 0x27c by name. The
  TBuilder<TExit> ctor (which writes the vtable into a freshly-allocated
  TExit) is somewhere in the 0x50C..0x50CFB0 range; not decompiled yet.
* `FUN_0050c8f0` (TExit::ReadExitList) is identified by its caller but
  the body wasn't extracted — most likely a near-direct port of
  `src/exit.cpp:54`.
* `FUN_004d4790` (called once from Activate before `SetPos`) was
  decompiled but its identity is unclear — too generic for a confident
  rename. May be a TCharacter "stop combat" / "clear movement intent"
  helper. Left BARE for now.
* Demo 1 ports needed for the retail behaviours above:
  - 6-state SetExitState enum (currently 4 in `src/exit.h`).
  - Direction-aware OnUsed toggle (currently absent).
  - Auto-activation gating to specific objtypes only (port-side: read
    the gate list from a config rather than hard-coding).
  - String-event scripting for `"user"` (depends on script-system port).
  - Localized DOORLOCKED string.
  - All of the above can land incrementally; the existing source-side
    Activate works for "walk into door, get teleported", which is the
    Demo 1 minimum bar.
