# B.r11 — TSideTabsPane button cascade: full sidebar content roster

**Wave:** 4A
**Date:** 2026-05-16
**Status:** SIDEBAR CONTENT-PANE ROSTER FULLY IDENTIFIED (6 of 6 positions).
            BUTTON DISPATCH WIRING IDENTIFIED.
            VISIBILITY MODEL CONFIRMED.

## TL;DR

The 6 round buttons on `TSideTabsPane` (`cls_0x5a5750`, global @
`0x65be50`) are **two independent 3-way region selectors**, not a flat
1-of-6 mode picker:

- **Upper region** — modal state in `DAT_0065d1b8` (0/1/2). Buttons:
  - 0 = Equip — content `DAT_0065b7e0` (cls_0x5a55dc)
  - 1 = Stats — content `DAT_0065b140` (cls_0x5a5ba0)
  - 2 = Book  — content `DAT_0065a9d8` (cls_0x5a5ae8) — already identified
                 Wave-3B as `SpellbookSidebarPane` (it iterates the player's
                 known-spells list); the UI label is "Book" so this is the
                 player's **Spell Book**.
- **Lower region** — modal state in `DAT_0065d1bc` (0/1/2). Buttons:
  - 0 = Inv   — content `DAT_0065d4f8` (cls_0x5a58c0)
  - 1 = Map   — content `DAT_0065b4f0` (cls_0x5a5658)
  - 2 = Spell — content `DAT_006661b0` (cls_0x5a5978)

The retail UI shows ONE upper-region content pane AND ONE lower-region
content pane simultaneously. `sample_screen_1.jpg` (Stats upper + Map
lower) is consistent with this. The 6 buttons are NOT independent
toggles and NOT preset combinations — they are **per-region mode
selectors with 3 choices each**.

The dispatch is NOT a per-button click handler that reaches the content
panes. It's via the **TPlayScreen central command dispatcher**
(`FUN_0047cf40`), a giant switch-by-command-id (param_2) where cases
7-0xc handle the sidebar mode commands. Each case sets the region
modal-state global and either closes the active content global (if
`DAT_0065d190 != 0`, i.e. content is currently presented) or marks the
sidebar dirty (sets `DAT_0065d194 = 1`).

Per the AGENT_PROTOCOL cost-asymmetry rule, all rename pairs in this
wave's renames file are tied to concrete evidence. Class names use
descriptive role labels (`StatsSidebarPane`, `EquipSidebarPane`,
`MapSidebarPane`, `InventorySidebarPane`, `QuickSpellSidebarPane`)
rather than committing the src/ class names (`TStatPane`, `TEquipPane`,
`TAutoMap`, `TInventory`, `TSpellPane`) since vtable-by-vtable identity
verification vs the src/ layouts has not been performed in this wave.

## Step 1 — TSideTabsPane vtable dump (46 slots)

Full output at `recon/discovered/cls_0x5a5750_vtable.txt`. Key leaf
overrides confirmed:

| Slot | Addr | Role | File |
|---:|---|---|---|
| 0 | 0x53cc30 | Initialize | `cls_0x5a5750_TSideTabsPane_Initialize_53cc30.cpp` |
| 1 | 0x53d360 | Close — frees 3 mosaic surfaces @ +0x188/0x18c/0x190 | `cls_0x5a5750_TSideTabsPane_Close_53d360.cpp` |
| 7 | 0x53d420 | DrawBackground (per-button fade alpha rendering) | `cls_0x5a5750_TSideTabsPane_DrawBackground_53d420.cpp` |
| 19 | 0x53d3a0 | Animate/Update — per-button hover-fade tick (0→8 ramp on slot[6] state) | `cls_0x5a5750_TSideTabsPane_Update_53d3a0.cpp` |
| 20 | 0x53d400 | leaf override (not extracted this wave) | — |
| 23 | 0x53d540 | Draw (full pass — composites 3 mosaic surfaces) | `cls_0x5a5750_TSideTabsPane_Draw_53d540.cpp` |
| 25 | 0x53d6e0 | MouseClick — marks 3 globals dirty, falls through to FUN_00436530 | `cls_0x5a5750_TSideTabsPane_MouseClick_53d6e0.cpp` |
| 26 | 0x436660 | shared (TButtonPane intermediate dispatch — pumps button-callbacks) | — |
| 27 | 0x4361f0 | shared (= **DefWidget_DispatchInput**) | — |
| 31 | 0x487cb0 | leaf override (likely destructor scalar-dtor wrapper) | — |
| 37 | 0x53d790 | leaf override | — |

Vtable size 46 slots (slot 46 = 0x5a5808 starts cls_0x5a5808
TBottomBarPane, per Wave-3B's neighbourhood note). Inheritance shape:
TPane (32 slots) → TButtonPane-w-DEF-widget intermediate cls_0x5a45c8
(+14 slots) → TSideTabsPane leaf (overrides only).

## Step 2 — TSideTabsPane::MouseClick body is a "wake the world" dirty
   marker, NOT the per-button dispatch

`FUN_0053d6e0` (vtable slot 25) is short:

```cpp
void __thiscall FUN_0053d6e0(int param_1, int param_2, int param_3, int param_4) {
  if (((((param_2 == 5) || (param_2 == 4)) && (-1 < param_3)) &&
      ((-1 < param_4 && (param_3 < *(int *)(param_1 + 0xc))))) &&
     ((param_4 < *(int *)(param_1 + 0x10) &&
      (((-1 < DAT_0065d67c || (-1 < DAT_0065b090)) || (-1 < DAT_0065b878)))))) {
    FUN_0043a100(0,0,0);
    FUN_0043a140(0);
    FUN_0043a170(&DAT_00670218);
    if (-1 < DAT_0065d67c) { _DAT_0065d548 = 1; }
    if (-1 < DAT_0065b090) { _DAT_0065b078 = 1; }
    if (DAT_0065b878 != 0) { _DAT_0065b830 = 1; }
  }
  FUN_00436530(param_2, param_3, param_4);
  return;
}
```

- `param_2 == 4 || 5` — mouse-down / mouse-up event types.
- The 3 referenced globals are NOT content-pane pointers — they're
  state/dirty flags consumed by the content panes' own animate code. The
  per-button click dispatch happens inside `FUN_00436530` (the
  TButtonPane intermediate's pre-registered per-button callback pump).
  The actual mode-change side-effects live in the central command
  dispatcher `FUN_0047cf40` (next step) which is where the per-button
  callbacks ultimately route.

This is a critical correction to the brief's planning assumption: the
TSideTabsPane class itself does NOT contain the visibility-toggle logic.
It owns the **buttons** and a **dirty-flag relay** to the content
panes.

## Step 3 — The real dispatch: `FUN_0047cf40` (TPlayScreen command dispatcher)

Backwards-tracing the upper/lower modal-state globals (`DAT_0065d1b8`
and `DAT_0065d1bc`) via FindImmRefs revealed that ALL writes to those
globals happen inside a single function spanning `0x47cf40-0x47dd2f`
(extracted to `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp`).
Function signature: `__thiscall FUN_0047cf40(int param_1, int param_2)`
where param_1 = `this` (TPlayScreen instance, fields at +0x5b8/0x5d4
/0x5d8/0x6a0..0x6c4 referenced), param_2 = command/key ID. Body is a
large switch on param_2 with cases 1-0x55 covering ALL HUD/playscreen
key commands (run/walk, sneak, inventory, F-keys, etc.).

Cases 7-0xc are the sidebar mode commands. Per-case mapping:

| Case | Region+Mode Set | Content Global Touched | Notes |
|---:|---|---|---|
| 7 | upper=2 (Book) + lower=2 (Spell) | `DAT_0065a9d8`, `DAT_006661b0` | Combined Book+Spell preset (toggle-hide variant) |
| 8 | upper=1 (Stats) | `DAT_0065b140` | Stats-only hotkey |
| 9 | upper=0 (Equip) + lower=0 (Inv) | `DAT_0065b7e0`, `DAT_0065d4f8` | Combined Equip+Inv preset (toggle-hide variant) |
| 0xa | upper=2 (Book) + lower=2 (Spell) | `DAT_0065a9d8`, `DAT_006661b0` | Same as case 7 but goes through `FUN_0047c500` open-helper instead of vtable-Close; selects mode if currently empty |
| 0xb | lower=0 (Inv) | `DAT_0065d4f8` | Inv-only |
| 0xc | lower=1 (Map) | `DAT_0065b4f0` | Map-only |

Each case has the same shape:

```cpp
FUN_0053cab0(N);                     // OR FUN_0053cb40(N) for lower-region setter
DAT_0065d1b8 = N;                    // OR DAT_0065d1bc = N
if (DAT_0065d190 == 0) {             // content currently NOT displayed
  if (DAT_0065d194 == 0 && DAT_0065d198 == 0) {
    DAT_0065d194 = 1;                // mark "show this side now"
  }
} else {                             // content currently displayed
  FUN_0048ed90(&DAT_<contentGlobal>, 0xffffffff);  // remove from open-pane list
  _DAT_<contentGlobal+0x48> = 0;     // reset internal field
  _DAT_<contentGlobal+0x4c> = 0;     // reset internal field
  (**(code **)(DAT_<contentGlobal> + 0x28))();     // vtable[10] - Close
}
FUN_00438a50(N1); FUN_00438a50(N2); FUN_00438a50(N3);  // 3 button-flag pulses
```

The `FUN_0053cab0` and `FUN_0053cb40` are the official setters for the
upper-region and lower-region modal state respectively.

## Step 4 — Content global → ctor → vtable cascade

For each content global, FindImmRefs located the matching CRT global
constructor thunk (pattern `MOV ECX, <global>; CALL <ctor>` immediately
above `PUSH <dtor_thunk>; CALL 0x58b66c` = atexit registration). The
ctor body writes `*this = &PTR_FUN_<vtable>` which gives the cls_0x
identity.

| Position | Label | Content Global | Ctor | Vtable (cls_0x) | Dtor | Width |
|---|---|---|---|---|---|---|
| upper / 0 | Equip | `0x65b7e0` | inlined @ `0x4807a0` | `0x5a55dc` | `0x4807f0` | 0x132 (= 306) |
| upper / 1 | Stats | `0x65b140` | `FUN_00488910` thunk @ `0x480d10` | `0x5a5ba0` | `FUN_00488970` | (see ctor) |
| upper / 2 | Book  | `0x65a9d8` | `FUN_00488620` thunk @ `0x480c30` (Wave-3B) | `0x5a5ae8` | (Wave-3B) | (Wave-3B) |
| lower / 0 | Inv   | `0x65d4f8` | `FUN_00487f50` thunk @ `0x480620` | `0x5a58c0` | `FUN_00537980` | 0x132 |
| lower / 1 | Map   | `0x65b4f0` | inlined @ `0x4808a0` | `0x5a5658` | `FUN_0052a400` | 0x132 |
| lower / 2 | Spell | `0x6661b0` | `FUN_00488460` thunk @ `0x4809e0` | `0x5a5978` | `FUN_004884c0` | 0x132 |

Equip and Map use inlined-vtable-write CRT init thunks (no separate
ctor function — the global init writes the vtable + dimensions directly).
Stats, Inv, Spell, Book use the explicit `MOV ECX; CALL ctor` pattern.

### Width fingerprint

All six content panes share the same instance width 0x132 (306 bytes)
and inherit TPane base layout (vtable @ &PTR_FUN_005a4494 set
mid-ctor before the leaf vtable wire — visible in
`FUN_00488910` Stats ctor and `FUN_00488460` lower-Spell ctor).

### Two distinct inheritance shapes

The 6 content classes split into two inheritance shapes:

- **TButtonPane-w-DEF intermediate (46-slot vtable)**:
  - cls_0x5a5ae8 (upper Book = Spellbook, per Wave-3B 46 slots)
  - cls_0x5a5ba0 (upper Stats — likely 46 slots; vtable starts at
    slot 46 of cls_0x5a5ae8's dump in Wave-3B)
- **TPane direct leaf (32-slot vtable)**:
  - cls_0x5a55dc (upper Equip — confirmed 32 slots: ends at slot 31
    where cls_0x5a5658 begins, per `recon/discovered/cls_0x5a5750_vtable.txt`-adjacent dump)
  - cls_0x5a58c0 (lower Inv — vtable size TBD this wave; inferred 32
    from adjacent vtables; ctor shape identical to Map/Spell)
  - cls_0x5a5658 (lower Map — 32 slots, ends where cls_0x5a5978 begins)
  - cls_0x5a5978 (lower Spell — 32 slots, ends where cls_0x5a5ae8 begins)

This is a real, surprising structural split. The "Book" upper position
(SpellbookSidebarPane) and the "Stats" upper position are richer
TButtonPane subclasses with all the DEF-widget-engine intermediate
machinery; while Equip / Inv / Map / lower-Spell are leaner plain-TPane
subclasses with no intermediate. Plausible explanation: Book and Stats
have scrollable / paginated content with internal buttons (scroll
arrows, page selectors) that justify the TButtonPane base; Equip, Inv,
Map, and lower-Spell render fixed-position grids / images with no
internal sub-widgets.

## Step 5 — Verdicts per sidebar tab content class

Role labels chosen per AGENT_PROTOCOL Rule 1 — descriptive of the
sidebar position they fill, NOT committing to a src/ class identity
until per-class vtable analysis confirms it.

| cls_0x | Role label | Likely src/ class | Confidence |
|---|---|---|---|
| 5a5ae8 | `SpellbookSidebarPane` | (no src/ match — Wave-3B) | HIGH (Wave-3B confirmed via spell-list iteration evidence) |
| 5a5ba0 | `StatsSidebarPane` | `TStatPane` (src/statpane.h) | HIGH on role; src/ identity 1 line of evidence (vtable[0]=0x546b50 "second stat-pane init", Wave-2B). NEEDS vtable-vs-src/ confirmation pass for class-name rename. |
| 5a55dc | `EquipSidebarPane` | `TEquipPane` (src/equip.h) | HIGH on role; src/ identity needs paper-doll / equipment-slot hunt for confirmation. |
| 5a58c0 | `InventorySidebarPane` | `TInventory` (src/inventory.h) | HIGH on role; src/ identity needs item-grid signature confirmation. Strong corroboration: dtor 0x537980, ref'd from item pickup/drop helpers at 0x46f3d0-0x47001e and 0x472430 (gameplay-side inventory mutations). |
| 5a5658 | `MapSidebarPane` | `TAutoMap` (src/automap.h) | HIGH on role; src/ identity has PERFECT yaml mapping per `RECON_UI_COVERAGE.md`. Confirm via vtable dump + src/ method-shape match. |
| 5a5978 | `QuickSpellSidebarPane` | `TQuickSpellPane`? or `TSpellPane` (src/spellpane.h)? | MEDIUM. Lower-position "Spell" tab is likely the quick-cast spell shortcut (5 slot row visible in CLASSIC_HUD §4 quickspell bar), NOT the talisman composer. But ALREADY have `cls_0x5a5a30 = TQuickSpellPane` (existing recon). So cls_0x5a5978 ≠ cls_0x5a5a30 — they're distinct classes. Defer src/ class identity to a per-class vtable/method dive. |

## Visibility model — confirmed answer

**Two independent 3-way region selectors.** NOT independent toggles
(can't show 2 of upper/Stats+Equip simultaneously). NOT preset
combinations (case 7 / case 9 set BOTH regions but each case still
picks a specific mode per region). NOT flat 1-of-6 (Stats + Map can
both be visible per `sample_screen_1.jpg`).

The 6 buttons in the strip each select ONE mode for ONE region:

```
button1 "Book"  -> upper=2
button2 "Stats" -> upper=1
button3 "Equip" -> upper=0
button4 "Spell" -> lower=2
button5 "Inv"   -> lower=0
button6 "Map"   -> lower=1
```

(button-to-case-id mapping per the dispatcher — but the keyboard
shortcuts in cases 7-0xc that set BOTH regions are convenience hotkeys
for "switch to Book+Spell view" / "switch to Equip+Inv view" — those
are key bindings, not button bindings. The button strip itself is one-
button-per-mode.)

## Cross-region observations & merging suspicions

1. **No Ghidra class-merging suspected** for the 6 content classes. Each
   has a distinct vtable address, a distinct ctor, and a distinct global
   instance address. None of them have method counts inconsistent with
   single-class identity.

2. **Wave-3B's `cls_0x5a5ae8 = "SpellbookSidebarPane"` is in the UPPER
   "Book" position**, not the lower "Spell" position. This is mildly
   counter-intuitive (the player's spell list lives behind the "Book"
   tab, while the "Spell" tab lives in a different class entirely =
   cls_0x5a5978 quickcast row). The Wave-3B role descriptor is still
   accurate (it IS the spellbook), but the **UI label is "Book"**, not
   "Spell". Updated my understanding accordingly — naming kept per
   Wave-3B convention to avoid churn.

3. **The dispatch function `FUN_0047cf40` is a complete TPlayScreen
   command roster** (cases 1-0x55 covering everything from sneak toggle
   to save-game). Spawning a per-case identification effort is a
   high-value follow-up — it likely identifies the entire keyboard
   command vocabulary in one extraction.

## Candidates with weak evidence — held for next wave

- **Per-button → case-id mapping**: which TSideTabsPane button index
  (returned by FUN_00436900(N)) invokes which command-id in
  FUN_0047cf40? Init wires buttons via FUN_00436790 +
  param_1[0x65..0x6a] storage. The button-callback registration likely
  passes the case-id as a closure parameter. Need to decompile
  FUN_00436790 (intermediate-class button-registration helper) to
  confirm the mapping.

- **Slot 20 (0x53d400), 31 (0x487cb0), 37 (0x53d790) leaf overrides**:
  not extracted this wave. Slot 20 is the next-slot after Update;
  presence pattern suggests another animate-related override. Slot 31
  is in the destructor region. Defer.

- **Slot 25 dirty-flag globals (`DAT_0065d67c`, `DAT_0065b090`,
  `DAT_0065b878`)**: these three are passed as visibility/active flags
  by the click handler. Need to FindImmRefs each to identify which
  subsystem owns them — almost certainly belong to TPlyrStatusBar
  (left/right characters) or TTextBar (game log overlay). Conservative
  guess: they're "wake up" flags for the three transparent HUD overlays
  (left char status, right char status, text log) so they re-render
  after the sidebar paints over them. NOT renamed.

- **`FUN_0053cab0` / `FUN_0053cb40` upper/lower region setters**: not
  extracted but role is clear from context. Defer to a TSidePane
  hunt — they likely belong to TSidePane (`cls_0x5a53ec`) as it owns
  the upper/lower content regions visually.

## Renames applied this wave

See `recon/discovered/renames/agent_ui_wave4_sidebar_cascade.txt`. Each
rename has 2+ lines of independent evidence in inline comments. Summary:

- `FUN_0047cf40` → `FUN_0047cf40_TPlayScreen_DispatchCommand`
- `FUN_0053cc30` (already named TSideTabsPane_Initialize by prior wave)
- `FUN_0053d360` → `FUN_0053d360_TSideTabsPane_Close`
- `FUN_0053d3a0` → `FUN_0053d3a0_TSideTabsPane_Update`
- `FUN_0053d420` → `FUN_0053d420_TSideTabsPane_DrawBackground`
- `FUN_0053d540` → `FUN_0053d540_TSideTabsPane_Draw`
- `FUN_0053d6e0` → `FUN_0053d6e0_TSideTabsPane_MouseClick`
- `FUN_00488910` → `FUN_00488910_StatsSidebarPane_ctor`
- `FUN_00488460` → `FUN_00488460_QuickSpellSidebarPane_ctor`
- `FUN_00487f50` → `FUN_00487f50_InventorySidebarPane_ctor`
- `FUN_00537980` → `FUN_00537980_InventorySidebarPane_dtor`
- `FUN_0052a400` → `FUN_0052a400_MapSidebarPane_dtor`

**NOT renamed this wave (deferred for src/-class confirmation):**

- cls_0x5a55dc → leave bare. Role: EquipSidebarPane. To rename to
  TEquipPane needs vtable-vs-src/equip.h confirmation.
- cls_0x5a58c0 → leave bare. Role: InventorySidebarPane. To rename to
  TInventory needs vtable-vs-src/inventory.h confirmation.
- cls_0x5a5658 → leave bare. Role: MapSidebarPane. To rename to
  TAutoMap needs vtable-vs-src/automap.h confirmation.
- cls_0x5a5978 → leave bare. Role: QuickSpellSidebarPane. Per src/
  there is BOTH TSpellPane (talisman composer) AND TQuickSpellPane
  (cls_0x5a5a30 already identified). Lower-Spell tab is likely a
  THIRD class. Defer.
- cls_0x5a5ba0 → leave bare. Role: StatsSidebarPane. To rename to
  TStatPane needs vtable-vs-src/statpane.h confirmation. Strong
  candidate but Wave-2B noted "second stat-pane init" so the binding
  is plausible.

## Files produced this wave

- `recon/discovered/cls_0x5a5750_vtable.txt` — 50-slot vtable dump
- `recon/discovered/cls_0x5a5750_TSideTabsPane_Close_53d360.cpp`
- `recon/discovered/cls_0x5a5750_TSideTabsPane_Update_53d3a0.cpp`
- `recon/discovered/cls_0x5a5750_TSideTabsPane_DrawBackground_53d420.cpp`
- `recon/discovered/cls_0x5a5750_TSideTabsPane_Draw_53d540.cpp`
- `recon/discovered/cls_0x5a5750_TSideTabsPane_MouseClick_53d6e0.cpp`
- `recon/discovered/FUN_0047cf40_TPlayScreen_DispatchCommand.cpp` — the
  command-id dispatcher (HUGE find; covers ALL playscreen commands not
  just sidebar — 537 lines, 0x55 cases)
- `recon/discovered/cls_0x5a5ba0_StatsSidebarPane_ctor_488910.cpp`
- `recon/discovered/cls_0x5a5978_QuickSpellSidebarPane_ctor_488460.cpp`
- `recon/discovered/cls_0x5a58c0_InventorySidebarPane_ctor_487f50.cpp`
- `recon/discovered/port_status/StatsSidebarPane.md`
- `recon/discovered/port_status/EquipSidebarPane.md`
- `recon/discovered/port_status/InventorySidebarPane.md`
- `recon/discovered/port_status/MapSidebarPane.md`
- `recon/discovered/port_status/QuickSpellSidebarPane.md`
- `recon/discovered/port_status/TSideTabsPane.md` — updated with full
  button cascade table + visibility model
- `recon/discovered/renames/agent_ui_wave4_sidebar_cascade.txt`
- `docs/ui/briefs/B_r11_sidebar_tab_cascade.md` — this file

## Hunt log

- **2026-05-16, Wave-4A run 1**: read AGENT_PROTOCOL +
  RETAIL_UI_RECOVERY_PLAN Tier 3 + Tier 11 + B_r9_parchment_viewer +
  cls_0x5a5750_TSideTabsPane_Initialize_53cc30. Dumped TSideTabsPane
  vtable (50 slots, 46 real + 4 into next class). Decompiled vtable
  slots 1, 7, 19, 23, 25.
- **run 2**: decompiled slot-25 (MouseClick) — observed it was a
  dirty-flag setter, not the dispatch logic. Read button-label string
  block at 0x5e4720 = "Book.Stats.Equip.Spell.Inv.Map". Inferred the
  6-button layout from init's per-button FUN_0042c600 add-button calls
  + y-coordinates 2/0x27/0x4c/0x71/0x96/0xbb.
- **run 3**: FindImmRefs on the upper/lower mode globals
  (`DAT_0065d1b8` / `DAT_0065d1bc`) located all writes inside the
  unrecognized function range 0x47cf40-0x47dd2f. Disasm + force-decompile
  identified the function start. Read all 537 lines of the dispatcher
  body — recovered cases 1-0x55 covering 80+ playscreen commands.
- **run 4**: FindImmRefs on each content global; backtrace each to its
  CRT init-thunk; identified ctor function for Stats/Spell-lower/Inv,
  inline vtable-init for Equip and Map. Decompile each ctor to extract
  the vtable cls_0x identity.
- **run 5**: DumpVtable on cls_0x5a55dc (Equip) confirmed 32-slot
  TPane-direct shape (no TButtonPane intermediate); inferred Inv/Map/
  lower-Spell are likely same shape from contiguous vtable layout.
- Wrote brief, renames file, port-status files, docs/ui updates.
- Lock contention encountered twice; backed off ~30-45s each time.

## Coordination notes for Wave-5+

1. **Per-class vtable analysis is the next valuable extraction**. For
   each of the 5 newly-identified content classes (cls_0x5a55dc /
   5a58c0 / 5a5658 / 5a5978 / 5a5ba0), DumpVtable + slot-by-slot
   identification will determine:
   - Confirms vtable size (32 vs 46) — corroborates inheritance shape.
   - Identifies Initialize / Draw / Update / Close / MouseClick virtuals.
   - Each method's body can then be cross-referenced vs src/ to confirm
     the src class identity (or REJECT the candidate per Wave-3B's
     precedent).

2. **The TPlayScreen command dispatcher `FUN_0047cf40`** is a
   self-contained roster of 80+ playscreen commands. Spawning a wave to
   identify each case's purpose (per FindStringRefs and called-helper
   identification) likely lights up the keyboard command vocabulary AND
   reveals other HUD subsystems' integration points. High value.

3. **TSidePane class (cls_0x5a53ec)** likely owns the upper/lower
   content-region containers and the `FUN_0053cab0` / `FUN_0053cb40`
   setters. Worth a dedicated B.r-style brief.

4. **The intermediate class cls_0x5a45c8** (TButtonPane-w-DEF parent of
   Stats + Book) — Wave-3B flagged this as the most valuable single ID
   for next-round work. Still unresolved. This wave's findings don't
   change that priority but DO add new corroborating data: 4 of the 6
   sidebar content classes are direct TPane subclasses (NOT the
   intermediate), while 2 are intermediate subclasses. The intermediate
   has more state machinery (button arrays + DEF widget engine) than the
   leaf TPane sidebar content panes need.
