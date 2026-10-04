# PauseSprite — Forensics Spec

**Status:** `forensics-complete` (negative finding).

> **TL;DR — there is no PAUSE sprite to reconstruct.** The brief's claim that
> `TPlayScreen::KeyPress` / `Joystick` "draw a PAUSE sprite when the game is
> paused" originates in a rename comment
> (`recon/discovered/renames/agent_screens.txt:176,181`) that mislabels a
> **status-flag name lookup** as a "sprite reference." The string `"PAUSE"`
> passed in those handlers is the **`OF_PAUSE = (1<<21)` object-flag name**
> (`src/object.h:546`), looked up by name in the flag-name table at
> `0x005d4770..0x005d47ec` (`recon/ghidra/_data.txt:79363-79435`). Engine pause
> is a pure state toggle (`DAT_00666924`, `recon/ghidra/_data.txt:132316-132318`)
> with **zero overlay draw**. No `.dat` archive (88 inspected) contains a
> `paus*` / `halt*` / `stop*` / `wait*` / `frozen*` entry; no `"Paused"`,
> `"Pause "`, `"GAME PAUSED"` literal exists in `recon/ghidra/_data.txt`.
> Implementation note for the reconstructor: ship the pause toggle (input gate
> + global flag); ship NO visual overlay.

---

## §0 — Sources & status

- **Class:** `TPlayScreen` (`cls_0x5a5320`, vtable @ `0x5a5320`).
- **Relevant vtable slots** (`recon/classes/cls_0x5a5320__vftable_5a5320.cpp:15,17`):
  - slot  8 (off `0x20`): `virt_meth_0x47c400_TPlayScreen_KeyPress`  @ `0x0047c400`.
  - slot 10 (off `0x28`): `virt_meth_0x47ce80_TPlayScreen_Joystick` @ `0x0047ce80`.
  - slot 19 (off `0x4c`): `virt_meth_0x47cf40_TPlayScreen_DispatchCommand` @ `0x0047cf40`.
- **Recon files read:**
  - `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp` (1-200) — references `s_PAUSE_005d769c` at `:29`; pause-flag guard at `:46`.
  - `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7926-7959` (`virt_meth_0x47ce80_TPlayScreen_Joystick` body).
  - `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7897-7922` (`meth_0x47c580` — the pause toggle).
  - `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7878-7893` (`meth_0x47c550` — request-pause setter).
  - `recon/classes_original/cls_0x5b4f30_TPlayScreen.cpp:535-558` (`meth_0x472e30` — the flag-name lookup that "PAUSE" is fed to).
  - `recon/ghidra/_data.txt:79363-79435` (object-flag-name pointer table starting at `PTR_s_IMMOBILE_005d4770`).
  - `recon/ghidra/_data.txt:86591-86646` (the four `s_PAUSE` literals referenced by KeyPress / Joystick / DispatchCommand).
  - `recon/ghidra/_data.txt:132316-132318` (`DAT_00666924` — global engine-pause flag, **3 XREFs total, none from draw code**).
  - `recon/discovered/renames/agent_screens.txt:176-184` (the rename note that mislabels "PAUSE" as a "sprite").
  - `src/object.h:546` (`#define OF_PAUSE (1<<21) // Script is paused`).
  - `src/playscreen.cpp:988-1035` (snapshot KeyPress / Joystick — no pause sprite, no toggle).
  - `src/ctrlmap.cpp:30` (`{VK_PAUSE, "PAUSE"}` — distinct: the friendly name for the Pause/Break virtual key in the key-binding system).
- **Asset archives scanned:** all 88 `.dat` files under `RevenantRevisited/data/resources_unzipped/` were dumped via `tools/ui/dump_dat.py`; **no entry name matches** `paus`, `halt`, `stop`, `wait`, `frozen`. `playscrn.dat` (47 entries: cursors, wedges, equip/spell/skill/map button up/down strips, hand/mouth/eye/swords/door verbs) does not contain a pause overlay.

**Snapshot note (supplementary — superseded by retail):**
`src/playscreen.cpp:988-1011` rewrites KeyPress (F12 editor-toggle then mode→`HandleKey`→`TScreen::KeyPress`); `:1032` stubs Joystick. The retail-style PAUSE gate path is not present in the snapshot. The snapshot is silent on the topic; nothing to import.

**Errors to close:** the rename note at `agent_screens.txt:176,181` says "references `\"PAUSE\"` sprite (system pause)". The evidence (object-flag-name table; toggle-only side effect; no asset; no draw site) shows this is a **misclassification** — `"PAUSE"` is the object-flag name `OF_PAUSE`, used as a *predicate* on the player object, not a sprite-asset name. The next pass over screen renames should rewrite those two comments.

---

## §1 — Overview

Two TPlayScreen input handlers (`KeyPress` slot 8, `Joystick` slot 10) and one command dispatcher (`DispatchCommand` slot 19) consult a global engine-pause state and toggle it on certain inputs. Mechanically:

- When the system pause is requested (mbr_0x5d8 != 0 in the latched-request field), and the player object exists and is not script-paused, the handler clears the request and invokes `meth_0x47c580` (the central pause helper). That helper sets `DAT_00666924 = 1` and pumps `TScreen::Joystick` + 9× `TScreen::TimerTick` to drain joystick state and let timer-driven UI catch up while paused.
- DispatchCommand's prologue (`:23-45`) repeats the same gate so command-driven pauses share the path; its `:46-48` early-out checks `DAT_00666924` and aborts any case-dispatch when the engine is paused.

**There is no visual overlay associated with this state.** No bitmap is loaded, no sprite is stamped, no text is composited. The state is purely behavioral (engine clock and input gating). Visual evidence the game is paused — if any — would come from the absence of motion in the world view and the absence of pulse/animation on existing HUD widgets, **not** from a "PAUSE" sprite.

---

## §2 — Asset roster

| asset | archive | entry name | WxH (measured) | role | source rects | cite |
|---|---|---|---|---|---|---|
| _(none)_ | — | — | — | — | — | brief asserts a "PAUSE sprite"; no asset exists. All 88 `.dat`s under `RevenantRevisited/data/resources_unzipped/` were enumerated with `tools/ui/dump_dat.py`; no entry name matches `paus`/`halt`/`stop`/`wait`/`frozen`. |

**The `"PAUSE"` literal at the KeyPress/Joystick call sites is a flag-name table key, not an asset name.** See §3 for the flag-name table layout.

---

## §3 — Coordinate frames & surfaces

**Not applicable — no draw site exists.**

For completeness: `DAT_00666924` (the global engine-pause flag) has exactly three XREFs in `recon/ghidra/_data.txt:132316-132318`:

| address | direction | function | cite |
|---|---|---|---|
| `0x0047c5a4` | W | `meth_0x47c580` (pause helper) | `_data.txt:132316` |
| `0x0047c625` | W | `meth_0x47c580` (pause helper) | `_data.txt:132317` |
| `0x0047d009` | R | `virt_meth_0x47cf40` (DispatchCommand prologue early-out) | `_data.txt:132318` |

Zero of those are draw / blit / surface calls. No element to lay out → no frame table, no chip model, no surfaces, no anchors. (Section deliberately empty — required per protocol §0; explained here per "if a section doesn't apply, say so explicitly and why".)

---

## §4 — Static element layout

**Not applicable.** No element to position.

---

## §5 — Draw order / composition

**Not applicable.** No draw call participates in the pause path. The only side effects of `meth_0x47c580` (`recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7897-7922`) are:

1. Set `mbr_0x5e0 = in_stack_4` (latch the requested pause state on the screen object). `(:7904)`
2. If unpausing (`in_stack_4 != 0` branch — note the inverted sense — actually means "request != 0 ⇒ stay paused": `mbr_0x5d8 = 0`, `DAT_00666924 = (in_stack_4 == 0)` i.e. set to 0). `(:7905-7908)`
3. If pausing (else branch): call `TScreen::Joystick`, then 9× `TScreen::TimerTick`, then `DAT_00666924 = 1`. `(:7910-7920)`

Step (3)'s nine `virt_meth_0x490660_48` (= `TScreen::TimerTick`, `recon/classes_original/cls_0x5a5ed4__vftable_5a5ed4.cpp:19`) calls **pump the screen's timer state** so any in-flight animations advance to a quiescent point before the freeze. They do not render a sprite.

---

## §6 — Algorithms (pseudocode per helper)

### `meth_0x472e30` — object-flag name → boolean (the function the brief calls a "sprite lookup")

Source: `recon/classes_original/cls_0x5b4f30_TPlayScreen.cpp:535-558`.

```
bool TPlayer::HasFlagNamed(const char* name) {
    int idx = 0;
    for (const char** p = &PTR_s_IMMOBILE_005d4770;
                       p < &PTR_s_talis1_005d47f0; p++, idx++) {
        if (stricmp(name, *p) == 0) {
            if (idx < 0) return false;
            return (this->TCharacter.TObjectInstance.flags & (1u << (idx & 0x1f))) != 0;
        }
    }
    return false;
}
```

**The flag-name table** (`recon/ghidra/_data.txt:79363-79435`) is a contiguous array of `char*` starting at `PTR_s_IMMOBILE_005d4770 = 0x005d4770` (index 0 = `"IMMOBILE"`) and ending just before `PTR_s_talis1_005d47f0 = 0x005d47f0`. The table is 32 entries (`(0x47f0 − 0x4770)/4 = 0x20/4 = 32`). `"PAUSE"` lives at `0x005d47c4`, **table index `(0x47c4 − 0x4770)/4 = 0x54/4 = 21`** — exactly `1<<21`, matching `#define OF_PAUSE (1<<21)` in `src/object.h:546`.

| idx | name | bit | `src/object.h:#define` |
|---|---|---|---|
| 0 (`0x5d4770`) | `IMMOBILE` | `1<<0` | `OF_IMMOBILE` (line 525) |
| 21 (`0x5d47c4`) | **`PAUSE`** | `1<<21` | **`OF_PAUSE` (line 546)** |
| 23 (`0x5d47cc`) | `PARALIZE` | `1<<23` | `OF_PARALIZE` (line 548) |

(Other entries — `LIGHT`, `MOVING`, `ANIMATING`, `DISABLED`, `INVISIBLE`, `EDITOR`, `FOREGROUND`, `SELDRAW`, `REVEAL`, `GENERATED`, `ANIMATE`, `PULSE`, `WEIGHTLESS`, `COMPLEX`, `NOTIFY`, `NONMAP`, `ONEXIT`, `NOWALK`, `NOCOLLISION`, `VIRGIN`, `LOADING`, `INVULNERABLE`, `BACKGROUND`, `INVENTORY`, `CALLEDPREDEL` — are decoded the same way and listed at `_data.txt:79370-79435` for cross-check.)

### `meth_0x47c580` — the pause toggle

Source: `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7897-7922`.

```
void TPlayScreen::TogglePause(int requested) {
    this->mbr_0x5e0 = requested;        // latch current active-pause state
    if (requested != 0) {
        // path A — "unpause / clear request":
        this->mbr_0x5d8 = 0;            // clear pause-request latch
        DAT_00666924 = (requested == 0); // = 0  -> engine running
        return;
    }
    // path B — "engage pause":
    TScreen::Joystick(this);            // drain any pending joystick edge
    for (int i = 0; i < 9; i++) TScreen::TimerTick(this); // pump 9 ticks
    DAT_00666924 = 1;                   // engine paused
}
```

> The two branches read counter-intuitively. The caller passes `requested = 1` to *unpause* (return to running) and `requested = 0` to *engage* pause. This is consistent with the call at `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7944` `meth_0x47c580(this)` (no explicit `requested` arg → stack-passed sentinel `1` from the `in_stack_00000004` ABI quirk; see UNCONFIRMED-A).

### Joystick handler (`virt_meth_0x47ce80`, slot 10)

Source: `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7926-7959`.

```
void TPlayScreen::Joystick(int param_1, int param_2) {
    if (this->mbr_0x5d4 != 0) return;            // screen disabled -> ignore
    if (DAT_00667fcc &&                          // global player object exists
        DAT_00667fcc->TScreen.mbr_0x84 != 0 &&   // player has a controller
        TPlayer::HasFlagNamed(DAT_00667fcc, "PAUSE") == 0 && // NOT script-paused
        ((cls_0x4922c0*)DAT_00667fcc->TScreen.mbr_0x84)->mbr_0x1c == 1 &&
        (cls_0x4922c0::meth_0x492490(controller), this->mbr_0x5d8 != 0)) {
        this->mbr_0x5d8 = 0;                     // consume the request
        meth_0x47c580(this);                     // engage / lift pause
        if (DAT_00667fcc) DAT_00667fcc->meth_0x472db0(&DAT_005d7604);  // clear "AI" flag (0x41 0x49 = "AI\0" at _data.txt:86602)
        if (DAT_0065cb34) (*DAT_0065cb34)[0x2c]();  // call something on a global UI object
        this->mbr_0x5dc = 1;                     // mark dirty / handled
        return;
    }
    TScreen::Joystick(this);                     // fall through to base
}
```

`s_PAUSE_005d7694` (the literal at `:7940`, cite `_data.txt:86641-86642`) is the **flag name string** passed to `HasFlagNamed`. It is not an asset name and is not used as a draw source.

### DispatchCommand prologue (`virt_meth_0x47cf40`, slot 19)

Source: `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:23-48`.

```
if (this->mbr_0x5d4 != 0 && command != 13) return;          // disabled gate (:23)
if (DAT_00667fcc && player->mbr_0x84 != 0 &&
    TPlayer::HasFlagNamed(player, "PAUSE") == 0 &&          // :29 — same gate as Joystick
    ((controller*)player->mbr_0x84)->mbr_0x1c == 1 &&
    cls_0x4922c0::meth_0x492490(controller) != 0 &&
    this->mbr_0x5d8 != 0) {
    this->mbr_0x5d8 = 0;
    meth_0x47c580(this);                                    // same toggle (:34)
    if (DAT_00667fcc) DAT_00667fcc->meth_0x472db0(&DAT_005d7604); // clear AI flag
    this->mbr_0x5dc = 1;
    if (DAT_0065cb34) (*DAT_0065cb34)[0x2c]();
}
if (DAT_00666924 != 0) { /* pause active -> skip command switch */ return; } // :46-48
switch(command) { ... }
```

### KeyPress (`virt_meth_0x47c400`, slot 8)

**Body not extracted** in any recon dump (verified — only XREF traces and rename note exist). From XREFs at `_data.txt:86591,86595` the body references `s_PAUSE_005d75f4` at `:0x47c45f` and `s_PAUSE_005d75fc` at `:0x47c4c3`. By analogy with the Joystick and DispatchCommand prologues (both feed the same gate to the same toggle), the KeyPress handler almost certainly does the same `HasFlagNamed(player, "PAUSE")` gate twice (perhaps once before and once after dispatching to the script-bound key action, or once each for two related shortcuts). See UNCONFIRMED-B for the resolve step.

---

## §7 — Effects & shadows

**Not applicable.** No draw occurs. No shadow setup (`FUN_00438d80`), no blit (`FUN_004bd680` / `vtable+0x5c`), no text composite (`FUN_004be2b0`), no chroma key, no effect-pipeline registration. UI_METHOD_MAP primitives play no role in the pause path.

---

## §8 — Text rendering

**Not applicable.** No `FUN_004be2b0` call is reachable on the pause path; no `"Paused"` / `"PAUSED"` / `"Game Paused"` literal exists in `_data.txt` (verified with `grep -E '"[Pp]aus'`).

---

## §9 — Animation & dynamic behavior

The only "dynamic" behavior is the **9× TimerTick pump** in path B of `meth_0x47c580` (`:7910-7918`), which drains pending screen-timer events to a quiescent state before the freeze. This is not an animation in the NOMENCLATURE §5 sense (no tweened value, no ramp, no per-frame counter) — it is a one-shot synchronous "wind-down."

Dirty/redraw model: `mbr_0x5dc = 1` (`:7949` in Joystick, `:40` in DispatchCommand) sets a per-screen dirty flag. The flag's read site is **not** in the recon (would live in `virt_meth_0x47b4d0_TPlayScreen_Pulse` or `virt_meth_0x47bd20_TPlayScreen_DrawBackground`, neither of which contains any `s_PAUSE` or pause-flag XREF). Setting `mbr_0x5dc` likely just forces one more redraw of the *existing* HUD (so any state-driven HUD widgets get a chance to repaint), **not** a redraw of a pause overlay.

---

## §10 — Input & dispatch

The PAUSE feature is purely an input path; this is the section it belongs in.

### The dispatch graph

```
keyboard:VK_PAUSE  ──┐
gamepad:select?    ──┼──> TPlayScreen::KeyPress  (slot 8,  0x47c400)  ── sets mbr_0x5d8=1  ──┐
                     │    TPlayScreen::Joystick  (slot 10, 0x47ce80) ── sets mbr_0x5d8=1  ──┤
command(13?)       ──┴──> TPlayScreen::DispatchCommand (slot 19, 0x47cf40)               ──┘
                                                                                            │
                                                                                            ▼
                              gate: this.mbr_0x5d4==0  AND player object exists  AND      ┌──┴──┐
                                    !HasFlagNamed(player,"PAUSE")  AND                    │  ✓  │
                                    controller.mbr_0x1c==1  AND  this.mbr_0x5d8!=0       │  ?  │
                                                                                          └──┬──┘
                                                                                             ▼
                                                                              meth_0x47c580  (toggle)
                                                                                             │
                                                              writes DAT_00666924 ∈ {0,1}    │
                                                                  (engine-pause flag)        │
                                                                                             ▼
                              DispatchCommand prologue early-out at :46  reads               │
                                  DAT_00666924 != 0  ⇒ skip command switch                  │
                                                                                             │
                              (no draw site reads DAT_00666924 — verified via 3-XREF count) │
```

### Hit rects

**Not applicable** — there is no clickable PAUSE control. The toggle is keyboard/gamepad/command-driven.

### Command id

The brief mentions "Joystick path." The Joystick handler at `0x47ce80` takes two `undefined4` params (no key index is decoded into a command id at this depth — the gate fires based on the screen's own `mbr_0x5d8` latch, which is set by some upstream input layer). UNCONFIRMED-C documents this.

### Modal-state globals

| global | role | values | cite |
|---|---|---|---|
| `DAT_00666924` | **system pause flag** (engine global) | `0` = running, `1` = paused | `_data.txt:132316-132318` (3 XREFs total) |
| `TPlayScreen::mbr_0x5d4` | screen-disabled gate | `0` = enabled (path runs), `!=0` = disabled (return) | `cls_0x5a5320_likely_TCharacter.cpp:7937` (Joystick), `:7816` (MouseClick), `FUN_0047cf40_TPlayScreen_DispatchCommand.cpp:23` |
| `TPlayScreen::mbr_0x5d8` | pause-request latch (set by upstream input → consumed by gate) | `0` = no pending request, `!=0` = request | `cls_0x5a5320_likely_TCharacter.cpp:7878-7893` (`meth_0x47c550` is the setter), `:7942` (consumer in Joystick), `:7905` (cleared in toggle) |
| `TPlayScreen::mbr_0x5dc` | "pause-handled / dirty" flag | `1` after toggle | `:7949` (Joystick), `FUN_0047cf40:40` |
| `TPlayScreen::mbr_0x5e0` | active pause state (mirrors `DAT_00666924`) | `0` / `1` | `:7904` |
| `DAT_00667fcc` | global player object pointer (`TPlayer*`) | non-null when in-game | `cls_0x5a5320_likely_TCharacter.cpp:7938`; gate condition |
| `DAT_005d7604` | the literal `"AI\0\0"` (`0x41 0x49 0x00 0x00`) passed to `meth_0x472db0` to clear the AI flag bit on the player | constant | `_data.txt:86599-86605` |

The AI-flag clearing (`:7946`) on toggle is consistent with "engaging pause stops the player object's AI ticking"; this is **behavior**, not a draw.

---

## §11 — Retail bugs NOT to reproduce

**Not applicable.** No visual exists; no rendering bug can.

The one bug-class relevant here is **the upstream rename note's mislabel** (`agent_screens.txt:176,181` — "references `\"PAUSE\"` sprite"). A reconstructor reading only that note (without consulting `_data.txt`) would hunt for a non-existent asset and likely invent a procedural stand-in (see [[feedback-no-standins]]). This spec exists to short-circuit that failure mode.

---

## §12 — Reconstruction pseudocode

```cpp
// In the port's PlayScreen input handlers — paint() / update() are unaffected;
// there is NO paint() for "PauseSprite" because there is no PauseSprite.

void TPlayScreen::PausableInputGate(/*params from caller*/) {
    if (this->disabled) return;                               // mbr_0x5d4 gate
    if (!Player) return;
    if (!Player->controller) return;
    if (Player->HasFlag(OF_PAUSE)) return;                    // <-- this is what "PAUSE" means.
                                                              //     OF_PAUSE = 1<<21 (src/object.h:546).
    if (Player->controller.state != 1) return;
    Player->controller.OnGateTick();                          // meth_0x492490 — opaque side-effect
    if (!this->pauseRequest) return;                          // mbr_0x5d8
    this->pauseRequest = 0;
    TogglePause(/*requested=*/ Engine::IsPaused() ? 1 : 0);   // see below
    if (Player) Player->ClearFlagByName("AI");                // meth_0x472db0(&"AI")
    this->pauseDirty = true;                                  // mbr_0x5dc
    if (SomeUiObject) SomeUiObject->Notify();                 // DAT_0065cb34 slot 0x2c
}

void TPlayScreen::TogglePause(int requested) {
    this->activePauseLatch = requested;                       // mbr_0x5e0
    if (requested != 0) {                                     // unpause path
        this->pauseRequest = 0;
        Engine::SetPaused(false);                             // DAT_00666924 = 0
    } else {                                                  // engage-pause path
        this->base.Joystick();                                // drain joystick edge
        for (int i = 0; i < 9; ++i) this->base.TimerTick();   // pump 9 ticks
        Engine::SetPaused(true);                              // DAT_00666924 = 1
    }
}

// In DispatchCommand prologue:
void TPlayScreen::DispatchCommand(int cmd) {
    if (this->disabled && cmd != 13) return;
    PausableInputGate();                                      // same path runs here
    if (Engine::IsPaused()) return;                           // skip case switch
    switch (cmd) { /* ... */ }
}

// NO paint code. There is no overlay. Do NOT add a procedural "PAUSE" text or
// box "to be safe" — retail showed no overlay; the port must not either.
```

---

## §13 — Port mapping notes

- `Engine::SetPaused(bool)` / `Engine::IsPaused()` map to a single `bool` (or the existing snapshot `g_running` editor flag — see `src/editor.cpp:1856`, conceptually adjacent). This is a behavior port, not a UI port.
- `OF_PAUSE` is already in the snapshot at `src/object.h:546` — no new define needed.
- The `"AI"` flag-name (`DAT_005d7604`) is in the same flag-name table at offset index `(0x47?? − 0x4770)/4` — confirm during port; preliminary scan suggests `EDITOR`/`AI` is not in the IMMOBILE table directly but is a separate one. UNCONFIRMED-D.
- The 9-tick TimerTick pump in `TogglePause` is unusual; the port should reproduce it (or document why it's omitted) since it's the only thing that lets in-flight per-tick UI (cursor blink, hover ramps) settle before the freeze.
- **No renderer primitives are needed.** `src/renderer.h` / `src/font.h` / `src/surface.h` see no call from this path.

---

## §14 — UNCONFIRMED / open questions

**UNCONFIRMED-A — `meth_0x47c580` toggle-arg semantics.**
The Ghidra ABI artifact `in_stack_00000004` makes the explicit `requested` arg invisible; I inferred from call sites (Joystick `:7944` calls bare `meth_0x47c580(this)`; DispatchCommand `:34` calls `FUN_0047c580(1)`) that the function takes one int (the stack quirk is Ghidra's `__thiscall` representation of arg 1). The argument-sense is also inverted-feeling (`requested != 0` ⇒ unpause path). **Impact if wrong:** the toggle could flip pause/unpause in reverse — easy to catch at first manual test, but worth confirming. **Resolve:** `DecompileAddr.java 0x0047c580` (clean extract) and `DecompileAddr.java 0x0047c500` / `0x0047c550` (siblings — `meth_0x47c550` is the *setter* for `mbr_0x5d8`; it calls `meth_0x47c580(this)` when its arg is 0 — `cls_0x5a5320_likely_TCharacter.cpp:7886`). Reading the three together pins the sense.

**UNCONFIRMED-B — KeyPress (slot 8, `0x0047c400`) body.**
Not extracted in any recon file. Confirmed via `grep -rn 'FUN_0047c400\|0047c400'` across `recon/discovered/`, `recon/classes/`, `recon/ghidra/`. The `agent_screens.txt:177-179` rename predicts the same PAUSE gate as Joystick (two `s_PAUSE` XREFs at `:0x47c45f`, `:0x47c4c3`), and the data globals `DAT_0065c134` (XREFs at `:0x47c452`, `:0x47c4b6`) and `DAT_0065d190` (`:0x47c422`) are referenced — likely the keymap and the multiplayer-state flag respectively. **Impact if wrong:** could miss a second pause-binding I haven't accounted for. **Resolve:** `DecompileAddr.java 0x0047c400`.

**UNCONFIRMED-C — which input key/button triggers the gate.**
Neither Joystick (`0x47ce80`) nor DispatchCommand (`0x47cf40`) decodes a key id at the gate — they consume `mbr_0x5d8` which is *latched* by some upstream input layer (`meth_0x47c550` is the latch setter, `:7878-7893`). Hunting for `meth_0x47c550`'s callers reveals the actual key/button. Likely candidates: VK_PAUSE (`src/ctrlmap.cpp:30`) and a gamepad start/select button. **Impact if wrong:** doesn't change the spec (no draw) but the port needs to wire the right input. **Resolve:** `grep -rn 'meth_0x47c550\|FUN_0047c550' recon/` (cross-reference all callers).

**UNCONFIRMED-D — the `"AI"` flag-name slot.**
`DAT_005d7604` is the bytes `"AI\0\0"` (`_data.txt:86602-86605`), passed to `meth_0x472db0` (the flag-bit *clearer*, sibling of `meth_0x472e30` the *getter* — `recon/classes_original/cls_0x5b4f30_TPlayScreen.cpp:535` vicinity). Whether "AI" lives in the same `PTR_s_IMMOBILE`-rooted table or a separate one is unverified. Doesn't affect the pause spec but matters for the port. **Resolve:** read `meth_0x472db0` body (sibling of `meth_0x472e30`).

**UNCONFIRMED-E — `agent_screens.txt` rename comments.**
Lines 176 and 181 say `"references \"PAUSE\" sprite (system pause)"`. The evidence in this spec contradicts the "sprite" framing. **Impact if wrong (i.e., if the rename note IS correct and I missed something):** there would have to be a `cls_0x5a5320_likely_TCharacter.cpp:7926-7959` (Joystick) or `0x47c400` (KeyPress, not extracted) draw call I overlooked, AND a matching `.dat` asset, AND a draw-site XREF on `DAT_00666924`. I find zero of those. **Resolve:** the rename comments should be edited to read `"references \"PAUSE\" object-flag name (OF_PAUSE, src/object.h:546) — gate, not asset"`. Out of scope for this spec to edit (forensics doesn't edit recon), but flagging it so the next rename pass can correct.

**UNCONFIRMED-F — `DAT_0065cb34->[0x2c]` (the post-toggle UI-object notification at Joystick `:7953` and DispatchCommand `:41`).**
A global UI object (likely the in-game menu screen or the cursor manager) gets a vtable call on engage. Could be a "show pause menu" path, in which case the *menu* (not a sprite overlay) is the visible artifact of pausing — but that menu is a separate screen (TInGameMenu et al.) with its own forensics scope. **Impact:** if this brings up an in-game menu, the user-facing "you're paused" cue exists but lives in a different panel and is already covered by `InGameMenuDef_SPEC.md`. **Resolve:** `DumpVtable.java` on `DAT_0065cb34` (read its type at runtime via `DecompileAddr.java 0x0065cb34`) and identify slot `+0x2c`.

---

## Summary (for the orchestrator)

Forensics-complete, negative finding. The "PAUSE sprite" in the task brief is a **propagated mislabel from `recon/discovered/renames/agent_screens.txt:176,181`**. The `"PAUSE"` literal at `0x005d75f4 / 0x005d75fc / 0x005d7694 / 0x005d769c` (referenced by KeyPress / Joystick / DispatchCommand) is the **`OF_PAUSE` object-flag name** (`src/object.h:546`, bit 21 of `TObjectInstance.flags`), used as a predicate in `meth_0x472e30` ("does player have the OF_PAUSE bit?"). The engine pause toggle (`meth_0x47c580`) sets the global `DAT_00666924`, drains joystick state, and pumps 9× TimerTick — it does **not** load, blit, or composite any asset. No `.dat` entry, no string literal, no draw XREF supports a "PAUSE sprite." The reconstruction agent must implement the input gate + global pause toggle and must **not** invent a visual overlay. See §14 for the five remaining open questions (KeyPress body, toggle-arg sense, input-key wiring, AI-flag slot, post-toggle UI notification — none affecting the negative-finding conclusion).
