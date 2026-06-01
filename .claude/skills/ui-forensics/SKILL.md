---
name: ui-forensics
description: This skill should be used when the user asks to "do forensics on" a Revenant UI panel, "document"/"trace" a panel for reconstruction, produce a "spec"/"reconstruction doc" for a HUD/menu/sidebar panel, or dispatch a UI forensics-phase agent. It produces a complete, self-contained pixel-accurate panel spec (assets, coordinate spaces, element layout, draw order, algorithms, effects/shadows, text, animation tweens, input/dispatch, retail-bug flags, pseudocode) with every fact cited to recon file:line, and writes NO engine code.
version: 0.1.0
---

# UI Forensics

Produce a complete, self-contained reconstruction SPEC for one Revenant UI panel
(stat bars, sidebar tabs, character pane, dialog, log, etc.). You do NOT write
engine code — the spec is the deliverable; a separate reconstruction agent
(`ui-reconstruct`) rebuilds the panel from your spec alone.

The full protocol, spec template, and source-of-truth order are the
version-controlled source of truth at:

**`docs/ui/forensics/FORENSICS_PROTOCOL.md`**

Read that file in the UI worktree
(`/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`) and follow it
exactly. Before tracing your panel, also read the shared references it depends
on:

- `docs/ui/forensics/NOMENCLATURE.md` — coordinate spaces, the chip model,
  anchoring/alignment, the tween/animation notation, ASCII diagram conventions.
- `docs/ui/forensics/UI_METHOD_MAP.md` — behavior of every common draw/blit/
  text/effect function. Cite it instead of re-deriving primitive behavior.
- `docs/ui/forensics/WORKED_EXAMPLE_text_coords.md` — a fully worked trace
  teaching the two-coordinate-space trap, the chip model, and SDrawParam decode.
- `docs/ui/AGENT_PROTOCOL.md` — cost asymmetry, golden path, Ghidra
  class-merging caveat.

## Quick reference (the protocol has the full template)

1. Locate sources, ranked: retail decomp (`recon/discovered/cls_<id>_*` paint
   method + every helper), prior briefs (`docs/ui/briefs/`), existing port shell
   (`src/<panel>*`, list its current ERRORS), pre-release `src/` (names/intent,
   not coords), reference image (regression check only — never derive coords
   from it).
2. Fill the template top to bottom into
   `docs/ui/forensics/<ClassName>_SPEC.md`: §0 sources, §1 overview, §2 assets,
   §3 coords/surfaces (+layout diagram), §4 element layout table, §5 draw order,
   §6 algorithms, §7 effects/shadows, §8 text table, §9 animation (tween blocks
   + temporal diagram), §10 input/dispatch, §11 retail-bugs-not-to-reproduce,
   §12 reconstruction pseudocode, §13 port mapping, §14 UNCONFIRMED.
3. **State the coordinate space** for every number (screen/pane/chip/surface/
   source). Watch the **two-stage blit** trap — the final dst is the
   `vtable+0x5c` `SDrawParam.dx/dy`, not the scratch-surface draw position.
4. **Cite every fact** `file:line` + literal hex/decimal (show both bases).
   Decode muddy stack-built structs from `objdump` disassembly when Ghidra
   aliases them.
5. **Trace every helper.** **Measure every asset** with `tools/ui/dump_dat.py`.
6. **Tables + ASCII diagrams + tween blocks** required. **Flag gaps honestly.**
   **Flag retail bugs** with the correct intended visual.
7. Self-test: could a stranger rebuild this pixel-accurate from the spec alone?
8. Update the README panel-roster row. Reply with a ~150-word summary + the
   UNCONFIRMED list. No engine code.

The test of success: a competent UI programmer who never read the retail decomp
could rebuild the panel pixel-accurate from your spec alone.
