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
  1. Retail boots for real in `BLUE` mode.
  2. Before the HUD panes initialize, the fixture selects the
     texture-overlay path (`DAT_006680c8 = 0`). Every pane's composition
     (textures, text, tints, positions) is then the hardware path's own
     code.
  3. Only the final 1:1 quad raster runs on the internal rasterizer instead
     of the D3D device.
  - The hardware rasterizer's own behavior (bilinear filtering, texel
    alignment via `TEXALIGN`, and probably the pink fringe) is a recorded
    renderer limit, the same treatment the VFX A/B gives the software
    raster.
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

**Port side: `Revenant --retail-ab=hud-<pane> --ab-cases=F --ab-out=F`**
(`src/retailab.*`, same CLI as gameflow).
1. Build the production pane over real game objects configured from the
   case (a `TPlayer` with those stats, …).
2. Render the HUD canvas offscreen at 640×480 1:1.
3. Read it back, quantize to RGB565, and write the capture plus its
   text-call log.

**Compare: `tools/retail_ab/hud_ab.py <pane>`.**
- Runs both sides over the case set.
- Writes per case: a retail | port | diff triptych PNG, the differing
  pixel count, the diff bbox, the max channel delta, and text-call
  differences.
- Writes a report with the retail build hash, port commit and case-set
  hash.
- The known-retail-bug class (pink fringe) is reported separately and
  never counted as a port defect.

**Acceptance per pane:** zero differing pixels outside text across its
case set, matching text calls; after P6, zero differing pixels overall.
Renderer limits found along the way (e.g. ARGB4444 quantization) are
documented, never tuned around.

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
- **Classic rendering = retail's 2D raster plus quads.** Measured in the
  emulator (2026-10-08):
  - Retail composes each pane's textures in software: chrome and bars in
    **ARGB4444**, text cells in **RGB565** keyed on magenta `0xf81f`.
  - It uses its draw-routine selector (`FUN_004ad1d0`, under `Draw`
    `FUN_004ad0d0`) over draw modes (default, transparent `0x100`, alpha
    `0x2000`, translucent, fade, change-colour) and destination formats
    (565/555/4444/1555/32).
  - It then draws the textures as 1:1 quads.
  - The port matches this the same way:
    - Panes compose into 16-bit CPU surfaces with the **port's existing
      `TSurface`/`graphics.cpp` software blitters** (the 1998 ancestors of
      retail's), retail-synced and extended with the 4444/1555
      destinations.
    - Composition happens only when content changes, as retail does.
    - The GPU draws the finished textures as nearest-sampled quads, with the
      tint alpha. Opaque texels come out exact; alpha-blended edges are
      exact to ±1 LSB (a recorded renderer limit).
  - Each draw routine is A/B'd as a unit against retail's own `Draw` in the
    emulator (crafted bitmaps, buffers and modes), before any pane uses
    it.
  - Revisited can later feed the same panes full-precision assets.
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
| **P1a** | Retail 2D raster parity: `graphics.cpp` `Draw` selector + the routines the status bar uses (Box fill; Put default/transparent and alpha into 565 and 4444; surface-from-bitmap into 4444; 4444/565 copies; keyed 565 blit), retail-synced, unit A/B in the emulator | every routine bit-exact on randomized inputs |
| **P1b** | `TPlyrStatusBar` production class (compose recipe recorded from retail: see `slots/hud/plyrstatusbar.py`), fades, bars kernel `0x54a5d0` (slices verified against traces), player + target sides | zero non-text diff across stat sweeps × target present/absent × fade ticks |
| **P2** | Bottom bar: quickspell row, potion shelf + counts, end cap | same |
| **P3** | `TSideTabsPane` + the sidebar mode cascade (`DAT_0065d1b8/bc`) | same, plus hover/pressed states |
| **P4** | Sidebar panes: Stats, Equip, Spellbook, Inventory, Map, SpellCreate | same, per pane |
| **P5** | `TTextBar` game log | text calls match; colors settled (pink fringe) |
| **P6** | Text pixels: Win98 GDI glyph capture → emulator GDI shim; port Classic text strategy | zero diff including text |
| **P7** | Integration: `TPlayScreen` pane tree, live data, input via command IDs, delete `ui*test`-hosted HUD code, docs | full-HUD cases A/B clean; the game plays with the live HUD |

P0 and P1 are done in this worktree to prove the pattern. P2–P5 can then
fan out to sub-agents in their own worktrees off `feature/ui`, one pane
each, merged back after their A/B report is clean.

## 7. Status

- 2026-10-07: survey done; retail pipeline verified (§3); P0 started.
- 2026-10-08:
  - Retail boots for real in the emulator, from a 0.5 s boot image (HUD slot).
  - `plyrstatusbar.py` runs cases in about 2 s each: the capture, the
    primitive recipe of the last frame (Put / ParamBlit / Box / Quad / Text
    with arguments) and the text calls.
  - Rendering architecture decided (§5, P1a/P1b).
  - **P1a started.** The port's software blitters were empty shells: 71
    routines whose 1998 x86 assembly was disabled under `#if 0`, with no
    C++ body. New C++ bodies so far:
    - the 2-byte `Put` copy and `Box` fill;
    - the new `Alpha4444` (retail `0x004b3790`, formula recovered and
      verified);
    - the `BM_ARGB4444` / `BM_ARGB1555` formats.
  - `tools/retail_ab/hud_ab.py draw-put`: **500/500 random cases
    bit-exact** against retail's own `TSurface::Put` (real StatusBar.dat
    bitmaps plus synthetic 4444; default, plain and alpha modes).
  - Findings, both open P1a items (the HUD's own draws are in bounds and
    its surfaces even-width):
    - Retail surfaces *wrap* draws that cross an edge (clip mode).
    - Retail's Put faults on odd-width surfaces with a DWORD-aligned
      pitch.
  - First light: retail's status bar paints (chrome, icons, bars).
  - GDI text calls are recorded.
  - §2 corrected after merging GitHub main (gameflow's live binding).
