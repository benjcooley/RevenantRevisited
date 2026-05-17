# B.r2 — Character-Panel Identity Hunt

**Status:** indeterminate (first pass). TStatusBar in `recon/classes_readable/` is the closest named match but the visible structure is structurally inadequate — likely a Ghidra extraction failure rather than a wrong-class. Next forensic step = Ghidra CLI full extraction of `cls_0x579bb0`.

This brief captures the findings of the first sub-agent pass (an `Explore` agent — which can read but not write files; output transcribed here). Subsequent passes update this file in place.

## Target hunted

The retail class that builds the upper-left + upper-right character panels per [CLASSIC_HUD_REFERENCE.md](../CLASSIC_HUD_REFERENCE.md):
- Portrait + name + level + three horizontal bars (health red / mana purple / stamina yellow) + numeric values
- One class, two instances (left bound to Player, right bound to current-target — target is player-controlled via circular orbit lock)
- Right instance reacts live to target changes; appears/disappears with target acquisition

## Candidate: TStatusBar (cls_0x579bb0) — WEAK match

**Why this is the closest named candidate:**
- `recon/classes_readable/TStatusBar.{h,cpp}` — flagged in `RECON_UI_COVERAGE.md` as a candidate for the multi-bar combined panel (the "bar" in the name likely refers to the multi-bar grouping, not a single tube).
- Address: `cls_0x579bb0`.

**Why it fails the spec as visible:**
1. **Too small:** 328 bytes / 4 fields (`level`, `targetlevel`, `animating`, `mbr_0x144`) is insufficient for portrait reference + name string + three bar state triples + animation state. Expected minimum ~400 bytes for the full panel.
2. **No render methods recovered:** only 1 method (`meth_0x579bb0`, threading/mutex). No `Draw` / `DrawBackground` / `Update` / display hooks.
3. **No bitmap / name / portrait fields:** absent from the header.
4. **Decompilation almost certainly incomplete:** a real HUD class would have many more methods than 1. Recovery failure suspected.

**Class index summary for cls_0x579bb0:** `4 fields, 1 method, 52 lines`.

## Pane architecture evidence in TPlayScreen

From `recon/classes_converted/cls_0x5b4f30_likely_TPlayScreen.cpp` (119 KB):
- TPlayScreen holds pane pointers at offsets `0x2a0, 0x2a8, 0x2ac, 0x2b0, 0x2b4, 0x2b8, 0x2bc, 0x2c4, 0x2c8`.
- Method `meth_0x519850` (lines 2619-2658 of the decomp) iterates these slots querying pane state at VFT+0x1f0.
- Architecture supports two character-panel instances (slots accommodate them) but **slot-to-class mapping is not yet confirmed**.
- Classes `cls_0x5756d0` and `cls_0x51f6b0` are referenced from TPlayScreen pane-update dispatch — possibly the character-state→pane binding layer, not yet identified.

## Asset / string evidence

- "Locke" string in TPlayScreen (default player name, not character-panel-specific).
- **No character-panel-specific asset filenames** found in `recon/analysis/class_index.tsv` for the strings hunted (`portrait`, `face`, `charwin`, `playerwin`, `targetwin`, etc.). Suggests asset loading is abstracted in a resource manager that wasn't scanned.

## Alternatives considered and rejected

- `TEquipPane` (cls_0x54dd40) — 1.7 KB, right-sidebar equipment paper-doll, not upper-corner character panel.
- `TButtonPane` (cls_0x42bd90) — 8.1 KB, generic button container.
- `TConsolePane` (cls_0x4a22f0) — game-log overlay, not character stats.

## Pre-release retrofit confirmed

- Pre-release `src/statusbar.{h,cpp}` (TStatusBar / THealthBar / TStaminaBar) are the **standalone vertical fluid tubes** the user flagged as off-spec.
- The retail combined character panel subsumes their function.
- Pre-release `TTextBar::SetHealthDisplay(name, level)` in `src/textbar.h` is the **primitive predecessor** of the right-side target panel — "show opponent name + health on the textbar line" became the dedicated target panel by retail.

## Two-instance + live-update evidence

- TPlayScreen pane array supports multiple instances at consecutive slots — consistent with left+right character panels, but no explicit confirmation.
- **No evidence yet** for a target-binding mechanism that updates the right instance live. The orbit-lock target system (per user — see `CLASSIC_HUD_REFERENCE.md` design context) means the panel must `Update()` on target change; the method that does this hasn't been located.

## Recommended next steps (priority order)

1. **Ghidra CLI full extraction of cls_0x579bb0** (TStatusBar). Validate: total struct size, complete field list, all methods (especially Draw / DrawBackground / Initialize / Update / SetTarget). Confirms whether the readable decomp is incomplete or the class is genuinely minimal.
2. **TPlayScreen initialization deep dive.** Locate the function that populates the pane slots (`0x2a0`, `0x2a8`, ...). Identify which slot holds the character panel. Look for two consecutive `new <Class>(...)` calls in close proximity (the dual-instance signature).
3. **Trace cls_0x5756d0 and cls_0x51f6b0.** They appear in TPlayScreen's pane-update dispatch — may be the character-state ↔ pane binding layer that updates the target panel live on target change.
4. **Binary string search.** Grep `data/Revenant.exe` directly (not Ghidra-extracted strings) for `*portrait*`, `*face*`, `*win*`, `*bar*`, `Level `, `Lvl:`. Cross-reference asset names against class_index.
5. **Visual validation harness.** Once a candidate is identified and ported, build `--test=ui-character-panel` mode rendering two panels side-by-side. Compare against `docs/ui/sample_screen_3.jpg` via the `reference-visual-verification` screencapture loop.

## Critical do-not

**Do NOT port `cls_0x579bb0` TStatusBar from its current readable form.** The visible structure is either (a) incomplete due to Ghidra recovery failure, or (b) genuinely minimal and TStatusBar is just a state-tracker (not a UI container) — in which case the real panel class is elsewhere. Premature porting cascades errors into the rest of the HUD pipeline. Escalate Ghidra CLI extraction first.

## Open questions remaining

- Is `cls_0x579bb0` the full character panel (incomplete decomp) or just a sub-component (state tracker only)?
- Where does the portrait bitmap live? (asset name / atlas / per-character lookup table)
- What's the live-update path on target change? (which method gets called when player retargets via orbit-lock cycle?)
- Is the sword-icon-circle below the panel in `sample_screen_4` part of this class's rendering, or a sibling pane?

## Hunt log

- **2026-05-16** — first-pass `Explore` sub-agent hunt. Findings transcribed above. Next pass: Ghidra CLI extraction of cls_0x579bb0.
