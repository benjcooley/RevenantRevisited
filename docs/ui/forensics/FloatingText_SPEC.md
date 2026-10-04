# FloatingText (TPlayScreen post-char anim/text queues) — Forensics SPEC

> **Status:** `forensics-partial` (mechanism + emitters + retail-side font
> picked from snapshot + FONT.DEF; retail-side per-frame drain loop NOT
> extracted from Ghidra recon — see §0 + §14). The spec captures the system
> *intent* faithfully enough to reconstruct the snapshot behavior; the open
> question is whether shipped retail moved/reshaped the drain loop, since the
> recon's TPlayScreen Animate body is trivial and does NOT iterate the queues.
>
> **Important task-framing finding (read this before §1):** there is no
> persistent "floating text" widget with a rise-and-fade tween. The retail
> mechanism is a **per-frame transient queue** that the emitter (gameplay code)
> re-stamps every frame. The visual effect (rising number, sustained dialog
> bubble) is produced by the **emitter** updating its (x,y) and/or frame index
> each tick — *not* by any tween in the queue. The snapshot's `flashred`
> bitmap-impact emitter is gated `#if 0`; there is **no shipped damage-number
> emitter in the snapshot source tree**, so "damage numbers float over enemies"
> (PLAN.md:128) is a Phase B feature still to be added on top of this queue,
> not a thing the queue itself does.

---

## §0 — Sources & status

- Subsystem: TPlayScreen post-character overlay queues. Two parallel single-
  frame ring/list buffers + a per-frame drain in `TPlayScreen::Animate`:
  - `postanim[MAXPOSTCHARANIMS]` — bitmap overlays (spell sparkles, impact
    flashes, projectiles that aren't world-z'd).
  - `posttext[MAXPOSTCHARANIMS]` — dialog/floating text overlays.
- Port site of stub (the thing this spec exists to unblock):
  - `src/playscreen.cpp:1217-1223` — `TPlayScreen::AddPostCharAnim` no-op stub.
  - `src/playscreen.cpp:1225-1229` — `TPlayScreen::AddPostCharText` no-op stub
    (`TODO(port): same as AddPostCharAnim, for floating dialog text`,
    `src/playscreen.cpp:1228`).
  - `src/playscreen.h:172-180` — declarations + intent comment.
  - `src/renderer.h:432` — "Live overlay: do per-frame work in Draw() and issue
    draw calls directly (e.g. cursor, debug arrows, floating text)." —
    pre-existing class doc that names the floating-text path as the canonical
    live-overlay use case.
- Retail recon class (vtable confirmed): `TPlayScreen` = `cls_0x5a5320`
  (`recon/ghidra/cls_0x5a5320__vftable_5a5320.cpp:7-19`, slot 0 dtor 0x47f510
  / slot 1 Initialize 0x47a660 / slot 4 DrawBackground 0x47bd20 / slot 5
  Animate 0x47b4a0).
- **Retail Animate body is trivial** (`recon/discovered/cls_0x5a5320_TPlayScreen_Animate_47b4a0.cpp:1-12`):
  it only flushes the `+0x69c` callback (next-pane init) and chains to
  `TScreen::Animate` (`FUN_0048ff00`). It does NOT iterate any post-char
  arrays. There is no extracted recon body that drains a postanim/posttext
  queue. The Pulse body
  (`recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp`, 2119 bytes) is
  pane-visibility / multi-mode toggling — no array drain either. The
  DrawBackground body (slot 4) is the autosave/loadgame TTextBar driver
  (PlayScreenOverlay_SPEC.md) and does not draw post-char overlays. **Where
  shipped retail drains the queue — if it kept this design at all — is
  UNCONFIRMED-A** (§14).
- Recon files consulted:
  - `recon/ghidra/cls_0x5a5320__vftable_5a5320.cpp` (vtable slot identity)
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Animate_47b4a0.cpp` (trivial
    body — does NOT drain queues)
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Pulse_47b4d0.cpp` (no queue
    drain)
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Initialize_47a660.cpp` (no
    `numpostchar*=0` initialization visible at top — array offsets unconfirmed)
  - `recon/discovered/cls_0x5a5320_TPlayScreen_Close_47b290.cpp`
  - `recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7772-7783` (the trivial
    Animate, same content as above)
  - `docs/ui/forensics/PlayScreenOverlay_SPEC.md:1-80` (slot 4 / DrawBackground
    is the autosave/loading driver — NOT the post-char path)
  - `docs/ui/forensics/UI_METHOD_MAP.md` §5 (`FUN_004be2b0` text draw + 3-pass
    shadow), §1 (`SDrawParam` layout, drawmodes), §15 (vtable slots)
- **Snapshot note (supplementary — superseded by retail where they disagree).**
  The 1998/99 snapshot has the complete C++ for both the emitter API (header-
  inline) and the per-frame drain (in `TPlayScreen::Animate`), and the snapshot
  is the only authoritative source for the system *intent* given the recon gap
  (§14 UNCONFIRMED-A). The snapshot is **not** authoritative for whether
  shipped retail kept the same per-frame drain at the same call site, only for
  what the data-flow looks like algorithmically:
  - `RevenantRepo`-side: not present; only legacy mirrors remain.
  - `RevenantRevisited/legacy/playscreen.h:22` defines `MAXPOSTCHARANIMS 30`.
  - `RevenantRevisited/legacy/playscreen.h:105-117` — inline emitter API.
  - `RevenantRevisited/legacy/playscreen.h:189-193` — instance arrays + count
    fields (private). Struct shapes are the load-bearing fact:
    `postanim` = `{int x, y, z; PTBitmap bm; DWORD drawmode; int dim;}` (24 B
    on x86-32, given SColor=3B+pad pattern not applicable here), and
    `posttext` = `{char* text; int x, y; SColor color; int wrapwidth;}` (20 B
    after SColor's 3 RGB bytes pad to 4 before `wrapwidth`).
  - `RevenantRevisited/legacy/playscreen.cpp:288-289` (Initialize: zero counts).
  - `RevenantRevisited/legacy/playscreen.cpp:451-502` — Animate body: the
    drain loop. The dead `flashred` ResolveImpact site is at
    `RevenantRevisited/legacy/character.cpp:1737-1759` (gated `#if 0`); the
    floating dialog text caller is at
    `RevenantRevisited/legacy/character.cpp:545-573`. The `AddPostCharAnim`
    spell-anim caller is `RevenantRevisited/legacy/animimage.cpp:362-403`.
- **Status:** `forensics-partial` (see top banner) — emitter behavior, font,
  draw flags, and frame composition are HIGH confidence; the retail vs
  snapshot equivalence of the drain loop's call site is UNCONFIRMED-A. The
  *visual effect* the reconstruction must produce is unambiguous: a per-frame
  re-stamped bitmap/text at a world-projected screen position, with no
  per-overlay lifetime, no rise tween, and no fade tween inside the queue.
- **Errors to close (port stubs):** both `AddPostCharAnim` and `AddPostCharText`
  are `TODO(port)` no-ops at `src/playscreen.cpp:1217-1229`. No `attic` copy
  is wired to the new render path; the prior shim at
  `attic/src/playscreen.cpp:519-550` (the pre-rewrite port) shows the snapshot
  loop literally translated and is a useful pseudocode oracle.

---

## §1 — Overview

The post-char queues are two per-frame transient buffers TPlayScreen owns and
drains every frame, after the 3D world is rendered and before the UI
overhangs/cursor are composited. They exist so that gameplay simulation code
(monster combat code, projectile code, AI dialog code) can request a single
overlay draw without having direct access to the rendering surface or the
draw-order pipeline. The queue says:

- *I want this bitmap composited on top of the world this frame at this
  screen position (and this z, if the drawmode opts in).* — `AddPostCharAnim`.
- *I want this string composited on top of the world this frame at this
  screen position, with this RGB and this max wrap-width.* — `AddPostCharText`.

The "floating" / "rising" appearance is produced by the **emitter** updating
its world-position and frame counter each tick. The queue does **no animation
of its own**: each entry exists for exactly one frame and is then cleared
(plus, for text, the heap copy is `delete[]`d the same frame —
`legacy/playscreen.cpp:495`).

**When shown:** every frame the playscreen is animating (`draw==TRUE` and not
in `InCompleteExclusion()`). Drain skipped in exclusion modes (load/save
screen, modal pause).

**Two uses confirmed in the snapshot tree:**

1. **Non-z-buffered spell/effect animation frames.** Whenever
   `TAnimAnimator::Animate` picks a frame whose source bitmap does NOT have
   `BM_ZBUFFER` set (i.e. the artist did not author a z-mask), the path falls
   through to `AddPostCharAnim` with `drawmode | DM_ZSTATIC` so the frame
   draws on top of the world at a fixed z-class.
   - Cite: `RevenantRevisited/legacy/animimage.cpp:393-399`.
2. **Character speech float-text.** While a character has an `ACTION_SAY`
   action with `data != NULL` and `wait > 12` (the "half-second between
   sentences" gate), `TCharacter::Animate` projects `pos` to screen and
   submits the text 120 px above the head.
   - Cite: `RevenantRevisited/legacy/character.cpp:545-573`.

**Damage numbers (PLAN.md:128) are NOT shipped by the snapshot.** The only
damage-overlay site is the `flashred` 4-frame bitmap impact, which is gated
`#if 0` in *both* `character.cpp:1737-1759` and `charactersav.cpp:1642-1660`.
A damage-number emitter, when added in Phase B, will be a new caller that
formats a `"%d"` red string and calls `AddPostCharText` once per frame for the
overlay's intended life, stepping y upward and/or alpha downward in the
emitter's own state.

---

## §2 — Asset roster

Both queues take their bitmap/text payload from the caller — there is **no
queue-owned asset**. For each call site the relevant asset is:

| asset | archive | entry | WxH (measured) | role | cite |
|-------|---------|-------|----------------|------|------|
| spell/effect anim frame | per-imagery `.I3D` | per `TAnimation` state | varies | the bitmap the caller passes to `AddPostCharAnim` | `legacy/animimage.cpp:374` |
| `"flashred"` (Phase B reference) | UNCONFIRMED — likely `Misc\flashred.I3D` or similar; 4 frames | UNCONFIRMED | UNCONFIRMED | the dead damage-flash emitter would use this animation | `legacy/character.cpp:1750` |
| dialog font (text payload) | `font.def` entry **`"Dialog"`** = WINFONT Times New Roman 20 (TTF, color 50/180/255 default) | n/a | n/a | the font the snapshot drain uses for the colored pass | `RevenantRevisited/data/resources_unzipped/font.def:56`; selected via `DialogFont` (font id **101** at `RevenantRevisited/legacy/revmain.cpp:1581`) |
| dialog shadow font | `font.def`-derived shadow companion (snapshot uses `DialogFontShadow` = font id **102**) | n/a | n/a | the font the snapshot drain uses for the black y-1 shadow pass | `RevenantRevisited/legacy/revmain.cpp:1582`; usage `legacy/playscreen.cpp:486-489` |

> **Port note:** in the modern renderer the "shadow font / colored font" split
> collapses to a single `DrawTextShadowedToTarget` call that internally does
> 3 black passes + 1 colored pass (UI_METHOD_MAP §5, §12). The snapshot's
> 2-call (shadow font then dialog font) pattern is the legacy way; the port
> primitive replaces both with one call.

> **Phase B damage-number font (not used by current code, recorded for the
> implementor):** `font.def:59` defines `WINFONT "Numbers" FONT "Arial" 12
> COLOR 255 0 0`. The bright-red Arial-12 default is the obvious match for
> rising damage numbers. The "Numbers" WINFONT is only referenced through the
> global font initializer (`FUN_00485870:00485e15` per `recon/classes_original/_data.txt:86504-86509`)
> — no shipping caller in the snapshot or recon resolves it for a per-frame
> draw. UNCONFIRMED-B in §14.

---

## §3 — Coordinate frames & surfaces

**Pane / surface chain.** The drain runs while the **map pane** has its clip
rect set, so the destination surface is the `Display` framebuffer with the
clip rect restricted to the map pane's viewport
(`legacy/playscreen.cpp:464, 480` — two `MapPane.SetClipRect()` calls bracket
the anim and text loops). In the port the equivalent is the active swapchain
pass with the map-pane viewport rect set as the scissor; the renderer's
`DrawBitmap*` / `DrawText*ToTarget` calls run inside that pass.

**The (x,y) passed to both queues is in `screen` space** (the framebuffer's
top-left origin), NOT in pane-local space, NOT in world space. Both emitter
call sites convert from world via the `WorldToScreen` helper before submitting.
The map-pane clip rect is only a scissor; it does NOT translate the
coordinates. (Cite: `legacy/object.cpp:62-68` for `WorldToScreen`; emitter
calls at `legacy/character.cpp:577-579` and `legacy/animimage.cpp:386-399`
pass these directly to the queues.)

`WorldToScreen` is the iso 2:1 projection:
```
x = pos.x - pos.y
y = (pos.x + pos.y)/2 - (pos.z * 867 / 1000)        // approx pos.z * cos(30°)
z = WORLDZOFFSET - pos.z/2 - (pos.x + pos.y) * 867 / 1000
```
(`legacy/object.cpp:62-79`). `MYSTERYVAL1=867`, `MYSTERYVAL2=1000`. The `(x,y)`
the queue stores is the **already-projected screen pixel**.

### Frame table

| frame | parent | anchor | origin in parent (formula + literal) | what is expressed in it | cite |
|-------|--------|--------|---------------------------------------|--------------------------|------|
| `screen` | framebuffer | TL | (0,0) | the (x,y) stored in `postanim[i]` and `posttext[i]` | `legacy/playscreen.cpp:467-470, 486-493` |
| `map-pane-clip` | screen | TL | `(MAPPANEX, MAPPANEY) = (32, 16)` retail / port; `w=576, h=320` | the SCISSOR rect during the drain (no translation; only clip) | port `src/revdefs.h:109-112`; snapshot `legacy/playscreen.cpp:464` |
| `world` | n/a | n/a | iso 2:1 origin | the `(pos.x, pos.y, pos.z)` the EMITTER projects via `WorldToScreen` before stamping | `legacy/object.cpp:62-68` |
| `bitmap-local` | bitmap TL | TL | bm header `[0]=w, [1]=h` | for `AddPostCharAnim`: the registration point + the (sx,sy)=(0,0) full-frame source rect | port `src/renderer.h:740-748` |

**Composition formula:**
```
screen_x = WorldToScreen(world_pos).x     // emitter computes
screen_y = WorldToScreen(world_pos).y     //    "
queue.stamp(screen_x, screen_y, ...)      // queue stores screen-space (x,y)
drain.draw_bitmap(bm, queue.x, queue.y)   // drain blits at queue.x, queue.y
                                          //   with map-pane clip rect set
```

For text the emitter additionally does `screen_y -= 120` to lift the bubble
above the head (`legacy/character.cpp:579` — the `y - 120` in the call). The
`120 = 0x78` is the head-clearance offset and lives in the EMITTER, not the
queue.

**Surfaces.** There is **one** destination surface (the framebuffer / active
swapchain pass). The snapshot does NOT use a scratch surface for either anim
or text. The map-pane clip rect ensures overlays don't leak into the UI
chrome / status bars.

### Layout diagram

```
                        screen (0,0)
+---------------------------------------------------------+
|                                                         |
|   <- UI chrome (StatusBar, Inventory, Map, etc.)        |
|                                                         |
|   +-- MAP-PANE CLIP RECT (the scissor) -----------+     |
|   |   MAPPANEX=32, MAPPANEY=16, 576 x 320         |     |
|   |                                                |     |
|   |         . dialog bubble (DialogFont, color)   |     |
|   |         .                                      |     |
|   |         .   <-- y is head_screen_y - 120       |     |
|   |        ===                                     |     |
|   |        |O|  <-- iso-projected character        |     |
|   |        ===  head_screen_y = WorldToScreen(pos).y    |
|   |                                                |     |
|   |     .* spell sparkle frame (animimage)        |     |
|   |     .* drawmode = DM_TRANSPARENT|DM_USEREG    |     |
|   |     .* |DM_ALPHA(if BM_ALPHA)|DM_ZSTATIC      |     |
|   +------------------------------------------------+     |
|                                                         |
+---------------------------------------------------------+
```

---

## §4 — Static element layout

**Not applicable.** The queues have no static elements. Every (x,y) is a
caller-supplied screen pixel produced by `WorldToScreen` (and, for text, with
the emitter's `-120 px` head-clearance applied at the call site, not in the
queue). See §3 for the coordinate composition.

Per-entry stored fields:

| queue | per-entry fields (snapshot struct) | byte size | cite |
|-------|-------------------------------------|-----------|------|
| `postanim[i]` | `int x; int y; int z; PTBitmap bm; DWORD drawmode; int dim;` | 24 B (x86-32) | `legacy/playscreen.h:189` |
| `posttext[i]` | `char* text; int x; int y; SColor color; int wrapwidth;` | 20 B (SColor=3B→4B pad before next int) | `legacy/playscreen.h:192` |

`MAXPOSTCHARANIMS = 30` for both arrays (`legacy/playscreen.h:22`,
`legacy/playscreen.h:113` — the *text* cap also uses `MAXPOSTCHARANIMS`, so
the cap name is shared; both queues are capped at 30 entries/frame).

---

## §5 — Draw order / composition

The drain runs inside `TPlayScreen::Animate(draw==TRUE)`, AFTER the base
`TScreen::Animate` + `AreaManager.Animate(draw)`, and inside the `!Editor &&
!IsFullScreen()` / `!InCompleteExclusion()` guard. Sequence
(`legacy/playscreen.cpp:451-502`):

1. `TScreen::Animate(draw)` — the world / panes draw their content (this is
   when 3D world rendering happens and when each character's `TCharacter::Animate`
   has already stamped its dialog text into `posttext`, and each spell's
   `TAnimAnimator::Animate` has stamped its frame into `postanim`).
2. `AreaManager.Animate(draw)` — area-level effects.
3. `if (draw && !InCompleteExclusion())` — the post-char drain:
   1. `MapPane.SetClipRect()` — set scissor to map-pane viewport.
   2. **Anim drain (snapshot, `:465-471`):** for each `i` in `0..numpostcharanims`:
      - If `postanim[i].drawmode & (DM_ZBUFFER | DM_ZSTATIC)`:
        `Display->ZPut(x, y, z, bm, drawmode)`.
      - Else:
        `Display->PutDim(x, y, bm, drawmode, dim)`.
   3. `DrawMouseShadow()` — the mouse shadow goes between the anims and the
      text deliberately so the shadow "sits in" the map pane.
   4. `MapPane.SetClipRect()` (again — `DrawMouseShadow` may have cleared it).
   5. **Text drain (snapshot, `:481-496`):** for each `i` in `0..numpostchartexts`:
      - First a **shadow pass**: `Display->WriteText(text, x, y-1, 99,
        DialogFontShadow, &black, DM_TRANSPARENT|DM_ALIAS, wrapwidth, 0,
        JUSTIFY_CENTER|JUSTIFY_CLIP)`.
      - Then a **colored pass**: `Display->WriteText(text, x, y, 99,
        DialogFont, &posttext[i].color, DM_TRANSPARENT|DM_ALIAS, wrapwidth, 0,
        JUSTIFY_CENTER|JUSTIFY_CLIP)`.
      - `delete posttext[i].text;` — free the `strdup`'d copy (`:495`).
   6. `DrawOverhangs(TRUE)` — composite UI overhangs back on top, so the
      anim/text never paints over the UI chrome.
4. **Reset both counts to 0** (`:501-502`): `numpostcharanims = 0;
   numpostchartexts = 0;`. This is the queue-cleared point. Next frame's
   emitters fill it fresh.

**Pre-AND-post conditions:**
- Anim drain runs BEFORE text drain. If both are present at the same screen
  spot the text appears on top of the bitmap.
- The mouse shadow draws BETWEEN them (so its shadow can be occluded by text
  but not by chrome overhangs).
- `DrawOverhangs` re-blits the UI overhang sprites AFTER both drains, so the
  status bar / inventory area always wins z-order over a stray off-map post-
  char overlay.

> **UNCONFIRMED-A (retail equivalence — same as §0).** The recon's
> `TPlayScreen::Animate` does NOT contain this loop. Where shipped retail
> drains (or whether it kept this exact design) is open. Reasonable
> hypotheses, in order of likely:
> - The drain moved into the **map pane**'s slot (TMapPane Animate or its
>   per-frame post-draw hook) — no `_data.txt` xref pins it.
> - The drain moved into a TScreen helper invoked by the base class's
>   Animate at a slot the trivial player override doesn't intercept.
> - The whole subsystem was retired and damage numbers / dialog text live
>   somewhere else in shipped retail (e.g. a TTextBar-style scrollable widget
>   parking text temporarily; floats may not exist).
>
> Resolve-step: `DecompileAddr.java 0x47b4a0` (re-check; may have inlined
> bodies), `DecompileAddr.java 0x47bd20` (re-check after stripping the
> autosave string-only summary used in PlayScreenOverlay_SPEC.md), or
> dump `FUN_0048ff00_TScreen_Animate` and the TMapPane slot 5 to find the
> queue iterators. See §14.

---

## §6 — Algorithms (pseudocode per helper)

### `AddPostCharAnim(x, y, z, bm, drawmode, dim=0)`

Append-with-cap. The snapshot is a header-inline function
(`legacy/playscreen.h:105-109`).

```pseudocode
if numpostcharanims < MAXPOSTCHARANIMS:        # 30
    n = numpostcharanims++
    postanim[n] = { x, y, z, bm, drawmode, dim }
# else: silently drop (no warning).
```

**Param semantics:**

| param | meaning | cite |
|-------|---------|------|
| `x, y` | screen-space pixel position (already iso-projected by `WorldToScreen`) | `legacy/animimage.cpp:386-399` |
| `z` | screen-z (for z-buffer / z-static path); WORD-cast inside `ZPut`/`ZPutDim` | `legacy/surface.cpp:473-479` |
| `bm` | bitmap to blit | `legacy/animimage.cpp:374` |
| `drawmode` | OR-bag of `DM_*` flags. **`DM_ZBUFFER` or `DM_ZSTATIC`** routes to `ZPut`; everything else routes to `PutDim`. `DM_TRANSPARENT`, `DM_ALPHA`, `DM_USEREG`, `DM_ZSTATIC` are the typical bits the spell-anim emitter sets (`legacy/animimage.cpp:381-399`). | UI_METHOD_MAP §1 + `legacy/playscreen.cpp:467` |
| `dim` | 0..31 darkening for `PutDim` (intensity = `31 - min(31, dim)`); default 0 = full | `legacy/surface.cpp:460-471` |

### `AddPostCharText(text, x, y, color, wrapwidth)`

Append-with-cap, copies the string (`strdup`). The snapshot is also header-
inline (`legacy/playscreen.h:112-116`).

```pseudocode
if numpostchartexts < MAXPOSTCHARANIMS:        # 30 (shares the cap!)
    n = numpostchartexts++
    posttext[n] = {
        text:      strdup(t),                  # owned, free'd by drain
        x:         x,
        y:         y,
        color:     *color,                     # 3-byte RGB copied by value
        wrapwidth: wrapwidth
    }
# else: silently drop AND the strdup is not invoked (no leak).
```

**Param semantics:**

| param | meaning | cite |
|-------|---------|------|
| `text` | C string. Caller may free their copy immediately; the queue owns its `strdup`. | `legacy/playscreen.h:114` |
| `x, y` | screen-space pixel position. The text cell anchors HORIZONTALLY around x (because `JUSTIFY_CENTER`) and TOP-aligns at y (no v-center flag); see §8. | `legacy/character.cpp:579` |
| `color` | RGB for the colored pass. Snapshot caller uses `{0,150,255}` (cyan-blue) for NPCs and `{200,0,0}` (red) for the player character. | `legacy/character.cpp:557-564` |
| `wrapwidth` | max text width before wrap. Snapshot caller uses `clip_rect_w + 1 - 32` — i.e. clip-rect width minus a 32-px safety margin. | `legacy/character.cpp:565-579` |

### Anim drain (one entry)

```pseudocode
if entry.drawmode & (DM_ZBUFFER | DM_ZSTATIC):   # 0x400 | 0x800000
    Display.ZPut(entry.x, entry.y, entry.z, entry.bm, entry.drawmode)
else:
    Display.PutDim(entry.x, entry.y, entry.bm, entry.drawmode, entry.dim)
# bm extents come from the bitmap header (bm->width, bm->height); the queue
# does not stretch.
```

`DM_ZBUFFER = 0x400`, `DM_ZSTATIC = 0x800000`, `DM_TRANSPARENT = 0x100`,
`DM_ALPHA = 0x2000`, `DM_USEREG = 0x20000`
(`RevenantRevisited/data/resources_unzipped/font.def:24-53`,
`RevenantRevisited/legacy/RevDefs.h:307-338` — same constants).

### Text drain (one entry)

```pseudocode
black = {0, 0, 0}
# Shadow pass (y-1, black, DialogFontShadow):
Display.WriteText(entry.text, entry.x, entry.y - 1,
                  numlines=99, font=DialogFontShadow, color=&black,
                  drawmode=DM_TRANSPARENT|DM_ALIAS,
                  wrapwidth=entry.wrapwidth, startline=0,
                  justify=JUSTIFY_CENTER|JUSTIFY_CLIP)
# Colored pass (y, entry.color, DialogFont):
Display.WriteText(entry.text, entry.x, entry.y,
                  99, DialogFont, &entry.color,
                  DM_TRANSPARENT|DM_ALIAS,
                  entry.wrapwidth, 0,
                  JUSTIFY_CENTER|JUSTIFY_CLIP)
delete[] entry.text     # free strdup'd copy
```

`numlines=99` is a "as many as fit" cap. `JUSTIFY_CENTER = (1<<1) = 0x02`,
`JUSTIFY_CLIP = (1<<3) = 0x08`, so the combined `0x0a` means h-center + clip
to surface clip rect. `DialogFont` = font id 101 (`legacy/revmain.cpp:1581`),
`DialogFontShadow` = font id 102 (`legacy/revmain.cpp:1582`). The font.def
entry for "Dialog" is `WINFONT "Dialog" FONT "Times New Roman" 20 COLOR 50
180 255 GCOLOR 0 0 255 SCOLOR 255 255 255`
(`RevenantRevisited/data/resources_unzipped/font.def:56`).

---

## §7 — Effects & shadows

**Drop shadow (text).** The snapshot drain uses a 1-pixel `y-1` UP shadow (NOT
down). That is unusual vs the UI's down-right shadow convention
(NOMENCLATURE §4 — UI chrome shadow lands at +dx,+dy). The choice is
deliberate: floating dialog text is white-ish/cyan over the world, and a
black shadow ABOVE the colored text lifts the text away from a dark sky band
better than a shadow below it (which competes with the head silhouette).

> **Port note (load-bearing):** the snapshot's *two-pass* shadow (shadow font
> at `y-1`, then colored font at `y`) maps to ONE call in the modern port:
> `DrawTextShadowedToTarget`. But that primitive's built-in shadow is the
> retail UI **3-pass (base, +1x, +1y)** drop shadow at the *colored cell's*
> position — i.e. shadow at +1,+1 (down-right). It is NOT the snapshot's
> `y-1` up-shadow. The reconstruction must therefore either:
> 1. Replicate the snapshot's y-1 up-shadow with TWO `DrawTextToTarget`
>    calls (one black at y-1, one colored at y), bypassing the canonical
>    shadow primitive; OR
> 2. Use `DrawTextShadowedToTarget` and accept the +1,+1 down-shadow as the
>    port's deliberate normalization to the UI convention.
>
> Option (2) is the modernization-stance pick — float-text shadow becomes
> consistent with the rest of the HUD. Option (1) preserves the snapshot
> intent literally. Default to (2) unless visual-verify against the snapshot
> screenshot shows it looks wrong. This is **UNCONFIRMED-C** in §14.

**Drop shadow (anim).** None — the bitmap drains via plain `PutDim`/`ZPut`,
which don't apply any shadow effect. The blit-effect pipeline (UI_METHOD_MAP
§8) is not engaged for the anim path.

**Chroma key (anim).** Same as every retail HUD sprite: magenta `0xFF00FF` =
`0x7C1F` in RGB555 (UI_METHOD_MAP §16). The bitmap-decoder routes
`DM_TRANSPARENT` through the keycolor; the `DM_ALPHA` bit (set when
`bm->flags & BM_ALPHA`) routes through the real alpha channel (which is what
the spell-anim path uses). No special handling needed here — the renderer's
`DrawBitmap*` already implements both.

**Pink-halo bug** (NOMENCLATURE §4 / TPlyrStatusBar bug list) — irrelevant to
the post-char text path because the text is drawn at full resolution into the
framebuffer (not through a magenta-cleared scratch), so no anti-aliased edge
samples a magenta pixel. The pink-halo bug is a property of compositing text
on a magenta-cleared cache surface; this path does not use such a cache.

---

## §8 — Text rendering

Single text cell per entry. The cell rect is derived from the call args at
drain time (the queue stores the *anchor* (x,y), not a rect):

| string | cell (space, x,y,w,h) | font | px | color | h-align | v-align | shadow | format string | source value | cite |
|--------|-----------------------|------|----|----|---------|---------|--------|---------------|--------------|------|
| dialog speech (NPC) | screen: `(x - wrapwidth/2, y, wrapwidth, lineH*lines)` (centered horizontally around x; top of cell at y; wraps at `wrapwidth`) | `DialogFont` (id 101, WINFONT) = Times New Roman 20 | 20 | `{0,150,255}` (NPC) | center | top | y-1 black 1-pass shadow (snapshot) — see §7 port note | n/a — literal string from `doing->data` | the `ACTION_SAY` action's `data` field | `legacy/character.cpp:551, 557-579`; cell from `WriteText` semantics `legacy/surface.h:277-281` |
| dialog speech (player) | as above | `DialogFont` | 20 | `{200,0,0}` (red) | center | top | as above | n/a | as above | `legacy/character.cpp:559-564` |
| damage number (Phase B, not shipped) | screen: `(x, y, ?, lineH)` — TBD by Phase B emitter | candidate: `"Numbers"` (Arial 12 red) | 12 | `{255,0,0}` | center | top | per §7 port note | `"%d"` (caller formats) | the damage int | font.def:59; PLAN.md:128 (intent only) |

**Font flag decoding (FUN_004be2b0 conventions, UI_METHOD_MAP §5):** the
snapshot calls go through `TSurface::WriteText`, which itself wraps
`FUN_004be2b0`-equivalent (the `WriteText` wrapper in shipped retail). The
relevant `param_10` bits for `WriteText`'s JUSTIFY argument are:

- `JUSTIFY_CENTER (1<<1 = 0x02)` maps to `param_10 & 2` = h-center (UI_METHOD_MAP §5).
- `JUSTIFY_CLIP   (1<<3 = 0x08)` is a surface-layer "clip to surface clip rect"
  flag — NOT a `FUN_004be2b0` bit; the `WriteText` wrapper consumes it to
  early-reject text wholly outside the clip rect. Safe to map to "respect the
  active scissor" in the port.

**Shadow gate.** The snapshot does NOT set `font_id & 0x400` and does NOT set
`param_10 & 0x100` — instead it does the 2-pass shadow explicitly by drawing
the shadow font then the colored font. That means the canonical
`FUN_004be2b0` 3-pass shadow path is **not engaged** in the snapshot drain.
In the port, the modernization (§7 port note) consolidates to one
`DrawTextShadowedToTarget` call that DOES use the 3-pass shadow.

---

## §9 — Animation & dynamic behavior

**There is no per-entry animation inside the queue.** Each entry exists for
one frame and is then cleared. The visible "animation" is produced by the
emitter:

### Tween — spell anim frame index (driven by `TAnimAnimator`)

```
tween frame_index:
  from   0
  to     anim->NumFrames() - 1
  over   <per-anim duration, set by the animation's frame-rate / state>
  easing step (frame counter increments per-tick by the animator's frame
              advance code, not by this queue)
  drive  TObjectAnimator::Animate(draw==TRUE) being called every frame
  on-end animator-defined (loop / clamp / kill); when complete, the
         emitter stops calling AddPostCharAnim and the entry just isn't
         re-stamped next frame.
  cite   legacy/animimage.cpp:362-403 — the animator picks GetFrame(frame)
         each tick; nothing in the queue advances frame.
```

### Ramp — dialog speech text presence (driven by `TCharacter::Animate`)

```
ramp posttext_visible:
  range   stamp/no-stamp (a binary state, not a numeric ramp)
  step    n/a — re-evaluated each tick
  target  visible iff
            doing->action == ACTION_SAY AND
            doing->data != NULL AND
            doing->wait > 12        (half-second gate between sentences)
  maps-to one AddPostCharText call this tick with current (x, y-120)
          using current world->screen projection of pos
  on-end  silence (when wait <= 12, the call site stops firing)
  cite    legacy/character.cpp:549-579
```

### Phase B — rising damage number (TO BE ADDED, not in current code)

The expected pattern for a damage-number emitter (the Phase B goal), inferred
from the queue's per-frame stamp design:

```
tween damage_number.y:
  from   head_screen_y                            # WorldToScreen(target_pos).y
  to     head_screen_y - rise_distance            # ~ -60 px in screen-y over life
  over   rise_duration                            # ~ 24 ticks @ 24Hz = 1 second
  easing ease-out (decelerate at top)             # or linear; visual preference
  drive  per-character damage_number record (cur tick / max ticks)
  on-end kill (record removed, emitter stops re-stamping)
  cite   PLAN.md:128 (intent); NO SNAPSHOT CALLER — UNCONFIRMED-B

tween damage_number.alpha:
  from   255
  to     0
  over   rise_duration
  easing linear (or ease-in from 255 across last 25% only)
  drive  same record
  on-end kill
  cite   inferred; UNCONFIRMED-B
```

The emitter (a per-character "active damage numbers" list, NOT the post-char
queue) advances these tweens each tick and emits one `AddPostCharText("%d",
x, y_tween, color, wrap)` per active number per frame. The post-char queue
itself stays purely transient. The decision to add a fade requires the
emitter to drop the queue's character-color and instead per-tick fade
`color = {255, 0, 0} * (1 - age01)` — the queue stores RGB only, no alpha.

### Per-frame teardown

```
ramp queue.numpostcharanims / queue.numpostchartexts:
  reset    0 every frame, immediately after the drain
  drive    TPlayScreen::Animate(draw==TRUE) post-drain
  on-end   the queue is empty by the time any other code runs after Animate
  cite     legacy/playscreen.cpp:501-502 (zero counts);
           legacy/playscreen.cpp:495 (delete strdup'd text)
```

**Dirty/redraw model.** None. Every frame redraws every entry; entries do not
persist. No cache, no version counter.

---

## §10 — Input & dispatch

**Not applicable.** The queues are non-interactive — no hit rects, no
command ids, no modal state.

---

## §11 — Retail bugs NOT to reproduce

1. **Shared-cap bug (cosmetic, intentional?).** `numpostchartexts` is capped
   against `MAXPOSTCHARANIMS` not a separate `MAXPOSTCHARTEXTS`
   (`legacy/playscreen.h:113`). The two queues do NOT share storage, they
   just share the literal `30`. If retail intent was a separate text cap,
   the snapshot got it slightly wrong. Either fix is fine in the port; the
   simplest is to keep both caps at `30` and rename the constant for the
   text-side cap to `MAXPOSTCHARTEXTS` (alias of 30) so the per-queue intent
   is local. **Not a visual bug** — record but don't ship a fix as part of
   this reconstruction.

2. **`delete posttext[i].text;` should be `delete[]` for a `strdup` heap
   block.** `strdup` malloc's char[], but the snapshot uses `delete` (singular)
   on it (`legacy/playscreen.cpp:495`). On a strict CRT this is UB; on
   Windows MSVC's runtime it usually worked. In the port use `free()` (since
   we'd call `strdup`) or own the string in `std::string` to avoid this
   entirely. **Not a visual bug**; just a memory hygiene fix the port should
   silently apply.

3. **`MapPane.SetClipRect()` called twice around `DrawMouseShadow()`**
   (`legacy/playscreen.cpp:464, 480`). Indicates `DrawMouseShadow` mutates the
   active clip rect. In the port, scope the scissor with RAII (push/pop) so
   the drain code doesn't need to re-set it defensively.

4. **The y-1 up-shadow vs UI down-right convention** (see §7 port note +
   §14 UNCONFIRMED-C). This is a design inconsistency, not a bug per se;
   document and pick one for the port.

---

## §12 — Reconstruction pseudocode

The reconstruction owns two list-like buffers on `TPlayScreen` (concrete
types up to the implementor; the snapshot fixed-size array is fine, but a
`std::vector` reserved to 30 reads cleaner and obeys the "no raw fixed
arrays" style sweep). The destination is the active swapchain pass; the
canonical port primitives from UI_METHOD_MAP §12 do the drawing.

```cpp
// In TPlayScreen state:
struct PostCharAnim {
    int32_t x, y, z;
    PTBitmap bm;
    uint32_t drawmode;
    int32_t dim;
};
struct PostCharText {
    std::string text;       // owned by the queue
    int32_t x, y;
    SColor color;
    int32_t wrapwidth;
};
std::vector<PostCharAnim> postanim;     // reserve(30) in ctor
std::vector<PostCharText> posttext;     // reserve(30) in ctor
constexpr size_t MAXPOSTCHARANIMS = 30;
constexpr size_t MAXPOSTCHARTEXTS = 30; // separate const for clarity (snapshot used the anim const; see §11.1)

// Emitter API — keep the legacy signature exactly so existing callers compile:
void TPlayScreen::AddPostCharAnim(int32_t x, int32_t y, int32_t z,
                                  PTBitmap bm, uint32_t drawmode, int32_t dim)
{
    if (postanim.size() < MAXPOSTCHARANIMS)
        postanim.push_back({ x, y, z, bm, drawmode, dim });
    // else: silently drop; the snapshot does the same.
}
void TPlayScreen::AddPostCharText(const char* text, int32_t x, int32_t y,
                                  PSColor color, int32_t wrapwidth)
{
    if (text == nullptr || *text == '\0') return;
    if (posttext.size() < MAXPOSTCHARTEXTS)
        posttext.push_back({ std::string(text), x, y, *color, wrapwidth });
}

// Drain — called every frame inside TPlayScreen::Animate(draw==TRUE), gated
// by !InCompleteExclusion(), AFTER TScreen::Animate(draw) + AreaManager:
void TPlayScreen::DrainPostCharOverlays()
{
    // Scissor to map-pane viewport (RAII; restored on scope exit).
    auto clip = Renderer->ScopedScissor(MAPPANEX, MAPPANEY,
                                        MAPPANEWIDTH, MAPPANEHEIGHT);

    // 1) Anim drain.
    for (const auto& a : postanim) {
        if (a.drawmode & (DM_ZBUFFER | DM_ZSTATIC)) {
            // z-aware path. In the port, ZBuffer-tagged HUD overlays still
            // route through DrawBitmap on a frame-static layer at the
            // requested screen z; the renderer composites HUD after the
            // world so DM_ZSTATIC reduces to "draw on top of the world".
            Renderer->DrawBitmapZ(a.bm, a.x, a.y, a.z, a.drawmode);
            // ^ NB: if the renderer lacks a DrawBitmapZ HUD entry,
            //   DrawBitmap(bm, x, y) is functionally equivalent for DM_ZSTATIC
            //   (which is the only case used by animimage.cpp:399).
        } else {
            // Intensity-dimmed path: intensity = 31 - min(31, a.dim)
            // (snapshot Surface::PutDim, /surface.cpp:460-471).
            const int32_t intensity = 31 - std::min<int32_t>(31, a.dim);
            // The modern equivalent is a tinted draw with luminance scaled.
            // For dim==0 (the snapshot default and the only value the
            // shipping caller uses), this reduces to DrawBitmap.
            if (a.dim == 0) {
                Renderer->DrawBitmap(a.bm, a.x, a.y);
            } else {
                const float k = float(intensity) / 31.0f;
                Renderer->DrawBitmapTinted(a.bm, a.x, a.y, k, k, k, 1.0f);
            }
        }
    }

    // 2) Mouse shadow between anims and text (preserve snapshot z-order so
    //    the shadow sits in the map pane but doesn't obscure the cursor).
    DrawMouseShadow();

    // 3) Text drain.
    const SFontAtlas* dialogAtlas = BuildTTFAtlas("Times New Roman", 20);
    for (const auto& t : posttext) {
        // Choose: snapshot literal y-1 up-shadow (option 1, §7), or modern
        // UI-convention down-right shadow via DrawTextShadowedToTarget
        // (option 2). Default = option 2 (modernization-stance pick).
        //
        // Cell rect for h-center: width = wrapwidth, x_left = t.x - wrapwidth/2.
        const int32_t cellX = t.x - t.wrapwidth / 2;
        const int32_t cellY = t.y;
        const int32_t cellW = t.wrapwidth;
        const int32_t cellH = int32_t(TextLineHeight(dialogAtlas) * 99.0f); // numlines=99 cap
        DrawTextShadowedToTarget(
            dialogAtlas, t.text.c_str(),
            cellX, cellY, cellW, cellH,
            ETextAlign::Center,
            t.color.red / 255.0f, t.color.green / 255.0f, t.color.blue / 255.0f,
            Display.Width(), Display.Height());
    }

    // 4) Reset both queues — entries do not persist.
    postanim.clear();
    posttext.clear();
}

// Inside Animate:
void TPlayScreen::Animate(bool draw)
{
    if (dirty) return;
    TScreen::Animate(draw);
    AreaManager.Animate(draw);
    if (draw && !InCompleteExclusion()) {
        DrainPostCharOverlays();
        DrawOverhangs(true);     // UI chrome wins z-order against any stray overlay
    }
}
```

**Notes for the implementor:**
- Do NOT add a rise/fade tween inside the queue. The Phase B damage-number
  emitter, when added, will own its own per-number record with the tween
  state and submit `AddPostCharText` with the per-tick y and a per-tick
  alpha-modulated color.
- The `wrapwidth/2` h-center cell derivation assumes the caller's `(x,y)` is
  the *visual anchor* (the speech bubble's centerline). The snapshot caller
  satisfies this: it passes the iso-projected head x at `y - 120`. Don't
  re-anchor at the cell's TL.
- `JUSTIFY_CLIP` maps to "respect the active scissor" — the scoped scissor
  set in §3 already handles it; no additional clipping needed.
- The snapshot's `numlines=99` simply meant "no line cap"; the port's text
  primitive doesn't need a sentinel since it does TTF line breaking against
  the wrap width directly.

---

## §13 — Port mapping notes

| retail snapshot call | canonical port method | source |
|---|---|---|
| `Display->ZPut(x, y, z, bm, drawmode)` (anim with DM_ZBUFFER/DM_ZSTATIC) | `Renderer->DrawBitmap(bm, x, y)` for `DM_ZSTATIC` (since HUD is composited above the world the z is implicit); for true `DM_ZBUFFER` (z-mask sampling), this is rare on the post-char path — UNCONFIRMED-D in §14 | renderer.h:740-748 |
| `Display->PutDim(x, y, bm, drawmode, dim)` (anim default path) | `dim==0` → `Renderer->DrawBitmap(bm, x, y)`; `dim>0` → `Renderer->DrawBitmapTinted(bm, x, y, k,k,k,1)` with `k = (31-dim)/31` | renderer.h:756-762 |
| `Display->WriteText(text, x, y-1, 99, DialogFontShadow, &black, ...)` + `Display->WriteText(text, x, y, 99, DialogFont, &color, ...)` (two-pass shadow) | ONE call to `DrawTextShadowedToTarget(atlas, text, cellX, cellY, cellW, cellH, ETextAlign::Center, r,g,b, tw, th)` — applies the canonical UI 3-pass +1,+1 shadow (NOT the snapshot's y-1 up-shadow; see §7 port note, UNCONFIRMED-C) | font.h:143-146 |
| `MapPane.SetClipRect()` (set scissor to map-pane rect) | `Renderer->ScopedScissor(MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT)` RAII — restores on scope exit | port responsibility; consumes existing scissor support in the renderer |
| `BuildTTFAtlas("Times New Roman", 20)` for `"Dialog"` font | already provided | font.h:91 |
| `TTextBar` / chat — NOT to confuse with this path | irrelevant — TTextBar is a separate scrollable log; floating text is transient | TTextBar_SPEC.md |

**Needed primitives:**
- `Renderer->ScopedScissor(x,y,w,h)` — if not present, add (the existing
  `TSurface::SetClipRect` pattern in `mappane.cpp:3812` exists but is per-
  surface; the renderer-level scoped helper is the modern shape). UNCONFIRMED
  whether this primitive already exists on `TRenderer` — searching `src/renderer.h`
  shows no direct match (§14 UNCONFIRMED-E).
- `Renderer->DrawBitmapZ(bm, x, y, z, drawmode)` is the *snapshot's* `ZPut`
  shape. The port's HUD layer composites after the world so for `DM_ZSTATIC`
  this reduces to `DrawBitmap(bm, x, y)`. For `DM_ZBUFFER` (z-masking against
  the world depth buffer), no shipping caller in this path uses it (the
  animimage.cpp:396-397 branch routes those direct to `Display->ZPut`, NOT
  through `AddPostCharAnim`). Safe to drop the `DrawBitmapZ` requirement —
  every queued anim is `DM_ZSTATIC` in practice.

---

## §14 — UNCONFIRMED / open questions

**A. Where does shipped retail drain the queues?**
- What I tried: read the extracted retail `TPlayScreen::Animate`
  (`recon/discovered/cls_0x5a5320_TPlayScreen_Animate_47b4a0.cpp`) — trivial,
  no loop; the Pulse body (no loop); the bulk decompile
  (`recon/ghidra/cls_0x5a5320_likely_TCharacter.cpp:7772-7783` — same
  trivial body); the slot 4 DrawBackground summary (PlayScreenOverlay_SPEC.md)
  — autosave/loading only.
- Impact if wrong: significant. If shipped retail killed this design and the
  port re-implements the snapshot pattern, dialog/damage text will render in
  a place retail didn't — the test "could a stranger verify match?" fails.
  Visual-verify can still confirm the look matches a retail screenshot of an
  NPC speech bubble.
- Resolve: extract `FUN_0048ff00_TScreen_Animate` (the base TScreen::Animate
  the player override chains to) and look for a post-pane-iterate loop that
  walks two arrays of `{int x, int y, int z, void* bm, int drawmode, int dim}`
  records on a base-class field. Alternatively `DumpVtable.java` on TMapPane
  and look for an Animate override with an iterator over similar records.

**B. Damage-number caller — is the candidate font correct, and is there a
shipped emitter we missed?**
- What I tried: grepped `recon/discovered/` and `RevenantRevisited/legacy/`
  for `"Numbers"` font references — only the global font-init resolver
  (`FUN_00485870:00485e15` per `recon/classes_original/_data.txt:86504-86515`)
  references the literal "Numbers" string. No `AddPostCharText("%d", ...)`
  call site exists in snapshot src — and `ResolveImpact`'s flashred call is
  `#if 0` in both `character.cpp:1737-1759` and `charactersav.cpp:1642-1660`.
- Impact if wrong: PHASE B only, no impact on current port. The Phase B
  emitter is a NEW addition; choosing the wrong font is purely cosmetic until
  the gameplay caller is wired.
- Resolve: when Phase B lands, dump the retail combat damage-apply path
  (likely `TCharacter::Damage` = `cls_0x5b4f30_TPlayer_Damage_4c5810.cpp` and
  siblings) for any string-format + screen-position emission. If shipped
  retail added a damage-number system after the snapshot, it lives there.

**C. Shadow direction — y-1 up (snapshot) vs +1,+1 down-right (UI
convention).**
- What I tried: re-read snapshot `legacy/playscreen.cpp:486-489` for the
  literal `y-1` and `DialogFontShadow`; cross-checked NOMENCLATURE §4 and
  UI_METHOD_MAP §5 for the canonical UI shadow direction.
- Impact if wrong: text-shadow direction is visible; users notice. The
  modernization-stance default (option 2, +1,+1 down-right via
  `DrawTextShadowedToTarget`) is consistent with the rest of the HUD and is
  the recommended pick.
- Resolve: if a retail in-game NPC dialog screenshot is available, compare
  the shadow direction; if the retail look uses the y-1 up-shadow, implement
  option 1 (two explicit text calls bypassing the canonical primitive).

**D. `DM_ZBUFFER` vs `DM_ZSTATIC` in practice on the post-char queue.**
- What I tried: in `legacy/animimage.cpp:396-399`, `DM_ZBUFFER`-flagged
  bitmaps go DIRECT to `Display->ZPut` and skip the queue. Only the
  `DM_ZSTATIC` branch enters the queue (`:399`). So the queue's
  `DM_ZBUFFER|DM_ZSTATIC` test (`legacy/playscreen.cpp:467`) is in practice
  only ever testing `DM_ZSTATIC` for entries that came from
  `TAnimAnimator::Animate`.
- Impact if wrong: low — the drain still routes correctly via the boolean
  test; the question is only whether the port needs a separate
  `Renderer->DrawBitmapZ` for the `DM_ZBUFFER` case. From this trace: NO,
  it does not. The snapshot's defensive OR-test is harmless.
- Resolve: confirmed by trace.

**E. `Renderer->ScopedScissor` availability.**
- What I tried: grepped `src/renderer.h` for scissor / clip-rect APIs; the
  pattern in `mappane.cpp:3812` (`Display.SetClipRect(...)`) is per-display
  surface, not renderer-scope. The renderer-scope RAII wrapper for the
  composite pipeline doesn't show up in a quick scan.
- Impact if wrong: the port has to either add the primitive or use the
  per-surface clip rect (which works for surface-targeted draws but may not
  cover the swapchain HUD pass).
- Resolve: read `src/renderer.h` more carefully for an existing scissor /
  push-pop API, or ask UI-reconstruct phase to add `ScopedScissor` as the
  one new primitive this spec asks for.

**F. Per-frame ordering vs `DrawOverhangs`.**
- What I tried: re-read snapshot `legacy/playscreen.cpp:498` — `DrawOverhangs(TRUE)`
  is called AFTER both drains so UI chrome wins z-order. The shipped retail
  has dropped the bitmap-overhang chrome (it lives on real panes now). The
  port likely doesn't need the explicit `DrawOverhangs(TRUE)` call — the
  HUD pass composites the chrome later in the frame regardless.
- Impact if wrong: low — duplicated cosmetic work at worst. The port already
  comments out `DrawOverhangs` (`src/playscreen.cpp:1236` makes it a no-op).
- Resolve: confirmed dropped in port (`src/playscreen.cpp:1236`).

---

## Self-verify checklist (per FORENSICS_PROTOCOL §6)

- **Frame composition re-derived:** the queue's (x,y) is screen-space and is
  drawn directly inside the map-pane scissor. The emitters (`character.cpp:577`,
  `animimage.cpp:386`) compute it via `WorldToScreen`. No sub-surface
  composition step, no "chrome-local vs chip-local" trap.
- **Every coord tagged with its frame:** §3 + §4 explicit. The
  emitter's `-120 px` head-clearance is a screen-space delta applied at the
  call site, not a queue field.
- **Asset roster cites measured / authoritative source:** font.def for font
  identity; no bitmap is queue-owned (the spell-anim bitmap identity is the
  caller's responsibility per call).
- **Stranger test:** a competent UI programmer can implement
  `AddPostCharAnim`, `AddPostCharText`, and the per-frame drain from §6 + §12
  + §13 without re-opening recon. The Phase B damage-number emitter pattern
  in §9 is sketched, not specified, because no current code calls for it —
  the implementor adds that emitter when Phase B starts (PLAN.md:128), and
  this spec gives them the queue shape + the candidate font.
- **UNCONFIRMED honest:** §14 lists six items with retail-side gaps; A is
  load-bearing (where drain lives in shipped retail); B is Phase-B only; C
  is a design pick; D-F are low impact.
