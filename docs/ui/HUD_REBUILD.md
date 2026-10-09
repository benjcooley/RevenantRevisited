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

## 2. Where we start (survey 2026-10-07, main @ d876a3f)

- The live HUD is the `--test=ui-hud` harness. `TPlayScreen::Initialize`
  calls `InitializeUIHudMode()` and `Animate` calls
  `RenderUIHudModeEmbedded()`. Every pane is a `THudDrawable` in the
  anonymous namespace of a `src/ui*test.cpp`; none is a `TPane`.
- **No pane reads live game state.** The status bar draws a hard-coded
  "Locke 26" plus a synthetic "Vermis" target that cycles 5 s present /
  2 s absent *in the real game*. The potion shelf, quickspells, inventory,
  stats and map show demo data. Gameplay writes to `HealthBar`,
  `StaminaBar` and `TextBar`, which nothing displays. Mana goes to
  `StaminaBar`.
- `TTextBar` runs a scripted Print/SetHealthDisplay contract test at
  every game boot and draws a debug rect at the pre-release
  `TEXTBARX/Y`.
- **Text:**
  - Every HUD string except gold uses Arimo TTF at 10, 11 or 12 px,
    inconsistently.
  - Retail uses GDI WINFONTs (`font.def`: Small/Numbers = Arial 12,
    Med = Arial 16 Bold, Dialog/Large = Times New Roman 20,
    SpellTitle = Times New Roman 18).
  - Retail uses BMFONTs (`smallgold`, `medgold`, `scrlfont`, `sysfont`)
    for gold, scroll and system text.
- **Debug and tuning in the live path:**
  - env knobs `REVENANT_EQUIP_FACING` and `REVENANT_EQUIP_FREEZE`
  - "measured against retail" offsets (`kChipInset`, `kTargetBarXFix`,
    tab insets)
  - per-click log spam
- **Duplication:**
  - layout constants (188/60/…) in 9+ files
  - `LookupByName` copied 12×
  - hit-test geometry copied away from the painters; inventory paint and
    hit-test disagree at heights other than 480
  - mouse dispatch copied between `playscreen.cpp` and `testmodes.cpp`
- **Dead:** pre-release `statusbar.cpp`, `textbar.cpp` drawing,
  `equip/statpane/spellpane/inventory/automap/multictrl.cpp`,
  `SaveHudState`/`LoadHudState` (no callers).
- **No 640×480 Classic canvas.** Panes lay out in display pixels, with a
  mix of right-anchored and fixed-640 literals.

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
- **Drawing primitives** reproduce the retail draw modes (transparent,
  alpha, translucent, tint, fade) with nearest sampling and RGB565-exact
  results at Classic. The existing `DrawBitmap*ToTarget` family is
  cleaned up rather than replaced.
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
| **P1** | `TPlyrStatusBar`: player + target sides, bars (kernel `0x54a5d0`), chrome/rings/icons, portrait frame, fades `+0xd4/+0xdc` | zero non-text diff across stat sweeps × target present/absent × fade ticks |
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
