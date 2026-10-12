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
| **P2** ✓ | Bottom bar: quickspell row, potion shelf + counts, end cap | same |
| **P3** | `TSideTabsPane` + the sidebar mode cascade (`DAT_0065d1b8/bc`); the hover from pointer moves (TSideTabsPane_SPEC §9, not wired in play yet) | same, plus hover/pressed states |
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
- **Shelf**: `BarInvBox` (42×42, mode 0x10: opaque) at (220 + 45k, 10) for
  k < (w − 220) / 45 (5 at 452 wide).
  - Items: the player's belt slots (`0x10b + n`). (The pane has a scroll
    offset, +0x88, that only Initialize writes, with 0.) Each item draws
    its inventory icon at the box's top-left (item vtable +0x108, mode
    0x110); one with an inventory animation draws the frame
    `GameFrame % n` in Animate (`0x0052cd80`) instead.
  - Its amount, when over 1: "%d" in "Numbers" (FONT.DEF Arial 12, red),
    shadowed, right-aligned in (x, 10, 40, 2 lines).
  - A "Pouch" also shows the item in its slot 0 halved by the 2:1 reduction
    (`0x004a31a0`) at (x, 30), and its item count in white "Small",
    shadowed, in (x + 20, 36, 20, line + 2).
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
- **`TBottomBarPane`** (new, `src/bottombar.*`). Its host places it
  (`Place`, retail's SetRect) and it composes the bar. The shelf and the
  rings are its children (TPane hierarchy): retail's slot 20 paints them
  after it, and its SetRect sizes them. `TPlayScreen` adds it as one pane
  and places it along the bottom of the map view while the HUD's lower
  panel is open (`UpdateBottomBar`).
- **`TQuickSpellPane`** (evolves `src/spellpane.*`). Retail's four ring
  buttons replace the 1998 `TTalismanButton` strip; the quick spells are
  the player's (`TPlayer::GetQuickSpell`). Pulse sets each ring's
  disabled state. `TQuickSpellButton` composes icon, ring and label as
  above.
- **`TBarInvPane`** (new, `src/barinv.*`). It composes the boxes and the
  items through the shared item cell (`TInvSlot`), recomposing when what a
  box shows changes.

The A/B is `hud_ab.py bottombar` over `--test=ab-bottombar`. Its cases vary
the bar's width (452, 640), the quick spells (empty, castable, not
castable, one word, long names), and the belt: still and animated items at
several game frames, pouches, amounts (ammo, money), a pouch holding a
stack.

## 6b. P4 design: the inventory

**What retail draws** (measured with `slots/hud/inventory.py`, 2026-10-10).
TInventory (`0x0065d4f8`, vtable `0x005a58c0`) is a TButtonPane in the side
panel's lower slot, (452, 306), 188×174, sharing it with the automap and the
spell pane. Its art is Inventory.dat (`0x0065c130`). Slot 20 (`0x00537a30`)
repaints the grid (slot 21, `0x00537a70`) when dirty, then the buttons;
Animate (`0x00537f90`) draws the animated items.

- `Inventory` (188×174) at (0, 0), opaque.
- The container's icon, 20×20 at (10, 10), keyed on 0 (`0x00438df0`,
  DM_TRANSPARENT): `Backpack` for the player's own inventory; for a bag,
  the bag's inventory icon reduced to 20×20 when the pane opens it
  (`0x00538e40`). That reduction is not the shelf's: the icon goes on a
  black 40×40 surface, every 2×2 block is averaged unkeyed (red and blue
  the floor of the mean, green half the sum of the top five bits) and what
  comes out black is keyed (`EHalving::BoxOverBlack`).
- The player's gold, `GetInventoryAmount("Gold")`, as `"%d$"` in the
  "Gold" bitmap font (retail's font 9; the forensics' "GoldMed" was wrong),
  left-aligned at (80, 13) (the font draws `$` as "gp"); "0$" with no gold.
  Then `GoldPile` (32×32) at (40, 5) with alpha. The pile is a fixed
  bitmap, not a stack.
- A bitmap font hangs a line from its 'A': the font table gives it
  StartHeight('A') as its ascent (`0x004ac990`), and a glyph's top is at
  y + ascent − StartHeight(ch). Each glyph is one blit (`0x004b5820`) in
  the font's draw mode, DM_TRANSPARENT | DM_ALIAS: the keyed data and the
  alias buffer's edge pixels, each at its own coverage.
- Twelve cells at (8 + 45·col, 42 + 44·row), column-major: slot
  first + col·3 + row. Each is the shared item cell, exactly the shelf's
  recipe (`TInvSlot`): the icon, the amount over 1 in "Numbers", a pouch's
  slot-0 item halved with its count in white "Small".
- Two arrow buttons, "scrollleft" at (140, 11) and "scrollright" at (161, 12),
  24×24 with alpha (DM_USEDEFAULT | DM_ALPHA: DM_USEDEFAULT adds the
  bitmap's mode to the flags, it doesn't replace them), from
  `InvArwL{U,D,G}` / `InvArwR{U,D,G}`. A click moves
  the first slot one column (3), within 0..243 (`0x00537520`); an arrow that
  can't move is grey.
- The pane's container (+0x17c) is the main player's, set as the player
  becomes main (`0x0051eef0`, on the play screen only); SetContainer
  (`0x005391a0`) builds the bag icon and goes back to the first page.
  Clicking a bag opens it, clicking the container icon climbs back to its
  owner.

**Port.** `TInventory` (`src/inventory.*`) evolves into retail's pane: a
`TButtonPane` with the two arrows, twelve `TInvSlot` cells and the art
above; it owns its first slot (retail +0x188), which the side panel's drag
hit-tests read until the drag moves into the pane. `TPlayScreen` hosts it in
the lower slot while the side panel shows the inventory. The A/B is
`hud_ab.py inventory` over `--test=ab-inventory`: gold amounts, stacks and
pouches, animated items, pages, an opened bag.

## 6c. P4 design: the spell pane

**What retail draws** (measured with `slots/hud/spellpane.py` and the
disassembly, 2026-10-10). TSpellPane (`0x006661b0`, vtable `0x005a5978`)
is a TButtonPane in the side panel's lower slot, (452, 306), 188×174:
the player puts a spell together from the talismans in the spell pouch and
casts it. Its art is SpellPane.dat (`0x0065a574`); the talismans' plates
and icons are GameData's (playscrn.dat). Slot 20 (`0x005435c0`) repaints
the background (slot 21, `0x005435f0`) when dirty, then the buttons.

- `spellconstr` (188×174) at (0, 0), DM_BACKGROUND: unkeyed.
- The talismans in the player's "spell pouch" (the lookup of "spellpouch"
  after it is thrown away, retail's bug as in the snapshot), in talisman
  type order, from (33, 10): each a `%scandy` plate (28×26) and its icon
  (24×21), keyed, each first in black 2 px right (the plate) or 1 px right
  and down of it (the icon) -- a Put with a colour, DM_CHANGECOLOR. Plate
  at (x, y − 2), icon at (x + 2, y + 1); the pressed talisman 1 px down
  and right. With names on, the name (the dialog list's line for it, else
  the type name) in "Small", white, shadowed, at (x + 29, y + 10).
  The next goes 30 px right, 60 with names; past x 130 it wraps to
  33, 30 px down. Rows from y 126 are not drawn. With names on, the
  talismans before the first line (+0x180) are skipped.
- Buttons (`0x005432a0`): "spell" (24, 136) 142×36, SpellInvD/U, the
  talisman button (vtable `0x005b9bf0`): the construct spell (quick spell
  0) as glyphs `S<Talisman>` (32×32, DM_ALPHA) from x + 7 + (5 − n)·14,
  25 px apart, at y + 5, 1 px down while pressed (`0x005424a0`); "back"
  (164, 143) 22×20, SpellBackD/U, key VK_BACK; "up" (166, 82) and "down"
  (166, 112) 20×26, SpellArwU*/D*, repeating at 8; "min" (166, 37)
  18×18, SpellNameD/U with names on, SpellTalD/U off.
- Up shows with names on and a first line ≥ 1; down with names on and more
  than six talismans from the first line; hidden, they are released.
- A button's draw mode is its own (+0xd4); the default takes the bitmap's
  without DM_BACKGROUND / DM_USEREG, with DM_NORESTORE (`0x0042cc30`). So
  these buttons are unkeyed: keying follows DM_TRANSPARENT, here too.

**What it does.** A press on a talisman (x 33..162, y 10..135, the same
layout, 30×30 cells) picks it; moving off and back follows it; the release
on it adds its code to the construct spell (`0x00543b40`, `0x00543c40`,
`0x00543ca0`) -- at most 5; if the player can't cast the result
(`0x0051b7c0`) the talisman comes off again and the text bar says
SPLOUTTAL (`0x00542640`). Only while the player has control. "spell" casts
the construct spell (`0x0051b5d0`) and has the spellbook find it by its
talismans (`0x00544fb0`); "back" drops the last talisman; up and down move
the first line by two, not below 0; "min" toggles the names.

**Port.** `TSpellPane` (`src/spellpane.*`) becomes retail's pane, shaped
like TInventory: Pulse binds the pouch's talismans and the construct
spell, marking the pane dirty on a change and setting the arrows and the
names art; ComposeBackground draws from that; the layout is one function
for the draw and the hit test. `TTalismanButton` draws the bound spell.
`TButton` takes retail's draw-mode rule. `TPlayScreen` hosts the pane in
the lower slot while the side panel shows the spell tab. The A/B is
`hud_ab.py spellpane` over `--test=ab-spellpane`: no pouch, an empty one,
a few talismans and all twelve with names on and off, first lines, spells
of 0..5 talismans, a pressed talisman and a pressed "spell".

## 6d. P4 design: the automap

**What retail draws and does** (`docs/ui/forensics/AutoMapPane_RETAIL.md`,
read from the disassembly; the frame confirmed with `slots/hud/automap.py`,
2026-10-10). TAutoMap (`0x0065b4f0`, vtable `0x005a5658`) is a TPane in the
side panel's lower slot, (452, 306), 188×174, drawn entirely through
T3DScene's overlay quads (`0x00414d70`, point-sampled: TEXTUREMAG/MIN
NEAREST at `0x00414ab0`) in the texture pass (slot 7, `0x0052a5f0`):

1. The map body (`0x0052a830`): a 320×320 mosaic holding the 5×5 ring of
   64×64 module tiles (`<module>\automaps\<level>_<x>_<y>.bmp`, each in its
   slot of the torus, a missing one black), scaled by zoom · 144 / 256 and
   drawn four times over, clipped to the 144×144 window at (20, 18).
2. With fog of war on (`0x005d7a6c`, toggled by `fow`): the living, visible
   characters within 512 px of the player as 4×4 markers from the Marker
   sheet (frame across, every five pulses; player / enemy / other down),
   then the fog (`0x0052b520`): a 10×10 grid of black vertices, alpha the
   share of unexplored 128 px cells each samples, drawn as Gouraud
   triangles blended by alpha.
3. The frame (`0x0052bdb0`): `Amap` at (0, 0); `PlusSel` at (8, 3) while
   "+" is held, `MinusSel` at (144, 3) while "−" is. (The forensics'
   "−0x70" was 0x90 read as a signed char; the harness drew both always,
   "−" at x −112.)

Each pulse (`0x0052a280`): ±10 zoom while a button is held, 100..300; the
window follows the camera (the map view's centre ÷ 8), or keeps its centre
through a zoom once dragged off it; the tile ring and the fog catch up.
After the panes pulse, RecordTravels (`0x0052b9f0`) moves the ring with
the scroll and, when the player moved, explores ±512 px around the player
in the level's visgrid, which lives in the player's automap record
(TPlayer::automap, saved with the player). Input (`0x0052c1a0`,
`0x0052c2c0`): "+" / "−" held to zoom; a press in the ring and a drag
scroll the map a mosaic px a pointer px; a right release returns to the
camera.

**Port.** `TAutoMap` (`src/automap.*`; the 1998 pane is in
`attic/src/automap.*`) is retail's pane on the GPU: the mosaic is a
nearest-filtered render target stamped a tile at a time
(`TSurface::ContinuePass`), the body four stretched quads under a target
clip (`TRenderer::DrawSurfaceStretchedToTarget`, `SetTargetClip`), the fog
coloured triangles (`DrawColoredTrianglesToTarget`), all moved half a pixel
for D3D7's integer pixel centres. `TAutoMapGrid` is the visgrid with its RLE
codec. `TPlayScreen` hosts the pane in the lower slot for the map tab and
calls RecordTravels after its pane pass; `BindMainPlayerToScreen` binds the
main player's record (`0x0052c3d0`) and the player's save folds the shown
level in first (`0x0052c5c0`). `hud_ab.py automap` compares the frame with
the buttons released and held.

## 6e. P4 design: the spell book

**What retail draws and does** (`docs/ui/forensics/SpellBookPane_RETAIL.md`,
from the disassembly; the paint confirmed with `slots/hud/spellbook.py`,
2026-10-10). The spell book (`0x0065a9d8`, vtable `0x005a5ae8`) is a
TButtonPane in the side panel's upper slot, (452, 0), 188×306: the main
player's known spells (player +0x2ec, talisman codes, in the order
learned), down a scroll of parchment. Its art is SpellScroll.dat
(`0x00666640`); the circles and rings are SpellIcons.dat (`0x0065bc3c`).

- The paint (`0x005452d0`): `Scroll` at (0, 0), unkeyed. The rows go to a
  148×229 surface cleared to the key, blitted keyed to (18, 47): first the
  80 px `ScrollPaper` tiles from −(scroll mod 80), then each visible row.
  Then `ScrlTop` at (79, 45) and `ScrlBottom` at (75, 261), keyed.
- A row is `lines × 12 + 91` px tall, `lines` the description's word-wrap
  count in "Small" across 132 px (the rebuild, `0x00546830`). From its top:
  the spell's circle (ICONNAME, else "Aura"; magenta-keyed) and its ring at
  (12, 0), the ring RingU while the player holds the talismans and has the
  mana, RingD when the circle is also pressed (both shifted −2, −2), else
  RingG; the variant's name split at its first space, each part over 10
  characters cut to 8 and "..", in "SpellTitle", white, centred in
  (54, 4 / 16, 85, 86) (a one-word name at y 10); the description, the
  dialog line `SPELL<code>`, at (10, 44, 132, desc h) in RGB(242, 231,
  255); "Skill: n" / "Mana: n" (the variant's skill level and cost) at
  (10 / 90, desc h + 44), cyan and yellow-green; the talisman glyphs
  `S<Talisman>` 16 px apart from (12, desc h + 61), DM_ALPHA. Each part is
  drawn only while it can reach the window. Every text is a 0x004be2b0
  compose with the 3-pass shadow; SpellTitle's starts at (2, 1).
- The arrows (169, 150) / (169, 174), 24×24, overhanging the pane by 5 px,
  DM_USEDEFAULT | DM_ALPHA, step one row to the window's top (`0x00544f00`).
  The scroll moves toward its target at most 40 px a pulse, and not again
  until the book has been painted (`0x00546620`); dragging the parchment
  adds to the target. A press on a circle and the release on it casts the
  spell (fizzling with SPLMISTAL / SPLCASTFAIL, SPLCASTOK on success); a move
  with the button held picks the spell up for the quick-spell rings at
  once, its circle under the pointer at (20, 20); a spell let go on the book
  scrolls to its row. Casting in the spell pane learns the spell and scrolls
  to it (`0x00544fb0`), as does `abracadabra` for every spell.
- Retail repainted only the strip a scroll step exposed (the rows' surface
  wraps), so rings elsewhere went stale. The port recomposes the window.

**Port.** `TSpellBookPane` (`src/spellbook.*`, global `SpellBook`) is
retail's pane on the GPU: the window is a scissor rect over the pane's
layer, so the keyed rows surface isn't needed. Its rows are a cache of the
main player's list, rebuilt when the player or the list's version
(`TPlayer::KnownSpellsVersion`) changes; a ring that changes recomposes the
book. Spell drags carry the code (`SUIDragState::spell`,
`UIDragState::BeginSpellDrag`); the rings take it. `TPlayScreen` hosts the
book in the upper slot for the book tab. SpellIcons.dat is shared with the
rings (`AcquireSpellIcons`). `hud_ab.py spellbook` over
`--test=ab-spellbook`: empty and grey books, castable, mana-short and
pressed rings, truncated names, scroll positions and the clamp. Its text is
compared structurally: every compose's string, font, rect, alignment,
colour, shadow and wrapped lines against the port's recorded draws
(`SetTextDrawRecorder`).

## 6f. P4 design: the stats

**What retail draws and does** (`docs/ui/forensics/StatPane_RETAIL.md`, from
the disassembly; both pages confirmed with `slots/hud/statpane.py`,
2026-10-10). TStatPane (`0x0065b140`, vtable `0x005a5ba0`) is a TButtonPane
in the side panel's upper slot, (452, 0), 188×306. Its paint (`0x005491c0`)
puts StatsPane.dat's `Stats` and then runs statpane.def (resources.rvr's,
read packs first): a block picked by the bound object's class and the format
("page1" / "page2"), whose statements move a cursor (POS, MOVE, TAB,
NEXTLINE), set colours, and draw TEXT and FIELDs with GDI TextOutA in
"Small", each with the 3-pass shadow, the cursor following GDI's current
position (TA_UPDATECP: left text moves it right, right-aligned text leaves it
at its left edge). FIELDs resolve through the pane (`train<attribute>`: "+"
for the level-up picks; `objdesc`, `type`, `stmod...`) and then the object's
own field text (vtable +0xc8: TPlayer's class, TCharacter's armor, max
health / fatigue / mana, attack and defense %, damage, stealth, the stats by
name). The shipped DEF's stat-modifier columns name no field, so they never
draw. BUTTONs are invisible hot spots, made when the block changes: "page1" /
"page2" switch the format, train<attribute> picks it for the next level
(`TPlayer::TrainStat`, 0x0051fe50; STATMAX past 30). A right press shows
the player again. The examined-object formats (Character, Weapon, Armor,
Default) add the portrait (ICON) and a description paged by Prev / Next.

**Port.** `TStatPane` (`src/statpane.*`; the 1998 skill-tree pane is in
`attic/src/statpane.*`) parses statpane.def once into blocks of statements
and runs the selected block on each compose, its text through the canonical
helpers. The hot spots are rebuilt when the block changes
(`TButtonPane::ClearButtons`). Retail repainted on the bound object's
stat-changed notification, which the port lacks; the pane recomposes when
anything it shows changes. `TCharacter::GetFieldText` gains attackpct,
defensepct, damage and stealth (the getters named in the spec, retail's
quirks kept). `TPlayScreen` hosts the pane in the upper slot for the stats
tab. `hud_ab.py statpane` over `--test=ab-statpane`: both pages, the
level-up marks, wide values, and clicks on the hot spots. The port reports
what the fields read (the player's stats by name, the combat getters by
retail slot, max health / mana, the class) and the fixture answers with
it, so the A/B checks the interpreter, the formulas and the layout. The text
is compared structurally against retail's GDI calls (`gdi_draws`).

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
    - `ttfatlas.cpp`'s `kGdiTopLeading = 2` puts glyphs 2 px above the cell top,
      where GDI's DT_TOP never draws (the A/B's text overflow).
    - Both change every pane's text, so they get settled together against
      Win98 GDI captures.
  - Open, elsewhere:
    - `TDialogPane` takes its Ring from `statusbarnotex.dat`. Retail uses
      `DAT_0065a9d0`, which is `StatusBar.dat` on the Classic path.
- 2026-10-09: **P2 done.**
  - `TBottomBarPane`, `TQuickSpellPane` and `TBarInvPane` are production
    panes; `TPlayScreen` hosts the bar. The harness bar, shelf and rings
    are gone; `--test=ui-bottombar` and `--test=ab-bottombar` are thin
    hosts, and `--test=ui-hud` hosts the bar beside the harness's side.
  - `hud_ab.py bottombar`: 19/19 cases with no defects. Every opaque pixel
    is exact; the rings' alpha edges are within 17.
  - What the A/B found and fixed:
    - Keying follows the draw mode, as in retail: a draw without
      DM_TRANSPARENT keys nothing (`EBitmapDecode::Unkeyed`: the bar, the
      boxes, the DM_ALPHA rings). The quick-spell circle is keyed on
      magenta alone: retail sets its key to the display's before the Put
      (`0x00542ab3`; `EBitmapDecode::MagentaKeyed`).
    - Retail's 2D DM_ALPHA mixes through 5-bit scale tables (`0x004b3694`):
      `d * (31 - a) // 31 + s * a // 31` a field, green in two 3-bit halves.
      Over every input that is at most 17 (two RGB565 steps) from the
      port's float blend: the A/B's `alpha` tolerance, on the pixels the
      fixture marks.
    - The 2:1 reduction is a GPU pass (`DrawBitmapHalvedToTarget`) that
      takes the mean as retail does: green and blue the floor of the mean;
      red sums each pixel's top byte, green's top bits with it, so green
      can carry red up a step.
    - Ammo and money icons are stacks (`SInvIcon`, drawn as parts on the
      GPU), to retail's recipes: an "Arrow" stack shows IMGAMOUNT(amount)
      copies at AmmoPos − 4, other ammo one (`0x004bf990`, `0x004bfda0`); a
      gold pile 1..64 coins at MoneyPos, its key 0 (`0x00515fb0`). The
      snapshot's CPU composites (crashing for 3D imagery) are gone.
    - 8-bit bitmaps decode through their 15-bit palette, as retail draws
      them: the "Arrow" icon's 24-bit colours are stored RGB, not COLORREF.
    - `TSpellList::GetSpellDataByTalismans` / `GetVariantDataByTalismans`
      match the talismans exactly, first match (`0x0053ed70`,
      `0x0053ef90`): "Advanced healing" is DEB, "Restore Life" BED. Casting
      uses them too.
    - A spell dropped from the spell book onto a ring goes on it
      (`0x00544890`).
    - A pouch's count is of the items in it; retail counts its inventory
      array (`0x00470040`), holes left by items taken out included.
  - Retail bug, reported, not reproduced: a pouch's thumbnail of a stacked
    item sits on a black square. The stack's icon is composed on 0, its
    key; the reduction keys 16-bit sources on the one value it is given,
    the display's key, which also fills its empty pixels. The port's
    thumbnail has no square; the fixture fixes the bug so the A/B checks it
    (`bottombar.py`, `"retail_bugs": true` keeps it).
  - Emulator: DirectDraw surfaces it allocates pick their own pitch; retail's
    texture upload set DDSD_PITCH with a texel width and overran them.
    Items with amounts now build.
  - Open:
    - Dragging an item off the shelf (`0x0052d6e0`) and dropping one on it
      are still the side panel harness's hit tests; the hovered item's name
      (`0x0043a820`). P4, with the other item panes.
    - Revisited at 16:9: the shelf shows (w − 220) / 45 boxes, more than the
      belt's 11 slots at wide widths. A layout question for P7.
- 2026-10-10: **P4: the inventory done (still frames).**
  - `TInventory` is retail's pane (§6b), hosted by `TPlayScreen` in the side
    panel's lower slot and bound to the main player as the screen starts
    (`TPlayerManager::BindMainPlayerToScreen`: the session loads the game,
    and makes its player main, under the loading screen).
    `--test=ui-inventory` and `--test=ab-inventory` are thin hosts over
    `uipanehost.*`, which the bottom bar's hosts share.
  - `hud_ab.py inventory`: 14/14 cases with no defects (gold 0 / 1 / 64 /
    large, stacks, pouches, animated frames, pages 0 / 3 / 12 / last, an
    opened bag). Bottom bar 19/19 and status bar 20/20 still.
  - What the A/B found and fixed:
    - The gold readout sat a row high: bitmap fonts took the GDI fonts'
      top leading. A font atlas now carries its own cell baseline
      (`TextCellBaseline`): a bitmap font's 'A', a TrueType stand-in's
      ascent less 2 (P6 still settles that one).
    - Bitmap glyphs lacked their anti-aliased edges: the font atlas
      decodes them as retail draws them (`EBitmapDecode::PixelsAndAlias`),
      blitted once (the double strike is the TrueType stand-ins').
    - The backpack is keyed on 0, not on magenta alone.
    - The bag's icon takes retail's second reduction (`EHalving::BoxOverBlack`).
  - Fixture fixes: the alpha mask follows DM_USEDEFAULT as retail does and
    takes the glyphs' alias edges (same 5-bit tables, same tolerance); the
    pane gets its container through SetContainer, which builds the bag icon.
  - Open (P4, interactions): retail's clicks and drags (`0x00538210`:
    opening a bag, climbing out, use and swap) are still the side panel
    harness's hit tests.
- 2026-10-10: **P4: the spell pane done** (it never worked in the port).
  - `TSpellPane` is retail's pane (§6c), hosted by `TPlayScreen` in the
    lower slot for the spell tab, with its interactions: picking talismans
    (press, follow, release), back, the page arrows, the names toggle, and
    casting the construct spell. `--test=ui-spellcreate` hosts it for the
    demo player with a pouch of talismans; `--test=ab-spellpane` is the
    A/B's host. The 679-line harness pane is gone.
  - `hud_ab.py spellpane`: 15/15 cases with no defects (no pouch, an empty
    one, a few talismans, all twelve with names and without, first lines,
    spells of 1..5 talismans, lower-case codes, a pressed talisman, the held
    "spell" button). Inventory 14/14, bottom bar 19/19, status bar 20/20.
  - What the A/B found and fixed:
    - `TButton` puts its bitmap as retail does (`0x0042cc30`): its own draw
      mode, by default the bitmap's less DM_BACKGROUND / DM_USEREG; keyed
      only with DM_TRANSPARENT. The spell pane's buttons are unkeyed; the
      inventory's arrows say DM_ALPHA themselves.
    - The talisman names hang from a baseline: retail's text-at-a-point
      (`0x00438ed0`) draws with flag 0x80.
    - The A/B's text mask was too generous: a name laid out in an open-ended
      rect (x, y, x + 10000, y + 10000) masked everything right of and
      below it. A GDI call's cell is now bounded by its font's height a
      character across and a line down (`text_cell`).
  - Retail bug, reported, not reproduced: with no spell pouch the page
    arrows show with nothing to page (the background sets them only while
    there is a pouch). The fixture fixes it (`spellpane.py`).
  - The quick-spell API is const-correct (`TPlayer::GetQuickSpell`,
    `SetQuickSpell`, `HasTalismans`).
  - Open: casting has the spellbook find the spell (`0x00544fb0`), with the
    spellbook pane (P4).
- 2026-10-10: **P4: the automap done** (its buttons and layout had long been
  broken, its map never shown).
  - `TAutoMap` is retail's pane (§6d), hosted by `TPlayScreen` in the lower
    slot for the map tab: the module's tiles around the camera, the fog
    over the unexplored area, the markers, the frame; hold-to-zoom, drag to
    scroll, right-click back to the camera; the explored area recorded into
    the player's save. The `fow` console command works. `--test=ui-map` /
    `ab-automap` are thin hosts; the 409-line harness is gone.
  - `hud_ab.py automap`: 4/4 cases, every pixel exact (the frame, "+" and
    "−" released and held). The rest of the HUD A/B still clean.
  - Found on the way: the play screen kept pointer moves over the HUD from
    the screen's panes (clicks reached them), so the side panes never saw a
    drag; it routes them now.
  - Open: the map body's A/B needs a level in the fixture (the visgrid,
    the module's tiles, characters to mark) and OverlayRaster scaled quads
    and triangles; the mosaic texture's format (565 or 4444) is part of it.
- 2026-10-10: **P4: the spell book done** (it was harness code: the wrong
  circles, no rings by castability, the text roles swapped).
  - `TSpellBookPane` is retail's pane (§6e), hosted by `TPlayScreen` in the
    upper slot for the book tab; `--test=ui-spellbook` / `ab-spellbook` are
    thin hosts; the 874-line harness is gone. Casting in the spell pane and
    `abracadabra` learn spells through it; a circle dragged to a quick-spell
    ring sets that ring.
  - `hud_ab.py spellbook`: 15/15 cases, every opaque pixel exact, blends
    within 17, and every text compose structurally identical (string, font,
    rect, alignment, colour, shadow, line breaks). The rest of the HUD A/B
    still clean.
  - Text, P6 in part: the port's GDI stand-ins now take GDI's cell for each
    FONT.DEF WINFONT (the largest em size whose VDMX cell fits the height:
    "Small" is a 9 px em, 9 + 3; `TFontTable::Atlas`), with linear advance
    widths (Arimo and Tinos share Arial's and Times's). The wrapped
    descriptions now break where the emulator's GDI breaks them, so the
    book's rows are retail's heights. The shadow follows the font
    (`RetailTextShadow`: SpellTitle's starts at (2, 1)). Still open: the
    glyph shapes against Win98 captures, and the bold faces (drawn regular).
  - Found on the way: the save directory's Revenant.ini had the old CD
    install's paths (ClassDefPath "."), so the dialog list's base table
    resolved to the module's file and every SPELL / SPANE line was missing;
    it now has the GOG install's `.\Resources`. The port mounts the module
    before `InitGlobals`, which retail doesn't: a base file named bare can
    resolve into the module (gameflow's to settle).
- 2026-10-10: **P4: the stats done** (the harness drew a 1998-style skill
  tree in the wrong font).
  - `TStatPane` is retail's pane (§6f), hosted by `TPlayScreen` in the upper
    slot for the stats tab; `--test=ui-stats` / `ab-statpane` are thin hosts;
    the 509-line harness is gone. In game: page 1 shows the main player's
    figures, "/Skills" switches to page 2, the train hot spots pick the
    level-up attributes.
  - `hud_ab.py statpane`: 9/9 cases, every pixel outside text exact, every
    text draw structurally identical (string, font, position as GDI's
    current position flows, colour, shadow).
  - Found on the way: clearing a pane's buttons by count never ends
    (TPointerArray leaves holes); `TButtonPane::ClearButtons` (0x00436890)
    does it as Close does.
