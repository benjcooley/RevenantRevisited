# B.r6 — Target-side character panel, stat-pane disambiguation, TPane base ID

**Wave:** 2B
**Date:** 2026-05-16
**Status:** PARTIAL-WITH-DECISIVE-FINDINGS — TPane base CONFIRMED; the two "stat pane" inits DISAMBIGUATED as two distinct classes; the right-side target TPlyrStatusBar global instance hypothesis FALSIFIED (no such instance exists in the binary's static data).

## TL;DR

1. **`cls_0x5a4494` is confirmed TPane base.** 31 distinct base-vtable wire sites across the binary; 4 already-identified leaf panes (TPlyrStatusBar, TSidePane, TSideTabsPane, TBottomPane) inherit 25+ of their 31 vtable slots verbatim from this base; slot-0 / slot-1 method bodies match `src/screen.h` TPane::Initialize / TPane::Close semantics exactly. Apply: `cls_0x5a4494 -> cls_0x5a4494_TPane`. See [renames file](../../../recon/discovered/renames/agent_ui_wave2_charpane_base.txt).

2. **The two "stat pane" inits are TWO DIFFERENT CLASSES** (not two instances of one class). FUN_00546b50 (instance @ 0x65b140) loads `statpane.def` + "RingT" portrait sprite — this is the genuine sidebar **TStatPane**. FUN_005449e0 (instance @ 0x65a9d8) loads ScrlTop/ScrlBottom/ScrollPaper/Scroll graphics + creates scroll up/down buttons — this is a **scrollable parchment viewer** (probable TScrollPane / TPaperScrollPane / TBookPane), NOT TStatPane. The shared error string `"Trouble initializing stat pane"` is misleading.

3. **No second global instance of TPlyrStatusBar (cls_0x5a54e4) exists in the binary.** `FindBytes 0x5a54e4` returns ONE hit at 0x4806a2 (Wave-1A's known wire for the player instance @ 0x65a8c0). The static initializer region is the canonical place to wire global instance vtables in this codebase (see Wave-1A's 4 confirmed sibling panes all wired the same way: `MOV [<GLOBAL>], <vtable>; RET` stubs in `0x480420..0x4806a6`). There is no second such stub for 0x5a54e4 anywhere in `.text` or `.data`. The "one class, two global instances" framing for the player+target panel is FALSIFIED at the global-instance level.

## Step 1 — Right-side target TPlyrStatusBar instance hunt: NULL RESULT

### Method

```sh
FindBytes 0x5a54e4   # 4-byte LE search across all loaded memory sections
```

### Result

```
hit @ 004806a2
total: 1
```

Cross-reference: `0x4806a2` is at the trailing-byte position of Wave-1A's known wire:

```
0048069c MOV dword ptr [0x0065a8c0], 0x5a54e4
004806a6 RET
```

That's the SAME instruction. No other hits in the binary. The FindBytes search covers all of `.text` + `.data` + any other section that holds initialized memory, so this is conclusive: there is no second `MOV [<GLOBAL>], 0x5a54e4` stub and no `.data` slot pre-initialized to hold this vtable.

### Implications (per HUD reference §1.2)

The CLASSIC_HUD_REFERENCE document hypothesizes "Same panel class as §1 (mirrored right-aligned)... two instances of TPlyrStatusBar bound to player vs target." That hypothesis is incompatible with the forensic evidence at the global-instance level. Three alternatives remain:

**Alternative A — single instance, two-pass draw.** TPlyrStatusBar's per-frame Draw/Animate method is called twice per frame: first with `source = player`, then with `source = current_target`, drawing to two different screen rectangles. The instance is stateless w.r.t. the bound character — `Animate` reads a transient pointer parameter or a member that's swapped before each call. **Evaluation: PLAUSIBLE.** The 31-slot vtable + 0xe0-byte object size is consistent with carrying only render state, not character binding. This would also explain why TPlyrStatusBar's Init takes no character pointer.

**Alternative B — sibling class for the target panel.** A distinct class (e.g. `TTargetStatusBar`) with its own vtable + global. **Evaluation: POSSIBLE but no candidate identified.** None of the Wave-1A pane-init candidates have a string anchor that ties to the right panel specifically. If this alternative is correct, the class is one of the still-unidentified globals constructed by TPlayScreen's static initializer chain (need a broader sweep of `MOV [<global>], <vtable>` patterns in `0x480400..0x480740`).

**Alternative C — target panel is heap-allocated (no global).** Constructed lazily when the player first acquires a target; freed when target cleared. **Evaluation: LESS LIKELY given Revenant's static-allocation style** (every other HUD pane is a global), but not impossible. If correct, the ctor would be called from somewhere in the target-acquire path (probably TPlayer::SetTarget or similar). No evidence either way yet.

**Recommended next investigation (Wave-3):**
1. Extract TPlyrStatusBar's draw/animate method (vtable slot 19 or 20 — addresses 0x549da0 / 0x549e60) and check whether it takes a `source character*` parameter OR reads an instance member that's mutable per-call.
2. If single-instance + two-pass: trace what code calls TPlyrStatusBar::Animate or its sibling. If it's called twice per frame from TPlayScreen, the question is answered.
3. If the per-frame path only calls TPlyrStatusBar once, hunt for a sibling class via the static init region + look for any class that allocates a 0x80×0x40 mosaic (TPlyrStatusBar's three-bar fingerprint).

## Step 2 — Two "stat pane" inits disambiguated: TWO DIFFERENT CLASSES

### FUN_00546b50 = 1st "stat pane" init — likely TStatPane

**Call site context** (`TPlayScreen::Initialize` @ 0x47ace8):
```
MOV ECX, 0x65b140       ; this = global @ 0x65b140
CALL 0x00546b50         ; FUN_00546b50(this)
TEST EAX, EAX
JNZ ...                  ; on failure: "Trouble initializing stat pane" (string @ 0x5d73a8)
```

**Body fingerprint** (see [cls_unknown_StatPane_Initialize_546b50.cpp](../../../recon/discovered/cls_unknown_StatPane_Initialize_546b50.cpp)):
- First call: `FUN_00434e40()` — base-class init for `cls_0x5a45c8` (intermediate class, sets fields [0x18..0x5e] including ptr to global render context at [0x19])
- Allocates 1 mosaic surface (sized via param_1[3] x param_1[4] = pane width x height) at field [99]
- Allocates 1 sprite surface, loads sprite **"RingT"** (`s_RingT_005e539c`) — the Ring portrait icon
- Loads asset **"statpane.def"** (`s_statpane_def_005e53a4`) — the DEF (widget-engine definition) file
- On asset-load failure: `"Unable to load STATPANE DEF"` error
- Field [0x68] = pointer to loaded DEF buffer, [0x69] = its size
- Calls own vtable[0xb8/4 = 46] at the end with `DAT_00667fcc` — probably a "set bound character" call

**Identification:** This IS the sidebar **TStatPane** (the right-sidebar Stats tab content). Evidence chain:
- (a) Error string "stat pane" + (b) loads `statpane.def` + (c) loads ring portrait (the player's stats screen displays the character with their equipped ring; statpane.def likely lays out the stat bars + values).

**Not renaming the class yet** because we haven't yet extracted the vtable wire site for global 0x65b140 (the `MOV [0x65b140], <vtable>` pattern). Need: `FindImmRefs 0x65b140` scoped to look for stores rather than loads — or directly read `*(int*)0x65b140` from `.data`.

### FUN_005449e0 = 2nd "stat pane" init — NOT TStatPane, likely TScrollPane / TPaperScrollPane / TBookPane

**Call site context** (`TPlayScreen::Initialize` @ 0x47ad11):
```
MOV ECX, 0x65a9d8       ; this = global @ 0x65a9d8
CALL 0x005449e0         ; FUN_005449e0(this)
TEST EAX, EAX
JNZ ...                  ; on failure: "Trouble initializing stat pane" (string @ 0x5d73c8)
```

**Body fingerprint** (see [cls_0x5a5ae8_ScrollPane_Initialize_5449e0.cpp](../../../recon/discovered/cls_0x5a5ae8_ScrollPane_Initialize_5449e0.cpp)):
- First call: `FUN_00434e40()` — same intermediate-class init as FUN_00546b50 (so 2nd stat pane and TStatPane share the `cls_0x5a45c8` base)
- Allocates 6 sprite surfaces holding:
  - `"ScrlTop"` (sprite, alpha-blended, into field [0x184])
  - `"ScrlBottom"` (sprite, into field [0x188])
  - 0x94×0xe5 (148×229) plain surface into field [0x180] — main content area
  - `"ScrollPaper"` (sprite, into field [0x18c]) — the parchment-style background
  - 0x84×200 (132×200) plain surface into field [0x180]+0x5c (linked to main content as a sub-buffer)
- Loads sprite `"Scroll"` into field [400] (0x190)
- Creates 2 TButton instances at coords (0xa9, 0x96, 0x18, 0x18) and (0xa9, 0xae, 0x18, 0x18):
  - "ScrlArwUU"/"ScrlArwUD"/"ScrlArwUG" = scroll UP button (Unpressed / Pressed / Disabled states)
  - "ScrlArwDU"/"ScrlArwDD"/"ScrlArwDG" = scroll DOWN button (same)
- Buttons registered with `FUN_00436ae0` (likely AddButton to parent's button list)
- Sets fields [0x17c, 0x198, 0x19c, 0x1a0, 0x1c8, 0x1cc, 0x1d0] for scroll-state tracking

**Identification:** This is a **scrollable parchment-paper content viewer**. Strong candidates:
- **TScrollPane** — generic scrollable text/content pane (catch-all)
- **TPaperScrollPane** — parchment-scroll-themed UI (books, scrolls picked up in inventory)
- **TBookPane** — readable book overlay
- **TJournalPane** — quest journal / character notes

**Vtable wire** (see [cls_0x5a5ae8_ctor_488620.cpp](../../../recon/discovered/cls_0x5a5ae8_ctor_488620.cpp)): the constructor at FUN_00488620 does a **3-stage MSVC inheritance ctor**:
1. `*this = &PTR_FUN_005a4494` — base TPane vtable
2. `FUN_0041c7f0(0x10, 0x10)` — TPane base ctor call (sets x, y to passed values)
3. `*this = &PTR_FUN_005a45c8` — intermediate-class vtable (probably TButtonPane or similar mid-level)
4. allocates a 0x100-byte buffer at field [0x6a] (line item count = field [0x69] = 0x40 dwords)
5. `*this = &PTR_FUN_005a5ae8` — final derived vtable

So `cls_0x5a5ae8` is the leaf class for the 2nd "stat pane" global, inheriting **TPane → cls_0x5a45c8 → cls_0x5a5ae8**. Initial dimensions: x=y=0x10, width=0x1c4 (452), height=0xbc (188), some other field=0x132 (306). The 0x100-byte / 0x40-dword allocation at field [0x6a] (with a count field [0x69] = 0x40 short) is plausibly a **scrollable line buffer** (40 entries × 4 bytes = pointers per line, OR an offset table). Together with the scroll-up/down buttons + parchment graphics, this strongly suggests an in-game readable document of some kind.

**Not renaming the class yet** because the function-by-function role identification (TScrollPane vs TBookPane vs TJournalPane) needs more anchoring — the names `ScrollPaper` / `Scroll` are generic, and the class could be the base of multiple specialized scroll-content classes.

### Hypothesis for the duplicated error string

Both call sites in TPlayScreen::Initialize emit `"Trouble initializing stat pane"` even though they construct two different classes. Likely cause:
- The original code had two adjacent `Init(<x>); if (!ok) FATAL("Trouble initializing X pane");` patterns
- The second one was copy-pasted from the first and the developer forgot to change "stat pane" to the correct name
- This is a pre-existing source-level bug / typo, not a Ghidra artifact

This kind of error-message reuse is a known anti-pattern in code-by-copy-paste and is **NOT** evidence that the two classes share an identity.

## Step 3 — TPane base class CONFIRMED: cls_0x5a4494 = TPane

### Evidence A: vtable wire pattern at 31 distinct sites

`FindBytes 0x5a4494` returned **31 hits** in the binary. Sampling:

```
0044518b 0044545e 00445598 004699ae 0046a5ae 0046d047 0046d182 0046d277
0046d517 0046d5f2 00487c71 00487dd2 00487f88 00488495 004885b2 0048866d
00488942 00488d2d 00488f32 004890d7 00521d92 00521e82 00533727 00533ce7
00537447 00539fe2 0053a107 0053a372 0053bde2 0053c4e7 0053c5c7
```

These cluster heavily around `0x44...` (likely a default-vtable stub region), `0x46d...` (a button class family), `0x487c..0x489...` (the multi-stage ctor region — TSideTabsPane's ctor at 0x487c60 is one of these), and `0x521..0x53c...` (HUD pane ctors). This is the textbook signature of a widely-used base class.

### Evidence B: vtable-slot inheritance across confirmed leaf panes

Comparing the 4 Wave-1A confirmed leaf panes against cls_0x5a4494:

| Slot | cls_0x5a4494 (TPane) | TPlyrStatusBar | TSidePane | TSideTabsPane | TBottomPane |
|---:|:---:|:---:|:---:|:---:|:---:|
| 0 | 00491900 | 00549740* | 0053c8c0* | 0053cc30* | 0052d8a0* |
| 1 | 00491970 | 00549d40* | (override*) | (override*) | (override*) |
| 2 | 00491bd0 | 00491bd0 | 00491bd0 | 00435010* | 00491bd0 |
| 3 | 00444f40 | 00444f40 | 00444f40 | 00444f40 | 00444f40 |
| 4 | 00444f50 | 00444f50 | 00444f50 | 00444f50 | 00444f50 |
| 5 | 00444f60 | 00444f60 | 00444f60 | 00444f60 | 00444f60 |
| 6 | 00444f70 | 00444f70 | 00444f70 | 00444f70 | 00444f70 |
| 7 | 00444f80 | 0054ab80* | 00444f80 | 00444f80 | 00444f80 |
| 8 | 00444f90 | 00444f90 | 00444f90 | 00444f90 | 00444f90 |
| 9 | 00491a80 | 00491a80 | 00491a80 | 00491a80 | 00491a80 |
| 10..15 | 00444fb0..00445000 | (all match) | (all match) | (all match) | (all match) |
| 16 | 00491bb0 | 00491bb0 | 00491bb0 | 00491bb0 | 00491bb0 |
| 17, 18 | 00445020, 00445030 | (match) | (match) | (match) | (match) |
| 19 | 00445050 | 00549da0* | (match) | (match) | (match) |
| 20 | 004451d0 | 00549e60* | (match) | (match) | (match) |
| 21..30 | (base default bodies) | (mostly match, slot 23 + 27 override*) | (match) | (match) | (match) |

(`*` = leaf override.) Each leaf only overrides 0, 1, and a handful of others. Slot inheritance is decisive.

### Evidence C: slot 0 + slot 1 method body semantics match src TPane

**FUN_00491900 (slot 0 = Initialize)** — see [cls_0x5a4494_TPane_Initialize_491900.cpp](../../../recon/discovered/cls_0x5a4494_TPane_Initialize_491900.cpp):

```cpp
if (param_1[0x10] == 0) {                  // if (!is_open)
  param_1[1] = param_1[5];                  //   x = newx
  param_1[3] = param_1[7];                  //   width = newwidth
  param_1[2] = param_1[6];                  //   y = newy
  param_1[0xe] = 0;  param_1[0xc] = 0;      //   zero render state
  param_1[10] = 0;   param_1[0xd] = 0;      //
  param_1[0xb] = 0;  param_1[9] = 0;        //
  param_1[0x12] = 0; param_1[0x13] = 0;     //   zero scroll
  param_1[0x15] = 0;                        //
  param_1[0x16] = -1; param_1[0x17] = -1;   //   backgroundbuffer = -1
  param_1[4] = param_1[8];                  //   height = newheight
  param_1[0x14] = 1;                        //   dirty = 1
  (**(code **)(*param_1 + 0xc))();          //   this->CreateBackgroundBuffers()
  param_1[0x10] = 1;                        //   is_open = 1
}
return 1;
```

This is a near-line-for-line match to `src/screen.h::TPane::Initialize` as inferable from the class definition (which has `bool isopen` flag, `newx/newy/newwidth/newheight` paired with `x/y/width/height`, `dirty` flag, `backgroundbuffer = -1` default, and a `CreateBackgroundBuffers` virtual). The slot-3 call (`vtable[0xc/4 = 3]`) matches the slot-3 = `CreateBackgroundBuffers` ordering established below.

**FUN_00491970 (slot 1 = Close)** — see [cls_0x5a4494_TPane_Close_491970.cpp](../../../recon/discovered/cls_0x5a4494_TPane_Close_491970.cpp):

```cpp
(**(code **)(*param_1 + 0x10))();   // this->FreeBackgroundBuffers()  (slot 4)
param_1[0x10] = 0;                  // is_open = 0
return;
```

Matches `src/screen.h::TPane::Close` exactly.

### Vtable slot order — provisional mapping vs src/screen.h

Based on slot-0 / slot-1 body analysis + the call-into-vtable[3] = CreateBackgroundBuffers in Initialize:

| Slot | Body | Inferred name (src) | Confidence |
|---:|:---|:---|:---|
| 0 | 00491900 | `Initialize` | **HIGH** (body matches src semantics) |
| 1 | 00491970 | `Close` | **HIGH** (body matches src semantics) |
| 2 | 00491bd0 | likely `PaneResized` or `SetClipRect` (non-trivial, shared base method) | medium (need body extract) |
| 3 | 00444f40 | `CreateBackgroundBuffers` (called from slot 0; `{}` body fits src default) | **HIGH** |
| 4 | 00444f50 | `FreeBackgroundBuffers` (called from slot 1; `{}` body fits src default) | **HIGH** |
| 5 | 00444f60 | `SetBackgroundBuffer` (src: `{ backgroundbuffer = buf; }` — needs body check) | medium |
| 6 | 00444f70 | `ClearBackgroundBuffer` (src: `{ backgroundbuffer = -1; }` — needs body check) | medium |
| 7 | 00444f80 | maybe `SetClipRect`, `Update`, or `SetDirty` (TPlyrStatusBar overrides this!) | low |
| 8 | 00444f90 | next virtual in src order | low |
| 9 | 00491a80 | shared base method (non-trivial) — likely `Show` or `Hide` | medium |
| 10..15 | 00444fb0..00445000 | sequence of empty-body virtuals — `MouseClick`/`MouseMove`/`KeyPress`/`CharPress`/`Joystick` etc. | medium-low |
| 16 | 00491bb0 | shared non-trivial method (PaneToScreen / PutToScreen?) | low |
| 17..30 | mix | tail virtuals + maybe destructor/scalar-dtor cluster | low |

**Caveat — vtable layout vs src forward order.** MSVC vtables can re-order or insert thunks; the above mapping assumes leftmost slot = first virtual in declaration order. The slot 0 / 1 / 3 / 4 chain proves the FIRST FEW slots match src order, but later slots will need per-method body verification before naming. Don't promote the medium/low confidence mappings without that work.

## Coordination notes (for Wave-3+)

1. **TPlyrStatusBar's two-instance hypothesis needs a different angle.** Recommend extracting slot 19 (0x549da0) and slot 20 (0x549e60) of TPlyrStatusBar's vtable — these are the only TPlyrStatusBar-specific overrides in the "draw" slot range — to see if they take a character pointer parameter (suggesting Alternative A: single instance, two-pass draw with stat source swap).

2. **The "scroll pane" class (cls_0x5a5ae8 @ 0x65a9d8) deserves a focused identification pass.** Check whether it's used by the spell-scroll consumption UI, the in-game readable book/scroll UI, or the quest journal. Look for callers of the global @ 0x65a9d8 that pass it text content or book IDs. Notable observed callers: FUN_0047f0e0, FUN_00544fb0, FUN_00546390, FUN_00546620, FUN_0054d700.

3. **The intermediate class cls_0x5a45c8** is shared by both stat-panes AND TSideTabsPane uses base 0x5a4494 directly. So cls_0x5a45c8 is **not** the SideTabsPane base — it's a sibling intermediate (likely TButtonPane or a TPane-with-buttons mid-level). Worth a dedicated extraction: dump its vtable, identify its slot-0 override, see what class behaviors it adds beyond TPane.

4. **The "two adjacent identical error strings" pattern** is worth checking elsewhere in TPlayScreen::Initialize. If other pane-init pairs share their error strings, this could mean Wave-1A's pane-init table needs a "warning: identifying string may be misleading for this entry" annotation.

## Files produced this wave

Decompilations (under `recon/discovered/`):
- `cls_unknown_StatPane_Initialize_546b50.cpp` — 1st "stat pane" init (likely TStatPane)
- `cls_0x5a5ae8_ScrollPane_Initialize_5449e0.cpp` — 2nd "stat pane" init (scrollable parchment)
- `cls_0x5a5ae8_ctor_488620.cpp` — ctor for 2nd "stat pane" global (3-stage vtable wire pattern)
- `cls_0x5a4494_TPane_Initialize_491900.cpp` — TPane base Initialize body
- `cls_0x5a4494_TPane_Close_491970.cpp` — TPane base Close body
- `cls_0x5a4494_TPane_vtable.txt` — full 32-slot vtable dump for TPane base
- `cls_0x5a45c8_intermediate_init_434e40.cpp` — intermediate class init (shared by stat-pane class family)

Renames:
- `recon/discovered/renames/agent_ui_wave2_charpane_base.txt` — 3 pairs (cls_0x5a4494 -> TPane + 2 base method names).

Port-status updates:
- `recon/discovered/port_status/TPane.md` — created (this wave).
- `recon/discovered/port_status/TPlyrStatusBar.md` — appended (this wave; the "instances" section).
- `recon/discovered/port_status/TStatPane.md` — created (this wave; partial — class identity inferred, vtable not yet dumped).
- (NOT created) TScrollPane / TBookPane / etc. for the 2nd stat pane — class purpose not narrowed enough yet to commit a port-status name.

## Hunt log

- **2026-05-16, Wave-2B run 1:** ran FindBytes for 0x5a54e4 (TPlyrStatusBar vtable) — single hit at 0x4806a2 (= existing Wave-1A wire). No second global instance exists. Decompiled FUN_00546b50, FUN_005449e0, FUN_00488620 (ctor for 0x65a9d8), FUN_00491900 + FUN_00491970 + FUN_00434e40. Dumped vtable @ 0x5a4494 (32 slots) and ran FindBytes for 0x5a4494 (31 distinct base-vtable wire sites). All findings documented above. Applied 3 conservative renames + extensive deferred-rename notes.
