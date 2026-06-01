---
name: ui-reconstruct
description: This skill should be used when the user asks to "reconstruct"/"rebuild"/"implement" a Revenant UI panel from a forensics spec, or dispatch a UI reconstruction-phase agent. It takes a completed panel spec, rebuilds the panel in the engine via the real retail assets + port primitives (Renderer->DrawBitmap/Subrect/Shadowed, BuildTTFAtlas + CompositeSwapchain), wires a --test harness, verifies visually, and reports spec gaps back.
version: 0.1.0
---

# UI Reconstruction

Rebuild one Revenant UI panel in the engine from a completed forensics spec,
verify it visually, and report spec gaps. The spec is the single source of
truth — if it's complete you should not need to open the recon.

The full protocol is the version-controlled source of truth at:

**`docs/ui/forensics/RECONSTRUCTION_PROTOCOL.md`**

Read that file in the UI worktree
(`/Users/benjamincooley/projects/RevenantRevisited/worktrees/ui`) and follow it
exactly. Your spec is `docs/ui/forensics/<ClassName>_SPEC.md`. Also read the
shared references it leans on: `docs/ui/forensics/NOMENCLATURE.md` (coordinate
spaces, tween notation, diagrams) and `docs/ui/forensics/UI_METHOD_MAP.md` (the
"Port mapping" line per retail primitive → which `src/renderer.h`/`src/surface.h`
call reproduces it).

## Quick reference (the protocol has the full detail)

1. Read the spec end to end. If status is `forensics-partial` or §14
   UNCONFIRMED is large, flag before sinking time in.
1b. **Derive the burndown** → `docs/ui/forensics/<ClassName>_BURNDOWN.md`: a
   checkbox list of every discrete thing to build (assets, surfaces, elements,
   draw order, algorithms, effects, text, animation, input, new primitives,
   errors-to-close), each citing its spec §. Work it top to bottom, checking
   items off as you land + visually confirm each. It's the living status of the
   panel.
2. Build per the spec's §5 draw order + §6 algorithms, pulling coords (§4),
   assets (§2), text (§8), effects (§7), animation (§9) directly from the spec.
3. **The spec is the contract** — if you need the recon to answer a question,
   that's a GAP to report, not patch silently.
4. **No mocks/stand-ins** — render the real §2 assets through real port
   primitives; a wrong-looking bitmap is a loader bug to debug, not bypass.
5. **Preserve the direct-renderer contract** — compose the chip into a fixed
   render target, then blit that surface in the HUD swapchain pass. Don't mix a
   bitmap path and a glyph-straight-to-swapchain path (scale drift).
6. **Reproduce intent, not bugs** (spec §11). **Gate animation to 24Hz**
   (`kSimTickMs = 1000/24`).
7. Wire a `--test=<mode>` harness with synthetic state exercising every cell/
   value/animation. Build clean.
8. Verify visually: capture the window (`tools/ui/snap_compare.py` or the
   `find_revenant_window_id` + `screencapture -l` recipe), crop, `Read` the PNG,
   compare to the spec's layout diagram + reference image. Check positions, text
   size/color/alignment, shadow direction+offset, animation cadence, mirror.
9. **Report spec gaps** — every fact you had to re-derive from source. This
   feeds back into the spec + protocol + method map so the next handoff is
   cleaner. A reconstruction that needed zero source reading is the goal.
10. Commit on the UI feature branch.

## Common-methods rules (read the protocol's "Port primitives" section)

1. **Use the common methods — never hand-roll.** Drop shadow, tinted blit, text,
   text-with-shadow, glyph walk all already exist as shared primitives. Call
   them; don't open-code a tinted-black+bitmap pass or a per-glyph loop in a
   panel.
2. **Missing primitive → implement ONCE in the canonical location, register it,
   reuse forever.** Pixel/image primitives → `TRenderer` (`src/renderer.{h,cpp}`,
   game-agnostic). Text/font primitives → UI/font layer (`src/font.{h,cpp}`).
   Then add it to the protocol toolbox table + `UI_METHOD_MAP.md`. Never a
   panel-local one-off.

Panels follow the **compose-to-target contract** (compose the chip into one
offscreen `TSurface` RT, then `DrawSurface` to the HUD), so use the `…ToTarget`
family: `DrawBitmap{,Subrect}{,Tinted,Shadowed}ToTarget`, and for text
`DrawText{,Shadowed}ToTarget(atlas, text, cellX,cellY,cellW, ETextAlign, r,g,b,
tw,th)` with `TextWidth` / `TextAscent`. `DrawTextShadowedToTarget` already does
the retail 3-pass black shadow + colored top — never write that loop. Face =
Liberation Sans (`thirdparty/fonts/`). Magenta `0xFF00FF` is the chroma key.
