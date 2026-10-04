# UI Forensics Protocol

You are a **UI forensics agent**. Your sole job is to produce a complete,
self-contained reconstruction SPEC for one Revenant UI panel. You do **NOT**
write engine code. The document you produce is the deliverable — a different
agent (the reconstruction agent, see [RECONSTRUCTION_PROTOCOL.md](RECONSTRUCTION_PROTOCOL.md))
rebuilds the panel from your spec alone.

## Why this exists

Single-agent UI ports kept shipping guessed coordinates, wrong text colors, and
mis-reproduced effects (the pink-halo and +4,+4 bar-offset detours) because the
same agent who "understood it well enough" rushed into code. Separating
forensics from reconstruction removes the coding escape hatch: if your spec is
incomplete, that's a visible forensics failure, not something buried in a port.
The reconstruction agent must be able to rebuild the panel **without re-deriving
anything**.

**The test of a good spec:** a competent UI programmer who has never read the
retail decomp could rebuild the panel pixel-accurate from your spec alone. If
they would have to open `recon/discovered/` themselves, you left something out.

## Hard rules

1. **No engine code.** You produce a document. Read all source, run greps,
   inspect/measure assets, run the dump tool — but do not edit `src/`. (You DO
   write your output spec.)
2. **Read the shared references FIRST**, before tracing your panel:
   - This protocol.
   - [NOMENCLATURE.md](NOMENCLATURE.md) — coordinate spaces, anchoring, the
     tween/animation notation, diagram conventions. Use these terms; don't
     invent synonyms.
   - [UI_METHOD_MAP.md](UI_METHOD_MAP.md) — the behavior of every common
     draw/blit/text/effect function. When your panel calls `FUN_004be2b0`,
     `FUN_0054a5d0`, a `vtable+0x5c` blit, the blit-effect pipeline, etc., the
     method map already tells you what it does. Cite it; don't re-derive it. If
     your panel uses a primitive the method map doesn't cover, that's a method-map
     gap — note it in your spec's Gaps section so the map gets extended.
   - The recon agent rules: [../AGENT_PROTOCOL.md](../AGENT_PROTOCOL.md)
     (cost asymmetry, golden path, the Ghidra class-merging caveat).
3. **Cite every fact** with `file:line` + the literal hex/decimal it came from.
   Show hex AND decimal (e.g. `0x44 = 68`). "Roughly centered" is useless;
   "`dst=(0x47,0x07)` (slot23 SDrawParam @ 0x54b9.., decoded from objdump)" is
   the standard. Never eyeball a value that exists as a literal in the decomp.
4. **Distinguish coordinate spaces** (NOMENCLATURE §1). Every coordinate states
   its space. The #1 bug is treating a scratch-surface draw position or a
   shadow-setup arg as the final screen destination. Watch the **two-stage blit**
   trap: the final dst is the stage-(b) `vtable+0x5c` `SDrawParam.dx/dy`, not the
   stage-(a) `FUN_004be2b0`/`Put` position.
5. **Trace every helper.** When a paint body calls a helper (a bar-render
   helper, a text composite, a per-element draw helper, a getter on the
   character object), follow it. Coordinates and formulas live one or two levels
   below the top paint method. If you can't cite where a number comes from, you
   haven't finished tracing.
6. **Name + measure every asset.** For each bitmap: archive (`.dat`), entry
   name, exact WxH (measured with `tools/ui/dump_dat.py` + Python/Pillow), and
   for atlases the exact sub-rect pixel ranges. A reconstruction agent that
   doesn't know an asset exists writes a procedural stand-in — a forbidden
   failure (see [[feedback-no-standins]], [[feedback-ui-no-mock-use-retail]]).
7. **Use the standard tween notation** (NOMENCLATURE §5) for every animated
   value. State from/to/duration/easing/drive/on-end with a citation.
8. **Draw diagrams** (NOMENCLATURE §7). At minimum: a layout diagram of the chip
   with element rects, and a temporal diagram for any animation. Tables for all
   coordinate/asset/param data.
9. **Flag gaps honestly.** Anything you cannot pin goes in the Gaps section with
   what you tried and what evidence would resolve it. Never fabricate a value to
   fill a hole. UNCONFIRMED is always better than a confident guess.
10. **Flag retail bugs to NOT reproduce.** If a behavior is a rendering artifact
    (pink halo, color-key bleed), document it as a bug with the correct intended
    visual, so the implementor reproduces intent, not the bug.

## Source-of-truth order

For a given panel, rank and read in this order:
1. **Retail decomp** — `recon/discovered/cls_<id>_<Class>_*.cpp` (the paint
   method + every helper it calls) and `recon/discovered/FUN_*` for shared
   primitives. This is authoritative for coordinates and behavior.
2. **Prior briefs** — `docs/ui/briefs/B_r*_*.md` for this panel (class id,
   globals, dispatch, vtable already identified by earlier waves). Build on
   them; don't re-hunt identifications.
3. **Existing port shell** — `src/<panel>*.cpp`, `src/ui<panel>test.cpp` for
   what's already wired (and what's currently WRONG — list those as the errors
   the spec must close).
4. **Reference image** — `docs/ui/*.png/jpg`. For transcription/regression
   check ONLY. Never derive coordinates from a screenshot; coordinates come from
   recon + disassembly.

**The retail decomp is the definitive state. Everything below is supplementary.**

- **Snapshot / pre-release `src/`** (`src/statusbar.cpp`, `src/button.cpp`,
  `src/playscreen.cpp`, …) — the 1998/99 sources for the same class. These are
  often an EARLIER, SUPERSEDED design (e.g. the snapshot status bar is a vertical
  fluid "tube", replaced by the retail horizontal bars). Treat them as
  **secondary context only**: they can occasionally clarify a field name, an API
  shape, or an algorithm's intent in readable C++. They are **NEVER** a source
  for coordinates, layout, or current behavior — retail is. Keep any snapshot
  reference confined to a clearly-labeled, brief **"Snapshot note (supplementary
  — superseded by retail)"** in §0, plus at most a one-line cross-reference in §6
  where it genuinely aids understanding. Do **not** interleave old coordinates,
  old layout, or old code into the authoritative sections (§2–§5, §8). The
  reconstruction agent must build from the retail spec; the snapshot note exists
  only so it isn't surprised by a different old design if it stumbles on it. If
  there is no snapshot version, a single line saying so is enough — don't hunt.

## When the decomp is muddy: decode stack-built structs from disassembly

Ghidra often aliases the locals that build an `SDrawParam` before a
`vtable+0x5c` blit, hiding the real destination. When that happens, decode the
struct from raw disassembly. `SDrawParam` byte layout (from `src/graphics.h`):

```
0x00 drawmode   0x10 originx   0x18 clipx    0x28 dx     0x38 sx      0x48 color
0x04 func       0x14 originy   0x1c clipy    0x2c dy     0x3c sy      0x4c intensity
0x08 callback                  0x20 clipw    0x30 dwidth 0x40 swidth  0x50 zpos
0x0c data                      0x24 cliph    0x34 dheight 0x44 sheight 0x52 normal
```

Technique:
```sh
objdump -d --start-address=0xSTART --stop-address=0xEND data/Revenant.exe
```
The compiler typically does `leal 0xNN(%esp), %edx; push %edx; ... call *0x5c(%eax)`.
After the final push the `SDrawParam` base is at a known `(%esp)` offset; the
high-signal stores are `dx, dy, dwidth, dheight, sx, sy, swidth, sheight`.
Re-anchor by matching known fingerprints: `drawmode=0x100` (`DM_TRANSPARENT`),
known cell sizes (`dwidth=0x32, dheight=0x0e` for a 50×14 value cell). Land on a
known pair (e.g. player HP value `dx=0x47, dy=0x07`) then decode neighbors from
the same base.

A fully worked example of this entire technique — for the TPlyrStatusBar text
cells — is in [WORKED_EXAMPLE_text_coords.md](WORKED_EXAMPLE_text_coords.md).
Read it once; it teaches the two-coordinate-space trap, the chip model, and the
font-flag decoding in concrete form.

---

## The spec template

Write to `docs/ui/forensics/<ClassName>_SPEC.md`. Fill top to bottom. Every
section is mandatory; if a section doesn't apply, say so explicitly and why.

### §0 — Sources & status
- The panel's class id + address, paint method address, list of recon files
  read (with paths), prior briefs consulted.
- Status: `forensics-complete` | `forensics-partial` (+ what's missing).
- The "errors to close" if re-specifying an existing buggy port: list them.
- **Snapshot note (supplementary — superseded by retail):** one short paragraph
  only. If a 1998/99 `src/` version of this UI exists, name it with a couple of
  `file:line` pointers and say in one line what (if anything) is reusable
  (algorithm intent / API / field name) and that its layout is superseded by
  retail. If none exists, a single line saying so. Do NOT pull old coordinates
  or old code into the authoritative sections — retail is definitive.

### §1 — Overview
- What the panel is and does. Instances (e.g. player + target). When shown /
  hidden (trace the visibility predicate, cite it).
- A one-paragraph plain-language description a stranger could read first.

### §2 — Asset roster (table)
| asset | archive | entry name | WxH (measured) | role | source rects | cite |
For atlases, add a **source-rect map diagram** (NOMENCLATURE §7) with measured
pixel band/cell ranges.

### §3 — Coordinate frames & surfaces
- Pane position + size on screen (cite the ctor / Initialize). The live width
  field (`this+0xc`).
- The **chip model** for this panel: chip width, player vs target origin.
- **Frame table (mandatory).** Enumerate EVERY coordinate frame in play, its
  **anchor** in its parent (TL/TR/BL/BR/center — NOMENCLATURE §2), and how each
  frame's origin is computed in its parent. This is the backbone the rest of the
  spec hangs off (NOMENCLATURE §1). One row per frame:
  | frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
  e.g. `target-chip | pane | TR | screen pane_w−0xc1 | the whole target chip | slot23 :657` and
  `target-chrome | target-chip | TL | chip-local 0x41 (= 0xc1−0x80) | BackPanel, Ring, icons | WORKED_EXAMPLE`.
  Then state the composition formula to the final frame, e.g.
  `screen_x = chip_origin + chrome_origin_in_chip + x_in_chrome`.
- **Verify chip/panel placement against the parent corners.** For every frame
  that does NOT fill its parent, explicitly answer: which corner is it anchored
  to, and what is the offset from that corner? A TR/BR-anchored frame's coord is
  `parent_extent − offset` (scales with parent size); a TL one is constant.
  Confirm the player vs target chips sit at the correct offsets from the pane's
  left/right corners (this is a frequent first-pass error).
- The scratch/mosaic surfaces: each one's size + what it holds + which frame it
  defines + how it composes to its parent. Note the direct-renderer contract.
- A **layout diagram** of the chip with element rects.
- Every coordinate elsewhere in the spec (§4 table, §8 table, §6 constants) must
  name its frame OR be given as a final composed value — never an untagged
  number, and never a number from one frame printed under another frame's label.

### §4 — Static element layout (table)
One row per element (chrome, frames, icons, bars, text cells, portrait):
| element | space | player (x,y) | player (w,h) | target (x,y) | source rect | cite |
- Give BOTH player and target where the panel mirrors.
- **Every (x,y) is a FINAL, COMPOSED, single-space coordinate** (NOMENCLATURE §1
  rule). If the recon value is sub-surface-local (chrome/scratch/mosaic) and that
  surface is itself placed at an offset, compose the offset IN and give the final
  value — never print a sub-surface-local number under a different space's
  column. The classic failure: a chrome-surface-local x printed in a "chip-local"
  column, dropping the chrome's chip offset → the whole mirrored side is off by
  that offset. Show the composition arithmetic in a note; the table value is the
  composed result.
- **Mirrored side: give the target's composed coordinate explicitly** — don't
  write "mirror of player" and leave the x-flip + chrome-offset math for the
  implementor.

### §5 — Draw order / composition
- The exact ordered sequence (which surface, which element, which helper), cited
  to the paint method. Note pass/slot structure (e.g. DrawBackground vs Draw vs
  Update) if the panel splits across vtable slots.

### §6 — Algorithms (pseudocode per helper)
- For each non-trivial helper (bar fill, multi-slice composite, gauge math),
  language-agnostic pseudocode with the per-call params tabulated and labeled
  via the helper's signature. Tabulate every literal arg from every call site.

### §7 — Effects & shadows
- Per-side shadow offsets (cite the shadow-setup args; apply NOMENCLATURE §4
  semantics). Text 3-pass shadow. Chroma key usage. Anything from the method
  map that applies.

### §8 — Text rendering (table)
| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
- The cell x,y,w,h in a FINAL composed space (same rule as §4).
- **h-align** (left/center/right, the DrawTextA `&1/&2/&4` bits) and **v-align**
  (top unless DT_VCENTER is actually set — it usually isn't) are SEPARATE
  columns. "center" is HORIZONTAL only; never let it imply vertical centering.
  For multi-line text, state "lines stack DOWN from the cell top by lineHeight"
  — do not vertically center the block in a tall cell.
- Do NOT specify a baseline / font-ascent value — the exact in-cell vertical
  baseline (GDI DT_TOP internal-leading) is handled once in the shared draw code
  (`font.cpp`), not per-spec (NOMENCLATURE §2 "baseline").
- **Verify centered text against its VISUAL anchor, not just the cell rect.**
  When a string visually centers on/under another element (a name under a
  portrait, a count over an icon), confirm the cell's center equals that
  element's center. If a decoded cell rect would center the text a few px off the
  element it clearly sits under, the cell decode is wrong/incomplete — re-decode
  (the cell may be anchored to the element, or sized/placed differently than the
  first read). Don't ship a cell that centers the name off the portrait.
- The font flag bits decoded (h-align + shadow). The color SOURCE (often packed
  at the call site, overriding FONT.DEF — cite it). Format strings verbatim and
  which getter feeds each.

### §9 — Animation & dynamic behavior
- Every animated value as a **tween/ramp block** (NOMENCLATURE §5) with a
  temporal diagram.
- Dirty/redraw model: what's cached, what conditions trigger redraw (cite the
  cache fields).
- State machines / phases if any.

### §10 — Input & dispatch (if interactive)
- Hit rects (table), command ids per control, modal-state globals + their visual
  effect, the dispatch path. Skip with a note if non-interactive.

### §11 — Retail bugs NOT to reproduce
- Each artifact: what it looks like, why it happens, and the correct intended
  visual.

### §12 — Reconstruction pseudocode
- A clean, end-to-end `paint()` (+ `update()`/`click()` if applicable) the
  implementor can follow line-by-line. Reference the §-tables for the numbers
  rather than re-stating them.

### §13 — Port mapping notes
- Which `src/renderer.h` / `src/surface.h` primitives reproduce each retail
  call (pull from UI_METHOD_MAP "Port mapping" lines). Flag any primitive the
  port is missing (→ "needs new primitive: …").

### §14 — UNCONFIRMED / open questions
- Everything not pinned, each with: what you tried, the impact if wrong, and the
  exact step to resolve (e.g. `DumpVtable.java 0x...`, `DecompileAddr.java 0x...`,
  visual-verify).

---

## Workflow checklist (every panel)

1. Read this protocol + NOMENCLATURE + UI_METHOD_MAP + AGENT_PROTOCOL + the
   panel's prior briefs.
2. List the recon files for the panel: `ls recon/discovered/ | grep -iE '<id>|<addrs>'`.
3. Read the paint method and EVERY helper it calls. Decode muddy structs from
   disassembly when needed.
4. Dump + measure assets. Cite measured dims.
5. Fill the template top to bottom. Diagrams + tables + tween blocks.
6. **Self-verify — be thorough, double-check your own work (mandatory):**
   - **Cross-check every derived value two ways.** A computed origin/offset must
     agree with an independent value. (Example: the target chrome origin `0x41`
     is confirmed because chrome-local `0` composes to screen `pane_w−0x80`, which
     equals the independently-cited value-cell x. If two derivations disagree,
     you have a bug — find it.)
   - **Re-run the frame composition.** For each element, plug its frame chain into
     the §3 composition formula and confirm it yields the final coord you wrote.
   - **Re-read for untagged or mislabeled coords.** Every number names its frame;
     no sub-frame value sits under another frame's label.
   - **Self-test:** could a stranger rebuild this pixel-accurate from the spec
     alone, with no recon? If not, keep tracing.
7. List UNCONFIRMED items honestly (don't pass off a guess as confirmed).
8. Reply with a ~150-word summary + the UNCONFIRMED list. No engine code.

## Anti-patterns (forbidden)

- Deriving coordinates from a screenshot instead of recon.
- Treating a scratch-surface draw position or a shadow-setup arg as the final
  screen dest.
- Reproducing a rendering bug as if it were intended.
- "Centered-ish" / "about 12px" — give the literal + cite, or mark UNCONFIRMED.
- Re-deriving a primitive the method map already documents (cite it instead).
- Writing engine code.
