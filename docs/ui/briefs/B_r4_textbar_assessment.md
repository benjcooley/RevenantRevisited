# B.r4 — TTextBar assessment (Wave-1C)

**Status:** COMPLETE — with a high-impact reversal of an inherited assumption.

**Bottom line up front:**

1. **`cls_0x5a4358` is NOT TTextBar.** The "_likely_TTextBar" suffix already in the recon tree is a propagated mis-identification from the original size-heuristic mapping (recon/docs/CLASS_MAPPING.md). String anchors and field shape decisively identify `cls_0x5a4358` as a TPane subclass in the **editor** family — most likely `TConsolePane` (the editor's interactive command line console), inheriting from `TTextPane` (whose vtable address is `cls_0x5a42d4`, also seen in the recon tree, calling into `cls_0x5a4358`'s methods as base-class calls).
2. **The real retail TTextBar lives elsewhere** — its init wrapper is `FUN_0054bf70` (string-anchored as `"text bar"` in B.r3) and its health-display rendering is at `FUN_0054cb00` (Ghidra mis-assigned this to TCharacter / cls_0x5a5320 via inheritance flattening). Methods cluster in the `0x54bf70`..`0x54d800` range.
3. **Retail TTextBar IS substantially richer than pre-release** — three TMosaicSurface child buffers (transparent multi-buffered background), `0x450` byte text buffer (vs pre-release's 80-char), and ~6+ caller methods around 0x54c000 — consistent with the user's "transparent multi-line game-log overlay with scrollback" description. But this evidence sits behind `FUN_0054bf70`, not cls_0x5a4358.

This brief documents the mis-identification, the evidence for the correct identification, and what's needed to actually hunt the retail TTextBar in a future wave.

---

## Class identity confirmation (Rule 1 evidence chain for cls_0x5a4358 = TConsolePane)

### Line 1 — String anchors

Two distinctive strings appear in the recon decomp of `cls_0x5a4358` (file `recon/classes_converted/cls_0x5a4358_likely_TTextBar.cpp`, function `virt_meth_0x43e370`, lines 296–319):

| Recon symbol | Resolved literal | Source location |
|---|---|---|
| `s_Could_not_create_kill_event_for_c_005ceefc` | `"Could not create kill event for command processor!"` | `attic/src/editor.cpp:546` (`TConsolePane::Initialize`) |
| `s_Could_not_create_character_event_005cef30` | `"Could not create character event for command processor!"` | `attic/src/editor.cpp:550` (`TConsolePane::Initialize`) |

These two error strings are emitted **only** by `TConsolePane::Initialize` in the pre-release source (verified via grep across the codebase — they appear nowhere else). The retail function emitting these strings is unambiguously `TConsolePane::Initialize`.

### Line 2 — Struct shape and threading signature

The class layout (`recon/classes_converted/cls_0x5a4358.h`) shows fields up to `mbr_0xc8` (~204 bytes). The pre-release `TConsolePane` layout — `TPane(~80)` + `TTextPane(15 × 4 = 60)` + `TConsolePane-specific(11 × 4 = 44)` = ~184–204 bytes — matches that exactly.

The same `virt_meth_0x43e370` (TConsolePane::Initialize) does, in order:

1. Calls `virt_meth_0x43da00` on `this` (the parent class `Initialize` — see line 3 below)
2. Calls `CreateEventA` twice
3. Calls `FUN_0058bd47` (Win32 `_beginthreadex`), with thread entry `LAB_0043f5a0` and `(int)this` as argument, then `SetThreadPriority(hThread, 1)`

This is line-for-line `TConsolePane::Initialize` from `attic/src/editor.cpp:531–563` — kill event, character event, then begin the command-processor thread with `_beginthreadex`. `mbr_0xc8` stores the thread handle (matching pre-release `cmdthreadhandle`).

No other pre-release class spins up a thread inside its `Initialize`. This is a structural fingerprint.

### Line 3 — Inheritance hierarchy via parent-class call

Inside the same Initialize, the very first call is `this->virt_meth_0x43da00();` (line 296). I extracted that function to `recon/discovered/cls_0x5a4358_TConsolePane_parent_TTextPane_Initialize_43da00.cpp` and it:

- Allocates two `FUN_00482fb0(10000)` buffers (text buffer + undo/redo buffer; pre-release pattern)
- Initializes them with `0x7b 0x00` (= `{` + null terminator)
- The byte `'{'` is treated specially throughout the buffer-shifting methods (e.g. `meth_0x43dda0` lines 145–148: `if (*pcVar4 != '{') iVar5 = iVar5 + 1;` — color/style escape markers that don't count toward character width)
- Calls `FUN_00491900()` at the top — that's the TPane base `Initialize`

Cross-referencing `recon/classes_original/cls_0x5a42d4.cpp`: that class also implements `virt_meth_0x43da00` (line 34) and the implementation is **byte-identical** to the function at `0x43da00`. So `cls_0x5a42d4` IS the class that **owns** method `0x43da00` (i.e., `cls_0x5a42d4` is `TTextPane`).

That gives us the inheritance chain:

| Recon class | Retail vtable addr | Pre-release identity |
|---|---|---|
| `cls_0x5a4494` (probably — needs vtable extract) | likely `0x5a4494` | `TPane` (or close ancestor; calls `virt_meth_0x491900` = TPane::Initialize) |
| `cls_0x5a42d4` | `0x5a42d4` | **`TTextPane`** (160 bytes per OOAnalyzer header) |
| `cls_0x5a4358` | `0x5a4358` | **`TConsolePane`** (`TTextPane` + thread/event machinery) |

### Conclusion (98% threshold met)

`cls_0x5a4358` = `TConsolePane`. Three independent lines of evidence:
- Two retail strings exactly matching two src strings unique to `TConsolePane::Initialize`
- Thread-with-event-pair initialization pattern unique to `TConsolePane`
- Parent-class-call chain matching `TTextPane` (via cls_0x5a42d4 sharing the same `virt_meth_0x43da00`) → `TPane`

The "_likely_TTextBar" suffix on the recon file should be considered **retracted**.

---

## What the **real** retail TTextBar looks like (evidence trail, NOT yet a rename)

### Init wrapper at `FUN_0054bf70`

Already string-anchored in [B.r3 §"Pane init function candidates" line 53](B_r3_playscreen_panes.md): `FUN_0054bf70` → `"text bar"`. Called from two sites — one is the TPlayScreen pane construction sequence (`0x47abfd`).

I extracted it to `recon/discovered/FUN_0054bf70_TTextBar_init.cpp` (1110 bytes of decomp). Highlights:

- Allocates the TTextBar instance buffer via `FUN_00482fb0(0x450)` → 1104 bytes (**vastly larger than pre-release `TTextBar`**, which is ~`sizeof(TPane)` + 80 + 80 + 4 + 4 + 1 + 1 ≈ 256 bytes max).
- Allocates **three** `TMosaicSurface` child surfaces (`*piVar7 = (int)&PTR_FUN_005a3980;` for the foreground, `*puVar8 = &PTR_FUN_005a3e7c;` × 2 for two background surfaces).
- Stored at `this+0x84`, `this+0x88`, `this+0x8c` — three buffered surfaces.
- Branches on `DAT_006680c8` (likely a "video mode" / "double-buffered" flag) to choose surface count and flags (`0x488` vs `0x400` vs `0x4000888`).
- Error path: `FUN_00481c10(s_Couldn_t_initialize_mosaic_surfa_005cdc70, 0)` = `FatalError("Couldn't initialize mosaic surface!")`.
- Initializes fields up to `+0x120` (sets `+0x120 = 0xffffffff`) — TTextBar is at least 0x124 bytes wide (and likely larger; the 0x450 allocation gives plenty of room for an internal text buffer of e.g. 800–1000 chars).

This evidence places retail TTextBar's **per-instance memory at ~10× pre-release** and confirms a **three-mosaic-surface buffered rendering pipeline** — the infrastructure required for the transparent overlay with stable scrollback animation described in `CLASSIC_HUD_REFERENCE.md`.

### DrawBackground / health-display block at `FUN_0054cb00`

Extracted to `recon/discovered/cls_0x5a5320_TPlayScreen_or_TTextBar_DrawHealth_54cb00.cpp`.

Compare pre-release `TTextBar::DrawBackground` (the health-bar block) at `src/textbar.cpp:64–73`:

```cpp
int32_t hue = (level * 155) / 176;
if (hue > 16) hue -= 16; else hue = 0;
Display.PutHue(min(0, -(186 - level)), 1, GameData->Bitmap("texthealthbar"), drawmode, hue);
```

against retail `FUN_0054cb00`:

```
iVar5 = (this->mbr_0x98 * 0x9b) / 0xb0;     // 0x9b=155, 0xb0=176 ✓
if (iVar5 < 0x11) iVar5 = 0;                // 0x11=17 (>=17 path)
else iVar5 = iVar5 + -0x10;                 // -0x10=-16  ✓
uVar1 = this->mbr_0x98 - 0xba;              // 0xba=186  ✓
FUN_004bd8c0((0 < uVar1) - 1 & uVar1, 1, FUN_0046d710(s_texthealthbar_...), 0x100, iVar5);
```

**Exact constant-by-constant match** on (155, 176, 16, 186). The `(0 < uVar1) - 1 & uVar1` idiom is `min(0, uVar1)` for signed compare. `FUN_004bd8c0` is `Display::PutHue`. The string `"texthealthbar"` is referenced **exactly once in the entire binary** (per `_data.txt` xref count) and it's from this function.

`mbr_0x98` is therefore the retail equivalent of pre-release `level`. The tween-toward-targetlevel logic at the top of the function (`*0x98 += 4` / `-= 4` toward `*0x9c`) matches the pre-release `HEALTH_INCREMENT=4` increment.

So `FUN_0054cb00` is the health-bar drawing sub-block of `TTextBar::DrawBackground` (it's currently mis-labeled as `TCharacter::meth_0x54cb00` in `cls_0x5a5320_likely_TCharacter.cpp` lines 9994–10030 — Rule-2 inheritance flattening: Ghidra associated it with TCharacter because of caller-this register dataflow).

### Cluster of likely TTextBar methods

`CallersOf 0x0054cd40` shows tight caller cluster (8 callers all in `0x54c000`..`0x54d800`). That range is likely the entire TTextBar implementation:

- `FUN_0054bf70` — Initialize (string: "text bar")
- `FUN_0054cb00` — DrawHealthBar inner block
- `FUN_0054cd40` — likely DrawBackground (large; called by many siblings)
- `FUN_0054c440`, `FUN_0054c9c0`, `FUN_0054ca60`, `FUN_0054cad0`, `FUN_0054d0c0`, `FUN_0054d390`, `FUN_0054d4a0` — sibling drawing/state methods

A focused follow-up wave should dump each, find the vtable that `FUN_0054bf70` ultimately wires up, then identify `Print`, `Clear`, `SetHealthDisplay`, `ClearHealthDisplay`, `SetLevels` by signature.

---

## Feature diff retail vs pre-release (preliminary, from FUN_0054bf70 evidence only)

| Feature | Pre-release `TTextBar` | Retail (inferred) |
|---|---|---|
| Backing surfaces | None — draws directly via `Display.Box`/`Display.WriteText` | **Three TMosaicSurface child buffers** at +0x84/+0x88/+0x8c (one fg, two bg) — enables transparent multi-buffer scrolling |
| Instance size | ~256 bytes | **~1104 bytes** (`0x450` allocation) — room for ~800-char scrollback or wide structured per-line records |
| Text buffer | `char text[80]` (single line) | At minimum a much larger buffer; field +0x68 / +0x70 are line/section pointers (per init) |
| Health display | `name[80]`, `level`, `targetlevel`, `pulsecheck`, `animating` | Health tween at `+0x98`/`+0x9c` — confirmed present (DrawHealthBar block uses same 155/176/16/186 constants) |
| Color per line | Not present (single white font) | **Likely present** — `'{'`/`'}'` color-escape handling pattern is on the TConsolePane side; whether TTextBar uses it remains to be confirmed |
| Multi-line / scrollback | Not present — single 80-char line | **Strongly implied** by 3-surface + 1104-byte struct + `CLASSIC_HUD_REFERENCE.md` description; not yet confirmed in extracted decomp |
| Public API | `Print(char*, ...)`, `Clear()`, `SetHealthDisplay(name, level)`, `SetLevels`, `ClearHealthDisplay` | Unknown until methods extracted; the pre-release API is likely a subset that survived |

**Pre-release `SetHealthDisplay` disposition:** Confirmed retained — the constants 155/176/16/186 and the tween-by-4 in `FUN_0054cb00` are identical, so the opponent-name-and-health overlay still lives in TTextBar (it was NOT moved to TPlyrStatusBar / right-side character panel). This matches the `CLASSIC_HUD_REFERENCE.md` notes that the textbar still occasionally shows enemy health.

---

## Port plan recommendation

**Do NOT diff-and-port from cls_0x5a4358** — it's TConsolePane, an editor class that has nothing to do with the game-log overlay. The next wave needs to:

1. Extract `FUN_0054bf70` + sibling functions in `0x54c000`..`0x54d800` into proper recon files (use `<class>_TTextBar_<func>_<addr>.cpp` naming).
2. Find the TTextBar **vtable address** by scanning `FUN_0054bf70` for the constructor (the constructor sets `*this = &vtable_XXXXXX`). The init wrapper itself doesn't set the vtable; the constructor it calls (which I did not extract due to time) does.
3. Cross-reference each extracted method against pre-release `src/textbar.cpp` for the Print/Clear/SetHealthDisplay/SetLevels/ClearHealthDisplay golden-path matches.
4. Then port: keep pre-release `src/textbar.{h,cpp}` as the API scaffold, replace single-line `text[80]` with a scrollback line list, replace direct-draw `DrawBackground` with TMosaicSurface buffered composition, preserve `SetHealthDisplay`/`SetLevels`/`ClearHealthDisplay` verbatim from pre-release (they appear unchanged).

The editor `TConsolePane` body sitting in `cls_0x5a4358_likely_TTextBar.cpp` is genuinely useful — it's TTextPane buffer-manipulation logic that we'd want when porting the editor — but it should be relabeled `cls_0x5a4358_TConsolePane.cpp` (or split: cls_0x5a42d4 = TTextPane, cls_0x5a4358 = TConsolePane) so future agents don't keep building on the wrong anchor.

---

## Candidates with weak evidence (NOT in renames)

| Symbol | Tentative ID | Why deferred |
|---|---|---|
| `FUN_0054cd40` | TTextBar::DrawBackground (outer) | Strong by clustering but not extracted/inspected in this hunt |
| `FUN_0054c440` ..  `FUN_0054d4a0` | TTextBar::Print / Clear / Animate / etc. | Need extraction + per-method string anchors |
| `cls_0x5a42d4` | TTextPane | Decisive: implements `virt_meth_0x43da00` (TTextPane::Initialize) byte-identical to base call from cls_0x5a4358. Putting in renames file. |
| `cls_0x5a4494` (referenced as base of cls_0x5a42d4) | TPane | Highly likely — needs vtable dump to confirm |
| Real TTextBar leaf vtable address | unknown (~0x5a????) | Needs constructor extraction from `FUN_0054bf70` call graph |

---

## Merging suspicions (Rule 2)

1. **TCharacter's `meth_0x54cb00`** is actually TTextBar's DrawHealthBar block. Ghidra associated it with `cls_0x5a5320` (TCharacter) — false attribution. Likely cause: caller-this register dataflow heuristic. Don't trust any "TCharacter" method in `0x54c000`..`0x54d800`.
2. **cls_0x5a4358 may merge TTextPane + TConsolePane.** OOAnalyzer's `cls_0x5a42d4` (TTextPane, 160 bytes) and `cls_0x5a4358` (TConsolePane, 204 bytes) are listed as separate classes, so the merge isn't total — but the boundary may be fuzzy. The relabel should probably annotate both: `cls_0x5a42d4_TTextPane` and `cls_0x5a4358_TConsolePane`.

---

## Hunt log

**2026-05-16 (Wave-1C):**

- Read `B.r2`, `B.r3`, `AGENT_PROTOCOL.md`, `recon/discovered/README.md`.
- Read pre-release `src/textbar.{h,cpp}`. Pre-release TTextBar is small (3 KB), single-line, single-color, with `SetHealthDisplay` opponent-name overlay.
- Read `recon/classes_converted/cls_0x5a4358_likely_TTextBar.cpp` (510 lines, 12 KB) and `cls_0x5a4358.cpp` (the non-_likely_ variant — same content, different decomp pass).
- Extracted distinctive strings → `Could not create kill event for command processor!` + `Could not create character event for command processor!`.
- Grepped pre-release src for those strings → match only in `attic/src/editor.cpp:546,550` (`TConsolePane::Initialize`). **Decisive: cls_0x5a4358 is TConsolePane, not TTextBar.**
- Dumped vtable of `0x5a4358` (32 slots). Slot 0 = `0x43e370` = TConsolePane::Initialize. Confirms TPane-subclass hierarchy.
- Confirmed inheritance: `cls_0x5a42d4` shares method `0x43da00` with cls_0x5a4358's base call → cls_0x5a42d4 = TTextPane. Constructor field layout (`text` ptr at +0x60, cursorx at +0x74, etc.) consistent.
- Extracted parent Initialize at `0x43da00` → `recon/discovered/cls_0x5a4358_TConsolePane_parent_TTextPane_Initialize_43da00.cpp`. Two 10,000-byte buffers initialized to `'{' + null` (color-escape markers). Confirms TTextPane.
- Found `texthealthbar` string xref → `meth_0x54cb00` (Ghidra-attributed to TCharacter, but the function body has only one xref to texthealthbar).
- Extracted `FUN_0054cb00` → `recon/discovered/cls_0x5a5320_TPlayScreen_or_TTextBar_DrawHealth_54cb00.cpp`. Exact constant-for-constant match with pre-release `TTextBar::DrawBackground` health block (155, 176, 16, 186, /4 increment).
- Cross-referenced B.r3 → `FUN_0054bf70` is string-anchored to `"text bar"`. Extracted to `recon/discovered/FUN_0054bf70_TTextBar_init.cpp` (1110 bytes decomp). Confirms real TTextBar lives in `0x54c000`..`0x54d800` range with 3 mosaic surfaces and ~1104-byte instance.
- Wrote this brief + `port_status/TTextBar.md` + safe renames in `agent_ui_textbar.txt`.

**Time on task:** ~25 min wall-clock. Ghidra project lock contention briefly at start; resolved (no actual concurrent locker).

**Pending for next wave:**

- Extract TTextBar leaf vtable address (constructor called from `FUN_0054bf70`) — **DONE by Wave-2A, see section below**
- Per-method extract + golden-path match for Print / Clear / SetHealthDisplay / SetLevels / ClearHealthDisplay / DrawBackground / Pulse / Animate — partial (Close done by Wave-2A; rest pending)
- Rename `cls_0x5a4358_likely_TTextBar.{cpp,h}` files (this is a file-rename, not text-rename — needs coordinator)
- Update `docs/ui/RECON_UI_COVERAGE.md` and `recon/docs/CLASS_MAPPING.md` to retract the cls_0x5a4358 ↔ TTextBar mapping
- Update `RETAIL_UI_RECOVERY_PLAN.md` line 104 to point at `0x54bf70` instead

---

## Wave-2A leaf-class identification (2026-05-16) — `cls_0x5a5560 = TTextBar` CONFIRMED

### What was done

Followed the Wave-1A protocol from B.r3 to find the actual leaf class:

1. **Located the call site** in `TPlayScreen::Initialize @ 0x47a660` for the text-bar init: disassembled around the `"Trouble initializing text bar"` string anchor (`0x5d7300`) and found `0x0047abf8: MOV ECX, 0x65c5d0; CALL 0x54bf70`. So the global instance is **`0x65c5d0`**.
2. **Searched for the vtable wire** via `RefsTo.java 0x65c5d0` — exactly **one** WRITE-mode reference: `00480725 (WRITE) in <none>`. That's a global-ctor stub address (Wave-1A's per-pane ctor pattern).
3. **Decompiled** `FUN_00480725` (force-created via `DecompileAddr.java`). Body:
   ```cpp
   DAT_0065c5d0 = &PTR_FUN_005a5560;     // <- vtable address
   _DAT_0065c5d8 = 0x196;                // rect width = 406
   _DAT_0065c5e8 = 0x196;
   _DAT_0065c5dc = 0xc6;                 // rect height = 198
   _DAT_0065c5ec = 0xc6;
   _DAT_0065c5e0 = 0xe;                  // padding/border = 14
   _DAT_0065c5f0 = 0xe;
   ```
   406×198 — consistent with a wide transparent overlay band, **not** the pre-release single-line 80-char strip.
4. **Dumped vtable @ 0x5a5560** (35 slots) — exactly **31 used slots, leaf-TPane fingerprint** (slot 2 = `0x491bd0`, slot 9 = `0x491a80`, slot 16 = `0x491bb0`; matches TBottomPane/TSidePane/TPlyrStatusBar from Wave-1A). Overrides cluster in the documented `0x54c000..0x54d800` range (slots 7=0x54c600, 13=0x54c9c0, 19=0x54c460, 20=0x54c440, 23=0x54c780, 28=0x54d4a0). No method-count explosion, no mixed-purpose virtuals — **not Ghidra-merged**.
5. **Extracted slot 1** (`FUN_0054c3d0`) — recon/discovered/cls_0x5a5560_TTextBar_Close_54c3d0.cpp. Body:
   ```cpp
   FUN_004830f0(this+0x6c);              // free() text buffer
   if (this+0x84) (**(*this+0x84))(1);  // destroy mosaic surface 1
   if (this+0x88) (**(*this+0x88))(1);  // destroy mosaic surface 2
   if (this+0x8c) (**(*this+0x8c))(1);  // destroy mosaic surface 3
   FUN_00491970();                       // TPane::Close (base)
   ```
   This is **TTextBar::Close** — it tears down exactly the structure that `FUN_0054bf70` (Initialize) builds up (text buffer at +0x6c, three mosaic surfaces at +0x84/+0x88/+0x8c). The structural symmetry is the decisive confirmation.

### Results

| Symbol | Address | Role |
|---|---|---|
| `cls_0x5a5560` | `0x5a5560` (vtable) | **TTextBar** (the real retail leaf class) |
| `0x65c5d0` | (data, global) | TTextBar global instance (singleton) |
| `0x00480725` | (code, global ctor stub) | wires `[0x65c5d0] = 0x5a5560` and inits rect |
| `0x54bf70` | (vtable slot 0) | `TTextBar::Initialize` |
| `0x54c3d0` | (vtable slot 1) | `TTextBar::Close` |

### Evidence summary (4 lines, well over 98%)

1. Retail string `"Trouble initializing text bar"` at the call site — `[B.r3 anchor + B.r4 inheritance]`.
2. Vtable wire via the unique WRITE-mode reference to the global at `0x00480725` (.CRT$ region pattern).
3. Vtable @ `0x5a5560` has 31 slots with the leaf-TPane fingerprint shared with the four Wave-1A panes — not merged.
4. Slot-1 body at `0x54c3d0` (Close) frees exactly the 3 mosaic surfaces + text buffer that Initialize allocates — structural reciprocity is a structural fingerprint, not a coincidence.

### Renames added in `agent_ui_wave2_textbar_bottom.txt`

- Class label: `cls_0x5a5560 → cls_0x5a5560_TTextBar`
- Slot 0 (Initialize): `FUN_0054bf70` → `FUN_0054bf70_TTextBar_Initialize` (replaces Wave-1C's `_init` placeholder)
- Slot 1 (Close): `FUN_0054c3d0` → `FUN_0054c3d0_TTextBar_Close`
- Plus the `meth_`/`virt_meth_` variants of both.

### What's left for the next wave

- Identify slots 7/13/19/20/23/28 by extracting their bodies and matching against pre-release `Print` / `Clear` / `Pulse` / `Animate` / `DrawBackground` / `SetHealthDisplay` / `SetLevels` / `ClearHealthDisplay` — they should all sit in this set since B.r4 already confirmed `FUN_0054cb00` (DrawHealth inner block, exact constant match) and `FUN_0054cd40` (large, likely DrawBackground outer) are TTextBar methods. Most of the work is now identification by signature.
- Update the recon-classes file rename (`cls_0x5a4358_likely_TTextBar.{cpp,h}` → `cls_0x5a4358_TConsolePane.{cpp,h}`) — still coordinator's job.

