# HUD rebuild — plan of record

Started 2026-10-07. Owner: UI track (`feature/ui`, `worktrees/ui`).
Supersedes the HUD rows of [BURNDOWN.md](BURNDOWN.md) Phase B; the
per-pane `forensics/*_SPEC.md` files stay as the reading of the decomp,
corrected where the emulator shows otherwise.

## 1. Goal

1. **Classic HUD pixel-faithful to retail.** At 640×480, every HUD
   element matches what the shipped game draws: same pixels, same
   positions, same colors, in every state (stats, target present/absent,
   sidebar modes, hover/pressed, fades at each tick). Proven case by case
   against the original code running in the emulator, not by eye against
   JPEGs.
2. **Production code worth keeping.** Real `TPane` subclasses named for
   the retail classes, owned by `TPlayScreen`, reading live game state.
   No production code in `src/ui*test.cpp`, no demo data or synthetic
   drivers in the game, no env-var knobs or hand-measured offsets.
   Test modes become thin hosts over the production panes.
3. **Retail behavior.** Ramps and fades, sidebar mode cascade, hover,
   drag/drop and input dispatch through retail's command IDs.

Out of scope: Revisited (1920×1080) re-layout. It comes after Classic is
exact and builds on the same panes through anchors.

## 2. Where we start (GitHub main @ f049de4, 2026-10-07)

- **Live binding is done.** The gameflow track did it on 2026-10-05; see
  [HUD_LIVE_BINDING.md](HUD_LIVE_BINDING.md).
  - Every pane reads the main player (`Player`, retail `DAT_00667fcc`).
  - `src/uidemoplayer.*` builds a real demo `TPlayer` (and an opponent) for
    the `--test=ui-*` modes.
  - The `harness_*` inventory arrays are gone.
  - `TTextBar` is a production pane on `TPlayScreen` (149cc0a).
  - Its §5 lists the deviations still open: portrait fallback, paperdoll
    pose, spell icon key, quick-spell label wrap.
- **Still the test harness.** The pane code lives in the anonymous
  namespaces of `src/ui*test.cpp`, and `TPlayScreen` embeds the
  `--test=ui-hud` harness (`InitializeUIHudMode` /
  `RenderUIHudModeEmbedded`). Moving it into production pane classes and
  retiring the harness from `TPlayScreen` was explicitly left for this
  rebuild (HUD_LIVE_BINDING §4, gameflow ARCHITECTURE §9 step 4).
- **Pixels are off.** Panes are built from the decomp by reading, with
  measured fudges: `kChipInset`, `kTargetBarXFix`, tab insets, "≈" offsets.
  - The status bar loads the NoTex archives, while Classic is the
    texture-overlay path (§3).
  - The first emulator capture (2026-10-08) shows the port's chip 6 px low
    and 2–3 px right, with bars and the health tail off.
- **Text.**
  - Retail draws WINFONTs (FONT.DEF: Small/Numbers = Arial 12, Med = Arial
    16 Bold, Dialog/Large = Times New Roman 20, SpellTitle = Times New Roman
    18) with GDI `DrawTextA`.
  - The port draws them with Arimo/Tinos stb atlases: 2×2 oversampled and
    antialiased ([TEXT_RENDERING.md](TEXT_RENDERING.md)).
  - BMFONTs (`smallgold`, `medgold`, `scrlfont`, `sysfont`) are retail
    glyphs in both.
- **Debug and duplication in the harness files:**
  - env knobs `REVENANT_EQUIP_FACING` and `REVENANT_EQUIP_FREEZE`
  - per-click log spam
  - layout constants (188/60/…) repeated across files
  - `LookupByName` copied 12×
  - mouse dispatch copied between `playscreen.cpp` and `testmodes.cpp`
- **No 640×480 Classic canvas.** Panes lay out in display pixels.

## 3. What retail does (verified in the decomp 2026-10-07)

- **Two presentation paths.** `DAT_006680c8` is the NoTexOverlay flag.
  `FUN_004a7a20` sets it only for named problem cards (3dfx `0x121a`,
  Matrox `0x102b`, TI `0x104c`, S3 `0x5333` combos).
  - **Default 0 is the texture-overlay path most players saw. It is our
    Classic target.**
  - It uses `StatusBar.dat` / `SideBarTabs.dat`. The NoTex path uses
    `*NoTex.dat`.
  - The current port loads the `*notex.dat` archives. Fix this as each
    pane is A/B'd.
- **Composition.**
  1. Panes create texture surfaces through `TDDSurface::Initialize`
     `FUN_004a5740` (IDirectDraw4 `DAT_006695ac` → CreateSurface, vtable
     +0x18), plus "mosaic" surfaces `FUN_004bb5c0`, i.e. tiled textures.
  2. They compose bitmaps and text into those surfaces only when the
     source changes (cached stat values at pane +0x98.. are compared each
     paint).
  3. Each frame they draw the surfaces as textured quads with
     `FUN_00414d70` (mosaic walk) → `FUN_00414550` (3DScene.cpp: z,
     tint, mode bits → blend state).
  - `FUN_00414550` goes to the D3D device (`DAT_00668f14`) when
    `DAT_005d7a28 == 0`, otherwise to the original software rasterizer
    (`FUN_0056d260` begin, `FUN_0056d3a0` texture), the same raster the
    VFX probes already run.
- **Retail's two real configurations.**
  - **Hardware 3D:** texture overlays rasterized by the D3D device. This is
    the Classic target.
  - **Video memory rule:** the 3D device init (`FUN_004a75b0`) rounds
    video memory to whole MB and forces NoTex on cards with **8 MB or
    less**. Texture overlays therefore meant a 12–16 MB+ card; the emulator
    reports 16 MB.
  - **Software 3D:** chosen by the `BLUE`/SOFTWARE3D flags, or forced when
    the driver lacks `DDCAPS_3D`. DirectDraw init (`FUN_004a6bb0`) then
    forces `DAT_006680c8 = 1`, so the software mode always draws the NoTex
    2D HUD.
- **The emulator's HUD setup is a declared hybrid.**
  1. Retail boots for real, on a fake 16 MB 3D card whose Direct3D offers
     no texture-capable device (`slots/hud/README.md`).
  2. Retail then takes the texture-overlay path (`DAT_006680c8 = 0`) by its
     own rules. Every pane's composition (textures, text, tints, positions)
     is the hardware path's own code.
  3. Only the final quad draw is not retail's. Retail would fall back to
     its internal software rasterizer, which sets no texture stages. It
     draws overlay quads opaque whatever their tint, so chips never fade,
     and its texture step drifts a texel along a wide quad (measured
     2026-10-09).
     - The fixture takes over T3DScene's quad submit (`0x00414550`) and
       draws each quad as the device would (`slots/hud/overlayraster.py`):
       texels 1:1, modulated by the tint (blend mode 4: COLOROP and ALPHAOP
       MODULATE, `0x00417d60`), then alpha-blended.
  - Card-specific behavior (bilinear filtering, texel alignment via
    `TEXALIGN`, and probably the pink fringe) is a recorded renderer
    limit.
- **Text** is GDI. `FUN_004be2b0` (Surface.cpp) selects the font's HDC
  from `DAT_0065b020[font]`, draws with `TextOutA`/`DrawTextA`, then
  applies the 3-pass shadow / color handling. Imports present:
  `CreateFontA`, `TextOutA`, `DrawTextA`, `GetTextExtentPoint32A`,
  `GetTextMetricsA`, `SetTextColor`, `SetBkMode`.
- **Known retail bug: pink fringe.**
  - The 3D→2D overlay path draws pink fringe instead of shadows. The
    game-log area is the classic case; text and other elements show it
    too.
  - It comes from chroma key `0xf81f` bleeding through alpha edges, and
    likely also hardware bilinear filtering of keyed textures.
  - The A/B classifies magenta-family fringe in a retail capture as this
    bug and reports it. The port renders the intended black shadow.
  - The red/pink game-log colors in `sample_screen_1.jpg` are suspect;
    CLASSIC_HUD_REFERENCE §5c gets corrected once the emulator shows the
    real colors.

## 4. Method: A/B against the original code

Same shape as the gameflow track ([gameflow RETAIL_AB.md] on
`feature/gameflow`; emulator docs in the main checkout:
`docs/RETAIL_AB_TESTING.md`, `tools/retail_runtime/README.md`).

**Retail side: emulator slot `tools/retail_runtime/slots/hud/`** (main
checkout, alongside `gameflow/` and `combat/`). One persistent process:
setup once, checkpoint, restore per case.
- *Display environment.*
  - Original CRT init (`fixture.start`).
  - A fake IDirectDraw4 whose `CreateSurface` returns RAM-backed fake
    IDirectDrawSurface4 objects (Lock/Unlock/GetSurfaceDesc/...). These
    are implemented as hit; any other method fails explicitly.
  - The software-3D flag `DAT_005d7a28 = 1` and the overlay flag
    `DAT_006680c8 = 0`.
  - The original camera and raster tables, as in `software_probe.py`.
  - A 640×480 RGB565 screen plus z.
- *Assets through retail's own loader.* `resources.rvr` and
  `imagery.rvi` are mounted as virtual files. Retail's resource manager
  and `TMulti` load the `.dat` archives; no host-side decoding of retail
  bitmaps.
- *Fixture objects.* Player and target objects whose vtable slots the
  host answers (stats `+0x1c0/+0x1c8/+0x1d0/+0x354`, name, level,
  imagery), plus inventory, quickspell bindings and log lines. Everything
  comes from the case.
- *Text (staged).*
  - First, the GDI calls are a recorded boundary: font, string,
    position, color and flags are logged, and nothing is drawn.
  - Later (§6 P6), a GDI shim draws glyphs captured from real Win98 GDI
    in the DOSBox lab, so retail text pixels are exact.
- *Run.* The original pane ctor + `Initialize`, then paint at explicit
  ticks. The output is the 640×480 RGB565 capture, the per-pane rect and
  the text-call log.

**Port side: `Revenant --headless --test=ab-<pane> --ab-case=… --ab-out=F
--snap=F`.** A pane needs the GPU, so its A/B host is a test mode, not the
pre-engine `--retail-ab` dump (`src/uiplyrstatusbartest.*` for the first).
1. Build the production pane over real game objects (the demo player and
   its opponent) set up from the case (fills, the fight).
2. On a frame at a tick boundary (`--snapstep=0.03125`), stop the clock and
   run the pane the case's ticks.
3. Write the 640×480 frame (`--snap`). Write what the pane shows (names,
   levels, stats, the portraits' TBitmap bytes) as JSON (`--ab-out`). The
   retail side runs on exactly those inputs.

**Compare: `tools/retail_ab/hud_ab.py <pane>`.**
- Runs both sides over the case set, quantises both frames to RGB565.
- Holds each pixel to a tolerance the fixture's masks give:
  - exact by default;
  - one RGB565 step (9) where a quad's blend mixed (`blend.screen`);
  - two 4-bit steps plus that (43) where the pixel comes from a texel
    retail blended into its ARGB4444 texture in 4-bit steps
    (`blend.composed`, `slots/hud/blendmap.py`). The port blends in float.
- Masks retail's text cells until the emulator draws GDI glyphs (P6), and
  reports port text just outside a cell as text overflow.
- Writes per case a retail | port | diff triptych PNG and the counts, max
  deltas and defect bbox. Writes a report with the retail build hash, the
  port binary hash and the case set.
- The known-retail-bug class (pink fringe) is reported separately and
  never counted as a port defect.

**Acceptance per pane:** zero defects across its case set; after P6, zero
text overflow and text pixels compared too. Renderer limits found along the
way are documented, never tuned around.

## 5. Code shape (target)

- **Classes.** Each retail HUD class is one production class in `src/`,
  named for it.
  - Evolve the pre-release class where one exists: `TTextBar`,
    `TStatPane`, `TInventory`, `TEquipPane`, `TAutoMap`, `TSpellPane`.
  - Add the retail-only ones: `TPlyrStatusBar`, `TSideTabsPane`, bottom
    bar / bar-inventory, `TQuickSpellPane`.
  - Each lives in its own `.h/.cpp` with a `// REVSYNC:` header naming
    the retail class and functions it reproduces.
- **Ownership.** `TPlayScreen` creates the panes in retail's order as
  `TPane` children, the Tier 8 AddPane sequence. Layout constants live in
  one place: the pane that owns them, in Classic 640×480 coordinates.
- **Tick vs draw.**
  - `Pulse()` reads game state into the pane's model and advances ramps.
    Animation is time-based, sampled at tick boundaries for Classic A/B.
  - Paint composes the pane's surfaces only when the model changed,
    tracked by version counters as retail does with its cached values.
  - The renderer submits the overlay quads. Panes hold data; the
    renderer owns passes.
- **Data.** Panes read live objects (`::Player`, its target, inventory,
  spell bindings) through the same accessors retail uses. Gameplay's
  `HealthBar/StaminaBar/TextBar` writes are re-targeted to the real panes
  and the pre-release tube classes are removed.
- **Classic rendering: retail's visible result, by modern means.**
  The engine is GPU-only and modern, and we don't write poor code to copy
  retail's internals (user, 2026-10-08). Retail composes each pane's
  textures in software (ARGB4444 chrome and bars, 565 keyed text cells),
  then draws them as quads. The port does not emulate that pipeline:
  - Panes draw the same layers directly as ordinary sprites through the
    renderer (centralized rendering): backpanel, portrait, ring, icons,
    bar slices, text cells. They use retail's assets, in retail's order, at
    retail's exact positions, which the emulator records (e.g.
    `plyrstatusbar.py`'s primitive recipe).
  - Where retail fades a composite as one unit (a chip at fade < 6), the
    pane draws that group into a standard offscreen layer and draws the
    layer with opacity.
  - Retail details that change what the player sees, and are cheap and
    contained, are kept. Example: ARGB4444 decodes the way retail's raster
    shows it (colour channels shifted, red 15 → 30/31).
  - A/B acceptance:
    - exact: geometry (every element's position, size and layering) and
      opaque colours;
    - stated tolerance: blend rounding at alpha edges, where retail's 4-bit
      truncating compose and table blends differ from GPU float blending.
- **Revisited** can later feed the same panes full-precision assets.
- **Classic canvas.** The HUD renders into a 640×480 logical canvas,
  presented integer-scaled. The A/B renders that canvas offscreen 1:1.
- **Test modes** host the production panes over a fixture world built
  from the same case schema as the A/B; no paint code lives in them.

## 6. Phases

Each pane goes through the same loop:
1. retail cases
2. extract into the production class, shaped like the retail code
3. A/B to zero diff
4. live data
5. delete the test-hosted code
6. thin test mode

| Phase | Scope | Gate |
|---|---|---|
| **P0** | HUD slot (display env, loader, fixture objects, GDI boundary); port `--retail-ab=hud-*`; `hud_ab.py` | the retail status bar paints real pixels from `StatusBar.dat`; the port side round-trips one case; the triptych renders |
| **P1a** ✓ | Pane A/B harness: the port renders a pane over black at 640×480 from a case (headless test mode + readback); the compare checks geometry exactly and colour within the stated blend tolerance, and writes a retail / port / diff triptych | the status bar's current port output measured against retail |
| **P1b** ✓ | `TPlyrStatusBar` production class (compose recipe recorded from retail: see `slots/hud/plyrstatusbar.py`), fades, bars kernel `0x54a5d0` (slices verified against traces), player + target sides | zero non-text diff across stat sweeps × target present/absent × fade ticks |
| **P2** | Bottom bar: quickspell row, potion shelf + counts, end cap | same |
| **P3** | `TSideTabsPane` + the sidebar mode cascade (`DAT_0065d1b8/bc`) | same, plus hover/pressed states |
| **P4** | Sidebar panes: Stats, Equip, Spellbook, Inventory, Map, SpellCreate | same, per pane |
| **P5** | `TTextBar` game log | text calls match; colors settled (pink fringe) |
| **P6** | Text pixels: Win98 GDI glyph capture → emulator GDI shim; port Classic text strategy | zero diff including text |
| **P7** | Integration: `TPlayScreen` pane tree, live data, input via command IDs, delete `ui*test`-hosted HUD code, docs | full-HUD cases A/B clean; the game plays with the live HUD |

P0 and P1 are done in this worktree to prove the pattern. P2–P5 can then
fan out to sub-agents in their own worktrees off `feature/ui`, one pane
each, merged back after their A/B report is clean.

## 6a. P2 design: the bottom bar

**What retail draws** (measured with `slots/hud/bottombar.py`, 2026-10-09;
the cases' recipes name every bitmap). TBottomBarPane (`0x0065b638`) is a
TButtonPane with no buttons. Its slot 20 (`0x0052c800`) paints itself, then
the shelf (TBarInvPane `0x0065b028`, `0x0052ca70`), then the rings
(TQuickSpellPane `0x0065c6f8`, `0x005444c0`). All three share one rect: x 0,
y = display − 60, map-view width (452), 60 tall. `SetRect` `0x0052c930`
sizes all three. They draw in 2D, straight onto the screen, through
retail's own blitters: no overlay quads.

- **Bar** (`0x0052c880`): `UtilityBar` (640×60) *cropped* to the bar's width
  at (0, 0), not stretched. Then `BarEndCap` (10×60) at (w − 10, 0).
- **Shelf**: `BarInvBox` (42×42, mode 0x10) at (220 + 45k, 10) for
  k < (w − 220) / 45 (5 at 452 wide).
  - Items: the player's belt slots (`0x10b + n`), after a scroll offset.
    Each item draws its own inventory image at the box (item vtable
    +0x108), and its count when Amount (+0x198) > 1.
  - A "Pouch" also shows its first item's icon at 20×20 and that item's
    count. (Measured in P2b.)
- **Rings**: buttons "1".."4" (class `0x005b9c54`) at (10 + 50k, 10), 32×32
  hit rects, built from SpellIcons.dat RingU / RingD / RingG. Each frame the
  pane disables a ring (flag 4) unless the player has a quick spell there,
  the spell exists, and the player can cast it (`0x0051b7c0`). A ring's
  draw (`0x00542900`):
  1. restores the background around it: (x − 6, y − 10, 51×75);
  2. Puts the spell's icon (SpellIcons, its ICONNAME), magenta-keyed;
  3. Puts the ring with alpha: RingG disabled, RingD down, RingU otherwise.
     Down (flag 0x10000) shifts icon and ring by (+1, +1).
  4. Writes the spell's name in white "Small", centred, with no shadow.
     The first word goes in (x − 6, y − 10, 52 × 2 lines); the rest goes in
     (x − 6, y + 36, 52 × line + 4). A part over 9 characters shows its
     first 7 and "..". An empty ring shows only RingG.

**Port classes.** All three are production panes, drawn through the
engine's pane contract (§5):
- **`TButtonPane` composes.** It composes into one cached layer: a virtual
  background, then each visible button's `Compose`. It recomposes when a
  button or the pane goes dirty, and Draw submits the layer. This is the
  modern form of retail's draw loop (`0x00435de0`: dirty buttons drawn onto
  the pane's surface, each restoring its own background).
  - `TButton::Draw` (`Display.Put`) becomes `TButton::Compose` (to-target).
    All of the port's buttons are bitmap buttons; the 1998 generic frame
    path is dead and goes.
  - `TDialogPane` keeps its own Compose/Draw.
- **`TBottomBarPane`** (new, `src/bottombar.*`). It lays out against the
  map view and composes the bar. The shelf and the rings are its children
  (TPane hierarchy): retail's slot 20 paints them after it, and its
  SetRect sizes them. `TPlayScreen` adds it as one pane.
- **`TQuickSpellPane`** (evolves `src/spellpane.*`). Retail's four ring
  buttons replace the 1998 `TTalismanButton` strip; the quick spells are
  the player's (`TPlayer::GetQuickSpell`). Pulse sets each ring's
  disabled state. `TQuickSpellButton` composes icon, ring and label as
  above.
- **`TBarInvPane`** (new, `src/barinv.*`). It composes the boxes, and the
  items through the shared item cell (`TInvSlot`) once P2b has measured
  them.

The A/B is `hud_ab.py bottombar` over `--test=ab-bottombar`. Its cases vary
the bar's width (452, 640), the quick spells (empty, castable, not
castable, one word, long names) and, from P2b, the belt.

## 7. Status

- 2026-10-07: survey done; retail pipeline verified (§3); P0 started.
- 2026-10-08:
  - Retail boots for real in the emulator, from a 0.5 s boot image (HUD slot).
  - `plyrstatusbar.py` runs cases in about 2 s each: the capture, the
    primitive recipe of the last frame (Put / ParamBlit / Box / Quad / Text
    with arguments) and the text calls.
  - Rendering architecture decided (§5, P1a/P1b).
  - P1a, first pass:
    - The port's software blitters were empty shells (71 routines, x86 under
      `#if 0`). C++ bodies were written for the 4444 compose (500/500
      bit-exact against retail via `hud_ab.py draw-put`), then reverted:
      the port draws retail's layers directly with modern sprites (§5).
    - Kept: the retail formulas (§5, verified), the oracle `draw_ab.py`,
      the case generator, the `BM_ARGB4444` / `BM_ARGB1555` formats, and the
      4444 decode as retail's raster does it.
  - Findings, open P1a items (the HUD's own draws are in bounds and its
    surfaces even-width):
    - Retail surfaces *wrap* draws that cross an edge.
    - Retail's Put faults on odd-width surfaces with a DWORD-aligned pitch.
  - First light: retail's status bar paints (chrome, icons, bars).
  - GDI text calls are recorded.
  - §2 corrected after merging GitHub main (gameflow's live binding).
- 2026-10-09: **P1a and P1b done.**
  - `TPlyrStatusBar` (`src/statusbar.{h,cpp}`) replaces the 1998 tube bars
    and the harness. `TPlayScreen` hosts it after the text bar.
    `--test=ui-plyrstatusbar` and `--test=ab-plyrstatusbar` are thin hosts
    over it.
  - `hud_ab.py statusbar`: 20/20 cases with no defects. Every opaque pixel
    is exact; blends are within 9 (screen) and 41 (composed).
  - What the A/B found and fixed:
    - `T3DImagery::GetInvImage` answers with the state-0 icon whatever the
      state (`0x0040ce60`). Characters now have portraits; there is no
      LockeFace.
    - The portrait goes through a magenta-keyed 565 scratch into the
      ARGB4444 chip: black stays opaque and colour is 4-bit
      (`EBitmapDecode::Overlay4444`).
    - The fade alpha is retail's integer `fade * 255 / 6`.
  - Emulator findings:
    - Retail's software rasterizer ignores a quad's tint alpha and drifts a
      texel on wide quads. The fixture now draws quads as the D3D device
      does (§3).
    - Retail's 555 -> 565 widens green by a shift; 565 -> ARGB4444
      truncates (Put oracle).
  - Open, for P6:
    - Retail's "Small" is `CreateFontA(+12, "Arial")`: a 12 px cell, about
      9 px em. The port's "Small" atlas is 12 px by stb's
      ScaleForPixelHeight, about 10.7 px em.
    - `font.cpp`'s `kGdiTopLeading = 2` puts glyphs 2 px above the cell top,
      where GDI's DT_TOP never draws (the A/B's text overflow).
    - Both change every pane's text, so they get settled together against
      Win98 GDI captures.
  - Open, elsewhere:
    - `TDialogPane` takes its Ring from `statusbarnotex.dat`. Retail uses
      `DAT_0065a9d0`, which is `StatusBar.dat` on the Classic path.
    - `invslot.cpp`'s probe of other states for an icon is redundant now
      that GetInvImage follows retail.
